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
//原文件名：ringtab.cpp
//
//说明：tab控件控制
//
**********************************************************************/
#define MAKE_SELF_LIB
#include "ringdows.h"

RingTab::RingTab()
{
	m_windowType = ITIS_TAB;
	m_hImage = NULL;
	m_CurrTabWindow = NULL;
	m_dockbar = NULL;
	m_nCheckPos = 0; 
}

RingTab::~RingTab()
{
	if(m_hImage)
		ImageList_Destroy(m_hImage);
}

BOOL RingTab::Attach(HWND hWnd)
{
	if(RingControls::Attach(hWnd))
	{		
		//去除CS_HREDRAW|CS_VREDRAW，否则调整尺寸引起窗口闪烁
		LONG lc = GetClassLong(m_hWnd,GCL_STYLE);
		lc &= ~(CS_HREDRAW|CS_VREDRAW);
		SetClassLong(m_hWnd,GCL_STYLE,lc);
		return TRUE;
	}
	return FALSE;
}

BOOL RingTab::Create(DWORD dwStyle,DWORD dwExStyle,LPCTSTR szInitText,UINT uId,int x,int y,int width,int height)
{
	InitCommonControls();
	
	if(RingControls::Create(dwStyle|WS_CLIPCHILDREN|WS_CLIPSIBLINGS,dwExStyle,szInitText,uId,x,y,width,height))
	{
		//去除CS_HREDRAW|CS_VREDRAW，否则调整尺寸引起窗口闪烁
		LONG lc = GetClassLong(m_hWnd,GCL_STYLE);
		lc &= ~(CS_HREDRAW|CS_VREDRAW);
		SetClassLong(m_hWnd,GCL_STYLE,lc);
		return TRUE;
	}
	else
		return FALSE;
}

void RingTab::SetExStyle(DWORD dwExStyle)
{
	if(dwExStyle != 0 && m_hWnd)
	{
		//专有扩展属性必须用以下方法设置，
		DWORD dwStyle = SendMessage(m_hWnd,TCM_GETEXTENDEDSTYLE,0,0);
		dwStyle |= dwExStyle;
		SendMessage(m_hWnd,TCM_SETEXTENDEDSTYLE,0,dwStyle);
	}
}

HDWP RingTab::DefDeferWindowPos(HDWP hdwp,int x,int y,int width,int height,UINT uAppendFlag)
{
	//MoveWindow(m_hWnd,x,y,width,height,TRUE);
	//return hdwp;
	return DeferWindowPos(hdwp,m_hWnd,0,x,y,width,height,/*SWP_NOREDRAW|*/uAppendFlag);
		//SWP_NOZORDER|/*SWP_NOACTIVATE|*SWP_FRAMECHANGED|*/SWP_NOREDRAW|uAppendFlag);
}

RingDockBar* RingTab::EnableDock(LPCTSTR szTitle,UINT uDockStyle,
											UINT uDockFlag,LPRECT lprcPos)
{
	m_dockbar = RingBaseWnd::EnableDock(szTitle,uDockStyle,uDockFlag,lprcPos);
	return m_dockbar;
}

RingDockBar* RingTab::EnableDockSize(LPCTSTR szTitle,int nWidth,int nHeight,
												 UINT uDockFlag,UINT uDockStyle)
{
	m_dockbar = RingBaseWnd::EnableDockSize(szTitle,nWidth,nHeight,uDockFlag,uDockStyle);
	return m_dockbar;
}

BOOL RingTab::GetItemRect(int nIndex,LPRECT lprc)
{
	if(lprc)
		return TabCtrl_GetItemRect(m_hWnd,nIndex,lprc);
	else
		return FALSE;
}

/****************************************************
//
//TAB如果是多行标签，WM_SIZE消息里面调整子窗口尺寸时
//因为尚未调用WM_PAINT消息对标签进行绘制，如果标签行
//数变动，则TabCtrl_AdjustRect返回的仍是变动前的矩形，
//所以需要保存该矩形的top字段，WM_PAINT消息处理完后再
//次TabCtrl_AdjustRect，如果返回矩形与保存值不等，则
//表示标签行数变动，需要再次调整子窗口尺寸
//
//**************************************************/
void RingTab::CheckReposChild()
{
	RECT rc;
	GetClientRect(&rc);
	if(m_nCheckPos != rc.top)
	{
		m_nCheckPos = rc.top;
		::GetClientRect(m_hWnd,&rc);
		NotifyChild(m_hWnd,WM_SIZE,SIZE_RESTORED,MAKELONG(rc.right,rc.bottom));		
	}
}

LRESULT RingTab::RingdowProc(HWND,RINGPARAMS param)
{
	LRESULT res;

	switch(param.uMsg)
   {
		case WM_SIZE:
			InvalidateRect(m_hWnd,NULL,TRUE);
			//don't break;
		//case WM_ACTIVATE:
		//case WM_ENABLE:						
			NotifyChild(m_hWnd,param.uMsg,param.wParam,param.lParam);
			break;
		case WM_MEASUREITEM:
		case WM_DRAWITEM:
			NotifyChild(m_hWnd,param.uMsg,param.wParam,param.lParam);
			//Don't break;
		case WM_DELETEITEM:
		case WM_COMMAND:
		case WM_KEYPRESS:
			return SendMessage(m_parent->Handle(),param);
		case WM_ERASEBKGND:
			return TRUE;
		case WM_PAINT:
			return OnPaint(param);			
		case WM_NCDESTROY:
			//不把ImageList设置为空，窗口过程会销毁ImageList，导致
			//析构时ImageList_Destroy失败，BoundCheck报内存泄露，
			//但实际是否会内存泄露不清楚
			TabCtrl_SetImageList(m_hWnd,NULL);
			//DelBindWndData();
			CallWindowProc(m_DefProc,m_hWnd,param.uMsg,param.wParam,param.lParam);
			OnNcDestroy();
			return 0;
		default:
		{	
			res = CtlColorProc(m_hWnd,param);
			if(res == RM_NOTPROCESS)
				break;
			else
				return res;
		}
	}
	return RingControls::RingdowProc(m_hWnd,param);
}

BOOL RingTab::OnDrawBkg(HDC hDC)
{
	if((GetStyle() & (TCS_MULTILINE|TCS_OWNERDRAWFIXED)) == TCS_MULTILINE)
	{
		//多行标签且非自绘，避免多行标签闪烁
		int nCnt = GetCount();
		RECT rc;

		for(int i=0;i<nCnt;i++)
		{
			if(GetItemRect(i,&rc))
				ExcludeClipRect(hDC,rc.left,rc.top,rc.right,rc.bottom);
		}
	}
	return (m_dockbar && m_dockbar->DrawBkg(hDC));
}

LRESULT RingTab::OnPaint(RINGPARAMS& param)
{
	PAINTSTRUCT ps;
	LRESULT res;
	BeginPaint(m_hWnd,&ps);

	OnDrawBkg(ps.hdc);
	param.uMsg = WM_PRINTCLIENT;
	param.hdc = ps.hdc;
	res = RingControls::RingdowProc(m_hWnd,param);
	EndPaint(m_hWnd,&ps);
	CheckReposChild();
	return res;
}

LRESULT RingTab::OnDestroy(RINGPARAMS& param)
{
	DelBindWndData();
	return DefaultProc(param);
}

LRESULT RingTab::OnDefNotify(RingBaseWnd* w,RINGPARAMS& param)
{
	return OnTabSelect(param);	
}

int RingTab::AddIcon(UINT uidIcon,HINSTANCE hInst/*=GetInstance()*/)
{
	HICON hicon = LoadIcon(hInst,MAKEINTRESOURCE(uidIcon));
	return AddIcon(hicon);
}

int RingTab::AddIcon(HICON hicon)
{
	if(hicon == NULL)
		return -1;
	
	if(m_hImage == NULL)
	{
		m_hImage = ImageList_Create(16,16,ILC_COLOR32|ILC_MASK,5,5);
		if(m_hImage)
			TabCtrl_SetImageList(m_hWnd,m_hImage);
	}

	if(m_hImage)
		return ImageList_AddIcon(m_hImage,hicon);
	
	return -1;
}

BOOL RingTab::SetIcon(int indexTab,int indexIcon)
{
	TCITEM item;
	memset(&item,0,sizeof(TCITEM));
	
	item.mask = TCIF_IMAGE;

	if(TabCtrl_GetItem(m_hWnd,indexTab,&item))
	{
		item.iImage = indexIcon;
		
		return TabCtrl_SetItem(m_hWnd,indexTab,&item);
	}
	return FALSE;
}

BOOL RingTab::InsertTab(LPCTSTR szText,int subindex,UINT uidIcon)
{
	if(uidIcon != 0)
		uidIcon = AddIcon(uidIcon);

	TCITEM titem;
	memset(&titem,0,sizeof(TCITEM));
		
	if(uidIcon != -1)
	{
		titem.mask |= TCIF_IMAGE;
		titem.iImage = uidIcon;
	}
	if(szText)
	{
		titem.mask |= TCIF_TEXT;
		titem.pszText = (LPSTR)szText;
		titem.cchTextMax = strlen(szText);
	}
	return TabCtrl_InsertItem(m_hWnd,subindex,&titem);
}

BOOL RingTab::InsertTabs(LPCTSTR szText)
{	
	TCITEM titem;
	memset(&titem,0,sizeof(TCITEM));
		
	if(szText)
	{
		ringStr str = szText;
		int cnt = str.count("|") + 1;
		int start = 0;
		LPTSTR lps;

		titem.mask = TCIF_TEXT;
		for(int i=0;i<cnt;i++)
		{
			lps = (LPTSTR)str.string() + start;
			start = str.indexOf("|",start);
			if(str[start-1] == '\\')
			{
				start++;
				continue;
			}
			str[start] = '\0';

			titem.pszText = (LPSTR)(lps);
			titem.cchTextMax = strlen(lps);
			TabCtrl_InsertItem(m_hWnd,i,&titem);
			str[start++] = '\0';
		}
		return TRUE;
	}
	return FALSE;
}

BOOL RingTab::SetTabToWnd(int iCol,RingBaseWnd* wnd)
{
	TCITEM item;
	
	memset(&item,0,sizeof(TCITEM));

	item.mask = TCIF_PARAM;
	
	if(wnd && TabCtrl_GetItem(m_hWnd,iCol,&item))
	{
		if(item.lParam != 0)
			return FALSE;

		LPRINGTABITEMDATA lpdata = (LPRINGTABITEMDATA)New(sizeof(RINGTABITEMDATA));

		item.lParam = (LPARAM)lpdata;

		lpdata->uType = ITIS_WINDOW;
		lpdata->obj = wnd;

		if(!TabCtrl_SetItem(m_hWnd,iCol,&item))
			Del(lpdata);
		else
		{
			wnd->SetParent(m_hWnd);
			RECT rc;
			GetClientRect(&rc);
			wnd->SetPos(rc.left,rc.top);
			if(GetCurSel() == iCol)
			{
				m_CurrTabWindow = wnd;
				wnd->Show();
			}
			else
				wnd->Show(SW_HIDE);
			return TRUE;
		}
	}
	return FALSE;
}

int RingTab::Select(int iCol)
{
	int i = TabCtrl_SetCurSel(m_hWnd,iCol);

	NMHDR nm;
	nm.code = TCN_SELCHANGE;
	nm.hwndFrom = m_hWnd;
	nm.idFrom = GetId();

	SendMessage(m_parent->Handle(),WM_NOTIFY,GetId(),(LPARAM)&nm);
	return i;
}

LONG RingTab::GetItemData(int iCol)
{
	TCITEM item;
	
	memset(&item,0,sizeof(TCITEM));
	item.mask = TCIF_PARAM;

	if(TabCtrl_GetItem(m_hWnd,iCol,&item))
		return item.lParam;
	return 0;
}

void RingTab::DelBindWndData()
{
	int cnt = GetItemCount();
	TCITEM item;
	LPRINGTABITEMDATA lpdata;

	memset(&item,0,sizeof(TCITEM));
	
	item.mask = TCIF_PARAM;

	for(int i=0;i<cnt;i++)
	{
		if(TabCtrl_GetItem(m_hWnd,i,&item))
		{
			lpdata = (LPRINGTABITEMDATA)item.lParam;
			if(lpdata && lpdata->uType == ITIS_WINDOW)
			{
				try
				{
					Del(lpdata);
				}
				catch(...)
				{
				}
			}
		}
		item.lParam = 0;
	}
}

BOOL RingTab::GetClientRect(LPRECT lprc)
{
	if(lprc)
	{
		::GetClientRect(m_hWnd,lprc);
		TabCtrl_AdjustRect(m_hWnd,FALSE,lprc);
		return TRUE;
	}
	return FALSE;
}

LRESULT RingTab::OnTabSelect(RINGPARAMS& param)
{
	if(param.lpnmhdr && param.lpnmhdr->hwndFrom == m_hWnd)
	{
		if(param.lpnmhdr->code == TCN_SELCHANGE)
		{
			if(m_CurrTabWindow && IsWindowPtr(m_CurrTabWindow))
				m_CurrTabWindow->Show(SW_HIDE);

			int iCol = GetCurSel();
			LPRINGTABITEMDATA lpdata = (LPRINGTABITEMDATA)GetItemData(iCol);
			if(lpdata && lpdata->uType == ITIS_WINDOW)
			{
				m_CurrTabWindow = lpdata->obj;
				m_CurrTabWindow->Show(SW_SHOW);
			}
			return 0;
		}
	}
	//未处理的消息必须返回-1，由父窗口返回默认处理
	return -1;
}

int RingTab::GetItemText(int index,LPTSTR lpBuf,int size)
{
	TCITEM item;
	
	memset(&item,0,sizeof(TCITEM));
	item.mask = TCIF_TEXT;
	item.cchTextMax = size;
	item.pszText = (LPSTR)lpBuf;
	
	if(TabCtrl_GetItem(m_hWnd,index,&item))
		return item.cchTextMax;

	return 0;
}

