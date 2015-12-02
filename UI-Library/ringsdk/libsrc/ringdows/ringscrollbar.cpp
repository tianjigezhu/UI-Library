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
//原文件名：ringscrollbar.cpp
//
//本文件说明：RingScrollBar类实现代码
//
//类库功能：滚动条控件封装
//
**********************************************************************/
#define MAKE_SELF_LIB
#include "ringdows.h"

RingScrollBar::RingScrollBar()
{
	m_windowType = ITIS_SCROLLBAR;	
}

BOOL RingScrollBar::GetScrollRange(LPSIZE lpSize)
{
	if(lpSize)
		return ::GetScrollRange(m_hWnd,SB_CTL,(LPINT)&lpSize->cx,(LPINT)&lpSize->cy);
	else
		return FALSE;
}

BOOL RingScrollBar::SetScrollInfo(int nMask,int nPosMin,int nPosMax,int nPage,int nPos,
											 int nTrackPos,BOOL bRedraw)
{
	SCROLLINFO si;
	si.cbSize = sizeof(SCROLLINFO);
	si.fMask = nMask;
	si.nMin = nPosMin;
	si.nMax = nPosMax;
	si.nPage = nPage;
	si.nPos = nPos;
	si.nTrackPos = nTrackPos;
	
	return ::SetScrollInfo(m_hWnd,SB_CTL,&si,bRedraw);
}

BOOL RingScrollBar::GetScrollInfo(LPSCROLLINFO lpsi)
{
	if(lpsi)
		return ::GetScrollInfo(m_hWnd,SB_CTL,lpsi);
	else
		return FALSE;
}

BOOL RingScrollBar::SetScrollInfo(LPSCROLLINFO lpsi,BOOL bRedraw)
{
	if(lpsi)
		return ::SetScrollInfo(m_hWnd,SB_CTL,lpsi,bRedraw);
	else
		return FALSE;
}

int RingScrollBar::GetScrollLimit()
{
	SCROLLINFO si;
	si.cbSize = sizeof(SCROLLINFO);
	si.fMask = SIF_RANGE;
	if(::GetScrollInfo(m_hWnd,SB_CTL,&si))
		return si.nMax;
	else
		return 0;
}
