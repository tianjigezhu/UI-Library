/**********************************************************************
//
//
//        ##########                          ######  #########   # ######    #
//      #############                      ########### ######### #########  ###
//     ######## # ###  ##                 ############# ##    ## #####  # ####
//     ####  ##    ## ###                 ###     ### # #      #####   #####
//          #     ###  #            #     ##       ##  ##      ##     ###
//         ## ###### ##      ##  ####    ####          #       #     ##
//       #########  ###  ## ### #######   ######      ##      ##    ###
//      ######      ##  ######  ##  ##       ####     #      ##     ####
//     #######     ##   ###### ##  ###          ##   ##     ###    ######
//     #########   ##  ###### ## ######         ### ##    ###      #  #####
//    ##    ###### ####### ### #### ##  ## #######  ########      ##    ####
//    ##      #### ###  #  ### ### ##  ##########   ######       ##      ####
//   ##         ##                ##   #########    ####         #         ##
//               #              ###
//                              ##
//                             ###
//                             ##
//
//
//							Shell操作类库 ringShell.lib
//作者：临风
//
//版本：1.0
//
//声明：本类库可以自由使用而不须对作者作出任何回报，但作者希望能得到
//		  你的鼓励和支持。你可以对类库源码作出修改和改进，但希望你能在
//		  修改的同时给作者一份同样的副本。
//		  本类库不得用于任何商业用途，如确实需要，请与作者联系。
//
//e-mail:ringphone@sina.com
//
//原文件名：ringshell.h
//
//说明：Shell操作定义
//
**********************************************************************/
#ifndef _INC_RINGSHELL
#define _INC_RINGSHELL
#include "ringlib.h"

#include <shellAPI.h>
//#include <initguid.h>
#include <shlguid.h>
#include <shlobj.h>
#include <mapi.h>
//#include <ISGUIDS.H>
/*
#define DEFINE_MYGUID(name, l, w1, w2, b1, b2, b3, b4, b5, b6, b7, b8) \
        EXTERN_C const GUID name \
                = { l, w1, w2, { b1, b2,  b3,  b4,  b5,  b6,  b7,  b8 } }
#pragma option -b

#undef  INTERFACE
#define INTERFACE   ITaskbarList
DECLARE_INTERFACE_(ITaskbarList, IUnknown)
{
	STDMETHOD(QueryInterface) (THIS_ REFIID riid, LPVOID * ppvObj) PURE;
   STDMETHOD_(ULONG,AddRef) (THIS)  PURE;
   STDMETHOD_(ULONG,Release) (THIS) PURE;
   STDMETHOD(ActivateTab)(HWND) PURE;
	STDMETHOD(AddTab)(HWND) PURE;
	STDMETHOD(DeleteTab)(HWND) PURE;
	STDMETHOD(HrInit)(void) PURE;
};
typedef ITaskbarList *LPITaskbarList;

DEFINE_GUID(CLSID_TaskbarList, 0x56fdf344, 0xfd6d, 0x11d0, 0x95, 0x8a, 0x0, 0x60, 0x97, 0xc9, 0xa0, 0x90);
DEFINE_GUID(IID_ITaskbarList, 0x56fdf342, 0xfd6d, 0x11d0, 0x95, 0x8a, 0x0, 0x60, 0x97, 0xc9, 0xa0, 0x90);
#pragma option -b.
*/

extern "C" {

//创建快捷方式
HRESULT CreateShortcut( LPCSTR pszPathObj, LPSTR pszParam,LPSTR pszPath,LPSTR pszPathLink, LPSTR pszDesc);
//调用控制面板
BOOL RunControlPanel(LPSTR szCplName);
//干掉任务栏上的按钮
//extern void KillTaskButton(void);

//浏览器控件操作
//（以下函数已用调用系统控件的函数SelectDir代替。因为经常用到，已转移至ringlib）
//以下为非调用系统控件的版本，可嵌入对话框运行，不必受系统限制

extern BOOL Init_Browser(HWND hWnd,UINT nID);        //初始化浏览器控件
extern UINT Response_Browser(LONG lParam);           //响应控件操作(in WM_NOTIFY)
extern LPSTR Get_Browser_selFoldername(void);        //得到选中的目录路径

}

#ifndef MAKE_SELF_LIB
#ifdef _DEBUG
#pragma comment(lib, "ringshelld.lib")
#else
#pragma comment(lib, "ringshell.lib")
#endif
//#pragma comment(linker,"/NODEFAULTLIB:libc.lib")
#endif

#endif
