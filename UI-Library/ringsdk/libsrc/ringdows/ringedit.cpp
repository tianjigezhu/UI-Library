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
//原文件名：edit.cpp
//
//说明：Edit控件控制
//
**********************************************************************/
#define MAKE_SELF_LIB
#include "ringdows.h"

RingEdit::RingEdit()
{
	m_windowType = ITIS_EDIT;	
}

RingEdit::~RingEdit()
{

}

BOOL RingEdit::SetReadOnly(BOOL bReadOnly)
{
	return (BOOL)SendMessage(m_hWnd,EM_SETREADONLY,bReadOnly,0);
}

BOOL RingEdit::IsReadOnly()
{
	return (GetWindowLong(m_hWnd,GWL_STYLE) & ES_READONLY);
}

void RingEdit::LimitText(int nChars)
{
	SendMessage(m_hWnd,EM_LIMITTEXT,nChars,0);
}

void RingEdit::InsertLine(LPCTSTR lpText,BOOL bFirstLine/*=FALSE*/)
{
	if(!bFirstLine)
	{		
		SendMessage(m_hWnd,EM_SETSEL,0x7f,0x7f);
		
		char s[8];
		if(SendMessage(m_hWnd,WM_GETTEXT,4,(LPARAM)s) > 0)
			SendMessage(m_hWnd,EM_REPLACESEL,TRUE,(LPARAM)"\r\n");
		SendMessage(m_hWnd,EM_REPLACESEL,TRUE,(LPARAM)lpText);
	}
	else
	{
		SendMessage(m_hWnd,EM_SETSEL,0,0);
		SendMessage(m_hWnd,EM_REPLACESEL,TRUE,(LPARAM)lpText);
		SendMessage(m_hWnd,EM_REPLACESEL,TRUE,(LPARAM)"\r\n");
	}
}

LRESULT RingEdit::OnBkColor(UINT uMsg,HDC hDC)
{
	switch(uMsg)
	{
		case WM_CTLCOLORSTATIC:
		case WM_CTLCOLOREDIT:
			return SetColors(hDC);			
	}
	return 0;
}

LRESULT RingEdit::RingdowProc(HWND hWnd,RINGPARAMS param)
{
	switch(param.uMsg)
	{
		case WM_HSCROLL:
		case WM_VSCROLL:
		case WM_CHAR:
		case WM_KEYDOWN:
		case WM_MOUSEWHEEL:
		{
			LRESULT res = RingControls::RingdowProc(m_hWnd,param);
			if(m_brush)	//有背景，不刷新的话字会花
				InvalidateRect(m_hWnd,NULL,TRUE);
			//SendMessage(m_hWnd,WM_SETREDRAW,TRUE,0);
			return res;
		}
		/*case WM_PAINT:
		{
			PAINTSTRUCT ps;
			BeginPaint(m_hWnd,&ps);
			SetBkMode(ps.hdc,TRANSPARENT);
			param.wParam = (WPARAM)ps.hdc;
			LRESULT res = DefaultProc(param);
			EndPaint(m_hWnd,&ps);
			return res;
		}
		*/
		case WM_ERASEBKGND:
		{			
			if(m_brush)
			{
				RECT rc;
				::GetClientRect(m_hWnd,&rc);
				FillRect(param.hdc,&rc,m_brush);
				return TRUE;
			}
			else
				break;//FillRect(param.hdc,&rc,(HBRUSH)GetStockObject(18/DC_BRUSH///));			
		}
		
	}
	return RingControls::RingdowProc(m_hWnd,param);
}
/*
void RingEdit::OnParentNotify(HWND hWnd,UINT uMsg,UINT wParam,LONG lParam)
{
	switch(uMsg)
	{
		case WM_CTLCOLORSTATIC:
		case WM_CTLCOLOREDIT:
			if(m_brush)
				return 
	}
}
*/
/*
LRESULT CALLBACK RingEdit::RingdowProc(HWND hWnd,UINT uMsg,UINT wParam,LONG lParam)
{
	if(uMsg == WM_ENABLE)
	{
		RingCtrlWnd::RingdowProc(hWnd,uMsg,wParam,lParam);
		currStyle = wParam?RINGITEMSTYLE_FLAT:RINGITEMSTYLE_NORMAL;
		DrawItem();
		return 0;
	}

	if(m_ExStyle == RINGITEMSTYLE_FLAT && IsWindowEnabled(hWnd))
		switch(uMsg)
   	{
			case WM_NCPAINT:
	         return OnDrawItem(hWnd,wParam,lParam);
      	case WM_TIMER:
//				rf.Write("ontimer\r\n",9);
	         return OnTimer(hWnd,wParam,lParam);			
   	}
   return RingCtrlWnd::RingdowProc(hWnd,uMsg,wParam,lParam);
}

LRESULT RingEdit::OnDrawItem(HWND,UINT,LONG)
{
	DrawItem();

  	return 0;
}
//*/




