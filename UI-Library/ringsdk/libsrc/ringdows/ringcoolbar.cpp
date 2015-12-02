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
//本文件说明：工具栏实现代码，包括：RingToolBar和RingCoolBar
//				  RingToolBar:普通工具栏，不可移动或可拖动
//				  RingCoolBar:类似IE的工具栏，不可拖动，但可容纳多个
//								  工具栏或控件并支持在其范围内调整位置。
//
**********************************************************************/

#define MAKE_SELF_LIB
#include "ringdows.h"

////////////////////////////////////////////////////
//
//类似IE的工具栏
//
////////////////////////////////////////////////////
RingCoolBar::RingCoolBar()
{
	init();
}

void RingCoolBar::init()
{
	m_windowType = ITIS_COOLBAR;
}

RingCoolBar::~RingCoolBar()
{
}

BOOL RingCoolBar::Create(DWORD dwStyle,DWORD dwExStyle,LPCTSTR szInitText,UINT uId,int x,int y,int width,int height)
{
   if(m_hWnd)
   	return FALSE;

//	m_State = TBS_FLAG_DOCKTOP;

	DWORD style = 0;
/*
	if(m_State > TBS_FLAG_DOCKBOTTOM)
	{
		m_winpos.cx = 32;
		m_winpos.cy = 300;
		style = CCS_VERT;
	}
	else
	{
		m_winpos.cx = 400;
		m_winpos.cy = 32;
	}
*/
	INITCOMMONCONTROLSEX icex;		
	icex.dwSize = sizeof(INITCOMMONCONTROLSEX);		
	// Load the coolbar. 
	icex.dwICC = ICC_COOL_CLASSES | ICC_BAR_CLASSES;
	InitCommonControlsEx(&icex);

	RingCtrlWnd::Create(NULL,
		REBARCLASSNAME,		
		WS_VISIBLE | WS_BORDER | WS_CHILD | WS_CLIPCHILDREN |
      WS_CLIPSIBLINGS | CCS_NODIVIDER | CCS_NOPARENTALIGN |
      RBS_VARHEIGHT | RBS_AUTOSIZE | RBS_BANDBORDERS | style,
		m_parent->Handle(),0,(HMENU)gdwDockBarId++,
      0,0,width,height);	

	REBARINFO rbi;
	
	rbi.cbSize = sizeof(REBARINFO);  // Required when using this struct.
   rbi.fMask  = 0;
   rbi.himl   = (HIMAGELIST)NULL;
   if(!SendMessage(m_hWnd,RB_SETBARINFO,0,(LPARAM)&rbi))
      return FALSE;
/*	
	m_parentBar = ((RingWnd*)m_parent)->CreateDockSite(m_State);
	if(m_parentBar)
	{
		SetParent(m_parentBar->Handle());
		SetPos(0,0);
		((RingDockSite*)m_parentBar)->AddBar(this);
	}
*/
	return (BOOL)m_hWnd;
}

BOOL RingCoolBar::AddBand(RingCtrlWnd* child)
{
	if(child == NULL)
		return FALSE;

	REBARBANDINFO rbBand;
	   
   // Initialize REBARBANDINFO for all coolbar bands.
   rbBand.cbSize = sizeof(REBARBANDINFO);
   rbBand.fMask = RBBIM_CHILD |                     // hwndChild is valid
      RBBIM_CHILDSIZE |                 // cxMinChild and cyMinChild are valid
      RBBIM_STYLE |                     // fStyle is valid
      RBBIM_ID;// |                              // wID is valid      
   rbBand.fStyle =  RBBS_CHILDEDGE | RBBS_GRIPPERALWAYS;
   rbBand.hwndChild = child->Handle();
   rbBand.wID = child->m_ID;
   rbBand.cxMinChild = 100;//MIN_TBCX;
   rbBand.cyMinChild = 26;//MIN_CY;
	
   // Insert band into coolbar.
	//SetPos(0,0,400,32);
   return SendMessage(m_hWnd,RB_INSERTBAND,(UINT) -1,
		(LPARAM)(LPREBARBANDINFO)&rbBand);
	/*
	RECT rc;
	GetWindowRect(m_hWnd,&rc);
	OffsetRect(&rc,-rc.left,-rc.top);
	if(m_lineInfo && m_lineInfo->m_maxSize != nSize)
	{
		nSize -= m_lineInfo->m_maxSize;
		m_lineInfo->m_maxSize += nSize;
		((RingDockSite*)m_parentBar)->UpdateLine(m_lineInfo,nSize);
	}
	*/
}
/*
void RingCoolBar::ChangeParent(RingDockBar* parent/*=NULL)
{
	if(parent == NULL)
	{
		if(m_parentBar)
		{		
			m_parentBar->Show();
			m_CtrlBar = m_parentBar;			
		}
	}
	else
	{
//		if(!IsWindowVisible(m_hWnd))
//			parent->Show();
		m_CtrlBar = parent;
		if(m_parentBar && m_parentBar != parent)
			m_parentBar->Show(SW_HIDE);
	}
	//DefWindowProc(m_hWnd,TB_SETPARENT,(WPARAM)m_CtrlBar->Handle(), 0);
	SetParent(m_CtrlBar->Handle());
	SetParentObj(m_CtrlBar);
}
*/
LRESULT RingCoolBar::RingdowProc(HWND hWnd,RINGPARAMS param)
{
	switch(param.uMsg)
	{
		case WM_PAINT:
		case WM_PRINTCLIENT:
		case WM_ERASEBKGND:
			break;//return TRUE;

/*
		case RB_INSERTBAND:
		{
			LRESULT res = DefaultProc(param);
			RECT rc;
			GetWindowRect(m_hWnd,&rc);
			OffsetRect(&rc,-rc.left,-rc.top);
			UpdateSize(rc.right,rc.bottom);
			return res;
		}
		case WM_ERASEBKGND:
			return OnEraseBkg(param);
*/
	}
	return DefaultProc(param);//RingReBar::RingdowProc(hWnd,param);
}

//注意：HWND 是父窗口的HWND
//wParam为LPRINGBARLINEINFO
BOOL RingCoolBar::OnParentNotify(HWND hWnd,UINT uMsg,UINT wParam,LONG lParam)
{
	switch(uMsg)
	{
		case WM_SIZE:	//父窗口改变尺寸
		{
			SetPos(0,0,LOWORD(lParam),24);//HIWORD(lParam));			
		}
	}
	return 0;
}
