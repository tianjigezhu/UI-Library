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
//							RingSDK 演示程序(ISee图象浏览器)
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
//说明：MDI子窗口，图象显示部分代码
//
**********************************************************************/
#include "global.h"

//放大/缩小倍数设置
int gZoomOut[14] = {0,2,3,4,6,8,12,16,20,25,33,50,66,100};
int gZoomIn[10] = {1,2,3,4,5,6,7,8,12,16};

BEGIN_REGMSG(child)
	REGMSG(WM_PAINT)
	REGMSG(WM_ERASEBKGND)
	REGMSG(WM_SIZE)
	REGMSG(WM_LBUTTONDOWN)
	REGMSG(WM_MOUSEMOVE)
	REGMSG(WM_LBUTTONUP)
	REGMSG(WM_LBUTTONDBLCLK)
	REGMSG(WM_KEYDOWN)
	REGMSG(WM_INITDIALOG)
END_REGMSG

void RINGMETHOD(child,SetCursor)(HCURSOR hCur)
{
	m_hCursor = hCur;
	RingBaseWnd::SetCursor(hCur);
}

RINGMSG(child,WM_INITDIALOG)
{
	m_bFullScreen = FALSE;
	m_nRadio = 0;
	SetRectEmpty(&m_rcFloat);
	//设置m_dib的绘制目标为本窗口
	m_dib.SetTarget(m_hWnd);
	
	//设置窗口类光标为NULL，如不设置，则设置其他光标，鼠标一移动就会恢复为原始光标.
	SetCursor(NULL);
	m_hCurHand = LoadCursor(GetInstance(),MAKEINTRESOURCE(IDC_MOVEHAND));
	m_hCurGrab = LoadCursor(GetInstance(),MAKEINTRESOURCE(IDC_GRAB));
	m_hCurZoom = LoadCursor(GetInstance(),MAKEINTRESOURCE(IDC_ZOOMGLASS));
	m_hCurArr = LoadCursor(NULL,IDC_ARROW);
	m_hCursor = m_hCurArr;
	SetCursor(m_hCurArr);
	return FALSE;
}

RINGMSG(child,WM_SIZE)
{
	//尺寸变动，重新计算图象缩放倍率
	UpdatePicShow();
	InvalidateRect(m_hWnd,NULL,FALSE);
	return FALSE;
}

RINGMSG(child,WM_ERASEBKGND)
{
	return TRUE;
}

RINGMSG(child,WM_PAINT)
{
	RECT rc,rcImage;
	PAINTSTRUCT ps;
	
	GetClientRect(&rc);
	m_dib.GetDestRect(&rcImage);
	//绘制图象
	BeginPaint(m_hWnd,&ps);
	m_dib.Draw(ps.hdc);
	//排除图象区域绘制背景，这样不会产生闪烁
	ExcludeClipRect(ps.hdc,rcImage.left,rcImage.top,rcImage.right,rcImage.bottom);
	//绘制背景
	FillRect(ps.hdc,&rc,(HBRUSH)GetStockObject(BLACK_BRUSH));
	EndPaint(m_hWnd,&ps);
	return 0;
}

RINGMSG(child,WM_KEYDOWN)
{
	if(param.wParam == VK_RETURN || param.wParam == VK_ESCAPE)
	{
		SetFullScreen(!m_bFullScreen);
		NavPane->m_fileview->SetFocus();
	}
	else if(m_bFullScreen)
	{
		//全屏状态下按键处理
		switch(param.wParam)
		{
			case VK_NEXT:
				ViewNextImg();
				break;
			case VK_PRIOR:
				ViewNextImg(FALSE);
				break;
			case VK_ADD:
				//放大
				ZoomIn();
				break;
			case VK_SUBTRACT:
				//缩小
				ZoomOut();
				break;
			case VK_UP:
			case VK_DOWN:
			case VK_LEFT:
			case VK_RIGHT:
				MoveImg(param.wParam);
				InvalidateRect(m_hWnd,NULL,FALSE);
				break;
		}
	}
	return 0;
}

RINGMSG(child,WM_LBUTTONDOWN)
{
	SetCapture(m_hWnd);
	
	if(m_hCursor == m_hCurHand)
		SetCursor(m_hCurGrab);
	
	m_ptMouse.x = param.mousept.x;
	m_ptMouse.y = param.mousept.y;
	return DefaultProc(param);
}

RINGMSG(child,WM_MOUSEMOVE)
{
	if(m_hCursor == m_hCurGrab)
	{
		MoveImg(param.mousept.x - m_ptMouse.x,param.mousept.y - m_ptMouse.y);
		m_ptMouse.x = param.mousept.x;
		m_ptMouse.y = param.mousept.y;
	}
	return DefaultProc(param);
}

RINGMSG(child,WM_LBUTTONUP)
{
	ReleaseCapture();
	if(m_hCursor == m_hCurGrab)
		SetCursor(m_hCurHand);
	return DefaultProc(param);
}

RINGMSG(child,WM_LBUTTONDBLCLK)
{
	SetFullScreen(!m_bFullScreen);
	return 0;
}

void RINGMETHOD(child,ZoomIn)()
{
	if(m_nRadio == VIEW_FITSIZE)
		m_nRadio = CalcRadio();
	m_nRadio ++;
	if(m_nRadio > 9)
		m_nRadio = 9;
	SetImgDest();
	InvalidateRect(m_hWnd,NULL,FALSE);
}

void RINGMETHOD(child,ZoomOut)()
{
	if(m_nRadio == VIEW_FITSIZE)
		m_nRadio = CalcRadio();
	m_nRadio --;
	if(m_nRadio < 0)
	{
		SIZE dest = {m_dib.Width(),m_dib.Height()};
		int nStep = 1,nRadio = -m_nRadio - 1;
		while(nRadio > 13)
		{
			nRadio -= 14;
			nStep *= 100; 
		}
		if(nRadio == 0)
		{
			//66.7%
			dest.cx = (dest.cx*2)/3;
			dest.cy = (dest.cy*2)/3;
		}
		else
		{
			dest.cx /= gZoomOut[nRadio];
			dest.cy /= gZoomOut[nRadio];			
		}
		dest.cx /= nStep;
		dest.cy /= nStep;
		
		if(dest.cx < 10 || dest.cy < 10)
			m_nRadio ++;
	}
	SetImgDest();
	InvalidateRect(m_hWnd,NULL,FALSE);
}

void RINGMETHOD(child,ViewNextImg)(BOOL bNext)
{
	//上/下一幅图象
	int nIndex = window->m_fileview->GetCurSel();
	char szFile[MAX_PATH];

	if(bNext)
		nIndex ++;
	else
		nIndex --;

	while(nIndex >= 0 && nIndex < window->m_fileview->GetCount())
	{
		wsprintf(szFile,"%s\\%s",window->m_dirtree->GetSelFolderName(),
					window->m_fileview->RingListView::GetText(nIndex,0));
		if(m_dib.IsImage(szFile))
		{
			window->m_fileview->Select(nIndex);
			break;
		}
		if(bNext)
			nIndex ++;
		else
			nIndex --;
	}
}

//////////////////////////////////////////////////////////
//
//全屏模式切换，同样是停靠窗口的非典型用法，要求在浮动状态
//下全屏显示，只能更改浮动容器的窗口类型，这种情况很少见，
//因此这里不推荐这种做法，当然一般也用不着。
//
//////////////////////////////////////////////////////////
void RINGMETHOD(child,SetFullScreen)(BOOL bFullScreen)
{
	if(bFullScreen != m_bFullScreen)
	{
		RingBaseWnd* p;
		//状态改变
		if(bFullScreen)
		{
			RECT rc = {0,0,WINVAR_DESKTOP_X,WINVAR_DESKTOP_Y};
			//设置为浮动
			if(!IsFloat(GetDockState()))
				DockBack();
			//全屏处理
			p = GetWindowPtr(GetParent());
			if(p)
			{
				p->ReplaceStyle(WS_CAPTION | WS_THICKFRAME,0);
				//保存原窗口位置
				GetWindowRect(p->Handle(),&m_rcFloat);
				p->SetPos(0,0,WINVAR_DESKTOP_X,WINVAR_DESKTOP_Y);
			}
		}
		else
		{
			p = GetWindowPtr(GetParent());
			if(p)
			{
				p->ReplaceStyle(0,WS_CAPTION | WS_THICKFRAME);
			}
			DockBack();
			if(GetParent() == NavPane->GetDockSite(0)->Handle())
			{
				RECT rc;
				GetWindowRect(NavPane->m_navtool->Handle(),&rc);
				OffsetRect(&rc,-rc.left,Height());
				NavPane->m_navtool->DockTo(TBS_FLAG_DOCKTOP,&rc);
			}
		}
		m_bFullScreen = bFullScreen;
		m_nRadio = VIEW_FITSIZE;
		UpdatePicShow();
		InvalidateRect(m_hWnd,NULL,FALSE);
	}
}

BOOL RINGMETHOD(child,View)(LPCTSTR lpszPic)
{
	BOOL bOK = m_dib.Load((LPSTR)lpszPic);
	if(bOK)
	{
		m_dib.EnableAniGif();
		m_nRadio = VIEW_FITSIZE;
		UpdatePicShow();
		char szInfo[40],szFormat[8];
		wsprintf(szInfo,"%dx%d %s",m_dib.Width(),m_dib.Height(),m_dib.GetFormat(szFormat));
		window->m_statbar->SetValue(szInfo,3);
	}
	else
	{
		//m_dib载入图象失败，有可能仍保持原图象，设置绘制目标矩形为空（设宽高为负数）
		m_dib.SetDest(0,0,-1,-1);
		window->m_statbar->SetValue("",3);
	}
	InvalidateRect(m_hWnd,NULL,FALSE);
	return bOK;
}

void RINGMETHOD(child,UpdatePicShow)()
{
	RECT rc;
	SIZE dest;
	
	dest.cx = m_dib.Width();
	dest.cy = m_dib.Height();
	GetClientRect(&rc);
	
	//计算图象显示宽高
	if(rc.right < dest.cx)
	{
		//拉伸显示
		dest.cy = rc.right*m_dib.Height()/m_dib.Width();
		if(dest.cy > rc.bottom)
		{
			dest.cy = rc.bottom;
			dest.cx = dest.cy*m_dib.Width()/m_dib.Height();
		}
		else
			dest.cx = rc.right;
	}
	else if(rc.bottom < dest.cy)
	{
		//拉伸显示
		dest.cx = rc.bottom*m_dib.Width()/m_dib.Height();
		if(dest.cx > rc.right)
		{
			dest.cx = rc.right;
			dest.cy = dest.cx*m_dib.Height()/m_dib.Width();
		}
		else
			dest.cy = rc.bottom;
	}		
	
	//设置目标位置(注意：动画GIF必须，否则采用默认（0，0）位置，不会居中)
	m_dib.SetDest((rc.right - dest.cx)/2,(rc.bottom - dest.cy)/2,dest.cx,dest.cy);	
}

int RINGMETHOD(child,CalcRadio)()
{
	RECT rc;
	SIZE src;
	
	src.cx = m_dib.Width();
	src.cy = m_dib.Height();
	m_dib.GetDestRect(&rc);
	
	//计算图象显示比率
	int xRadio = src.cx/(rc.right - rc.left);
	int yRadio = src.cy/(rc.bottom - rc.top);
	int nradio,nCnt=0;
	if((xRadio >= 1 || yRadio >= 1) && 
		(src.cx%(rc.right - rc.left) > 0 || src.cy%(rc.bottom - rc.top) > 0))
	{
		//已缩小显示，计算比率
		if(xRadio == 1)
			return -1;	 //66.7%

		nradio = xRadio;
		while(nradio > 100)
		{
			nradio /= 100;
			nCnt ++;
		}
		int nAbs,nMin = 100000;
		for(int i=1;i<14;i++)
		{
			nAbs = abs(nradio - gZoomOut[i]);
			if(nAbs > nMin)
				break;
			else
				nMin = nAbs;
		}
		return -(i+nCnt*14);
	}
	return 0;
}

void RINGMETHOD(child,SetImgDest)()
{
	RECT rc,rcImage;
	SIZE dest;
	int nOffX=0,nOffY=0;
	
	dest.cx = m_dib.Width();
	dest.cy = m_dib.Height();
	GetClientRect(&rc);
	m_dib.GetDestRect(&rcImage);

	if(m_nRadio >= 0)
	{
		//放大
		dest.cx *= gZoomIn[m_nRadio];
		dest.cy *= gZoomIn[m_nRadio];		
	}
	else if(m_nRadio < 0)
	{
		//缩小
		int nStep = 1,nRadio = -m_nRadio - 1;
		while(nRadio > 13)
		{
			nRadio -= 14;
			nStep *= 100; 
		}
		if(nRadio == 0)
		{
			//66.7%
			dest.cx = (dest.cx*2)/3;
			dest.cy = (dest.cy*2)/3;
		}
		else
		{
			dest.cx /= gZoomOut[nRadio];
			dest.cy /= gZoomOut[nRadio];			
		}
		dest.cx /= nStep;
		dest.cy /= nStep;
	}
	
	rcImage.left += ((rc.right - rc.left)/2 - rcImage.left)*(rcImage.right - rcImage.left -dest.cx)/(rcImage.right - rcImage.left);
	rcImage.right = rcImage.left + dest.cx;
	rcImage.top += ((rc.bottom - rc.top)/2 - rcImage.top)*(rcImage.bottom - rcImage.top -dest.cy)/(rcImage.bottom - rcImage.top);
	rcImage.bottom = rcImage.top + dest.cy;
	
	//调整位置以防超出屏幕范围
	if(rcImage.left > rc.left && rcImage.right > rc.right)
		nOffX = rc.left - rcImage.left;
	else if(rcImage.left < rc.left && rcImage.right < rc.right)
		nOffX = rc.right - rcImage.right;
	if(dest.cx <= (rc.right - rc.left))
		nOffX = ((rc.right - rc.left) - dest.cx)/2 - rcImage.left;
	if(rcImage.top > rc.top && rcImage.bottom > rc.bottom)
		nOffY = rc.top - rcImage.top;
	else if(rcImage.top < rc.top && rcImage.bottom < rc.bottom)
		nOffY = rc.bottom - rcImage.bottom;
	if(dest.cy <= (rc.bottom - rc.top))
		nOffY = ((rc.bottom - rc.top) - dest.cy)/2 - rcImage.top;

	if(dest.cx > (rc.right - rc.left) || dest.cy > (rc.bottom - rc.top))
		SetCursor(m_hCurHand);
	else
		SetCursor(LoadCursor(NULL,IDC_ARROW));

	m_dib.SetDest(rcImage.left+nOffX,rcImage.top+nOffY,dest.cx,dest.cy);
}

//图象超过窗口尺寸，可移动图象（键盘控制）
BOOL RINGMETHOD(child,MoveImg)(int nDir)
{
	RECT rcImg;
	RECT rc;
	BOOL bSet = FALSE;
	int nOffX=0,nOffY=0;
	
	GetClientRect(&rc);
	m_dib.GetDestRect(&rcImg);

	switch(nDir)
	{
	case VK_UP:
		if(rcImg.top < rc.top)
			nOffY = -min(50,rc.top - rcImg.top);
		break;
	case VK_DOWN:
		if(rcImg.bottom > rc.bottom)
			nOffY = min(50,rcImg.bottom - rc.bottom);
		break;
	case VK_LEFT:
		if(rcImg.left < rc.left)
			nOffX = -min(50,rc.left - rcImg.left);
		break;
	case VK_RIGHT:
		if(rcImg.right > rc.right)
			nOffX = min(50,rcImg.right - rc.right);
		break;
	}
	if(nOffX != 0 || nOffY != 0)
	{
		OffsetRect(&rcImg,-nOffX,-nOffY);
		m_dib.SetDest(rcImg.left,rcImg.top,rcImg.right - rcImg.left,rcImg.bottom - rcImg.top);
		return TRUE;
	}
	return FALSE;
}

//图象超过窗口尺寸，可移动图象（鼠标拖动）
void RINGMETHOD(child,MoveImg)(int nOffX,int nOffY)
{
	RECT rcImg;
	RECT rc;
	
	GetClientRect(&rc);
	m_dib.GetDestRect(&rcImg);
	
	if(nOffX < 0)
	{
		if(rcImg.right > rc.right)
			nOffX = -max(nOffX,rc.right - rcImg.right);
		else
			nOffX = 0;
	}
	else
	{
		if(rcImg.left < rc.left)
			nOffX = -min(nOffX,rc.left - rcImg.left);
		else
			nOffX = 0;
	}
	if(nOffY < 0)
	{
		if(rcImg.bottom > rc.bottom)
			nOffY = -max(nOffY,rc.bottom - rcImg.bottom);
		else
			nOffY = 0;
	}
	else
	{
		if(rcImg.top < rc.top)
			nOffY = -min(nOffY,rc.top - rcImg.top);
		else
			nOffY = 0;
	}

	if(nOffX != 0 || nOffY != 0)
	{
		OffsetRect(&rcImg,-nOffX,-nOffY);
		m_dib.SetDest(rcImg.left,rcImg.top,rcImg.right - rcImg.left,rcImg.bottom - rcImg.top);
		m_dib.Draw();
	}
}
