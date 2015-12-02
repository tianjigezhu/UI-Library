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
//原文件名：ctrlbar.cpp
//
//本文件说明：停靠栏实现代码，类似IE可移动的菜单栏及工具栏
//
//设计思路：
//		RingSDK设计成任意控件，子窗口均可停靠，停靠栏并非窗口，仅包含
//	这些可停靠窗口停靠，拖动时的一些信息及处理窗口的停靠和拖动，绘制
//	窗口的拖动把手等。当窗口为浮动状态时会创建浮动栏窗口，容纳停靠
//	窗口。浮动栏必须自适应停靠窗口尺寸。
//
//	停靠实现：
//		主窗口四边会根据需要创建停靠容器栏，停靠窗口是停靠在
//	容器栏，工具栏，状态栏设计成必须停靠（默认无法拖动，必须调用
//	EnableDock才可拖动），这样设计是方便计算主窗口客户区尺寸，扣掉
//	四个停靠容器栏尺寸即可。
//
//	绘制实现：
//		停靠容器栏会调用DockBar::Draw绘制背景，把手，停靠栏边框，而
//	在浮动状态，FlyBar浮动栏不调用该函数，因此无边框及把手。
//
//	拖动实现：
//		开始拖动时需要初始化3个矩形：横向和纵向停靠矩形（边框宽度为1），
//	浮动矩形（边框宽度为3），拖动时根据鼠标位置偏移这些矩形并随时调整，
//	判断停靠位置决定显示哪个矩形。
//	
//	主窗口尺寸变动响应：
//		主窗口尺寸变动，停靠容器栏进行停靠窗口的重新排列，按行依次查询
//	每一停靠栏前面空位尺寸，获取到整行可调整位置空间尺寸，然后调用
//	行内最后一个停靠栏的AdjustDockPos调整停靠窗口位置，停靠窗口如果
//	调整后与前面窗口重叠，会调用前一个停靠栏的该函数引起连锁反应直到
//	行内窗口全部调整完毕。
//
//	停靠窗口尺寸响应：
//		停靠窗口尺寸变动（工具栏，菜单栏增删按钮等）由工具栏通知停靠栏，
//	停靠栏更新尺寸后通知父窗口，父窗口响应后产生主窗口尺寸变动事件，导致
// 主窗口尺寸变动响应。
**********************************************************************/

#define MAKE_SELF_LIB
#include "ringdows.h"

DWORD	gdwDockBarId = 39000;

RingFlyBar::RingFlyBar(RingBaseWnd* parent)
{
   SetParentObj(parent);
   m_child = NULL;
	m_dragger = NULL;
	SetAutoDelete();
   m_windowType = ITIS_FLYBAR;
}

RingFlyBar::~RingFlyBar()
{
	if(m_child && m_child->m_State == TBS_FLAG_FLY)
		delete m_child;
}

BOOL RingFlyBar::Create(LPCTSTR lpszTitle)
{
   if(m_hWnd)
		return TRUE;

	if(!Register("RingFlyBar",
		(HBRUSH)(COLOR_BTNFACE + 1),
		NULL,
		NULL,
		m_windowInfo.hCursor,
		NULL))
		return NULL;
	
   CreateEx(lpszTitle,"RingFlyBar",//0x94CC1200, 
		WS_POPUPWINDOW | WS_CAPTION | WS_THICKFRAME | 
		/*WS_OVERLAPPED |*/ WS_CLIPCHILDREN | WS_CLIPSIBLINGS,
		WS_EX_TOOLWINDOW,NULL,NULL);//,0,0,0,0);
	
   RingMenu rm;
	rm.Attach(GetSystemMenu(m_hWnd,FALSE));
	//rm.Delete(SC_CLOSE);
	//rm.Delete(SC_SIZE);
	rm.Delete(SC_MINIMIZE);
	rm.Delete(SC_MAXIMIZE);
	rm.Delete(SC_MOVE);
	rm.Delete(SC_RESTORE);

	return (BOOL)m_hWnd;
}

LRESULT RingFlyBar::RingdowProc(HWND hWnd,RINGPARAMS param)
{
	if(param.uMsg >= WM_KEYFIRST && param.uMsg <= WM_KEYLAST)
	{
		//消息转发给子窗口
		if(m_child && m_child->GetChild())
			return SendMessage(m_child->GetChild()->Handle(),param);
		else
			return DefaultProc(param);
	}
	switch(param.uMsg)
   {
		case WM_COMMAND:
			//消息转发给父窗口
			if(m_child && m_child->GetChild())
				return SendMessage(m_child->GetChild()->Parent()->Handle(),param);
			else
				return DefaultProc(param);
		case WM_CONTEXTMENU:
			//取消弹出菜单
			//if(HIWORD(lParam))
			//PostMessage(hWnd,WM_CANCELMODE,0,0);
			return 0;
		case WM_SHOWWINDOW:
			if(param.wParam)
			{				
				Update();
			}
			return DefaultProc(param);
		case WM_PAINT:
			//检测子窗口显示状态，不允许外力干预（调用者或外挂）
			if(m_child && m_child->GetChild() && !m_child->GetChild()->IsVisible())
				m_child->GetChild()->Show();
			return DefaultProc(param);
		case WM_CLOSE:
      	Show(SW_HIDE);
			if(m_child && m_child->m_pSite)
				m_child->m_pSite->OnDockBarHide(m_child);
         break;
      case WM_ACTIVATE:
         if(IsWindowEnabled(hWnd))
         {
	         SendMessage(m_parent->Handle(),WM_NCACTIVATE,TRUE,0);
   		   SendMessage(m_hWnd,WM_NCACTIVATE,TRUE,0);
         }
         break;
		case WM_NOTIFY:
			switch(param.lpnmhdr->code)
			{
			case TTN_NEEDTEXT:
				{
					((LPTOOLTIPTEXT)param.lParam)->hinst = GetInstance();
					return SendMessage(m_parent->Handle(),WM_RINGTOOLTIP,((LPTOOLTIPTEXT)param.lParam)->hdr.idFrom,param.lParam);
				}
				
				//case TBN_DROPDOWN:
				//工具栏DropDown按钮
				//	return SendMessage(param.lpnmhdr->hwndFrom,param);
			}
			if(IsWindowPtr(m_parent))
				return SendMessage(m_parent->Handle(),WM_NOTIFY,param.wParam,param.lParam);
			return DefaultProc(param);
      case WM_NCLBUTTONDBLCLK:
			if(param.wParam == HTCAPTION)
         {
				if(m_child)
					m_child->DockBack();
			}
			return 0;
		case WM_NCLBUTTONDOWN:
         if(param.wParam == HTCAPTION)
         {
				if(m_child)
				{
					POINT pt = {param.mousept.x,param.mousept.y};
					RingDockSite* pSite = m_child->m_pSite;
					if(pSite == NULL && m_child->m_child)
						pSite = ((RingWnd*)(m_child->GetChild()->Parent()))->CreateDockSite(TBS_FLAG_DOCKTOP);

					if(pSite == NULL)
						return 0;

					if(m_dragger == NULL)
						m_dragger = &pSite->m_dragger;
					
					ScreenToClient(m_hWnd,&pt);
					
					param.mousept.x = (short)pt.x;
					param.mousept.y = (short)pt.y;
					m_child->StartDrag(m_hWnd,m_dragger,param);
				}
            return 0;
         }
         else
         	return DefaultProc(param);
		case WM_MOUSEMOVE:
			if(m_dragger && m_dragger->IsDragging() && m_child)
				return m_child->OnDrag(m_hWnd,m_dragger,param);
			return DefaultProc(param);
		case WM_LBUTTONUP:
			if(m_dragger && m_dragger->IsDragging() && m_child)
				return m_child->StopDrag(m_child->GetChild()->Parent()->Handle(),m_dragger,param);
			return DefaultProc(param);
		/*case WM_ERASEBKGND:
			if(!DrawBkg(param.hdc))
				DefaultProc(param);
			break;
		*/
		case WM_SIZE:
			//NotifyChild(hWnd,param.uMsg,param.wParam,param.lParam);
			if(m_child)
			{
				m_child->UpdateFlySize(param.mousept.x + WINVAR_FRAME_X*2,
												param.mousept.y + BARTOOLY + WINVAR_FRAME_Y);
				/*if(m_child->m_child)
				{
					RECT rc;
					GetWindowRect(m_child->m_child->Handle(),&rc);
					OffsetRect(&rc,-rc.left,-rc.top);
					m_child->UpdateSize(rc.right,rc.bottom,FALSE);
				}
				*/
			}
			break;
		case WM_SIZING:
		{
			RECT rctmp;
			LPRECT lprc = (LPRECT)param.lParam;
						
			CopyRect(&rctmp,lprc);
			param.lParam = (LPARAM)&rctmp;
			InflateRect(&rctmp,-WINVAR_FRAME_X,-WINVAR_FRAME_Y);
			rctmp.top += (BARTOOLY);// - WINVAR_FRAME_Y);
			
			if(m_child)
				m_child->OnSizing(param);
			//SendMessage(GetChild(),param);
			InflateRect(&rctmp,WINVAR_FRAME_X,WINVAR_FRAME_Y);
			rctmp.top -= (BARTOOLY);// - WINVAR_FRAME_Y);
			param.lParam = (LPARAM)lprc;
			CopyRect(lprc,&rctmp);
			return DefaultProc(param);
			/*
			switch(param.wParam)
			{
				case WMSZ_LEFT:
					lprc->left = lprc->right - rc.right - WINVAR_FRAME_X*2;
					break;
				case WMSZ_RIGHT:
					lprc->right = lprc->left + rc.right + WINVAR_FRAME_X*2;
					break;
				case WMSZ_TOP:
					lprc->top = lprc->bottom - rc.bottom - BARTOOLY - WINVAR_FRAME_Y - 2;
					break;
				case WMSZ_TOPLEFT:
					lprc->top = lprc->bottom - rc.bottom - BARTOOLY - WINVAR_FRAME_Y - 2;
					lprc->left = lprc->right - rc.right - WINVAR_FRAME_X*2;
					break;
				case WMSZ_TOPRIGHT:
					lprc->top = lprc->bottom - rc.bottom - BARTOOLY - WINVAR_FRAME_Y - 2;
					lprc->right = lprc->left + rc.right + WINVAR_FRAME_X*2;
					break;
				case WMSZ_BOTTOM:
					lprc->bottom = lprc->top + rc.bottom + BARTOOLY + WINVAR_FRAME_Y + 2;
					break;
				case WMSZ_BOTTOMLEFT:
					lprc->left = lprc->right - rc.right - WINVAR_FRAME_X*2;
					lprc->bottom = lprc->top + rc.bottom + BARTOOLY + WINVAR_FRAME_Y + 2;
					break;
				case WMSZ_BOTTOMRIGHT:
					lprc->right = lprc->left + rc.right + WINVAR_FRAME_X*2;
					lprc->bottom = lprc->top + rc.bottom + BARTOOLY + WINVAR_FRAME_Y + 2;
					break;
			}*/			
		}
      default:
		{	
			LRESULT res = CtlColorProc(hWnd,param);
			if(res != RM_NOTPROCESS)
				return res;
			else
				return DefaultProc(param);
		}
   }
   return 0;
}
/*
BOOL RingFlyBar::DrawBkg(HDC hDC,LPRECT lprc/*=NULL/)
{
	RingDockSite* tmp = NULL;
	
	if(m_child == NULL)
		return FALSE;

	RingWnd* pa = (RingWnd*)m_child->m_parent;

	for(int i=0;i<4;i++)
	{
		tmp = pa->GetDockSite(i);
		if(tmp)
			break;
	}
	
	if(tmp)
	{
		RECT rc;
		::GetClientRect(m_hWnd,&rc);
		ClientToScreen(m_hWnd,(LPPOINT)&rc);
		ClientToScreen(m_hWnd,(LPPOINT)&rc+1);
		ScreenToClient(pa->Handle(),(LPPOINT)&rc);
		ScreenToClient(pa->Handle(),(LPPOINT)&rc+1);
		//CopyRect(&rc,(LPRECT)&m_winpos);
		if(lprc)
			CopyRect(lprc,&rc);
		//OffsetRect(&rc,tmp->m_winpos.x,tmp->m_winpos.y);
		return tmp->DrawBkg(hDC,&rc);			
	}
	return FALSE;
}
*/
BOOL RingFlyBar::OnParentNotify(HWND hWnd,UINT uMsg,UINT wParam,LONG)
{
   if(hWnd == m_parent->Handle() && IsWindowVisible(m_hWnd))
		if(uMsg == WM_ENABLE)
      {
       	EnableWindow(m_hWnd,(BOOL)wParam);
         if(wParam)
   	   	SendMessage(m_hWnd,WM_NCACTIVATE,TRUE,0);
         else
         {
         	SendMessage(m_hWnd,WM_NCACTIVATE,FALSE,0);
            SendMessage(m_parent->Handle(),WM_NCACTIVATE,FALSE,0);
         }
      }
		//else if(uMsg == WM_SIZE)
		//	InvalidateRect(m_hWnd,NULL,TRUE);

	return FALSE;
}

void RingFlyBar::AddChild(RingDockBar* child)
{
	if(child)
	{
		m_child = child;
		
		RingBaseWnd *pwnd = child->GetChild();
		if(pwnd == NULL)
			return;

		if(child->IsMenuBar())
		{
			RingMenu rm;
			rm.Attach(GetSystemMenu(m_hWnd,FALSE));
			rm.EnableItem(SC_CLOSE,FALSE);			
		}
		pwnd->SetParent(m_hWnd);
		pwnd->SetPos(0,0);
	}
}

void RingFlyBar::Update()
{
	if(m_child)
	{
		RECT rc;
		RingBaseWnd* wnd = m_child->GetChild();
		
		if(wnd)
		{
			GetWindowRect(wnd->Handle(),&rc);
			OffsetRect(&rc,-rc.left,-rc.top);
			if(m_child->IsSizeBar())
				SetPos(0,0,rc.right,rc.bottom,0,SWP_NOMOVE);
			else
				SetPos(0,0,rc.right + 2 + WINVAR_FRAME_X*2,
						rc.bottom + 2 + BARTOOLY + WINVAR_FRAME_Y,0,SWP_NOMOVE);
		}
	}
}

void RingFlyBar::UpdateDockSite()
{
	if(m_child)
	{
		RingBaseWnd* pwnd = m_child->GetChild();
		if(pwnd)
		{
			SendMessage(pwnd->Handle(),WM_DOCKSTATECHANGE,
					MAKELONG(TBS_FLAG_FLY,TBS_FLAG_FLY),(LPARAM)pwnd->Handle());			
			Update();
			InvalidateRect(pwnd->Handle(),NULL,FALSE);
		}
	}
}

//////////////////////////////////////////////////////////
//
//	RingDockBar
//
//////////////////////////////////////////////////////////
RingDockBar::RingDockBar()
{
	m_State = TBS_FLAG_NONE;
	m_uStyle = TBS_EX_NONE;
	m_id = gdwDockBarId ++;

	SetRectEmpty(&m_rcDockPos);
	SetRectEmpty(&m_rcClient);
	SetRectEmpty(&m_hrc);
	SetRectEmpty(&m_vrc);
	SetRectEmpty(&m_frc);
	SetRectEmpty(&m_lfrc);
	SetRectEmpty(&m_ldrc);

	m_pSite = NULL;
	m_next = m_prev = NULL;
	m_lineInfo = NULL;
	m_flyBar = NULL;
	m_BorderSpace = 3;
	m_GripperSpace = 9;
	m_bNCBtDown = FALSE;
	m_nSize = 0;
	m_child = NULL;
	m_szTitle = NULL;
	m_nTitleSize = 0;
	m_bVisible = TRUE;
}

RingDockBar::~RingDockBar()
{
	if(m_szTitle)
		Del(m_szTitle);
}

void RingDockBar::SetDockStyle(UINT style)
{
	m_uStyle = style;
	//SizeBar允许TBS_EX_NONE与TBS_EX_CANCLOSE并存，因此这里判断顺序不可搞错
	if(style & TBS_EX_NONE)
	{
		m_GripperSpace = 0;
		if(style & TBS_EX_ALLLINE)
			m_BorderSpace = 2;
		else
			m_BorderSpace = 0;
	}
	else
	{
		m_GripperSpace = 9;
		m_BorderSpace = 3;
	}		

	if((style & TBS_EX_CANCLOSE))
		m_GripperSpace = 15;
}

BOOL RingDockBar::GetDockRect(LPRECT lprc)
{
	if(lprc && m_child && m_pSite)
	{
		if(m_State == TBS_FLAG_FLY)
		{
			if(m_flyBar)
			{
				GetWindowRect(m_flyBar->Handle(),lprc);
				MapWindowPoints(m_flyBar->Handle(),m_pSite->Parent()->Handle(),(LPPOINT)lprc,2);
				return TRUE;
			}
		}
		else
		{
			CopyRect(lprc,&m_rcDockPos);
			MapWindowPoints(m_pSite->Handle(),m_pSite->Parent()->Handle(),(LPPOINT)lprc,2);
			return TRUE;
		}
	}
	return FALSE;
}

//得到指定停靠位置的客户坐标
BOOL RingDockBar::GetClientRect(int state,LPRECT lprc)
{
	if(m_child && lprc)
	{
		LONG x = 0;
		//先向父窗口查询
		NMHDR hdr;
		hdr.hwndFrom = m_child->Handle();
		hdr.code = state;
		hdr.idFrom = (UINT)m_child;

		if(m_pSite)
			x = SendMessage(m_pSite->Parent()->Handle(),WM_GETDRAGRECTS,
									(WPARAM)lprc,(LPARAM)&hdr);
		else if(m_flyBar)
			x = SendMessage(m_flyBar->Parent()->Handle(),WM_GETDRAGRECTS,
									(WPARAM)lprc,(LPARAM)&hdr);
		//查询失败，向控件查询
		if(x == 0)
			x = SendMessage(m_child->Handle(),WM_GETDRAGRECTS,(WPARAM)lprc,(LPARAM)&hdr);
		//再失败，获取控件尺寸处理
		if(x == 0)
		{
			if(m_flyBar && IsWindow(m_flyBar->Handle()))
				m_flyBar->GetClientRect(lprc);
			else
				GetWindowRect(m_child->Handle(),lprc);
		}
		return TRUE;
	}
	else
		return FALSE;
}

LPRECT RingDockBar::GetDragRects(int state)
{
	if(!GetClientRect(state,&m_rcCurr))
		CopyRect(&m_rcCurr,&m_rcDockPos);

	//OffsetRect(&m_rcCurr,-m_rcCurr.left,-m_rcCurr.top);

	if(state == TBS_FLAG_FLY)
	{
		if(!IsSizeBar())
		{
			m_rcCurr.top -= BARTOOLY;
			m_rcCurr.bottom += WINVAR_FRAME_Y;
			m_rcCurr.left -= WINVAR_FRAME_X;
			m_rcCurr.right += WINVAR_FRAME_X;
		}
	}
	else
	{
		InflateRect(&m_rcCurr,m_BorderSpace,m_BorderSpace);

		if(::IsVert(state))
			m_rcCurr.top -= m_GripperSpace + m_BorderSpace;
		else
			m_rcCurr.left -= m_GripperSpace + m_BorderSpace;
	}
	OffsetRect(&m_rcCurr,m_rcDockPos.left-m_rcCurr.left,m_rcDockPos.top-m_rcCurr.top);
	return &m_rcCurr;
}

BOOL RingDockBar::SetChild(RingBaseWnd* child)
{
	if(child && child->IsWindow() && child->Parent() != child && m_child == NULL)
	{
		m_child = child;
		//UpdateClientSize();
		if(m_child->GetWindowType() == ITIS_STATUSBAR)
		{
			m_BorderSpace = 0;
			m_GripperSpace = 0;
		}
		return TRUE;
	}
	return FALSE;
}

BOOL RingDockBar::IsVisible(BOOL *lpbUpdate)
{
	if(lpbUpdate == NULL)	//不检查，直接返回状态值
		return m_bVisible;

	if(m_child && m_child->IsVisible() != m_bVisible)
	{
		m_bVisible = m_child->IsVisible();
		*lpbUpdate = TRUE;
	}
	else
		*lpbUpdate = FALSE;

	return m_bVisible;
}

//获取停靠窗口尺寸，更新m_rcClient，根据m_rcDockPos的停靠位置平移到适当位置
void RingDockBar::UpdateClientSize()
{
	if(m_child)
	{
		RECT rc;
		GetWindowRect(m_child->Handle(),&rc);
		
		if(IsVert())
			OffsetRect(&rc,m_rcDockPos.left + m_BorderSpace - rc.left,
							m_rcDockPos.top + m_BorderSpace + m_GripperSpace - rc.top);
		else
			OffsetRect(&rc,m_rcDockPos.left + m_BorderSpace + m_GripperSpace - rc.left,
							m_rcDockPos.top + m_BorderSpace - rc.top);
		CopyRect(&m_rcClient,&rc);
	}	
}

//客户区尺寸改变，获取停靠窗口尺寸，更新m_rcClient和m_rcDockPos(不更新位置信息)
void RingDockBar::UpdateSize()
{
	UpdateClientSize();

	if(IsVert())
		m_nSize = m_rcClient.right - m_rcClient.left + m_BorderSpace*2;
	else
		m_nSize = m_rcClient.bottom - m_rcClient.top + m_BorderSpace*2;
	
	m_rcDockPos.right = m_rcClient.right + m_BorderSpace;
	m_rcDockPos.bottom = m_rcClient.bottom + m_BorderSpace;

	if(m_lineInfo)
		m_lineInfo->m_maxSize = max(m_lineInfo->m_maxSize,m_nSize);
}

//更新浮动状态下的尺寸，本函数由FlyBar的WM_SIZE消息调用
void RingDockBar::UpdateSize(int nWidth,int nHeight,BOOL bUpdate)
{
/*	m_winpos.cx = nWidth;
	m_nSize = m_winpos.cy = nHeight + m_BorderSpace*2;
	m_rcClient.right = m_rcClient.left + nWidth - m_GripperSpace - m_BorderSpace*2;
	m_rcClient.bottom = m_rcClient.top + nHeight;
	m_winpos.right = m_winpos.x + nWidth;
	m_winpos.bottom = m_winpos.y + m_winpos.cy;
*/
}

void RingDockBar::UpdateFlySize(int nWidth,int nHeight)
{
	m_frc.right = m_frc.left + nWidth;
	m_frc.bottom = m_frc.top + nHeight;
}

//更新所有DockBar或更新FlyBar
void RingDockBar::UpdateAllSite()
{
	if(IsFloat())
	{
		if(m_flyBar && m_flyBar->IsVisible())
			m_flyBar->UpdateDockSite();
	}
	else if(m_pSite)
		m_pSite->UpdateAllSite();
}

BOOL RingDockBar::SetDockTitle(LPCTSTR lpszTitle)
{
	if(lpszTitle == NULL)
		return FALSE;

	LPSTR szTitle;
	int cch = strlen(lpszTitle);

	if(m_szTitle == NULL || cch > m_nTitleSize)
	{		
		m_nTitleSize = cch + 2;
		szTitle = (LPSTR)New(m_nTitleSize);
		if(szTitle)
		{
			Del(m_szTitle);
			m_szTitle = szTitle;
		}
		else
			return FALSE;
	}
	else
		memset(m_szTitle,0,m_nTitleSize);
	
	strcpy(m_szTitle,lpszTitle);

	return TRUE;
}

LRESULT RingDockBar::OnClose()
{
	if(m_child)
		m_child->Show(SW_HIDE);
	m_bVisible = FALSE;
	if(m_pSite)
		m_pSite->HideBar(this);
	return 0;
}

void RingDockBar::Show(int nState)
{
	if(nState == SW_SHOW || nState == SW_HIDE)
	{
		if(m_State == TBS_FLAG_FLY && m_flyBar)
			m_flyBar->Show(nState);
		else
		{
			if(m_child)
				m_child->Show(nState);
			m_bVisible = (nState == SW_SHOW);
			if(m_lineInfo && m_bVisible)
				m_lineInfo->isVisible = TRUE;
			UpdateAllSite();
		}
	}
}

LRESULT RingDockBar::OnSizing(RINGPARAMS& param)
{	
	RECT rc;
	LPRECT lprc = (LPRECT)param.lParam;
	//param.lParam = (LPARAM)&rc;
	CopyRect(&rc,lprc);

	if(m_child)
		SendMessage(m_child->Handle(),param.uMsg,param.wParam,param.lParam);
	
//	SetPos(0,0,lprc->right - lprc->left,lprc->bottom - lprc->top);

	AdjustSizingRect(param.wParam,lprc,rc);
	return TRUE;
}

void RingDockBar::AdjustSizingRect(int flag,LPRECT lprc,RECT& rc)
{
	switch(flag)
	{
		case WMSZ_LEFT:
		case WMSZ_BOTTOMLEFT:
			OffsetRect(lprc,rc.right - lprc->right,rc.top - lprc->top);
			break;
		case WMSZ_RIGHT:
		case WMSZ_BOTTOM:
		case WMSZ_BOTTOMRIGHT:
			OffsetRect(lprc,rc.left - lprc->left,rc.top - lprc->top);
			break;
		case WMSZ_TOP:
		case WMSZ_TOPRIGHT:
			OffsetRect(lprc,rc.left - lprc->left,rc.bottom - lprc->bottom);
			break;			
		case WMSZ_TOPLEFT:
			OffsetRect(lprc,rc.right - lprc->right,rc.bottom - lprc->bottom);
			break;
	}
}

//判断鼠标位于何方，本函数进入前已经过判断鼠标是在本DockBar范围内
UINT RingDockBar::HitTest(POINT* lpt)
{
	if((m_uStyle & TBS_EX_CANCLOSE) && PtInRect(&m_rcClose,*lpt))
		return HTCLOSE;
	else
		return HTCAPTION;
}

void RingDockBar::StartPushFrameControl(HWND hWnd,RingDragger* dragger,RINGPARAMS& param)
{
	if(m_CurWhere == HTCLOSE)
	{
		m_bNCBtDown = DFCS_CAPTIONCLOSE|DFCS_PUSHED;
		
		HDC hDC = GetDC(hWnd);
		DrawFrameControl(hDC,&m_rcClose,DFC_CAPTION,m_bNCBtDown);
		ReleaseDC(hWnd,hDC);
		
		SetCapture(hWnd);
	}
}

void RingDockBar::OnPushHoverFrameControl(HWND hWnd,RingDragger* dragger,LPPOINT lpt)
{
	if(m_CurWhere == HTCLOSE)
	{
		HDC hDC = GetDC(hWnd);
		
		if(PtInRect(&m_rcClose,*lpt))
		{
			if((m_bNCBtDown & DFCS_PUSHED)!= DFCS_PUSHED)
			{
				m_bNCBtDown |= DFCS_PUSHED;
				DrawFrameControl(hDC,&m_rcClose,DFC_CAPTION,m_bNCBtDown);
			}
		}
		else
		{
			if(m_bNCBtDown != DFCS_CAPTIONCLOSE)
			{
				m_bNCBtDown = DFCS_CAPTIONCLOSE;
				DrawFrameControl(hDC,&m_rcClose,DFC_CAPTION,DFCS_CAPTIONCLOSE);
			}
		}
		ReleaseDC(hWnd,hDC);
	}
}

void RingDockBar::StopPushFrameControl(HWND hWnd,RingDragger* dragger,LPPOINT lpt)
{
	if(PtInRect(&m_rcClose,*lpt) && m_CurWhere == HTCLOSE)
		OnClose();
}

//注意：hWnd有可能是FlyBar的hWnd
LRESULT RingDockBar::StartDrag(HWND hWnd,RingDragger* dragger,RINGPARAMS& param)
{
	POINT pt = {param.mousept.x,param.mousept.y};
	m_CurWhere = HitTest(&pt);
	
	if(m_CurWhere == HTCAPTION)
	{
		if(LOWORD(m_uStyle) <= TBS_EX_DOCKABLE)
		{
			m_bNCBtDown = 2;
			
			InitDragRects(hWnd,pt,(IsRectEmpty(&m_hrc) || !IsFloat()));
			m_currState = m_State;

			if(IsFloat())
				dragger->StartDrag(hWnd,pt.x,pt.y,&m_frc,3);
			else
			{
				if(IsVert())
					dragger->StartDrag(hWnd,pt.x,pt.y,&m_vrc,1);
				else
					dragger->StartDrag(hWnd,pt.x,pt.y,&m_hrc,1);
			}
		}
	}
	else
		StartPushFrameControl(hWnd,dragger,param);

	return 0;
}

LRESULT RingDockBar::StopDrag(HWND hWnd,RingDragger* dragger,RINGPARAMS& param)
{
	if(dragger && dragger->IsDragging())
	{
		RECT rc;
		CopyRect(&rc,dragger->StopDrag(hWnd,param.mousept.x,param.mousept.y));
		Dockto(m_currState,&rc);
	}
	else
	{
		POINT pt = {param.mousept.x,param.mousept.y};

		ReleaseCapture();
		m_bNCBtDown = 0;
		StopPushFrameControl(hWnd,dragger,&pt);
	}
	return 0;
}

//处理窗口拖动
LRESULT RingDockBar::OnDrag(HWND hWnd,RingDragger* dragger,RINGPARAMS& param)
{
	POINT pt = {param.mousept.x,param.mousept.y};

	if(dragger && dragger->IsDragging())
	{
		RECT rc;
		LPRECT lprc;
		
		CopyRect(&rc,dragger->GetCurrRect());
		//擦除旧矩形
		lprc = dragger->EraseRect(pt.x,pt.y);
/*
RECT rc1;
CopyRect(&rc1,&m_hrc);
MapWindowPoints(hWnd,HWND_DESKTOP,(LPPOINT)&rc1,2);
dragger->DrawRect(&rc1,1);
CopyRect(&rc1,&m_vrc);
MapWindowPoints(hWnd,HWND_DESKTOP,(LPPOINT)&rc1,2);
dragger->DrawRect(&rc1,1);
CopyRect(&rc1,&m_frc);
MapWindowPoints(hWnd,HWND_DESKTOP,(LPPOINT)&rc1,2);
dragger->DrawRect(&rc1,1);
*/
		//计算新旧矩形的偏移量
		rc.left = lprc->left - rc.left;
		rc.top = lprc->top - rc.top;
		//偏移三个矩形
		OffsetRect(&m_hrc,rc.left,rc.top);
		OffsetRect(&m_vrc,rc.left,rc.top);
		OffsetRect(&m_frc,rc.left,rc.top);
		
		//GetWindowRect(m_child->Parent()->Handle(),&rc);
		//MapWindowPoints(HWND_DESKTOP,hWnd,(LPPOINT)&rc,2);
		m_child->Parent()->GetClientRect(&rc);
		MapWindowPoints(m_child->Parent()->Handle(),hWnd,(LPPOINT)&rc,2);
		
		//AdjustDragRects(pt,&rc);

		ClientToScreen(hWnd,&pt);
		
		m_currState = CheckFloatRect(hWnd,lprc,pt);		
		dragger->DrawRect(SetDragRect(hWnd,dragger));
/*
CopyRect(&rc1,&m_hrc);
MapWindowPoints(hWnd,HWND_DESKTOP,(LPPOINT)&rc1,2);
dragger->DrawRect(&rc1,1);
CopyRect(&rc1,&m_vrc);
MapWindowPoints(hWnd,HWND_DESKTOP,(LPPOINT)&rc1,2);
dragger->DrawRect(&rc1,1);
CopyRect(&rc1,&m_frc);
MapWindowPoints(hWnd,HWND_DESKTOP,(LPPOINT)&rc1,2);
dragger->DrawRect(&rc1,1);
*/
	}
	else if(param.wParam == MK_LBUTTON)
		OnPushHoverFrameControl(hWnd,dragger,&pt);
	
	return 0;
}

int RingDockBar::SetDragRect(HWND hWnd,RingDragger* dragger)
{
	switch(m_currState)
	{
		case TBS_FLAG_FLY:
			dragger->SetDragRect(&m_frc,hWnd);
			return 3;
		case TBS_FLAG_DOCKLEFT:
		case TBS_FLAG_DOCKRIGHT:
			dragger->SetDragRect(&m_vrc,hWnd);
			break;
		case TBS_FLAG_DOCKTOP:
		case TBS_FLAG_DOCKBOTTOM:
			dragger->SetDragRect(&m_hrc,hWnd);
			break;			
	}
	return 1;
}

void RingDockBar::UpdateCaption()
{
	if(m_uStyle & TBS_EX_CANCLOSE)
	{
		if(IsVert())
		{
			m_rcClose.left = m_rcDockPos.right - 15;
			m_rcClose.top = m_rcDockPos.top + 3;
			m_rcClose.right = m_rcDockPos.right - 3;
			m_rcClose.bottom = m_rcDockPos.top + 15;
		}
		else
		{
			m_rcClose.left = m_rcDockPos.left + 3;
			m_rcClose.top = m_rcDockPos.top + 3;
			m_rcClose.right = m_rcDockPos.left + 15;
			m_rcClose.bottom = m_rcDockPos.top + 15;
		}
	}
}

void RingDockBar::OffsetDockPos(int x,int y)
{
	OffsetRect(&m_rcClient,x,y);
	OffsetRect(&m_rcDockPos,x,y);
}

BOOL RingDockBar::SetDockPos(int x,int y,int width/*=-999*/,int height/*=-999*/)
{
	if(m_child == NULL)
		return FALSE;

	OffsetDockPos(x - m_rcDockPos.left,y - m_rcDockPos.top);
	return TRUE;
}

void RingDockBar::UpdateFlyBkg()
{
	//SizeBar占满FlyBar，参数为TRUE可能引起闪烁
	if(m_State == TBS_FLAG_FLY && m_flyBar && m_child)
		InvalidateRect(m_child->Handle(),NULL,!IsSizeBar());
}

BOOL RingDockBar::DrawBkg(HDC hDC,LPRECT lprc/*=NULL*/)
{	
	RingDockSite* tmp = NULL;
	RECT rc;
	SIZE offset={0,0};
	
	if(m_State == TBS_FLAG_FLY)
	{
		if(m_flyBar && m_child)
		{
			RingWnd* parent;
		
			parent = (RingWnd*)m_child->Parent();			
			if(parent)
			{
				for(int i=0;i<4;i++)
				{
					tmp = parent->GetDockSite(i);
					if(tmp)
					{
						m_flyBar->GetClientRect(&rc);
						//GetWindowRect(m_flyBar->Handle(),&rc);
						MapWindowPoints(m_flyBar->Handle(),tmp->Handle(),(LPPOINT)&rc,2);
						offset.cx = rc.left;
						offset.cy = rc.top;
						OffsetRect(&rc,-rc.left,-rc.top);
						return tmp->DrawBkg(hDC,&rc,&offset);
					}
				}
			}
		}
	}
	else
		tmp = m_pSite;
		
	if(tmp)
	{		
		if(lprc == NULL)
		{
			//NCPAINT
			CopyRect(&rc,&m_rcClient);
			OffsetRect(&rc,-m_rcClient.left,-m_rcClient.top);
			lprc = &rc;
			//offset.cx = m_rcClient.left;
			//offset.cy = m_rcClient.top;
		}
/*		else
		{
			RingBaseWnd::GetClientRect(&rc);
			CopyRect(lprc,&rc);
			//offset.cx = m_winpos.x + m_rcClient.left;
			//offset.cy = m_winpos.y + m_rcClient.top;
		}
*/
		offset.cx = m_rcClient.left;
		offset.cy = m_rcClient.top;

		return tmp->DrawBkg(hDC,lprc,&offset);
	}
	return FALSE;
}

//////////////////////////////////////
//
//绘制矩形框及手柄，如果子窗口显示状态与m_bVisible不一致，不绘制，返回FALSE，
//调用着应该更新DockSite，因为有可能要进行工具栏重排
//
/////////////////////////////////////
BOOL RingDockBar::Draw(HDC hDC)
{
	BOOL bUpdate;	
	if(IsVisible(&bUpdate) && !bUpdate)
	{
		//可视且不更新才绘制
		DrawFrame(hDC);
		DrawGripper(hDC);
//		if(m_child)
//			m_child->Repaint();//InvalidateRect(m_child->Handle(),NULL,TRUE);
	}
	return !bUpdate;
}

void RingDockBar::DrawFrame(HDC hDC)
{
	if((m_uStyle & TBS_EX_ALLLINE) == TBS_EX_ALLLINE)
	{
		if(IsVert())
			DrawEdge(hDC,&m_rcDockPos,BDR_RAISEDINNER,BF_RECT);
		else
		{
			//if(m_pSite && m_pSite->)
			RECT rc = {m_rcDockPos.left,m_rcDockPos.top-1,m_rcDockPos.right,m_rcDockPos.top+1};
			DrawEdge(hDC,&rc,BDR_SUNKENOUTER,BF_TOP | BF_BOTTOM);
			OffsetRect(&rc,0,m_rcDockPos.bottom - m_rcDockPos.top);
			DrawEdge(hDC,&rc,BDR_SUNKENOUTER,BF_TOP | BF_BOTTOM);
		}
	}
	else if(LOWORD(m_uStyle) != TBS_EX_NONE)
		DrawEdge(hDC,&m_rcDockPos,BDR_RAISEDINNER,BF_RECT);
}

void RingDockBar::DrawGripper(HDC hDC)
{	
	if(LOWORD(m_uStyle) == TBS_EX_NONE)
		return;

	RECT rc;
	CopyRect(&rc,&m_rcDockPos);
	InflateRect(&rc,-1,-2);
	int x=0,y=0,bx=2,by=2;
	
	if(IsVert())
	{
		rc.bottom = rc.top + 3;
		OffsetRect(&rc,0,1);

		if((m_uStyle & TBS_EX_CANCLOSE))
		{
			rc.right -= 17;
			OffsetRect(&rc,0,3);
			bx = rc.right + 2;
		}
		y = 4;
	}
	else
	{
		rc.right = rc.left + 3;
		OffsetRect(&rc,1,0);

		if((m_uStyle & TBS_EX_CANCLOSE))
		{
			rc.top += 17;
			OffsetRect(&rc,3,0);
			by = rc.top - 15;
		}		
		x = 4;
	}
	DrawEdge(hDC,&rc,BDR_RAISEDINNER,BF_RECT);
	OffsetRect(&rc,x,y);
	DrawEdge(hDC,&rc,BDR_RAISEDINNER,BF_RECT);

	if((m_uStyle & TBS_EX_CANCLOSE))
	{
		rc.left = bx;
		rc.top = by;
		rc.right = rc.left + 14;
		rc.bottom = rc.top + 14;
		DrawFrameControl(hDC,&m_rcClose,DFC_CAPTION,DFCS_CAPTIONCLOSE);
	}
}

//返回与前面一个dockbar之间的空间距离，同时修正停靠坐标
//调用该函数前提必须是已停靠
int RingDockBar::GetPreSpace()
{
	int presize = 0;
	
	if(m_child && m_pSite && m_bVisible)
	{
		if(IsVert())
		{
			if(m_prev)
				presize = m_prev->m_rcDockPos.bottom;
			presize = m_rcDockPos.top - presize;
			if(!IsVisible())
				presize += (m_rcDockPos.bottom - m_rcDockPos.top);
		}
		else
		{
			if(m_prev)
				presize = m_prev->m_rcDockPos.right;
			presize = m_rcDockPos.left - presize;
			if(!IsVisible())
				presize += (m_rcDockPos.right - m_rcDockPos.left);
		}		
	}
	return presize;
}

/***************************************************************************
//
//调整停靠位置，nEdge为右边界，nPreSpace为前面可移动空间，工具栏
//超过右边界，应往前移，但移动距离不可超过可移动空间。
//本函数假设所有工具栏都不重叠
//
***************************************************************************/
HDWP RingDockBar::AdjustDockPosH(HDWP hdwp,int nEdge,int nPreSpace,BOOL bCanWrap)
{
	int nPrevLen,offset = 0;

	if(m_child && m_pSite && m_bVisible)
	{
		RECT rcp;
		CopyRect(&rcp,&m_rcClient);
		if(m_prev)
			nPrevLen = m_rcDockPos.left - m_prev->m_rcDockPos.right;

		if(m_rcDockPos.right > nEdge || m_pSite->IsSizeBarInLine(m_lineInfo))
		{
			offset = -min(m_rcDockPos.right - nEdge,nPreSpace);			
			OffsetDockPos(offset,0);
		}
		else if(nPreSpace < 0)
		{
			offset = -nPreSpace;
			OffsetDockPos(offset,0);
		}			
		UpdateCaption();

		hdwp = m_child->DefDeferWindowPos(hdwp,m_rcClient.left,m_rcClient.top,
			m_rcClient.right - m_rcClient.left,m_rcClient.bottom - m_rcClient.top,
			IsAllLineBar()?0:SWP_NOSIZE);
		if(EqualRect(&rcp,&m_rcClient) && m_State == TBS_FLAG_DOCKBOTTOM)
			InvalidateRect(m_child->Handle(),NULL,FALSE);

		if(m_prev)
		{
			nPreSpace -= nPrevLen;//min(offset,m_winpos.x - m_prev->m_winpos.right);
			hdwp = m_prev->AdjustDockPosH(hdwp,m_rcDockPos.left,nPreSpace,bCanWrap);
		}
	}
	return hdwp;
}

void RingDockBar::SetDockPos(HDWP hdwp)
{
	if(IsMenuBar())
		DeferWindowPos(hdwp,m_child->Handle(),0,m_rcClient.left,m_rcClient.top,
		m_rcClient.right - m_rcClient.left,m_rcClient.bottom - m_rcClient.top,
		SWP_NOZORDER|SWP_NOACTIVATE);//|SWP_NOREDRAW);
	else
		DeferWindowPos(hdwp,m_child->Handle(),0,m_rcClient.left,m_rcClient.top,0,0,
		SWP_NOZORDER|SWP_NOSIZE|SWP_NOACTIVATE);//|SWP_NOREDRAW);
}

/***************************************************************************
//
//调整停靠位置，nEdge为下边界，nPreSpace为上面可移动空间，工具栏
//超过下边界，应往上移，但移动距离不可超过可移动空间。
//本函数假设所有工具栏都不重叠
//
***************************************************************************/
HDWP RingDockBar::AdjustDockPosV(HDWP hdwp,int nEdge,int nPreSpace,BOOL bCanWrap)
{
	int nPrevLen,offset = 0;

	if(m_child && m_pSite && m_bVisible)
	{
		RECT rcp;
		CopyRect(&rcp,&m_rcClient);
		if(m_prev)
			nPrevLen = m_rcDockPos.top - m_prev->m_rcDockPos.bottom;

		if(m_rcDockPos.bottom > nEdge)
		{
			offset = -min(m_rcDockPos.bottom - nEdge,nPreSpace);
			OffsetDockPos(0,offset);
		}
		else if(nPreSpace < 0)
		{
			offset = -nPreSpace;
			OffsetDockPos(0,offset);
		}			
		UpdateCaption();
		//if(m_winpos.x > nEdge && bCanWrap)
		//	m_pSite->WrapBar(m_lineInfo,this,(m_next == NULL));
		//else

		hdwp = m_child->DefDeferWindowPos(hdwp,m_rcClient.left,m_rcClient.top,
			m_rcClient.right - m_rcClient.left,m_rcClient.bottom - m_rcClient.top,
			IsAllLineBar()?0:SWP_NOSIZE);
		if(EqualRect(&rcp,&m_rcClient) && m_State == TBS_FLAG_DOCKRIGHT)
			InvalidateRect(m_child->Handle(),NULL,FALSE);
		/*
		if(IsMenuBar())
			hdwp = DeferWindowPos(hdwp,m_child->Handle(),0,m_rcClient.left,m_rcClient.top,
						m_rcClient.right - m_rcClient.left,m_rcClient.bottom - m_rcClient.top,
						SWP_NOZORDER|SWP_NOACTIVATE|SWP_SHOWWINDOW);//|SWP_NOCOPYBITS|SWP_NOREDRAW);
		else
			hdwp = DeferWindowPos(hdwp,m_child->Handle(),0,m_rcClient.left,m_rcClient.top,0,0,
							SWP_NOZORDER|SWP_NOSIZE|SWP_NOACTIVATE|SWP_SHOWWINDOW);//|SWP_NOCOPYBITS|SWP_NOREDRAW);
		//InvalidateRect(m_child->Handle(),NULL,FALSE);
		*/
		if(m_prev && hdwp)
		{
			//if(offset < 0)
			//	offset = m_prev->m_winpos.right - m_winpos.x;
			nPreSpace -= nPrevLen;//min(offset,m_winpos.x - m_prev->m_winpos.right);
			hdwp = m_prev->AdjustDockPosV(hdwp,m_rcDockPos.top,nPreSpace,bCanWrap);
		}
		//else
		//	m_pSite->WrapBar(m_lineInfo,NULL,FALSE);
	}
	return hdwp;
}

//根据工具栏拖动时的矩形判断应该停靠在哪一边，lprc,pt均为屏幕坐标,
//返回停靠位置（上下左右或浮动）
int RingDockBar::CheckFloatRect(HWND hWnd,LPRECT lprc,POINT pt)
{
	RECT rcClient,rcCheck;
	
	//判断窗口区域
	GetWindowRect(m_child->Parent()->Handle(),&rcClient);
	
	if(!PtInRect(&rcClient,pt))
		return TBS_FLAG_FLY;	  //鼠标不在窗口内部，返回浮动

	RingDockSite* pSite;
	RECT rc[4];
	int i,state=TBS_FLAG_FLY,max=0,cmp,hrch,vrcw;
	RECT rcCmp;
	POINT lpt = {pt.x,pt.y};

	ScreenToClient(hWnd,&lpt);
	m_child->Parent()->GetClientRect(&rcClient);
	MapWindowPoints(m_child->Parent()->Handle(),hWnd,(LPPOINT)&rcClient,2);
	
	hrch = m_hrc.bottom - m_hrc.top;
	vrcw = m_vrc.right - m_vrc.left;

	for(i=0;i<4;i++)
	{
		pSite = ((RingWnd*)m_child->Parent())->GetDockSite(i);
		if(CanDock(i))
		{
			if(pSite)
			{
				GetWindowRect(pSite->Handle(),&rcCheck);
				if(PtInRect(&rcCheck,pt))
				{
					//鼠标在该区域，优先返回
					if(i != m_currState)
					{
						ScreenToClient(hWnd,&pt);
						AdjustRects(hWnd,pt,i);
					}
					return i;
				}
				MapWindowPoints(HWND_DESKTOP,hWnd,(LPPOINT)&rcCheck,2);
			}
			else
				CopyRect(&rcCheck,&rcClient);

			switch(i)
			{
			case TBS_FLAG_DOCKTOP:
				if(!pSite || rcCheck.top == rcCheck.bottom)
				{
					//该方向无DockSite或DockSite内无停靠栏,设置判断矩形为
					//与该方向停靠栏等宽/高，与客户区重叠1像素
					rcCheck.bottom = rcCheck.top + 1;
					rcCheck.top = rcCheck.bottom - (m_hrc.bottom - m_hrc.top);
				}
				//记录重叠区域
				if(m_currState == TBS_FLAG_FLY)
				{
					rcCmp.left = m_hrc.left;
					rcCmp.right = m_hrc.right;
					if(lpt.y > m_frc.top + hrch)
					{
						rcCmp.bottom = lpt.y + m_BorderSpace;
						rcCmp.top = rcCmp.bottom - hrch;
					}
					else
					{
						rcCmp.top = m_frc.top;
						rcCmp.bottom = rcCmp.top + hrch;
					}					
				}
				else
					CopyRect(&rcCmp,&m_hrc);
				break;
			case TBS_FLAG_DOCKBOTTOM:
				if(!pSite || rcCheck.top == rcCheck.bottom)
				{
					rcCheck.top = rcCheck.bottom - 1;
					rcCheck.bottom = rcCheck.top + (m_hrc.bottom - m_hrc.top);
				}
				if(m_currState == TBS_FLAG_FLY)
				{
					rcCmp.left = m_hrc.left;
					rcCmp.right = m_hrc.right;
					if(lpt.y < m_frc.bottom - hrch)
					{
						rcCmp.top = lpt.y - m_BorderSpace;
						rcCmp.bottom = rcCmp.top + hrch;
					}
					else
					{
						rcCmp.bottom = m_frc.bottom;
						rcCmp.top = rcCmp.bottom - hrch;
					}					
				}
				else
					CopyRect(&rcCmp,&m_hrc);
				break;
			case TBS_FLAG_DOCKLEFT:
				if(!pSite || rcCheck.left == rcCheck.right)
				{
					rcCheck.right = rcCheck.left + 1;
					rcCheck.left = rcCheck.right - (m_vrc.right - m_vrc.left);
				}
				if(m_currState == TBS_FLAG_FLY)
				{
					rcCmp.top = m_vrc.top;
					rcCmp.bottom = m_vrc.bottom;
					if(lpt.x > m_frc.left + vrcw)
					{
						rcCmp.right = lpt.x + m_BorderSpace;
						rcCmp.left = rcCmp.right - vrcw;
					}
					else
					{
						rcCmp.left = m_frc.left;
						rcCmp.right = rcCmp.left + vrcw;
					}
				}
				else
					CopyRect(&rcCmp,&m_vrc);
				break;
			case TBS_FLAG_DOCKRIGHT:
				if(!pSite || rcCheck.left == rcCheck.right)
				{
					rcCheck.left = rcCheck.right - 1;
					rcCheck.right = rcCheck.left + (m_vrc.right - m_vrc.left);
				}
				if(m_currState == TBS_FLAG_FLY)
				{
					rcCmp.top = m_vrc.top;
					rcCmp.bottom = m_vrc.bottom;
					if(lpt.x < m_frc.right - vrcw)
					{
						rcCmp.left = lpt.x - m_BorderSpace;
						rcCmp.right = rcCmp.left + vrcw;
					}
					else
					{
						rcCmp.right = m_frc.right;
						rcCmp.left = rcCmp.right - vrcw;
					}
				}
				else
					CopyRect(&rcCmp,&m_vrc);
				break;
			}
			//按重叠面积大小判断应该停靠何方
			IntersectRect(rc+i,&rcCmp,&rcCheck);
			if(!IsRectEmpty(rc+i))
			{
				cmp = (rc[i].right - rc[i].left) * (rc[i].bottom - rc[i].top);
				if(cmp > max)
				{
					max = cmp;
					state = i;
				}
			}
		}
	}
	
	if(state != m_currState)// || state == TBS_FLAG_FLY)
	{
		ScreenToClient(hWnd,&pt);
		AdjustRects(hWnd,pt,state);
	}

	return state;
}

/*
int RingDockBar::CheckFloatRect(HWND hWnd,LPRECT lprc,POINT pt)
{
	RECT rcClient,rcCheck;
	
	//判断窗口区域
	GetWindowRect(m_child->Parent()->Handle(),&rcClient);
	
	if(!PtInRect(&rcClient,pt))
		return TBS_FLAG_FLY;	  //鼠标不在窗口内部，返回浮动
	
	RingDockSite* pSite;
	RECT rc[4];
	int i,state=TBS_FLAG_FLY,max=0,cmp;
	LPRECT lprcCmp;
	
	m_child->Parent()->GetClientRect(&rcClient);
	MapWindowPoints(m_child->Parent()->Handle(),hWnd,(LPPOINT)&rcClient,2);
	
	for(i=0;i<4;i++)
	{
		pSite = ((RingWnd*)m_child->Parent())->GetDockSite(i);
		if(CanDock(i))
		{
			if(pSite)
			{
				GetWindowRect(pSite->Handle(),&rcCheck);
				if(PtInRect(&rcCheck,pt))
				{
					//鼠标在该区域，优先返回
					ScreenToClient(hWnd,&pt);
					AdjustRects(hWnd,pt,i);
					return i;
				}
				MapWindowPoints(HWND_DESKTOP,hWnd,(LPPOINT)&rcCheck,2);
			}
			else
				CopyRect(&rcCheck,&rcClient);
			
			if(!pSite || rcCheck.left == rcCheck.right || rcCheck.top == rcCheck.bottom)
			{
				//该方向无DockSite或DockSite内无停靠栏,设置判断矩形为
				//与该方向停靠栏等宽/高，与客户区重叠1像素
				if(i == 0)
				{					
					rcCheck.bottom = rcCheck.top + 1;
					rcCheck.top = rcCheck.bottom - (m_hrc.bottom - m_hrc.top);
				}
				else if(i == 1)
				{
					rcCheck.top = rcCheck.bottom - 1;
					rcCheck.bottom = rcCheck.top + (m_hrc.bottom - m_hrc.top);
				}
				else if(i == 2)
				{
					rcCheck.right = rcCheck.left + 1;
					rcCheck.left = rcCheck.right - (m_vrc.right - m_vrc.left);
				}
				else
				{
					rcCheck.left = rcCheck.right - 1;
					rcCheck.right = rcCheck.left + (m_vrc.right - m_vrc.left);
				}
			}
			if(i < 2)
				lprcCmp = &m_hrc;
			else
				lprcCmp = &m_vrc;
			
			IntersectRect(rc+i,lprcCmp,&rcCheck);
		}
		else
			SetRectEmpty(rc+i);
	}
	//按重叠面积大小判断应该停靠何方
	for(i=0;i<4;i++)
	{
		if(!IsRectEmpty(rc+i))
		{
			cmp = (rc[i].right - rc[i].left) * (rc[i].bottom - rc[i].top);
			if(cmp > max)
			{
				max = cmp;
				state = i;
			}
		}
	}
	//if(state != m_currState || state == TBS_FLAG_FLY)
	{
		ScreenToClient(hWnd,&pt);
		AdjustRects(hWnd,pt,state);
	}
	
	return state;
}
*/
//根据宽高参数返回客户区宽高
int RingDockBar::GetClientWidth(int width)
{
	if(IsHorz())
	{
		if(width == 0)	//==0表示采用父窗口宽度
		{
			if(m_pSite)
				width = ::ClientWidth(m_pSite->Parent()->Handle());
			else
				return 0;
		}
		return width - m_BorderSpace * 2 - m_GripperSpace;
	}
	else if(IsFloat())
	{
		if(width == 0)	//==0表示采用父窗口宽度
		{
			if(m_flyBar)
				width = ::ClientWidth(m_flyBar->Handle());
			else
				return 0;
		}
		else
			width += m_BorderSpace * 2;
		return width;
	}
	else
	{
		if(width == 0)
			width = m_rcDockPos.right - m_rcDockPos.left;
		return width - m_BorderSpace * 2;
	}
}

int RingDockBar::GetClientHeight(int height)
{
	if(IsVert())
	{
		if(height == 0)	//==0表示采用父窗口宽度
		{
			if(m_pSite)
			{
				RECT rc;
				m_pSite->Parent()->GetClientRect(&rc);
				height = rc.bottom - rc.top;
			}
			else
				return 0;
		}
		return height - m_BorderSpace * 2 - m_GripperSpace;
	}
	else
	{
		if(height == 0)
			height = m_rcDockPos.bottom - m_rcDockPos.top;
		return height - m_BorderSpace * 2;
	}
}

//设置停靠坞，如果已停靠，从原停靠坞脱离
BOOL RingDockBar::SetDockSite(RingDockSite* docksite)
{	
	if(docksite == NULL)
	{		
		//浮动，不能改变m_State的值，该值由DockFly用到且函数结束时设置
		if(m_pSite && m_State != TBS_FLAG_FLY && m_State != TBS_FLAG_NONE)
			m_pSite->DelBar(this,FALSE);

		//如果是SizeBar，SizeBar的该函数会改变窗口尺寸
		OnDockStateChange(m_State,TBS_FLAG_FLY);
		return TRUE;
	}
	else
	{		
		//docksite有效，取docksite的停靠位置
		int state = docksite->GetState();
		
		if(m_child)	//已设定子窗口
		{
			if(m_State == TBS_FLAG_FLY)
			{
				//原浮动，隐藏浮动栏
				if(m_flyBar)
					m_flyBar->Show(SW_HIDE);
			}
			else if(m_pSite && m_pSite != docksite)	//已有停靠窗口，脱离
				m_pSite->DelBar(this,FALSE);
			
			//设置子窗口的父窗口为docksite
			m_child->SetParent(docksite->Handle());
			
			int nOldState = m_State;
			m_State = state;
			m_pSite = docksite;

			if(!(nOldState == state || (IsVert() && ::IsVert(nOldState)) ||
				(IsHorz() && ::IsHorz(nOldState))))
				if(0 == SendMessage(m_child->Handle(),WM_DOCKSTATECHANGE,
						MAKELONG(nOldState,state),(LPARAM)m_child->Handle()))
				{
					//未处理，采用默认处理
					OnDockStateChange(nOldState,state);
				}
			
			UpdateSize();
			return TRUE;		
		}
	}
	return FALSE;
}

//lprc:屏幕坐标
BOOL RingDockBar::DockFly(LPRECT lprc/*=NULL*/)
{
	if(m_child == NULL)
		return FALSE;

	if(m_flyBar == NULL)
	{
		m_flyBar = new RingFlyBar(m_child->Parent());
		CopyRect(&m_ldrc,&m_rcDockPos);
		MapWindowPoints(m_pSite->Handle(),m_pSite->Parent()->Handle(),(LPPOINT)&m_ldrc,2);
	}

	if(m_flyBar)
	{
		//winpos与flybar窗口左上角对齐。
		//浮动时停靠栏与停靠在最上方状态相同，其top位置为1，因此垂直位置-1
		OffsetDockPos(-m_rcDockPos.left - WINVAR_FRAME_X,//m_BorderSpace,
							- m_rcDockPos.top - 1 - BARTOOLY);
		
		if(m_child->GetParent() == m_flyBar->Handle())
		{
			if(m_State != TBS_FLAG_FLY)
				m_pSite->RegFlyBar(this);

			m_State = TBS_FLAG_FLY;

			if(lprc)
			{
				m_flyBar->SetPos(lprc->left,lprc->top);
				CopyRect(&m_lfrc,lprc);
				if(m_pSite)
				{
					ScreenToClient(m_pSite->Parent()->Handle(),(LPPOINT)&m_lfrc);
					ScreenToClient(m_pSite->Parent()->Handle(),(LPPOINT)&m_lfrc+1);
				}
			}
			else
				CopyRect(&m_lfrc,&m_frc);

			m_flyBar->Show();
			InvalidateRect(m_child->Handle(),NULL,TRUE);			
			return TRUE;
		}
		else if(m_flyBar->Create(m_szTitle))
		{
			int nOldState = m_State;

			if(m_State != TBS_FLAG_FLY)
				m_pSite->RegFlyBar(this);
			m_State = TBS_FLAG_FLY;

			SendMessage(m_child->Handle(),WM_DOCKSTATECHANGE,
							MAKELONG(nOldState,TBS_FLAG_FLY),(LPARAM)m_child->Handle());
			
			if(lprc)
			{
				m_flyBar->SetPos(lprc->left,lprc->top);
				CopyRect(&m_lfrc,lprc);
				if(m_pSite)
				{
					ScreenToClient(m_pSite->Parent()->Handle(),(LPPOINT)&m_lfrc);
					ScreenToClient(m_pSite->Parent()->Handle(),(LPPOINT)&m_lfrc+1);
				}
			}
			else
				CopyRect(&m_lfrc,&m_frc);
			
			m_flyBar->AddChild(this);
			m_flyBar->Show();
			
			return TRUE;
		}
		else
		{
			delete m_flyBar;
			m_flyBar = NULL;
		}
	}
	return FALSE;
}

//////////////////////////////////////
//
//lprc:为父窗口的相对坐标
//
//////////////////////////////////////
BOOL RingDockBar::Dockto(int state,LPRECT lprc)
{
	if(m_child == NULL)
		return FALSE;

	//条件如果成立，表示停靠位置变动，设置DockSite
	if(m_State != state)
		SetDockSite(((RingWnd*)m_child->Parent())->CreateDockSite(state));
	
	if(state != TBS_FLAG_FLY)
	{
		//AddBar会更新所有DockSite尺寸
		if(m_pSite)
		{
			if(m_State == TBS_FLAG_FLY)
				m_pSite->UnregFlyBar(this);
			
			m_pSite->AddBar(this,lprc);
		}
	}
	else
	{
		RECT rc;

		if(lprc == NULL)
			CopyRect(&rc,&m_frc);
		else
		{
			CopyRect(&rc,lprc);
			ClientToScreen(m_child->Parent()->Handle(),(LPPOINT)&rc);
			ClientToScreen(m_child->Parent()->Handle(),(LPPOINT)&rc+1);
		}

		//更新DockSite尺寸
		if(m_State != TBS_FLAG_FLY && m_State != TBS_FLAG_NONE)
			m_pSite->UpdateAllSite();
				
		DockFly(&rc);
	}
	return TRUE;
}

int RingDockBar::DockBack()
{
	if(m_State == TBS_FLAG_FLY)
	{
		if(IsRectEmpty(&m_ldrc))
			Dockto(TBS_FLAG_DOCKTOP);
		else
			Dockto(m_pSite->GetState(),&m_ldrc);
	}
	else
	{
		CopyRect(&m_ldrc,&m_rcDockPos);
		MapWindowPoints(m_pSite->Handle(),m_pSite->Parent()->Handle(),(LPPOINT)&m_ldrc,2);
		//if(IsRectEmpty(&m_hrc))
		{
			POINT pt = {0,0};
			InitDragRects(m_pSite->Handle(),pt,TRUE);
		}
		Dockto(TBS_FLAG_FLY,&m_lfrc);
	}
	return GetState();
}

//初始化三个矩形
void RingDockBar::InitDragRects(HWND hWnd,POINT pt,BOOL bInit)
{
	if(bInit)
	{
		CopyRect(&m_hrc,GetDragRects(TBS_FLAG_HORZ));
		CopyRect(&m_vrc,GetDragRects(TBS_FLAG_VERT));
		CopyRect(&m_frc,GetDragRects(TBS_FLAG_FLY));
	}
	else
	{
		//CopyRect(&m_frc,GetDragRects(TBS_FLAG_FLY));
		OffsetRect(&m_hrc,m_rcDockPos.left-m_hrc.left,m_rcDockPos.top-m_hrc.top);
		OffsetRect(&m_vrc,m_rcDockPos.left-m_vrc.left,m_rcDockPos.top-m_vrc.top);
		OffsetRect(&m_frc,m_rcDockPos.left-m_frc.left,m_rcDockPos.top-m_frc.top);
	}
	RECT rc = {0,0,0,0};
	if(m_child)
		GetWindowRect(m_child->Parent()->Handle(),&rc);
	MapWindowPoints(HWND_DESKTOP,hWnd,(LPPOINT)&rc,2);
	InitAdjustRects(pt,&rc);	
}

void RingDockBar::AdjustDragRects(POINT& pt,LPRECT lprcCheck)
{
	//浮动状态，把hrc往上移对齐
	if(pt.y < (lprcCheck->top + lprcCheck->bottom)/2)
	{
		//鼠标在上半部分，上对齐
		OffsetRect(&m_hrc,m_frc.left - m_hrc.left,m_frc.top - m_hrc.top);
		if(!PtInRect(&m_hrc,pt))
			OffsetRect(&m_hrc,0,pt.y - m_hrc.bottom);
	}
	else
	{
		//鼠标在下半部分，下对齐
		OffsetRect(&m_hrc,m_frc.left - m_hrc.left,m_frc.bottom - m_hrc.bottom);
		if(!PtInRect(&m_hrc,pt))
			OffsetRect(&m_hrc,0,pt.y - m_hrc.top);
	}
}

void RingDockBar::InitAdjustRects(POINT& pt,LPRECT lprcCheck)
{
	//初始化对齐
	if(m_State == TBS_FLAG_FLY)
	//if(m_CurrState == TBS_FLAG_FLY)
	{		
		//浮动状态，把hrc往上移对齐
		if(pt.y < (lprcCheck->top + lprcCheck->bottom)/2)
		{
			//鼠标在上半部分，上对齐
			OffsetRect(&m_hrc,m_frc.left - m_hrc.left,m_frc.top - m_hrc.top);
			if(!PtInRect(&m_hrc,pt))
				OffsetRect(&m_hrc,0,pt.y - m_hrc.bottom);
		}
		else
		{
			//鼠标在下半部分，下对齐
			OffsetRect(&m_hrc,m_frc.left - m_hrc.left,m_frc.bottom - m_hrc.bottom);
			if(!PtInRect(&m_hrc,pt))
				OffsetRect(&m_hrc,0,pt.y - m_hrc.top);
		}
	}
	else if(IsVert())
	{
		//纵向
		if(pt.y < (lprcCheck->top + lprcCheck->bottom)/2)
		{
			//鼠标在上半部分，hrc与vrc左上角对齐
			OffsetRect(&m_hrc,m_vrc.left - m_hrc.left,m_vrc.top - m_hrc.top);
			if(!PtInRect(&m_hrc,pt))	//鼠标不在hrc范围，hrc底边与鼠标对齐
				OffsetRect(&m_hrc,0,pt.y - m_hrc.bottom + m_BorderSpace);
		}
		else
		{
			//鼠标在下半部分，hrc与vrc左下角对齐
			OffsetRect(&m_hrc,m_vrc.left - m_hrc.left,m_vrc.bottom - m_hrc.bottom);
			if(!PtInRect(&m_hrc,pt))	//鼠标不在hrc范围，hrc上边与鼠标对齐
				OffsetRect(&m_hrc,0,pt.y - m_hrc.left - m_BorderSpace);
		}
		//frc与vrc左上角对齐
		OffsetRect(&m_frc,m_vrc.left - m_frc.left,m_vrc.top - m_frc.top);
		if(!PtInRect(&m_frc,pt))
		{
			//鼠标不在frc范围，frc中线移至鼠标位置(纵向)
			OffsetRect(&m_frc,0,pt.y - (m_frc.bottom + m_frc.top)/2);
			if(!PtInRect(&m_frc,pt))
				//仍不在，frc中线移至鼠标位置(横向)
				OffsetRect(&m_frc,pt.x - (m_frc.right + m_frc.left)/2,0);
		}
	}
	else
	{		
		if(pt.x < (lprcCheck->right + lprcCheck->left)/2)
		{
			//鼠标在左半部分，左对齐
			OffsetRect(&m_vrc,m_hrc.left - m_vrc.left,m_hrc.top - m_vrc.top);
			if(!PtInRect(&m_vrc,pt))
				OffsetRect(&m_vrc,pt.x - m_vrc.right,0);
		}
		else
		{
			//鼠标在右半部分，右对齐
			OffsetRect(&m_vrc,m_hrc.right - m_vrc.right,m_hrc.top - m_vrc.top);
			if(!PtInRect(&m_vrc,pt))
				OffsetRect(&m_vrc,pt.x - m_vrc.left,0);
		}
		//frc与hrc左上角对齐
		OffsetRect(&m_frc,m_hrc.left - m_frc.left,m_hrc.top - m_frc.top);
		if(!PtInRect(&m_frc,pt))
		{
			//鼠标不在frc范围，frc中线移至鼠标位置(横向)
			OffsetRect(&m_frc,pt.x - (m_frc.right + m_frc.left)/2,0);
			if(!PtInRect(&m_frc,pt))
				//仍不在，frc中线移至鼠标位置(纵向)
				OffsetRect(&m_frc,0,pt.x - (m_frc.bottom + m_frc.top)/2);
		}
	}	
}

/*********************************************************
//
//调整三个矩形,state：即将变成的状态，m_CurrState为原状态
//
//调整规则：
//		矩形由停靠状态变为浮动状态，先调整m_frc,
//		然后需要调整横竖矩形
//		根据m_frc为横竖向（宽高比决定），如果高比较大
//		则根据鼠标在客户区位置：上半部分，m_hrc与m_frc
//		上对齐，下半部分下对齐。宽比较大，则判断左右
//
*********************************************************/
void RingDockBar::AdjustRects(HWND hWnd,POINT pt,int state)
{
	switch(state)
	{
	case TBS_FLAG_DOCKTOP:
		OffsetRect(&m_hrc,0,m_frc.top - m_hrc.top);
		if(!PtInRect(&m_hrc,pt))
			OffsetRect(&m_hrc,0,pt.y - m_hrc.bottom + m_BorderSpace);
		if(!PtInRect(&m_hrc,pt))
			OffsetRect(&m_hrc,m_frc.left - m_hrc.left,0);
		break;
	case TBS_FLAG_DOCKBOTTOM:
		OffsetRect(&m_hrc,0,m_frc.bottom - m_hrc.bottom);
		if(!PtInRect(&m_hrc,pt))
			OffsetRect(&m_hrc,0,pt.y - m_hrc.top - m_BorderSpace);
		if(!PtInRect(&m_hrc,pt))
			OffsetRect(&m_hrc,m_frc.left - m_hrc.left,0);
		break;
	case TBS_FLAG_DOCKLEFT:
		OffsetRect(&m_vrc,m_frc.left - m_vrc.left,0);
		if(!PtInRect(&m_vrc,pt))
			OffsetRect(&m_vrc,pt.x - m_vrc.right + m_BorderSpace,0);
		if(!PtInRect(&m_hrc,pt))
			OffsetRect(&m_vrc,0,m_frc.top - m_vrc.top);
		break;
	case TBS_FLAG_DOCKRIGHT:
		OffsetRect(&m_vrc,m_frc.right - m_vrc.right,0);
		if(!PtInRect(&m_vrc,pt))
			OffsetRect(&m_vrc,pt.x - m_vrc.left - m_BorderSpace,0);
		if(!PtInRect(&m_hrc,pt))
			OffsetRect(&m_vrc,0,m_frc.top - m_vrc.top);
		break;
	case TBS_FLAG_FLY:
		//if((m_frc.bottom - m_frc.top) > (m_frc.right - m_frc.left))
		if(::IsHorz(m_currState))
		{
			if(m_currState == TBS_FLAG_DOCKTOP)
			{
				OffsetRect(&m_frc,0,m_hrc.top - m_frc.top);
				if(!PtInRect(&m_frc,pt))
					OffsetRect(&m_frc,0,pt.y - m_frc.bottom + m_BorderSpace);
			}
			else if(m_currState == TBS_FLAG_DOCKBOTTOM)
			{
				OffsetRect(&m_frc,0,m_hrc.bottom - m_frc.bottom);
				if(!PtInRect(&m_frc,pt))
					OffsetRect(&m_frc,0,pt.y - m_frc.top - m_BorderSpace);
			}
			//上下移动后不在鼠标范围，左右移动至中线与鼠标对齐
			if(!PtInRect(&m_frc,pt))
				OffsetRect(&m_frc,pt.x - (m_frc.right - m_frc.left)/2,0);
			
			//frc已上下移动，调整vrc
			OffsetRect(&m_vrc,0,m_frc.top - m_vrc.top);
		}
		else
		{
			if(m_currState == TBS_FLAG_DOCKLEFT)
			{
				OffsetRect(&m_frc,m_vrc.left - m_frc.left,0);
				if(!PtInRect(&m_frc,pt))
					OffsetRect(&m_frc,pt.x - m_frc.right + m_BorderSpace,0);
			}
			else if(m_currState == TBS_FLAG_DOCKRIGHT)
			{
				OffsetRect(&m_frc,m_vrc.right - m_frc.right,0);
				if(!PtInRect(&m_frc,pt))
					OffsetRect(&m_frc,pt.x - m_frc.left - m_BorderSpace,0);
			}
				
			if(!PtInRect(&m_frc,pt))
				OffsetRect(&m_frc,pt.y - (m_frc.bottom - m_frc.top)/2,0);
			//frc已左右移动，调整hrc
			OffsetRect(&m_hrc,m_frc.left - m_hrc.left,0);
		}
	}
	/*原代码
	{
	//变为m_frc
	RECT rcCheck;
	m_child->Parent()->GetClientRect(&rcCheck);
	MapWindowPoints(m_child->Parent()->Handle(),hWnd,(LPPOINT)&rcCheck,2);
	
	  if((m_frc.bottom - m_frc.top) > (m_frc.right - m_frc.left))
	  {
	  if(state != m_currState)
	  {
	  if(m_currState == TBS_FLAG_DOCKTOP)
					OffsetRect(&m_frc,0,m_hrc.top - m_frc.top);
					else if(m_currState == TBS_FLAG_DOCKBOTTOM)
					OffsetRect(&m_frc,0,m_hrc.bottom - m_frc.bottom);
					
					  if(!PtInRect(&m_frc,pt))
					  OffsetRect(&m_frc,pt.x - (m_frc.right - m_frc.left)/2,0);
					  //frc已上下移动，调整vrc
					  OffsetRect(&m_vrc,0,m_frc.top - m_vrc.top);
					  }
					  //调整hrc
					  if(pt.y > (rcCheck.bottom + rcCheck.top)/2)
					  {
					  //鼠标在下半部分，下移m_hrc;
					  OffsetRect(&m_hrc,0,m_frc.bottom - m_hrc.bottom);
					  if(pt.y <= m_hrc.top)
					  OffsetRect(&m_hrc,0,pt.y - m_hrc.top - m_BorderSpace);
					  }
					  else
					  {
					  //鼠标在上半部分，上移m_hrc;
					  OffsetRect(&m_hrc,0,m_frc.top - m_hrc.top);
					  if(pt.y >= m_hrc.bottom)
					  OffsetRect(&m_hrc,0,pt.y - m_hrc.bottom + m_BorderSpace);
					  }
					  }
					  else
					  {
					  if(state != m_currState)
					  {
					  if(m_currState == TBS_FLAG_DOCKLEFT)
					  OffsetRect(&m_frc,m_vrc.left - m_frc.left,0);
					  else if(m_currState == TBS_FLAG_DOCKRIGHT)
					  OffsetRect(&m_frc,m_vrc.right - m_frc.right,0);
					  
						 if(!PtInRect(&m_frc,pt))
						 OffsetRect(&m_frc,pt.y - (m_frc.bottom - m_frc.top)/2,0);
						 //frc已左右移动，调整hrc
						 OffsetRect(&m_hrc,m_frc.left - m_hrc.left,0);
						 }
						 //调整vrc
						 if(pt.x > (rcCheck.right + rcCheck.left)/2)
						 {
						 //鼠标在右半部分，右移m_vrc;
						 OffsetRect(&m_vrc,m_frc.right - m_vrc.right,0);
						 if(pt.x <= m_vrc.left)
						 OffsetRect(&m_vrc,pt.x - m_vrc.left - m_BorderSpace,0);
						 }
						 else
						 {
						 //鼠标在左半部分，左移m_vrc;
						 OffsetRect(&m_vrc,m_frc.left - m_vrc.left,0);
						 if(pt.x >= m_vrc.right)
						 OffsetRect(&m_vrc,pt.x - m_vrc.right + m_BorderSpace,0);
						 }
						 }
						 }
	*/
}

BOOL RingDockBar::AdjustSizeChildH()
{
	int step;
	
	m_rcDockPos.left = 0;	
	m_rcDockPos.right = m_pSite->m_rcPos.right - m_pSite->m_rcPos.left;
	m_rcClient.left = m_BorderSpace + m_GripperSpace;
	m_rcClient.right = m_rcDockPos.right - m_BorderSpace;

	step = m_pSite->m_rcPos.right - m_pSite->m_rcPos.left - m_GripperSpace - m_BorderSpace*2;	
	step = m_child->OnParentNotify(NULL,MYWM_SIZE,step,TBS_FLAG_HORZ);
	m_nSize = step + m_BorderSpace * 2;
	
	if(m_lineInfo)
	{
		//纵向尺寸
		m_rcDockPos.top = m_lineInfo->m_pos;
		m_rcDockPos.bottom = m_rcDockPos.top + m_nSize;
		m_rcClient.top = m_rcDockPos.top + m_BorderSpace;
		m_rcClient.bottom = m_rcClient.top + step;

		step = m_nSize - m_lineInfo->m_maxSize;
		if(step != 0)
		{
			m_lineInfo->m_maxSize = m_nSize;
			if(m_pSite && m_State != TBS_FLAG_FLY)
				m_pSite->UpdateLine(m_lineInfo,step,FALSE);
		}
		return TRUE;
	}
	return FALSE;
}

BOOL RingDockBar::AdjustSizeChildV()
{
	int step;
	
	m_rcDockPos.top = 0;	
	m_rcDockPos.bottom = m_pSite->m_rcPos.bottom - m_pSite->m_rcPos.top;
	m_rcClient.top = m_BorderSpace + m_GripperSpace;
	m_rcClient.bottom = m_rcDockPos.bottom - m_BorderSpace;
	
	step = m_pSite->m_rcPos.bottom - m_pSite->m_rcPos.top - m_GripperSpace - m_BorderSpace*2;	
	step = m_child->OnParentNotify(NULL,MYWM_SIZE,step,TBS_FLAG_VERT);
	m_nSize = step + m_BorderSpace * 2;
	
	if(m_lineInfo)
	{
		//横向尺寸
		m_rcDockPos.left = m_lineInfo->m_pos;
		m_rcDockPos.right = m_rcDockPos.left + m_nSize;
		m_rcClient.left = m_rcDockPos.left + m_BorderSpace;
		m_rcClient.right = m_rcClient.left + step;
		
		step = m_nSize - m_lineInfo->m_maxSize;
		if(step != 0)
		{
			m_lineInfo->m_maxSize = m_nSize;
			if(m_pSite && m_State != TBS_FLAG_FLY)
				m_pSite->UpdateLine(m_lineInfo,step,FALSE);
		}
		return TRUE;
	}
	return FALSE;
}


BOOL RingDockBar::AdjustChildH(int)
{
	if(m_child && m_pSite && m_bVisible)
	{
		if(IsAllLineBar())
			return AdjustSizeChildH();

		if(m_lineInfo)
		{
			//纵向尺寸
			m_rcDockPos.top = m_lineInfo->m_pos;
			if(m_pSite->IsSizeBarInLine(m_lineInfo))
			{
				//有SizeBar
				if(m_prev == NULL || !m_prev->IsVisible())
					m_rcDockPos.left = 0;
				else
					m_rcDockPos.left = m_prev->m_rcDockPos.right;
			}
			UpdateSize();
		}
		return TRUE;
	}
	return FALSE;
}

BOOL RingDockBar::AdjustChildV(int)
{
	if(m_child && m_pSite && m_bVisible)
	{
		if(IsAllLineBar())
			return AdjustSizeChildV();
		
		if(m_lineInfo)
		{
			//横向尺寸
			m_rcDockPos.left = m_lineInfo->m_pos;
			if(m_pSite->IsSizeBarInLine(m_lineInfo))
			{
				//有SizeBar
				if(m_prev == NULL || !m_prev->IsVisible())
					m_rcDockPos.top = 0;
				else
					m_rcDockPos.top = m_prev->m_rcDockPos.bottom;
			}
			UpdateSize();
		}
		return TRUE;
	}
	return FALSE;
}

///////////////////////////////////////////////////////
//
//	RingAllLineBar
//
///////////////////////////////////////////////////////
RingAllLineBar::RingAllLineBar()
{
	m_bSetH = m_bSetV = FALSE;
}

BOOL RingAllLineBar::ResetDragRect(LPRECT lprc,int state)
{
	RECT rc = {0,0,0,0};
	NMHDR hdr;
	hdr.hwndFrom = m_child->Handle();
	hdr.code = state;
	hdr.idFrom = (UINT)m_child;
	
	if(SendMessage(m_child->Handle(),WM_GETDRAGRECTS,(WPARAM)&rc,(LPARAM)&hdr))
	{
		InflateRect(&rc,m_BorderSpace,m_BorderSpace);
			
		if(::IsVert(state))
			rc.top -= m_GripperSpace + m_BorderSpace;
		else
			rc.left -= m_GripperSpace + m_BorderSpace;
		
		OffsetRect(&rc,lprc->left-rc.left,lprc->top-rc.top);
		CopyRect(lprc,&rc);
		return TRUE;
	}
	return FALSE;
}

//重载函数，设置拖动矩形，返回矩形宽度
int RingAllLineBar::SetDragRect(HWND hWnd,RingDragger* dragger)
{
	if(m_currState != m_State)
	{
		//恢复原矩形
		if(m_child)
		{
			if(::IsVert(m_State) && !m_bSetV)
			{
				if(!m_bSetV)
					m_bSetV = ResetDragRect(&m_vrc,TBS_FLAG_DOCKRIGHT);
			}
			else
			{
				if(!m_bSetH)
					m_bSetH = ResetDragRect(&m_hrc,TBS_FLAG_DOCKBOTTOM);
			}			
		}
	}
	return RingDockBar::SetDragRect(hWnd,dragger);
}

LRESULT RingAllLineBar::StopDrag(HWND hWnd,RingDragger* dragger,RINGPARAMS& param)
{
	m_bSetH = m_bSetV = FALSE;
	return RingDockBar::StopDrag(hWnd,dragger,param);
}
/*
void RingAllLineBar::DrawFrame(HDC hDC)
{
	if(LOWORD(m_uStyle) != TBS_EX_NONE)
		if(IsVert())
			DrawEdge(hDC,(LPRECT)&m_winpos,BDR_RAISEDINNER,BF_RECT);
		else
			DrawEdge(hDC,(LPRECT)&m_winpos,BDR_RAISEDINNER,BF_BOTTOM|BF_TOP);
}
*/