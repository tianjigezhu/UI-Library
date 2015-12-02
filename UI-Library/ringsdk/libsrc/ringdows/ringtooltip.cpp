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
//版本：1.0
//
//声明：本类库可以自由使用而不须对作者作出任何回报，但作者希望能得到
//		  你的鼓励和支持。你可以对类库源码作出修改和改进，但希望你能在
//		  修改的同时给作者一份同样的副本。
//		  本类库不得用于任何商业用途，如确实需要，请与作者联系。
//
//e-mail:ringphone@sina.com
//
//原文件名：ringtoolbar.cpp
//
//本文件说明：提示框实现代码。
//
**********************************************************************/

#define MAKE_SELF_LIB
#include "ringdows.h"

RingToolTip::RingToolTip()
{
	InitBarControl();
	m_tiptext = NULL;
	SetTipStyle(TTF_SUBCLASS);
}

RingToolTip::~RingToolTip()
{
	LPRINGTIPTEXT curr,save;
	curr = save = m_tiptext;
	while(curr)
	{
		Del((LPSTR)curr->lpszTip);
		save = curr;
		curr = curr->m_next;
		Del(save);
	}
}

//重载RingControls的Create，RingControls::Create始终创建子窗口，而这里需要WS_POPUP
BOOL RingToolTip::Create(DWORD dwStyle,DWORD dwExStyle,LPCTSTR szInitText,UINT uId,
								 int x,int y,int width,int height)
{
	if(m_hWnd && IsWindow())
		return TRUE;
	
	m_ExStyle = dwExStyle & ~RTV_FITPARENTSIZE;
	
	m_hWnd = RingCtrlWnd::Create(
		szInitText,
		WndClassName(),
		WS_POPUP | dwStyle,
		0,dwExStyle,
		(HMENU)uId,x,y,width,height);
	
	return (BOOL)m_hWnd;
}

BOOL RingToolTip::Create(DWORD dwStyle/*=TTS_NOPREFIX*/)
{
   return Create(dwStyle,WS_EX_TOPMOST,NULL,0,0,0,0,0);
}

BOOL RingToolTip::Attach(HWND hWnd)
{
	TOOLINFO ti;
	
	ti.cbSize = sizeof(TOOLINFO);
	ti.uFlags = m_dwTipStyle|TTF_IDISHWND;
	ti.hwnd   = m_parent->Handle();
	ti.uId    = (UINT)hWnd;
	ti.hinst  = GetInstance();
	ti.lpszText  = LPSTR_TEXTCALLBACK;
	::GetClientRect(hWnd,&ti.rect);
	//MapWindowPoints(hWnd,m_parent->Handle(),(LPPOINT)&ti.rect,2);
	return ::SendMessage(m_hWnd,TTM_ADDTOOL,0,(LPARAM)&ti);
}

BOOL RingToolTip::SetPos(int x,int y,int width/*=-999*/,int height/*=-999*/,
								 HWND hPos/* = NULL*/,UINT uFlags/* = SWP_NOZORDER*/)
{
	return ::SendMessage(m_hWnd,TTM_TRACKPOSITION,0,MAKELONG(x,y));
}

BOOL RingToolTip::SetValue(int val,int index/*=INDEX_CURSEL*/)
{
	HWND hwnd = GetDlgItem(m_parent->Handle(),index);
	TOOLINFO ti;
	
	ti.cbSize = sizeof(TOOLINFO);
	ti.hwnd   = hwnd;
	ti.uId    = (UINT)hwnd;
	ti.hinst  = GetInstance();
	ti.lpszText  = MAKEINTRESOURCE(val);
	
	return ::SendMessage(m_hWnd,TTM_UPDATETIPTEXT,0,(LPARAM)&ti);
}

BOOL RingToolTip::SetValue(LPCTSTR lpszVal,int index/*=INDEX_CURSEL*/)
{
	HWND hwnd = GetDlgItem(m_parent->Handle(),index);
	TOOLINFO ti;
	
	ti.cbSize = sizeof(TOOLINFO);
	ti.hwnd   = hwnd;
	ti.uId    = (UINT)hwnd;
	ti.hinst  = NULL;
	ti.lpszText = (LPSTR)lpszVal;
	
	return ::SendMessage(m_hWnd,TTM_UPDATETIPTEXT,0,(LPARAM)&ti);
}

LPCTSTR RingToolTip::AddTipText(UINT uId,LPCTSTR lpszText)
{
	if(IS_INTRESOURCE(lpszText) || (LPSTR)lpszText == LPSTR_TEXTCALLBACK)
		return lpszText;

	LPRINGTIPTEXT lpTip;

	if(m_tiptext == NULL)
		m_tiptext = (LPRINGTIPTEXT)New(sizeof(RINGTIPTEXT));

	lpTip = m_tiptext;
	while(lpTip)
	{
		if(lpTip->m_nSize == 0 && lpTip->lpszTip == NULL)
		{
			//找到空的
			lpTip->m_nSize = (lstrlen(lpszText) + 2) * sizeof(TCHAR);
			lpTip->lpszTip = (LPCTSTR)New(lpTip->m_nSize);
			if(lpTip->lpszTip)
			{
				lstrcpy((LPSTR)lpTip->lpszTip,lpszText);
				lpTip->m_id = uId;
				return lpTip->lpszTip;
			}
			else
			{
				lpTip->m_nSize = 0;
				return NULL;
			}
		}
		else
		{
			if(lpTip->m_next == NULL)
				lpTip->m_next = (LPRINGTIPTEXT)New(sizeof(RINGTIPTEXT));
			lpTip = lpTip->m_next;
		}
	}
	return NULL;
}

BOOL RingToolTip::AddTool(HWND hWnd,LPCTSTR lpszTip,LPRECT lprc)
{
	TOOLINFO ti;
	
	ti.cbSize = sizeof(TOOLINFO);
	ti.uFlags = m_dwTipStyle|TTF_IDISHWND;
	ti.hwnd   = m_parent->Handle();
	ti.uId    = (UINT)hWnd;
	ti.hinst  = GetInstance();
	if(IS_INTRESOURCE(lpszTip) || lpszTip == LPSTR_TEXTCALLBACK)
		ti.lpszText = (LPSTR)lpszTip;
	else
	{
		//复制保存
		ti.lpszText = (LPSTR)AddTipText((UINT)hWnd,lpszTip);
	}
	if(lprc == NULL)
		::GetClientRect(hWnd,&ti.rect);
	else
		CopyRect(&ti.rect,lprc);
	
	return ::SendMessage(m_hWnd,TTM_ADDTOOL,0,(LPARAM)&ti);
}

BOOL RingToolTip::AddTool(UINT uId,LPCTSTR lpszTip,LPRECT lprc)
{
	return AddTool(GetDlgItem(m_parent->Handle(),uId),lpszTip,lprc);
}
