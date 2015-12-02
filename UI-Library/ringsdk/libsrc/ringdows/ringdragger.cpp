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
//原文件名：ringdragger.cpp
//
//本文件说明：窗口拖动的辅助类实现。
//
**********************************************************************/
#define MAKE_SELF_LIB
#include "ringdows.h"
/*
HPEN hPen[6]={
	CreatePen(PS_SOLID,1,0x000000FF),
	CreatePen(PS_SOLID,2,0x0000FF00),
	CreatePen(PS_SOLID,3,0x00FF0000),
	CreatePen(PS_SOLID,4,0x0000FFFF),
	CreatePen(PS_SOLID,5,0x00FF00FF),
	CreatePen(PS_SOLID,6,0x00FFFF00),
};
*/
void WINAPI DrawDragRect(HDC hDC,LPRECT pRect,int nWidth)
{
	assert(pRect);
	
	RECT rc;
	CopyRect(&rc,pRect);
	for(int i=0;i<nWidth;i++)
	{
		DrawFocusRect(hDC,&rc);
		InflateRect(&rc,-1,-1);
	}
}

void WINAPI DrawLineRect(HDC hDC,LPRECT pRect)
{
   assert(pRect);

	SetROP2(hDC,R2_NOT);
	MoveToEx(hDC,pRect->left,pRect->top,NULL);
	LineTo(hDC,pRect->right,pRect->top);
	LineTo(hDC,pRect->right,pRect->bottom);
	LineTo(hDC,pRect->left,pRect->bottom);
	LineTo(hDC,pRect->left,pRect->top);
}

//LPRECT 为相对于hWnd的坐标
void RingDragger::StartDrag(HWND hWnd,int xMouse,int yMouse,LPRECT lprc,int width)
{
	m_bDragging = TRUE;
	m_width = width;
	m_pt.x = xMouse;
	m_pt.y = yMouse;

	SetCapture(hWnd);
	
	CopyRect(&m_DragRc,lprc);

	ClientToScreen(hWnd,(LPPOINT)&m_DragRc);
	ClientToScreen(hWnd,(LPPOINT)&m_DragRc+1);
	
	m_DC = GetDC(NULL);

	DrawRect(m_width);
}

//擦除旧矩形，返回新位置新矩形（但不绘制新矩形）提供机会给
//调用者判断如何绘制新矩形或进行处理
//注意返回的LPRECT是屏幕坐标。lprc返回hWnd的相对坐标
LPRECT RingDragger::EraseRect(int xMouse,int yMouse,HWND hWnd/*=NULL*/,LPRECT lprc/*=NULL*/)
{
	DrawRect(m_width);
	
	OffsetRect(&m_DragRc,xMouse - m_pt.x,yMouse - m_pt.y);
	m_pt.x = xMouse;
	m_pt.y = yMouse;

	if(hWnd && lprc)
	{
		CopyRect(lprc,&m_DragRc);
		ScreenToClient(hWnd,(LPPOINT)lprc);
		ScreenToClient(hWnd,(LPPOINT)lprc+1);
	}
	return &m_DragRc;
}

//绘制当前矩形
//注意：如果绘制后m_DragRc位置没有变动，则再次调用为擦除矩形
void RingDragger::DrawRect(int width)
{
	if(width > 1)
		DrawDragRect(m_DC,&m_DragRc,width);
	else
		DrawLineRect(m_DC,&m_DragRc);

	m_width = width;
}

void RingDragger::DrawRect(LPRECT lprc,int width)
{
	if(lprc)
	{
//		SelectObject(m_DC,hPen[width%6]);
		for(int i=0;i<width;i++)
		{
			DrawLineRect(m_DC,lprc);
			InflateRect(lprc,-1,-1);
		}
	}
}

void RingDragger::Drag(int xMouse,int yMouse)
{
	EraseRect(xMouse,yMouse);
	DrawRect(m_width);
}

LPRECT RingDragger::StopDrag(HWND hWnd,int xMouse,int yMouse)
{
	ReleaseCapture();
	m_bDragging = FALSE;
	DrawRect(m_width);
	//EraseRect(xMouse,yMouse);
	ReleaseDC(NULL,m_DC);

	ScreenToClient(hWnd,(LPPOINT)&m_DragRc);
	ScreenToClient(hWnd,(LPPOINT)&m_DragRc+1);

	return &m_DragRc;
};

LPRECT RingDragger::GetCurrRect(HWND hWnd/*=NULL*/,LPRECT lprc/*=NULL*/)
{
	if(hWnd && lprc)
	{
		CopyRect(lprc,&m_DragRc);
		ScreenToClient(hWnd,(LPPOINT)lprc);
		ScreenToClient(hWnd,(LPPOINT)lprc+1);		
	}
	return &m_DragRc;
}

void RingDragger::SetDragRect(LPRECT lprc,HWND hWnd/*=NULL*/)
{
	CopyRect(&m_DragRc,lprc);

	if(hWnd)
	{
		ClientToScreen(hWnd,(LPPOINT)&m_DragRc);
		ClientToScreen(hWnd,(LPPOINT)&m_DragRc+1);
	}	
}