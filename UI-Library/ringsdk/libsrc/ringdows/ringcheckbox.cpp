/**********************************************************************
//							临风程序界面类库 ringdows.lib
//作者：临风
//
//版本：0.1
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
/*
#define MAKE_SELF_LIB
#include "ringdows.h"

RingCHECKBOX::RingCHECKBOX(HWND hParent,UINT uId)
{
	m_hParent = hParent;
   m_TimerCnt ++;
   timerId = m_TimerCnt;
   currStyle = EDITSTYLE_FLAT;
   bTimerSetted = FALSE;
   m_preProc = NULL;

   if(uId != 0)
   {
   	m_hWnd = GetDlgItem(hParent,uId);
      m_ID = uId;
      if(RegCtrl(m_hWnd))
      {
	      m_preProc = (WNDPROC)GetWindowLong(m_hWnd,GWL_WNDPROC);
   		SetWindowLong(m_hWnd,GWL_WNDPROC,(LONG)RingControls::RingCtrlProc);
      }
   }
}

RingCHECKBOX::~RingCHECKBOX()
{
   KillTimer(m_hWnd,timerId);
   if(m_preProc != NULL)
		SetWindowLong(m_hWnd,GWL_WNDPROC,(LONG)m_preProc);
}

BOOL RingCHECKBOX::Create(LPCTSTR szCaption,DWORD dwStyle,int x,int y,int width,int height)
{
	m_hWnd = CreateWindow("edit",szCaption,
									WS_CHILD | WS_VISIBLE | dwStyle,
									x,y,width,height,
									m_hParent,
									(HMENU)m_ID,
									m_hInst,NULL);

   if(RegCtrl(m_hWnd))
   {
      m_preProc = (WNDPROC)GetWindowLong(m_hWnd,GWL_WNDPROC);
  		SetWindowLong(m_hWnd,GWL_WNDPROC,(LONG)RingControls::RingCtrlProc);
      return TRUE;
   }
   return FALSE;
}

LRESULT WINAPI RingCHECKBOX::DefCtrlProc(HWND hWnd,UINT uMsg,UINT wParam,LONG lParam)
{
	switch(uMsg)
   {
      case WM_NCPAINT:
      {
         if(!bPainted)
         {
				GetWindowRect(hWnd,&rcItem);

            rcItem.right -= (rcItem.left);
  	      	rcItem.left = 0;
	     	   rcItem.bottom -= (rcItem.top);
   	     	rcItem.top = 0;

            hDC = GetWindowDC(hWnd);
            if(currStyle == EDITSTYLE_FLAT)
            {
              	hpen = CreatePen(PS_SOLID,1,m_crSys.crBTNFACE);
		         SelectObject(hDC,hpen);
   		      SelectObject(hDC,GetStockObject(NULL_BRUSH));
      		   Rectangle(hDC,rcItem.left,rcItem.top,rcItem.right,rcItem.bottom);
	         	DeleteObject(hpen);
            }
            else
         		DrawEdge(hDC,&rcItem,BDR_SUNKENOUTER,BF_RECT);

            ReleaseDC(hWnd,hDC);
            bPainted = TRUE;
      	}
      	return 0;
      }
      case WM_MOUSEMOVE:
         if(!bTimerSetted)
         {
         	SetTimer(hWnd,timerId,10,NULL);
            bTimerSetted = TRUE;
         }
      	return m_preProc(hWnd,uMsg,wParam,lParam);
      case WM_SETFOCUS:
			bLocked = TRUE;
			return m_preProc(hWnd,uMsg,wParam,lParam);
      case WM_KILLFOCUS:
      	bLocked = FALSE;
         SendMessage(hWnd,WM_NCPAINT,0,0);
         return m_preProc(hWnd,uMsg,wParam,lParam);
      case WM_TIMER:
      {
         GetWindowRect(hWnd,&rcItem);
         GetCursorPos(&pt);
         if(PtInRect(&rcItem,pt))
         {
         	if(currStyle == EDITSTYLE_FLAT)
            {
            	bPainted = FALSE;
               currStyle = EDITSTYLE_NORMAL;
               SendMessage(hWnd,WM_NCPAINT,0,0);
               return 0;
            }
         }
         else
         {
         	if(!bLocked && currStyle == EDITSTYLE_NORMAL)
	         {
            	bPainted = FALSE;
               currStyle = EDITSTYLE_FLAT;
               SendMessage(hWnd,WM_NCPAINT,0,0);
               return 0;
            }
            else if(bLocked && currStyle == EDITSTYLE_FLAT)
	         {
            	bPainted = FALSE;
               currStyle = EDITSTYLE_NORMAL;
               SendMessage(hWnd,WM_NCPAINT,0,0);
               return 0;
            }
         }
         return 0;
      }
      default:
         return m_preProc(hWnd,uMsg,wParam,lParam);
   }
}
*/

