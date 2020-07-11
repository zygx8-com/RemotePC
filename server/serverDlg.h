
// serverDlg.h: 头文件
//

#pragma once

#include <map>
#include <mutex>



//定义一些公共的结构体等

//定义一些宏，来表示包的具体处理类型
//cmd类型数据
#define PACKET_REQ_CMD 10001	//REQ表示request CMD表示cmd类型数据		客户端 ---> 服务端
#define PACKET_RLY_CMD 10002	//RLY reply	CMD 表示cmd类型回复数据		服务端 ---> 客户端
//键盘类型数据
#define PACKET_REQ_KEYBOARD 20001	//REQ表示request 键盘 表示键盘类型数据		客户端 ---> 服务端
#define PACKET_RLY_KEYBOARD 20002	//RLY reply	键盘 表示键盘类型回复数据		服务端 ---> 客户端


//对其 按1字节对齐
#pragma pack(push)	//表示原有的结构体对齐值大小压栈
#pragma pack(1)
struct MyPacket {
	unsigned int type;	//表示包的类型
	unsigned int length;	//表示长度
	char data[1];		//定义一个1字节的数据，	柔性数组	作为指针存在
};
#pragma pack(pop)	//表示原有的结构体对齐值大小出栈










//建立一个结构体来表示当前的连接的数据 session回话
struct MySession {
	SOCKET client;		//表示客户机的socket
	sockaddr_in addr;
	DWORD dwLastTickout;	//客户机通信的最后活跃时间,后面用于心跳管理
	std::string strIp;
};




// CserverDlg 对话框
class CserverDlg : public CDialogEx
{
// 构造
public:
	CserverDlg(CWnd* pParent = nullptr);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SERVER_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持

	//构建一个初始化网络的操作

	bool InitAcceptSocket();

	//定义一个静态成员函数来表示我们的线程回调函数
	static DWORD AcceptFuncThread(LPVOID lpParam);




// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	CListCtrl m_List;
	afx_msg void OnLvnItemchangedList1(NMHDR* pNMHDR, LRESULT* pResult);
	//CListCtrl m_List;

	SOCKET m_hAcceptSocket;		//用于监听连接的socket

	HANDLE m_hAcceptThd;		//表示接收连接我们的线程

	//定义一个hash表来存储连接的所有人  key -- value
	std::map<SOCKET, MySession*> m_map;
	std::mutex m_AcceptMtx;		//用于记录map的同步问题

	CString szOutBuffer;



};
