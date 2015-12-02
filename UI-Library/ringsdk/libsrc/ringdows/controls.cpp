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
//原文件名：controls.cpp
//
//本文件说明：控件基类实现。
//
**********************************************************************/
#define MAKE_SELF_LIB
//#ifndef __BORLANDC__
#include "ringdows.h"
//#else
//#include "include\ringdows.h"
//#endif

//RingOwnerDrawer* RingControls::m_drawer = NULL;

RingCtrlWnd::~RingCtrlWnd()
{
	//RingBaseWnd析构会自动判断是否需要Detach	
}

HWND RingCtrlWnd::Create(LPCTSTR szTitle,LPCTSTR szClass,DWORD dwStyle,HWND hParentWnd/*=NULL*/,DWORD dwExStyle,HMENU hMenu,int x,int y,int width,int height)
{
	if(IsWindow())
		return m_hWnd;

	if(hParentWnd == NULL)
		hParentWnd = m_parent->Handle();
	//else
	//	SetParentObj(GetWindowPtr(hParentWnd));

	m_hWnd = CreateWindowEx(
				  		dwExStyle,
                  szClass,
                  szTitle,
                  dwStyle,
                  x,
                  y,
                  width,
                  height,
                  hParentWnd,
                  hMenu,
                  GetInstance(),
                  NULL
                  );
	RingBaseWnd::Attach(m_hWnd);
	
   m_ID = (UINT)hMenu;

   return m_hWnd;
}


LRESULT RingCtrlWnd::DefaultProc(RINGPARAMS& param)
{
	if(param.uMsg == WM_NCDESTROY)
	{
		CallWindowProc(m_DefProc,m_hWnd,param.uMsg,param.wParam,param.lParam);
		//析构时无需Detach,RingBaseWnd析构时会Detach
		OnNcDestroy();
		return 0;
	}

	return CallWindowProc(m_DefProc,m_hWnd,param.uMsg,param.wParam,param.lParam);
}

RingControls::RingControls()
{
	m_brush = NULL;//(HBRUSH)GetStockObject(NULL_BRUSH/*DC_BRUSH*/);
	m_bNotifiParentKeyEvent = FALSE;
	m_buf = NULL;
	m_bufsize = 0;
	m_crTextColor = GetSysColor(COLOR_WINDOWTEXT);
	m_crBkgColor = CLR_INVALID;
	m_windowType = ITIS_DLGITEM;
	m_bOwnerDraw = FALSE;
	m_drawer = NULL;
}

RingControls::~RingControls()
{
	m_buf=(LPSTR)Del(m_buf);
	if(m_brush && m_brush != (HBRUSH)CLR_INVALID)
		DeleteObject(m_brush);
}

void RingControls::SetAutoSize(BOOL bAuto)
{
	if(bAuto)
		m_ExStyle |= RTV_FITPARENTSIZE;
	else
		m_ExStyle &= ~RTV_FITPARENTSIZE;
}

LRESULT RingControls::RingdowProc(HWND hWnd,RINGPARAMS param)
{
	if(m_drawer)
	{
		LRESULT res = m_drawer->DrawProc(this,param);
		if(res != 0)
			return res;
	}
	if(m_bNotifiParentKeyEvent)
	{
		if(param.uMsg == WM_KEYDOWN ||
			param.uMsg == WM_KEYUP ||
			param.uMsg == WM_CHAR)
			//父窗口如想截取该消息不让控件继续处理必须返回非0值
			if(SendMessage(m_parent->Handle(),param))
				return 0;
	}
	switch(param.uMsg)
	{
		case WM_DESTROY:
			return OnDestroy(param);
		case WM_DOCKSTATECHANGE:
			return OnDockStateChange(param.shortval.low,param.shortval.high);
		case WM_GETDRAGRECTS:
			if(param.lpnmhdr->hwndFrom == m_hWnd)
				return OnGetDragRects(param.lpnmhdr->code,(LPRECT)param.wParam);
	}
	return DefaultProc(param);
}

int RingControls::OnDockStateChange(int stateOld,int stateNew)
{
	return SendMessage(m_parent->Handle(),WM_DOCKSTATECHANGE,MAKELONG(stateOld,stateNew),(LPARAM)m_hWnd);
}

BOOL RingControls::OnGetDragRects(int state,LPRECT lprc)
{
	//return SendMessage(m_parent->Handle(),WM_GETDRAGRECTS,(WPARAM)lprc,(LPARAM)m_hWnd);
	//先向父窗口查询失败然后发过来的消息，没必要再次查询
	return 0;
}

BOOL RingControls::OnParentNotify(HWND hWnd,UINT uMsg,UINT wParam,LONG lParam)
{
	switch(uMsg)
	{
		case WM_DOCKSITESIZECHANGE:	
			hWnd = NULL;	 //标记是WM_DOCKSITESIZECHANGE消息，DOCKSITE调整尺寸
		case WM_SIZE:		
			return OnFitSize(hWnd,wParam,lParam);
		case WM_DRAWITEM:
			//m_bOwnerDraw=TRUE表示由父窗口处理
			if(!m_bOwnerDraw)
				if(m_ID == wParam)
					return OnDrawItem((LPDRAWITEMSTRUCT)lParam);
			return FALSE;
	}
	return FALSE;
}

BOOL RingControls::OnFitSize(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	if(m_ExStyle & RTV_FITPARENTSIZE)
	{
		if(wParam == -1)	//-1表示根据lParam的值设置大小，未来支持，暂时不会用到
			SetPos(0,0,LOWORD(lParam),HIWORD(lParam),NULL,SWP_NOMOVE|SWP_NOZORDER);//SWP_NOACTIVATE|SWP_NOREDRAW);
		else
		{
			RECT rc;
			//必须m_parent->GetClientRect才返回正确的RECT
			RingBaseWnd* parent = GetWindowPtr(GetParent());
			if(parent)
				parent->GetClientRect(&rc);
			else
				::GetClientRect(GetParent(),&rc);
			
			MoveWindow(m_hWnd,rc.left,rc.top,rc.right - rc.left,rc.bottom - rc.top,TRUE);
			if(hWnd == NULL)
			{
				//WM_DOCKSITESIZECHANGE消息，DockSite尺寸变动，有可能覆盖控件，
				//某些控件如ListView等不会刷新导致界面会花，因此须刷新
				InvalidateRect(m_hWnd,NULL,TRUE);
			}
		}		
		return TRUE;
	}
	return FALSE;
}

int RingControls::GetInt(int index/*=INDEX_CURSEL*/)
{
	return (int)GetDlgItemInt(m_parent->Handle(),m_ID,NULL,TRUE);
}

LPCTSTR RingControls::GetText(int index/*=INDEX_CURSEL*/,
										int size/*=RBUF_SIZE*/,
										LPTSTR lpBuf/*=NULL*/)
{
   LPTSTR lpstr = lpBuf;
	if(lpstr == NULL && GetBuffer(size))
		lpstr = m_buf;

	if(lpstr)
	{
		memset(lpstr,0,size);
		GetItemText(index,lpstr,size);
		//GetItemText有可能重新分配m_buf，因此需以下判断
   	if(lpstr != m_buf && lpBuf == NULL)
			return m_buf;
		else
			return lpstr;
	}
	else
   	return "";	
}

int RingControls::GetItemText(int,LPTSTR lpBuf,int size)
{
	return GetWindowText(m_hWnd,lpBuf,size);
}

BOOL RingControls::SetValue(int val,int index/*=INDEX_CURSEL*/)
{
	return SetDlgItemInt(GetParent(),m_ID,val,TRUE);
}

BOOL RingControls::SetValue(LPCTSTR lpszVal,int index/*=INDEX_CURSEL*/)
{
	return SetDlgItemText(GetParent(),m_ID,lpszVal);
}

int RingControls::GetBuffer(int size)
{
   //分配内存，比申请尺寸大4以防溢出，但不记录该实际尺寸
	//m_bufsize仍为申请尺寸
	if(m_buf == NULL && size > 0)
   {
   	m_buf = (LPSTR)New(size + 4);
      if(m_buf)
      	m_bufsize = size;
   }
   else
   	if(size > m_bufsize)
      {
      	LPSTR temp = (LPSTR)New(size + 4);
         if(temp)
         {
         	m_bufsize = size;
            Del(m_buf);
            m_buf = temp;
         }
      }
   return m_bufsize;
}

BOOL RingControls::TypeMatched(UINT id)
{
	HWND hwnd = GetDlgItem(m_parent->Handle(),id);
	if(hwnd)
		return TypeMatched(hwnd);
	return FALSE;
}

BOOL RingControls::TypeMatched(HWND hWnd)
{
	char szClass[80];
	memset(szClass,0,80);
	GetClassName(hWnd,szClass,80);
	return (strcmpi(WndClassName(),szClass) == 0);
}

BOOL RingControls::Attach(UINT uId,RingBaseWnd* parent/*=NULL*/)
{
	if(m_parent == this && parent)
		m_parent = parent;

	m_hWnd = GetDlgItem(m_parent->Handle(),uId);
	return Attach(m_hWnd);	
}

BOOL RingControls::Attach(HWND hWnd)
{
	if(RingBaseWnd::Attach(hWnd))
	{		
		m_ID = GetWindowLong(hWnd,GWL_ID);
		//m_ExStyle有可能被SetAutoSize设置过，因此需或操作
		m_ExStyle = GetWindowLong(m_hWnd,GWL_EXSTYLE) | m_ExStyle;
		//m_brush = (HBRUSH)GetClassLong(m_hWnd,GCL_HBRBACKGROUND);
		return TRUE;
	}
	return FALSE;
}

BOOL RingControls::Create(DWORD dwStyle,DWORD dwExStyle,LPCTSTR szInitText,UINT uId,int x,int y,int width,int height)
{
	if(m_hWnd && IsWindow())
		return TRUE;

	m_ExStyle = dwExStyle;
	
	if((dwExStyle & RTV_FITPARENTSIZE))
	{
		RECT rc;
		m_parent->GetClientRect(&rc);
		x = y = 0;
		width = rc.right;
		height = rc.bottom;
		dwExStyle &= ~RTV_FITPARENTSIZE;
	}
	m_hWnd = RingCtrlWnd::Create(
		szInitText,
		WndClassName(),
		WS_CHILD | WS_VISIBLE | dwStyle,
		0,dwExStyle,
		(HMENU)uId,x,y,width,height);

	if(m_hWnd)
	{
		//m_brush = (HBRUSH)GetClassLong(m_hWnd,GCL_HBRBACKGROUND);
		ringFont rf = (HFONT)GetStockObject(DEFAULT_GUI_FONT);
		rf.SetWindowFont(m_hWnd);
	}
   return (BOOL)m_hWnd;
}

void RingControls::SetOwnerDraw(BOOL bParentDraw,UINT uItemStyle)
{
	m_bOwnerDraw = bParentDraw;

	if(uItemStyle != 0xFFFFFFFF && (GetStyle() & uItemStyle) != uItemStyle)
		ReplaceStyle(0,uItemStyle);	
}

BOOL RingControls::SetBkColor(COLORREF crColor)
{	
	m_crBkgColor = crColor;
	HDC hDC = GetDC(m_hWnd);
	::SetBkColor(hDC,crColor);
	ReleaseDC(m_hWnd,hDC);
	
	if(m_brush)
		DeleteObject(m_brush);

	if(crColor == CLR_INVALID)
		m_brush = (HBRUSH)GetStockObject(NULL_BRUSH);
	else
		m_brush = CreateSolidBrush(crColor);
	return (BOOL)m_brush;
}

void RingControls::SetFont(LPCTSTR szFontName,int size,int nStyle,COLORREF crText)
{
	m_crTextColor = crText;
	m_font.SetWindowFont(m_hWnd,szFontName,size,nStyle);
}

void RingControls::SetTextColor(COLORREF crColor)
{
	m_crTextColor = crColor;
}

LRESULT RingControls::SetColors(HDC hDC)
{
	//if(m_crBkgColor != 0xFFFFFFFF)
	{
		::SetTextColor(hDC,m_crTextColor);
		//SelectObject(hDC,m_brush);
		SetBkMode(hDC,TRANSPARENT);
		//::SetBkColor(param.hdc,0);//((GetSysColor(COLOR_WINDOW));
		//SetROP2(param.hdc,R2_BLACK);
		//if(m_brush == NULL)
		//	SetBkColor(GetSysColor(COLOR_WINDOW));
	}
	//SetBrushOrgEx(hDC,0,0,NULL);
	return (LONG)m_brush;
}

void RingControls::SetMouseTrack(RINGPARAMS& param)
{
	if((param.uMsg >= WM_MOUSEFIRST) && (param.uMsg <= WM_MOUSELAST))
	{
		TRACKMOUSEEVENT tme;
		
		tme.cbSize = sizeof(tme);
		tme.hwndTrack = m_hWnd;
		tme.dwHoverTime = 10;//HOVER_DEFAULT;
		tme.dwFlags = TME_LEAVE | TME_HOVER;
		
		TrackMouseEvent(&tme);         
	}
}


