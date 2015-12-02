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
//原文件名：ringstatus.cpp
//
//本文件说明：状态栏实现代码
//
**********************************************************************/
#define MAKE_SELF_LIB
//#ifndef __BORLANDC__
#include "ringdows.h"
#include <malloc.h>

#define MIN_PARTWIDTH	6

//RingMsg mg;

RingStatusBar::RingStatusBar()
{
	InitCommonControls();
	m_windowType = ITIS_STATUSBAR;
	m_pane = NULL;
	m_nPartCount = 0;
//mg.SetMsgFile((ringStr(RSTR_CURRPATH,TRUE) + "\\msg.txt").string());
}

RingStatusBar::~RingStatusBar()
{
}

BOOL RingStatusBar::Create(DWORD dwStyle/*=SBARS_SIZEGRIP*/)
{
	if(IsWindow())
		return TRUE;

	//父窗口不能是控件
	if(m_parent->IsControls())
		return FALSE;
	
	RingDockSite* pSite;
	
	if(m_parent->GetWindowType() != ITIS_DOCKSITE)
		pSite = ((RingWnd*)m_parent)->CreateDockSite(TBS_FLAG_DOCKBOTTOM);
	else
		pSite = (RingDockSite*)m_parent;
	
	if(pSite == NULL)
		return FALSE;
	
	//去边框，RingControls::Create自动加WS_CHILD | WS_VISIBLE
	dwStyle &= ~(WS_BORDER | CCS_NODIVIDER);
	
	RingControls::Create(dwStyle,0,"",(UINT)m_windowType,0,0,10,10);
	
	m_dockbar = EnableDock(NULL,TBS_EX_NONE|TBS_EX_ALLLINE,TBS_FLAG_DOCKBOTTOM);
	return (BOOL)m_hWnd;
}

LRESULT RingStatusBar::RingdowProc(HWND hWnd,RINGPARAMS param)
{
	switch(param.uMsg)
	{
		case SB_SIMPLE:
			//for MenuHelp
			if(m_pane && (m_pane->m_uStyle & SBPS_STRETCH))
			{
				//如果第一格是不定长，则为显示菜单帮助，此时需要检测是否屏蔽该消息
				//该处理是为做出类似VC状态栏的效果，即前面显示帮助，后面显示状态格
				if(param.wParam && param.lParam == 0)
					return 0;//系统发送的消息，屏蔽
			}
			return CallWindowProc(m_DefProc,m_hWnd,SB_SIMPLE,param.wParam,0);
		case SB_SETTEXTA:
		case SB_SETTEXTW:
			//Win2000内部是UNICODE版本，因此此处两个都要判断
			if((param.wParam & 0xFF) == 0xFF)
			{
				//MenuHelp,系统发送的消息
				//如果第一格是不定长，则为显示菜单帮助，此时需要在第一格显示菜单帮助，
				//保持后面的状态格以做出类似VC状态栏的效果
				if(m_pane && (m_pane->m_uStyle & SBPS_STRETCH) && !IsSimple())
				{
					if(CallWindowProc(m_DefProc,m_hWnd,param.uMsg,SBT_NOBORDERS,param.lParam))
					{
						InvalidateRect(m_hWnd,NULL,TRUE);
						return TRUE;
					}
					else
						return FALSE;
				}
			}
			return CallWindowProc(m_DefProc,m_hWnd,param.uMsg,param.wParam,param.lParam);
		case SB_SETPARTS:
			if(CallWindowProc(m_DefProc,m_hWnd,SB_SETPARTS,param.wParam,param.lParam))
				m_nPartCount = param.wParam;
			return (BOOL)m_nPartCount;
		case WM_COMMAND:
			return SendMessage(m_parent->Handle(),param.uMsg,param.wParam,param.lParam);		
		case WM_SIZE:
			UpdateAllParts();
			InvalidateRect(m_hWnd,NULL,TRUE);
			return 0;
		case WM_NCDESTROY:
			CallWindowProc(m_DefProc,m_hWnd,param.uMsg,param.wParam,param.lParam);
			FreeAllPart();
			OnNcDestroy();
			return 0;			
	}
	return DefaultProc(param);
}

/**********************************************************
//
//分隔栏设置
//参数：
//		nParts:分隔栏个数
//		lpArr:数组，每个元素高4位指定字符串资源ID，低3位指定宽度
//				0为适应文字，0xFFF为可伸缩长度，低4位最高4BIT指定
//				文字显示方式，该数组元素值可用MAKESBPINT宏生成
//
//********************************************************/
BOOL RingStatusBar::SetParts(int nParts,LPINT lpArr/*=NULL*/)
{
	if(nParts > 0 && nParts < 256)
	{
		int i;
		UINT uId;
		LPINT lpi = lpArr;
		LPRINGSTATUSPANE pane = NULL;
		int nStep = Width() / nParts,nEdge = 0;

		if(lpi == NULL)
		{
			lpi = (LPINT)_alloca(nParts * sizeof(int));
			memset(lpi,0,nParts * sizeof(int));
			*lpi = MAKESBPINT(0,SBTS_NOBORDER,SBPW_STRETCH);
		}		
		
		CreatePaneInfo(nParts);

		for(i=0;i<nParts;i++)
		{
			pane = GetPanePtr(i);
			if(pane)
			{
				uId = lpi[i] >> 16;
				if(uId)
				{
					//载入资源字符串
					if(pane->m_str == NULL)
						pane->m_str = new ringStr;
					if(pane->m_str)
						pane->m_str->Load(uId);					
				}
				//检测类型
				pane->m_nWidth = lpi[i] & 0x0FFF;
				if(pane->m_nWidth > 0 && pane->m_nWidth < SBPW_FITTEXT)					
				{
					//固定宽度
					pane->m_uStyle = SBPF_UPDATE | SBPS_STATICW | SBPWSTYLE(lpi[i]);					
				}
				else if(pane->m_nWidth == SBPW_STRETCH)// || lpi[i] == -1)
				{
					//不定长
					pane->m_uStyle = SBPF_UPDATE | SBPS_STRETCH | SBPWSTYLE(lpi[i]);
				}
				else if(pane->m_nWidth == SBPW_FITTEXT)
				{
					//适应文字
					pane->m_uStyle = SBPF_UPDATE | SBPS_FITTEXT | SBPWSTYLE(lpi[i]);
				}
				else
				{
					//默认固定宽度
					pane->m_nWidth = nStep;
					pane->m_uStyle = SBPF_UPDATE | SBPS_STATICW;
				}
			}
			else
				return FALSE;
		}		
		UpdateAllParts();
		return TRUE;
	}
	else
		return FALSE;
}

//设置文字，val为字符串资源ID
BOOL RingStatusBar::SetValue(int val,int index/*=INDEX_CURSEL*/)
{
	//状态栏限制每格字符最大127
	char s[160];
	memset(s,0,160);
	if(!LoadString(GetInstance(),val,s,160))
		wsprintf(s,"%d",val);

	return SetValue(s,index);	
}

BOOL RingStatusBar::SetValue(LPCTSTR lpszVal,int index/*=INDEX_CURSEL*/)
{	
	LPRINGSTATUSPANE pane = GetPanePtr(LOWORD(index));
	if(pane)
		return SetPaneText(pane,lpszVal,HIWORD(index));
	else
		return ::SendMessage(m_hWnd,SB_SETTEXT,index,(LPARAM)lpszVal);
}

int RingStatusBar::GetItemText(int index,LPTSTR lpBuf,int size)
{
	//本函数必须返回字符长度，因此需LOWORD去掉HIWORD的类型
	return (int)LOWORD(SendMessage(m_hWnd,SB_GETTEXT,index,(LPARAM)lpBuf));
}

BOOL RingStatusBar::OnParentNotify(HWND hWnd,UINT uMsg,UINT wParam,LONG lParam)
{
	switch(uMsg)
	{
		case MYWM_SIZE:
			if(IsVert(lParam))
				return WinWidth(m_hWnd);
			else
				return WinHeight(m_hWnd);
/*		case WM_SIZE:
			UpdateAllParts();
			break;
*/
	}
	return FALSE;
}

UINT RingStatusBar::GetPartStyle(int nPart)
{
	LPRINGSTATUSPANE pane = GetPanePtr(nPart);
	if(pane)
		return pane->m_uStyle;
	else
		return 0;
}

int RingStatusBar::GetPartWidth(int nPart)
{
	RECT rc;
	if(GetRect(nPart,&rc))
		return rc.right - rc.left;
	else
		return 0;
}

/*****************************************************
//
//设置分隔栏类型
//
//参数：
//	nPart:分隔栏序号（从0开始）
//	lpszVal:要显示的文字
//	uStyle:类型，如果是-1，则不改变原类型
//	nWidth:宽度，如果类型指定为适应文字，则忽略该参数
//
******************************************************/
BOOL RingStatusBar::SetValue(int nPart,LPCTSTR lpszVal,int uStyle/*=-1*/,int nWidth/*=0*/)
{
	LPRINGSTATUSPANE pane = GetPanePtr(nPart);
	if(pane)
	{
		if(uStyle != -1)
			pane->m_uStyle = uStyle;

		//判断是否改变宽度
		if(pane->m_uStyle == SBPS_STATICW && nWidth > 0)
			pane->m_nWidth = nWidth;
		
		if(pane->m_str == NULL)
			pane->m_str = new ringStr(lpszVal);
		else
			*(pane->m_str) = lpszVal;
		
		pane->m_uStyle |= SBPF_UPDATE;
		UpdateAllParts();
		return TRUE;
	}
	return FALSE;
}

BOOL RingStatusBar::SetPaneText(LPRINGSTATUSPANE lpPart,LPCTSTR lpszVal,UINT uStyle)
{
	if(lpPart)
	{
		//DestroyTimerPart(lpPart);
		//lpPart->m_uStyle |= SPS_TEXT;
		if(lpPart->m_str == NULL)
			lpPart->m_str = new ringStr(lpszVal);
		else
			*(lpPart->m_str) = lpszVal;
		
		lpPart->m_uStyle &= 0xFFFF00FF;
		lpPart->m_uStyle |= uStyle|SBPF_UPDATE;
		if(lpPart->m_uStyle & SBPS_FITTEXT)
			UpdateAllParts();
		else
			return ::SendMessage(m_hWnd,SB_SETTEXT,MAKELONG(lpPart->m_nIndex,uStyle),
										(LPARAM)lpPart->m_str->string());
		return TRUE;
	}
	else
		return FALSE;
}

void RingStatusBar::UpdateAllParts()
{
	HDC hDC;
	int *lpEdge = (int*)_alloca(m_nPartCount * sizeof(int));
	int nRight = Width();
	int i,nEdge=0,nStretchCnt = 0;
	SIZE size;
	LPRINGSTATUSPANE pane;
	int nBorder[3],nMin0 = 0;
	int nBordSize = GetSystemMetrics(SM_CXBORDER);

	GetBorders(nBorder);
	if((GetStyle() & SBARS_SIZEGRIP) && !::IsZoomed(m_parent->Handle()))
	{
		// size grip uses a border + size of scrollbar + cx border
		nRight -= nBorder[0] + GetSystemMetrics(SM_CXVSCROLL) + nBordSize * 2;
	}

	hDC = GetDC(m_hWnd);
	HFONT hf;
	
	hf = (HFONT)SendMessage(m_hWnd,WM_GETFONT,0,0);
	SelectObject(hDC,hf);

	for(i=0;i<m_nPartCount;i++)
	{
		pane = GetPanePtr(i);
		if(pane)
		{
			if(pane->m_child)
			{
				//检测子窗口有效性，以免用户移走但未通知本窗口
				if(!IsWindowPtr(pane->m_child) || pane->m_child->GetParent() != m_hWnd)
				{
					pane->m_child = NULL;
					pane->m_nWidth = MIN_PARTWIDTH;
				}
			}

			if(MASK_MATCH(pane->m_uStyle,SBPS_STRETCH))
				nStretchCnt ++;
			else 
			{
				if(MASK_MATCH(pane->m_uStyle,SBPS_FITTEXT))
				{
					if(MASK_MATCH(pane->m_uStyle,SBPF_UPDATE))
					{						
						if(pane->m_str && !pane->m_str->isClear())
						{							
							GetTextExtentPoint32(hDC,pane->m_str->string(),
														pane->m_str->len(),&size);
							pane->m_nWidth = size.cx + 12 + nBordSize * 2;
							if(GetIcon(i))
								pane->m_nWidth += size.cy + nBordSize * 2;
						}
						else if(pane->m_child)
							pane->m_nWidth = pane->m_child->Width() + nBordSize * 2;
						else
							pane->m_nWidth = MIN_PARTWIDTH;
					}
				}
				nRight -= pane->m_nWidth;
			}
		}
		else
			nRight -= (GetPartWidth(i) + nBorder[2]);
	}	
	ReleaseDC(m_hWnd,hDC);

	int nPrevEdge=nBordSize,x = 0;
	RECT rc;
	GetRect(0,&rc);
	for(i=0;i<m_nPartCount;i++)
	{
		pane = GetPanePtr(i);
		if(pane)
		{
			if(pane->m_uStyle & SBPS_STRETCH)
			{
				if(i == 0)
				{
					//第一格变长，为显示菜单帮助用，至少1/4宽
					nEdge = max(GetSystemMetrics(SM_CXSCREEN)/4,nRight / nStretchCnt);
					x = 1;
					nMin0 = nEdge;					
				}
				else					
					nEdge += max(MIN_PARTWIDTH,(nRight-nMin0) / (nStretchCnt-x));
			}
			else
				nEdge += pane->m_nWidth;
			
			if(pane->m_child)
			{
				if(MASK_MATCH(pane->m_child->GetExStyle(),RTV_FITPARENTSIZE))
					pane->m_child->SetPos(nPrevEdge,rc.top,nEdge-nBordSize-nPrevEdge,rc.bottom-rc.top);
				else
					pane->m_child->SetPos(nPrevEdge,rc.top);
			}
			nPrevEdge = nEdge + nBordSize;

			if(pane->m_uStyle & SBPF_UPDATE)
			{
				pane->m_uStyle &= ~SBPF_UPDATE;
				if(pane->m_str)
					::SendMessage(m_hWnd,SB_SETTEXT,i|(pane->m_uStyle&0xFF00),
										(LPARAM)pane->m_str->string());
			}			
		}
		else
			nEdge += GetPartWidth(i) + nBorder[2];

		lpEdge[i] = nEdge;
	}
	::SendMessage(m_hWnd,SB_SETPARTS,m_nPartCount,(LPARAM)lpEdge);	
}

LPRINGSTATUSPANE RingStatusBar::GetPanePtr(int index)
{
	if(index >=0 && index < m_nPartCount)
		return m_pane?(m_pane + index):NULL;
	else
		return NULL;
}

void RingStatusBar::FreeAllPart()
{
	LPRINGSTATUSPANE pane = m_pane;
	
	for(int i=0;i<m_nPartCount;i++,pane++)
	{
		delete pane->m_str;		
	}
	m_pane = (LPRINGSTATUSPANE)Del(m_pane);
}

BOOL RingStatusBar::CreatePaneInfo(int nParts)
{
	if(nParts > 0 && nParts < 256)
	{
		if(m_pane == NULL)
		{
			m_pane = (LPRINGSTATUSPANE)New(nParts * sizeof(RINGSTATUSPANE));
			m_nPartCount = nParts;
		}
		else if(m_nPartCount != nParts)
		{
			LPRINGSTATUSPANE pane = (LPRINGSTATUSPANE)New(nParts * sizeof(RINGSTATUSPANE));
			if(pane)
			{
				memcpy(pane,m_pane,m_nPartCount * sizeof(RINGSTATUSPANE));
				Del(m_pane);
				m_pane = pane;
				m_nPartCount = nParts;
			}
		}
		else
			return TRUE;

		if(m_pane)
		{
			for(int i=0;i<m_nPartCount;i++)
				m_pane[i].m_nIndex = i;
			return TRUE;
		}
	}
	return FALSE;
}

int RingStatusBar::OnMenuHelp(RINGPARAMS &param,HMENU hMenu)
{
	UINT uIds[] = {MH_BASE,MH_POPUPBASE,0,0};
	MenuHelp(WM_MENUSELECT,param.wParam,param.lParam,hMenu,GetInstance(),m_hWnd,uIds);
	return 0;
}

BOOL RingStatusBar::InsertItem(RingBaseWnd* wnd,int iPart)
{
	if(IsWindowPtr(wnd))
	{
		LPRINGSTATUSPANE pane = GetPanePtr(iPart);
		if(pane && pane->m_child == NULL)
		{
			pane->m_child = wnd;
			pane->m_uStyle |= SBPF_UPDATE;
			wnd->SetParent(m_hWnd);
			UpdateAllParts();
			return TRUE;
		}
	}
	return FALSE;
}

/*
BOOL RingStatusBar::SetTimerFormat(int index,LPCTSTR lpszFormat)
{
	if(lpszFormat == NULL || *lpszFormat == '\0')
		return FALSE;

	LPRINGSTATUSPANE pane = GetPanePtr(index,TRUE);
	if(pane)
	{
		if((pane->m_uStyle & SPS_TIMER) == 0)
		{
			pane->m_uStyle |= SPS_TIMER;
			pane->m_Timer = NULL;
			return SetTimerFormat(pane,lpszFormat);
		}
	}
	return TRUE;
}

BOOL RingStatusBar::SetTimerFormat(LPRINGSTATUSPANE lpPart,LPCTSTR lpszFormat)
{
	if((lpPart->m_uStyle & SPS_TIMER) && lpszFormat)
	{
		if(lpPart->m_str == NULL)
		{
			lpPart->m_str = new ringStr;
			if(lpPart->m_str == NULL)
				return FALSE;
		}
		
		if(lpPart->m_Timer == NULL)
			lpPart->m_Timer = new RingThread;

		if(lpPart->m_Timer)
		{
			lpPart->m_Timer->Wait(TRUE);
			lpPart->m_Timer->Start(this,(THREADFUNC)m_TimerProc,(LONG)lpPart);
			Sleep(200);
			ringStr str1 = lpszFormat;
			ringStr str2;
			str1.replace(str2,"YYYY","%4d",FALSE);
			str2.replace(str1,"MM","%02d",FALSE);
			str1.replace(str2,"DD","%02d",FALSE);
			str2.replace(str1,"hh","%02d",FALSE);
			str1.replace(str2,"mm","%02d",FALSE);
			str2.replace(str1,"ss","%02d",FALSE);
			str1.replace(str2,"M","%d",FALSE);
			str2.replace(str1,"D","%d",FALSE);
			str1.replace(str2,"h","%d",FALSE);
			str2.replace(str1,"m","%d",FALSE);
			str1.replace(str2,"s","%d",FALSE);
			*(lpPart->m_str) = str1;
			lpPart->m_Timer->Continue();
			return TRUE;
		}		
	}
	return FALSE;
}

BOOL RingStatusBar::SetProgress(int index,int nStep)
{
	return TRUE;
}

void RingStatusBar::DestroyTimerPart(LPRINGSTATUSPANE lpPart)
{
	if(lpPart && (lpPart->m_uStyle & SPS_TIMER))
	{
		try
		{
			delete lpPart->m_Timer;
		}
		catch(...)
		{
		}
		lpPart->m_Timer = NULL;
		lpPart->m_uStyle &= ~SPS_TIMER;
	}
}

DWORD RingStatusBar::m_TimerProc(LPRINGSTATUSPANE lpPart)
{
	return 0x57a7;
}
*/
