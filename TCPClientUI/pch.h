// pch.h: 这是预编译标头文件。
// 下方列出的文件仅编译一次，提高了将来生成的生成性能。
// 这还将影响 IntelliSense 性能，包括代码完成和许多代码浏览功能。
// 但是，如果此处列出的文件中的任何一个在生成之间有更新，它们全部都将被重新编译。
// 请勿在此处添加要频繁更新的文件，这将使得性能优势无效。

#ifndef PCH_H
#define PCH_H

// 添加要在此处预编译的标头
#include "framework.h"



#include <tchar.h>

//声明标准库线程头文件
#include <thread>

//包含通信的头文件和库
//头文件
#include <winsock2.h>
//库文件
#pragma comment(lib,"ws2_32.lib")

//#define _WINSOCK_DEPRECATED_NO_WARNINGS






#endif //PCH_H
