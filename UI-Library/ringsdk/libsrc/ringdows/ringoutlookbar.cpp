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
//							临风程序界面类库 ringdows.lib
//作者：临风
//
//版本：0.2
//
//声明：本类库可以自由使用而不须对作者作出任何回报，但作者希望能得到
//		  你的鼓励和支持。你可以对类库源码作出修改和改进，但希望你能在
//		  修改的同时给作者一份同样的副本。
//		  本类库不得用于任何商业用途，如确实需要，请与作者联系。
//
//e-mail:ringphone@sina.com
//
//原文件名：ringoutlookbar.cpp
//
//本文件说明：RingOutLookBar类实现代码
//
//类库功能：OutlookBar控件封装
//
**********************************************************************/
#define MAKE_SELF_LIB
#include "ringdows.h"

RingOutLookBar::RingOutLookBar()
{
	SetAutoDelete();
}

RingOutLookBar::~RingOutLookBar()
{
}

BOOL RingOutLookBar::Create()
{
	if(m_hWnd)
		return TRUE;
	
	WNDCLASSEX wndclsex =
   {
		sizeof(WNDCLASSEX),
			CS_DBLCLKS | CS_HREDRAW | CS_VREDRAW,
			InitWndProc,
			0,0,
			GetInstance(),
			NULL,
			NULL,
			(HBRUSH)(COLOR_WINDOW + 1),
			NULL,
			"RingOutLookBar",
			NULL
   };
	
	if(!RegRingdowClass(&wndclsex))
		return NULL;
	
   CreateEx("","RingOutLookBar", 
		WS_CHILD | WS_CLIPCHILDREN | WS_CLIPSIBLINGS,
		WS_EX_TOOLWINDOW,NULL,NULL,0,0,0,0);
	
	Show(SW_SHOW);
	
	return (BOOL)m_hWnd;
}

BOOL RingOutLookBar::AddBarItem(LPCTSTR lpszTitle,UINT uId,UINT style)
{
	return TRUE;
}

LRESULT RingOutLookBar::RingdowProc(HWND,RINGPARAMS param)
{
	switch(param.uMsg)
	{
		case WM_COMMAND:
			break;
		case WM_PAINT:
		{
			PAINTSTRUCT ps;
			BeginPaint(m_hWnd,&ps);
			TextOut(ps.hdc,0,0,"hello",5);
			EndPaint(m_hWnd,&ps);
			break;
		}
	}
	return DefaultProc(param);
}

BOOL RingOutLookBar::OnParentNotify(HWND hWnd,UINT uMsg,UINT wParam,LONG lParam)
{
	switch(uMsg)
	{
		case WM_SIZE:
			SetPos(0,0,LOWORD(lParam),HIWORD(lParam));
			NotifyChild(hWnd,uMsg,wParam,lParam);
	}
	return FALSE;
}
