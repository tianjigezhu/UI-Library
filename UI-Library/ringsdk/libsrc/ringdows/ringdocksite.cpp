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
//原文件名：ringctrlbar.cpp
//
//本文件说明：
//		工具容纳窗口实现代码，该工具容纳窗口只是一个容器，
//		容纳类似IE可移动的菜单栏及工具栏。
//		本窗口类位于父窗口四边，根据工具栏等自动调整尺寸及
//		隐藏/显示，所有操作自动完成，因此用户无须关心及调用
//		本类代码。
//
//
**********************************************************************/

#define MAKE_SELF_LIB
#include "ringdows.h"

LPRINGBARLINEINFO RingDockSite::m_EmptyLine = NULL;
RingDockBar* RingDockSite::m_flybar = NULL;

RingDockSite::RingDockSite(RingBaseWnd* parent,DWORD state/*=TBS_FLAG_DOCKTOP*/)
{
	m_windowType = ITIS_DOCKSITE;
	SetParentObj(parent);
	SetAutoDelete();

	m_SplitterSize = RING_SPLITERSIZE;

   RtlZeroMemory(&m_Background,sizeof(BARBMPINFO));
	m_First = m_Curr = NULL;
	m_nChild = 0;
	m_CaptureBar = NULL;

	if(state > TBS_FLAG_DOCKRIGHT || state < TBS_FLAG_DOCKTOP)
		state = TBS_FLAG_DOCKTOP;

	m_State = state;

	RingWnd* temp = (RingWnd*)m_parent;
	temp->GetClientRect(&m_rcPos);
	RECT rc;
	::GetClientRect(m_parent->Handle(),&rc);

	m_windowInfo.hCursor = LoadCursor(NULL,IDC_ARROW);

   switch(m_State)
	{
		case TBS_FLAG_DOCKTOP:
			m_rcPos.left = 0;
			m_rcPos.right = rc.right;
			m_rcPos.bottom = 0;			
			m_cursor = LoadCursor(NULL,IDC_SIZENS);
			m_cursorLine = LoadCursor(NULL,IDC_SIZEWE);
			break;
		case TBS_FLAG_DOCKBOTTOM:
			m_rcPos.left = 0;
			m_rcPos.right = rc.right;
			m_rcPos.top = m_rcPos.bottom;
			m_cursor = LoadCursor(NULL,IDC_SIZENS);
			m_cursorLine = LoadCursor(NULL,IDC_SIZEWE);
			break;
		case TBS_FLAG_DOCKLEFT:
			m_rcPos.right = 0;
			m_cursor = LoadCursor(NULL,IDC_SIZEWE);
			m_cursorLine = LoadCursor(NULL,IDC_SIZENS);
			break;
		case TBS_FLAG_DOCKRIGHT:
			m_rcPos.left = m_rcPos.right;
			m_cursor = LoadCursor(NULL,IDC_SIZEWE);
			m_cursorLine = LoadCursor(NULL,IDC_SIZENS);
			break;
	}

	if(temp->m_dockSite)
		for(int i=0;i<4;i++)
			if(temp->m_dockSite[i] && temp->m_dockSite[i]->m_Background.hBrush)
			{
				//SetBgBmp(temp->m_dockSite[i]->m_Background.hbmp);
				m_Background = temp->m_dockSite[i]->m_Background;
				m_Background.bNeedDelete = FALSE;
				break;
			}
}

RingDockSite::~RingDockSite()
{
	if(m_State != TBS_FLAG_DOCKTOP)
		m_pInterMenu = NULL;
	Free();
}

//本函数必须由父窗口的CreateDockSite调用
HWND RingDockSite::Create()
{
/*	WNDCLASSEX wndclsex =
   {
		sizeof(WNDCLASSEX),
			CS_DBLCLKS,
			InitWndProc,
			0,
			0,
			GetInstance(),
			NULL,
			NULL,
			NULL,//(HBRUSH)(COLOR_BTNFACE + 1),
			NULL,
			"RingDockSite",
			NULL
   };
	
   RegRingdowClass(&wndclsex);
*/
	if(!Register("RingDockSite",
		(HBRUSH)NULL,
		NULL,
		NULL,
		m_windowInfo.hCursor,
		NULL))
		return NULL;

	if(m_Background.hBrush == NULL)
	{
		//m_Background.cx = m_Background.cy = 0;
		m_Background.hBrush = CreateSolidBrush(GetSysColor(COLOR_BTNFACE));
		m_Background.bNeedDelete = TRUE;
	}
	
   CreateEx("","RingDockSite",WS_CHILD | WS_CLIPCHILDREN | WS_CLIPSIBLINGS);// | WS_VISIBLE);
	Show();
   return m_hWnd;
}

void RingDockSite::Free()
{
	if(m_Background.bNeedDelete)
   	DeleteObject(m_Background.hBrush);

	LPRINGBARLINEINFO pre,tmp = m_First;
	RingDockBar* dockbar,*save;
	
	while(tmp)
	{
		dockbar = tmp->m_first;
		pre = tmp;
		tmp = tmp->m_nextline;
		while(dockbar)
		{
			save = dockbar->m_next;
			delete dockbar;
			dockbar = save;
		}		
		Del(pre);
	}
	tmp = m_EmptyLine;
	m_EmptyLine = NULL;
	while(tmp)
	{
		pre = tmp;
		tmp = tmp->m_nextline;
		Del(pre);
	}
}

RingCoolMenu* RingDockSite::GetDockSiteMenu()
{
	if(m_State == TBS_FLAG_DOCKTOP)
	{
		if(m_pInterMenu)
			return m_pInterMenu;
		
		m_pInterMenu = new RingCoolMenu;
		if(m_pInterMenu)
			m_pInterMenu->CreatePopup();
			
		return m_pInterMenu;
	}
	else
	{
		RingDockSite* pSite = ((RingWnd*)m_parent)->CreateDockSite(TBS_FLAG_DOCKTOP);
		if(pSite)
			return pSite->GetDockSiteMenu();
	}
	return NULL;
}

//////////////////////////////////////////////////////
//
//设置画刷时如果原画刷需要销毁，则需要遍历其他DockSite，
//因为其他DockSite也可能在使用该画刷
//
//////////////////////////////////////////////////////
void RingDockSite::UpdateAllSiteBkg(HBRUSH hBrush)
{
	RingWnd* temp = (RingWnd*)m_parent;
	if(temp->m_dockSite)
	{
		for(int i=0;i<4;i++)
		{
			if(temp->m_dockSite[i] && temp->m_dockSite[i]->m_Background.hBrush == hBrush)
			{
				RtlCopyMemory(&temp->m_dockSite[i]->m_Background,&m_Background,sizeof(BARBMPINFO));
				temp->m_dockSite[i]->m_Background.bNeedDelete = FALSE;
				temp->m_dockSite[i]->UpdateBkg();
			}
		}			
	}	
}

void RingDockSite::SetBgBmp(HBITMAP hbm,BOOL bSetOrg/*=FALSE*/)
{
	if(hbm)
	{
		HBRUSH hBrush = m_Background.hBrush;
		GetBmpSize(hbm,(SIZE*)&m_Background);
		m_Background.hBrush = CreatePatternBrush(hbm);		
		m_Background.bSetOrg = bSetOrg;

		if(m_Background.bNeedDelete)
		{
			UpdateAllSiteBkg(hBrush);
			DeleteObject(hBrush);
		}
		m_Background.bNeedDelete = FALSE;
		UpdateBkg();
	}
	else
		SetBgColor(GetSysColor(COLOR_BTNFACE));
}

void RingDockSite::SetBgBmp(LPCTSTR lpszBmp,BOOL bSetOrg/*=FALSE*/,HINSTANCE hInst/*=GetInstance()*/)
{
   HBITMAP hbmp = NULL;
	if(lpszBmp)
		hbmp = LoadBitmap(hInst,lpszBmp);
   
	SetBgBmp(hbmp,bSetOrg);
	if(hbmp)
	{
		DeleteObject(hbmp);
		m_Background.bNeedDelete = TRUE;
	}
}

void RingDockSite::SetBgColor(COLORREF crColor)
{
	HBRUSH hBrush = m_Background.hBrush;
	
	m_Background.cx = m_Background.cy = 0;
	m_Background.hBrush = CreateSolidBrush(crColor);
	m_Background.bSetOrg = FALSE;
	if(m_Background.bNeedDelete)
	{
		UpdateAllSiteBkg(hBrush);
		DeleteObject(hBrush);
	}
	m_Background.bNeedDelete = TRUE;
	UpdateBkg();
}

LPRINGBARLINEINFO RingDockSite::GetEmptyLine()
{
	if(m_EmptyLine)
	{
		LPRINGBARLINEINFO line = m_EmptyLine;
		m_EmptyLine = m_EmptyLine->m_nextline;
		memset(line,0,sizeof(RINGBARLINEINFO));
		return line;
	}
	else
		return (LPRINGBARLINEINFO)New(sizeof(RINGBARLINEINFO));
}

void RingDockSite::SetLineEmpty(LPRINGBARLINEINFO line)
{
	if(line)
	{
		memset(line,0,sizeof(RINGBARLINEINFO));
		line->m_nextline = m_EmptyLine;
		m_EmptyLine = line;
	}
}

void RingDockSite::RegFlyBar(RingDockBar* dockbar)
{
	if(m_flybar == NULL)
		m_flybar = dockbar;
	else if(dockbar)
	{
		dockbar->m_next = m_flybar->m_next;
		m_flybar = dockbar;
	}
}

void RingDockSite::UnregFlyBar(RingDockBar* dockbar)
{
	RingDockBar* bar = m_flybar,*prevbar = NULL;
	
	if(dockbar)
	{
		while(bar)
		{
			if(bar == dockbar)
			{
				if(prevbar == NULL)
					m_flybar = bar->m_next;
				else
					prevbar->m_next = bar->m_next;
				return;
			}
			prevbar = bar;
			bar = bar->m_next;
		}
	}
}

//判断鼠标是否在DockBar可移动边框内
RingDockBar* RingDockSite::CheckMouseInDockBar(RINGPARAMS& param)
{
	LPRINGBARLINEINFO tmp = m_First;
	POINT pt = {param.mousept.x,param.mousept.y};
	RingDockBar* dockbar;

	while(tmp)
	{
		if(tmp->isVisible)
		{
			dockbar = tmp->m_first;
			while(dockbar)
			{
				if(dockbar->IsVisible() && PtInRect(&dockbar->m_rcDockPos,pt))
					return dockbar;
				dockbar = dockbar->m_next;
			}
		}
		tmp = tmp->m_nextline;
	}
	return NULL;
}

//判断鼠标是否在SizeBar移动边框内，
//同时计算该移动框可移动的极限位置
BOOL RingDockSite::CheckMouseInSpliter(RINGPARAMS& param)
{
	LPRINGBARLINEINFO tmp = m_First;
	POINT pt = {param.mousept.x,param.mousept.y};
	
	BOOL bIn = FALSE;

	while(tmp)
	{
		if(tmp->isVisible && PtInRect(&tmp->m_SplitRc,pt))//tmp->m_SplitRc.bottom != 0)
		{			
			bIn = TRUE;
			m_Curr = tmp;
			m_CaptureBar = NULL;
			SetCursor(m_cursor);
			
			RECT crc;
			((RingWnd*)m_parent)->GetClientRect(&crc);
			switch(m_State)
			{
				case TBS_FLAG_DOCKTOP:
					m_ptLimit.x = tmp->m_SplitRc.top - tmp->m_maxSize + 10;
					m_ptLimit.y = crc.bottom;
					break;
				case TBS_FLAG_DOCKBOTTOM:
					m_ptLimit.x = crc.top - m_rcPos.top;
					m_ptLimit.y = tmp->m_SplitRc.bottom + tmp->m_maxSize - 10;
					break;
				case TBS_FLAG_DOCKLEFT:
					m_ptLimit.x = tmp->m_SplitRc.left - tmp->m_maxSize + 10;
					m_ptLimit.y = crc.right;
					break;
				case TBS_FLAG_DOCKRIGHT:
					m_ptLimit.x = crc.left - m_rcPos.left;
					m_ptLimit.y = tmp->m_SplitRc.right + tmp->m_maxSize - 10;
					break;
			}
			break;						
		}
		tmp = tmp->m_nextline;
	}
	return bIn;
}

//处理拖动分隔栏
void RingDockSite::OnDragSpliter(RINGPARAMS& param)
{
	//注意：param的鼠标坐标相对于本窗口，
	//		  m_rcPos,crc,rc坐标相对于父窗口，
	//		  m_dragger.GetCurrRect()坐标相对于屏幕
	//		  计算时必须统一坐标
	RECT rc,crc;
	((RingWnd*)m_parent)->GetClientRect(&crc);
	
	if(m_bSPLimited)
	{			
		if(m_State > TBS_FLAG_DOCKBOTTOM)
		{
			if(param.mousept.x < m_ptLimit.x || param.mousept.x > m_ptLimit.y)
			{
				m_dragger.GetCurrRect(m_hWnd,&rc);
				m_dragger.SetPointX(rc.left);
				return;
			}
		}
		else
		{
			if(param.mousept.y < m_ptLimit.x || param.mousept.y > m_ptLimit.y)
			{
				m_dragger.GetCurrRect(m_hWnd,&rc);
				m_dragger.SetPointY(rc.top);
				return;
			}
		}
	}
	int ox = 0,oy = 0;
	LPRECT lprc = m_dragger.EraseRect(param.mousept.x,param.mousept.y,m_parent->Handle(),&rc);
	
	m_bSPLimited = TRUE;
	OffsetRect(&rc,-m_rcPos.left,-m_rcPos.top);
	switch(m_State)
	{
	case TBS_FLAG_DOCKTOP:
		ox = -rc.left;
		if(rc.bottom >= m_ptLimit.y)
			oy = max(m_ptLimit.y,10) - rc.bottom;
		else if(rc.top <= m_ptLimit.x)
			oy = m_ptLimit.x - rc.top;
		else
			m_bSPLimited = FALSE;
		break;
	case TBS_FLAG_DOCKBOTTOM:
		ox = -rc.left;
		if(rc.top <= m_ptLimit.x)
			oy = m_ptLimit.x - rc.top;
		else if(rc.bottom >= m_ptLimit.y)
			oy = m_ptLimit.y - rc.bottom;
		else
			m_bSPLimited = FALSE;				
		break;
	case TBS_FLAG_DOCKLEFT:
		oy = -rc.top;
		if(rc.right > m_ptLimit.y)
			ox = max(m_ptLimit.y,10) - rc.right;
		else if(rc.left < m_ptLimit.x)
			ox = m_ptLimit.x - rc.left;
		else
			m_bSPLimited = FALSE;				
		break;
	case TBS_FLAG_DOCKRIGHT:
		oy = -rc.top;
		if(rc.left <= m_ptLimit.x)
			ox = m_ptLimit.x - rc.left;
		else if(rc.right >= m_ptLimit.y)
			ox = m_ptLimit.y - rc.right;
		else
			m_bSPLimited = FALSE;
		break;
	}
	OffsetRect(lprc,ox,oy);
	m_dragger.DrawRect(4);
}

LRESULT RingDockSite::OnLButtonDbClk(RINGPARAMS& param)
{
	RingDockBar* bar = CheckMouseInDockBar(param);
	if(bar)
		bar->DockBack();
	return 0;
}

LRESULT RingDockSite::OnLButtonDown(RINGPARAMS& param)
{
	m_bSPLimited = FALSE;

	if(CheckMouseInSpliter(param))
	{
		if(m_ptLimit.y < m_ptLimit.x)
			m_ptLimit.y = m_ptLimit.x + 10;
		m_dragger.StartDrag(m_hWnd,param.mousept.x,param.mousept.y,&m_Curr->m_SplitRc,4);
		return 0;
	}
	else
	{
		m_CaptureBar = CheckMouseInDockBar(param);
		if(m_CaptureBar)
			return m_CaptureBar->StartDrag(m_hWnd,&m_dragger,param);
	}
	return 0;
}

LRESULT RingDockSite::OnMouseMove(RINGPARAMS& param)
{	
	if(m_CaptureBar)
		return m_CaptureBar->OnDrag(m_hWnd,&m_dragger,param);
	else if(m_dragger.IsDragging())
		OnDragSpliter(param);
	else if(!CheckMouseInSpliter(param))
		SetCursor(m_windowInfo.hCursor);

	return 0;
}

LRESULT RingDockSite::OnLButtonUp(RINGPARAMS& param)
{
	SetCursor(m_windowInfo.hCursor);

	if(m_CaptureBar)
	{
		m_CaptureBar->StopDrag(m_parent->Handle(),&m_dragger,param);
		m_CaptureBar = NULL;
	}
	else if(m_dragger.IsDragging())
	{
		int nStep;

		LPRECT lprc = m_dragger.StopDrag(m_hWnd,param.mousept.x,param.mousept.y);
		
		if(IsVert())
		{
			nStep = lprc->left - m_Curr->m_SplitRc.left;
			if(m_State == TBS_FLAG_DOCKRIGHT)
				nStep = -nStep;
			else
				CopyRect(&m_Curr->m_SplitRc,lprc);			
		}
		else
		{
			nStep = lprc->top - m_Curr->m_SplitRc.top;
			if(m_State == TBS_FLAG_DOCKBOTTOM)
				nStep = -nStep;
			else
				CopyRect(&m_Curr->m_SplitRc,lprc);			
		}

		m_Curr->m_maxSize += nStep;
		UpdateLine(m_Curr,nStep);
	}
	return 0;
}

void RingDockSite::UpdateLine(LPRINGBARLINEINFO lpline,int nStep,BOOL bUpdate/*=TRUE*/)
{
	//调整位于lpline后面的边线框位置
	LPRINGBARLINEINFO tmp;
	//RingDockBar* bar;
	int lastpos = 0,step;

	if(lpline == NULL || lpline == m_First)
	{
		tmp = m_First;
		//检测调整线是否在最上或最左位置
		//if(tmp && tmp->m_SplitRc.right != 0 && ((m_State & 1) == 1))
		//	lastpos = RING_SPLITERSIZE;
	}
	else
	{
		tmp = lpline;	
		lastpos = tmp->m_pos;
		if(tmp && tmp->m_SplitRc.right != 0 && ((m_State & 1) == 1))
			lastpos -= RING_SPLITERSIZE;
	}
	
	while(tmp)
	{
		step = lastpos - tmp->m_pos;
		tmp->m_pos = lastpos;
		
		if(tmp->m_SplitRc.right != 0)
		{
			//有分隔线且为下或右方停靠
			if((m_State & 0x1) == 1)// && tmp != m_First)
			{
				step += RING_SPLITERSIZE;
				tmp->m_pos += RING_SPLITERSIZE;
			}
			lastpos += RING_SPLITERSIZE;
			if(IsVert())
				OffsetRect(&tmp->m_SplitRc,step,0);
			else
				OffsetRect(&tmp->m_SplitRc,0,step);
		}
		
		lastpos += tmp->m_maxSize;
		tmp = tmp->m_nextline;				
	}
	UpdatePos(nStep,bUpdate);
}

LRESULT RingDockSite::OnRButtonDown(RINGPARAMS& param)
{
	RingMenu* rm = GetDockSiteMenu();
	if(rm)
		rm->Popup(m_hWnd);
	return 0;
}

//刷新背景及刷新所有停靠窗口
void RingDockSite::UpdateBkg()
{
	LPRINGBARLINEINFO tmp = m_First;
	RingDockBar* bar;
	
	InvalidateRect(m_hWnd,NULL,TRUE);

	while(tmp)
	{
		if(tmp->isVisible)
		{
			bar = tmp->m_first;
			while(bar)
			{
				if(bar->IsVisible() && bar->GetChild())
					InvalidateRect(bar->GetChild()->Handle(),NULL,TRUE);
				bar = bar->m_next;
			}
		}
		tmp = tmp->m_nextline;
	}
}

void RingDockSite::DrawSite(HDC hDC,RINGPARAMS& param)
{
	LPRINGBARLINEINFO tmp = m_First;
	RingDockBar* bar;
	
	//if(param.uMsg == WM_ERASEBKGND && !DrawBkg(hDC))
	//	DefaultProc(param);
	DrawBkg(hDC);

	if(m_State == TBS_FLAG_DOCKTOP && MASK_MATCH(m_parent->GetStyle(),WS_CAPTION) && 
		tmp && tmp->isVisible)
	{
		//非底部且父窗口有标题栏，绘制最上一条横线
		RECT rc = {0,0,m_rcPos.right,2};
		DrawEdge(hDC,&rc,BDR_SUNKENOUTER,BF_TOP);
		DrawEdge(hDC,&rc,BDR_SUNKENOUTER,BF_BOTTOM);
	}

	while(tmp)
	{
		bar = tmp->m_first;
		while(bar)
		{
			//ExcludeClipRect(param.hdc,bar->m_rcClient.left,bar->m_rcClient.top,
			//	bar->m_rcClient.right,bar->m_rcClient.bottom);
			if(!bar->Draw(hDC))
			{
				OnDockBarHide(bar);
				bar->Show(bar->IsVisible()?SW_SHOW:SW_HIDE);
				return;
			}
			bar = bar->m_next;
		}
		if(tmp->isVisible && tmp->m_SplitRc.right != 0)
		{
			if(IsVert())
				tmp->m_SplitRc.bottom = tmp->m_SplitRc.top + m_rcPos.bottom - m_rcPos.top;
			else
				tmp->m_SplitRc.right = tmp->m_SplitRc.left + m_rcPos.right - m_rcPos.left;

			DrawEdge(hDC,&tmp->m_SplitRc,BDR_RAISEDINNER,BF_RECT);
			DrawEdge(hDC,&tmp->m_SplitRc,BDR_RAISEDOUTER,BF_BOTTOMRIGHT);
		}
		tmp = tmp->m_nextline;
	}
	/*if(IsVert())
	{
		RECT rc;
		GetClientRect(&rc);
		if(rc.right > 1)
			DrawEdge(hDC,&rc,BDR_SUNKENOUTER,BF_TOP);
	}
	*/
}

LRESULT RingDockSite::RingdowProc(HWND hWnd,RINGPARAMS param)
{
	switch(param.uMsg)
   {
		case WM_CREATE:
			SetPos(m_rcPos.left,m_rcPos.top);
			SetCursor(m_windowInfo.hCursor);			
			break;
		case WM_COMMAND:			
			if(param.lParam != 0)
				return SendMessage(m_parent->Handle(),param.uMsg,param.wParam,param.lParam);
			else
				return OnDockBarHide(param.wParam);
		case RB_SETBKCOLOR:
			switch(param.wParam)
			{
				case RBIMG_HBITMAP:
					SetBgBmp((HBITMAP)param.lParam);
					return TRUE;
				case RBIMG_IDBMP:
					SetBgBmp((LPCTSTR)param.lParam);
					return TRUE;
				case RBIMG_BKCOLOR:
					SetBgColor(param.lParam);
					return TRUE;
				default:
					return 0;
			}
		case WM_MOVE:
		case WM_SIZE:
			InvalidateRect(m_hWnd,NULL,TRUE);
			return DefaultProc(param);
		case WM_LBUTTONDBLCLK:
			return OnLButtonDbClk(param);
		case WM_LBUTTONDOWN:
			return OnLButtonDown(param);
		case WM_LBUTTONUP:
			return OnLButtonUp(param);
		case WM_MOUSEMOVE:
			return OnMouseMove(param);
		case WM_RBUTTONDOWN:
			return OnRButtonDown(param);
		case WM_NCPAINT:
			return 0;		
		case WM_ERASEBKGND:
			return TRUE;
		case WM_PAINT:
		{
			PAINTSTRUCT ps;
			DrawSite(BeginPaint(m_hWnd,&ps),param);
			EndPaint(m_hWnd,&ps);
			return 0;
      }
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
				return SendMessage(m_parent->Handle(),param);
			return DefaultProc(param);
	   case WM_CLOSE:
      	Show(SW_HIDE);
         break;
		//case WM_WINDOWPOSCHANGED:
		//	InvalidateRect(m_hWnd,NULL,FALSE);
		//	return 0;
		case WM_MEASUREITEM:
		{
			RingCoolMenu* rm = GetDockMenu();
			if(rm)
				rm->MeasureItem(param.lpmeasureitem);				
			return SendMessage(m_parent->Handle(),param.uMsg,param.wParam,param.lParam);
		}
      case WM_DRAWITEM:
		{
			RingCoolMenu* rm = GetDockMenu();
			if(rm)
				rm->Draw(param.lpdrawitem);			
			return SendMessage(m_parent->Handle(),param.uMsg,param.wParam,param.lParam);
		}
		//case WM_EXITSIZEMOVE:
		//	InvalidateRect(m_hWnd,NULL,TRUE);
		//	break;
		default:
		{	
			LRESULT res = CtlColorProc(hWnd,param);
			if(res != RM_NOTPROCESS)
				return res;
			else
				return RingWnd::DefaultProc(param);
		}
   }
   return 0;
}

BOOL RingDockSite::DrawBkg(HDC hDC,LPRECT lprc/*=NULL*/,LPSIZE lpoffset/*=NULL*/)
{
	RECT rc,lrc={0,0,0,0};
	SIZE offsz={0,0};
	SIZE offsw={0,0};
	
	if(lprc == NULL)
	{
		GetClientRect(&rc);
		lprc = &rc;
	}

	if(lpoffset == NULL)
		lpoffset = &offsz;
	
	if(m_Background.hBrush)
	{
		if(m_Background.bSetOrg)
		{
			if(m_Background.hStandWnd != (HWND)0xFFFFFFFF)
			{
				//计算背景相对偏移位置
				GetWindowRect(m_hWnd,&lrc);
				MapWindowPoints(GetDesktopWindow(),m_Background.hStandWnd,(LPPOINT)&lrc,2);				
			}
			SetBrushOrgEx(hDC,m_Background.cx - m_rcPos.left-lpoffset->cx-lrc.left,
								m_Background.cy - m_rcPos.top-lpoffset->cy-lrc.top,NULL);
		}
/*		int nDC = SaveDC(hDC);
		LPRINGBARLINEINFO tmp = m_First;
		RingDockBar* bar;
		
		while(tmp)
		{
			bar = tmp->m_first;
			while(bar)
			{
				ExcludeClipRect(hDC,bar->m_rcClient.left,bar->m_rcClient.top,
										bar->m_rcClient.right,bar->m_rcClient.bottom);
				bar = bar->m_next;
			}
			tmp = tmp->m_nextline;
		}
*/
		FillRect(hDC,lprc,m_Background.hBrush);
//		RestoreDC(hDC,nDC);
		return TRUE;
	}
	else
		return FALSE;
}

//处理父窗口发来的消息，注意：hWnd是父窗口的HWND
//这里有一些特别处理，必须上下的Docksite改变尺寸
//后才能改变左右的尺寸，由RingWnd确定更新顺序。
BOOL RingDockSite::OnParentNotify(HWND hWnd,UINT uMsg,UINT wParam,LONG lParam)
{
	switch(uMsg)
   {
		case WM_DOCKSITESIZECHANGE:	//有DockSite尺寸变动，需要重新调整尺寸
		case WM_SIZE:						//父窗口改变尺寸
			if(wParam != SIZE_MINIMIZED)
				return ParentReSize(LOWORD(lParam),HIWORD(lParam));
			break;
		case WM_MOVE:
		{	
			RingDockBar* dockbar = m_flybar;
			while(dockbar)
			{
				dockbar->UpdateFlyBkg();
				dockbar = dockbar->m_next;
			}
		}
	}
	return FALSE;
}

BOOL RingDockSite::ParentReSize(int width,int height)
{	
	int nChk;

	if(IsVert())
	{		
		RingDockSite* tmp = ((RingWnd*)m_parent)->GetDockSite(TBS_FLAG_DOCKTOP);
		if(tmp)
			m_rcPos.top = tmp->m_rcPos.bottom;
		else
			m_rcPos.top = 0;
		
		tmp = ((RingWnd*)m_parent)->GetDockSite(TBS_FLAG_DOCKBOTTOM);
		if(tmp)
			m_rcPos.bottom = tmp->m_rcPos.top;
		else
			m_rcPos.bottom = height;
		
		if(m_State == TBS_FLAG_DOCKRIGHT)
		{
			m_rcPos.left = width - (m_rcPos.right - m_rcPos.left);
			m_rcPos.right = width;
		}
		//必须先调整尺寸，否则整行工具栏仍采用原尺寸为最大尺寸
		nChk = MAKELONG(m_rcPos.left,m_rcPos.right);
		MoveWindow(m_hWnd,m_rcPos.left,m_rcPos.top,m_rcPos.right-m_rcPos.left,m_rcPos.bottom-m_rcPos.top,TRUE);
		
		AdjustChildrenV(m_rcPos.bottom-m_rcPos.top);

		//检查AdjustChildrenH是否调整了m_rcPos
		nChk = (nChk == MAKELONG(m_rcPos.left,m_rcPos.right));
	}
	else
	{
		m_rcPos.right = width;
		if(m_State == TBS_FLAG_DOCKBOTTOM)
		{
			m_rcPos.top = height - (m_rcPos.bottom - m_rcPos.top);
			m_rcPos.bottom = height;
		}
		nChk = MAKELONG(m_rcPos.top,m_rcPos.bottom);

		//必须先调整尺寸，否则整行工具栏仍采用原尺寸为最大尺寸
		MoveWindow(m_hWnd,m_rcPos.left,m_rcPos.top,m_rcPos.right-m_rcPos.left,m_rcPos.bottom-m_rcPos.top,TRUE);
		AdjustChildrenH(m_rcPos.right-m_rcPos.left);
		
		//检查AdjustChildrenH是否调整了m_rcPos
		nChk = (nChk == MAKELONG(m_rcPos.top,m_rcPos.bottom));
	}
	if(nChk == 0)
		MoveWindow(m_hWnd,m_rcPos.left,m_rcPos.top,m_rcPos.right-m_rcPos.left,m_rcPos.bottom-m_rcPos.top,TRUE);
	
	return TRUE;
}

void RingDockSite::AdjustChildrenH(int nEdge)
{
	LPRINGBARLINEINFO lpbarinfo = m_First,saveline;
	HDWP hdwp = BeginDeferWindowPos(10);
	int height = 0;

	if(m_State == TBS_FLAG_DOCKTOP)
	{
		height = 1;
		saveline = NULL;
	}
	else
		saveline = lpbarinfo;

	while(lpbarinfo)
	{
		if(saveline)
		{
			if(IsSizeBarInLine(saveline) && saveline->isVisible)
			{
				OffsetRect(&saveline->m_SplitRc,0,height - saveline->m_SplitRc.top);
				height += m_SplitterSize;
			}
			saveline = saveline->m_nextline;
		}
		else
			saveline = lpbarinfo;

		lpbarinfo->m_pos = height;
		hdwp = BarLineResizeH(hdwp,lpbarinfo,nEdge);//AdjustBarLineH(hdwp,nEdge,lpbarinfo,NULL);		
		if(lpbarinfo->isVisible)
			height += lpbarinfo->m_maxSize;
		lpbarinfo = lpbarinfo->m_nextline;
	}
	if(IsSizeBarInLine(saveline) && saveline->isVisible)
	{
		OffsetRect(&saveline->m_SplitRc,0,height - saveline->m_SplitRc.top);
		height += m_SplitterSize;
	}
	
	if(hdwp)
		EndDeferWindowPos(hdwp);

	if(m_State == TBS_FLAG_DOCKTOP)
		m_rcPos.bottom = m_rcPos.top + height;
	else
		m_rcPos.top = m_rcPos.bottom - height;
}

void RingDockSite::AdjustChildrenV(int nEdge)
{
	LPRINGBARLINEINFO lpbarinfo = m_First,saveline;
	HDWP hdwp = BeginDeferWindowPos(10);
	int width = 0;
	
	if(m_State == TBS_FLAG_DOCKLEFT)
		saveline = NULL;
	else
		saveline = lpbarinfo;
	
	while(lpbarinfo)
	{
		if(saveline)
		{
			if(IsSizeBarInLine(saveline) && saveline->isVisible)
			{
				OffsetRect(&saveline->m_SplitRc,width - saveline->m_SplitRc.left,0);
				width += m_SplitterSize;
			}
			saveline = saveline->m_nextline;
		}
		else
			saveline = lpbarinfo;
		
		lpbarinfo->m_pos = width;
		hdwp = BarLineResizeV(hdwp,lpbarinfo,nEdge);//AdjustBarLineH(hdwp,nEdge,lpbarinfo,NULL);		
		if(lpbarinfo->isVisible)
			width += lpbarinfo->m_maxSize;
		lpbarinfo = lpbarinfo->m_nextline;
	}
	if(IsSizeBarInLine(saveline) && saveline->isVisible)
	{
		OffsetRect(&saveline->m_SplitRc,width - saveline->m_SplitRc.left,0);
		width += m_SplitterSize;
	}
	if(hdwp)
		EndDeferWindowPos(hdwp);
	m_rcPos.right = m_rcPos.left + width;
}

//窗口尺寸改变，需要调整子窗口位置
//本函数应该仅由OnParentNotify调用。主窗口改变尺寸调用OnParentNotify
//改变本窗口尺寸。本窗口尺寸改变后调用本函数调整子窗口位置。
//本窗口只有两种情况改变尺寸：
//父窗口改变尺寸，通过OnParentNotify通知;有SIZEBAR，用户鼠标
//拖动改变尺寸，这时通过鼠标按键放开的消息调用UpdatePos，
//UpdatePos通过向父窗口发送WM_SIZE消息触发OnParentNotify，
//这样可以调整所有CtrlBar。
void RingDockSite::NotifyChild(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
{	
	RingBaseWnd::NotifyChild(hWnd,uMsg,wParam,lParam);
/*	LPRINGBARLINEINFO tmp = m_First;
	RingDockBar* dockbar;
	while(tmp)
	{
		dockbar = tmp->m_first;
		while(dockbar)
		{
			dockbar->OnParentNotify(hWnd,uMsg,wParam,lParam);
			dockbar = dockbar->m_next;
		}
		tmp = tmp->m_nextline;
	}
*/
}

//调整窗口尺寸，通过向父窗口发送WM_SIZE消息触发OnParentNotify，
//这样可以调整所有CtrlBar。
void RingDockSite::UpdatePos(int nAddSize,BOOL bUpdate/*=TRUE*/)
{
	switch(m_State)
	{
	case TBS_FLAG_DOCKBOTTOM:
		m_rcPos.top -= nAddSize;
		break;
	case TBS_FLAG_DOCKTOP:
		m_rcPos.bottom += nAddSize;
		break;
	case TBS_FLAG_DOCKRIGHT:
		m_rcPos.left -= nAddSize;
		break;		
	case TBS_FLAG_DOCKLEFT:
		m_rcPos.right += nAddSize;
		break;
	}
	//如未调整尺寸，则不会刷新，需要手工刷新一下
	InvalidateRect(m_hWnd,NULL,TRUE);
	if(bUpdate)
		UpdateAllSite();
}

void RingDockSite::UpdateAllSite()
{
	((RingWnd*)m_parent)->UpdateDockSite();
}

/**************************************************
//
//创建并加入停靠行信息
//	参数：
//		prev:前一行信息
//
**************************************************/
LPRINGBARLINEINFO RingDockSite::AddLine(LPRINGBARLINEINFO prev)
{
	LPRINGBARLINEINFO addline = GetEmptyLine();
	if(addline)
	{
		addline->isVisible = TRUE;
		addline->m_maxSize = 0;
		addline->m_first = NULL;
		SetRectEmpty(&addline->m_SplitRc);

		if(prev == NULL)
		{
			//加到最前
			prev = m_First;
			m_First = addline;
			addline->m_nextline = prev;
			addline->m_pos = 0;
		}
		else
		{
			addline->m_nextline = prev->m_nextline;
			prev->m_nextline = addline;
			addline->m_pos = prev->m_pos + prev->m_maxSize;
			if(prev->m_SplitRc.right != 0 &&	(m_State & 1) == 0)			
				addline->m_pos += m_SplitterSize;
		}
	}
	return addline;
}

BOOL RingDockSite::UpdateSysMenu(RingDockBar* dockbar,LPCTSTR szText)
{
	if(szText == NULL || *szText == '\0')
		return FALSE;

	BOOL bOK = FALSE;

	if(m_pInterMenu == NULL)
		m_pInterMenu = GetDockSiteMenu();
	if(m_pInterMenu && !dockbar->IsMenuBar())
	{
		bOK = m_pInterMenu->SetItemText(dockbar->GetId(),szText);
		if(!bOK)
			bOK = m_pInterMenu->Insert(dockbar->GetId(),m_pInterMenu->GetItemCount(),(LPSTR)szText,BY_POSITION);

		m_pInterMenu->CheckItem(dockbar->GetId(),dockbar->IsVisible());
	}
	return bOK;
}

void RingDockSite::RemoveSysMenu(RingDockBar* dockbar)
{
	if(m_pInterMenu && dockbar && !dockbar->IsMenuBar())
		m_pInterMenu->Delete(dockbar->GetId());
}

//响应菜单消息，隐藏/显示工具栏
BOOL RingDockSite::OnDockBarHide(UINT uId)
{
	if(m_pInterMenu)
	{
		UINT uState = m_pInterMenu->GetItemCheckState(uId);
		if(uState != MF_ERRORS)
		{
			m_pInterMenu->CheckItem(uId,(uState==MF_UNCHECKED));
			RingDockBar* dockbar = FindDockBar(uId);
			if(dockbar)
				dockbar->Show((uState==MF_UNCHECKED)?SW_SHOW:SW_HIDE);
			return TRUE;
		}
	}
	return FALSE;
}

void RingDockSite::OnDockBarHide(RingDockBar* dockbar)
{
	if(m_pInterMenu && dockbar)
		m_pInterMenu->CheckItem(dockbar->GetId(),dockbar->IsVisible()?MF_CHECKED:MF_UNCHECKED);
}

RingDockBar* RingDockSite::GetDockBar(RingBaseWnd* wnd)
{
	if(wnd == NULL)
		return NULL;

	LPRINGBARLINEINFO tmp = m_First;
	RingDockBar* dockbar;

	while(tmp)
	{
		dockbar = tmp->m_first;
		while(dockbar)
			if(dockbar->GetChild() == wnd)
				return dockbar;
			else
				dockbar = dockbar->m_next;
		tmp = tmp->m_nextline;
	}
	return NULL;
}

RingDockBar* RingDockSite::GetDockBar(UINT uDockBarId)
{
	LPRINGBARLINEINFO tmp = m_First;
	RingDockBar* dockbar;
	
	while(tmp)
	{
		dockbar = tmp->m_first;
		while(dockbar)
			if(dockbar->GetId() == uDockBarId)
				return dockbar;
			else
				dockbar = dockbar->m_next;
			tmp = tmp->m_nextline;
	}
	return NULL;
}

RingDockBar* RingDockSite::FindDockBar(UINT uDockBarId)
{
	RingDockSite *site;
	RingDockBar *dockbar = NULL;
	for(int i=0;i<4;i++)
	{
		site = ((RingWnd*)m_parent)->GetDockSite(i);
		if(site)
		{
			dockbar = site->GetDockBar(uDockBarId);
			if(dockbar)
				return dockbar;
		}
	}
	dockbar = m_flybar;
	while(dockbar)
	{
		if(dockbar->GetId() == uDockBarId)
			return dockbar;
		else
			dockbar = dockbar->m_next;
	}
	return NULL;
}

//创建或获取DockBar
RingDockBar* RingDockSite::CreateDockBar(RingBaseWnd* wnd,BOOL bCanSize)
{
	if(wnd == NULL)
		return NULL;
	
	RingDockBar* dockbar = NULL;

	//判断是否已有dockbar与wnd绑定，是为防止多次调用AddBar
	if(wnd->GetParent() == m_hWnd)
		dockbar = GetDockBar(wnd);
	else
	{
		//DockBar内的窗口其父窗口句柄为DockSite，但m_parent为原父窗口对象
		RingBaseWnd* parent = GetWindowPtr(wnd->GetParent());
		if(parent)
		{			
			if(parent->GetWindowType() == ITIS_DOCKSITE)	//已加入，但非停靠在本区域
				dockbar = ((RingDockSite*)parent)->GetDockBar(wnd);
			else if(parent->GetWindowType() == ITIS_FLYBAR)
				dockbar = ((RingFlyBar*)parent)->GetDockBar();
		}
	}
	
	if(dockbar == NULL)
	{
		//创建dockbar
		if(bCanSize)
			dockbar = new RingSizeBar;
		else
		{
			if(wnd->GetWindowType() == ITIS_MENUBAR)
				dockbar = new RingAllLineBar;
			else
				dockbar = new RingDockBar;
		}
	}
	return dockbar;
}

BOOL RingDockSite::ReleaseDockBar(RingBaseWnd* wnd)
{
	RingDockBar* bar = GetDockBar(wnd);
	if(bar == NULL)
	{
		RingDockBar *prevbar = NULL;
		bar = m_flybar;
		
		while(bar)
		{
			if(bar->m_child && bar->m_child == wnd)
			{
				if(prevbar == NULL)
					m_flybar = bar->m_next;
				else
					prevbar->m_next = bar->m_next;
				break;
			}
			prevbar = bar;
			bar = bar->m_next;
		}
	}
	if(bar)
	{
		DelBar(bar);
		HWND hFlyWnd = wnd->GetParent();
		wnd->SetParent(wnd->Parent()->Handle());
		RemoveSysMenu(bar);
		if(bar->GetState() == TBS_FLAG_FLY)
			DestroyWindow(hFlyWnd);		 //flybar析构会销毁bar
		else
			delete bar;
		return TRUE;
	}
	return FALSE;
}

//加入停靠窗口,lprcPos为相对于父窗口坐标
RingDockBar* RingDockSite::AddBar(RingBaseWnd* wnd,LPCTSTR szTitle,
								  UINT uDockStyle/*=TBS_EX_DOCKABLE*/,
								  UINT uDockFlag/*=TBS_FLAG_DOCKTOP*/,
								  LPRECT lprcPos/*=NULL*/,
								  BOOL bCanSize/* = FALSE*/)
{
	//创建或获取dockbar
	RingDockBar* dockbar = CreateDockBar(wnd,bCanSize);
	
	if(dockbar)
	{
		dockbar->SetDockStyle(uDockStyle);
		dockbar->SetDockTitle(szTitle);
		UpdateSysMenu(dockbar,dockbar->GetDockTitle());
		
		if(dockbar->SetChild(wnd))
		{
			//dockbar为新创建（未设置停靠窗口）,更新尺寸
			dockbar->UpdateSize();
		}
		if(dockbar->GetDockSite() == NULL)
			dockbar->m_pSite = this;	//无DockSite，必须设置

		dockbar->Dockto(uDockFlag,lprcPos);
	}
	return dockbar;
}

/////////////////////////////////////////////////////////////
//
//加入工具栏,工具栏必须是本窗口的子窗口且dockbar已调用SetDockSite加入到本停靠坞
//函数自动判断该工具栏是新加入或需要调整位置
//lprc为相对于父窗口坐标
//
/////////////////////////////////////////////////////////////
BOOL RingDockSite::AddBar(RingDockBar* dockbar,LPRECT lprc)
{	
	if(dockbar && dockbar->m_pSite == this)
	{
		int m,nSize,nDel=0;
		LPRINGBARLINEINFO tmp = NULL;
		LPRINGBARLINEINFO lpline = NULL;
		BOOL bLast = FALSE;
		RECT rc;
		if(lprc)
		{
			CopyRect(&rc,lprc);
			MapWindowPoints(m_parent->Handle(),m_hWnd,(LPPOINT)&rc,2);
			lprc = &rc;

			//指定位置停靠，先根据lprc的中线判断停靠位置
			if(IsVert())
			{
				if(dockbar->IsAllLineBar())
				{
					m = lprc->left;
					lprc = NULL;
				}
				else
					m = (lprc->left + lprc->right)/2;
			}
			else
			{
				if(dockbar->IsAllLineBar())
				{
					m = lprc->top;
					lprc = NULL;
				}
				else
					m = (lprc->top + lprc->bottom)/2;
			}
		}
		else
			m = 9999;	//加到最后

		//寻找要停靠的行
		lpline = GetDockLine(m);
		LPRINGBARLINEINFO save = dockbar->m_lineInfo;
		if(save)		  //已停靠在本窗口位置
		{
			nDel = BarOutLine(dockbar,(lpline != save),tmp);
			if(tmp == NULL && lpline != save)
			{
				//要删除行
				DelLine(save);
			}			
		}
		
		nSize = AddBarInLine(dockbar,lpline,lprc);
		if(dockbar->IsSizeBar())
		{
			if(lpline->m_nSizeBarCnt == 0)
				nSize += m_SplitterSize;
			
			lpline->m_nSizeBarCnt ++;
			if(lprc == NULL)	//第一次加入
				dockbar->InitDockSize();
			SetSizeSplitRect(lpline);
		}
			
		UpdateLine(NULL,nSize + nDel);
		return TRUE;
	}
	else
		return FALSE;
}

LPRINGBARLINEINFO RingDockSite::GetDockLine(int nPos)
{
	LPRINGBARLINEINFO tmp = m_First,prev = NULL,prev2 = NULL;
	
	if(tmp == NULL || nPos <= tmp->m_pos)
		return AddLine(NULL);
	
	while(tmp)
	{
		if(tmp->isVisible)
		{
			if(tmp->m_pos + tmp->m_maxSize > nPos)
			{
				//找到停靠行，判断是否独占行
				if(tmp->m_first && tmp->m_first->IsAllLineBar())//(!tmp->m_first->CanDock(m_State) || tmp->m_first->IsAllLineBar()))
				{
					//独占行，停靠到新建行
					if(nPos <= tmp->m_pos + tmp->m_maxSize/2 || tmp->m_first->IsStatusBar())
					{
						//加到前面
						return AddLine(prev);
					}
					else
					{
						//加到后面
						return AddLine(tmp);						
					}
				}
				else
					return tmp;
			}			
		}
		prev2 = prev;
		prev = tmp;
		tmp = tmp->m_nextline;		
	}
	
	if(prev && prev->m_first && prev->m_first->IsStatusBar())
		return AddLine(prev2);
	else
		return AddLine(prev);
}

//在行内添加工具栏
int RingDockSite::AddBarInLine(RingDockBar* dockbar,LPRINGBARLINEINFO lpline,LPRECT lprc/*=NULL*/)
{	
	if(dockbar == NULL || lpline == NULL)
		return 0;
	
	RECT rc;
	if(lprc == NULL)
		lprc = &rc;
	
	dockbar->m_prev = dockbar->m_next = NULL;
	dockbar->m_lineInfo = lpline;

	if(IsVert())
	{
		rc.left = lpline->m_pos;
		rc.right = rc.left + dockbar->m_rcDockPos.right - dockbar->m_rcDockPos.left;
		
		OffsetRect(lprc,lpline->m_pos - lprc->left,0);
		if(lpline->m_first == NULL)
		{
			rc.top = 0;
			rc.bottom = dockbar->m_rcDockPos.bottom - dockbar->m_rcDockPos.top;

			lpline->m_first = dockbar;
			RingDockSite* pSite = ((RingWnd*)m_parent)->GetDockSite(TBS_FLAG_DOCKBOTTOM);
			int nEdge; 
			//工具栏有可能是从底部移过来，且底部DockSite有可能缩小高度，而此时本DockSite
			//的高度尚未变化，因此需依据底部DockSite尺寸判断
			if(pSite)
				nEdge = pSite->m_rcPos.top;
			else
				nEdge = m_rcPos.bottom;

			if(lprc->bottom > m_rcPos.bottom)
				OffsetRect(lprc,0,nEdge - lprc->bottom);
			if(lprc->top < 0)
				OffsetRect(lprc,0,-lprc->top);
			dockbar->SetDockPos(lprc->left,lprc->top);
			rc.left = lpline->m_maxSize;
			lpline->m_maxSize = dockbar->m_nSize;
			InvalidateRect(m_hWnd,NULL,TRUE);
			return lpline->m_maxSize - rc.left;
		}
		else
		{
			rc.top = 9999;
			rc.bottom = 9999 + dockbar->m_rcDockPos.bottom - dockbar->m_rcDockPos.top;
			return AddBarInLineV(dockbar,lpline,lprc,(lprc->top + lprc->bottom)/2);
		}
	}
	else
	{
		rc.top = lpline->m_pos;
		rc.bottom = rc.top + dockbar->m_rcDockPos.bottom - dockbar->m_rcDockPos.top;
				
		OffsetRect(lprc,0,lpline->m_pos - lprc->top);
		if(lpline->m_first == NULL)
		{
			rc.left = 0;
			rc.right = dockbar->m_rcDockPos.right - dockbar->m_rcDockPos.left;

			lpline->m_first = dockbar;
			if(lprc->right > m_rcPos.right - m_rcPos.left)
				OffsetRect(lprc,m_rcPos.right - m_rcPos.left - lprc->right,0);
			if(lprc->left < 0)
				OffsetRect(lprc,-lprc->left,0);
			dockbar->SetDockPos(lprc->left,lprc->top);
			rc.left = lpline->m_maxSize;
			lpline->m_maxSize = dockbar->m_nSize;
			InvalidateRect(m_hWnd,NULL,TRUE);
			return lpline->m_maxSize - rc.left;//lpline->m_maxSize;
		}
		else
		{			
			rc.left = 9999;
			rc.right = 9999 + dockbar->m_rcDockPos.right - dockbar->m_rcDockPos.left;
			return AddBarInLineH(dockbar,lpline,lprc,(lprc->left + lprc->right)/2);
		}
	}
}

//插入insbar到dockbar前面，仅插入，不作任何位置调整
void RingDockSite::InsertDockBar(LPRINGBARLINEINFO lpline,RingDockBar* insbar,RingDockBar* dockbar)
{
	insbar->m_prev = dockbar->m_prev;
	insbar->m_next = dockbar;
	dockbar->m_prev = insbar;
	if(insbar->m_prev)
		insbar->m_prev->m_next = insbar;
	else
		lpline->m_first = insbar;
}

int RingDockSite::AddBarInLineV(RingDockBar* dockbar,
										  LPRINGBARLINEINFO lpline,
										  LPRECT lprc,
										  int checkpos)
{
	RingDockBar* bar = lpline->m_first,*save = NULL;
	
	while(bar)
	{
		if(lprc->top <= bar->m_rcDockPos.top ||
			checkpos <= (bar->m_rcDockPos.top + bar->m_rcDockPos.bottom)/2)
		{
			//找到插入位置
			InsertDockBar(lpline,dockbar,bar);
			save = NULL;
			break;
		}
		else
		{
			save = bar;
			bar = bar->m_next;
		}
	}
	if(save)
	{
		//未找到，save是最后一个工具栏
		save->m_next = dockbar;
		dockbar->m_prev = save;
		dockbar->m_next = NULL;
		if(checkpos > 9999)	//紧跟最后一个工具栏
			OffsetRect(lprc,0,save->m_rcDockPos.bottom - lprc->top);
	}
	
	int nSize = dockbar->m_nSize - lpline->m_maxSize;
	lpline->m_maxSize = max(lpline->m_maxSize,dockbar->m_nSize);
	if(nSize > 0)
	{		
		if(lpline->m_SplitRc.right != 0 && m_State == TBS_FLAG_DOCKLEFT)
			OffsetRect(&lpline->m_SplitRc,nSize,0);
	}
	if(lprc->top < 0)
		OffsetRect(lprc,0,-lprc->top);
	
	dockbar->OffsetDockPos(lprc->left-dockbar->m_rcDockPos.left,lprc->top-dockbar->m_rcDockPos.top);
	
	while(dockbar->m_next)
	{
		checkpos = dockbar->m_rcDockPos.bottom - dockbar->m_next->m_rcDockPos.top;
		if(checkpos > 0)
		{
			dockbar = dockbar->m_next;
			dockbar->OffsetDockPos(0,checkpos);
		}
		else
			break;
	}
	return max(0,nSize);
}

int RingDockSite::AddBarInLineH(RingDockBar* dockbar,
										  LPRINGBARLINEINFO lpline,
										  LPRECT lprc,
										  int checkpos)
{
	RingDockBar* bar = lpline->m_first,*save = NULL;
	
	while(bar)
	{
		if(lprc->left <= bar->m_rcDockPos.left ||
			checkpos <= (bar->m_rcDockPos.left + bar->m_rcDockPos.right)/2)
		{
			//找到插入位置
			InsertDockBar(lpline,dockbar,bar);			
			save = NULL;
			break;
		}
		else
		{
			save = bar;
			bar = bar->m_next;
		}
	}
	if(save)
	{
		//未找到，save是最后一个工具栏
		save->m_next = dockbar;
		dockbar->m_prev = save;
		dockbar->m_next = NULL;
		if(checkpos > 9999)	//紧跟最后一个工具栏
			OffsetRect(lprc,save->m_rcDockPos.right - lprc->left,0);
	}
	
	int nSize = dockbar->m_nSize - lpline->m_maxSize;
	lpline->m_maxSize = max(lpline->m_maxSize,dockbar->m_nSize);
	if(nSize > 0)
	{
		//lpline->m_maxSize += nSize;
		if(lpline->m_SplitRc.right != 0 && m_State == TBS_FLAG_DOCKTOP)
			OffsetRect(&lpline->m_SplitRc,0,nSize);
	}
	if(lprc->left < 0)
		OffsetRect(lprc,-lprc->left,0);
	
	dockbar->OffsetDockPos(lprc->left-dockbar->m_rcDockPos.left,lprc->top-dockbar->m_rcDockPos.top);

	while(dockbar->m_next)
	{
		checkpos = dockbar->m_rcDockPos.right - dockbar->m_next->m_rcDockPos.left;
		if(checkpos > 0)
		{
			dockbar = dockbar->m_next;
			dockbar->OffsetDockPos(checkpos,0);
		}
		else
			break;
	}
	//AdjustBarLineH(NULL,m_winpos.cx,lpline,dockbar,TRUE);
	return max(0,nSize);
}

//调整行内工具栏位置,dockbar如果非空，则表示优先满足该工具栏的位置
//nSize:DockSite的宽度
//int RingDockSite::AdjustBarLineH(HDWP hdwp,int nSize,LPRINGBARLINEINFO lpLine,
//											RingDockBar* dockbar,BOOL bCanWrap/*=FALSE*/)
/*
{
	if(lpLine && lpLine->m_first)
	{
		RingDockBar *bar = lpLine->m_first,*save = bar;
		int nSpace = 0,nPreSpace = 0;
		int nVSize = 0;
		int nTmp;
		
		if(dockbar == NULL)
			nTmp = nSize - ClientWidth();
		else
		{
			if(dockbar->IsSizeBar())
				nTmp = 0;
			else
				nTmp = -dockbar->m_winpos.cx;
		}

		int &nAddSpace = nTmp;

		while(bar && bar != dockbar)
		{
			save = bar;
			bar->AdjustChildH(nAddSpace);
			nPreSpace += bar->GetPreSpace(nAddSpace);
			nVSize = max(nVSize,bar->m_nSize);
			bar = bar->m_next;
		}
		if(dockbar == NULL)
		{
			//dockbar为NULL表示是调整父窗口尺寸时调用本函数，置为最后的工具栏
			dockbar = save;
			nSpace = nPreSpace;
		}
		else
			nSpace = nPreSpace + dockbar->GetPreSpace(nAddSpace);

		if(nSpace < 0)
		{
			//空间不够，BAR需要后移
			dockbar->OffsetDockPos(-nPreSpace,0);
			nPreSpace = 0;
			if(dockbar->m_winpos.x >= m_winpos.cx && bCanWrap)
			{
				nSpace = lpLine->m_maxSize;				
				lpLine->m_maxSize = nVSize;
				return nVSize - nSpace + WrapLine(dockbar);
			}
			if(dockbar->m_winpos.right >= m_winpos.cx && bCanWrap && dockbar->m_next)
			{
				//无空间，后面的bar必须另起一行，WrapLine后dockbar->m_next为NULL
				nSpace = lpLine->m_maxSize;
				nVSize = max(nVSize,dockbar->m_nSize);
				lpLine->m_maxSize = nVSize;
				return nVSize - nSpace + WrapLine(dockbar->m_next);
			}
		}
		else
			nPreSpace = nSpace;
		
		//前面部分判断完毕
		nVSize = max(nVSize,dockbar->m_nSize);
		save = dockbar;
		bar = dockbar->m_next;
		while(bar)
		{
			bar->AdjustChildH(nAddSpace);
			nSpace = bar->GetPreSpace(nAddSpace);
			nVSize = max(nVSize,bar->m_nSize);
			if(nSpace > 0)
			{
				//后面空间足够，到此为止
				bar = save;
				break;
			}
			else
			{
				//与前面的BAR重叠，需要后移
				bar->OffsetDockPos(-nSpace,0);
				save = bar;
				bar = bar->m_next;
			}
		}
		if(bar == NULL)
			bar = save;
/*
		if(hdwp == NULL)
		{
			hdwp = BeginDeferWindowPos(10);
			bar->AdjustDockPosH(hdwp,m_winpos.right,nPreSpace,FALSE);
			EndDeferWindowPos(hdwp);
			InvalidateRect(m_hWnd,NULL,TRUE);
		}
		else
			bar->AdjustDockPosH(hdwp,m_winpos.right,nPreSpace,FALSE);
*/		
/*		nSpace = lpLine->m_maxSize;
		lpLine->m_maxSize = nVSize;
		return nVSize - nSpace;
	}
	return 0;
}
//*/
//工具栏一行中排不下时折行，bar是需要折行的第一个工具栏
int RingDockSite::WrapLine(RingDockBar* bar)
{
	if(bar)
	{		
		int *pos,x=0,y=0;
		int nSize = 0,nExtra = 0;
		LPRINGBARLINEINFO lpline = AddLine(bar->m_lineInfo);

		if(bar->m_prev)
			bar->m_prev->m_next = NULL;
		bar->m_prev = NULL;
		lpline->m_first = bar;
		
		if(IsVert())
		{
			x = lpline->m_pos;
			pos = &y;
		}
		else
		{
			y = lpline->m_pos;
			pos = &x;
		}

		while(bar)
		{
			bar->m_lineInfo = lpline;
			bar->SetDockPos(x,y);
			
			if(bar->IsVisible())
			{
				lpline->m_maxSize = max(lpline->m_maxSize,bar->m_nSize);
				if(IsVert())
					*pos += (bar->m_rcDockPos.bottom - bar->m_rcDockPos.top);
				else
					*pos += (bar->m_rcDockPos.right - bar->m_rcDockPos.left);
			}
			bar = bar->m_next;
		}
		return lpline->m_maxSize;
	}
	return 0;
}

//删除工具栏
BOOL RingDockSite::DelBar(RingDockBar* dockbar,BOOL bUpdate/*=TRUE*/)
{
	if(dockbar && dockbar->GetChild() && dockbar->GetChild()->GetParent() == m_hWnd)
	{
		LPRINGBARLINEINFO line = NULL,save = dockbar->m_lineInfo;
		int nSize = BarOutLine(dockbar,TRUE,line);
				
		//line作为引用,由BarOutLine设置
		if(line == NULL)
			DelLine(save);

		UpdateLine(line,nSize,bUpdate);
		return TRUE;
	}
	return FALSE;
}

BOOL RingDockSite::HideBar(RingDockBar* dockbar)
{
	if(dockbar == NULL)
		return FALSE;

	OnDockBarHide(dockbar);
	RingDockBar* bar = dockbar->m_lineInfo->m_first;
	
	while(bar)
	{
		if(bar->IsVisible())
			return TRUE;
		bar = bar->m_next;
	}
	dockbar->m_lineInfo->isVisible = FALSE;

	int step = -dockbar->m_lineInfo->m_maxSize;

	UpdateLine(dockbar->m_lineInfo,-dockbar->m_lineInfo->m_maxSize);
	return TRUE;
}

//得到dockbar所在行的前一行。
//nPrevSize:前面行的总宽/高度。
LPRINGBARLINEINFO RingDockSite::GetPrevLine(LPRINGBARLINEINFO lpline)
{
	if(m_First == lpline)
		return NULL;

	LPRINGBARLINEINFO tmp = m_First,prev = tmp;
	
	if(tmp)
		tmp = tmp->m_nextline;

	while(tmp)
	{
		if(tmp->isVisible)
		{
			if(tmp == lpline)
				return prev;
		}
		prev = tmp;
		tmp = tmp->m_nextline;
	}
	return prev;
}

//得到dockbar停靠行，仅在拖动工具栏判断使用。
//nPos:dockbar的中线位置，bLast:是否加到最后。
//返回：NULL-加到最前，非空-要加入的行，bLast=TRUE-加到最后，返回值为最后一行
/*
LPRINGBARLINEINFO RingDockSite::GetDockLine(int nPos,BOOL &bLast)
{
	LPRINGBARLINEINFO tmp = m_First,prev = NULL,prev2 = NULL;
	bLast = FALSE;

	if(tmp == NULL || nPos <= tmp->m_pos)
		return NULL;
	
	while(tmp)
	{
		if(tmp->isVisible)
		{
			if(tmp->m_pos + tmp->m_maxSize > nPos)
			{
				if(tmp->m_first && tmp->m_first->IsAllLineBar())//(!tmp->m_first->CanDock(m_State) || tmp->m_first->IsAllLineBar()))
				{
					//独占一行，停靠到新建行
					if(nPos <= tmp->m_pos + tmp->m_maxSize/2 || tmp->m_first->IsStatusBar())
					{
						//加到前面
						if(prev)
						{
							tmp = GetEmptyLine();
							tmp->m_nextline = prev->m_nextline;
							tmp->isVisible = TRUE;
							prev->m_nextline = tmp;
							return tmp;
						}
						else
							return NULL;						
					}
					else
					{
						//加到后面
						prev = GetEmptyLine();
						prev->m_nextline = tmp->m_nextline;
						prev->isVisible = TRUE;
						tmp->m_nextline = prev;
						return prev;						
					}
				}
				else
					return tmp;
			}			
		}
		prev2 = prev;
		prev = tmp;
		tmp = tmp->m_nextline;		
	}
	
	if(prev && prev->m_first && prev->m_first->IsStatusBar())
	{
		if(prev2)
		{
			tmp = GetEmptyLine();
			prev2->m_nextline = tmp;
			tmp->m_nextline = prev;
			tmp->isVisible = TRUE;
			return tmp;
		}
		else
			return NULL;
	}
	else
	{
		bLast = TRUE;
		return prev;
	}
}
*/

//删除工具行
BOOL RingDockSite::DelLine(LPRINGBARLINEINFO lpline)
{
	if(lpline)
	{
		LinkLine(GetPrevLine(lpline),lpline->m_nextline);
		SetLineEmpty(lpline);
		return TRUE;
	}	
	return FALSE;
}

//工具栏从停靠行脱离，调用本函数须确保dockbar有效
int RingDockSite::BarOutLine(RingDockBar* dockbar,BOOL bCalcSize,LPRINGBARLINEINFO& line)
{	
	int nMax=0,nExtra=0;
	RingDockBar* bar = dockbar->m_lineInfo->m_first;
	
	if(bCalcSize)
	{
		//计算除要删除的工具栏外行的总高/宽度
		while(bar)
		{
			if(bar != dockbar)
			{
				if(bar->IsVisible())
					nMax = max(nMax,bar->m_nSize);
			}			
			bar = bar->m_next;
		}
	}
	else
		nMax = dockbar->m_lineInfo->m_maxSize;
	
	line = dockbar->m_lineInfo;
	//连接前后工具栏
	if(dockbar->m_next)
		dockbar->m_next->m_prev = dockbar->m_prev;	
	if(dockbar->m_prev)
		dockbar->m_prev->m_next = dockbar->m_next;
	else
		line->m_first = dockbar->m_next;
	//设置行的总高/宽度
	line->m_maxSize = nMax;
	if(dockbar->IsSizeBar())
	{
		line->m_nSizeBarCnt --;
		if(line->m_nSizeBarCnt == 0)
		{
			nExtra = m_SplitterSize;
			//已无SizeBar,清空调整线信息
			if(line->m_SplitRc.right != 0)
				SetRectEmpty(&line->m_SplitRc);
		}
	}

	//清空工具栏连接信息
	dockbar->m_lineInfo = NULL;
	dockbar->m_prev = dockbar->m_next = NULL;
		
	if(line->m_first == NULL)	//行内没有工具栏了，返回NULL
		line = NULL;	
	
	if(line && nMax == 0)	//其他工具栏是隐藏的
		line->isVisible = FALSE;
	
	return min(0,nMax - dockbar->m_nSize - nExtra);
}

//连接两个barline，nPos:prev==NULL时根据nPos查找前一个barline
void RingDockSite::LinkLine(LPRINGBARLINEINFO prev,LPRINGBARLINEINFO next)
{
	if(prev)				
		prev->m_nextline = next;
	else
		m_First = next;
}

BOOL RingDockSite::SetSizeSplitRect(LPRINGBARLINEINFO line)
{
	if(line)
	{
		switch(m_State)
		{
			case TBS_FLAG_DOCKTOP:
				SetRect(&line->m_SplitRc,0,line->m_pos + line->m_maxSize,m_rcPos.right - m_rcPos.left,
							line->m_pos + line->m_maxSize + RING_SPLITERSIZE);
				break;
			case TBS_FLAG_DOCKBOTTOM:
				SetRect(&line->m_SplitRc,0,line->m_pos,m_rcPos.right - m_rcPos.left,line->m_pos + RING_SPLITERSIZE);
				break;
			case TBS_FLAG_DOCKLEFT:
				SetRect(&line->m_SplitRc,line->m_pos + line->m_maxSize,0,
					line->m_pos + line->m_maxSize + RING_SPLITERSIZE,m_rcPos.bottom - m_rcPos.top);
				break;
			case TBS_FLAG_DOCKRIGHT:
				SetRect(&line->m_SplitRc,line->m_pos,0,line->m_pos + RING_SPLITERSIZE,m_rcPos.bottom - m_rcPos.top);
				break;
		}
		return TRUE;
	}
	return FALSE;
}

HDWP RingDockSite::BarLineResizeH(HDWP hdwp,LPRINGBARLINEINFO lpLine,int nWidth)
{
	if(lpLine && lpLine->m_first)
	{
		RingDockBar *bar = lpLine->m_first,*save = NULL;
		int nPreSpace = 0;
		int nFirstStep=0,nStep=0,nAddSpace = nWidth - Width();

		if(lpLine->m_nSizeBarCnt > 0)
		{
			nStep = nAddSpace / lpLine->m_nSizeBarCnt;
			nFirstStep = nStep + (nAddSpace % lpLine->m_nSizeBarCnt);
		}
		else
			nFirstStep = nStep = nAddSpace;

		while(bar)
		{
			if(bar->IsVisible())
				save = bar;

			bar->AdjustChildH(nFirstStep);
			nPreSpace += bar->GetPreSpace();
			bar = bar->m_next;
			nFirstStep = nStep;
		}
		if(save)
			hdwp = save->AdjustDockPosH(hdwp,m_rcPos.right,nPreSpace,FALSE);
		else
			lpLine->isVisible = FALSE;
	}
	return hdwp;
}

HDWP RingDockSite::BarLineResizeV(HDWP hdwp,LPRINGBARLINEINFO lpLine,int nHeight)
{
	if(lpLine && lpLine->m_first)
	{
		RingDockBar *bar = lpLine->m_first,*save = NULL;
		int nPreSpace = 0;
		int nFirstStep=0,nStep=0,nAddSpace = nHeight - Height();
		
		if(lpLine->m_nSizeBarCnt > 0)
		{
			nStep = nAddSpace / lpLine->m_nSizeBarCnt;
			nFirstStep = nStep + (nAddSpace % lpLine->m_nSizeBarCnt);
		}
		else
			nFirstStep = nStep = nAddSpace;

		while(bar)
		{
			if(bar->IsVisible())
				save = bar;
			bar->AdjustChildV(nFirstStep);
			nPreSpace += bar->GetPreSpace();
			bar = bar->m_next;
			nFirstStep = nStep;
		}
		if(save)
			hdwp = save->AdjustDockPosV(hdwp,nHeight/*m_winpos.bottom*/,nPreSpace,FALSE);
		else
			lpLine->isVisible = FALSE;
	}
	return hdwp;
}
/*
BOOL RingDockSite::NeedDrawDivider(RingDockBar* dockbar)
{
	if(m_State == TBS_FLAG_DOCKTOP && m_First && m_First->m_first == dockbar)
		return TRUE;
	else
		return FALSE;
}

/*
int RingDockSite::AdjustBarLineV(RingDockBar* bar,int nSpace,RingDockBar* dockbar,int& nSize)
{
	if(bar)
	{
		int nAdd = 0,space = 0;
		
		nSize = max(nSize,bar->m_nSize);

		if(bar->m_prev)
		{
			//前面有工具栏，判断前面总可移动空间
			space = bar->m_winpos.y - 
			(bar->m_prev->m_winpos.y + bar->m_prev->m_winpos.cy);
			if(space < 0)
			{
				//两者重叠
				if(bar == dockbar)
				{
					//以dockbar为准，前面的工具栏前移
					space = nSpace + space;
					nSpace = max(0,space);
					space = min(0,space);
				}
				bar->m_winpos.y -= space;
				space = 0;
			}
			nSpace += space;
		}

		space = nSpace + m_winpos.cy - (bar->m_winpos.y + bar->m_winpos.cy);
		if(space <= 0)
		{
			//最后的bar,后面的bar必须另起一行
			bar->SetDockPos(bar->m_winpos.x,bar->m_winpos.y - nSpace);
			bar->m_lineInfo->m_maxSize = nSize;
			nSize += WrapLine(bar->m_next);
			return bar->m_winpos.y;
		}
		if(bar->m_next == NULL)
		{
			//最后的bar,必须返回
			if(space >= nSpace)
				space = 0;
			else
				space = nSpace - space;
			//bar->SetPos(bar->m_winpos.x,bar->m_winpos.y - space);
			//return bar->m_winpos.y;
		}
		else
		{
			//还有空间
			space = (bar->m_winpos.y + bar->m_winpos.cy) - 
						AdjustBarLineV(bar->m_next,nSpace,dockbar,nSize);
			if(space < 0)
				space = 0;
		}

		bar->SetDockPos(bar->m_winpos.x,bar->m_winpos.y - space);
		return bar->m_winpos.y;
	}
	return 9999;
}
*/