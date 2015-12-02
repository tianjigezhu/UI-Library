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
//原文件名：ringsize.cpp
//
//本文件说明：工具条窗口实现代码，该工具条窗口只是一个容器，
//					可调整尺寸
//
**********************************************************************/

#define MAKE_SELF_LIB
#include "ringdows.h"

#define RSMAXMIN_ZOOMUP		12
#define RSMAXMIN_ZOOMDOWN	24
#define RSMAXMIN_ZOOMLEFT	36
#define RSMAXMIN_ZOOMRIGHT	48

RingSizeBar::RingSizeBar()
{
	m_vfont = NULL;
	m_CurWhere = HTCLIENT;
	SetRectEmpty(&m_rcAllLine);
	SetRectEmpty(m_rcSpliter);
	SetRectEmpty(m_rcSpliter+1);
	m_nZoom = SW_RESTORE;
	m_font = (HFONT)GetStockObject(DEFAULT_GUI_FONT);
}

RingSizeBar::~RingSizeBar()
{
	if(m_vfont)
		DeleteObject(m_vfont);
}

//初始化停靠窗口尺寸
void RingSizeBar::InitDockSize()
{
	if(m_child && m_pSite)
	{
		RECT rc;
		GetWindowRect(m_child->Handle(),&rc);
		OffsetRect(&rc,-rc.left,-rc.top);

		//记录浮动尺寸
		CopyRect(&m_frc,&rc);

		if(m_pSite->IsVert())
			m_nSize = rc.right + m_BorderSpace*2;
		else
			m_nSize = rc.bottom + m_BorderSpace*2;

		//GetMaxClientRect(&rc);
		//m_child->SetPos(0,0,rc.right,rc.bottom,NULL,SWP_NOMOVE);
		UpdateSize();
	}
}

void RingSizeBar::OnDockStateChange(int stateOld,int stateNew)
{
	//默认处理
	if(stateOld != TBS_FLAG_NONE)
	{
		int w,h;
		if(::IsVert(stateNew))
		{
			w = m_vrc.right - m_vrc.left - m_BorderSpace*2;
			if(w <= 0)
				w = m_rcClient.bottom - m_rcClient.top;
			h = m_rcDockPos.bottom - m_rcDockPos.top;
		}
		else if(::IsHorz(stateNew))
		{
			w = m_rcDockPos.right - m_rcDockPos.left;
			h = m_hrc.bottom - m_hrc.top - m_BorderSpace*2;
			if(h <= 0)
				h = m_rcClient.right - m_rcClient.left;
		}
		else
		{
			w = m_frc.right - m_frc.left;;
			h = m_frc.bottom - m_frc.top;;
		}
		m_child->SetPos(0,0,w,h,0,SWP_NOMOVE);
	}
}

//得到指定停靠位置的客户坐标
BOOL RingSizeBar::GetClientRect(int state,LPRECT lprc)
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
			if(state == TBS_FLAG_FLY)
			{
				//浮动尺寸由InitDokSize初始化，复制
				CopyRect(lprc,&m_frc);
			}
			else
			{
				GetWindowRect(m_child->Handle(),lprc);
				if((state == TBS_FLAG_VERT && !IsVert()) || 
					(state == TBS_FLAG_HORZ && IsVert()))
				{
					//横竖状态改变，矩形宽高调换
					x = lprc->bottom;
					lprc->bottom = lprc->right;
					lprc->right = x;
					x = lprc->left;
					lprc->left = lprc->top;
					lprc->top = x;
				}
			}
		}
		return TRUE;
	}
	else
		return FALSE;
}

int RingSizeBar::GetPreSpace()
{
	int m,n;
	
	if(IsVert())
	{
		m = m_rcClient.bottom - m_rcClient.top;
		n = m_GripperSpace + ((RING_SPLITERSIZE + m_BorderSpace) << 1);
	}
	else
	{			
		m = m_rcClient.right - m_rcClient.left;
		n = m_GripperSpace + ((RING_SPLITERSIZE + m_BorderSpace) << 1);
	}

	//UpdateCaption(0);
	
	return IsVisible()?max(0,(m - n)):m;
}

void RingSizeBar::UpdateFlySize(int nWidth,int nHeight)
{
	RingDockBar::UpdateFlySize(nWidth,nHeight);
	if(m_child)
		MoveWindow(m_child->Handle(),0,0,nWidth - WINVAR_FRAME_X*2,
						nHeight - BARTOOLY - WINVAR_FRAME_Y,TRUE);
}

/*
LRESULT RingSizeBar::OnSizing(RINGPARAMS& param)
{	
	//if(m_child)
	//	SendMessage(m_child->Handle(),param.uMsg,param.wParam,param.lParam);
	return TRUE;
}
*/

/***************************************************************************
//
//调整停靠位置，nEdge为右边界，nPreSpace为前面可移动空间，工具栏
//超过右边界，应往前移，但移动距离不可超过可移动空间。
//本函数假设所有工具栏都不重叠
//
***************************************************************************/
HDWP RingSizeBar::AdjustDockPosH(HDWP hdwp,int nEdge,int nPreSpace,BOOL bCanWrap)
{
	int nPrevLen=0,offset = 0;
	int nMinSize = m_GripperSpace + ((RING_SPLITERSIZE + m_BorderSpace) << 1);

	if(m_child && m_pSite && m_bVisible)
	{
		if(nPreSpace < 0)
		{
			m_rcDockPos.left -= nPreSpace;
			m_rcClient.left -= nPreSpace;
			nPreSpace = 0;
		}
		if(m_rcDockPos.right > nEdge && m_rcDockPos.right - m_rcDockPos.left <= nMinSize)
		{
			nPrevLen = m_rcDockPos.right - nEdge;
			nPrevLen = min(nPrevLen,nPreSpace);
			OffsetDockPos(-nPrevLen,0);
		}
		else
		{
			if(m_nZoom == SW_MINIMIZE)// || (HIWORD(m_nZoom) == 1 && nPreSpace - nPrevLen==0))
				OffsetDockPos(nEdge - m_rcDockPos.right,0);
			else
			{
				nPrevLen = m_rcDockPos.right - nEdge;
				//nPrevLen如果为负，不能把后面空间算在nPreSpace里面
				nPrevLen = max(0,min(nPrevLen,nPreSpace));
				m_rcDockPos.right = nEdge;
				
				if(m_rcDockPos.right - m_rcDockPos.left < nMinSize)
				{
					if(nPreSpace - nPrevLen > 0)
					{
						m_rcDockPos.left = nEdge - (nMinSize + (nPreSpace - nPrevLen)/m_lineInfo->m_nSizeBarCnt);
						m_rcClient.left = m_rcDockPos.left + m_GripperSpace;
					}
					else
						m_rcDockPos.right = m_rcDockPos.left + nMinSize;					
				}
				m_rcClient.right = m_rcDockPos.right - m_BorderSpace;
			}
		}
		
		offset = m_lineInfo->m_maxSize - m_nSize;// - RING_SPLITERSIZE - m_nSize;
		m_nSize += offset;
		m_rcClient.bottom += offset;
		m_rcDockPos.bottom += offset;
		
		UpdateCaption();
		
		//调整尺寸后不刷新，在DockSite的WM_ERASEBKGND消息调用Draw时刷新
		hdwp = m_child->DefDeferWindowPos(hdwp,m_rcClient.left,m_rcClient.top,
			m_rcClient.right - m_rcClient.left,m_rcClient.bottom - m_rcClient.top);

		if(m_nZoom == SW_RESTORE)
			m_nRestoreSize = m_rcClient.right - m_rcClient.left;
		if(m_prev && hdwp)
		{
			//if(offset < 0)
			//	offset = m_prev->m_winpos.right - m_winpos.x;
			nPreSpace -= nPrevLen;//min(offset,m_winpos.x - m_prev->m_winpos.right);
			m_prev->AdjustDockPosH(hdwp,m_rcDockPos.left,nPreSpace,bCanWrap);
		}
		//else
		//	m_pSite->WrapBar(m_lineInfo,NULL,FALSE);
	}
	return hdwp;
}

/***************************************************************************
//
//调整停靠位置，nEdge为下边界，nPreSpace为上面可移动空间，工具栏
//超过下边界，应往上移，但移动距离不可超过可移动空间。
//本函数假设所有工具栏都不重叠
//
***************************************************************************/
HDWP RingSizeBar::AdjustDockPosV(HDWP hdwp,int nEdge,int nPreSpace,BOOL bCanWrap)
{
	int nPrevLen,offset = 0;
	int nMinSize = m_GripperSpace + ((RING_SPLITERSIZE + m_BorderSpace) << 1);

	if(m_child && m_pSite && m_bVisible)
	{
		if(nPreSpace < 0)
		{
			m_rcDockPos.top -= nPreSpace;
			m_rcClient.top -= nPreSpace;
			nPreSpace = 0;
		}
		if(m_rcDockPos.bottom > nEdge && m_rcDockPos.bottom - m_rcDockPos.top <= nMinSize)
		{
			nPrevLen = m_rcDockPos.bottom - nEdge;
			nPrevLen = min(nPrevLen,nPreSpace);
			OffsetDockPos(0,-nPrevLen);
		}
		else
		{
			if(m_nZoom == SW_MINIMIZE)// || (HIWORD(m_nZoom) == 1 && nPreSpace - nPrevLen==0))
				OffsetDockPos(0,nEdge - m_rcDockPos.bottom);
			else
			{
				nPrevLen = m_rcDockPos.bottom - nEdge;
				//nPrevLen如果为负，不能把后面空间算在nPreSpace里面
				nPrevLen = max(0,min(nPrevLen,nPreSpace));
				m_rcDockPos.bottom = nEdge;
				
				if(m_rcDockPos.bottom - m_rcDockPos.top < nMinSize)
				{
					if(nPreSpace - nPrevLen > 0)
					{
						m_rcDockPos.top = nEdge - (nMinSize + (nPreSpace - nPrevLen)/m_lineInfo->m_nSizeBarCnt);
						m_rcClient.top = m_rcDockPos.top + m_GripperSpace;
					}
					else
						m_rcDockPos.bottom = m_rcDockPos.top + nMinSize;					
				}
				m_rcClient.bottom = m_rcDockPos.bottom - m_BorderSpace;
			}
		}
		/*原有代码
		if(m_prev)
			nPrevLen = m_winpos.y - m_prev->m_winpos.bottom;

		if(m_winpos.bottom > nEdge)
		{
			offset = m_winpos.bottom - nEdge;
			m_winpos.bottom = nEdge;
			m_winpos.cy -= offset;
			m_rcClient.bottom -= offset;
		}
		else if(nPreSpace < 0)
		{
			offset = -nPreSpace;
			m_winpos.bottom += offset;
			m_rcClient.bottom += offset;
			m_winpos.cy += offset;			
		}
		*/
		offset = m_lineInfo->m_maxSize - m_nSize;// - RING_SPLITERSIZE - m_nSize;
		m_nSize += offset;
		m_rcClient.right += offset;
		m_rcDockPos.right += offset;

		UpdateCaption();
		//if(m_winpos.x > nEdge && bCanWrap)
		//	m_pSite->WrapBar(m_lineInfo,this,(m_next == NULL));
		//else
		hdwp = m_child->DefDeferWindowPos(hdwp,m_rcClient.left,m_rcClient.top,
							m_rcClient.right - m_rcClient.left,m_rcClient.bottom - m_rcClient.top);

		if(m_nZoom == SW_RESTORE)
			m_nRestoreSize = m_rcClient.right - m_rcClient.left;
		if(m_prev && hdwp)
		{
			//if(offset < 0)
			//	offset = m_prev->m_winpos.right - m_winpos.x;
			nPreSpace -= nPrevLen;//min(offset,m_winpos.x - m_prev->m_winpos.right);
			m_prev->AdjustDockPosV(hdwp,m_rcDockPos.top,nPreSpace,bCanWrap);
		}
		//else
		//	m_pSite->WrapBar(m_lineInfo,NULL,FALSE);
	}
	return hdwp;
}

//计算子窗口尺寸
BOOL RingSizeBar::AdjustChildH(int nAddSpace)
{
	if(m_child && m_pSite && m_bVisible)
	{
		int nStep = 0;
		RECT rc;
		GetWindowRect(m_child->Handle(),&rc);
		OffsetRect(&rc,-rc.left,-rc.top);

		if(m_lineInfo)
		{
			//纵向尺寸
			m_rcDockPos.top = m_lineInfo->m_pos;
			m_nSize = rc.bottom + m_BorderSpace*2;
			
			if(m_prev == NULL || !m_prev->IsVisible())
				m_rcDockPos.left = 0;
			else
				m_rcDockPos.left = m_prev->m_rcDockPos.right;

			//当HIWORD(m_nZoom)==1时表示由最大化变为正常，此时尺寸由最大化尺寸变为
			//正常尺寸，与后面的停靠栏有空挡，不能扩展尺寸变回去
			if(m_nZoom != SW_MINIMIZE && HIWORD(m_nZoom) == 0)
			{
				if(m_next == NULL)
					rc.right = m_pSite->m_rcPos.right - m_pSite->m_rcPos.left - m_GripperSpace - m_BorderSpace*2 + nAddSpace - m_rcDockPos.left;
				else
					rc.right = m_next->m_rcDockPos.left - m_GripperSpace - m_BorderSpace*2 + nAddSpace - m_rcDockPos.left;
			}
			else
				m_nZoom &= 0x0000FFFF;

			//客户区尺寸
			OffsetRect(&rc,m_rcDockPos.left + m_BorderSpace + m_GripperSpace,
				m_rcDockPos.top + m_BorderSpace);
			CopyRect(&m_rcClient,&rc);

			int w = m_rcClient.right - m_rcClient.left;
			int h = RING_SPLITERSIZE * 2;
			h = max(w,h);
			m_rcClient.right = m_rcClient.left + h;
						
			m_rcDockPos.right = m_rcClient.right + m_BorderSpace;
			m_rcDockPos.bottom = m_rcClient.bottom + m_BorderSpace;			
		}
		return TRUE;
	}
	return FALSE;
}

BOOL RingSizeBar::AdjustChildV(int nAddSpace)
{
	if(m_child && m_pSite && m_bVisible)
	{
		int nStep = 0;
		RECT rc;
		GetWindowRect(m_child->Handle(),&rc);
		OffsetRect(&rc,-rc.left,-rc.top);
/*
		if(m_prev == NULL && m_next == NULL)
		{
			m_winpos.y = 0;
			rc.bottom = m_pSite->m_winpos.cy - m_GripperSpace - m_BorderSpace*2;
		}
*/		
		if(m_lineInfo)
		{
			//横向尺寸
			m_rcDockPos.left = m_lineInfo->m_pos;
			m_nSize = rc.right + m_BorderSpace*2;
			
			if(m_prev == NULL || !m_prev->IsVisible())
				m_rcDockPos.top = 0;
			else
				m_rcDockPos.top = m_prev->m_rcDockPos.bottom;

			//当HIWORD(m_nZoom)==1时表示由最大化变为正常，此时尺寸由最大化尺寸变为
			//正常尺寸，与后面的停靠栏有空挡，不能扩展尺寸变回去
			if(m_nZoom != SW_MINIMIZE && HIWORD(m_nZoom) == 0)
			{
				if(m_next == NULL)
					rc.bottom = m_pSite->m_rcPos.bottom - m_pSite->m_rcPos.top - m_GripperSpace - m_BorderSpace*2 + nAddSpace - m_rcDockPos.top;
				else
					rc.bottom = m_next->m_rcDockPos.top - m_GripperSpace - m_BorderSpace*2 + nAddSpace - m_rcDockPos.top;
			}
			else
				m_nZoom &= 0x0000FFFF;
			
			//客户区尺寸
			OffsetRect(&rc,m_rcDockPos.left + m_BorderSpace,
					m_rcDockPos.top + m_BorderSpace + m_GripperSpace);
			CopyRect(&m_rcClient,&rc);

			int w = m_rcClient.bottom - m_rcClient.top;
			int h = RING_SPLITERSIZE * 2;//m_GripperSpace + ((RING_SPLITERSIZE + m_BorderSpace) << 1);
			h = max(w,h);			
			m_rcClient.bottom = m_rcClient.top + h;
						
			m_rcDockPos.right = m_rcClient.right + m_BorderSpace;
			m_rcDockPos.bottom = m_rcClient.bottom + m_BorderSpace;
			//if(m_nZoom == SW_RESTORE)
			//	m_nRestoreSize = m_rcClient.right - m_rcClient.left;
		}
		return TRUE;
	}
	return FALSE;
}

void RingSizeBar::DrawGripper(HDC hDC)
{
	if(m_uStyle == TBS_EX_NONE)
		return;

	RECT rc;
	CopyRect(&rc,(LPRECT)&m_rcDockPos);
	InflateRect(&rc,-1,-2);
	int x=0,y=0;
	UINT step = 3,flag = 0;

	if(IsVert())
	{
		rc.bottom = rc.top + 3;
		OffsetRect(&rc,0,1);

		if(m_uStyle & TBS_EX_CANCLOSE)
		{
			//绘制关闭按钮
			DrawFrameControl(hDC,&m_rcClose,DFC_CAPTION,DFCS_CAPTIONCLOSE);
			rc.right -= 17;
			OffsetRect(&rc,0,step);
			rc.left += step;
			step = 0;
		}
		if((m_uStyle & TBS_EX_DOCKABLE))
		{
			if(m_uStyle & TBS_EX_CANCLOSE)
			{
				//可拖动，绘制最小/大化按钮，必须已绘制关闭按钮才绘制
				if(m_lineInfo && m_lineInfo->m_nSizeBarCnt == 1)
					flag = DFCS_INACTIVE;
				
				if(m_nZoom != SW_MAXIMIZE)
					DrawFrameControl(hDC,&m_rcMinMax,DFC_SCROLL,DFCS_SCROLLUP|flag);
				else
					DrawFrameControl(hDC,&m_rcMinMax,DFC_SCROLL,DFCS_SCROLLDOWN|flag);

				rc.right -= 15;
				OffsetRect(&rc,0,step);
				rc.left += step;
			}			

			//绘制把手
			DrawEdge(hDC,&rc,BDR_RAISEDINNER,BF_RECT);
			OffsetRect(&rc,0,4);
			DrawEdge(hDC,&rc,BDR_RAISEDINNER,BF_RECT);
		}
		else
		{
			//绘制文字
			rc.top -= 3;
			rc.bottom += 7;
			SelectObject(hDC,m_font);
			DrawText(hDC,m_szTitle,strlen(m_szTitle),&rc,DT_LEFT|DT_VCENTER|DT_SINGLELINE);
		}
	}
	else
	{
		rc.right = rc.left + 3;
		OffsetRect(&rc,1,0);

		if(m_uStyle & TBS_EX_CANCLOSE)
		{
			DrawFrameControl(hDC,&m_rcClose,DFC_CAPTION,DFCS_CAPTIONCLOSE);
			rc.top += 17;
			OffsetRect(&rc,step,0);
			rc.bottom -= step;
			step = 0;
		}
		if((m_uStyle & TBS_EX_DOCKABLE))
		{
			if(m_uStyle & TBS_EX_CANCLOSE)
			{
				if(m_lineInfo && m_lineInfo->m_nSizeBarCnt == 1)
					flag = DFCS_INACTIVE;
			
				if(m_nZoom != SW_MAXIMIZE)
					DrawFrameControl(hDC,&m_rcMinMax,DFC_SCROLL,DFCS_SCROLLLEFT|flag);
				else
					DrawFrameControl(hDC,&m_rcMinMax,DFC_SCROLL,DFCS_SCROLLRIGHT|flag);

				rc.top += 15;
				OffsetRect(&rc,step,0);
				rc.bottom -= step;
			}

			DrawEdge(hDC,&rc,BDR_RAISEDINNER,BF_RECT);
			OffsetRect(&rc,4,0);
			DrawEdge(hDC,&rc,BDR_RAISEDINNER,BF_RECT);
		}
		else
		{
			//绘制文字
			rc.left -= 3;
			rc.right += 7;
			SelectObject(hDC,m_vfont);
			SetBkMode(hDC,TRANSPARENT);
			TextOut(hDC,rc.left,rc.bottom,m_szTitle,strlen(m_szTitle));			
		}
	}
	if(m_rcSpliter->right != 0)
	{
		DrawEdge(hDC,m_rcSpliter,BDR_RAISEDINNER,BF_RECT);
		DrawEdge(hDC,m_rcSpliter,BDR_RAISEDOUTER,BF_BOTTOMRIGHT);
	}
	if(m_rcSpliter[1].right != 0)
	{
		DrawEdge(hDC,m_rcSpliter+1,BDR_RAISEDINNER,BF_RECT);
		DrawEdge(hDC,m_rcSpliter+1,BDR_RAISEDOUTER,BF_BOTTOMRIGHT);
	}
}

void RingSizeBar::UpdateCaption()
{
	//更新关闭按钮位置
	RingDockBar::UpdateCaption();
	//更新最小/大化按钮位置
	if(IsVert())
	{
		if((m_uStyle & TBS_EX_DOCKABLE) || (m_uStyle & TBS_EX_CANCLOSE))
		{
			//可拖动，不管有无关闭按钮，总有最小/大化按钮
			if((m_uStyle & TBS_EX_CANCLOSE))
			{
				CopyRect(&m_rcMinMax,&m_rcClose);
				OffsetRect(&m_rcMinMax,-14,0);
			}
			else
			{
				m_rcMinMax.left = m_rcDockPos.right - m_rcDockPos.left - 15;
				m_rcMinMax.top = 3;
				m_rcMinMax.right = m_rcDockPos.right - m_rcDockPos.left - 3;
				m_rcMinMax.bottom = 15;
			}

			m_rcCaption.left = 0;
			m_rcCaption.top = 0;
			m_rcCaption.right = m_rcDockPos.right - m_rcDockPos.left;
			m_rcCaption.bottom = 18;
		}
	}
	else
	{
		if((m_uStyle & TBS_EX_DOCKABLE))
		{
			if((m_uStyle & TBS_EX_CANCLOSE))
			{
				CopyRect(&m_rcMinMax,&m_rcClose);
				OffsetRect(&m_rcMinMax,0,14);
			}
			else
			{
				m_rcMinMax.left = 3;
				m_rcMinMax.top = 3;
				m_rcMinMax.right = 15;
				m_rcMinMax.bottom = 15;
			}
			
			m_rcCaption.left = 0;
			m_rcCaption.top = 0;
			m_rcCaption.right = 18;
			m_rcCaption.bottom = m_rcDockPos.bottom - m_rcDockPos.top;
		}
		else
		{
			//不可拖动，检测有无关闭按钮
			if((m_uStyle & TBS_EX_CANCLOSE) && m_vfont == NULL)
			{
				//创建竖向字体
				LOGFONT lf;
				if(GetObject(m_font,sizeof(LOGFONT),&lf))
				{
					lf.lfEscapement = 900;
					m_vfont = CreateFontIndirect(&lf);
				}
				m_rcCaption.left = 0;
				m_rcCaption.top = 0;
				m_rcCaption.right = 18;
				m_rcCaption.bottom = m_rcDockPos.bottom - m_rcDockPos.top;
			}
		}
	}
}

int RingSizeBar::SetDragRect(HWND hWnd,RingDragger* dragger)
{
	if(m_currState ==  TBS_FLAG_FLY)
	{
		dragger->SetDragRect(&m_frc,hWnd);
		return 3;
	}
	else
	{
		RECT rcClient,rcWindow;
		int m;
		m_child->Parent()->GetClientRect(&rcClient);
		MapWindowPoints(m_child->Parent()->Handle(),hWnd,(LPPOINT)&rcClient,2);
		::GetClientRect(m_child->Parent()->Handle(),&rcWindow);
		MapWindowPoints(m_child->Parent()->Handle(),hWnd,(LPPOINT)&rcWindow,2);

		if(::IsVert(m_currState))
		{
			m = (m_vrc.right + m_vrc.left)/2;	//求中线位置
			if((m > rcClient.right && m < rcWindow.right) || 
				(m < rcClient.left && m > rcWindow.left))
			{
				if(m_currState == m_State)
					dragger->SetDragRect(&m_vrc,hWnd);
				else
					dragger->SetDragRect(&m_frc,hWnd);
			}
			else
			{
				//设置整行矩形
				m_rcAllLine.top = rcClient.top;
				m_rcAllLine.bottom =  rcClient.bottom;
				m_rcAllLine.left = m_vrc.left;
				m_rcAllLine.right = m_vrc.right;
				dragger->SetDragRect(&m_rcAllLine,hWnd);
			}
		}
		else
		{
			m = (m_hrc.bottom + m_hrc.top)/2;	//求中线位置
			if((m > rcClient.bottom && m < rcWindow.bottom) || 
				(m < rcClient.top && m > rcWindow.top))
			{
				if(m_currState == m_State)
					dragger->SetDragRect(&m_hrc,hWnd);
				else
					dragger->SetDragRect(&m_frc,hWnd);
			}
			else
			{
				//设置整行矩形
				m_rcAllLine.left = rcWindow.left;
				m_rcAllLine.right =  rcWindow.right;
				m_rcAllLine.top = m_hrc.top;
				m_rcAllLine.bottom = m_hrc.bottom;
				dragger->SetDragRect(&m_rcAllLine,hWnd);
			}
		}
	}
	return 1;
}

void RingSizeBar::Show(int nState)
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
			
			//标记一下，以后显示时根据m_nRestoreSize决定尺寸（同一行/列内有其他SizeBar的情况）
			m_nZoom |= 0x00010000;

			UpdateAllSite();
		}
	}
	else
	{
		m_nZoom = nState;
		if(m_child)
		{
			if(IsVert())
			{
				if(nState == SW_MINIMIZE)
					m_child->SetPos(m_rcClient.left,m_rcClient.top,
										m_rcClient.right - m_rcClient.left,0);
				else if(LOWORD(nState) == SW_RESTORE)
				{
					if(m_nRestoreSize > m_pSite->Height() - (m_GripperSpace + m_BorderSpace)*m_lineInfo->m_nSizeBarCnt)
						m_nRestoreSize = (m_pSite->Height() - (m_GripperSpace + m_BorderSpace)*m_lineInfo->m_nSizeBarCnt)/m_lineInfo->m_nSizeBarCnt;
					m_child->SetPos(m_rcClient.left,m_rcClient.top,
										m_rcClient.right - m_rcClient.left,m_nRestoreSize);
				}
			}
			else
			{
				if(nState == SW_MINIMIZE)
					m_child->SetPos(m_rcClient.left,m_rcClient.top,0,
										m_rcClient.bottom - m_rcClient.top);
				else if(LOWORD(nState) == SW_RESTORE)
				{
					if(m_nRestoreSize > m_pSite->Width() - (m_GripperSpace + m_BorderSpace)*m_lineInfo->m_nSizeBarCnt)
						m_nRestoreSize = (m_pSite->Width() - (m_GripperSpace + m_BorderSpace)*m_lineInfo->m_nSizeBarCnt)/m_lineInfo->m_nSizeBarCnt;
					m_child->SetPos(m_rcClient.left,m_rcClient.top,m_nRestoreSize,
										m_rcClient.bottom - m_rcClient.top);
				}
			}
		}
	}
}

//判断鼠标位于何方，本函数进入前已经过判断鼠标是在本DockBar范围内
UINT RingSizeBar::HitTest(POINT* lpt)
{
	if((m_uStyle & TBS_EX_CANCLOSE) && PtInRect(&m_rcClose,*lpt))
		return HTCLOSE;
	else if(PtInRect(&m_rcMinMax,*lpt))
	{
		if(m_lineInfo && m_lineInfo->m_nSizeBarCnt > 1)
			return HTZOOM;
		else
			return HTNOWHERE;
	}
	else
		return HTCAPTION;
}

void RingSizeBar::StartPushFrameControl(HWND hWnd,RingDragger* dragger,RINGPARAMS& param)
{
	if(m_CurWhere == HTZOOM)
	{
		HDC hDC = GetDC(hWnd);
		if(IsVert())
		{
			if(m_nZoom != SW_MAXIMIZE)
				m_bNCBtDown = DFCS_SCROLLUP|DFCS_PUSHED;
			else
				m_bNCBtDown = DFCS_SCROLLDOWN|DFCS_PUSHED;
		}
		else
		{
			if(m_nZoom != SW_MAXIMIZE)
				m_bNCBtDown = DFCS_SCROLLLEFT|DFCS_PUSHED;
			else
				m_bNCBtDown = DFCS_SCROLLRIGHT|DFCS_PUSHED;
		}
		DrawFrameControl(hDC,&m_rcMinMax,DFC_SCROLL,m_bNCBtDown);
		ReleaseDC(hWnd,hDC);
		SetCapture(hWnd);
	}
	else
		RingDockBar::StartPushFrameControl(hWnd,dragger,param);
}

void RingSizeBar::OnPushHoverFrameControl(HWND hWnd,RingDragger* dragger,LPPOINT lpt)
{
	if(m_CurWhere == HTZOOM)
	{
		HDC hDC = GetDC(hWnd);
		if(PtInRect(&m_rcMinMax,*lpt))
		{
			if((m_bNCBtDown & DFCS_PUSHED) != DFCS_PUSHED)
			{
				m_bNCBtDown |= DFCS_PUSHED;
				DrawFrameControl(hDC,&m_rcMinMax,DFC_SCROLL,m_bNCBtDown);
			}
		}
		else
		{
			if((m_bNCBtDown & DFCS_PUSHED) == DFCS_PUSHED)
			{
				m_bNCBtDown &= ~DFCS_PUSHED;
				DrawFrameControl(hDC,&m_rcMinMax,DFC_SCROLL,m_bNCBtDown);
			}
		}
		ReleaseDC(hWnd,hDC);
	}
	else
		RingDockBar::OnPushHoverFrameControl(hWnd,dragger,lpt);
}

void RingSizeBar::StopPushFrameControl(HWND hWnd,RingDragger* dragger,LPPOINT lpt)
{
	if(PtInRect(&m_rcMinMax,*lpt) && m_CurWhere == HTZOOM)
		OnMinMax();
	else
		RingDockBar::StopPushFrameControl(hWnd,dragger,lpt);
	
	if(m_pSite)
	{
		HDWP hdwp = BeginDeferWindowPos(10);
		if(IsVert())
			m_pSite->BarLineResizeV(hdwp,m_lineInfo,m_pSite->Height());
		else
			m_pSite->BarLineResizeH(hdwp,m_lineInfo,m_pSite->Width());
		EndDeferWindowPos(hdwp);
		InvalidateRect(m_pSite->Handle(),NULL,TRUE);
	}
}

void RingSizeBar::OnMinMax()
{
	int nState;
	if(m_nZoom == SW_MAXIMIZE)
	{
		//nState = SW_RESTORE;
		//Show(SW_RESTORE);
		if(m_child)
		{
			if(IsVert())
			{
				if(m_nRestoreSize > m_pSite->Height() - (m_GripperSpace + m_BorderSpace)*m_lineInfo->m_nSizeBarCnt)
					m_nRestoreSize = (m_pSite->Height() - (m_GripperSpace + m_BorderSpace)*m_lineInfo->m_nSizeBarCnt)/m_lineInfo->m_nSizeBarCnt;
				MoveWindow(m_child->Handle(),m_rcClient.left,m_rcClient.bottom-m_nRestoreSize,
								m_rcClient.right - m_rcClient.left,m_nRestoreSize,TRUE);
				//m_child->SetPos(m_rcClient.left,m_rcClient.bottom-m_nRestoreSize,
				//					m_rcClient.right - m_rcClient.left,m_nRestoreSize);
			}
			else
			{
				if(m_nRestoreSize > m_pSite->Width() - (m_GripperSpace + m_BorderSpace)*m_lineInfo->m_nSizeBarCnt)
					m_nRestoreSize = (m_pSite->Width() - (m_GripperSpace + m_BorderSpace)*m_lineInfo->m_nSizeBarCnt)/m_lineInfo->m_nSizeBarCnt;
				MoveWindow(m_child->Handle(),m_rcClient.right-m_nRestoreSize,m_rcClient.top,
								m_nRestoreSize,m_rcClient.bottom - m_rcClient.top,TRUE);
				//m_child->SetPos(m_rcClient.right-m_nRestoreSize,m_rcClient.top,
				//					m_nRestoreSize,m_rcClient.bottom - m_rcClient.top);
			}
		}
		//标记由最大化变为正常，与后面的停靠栏有空挡，不能扩展尺寸变回去
		m_nZoom = nState = MAKELONG(SW_RESTORE,1);		
	}
	else
	{
		//最小化及正常状态，改变为最大化
		m_nZoom = SW_MAXIMIZE;
		nState = SW_MINIMIZE;
	}
	
	RingDockBar *dockbar = m_prev;
	while(dockbar)
	{
		dockbar->Show(nState);
		dockbar = dockbar->m_prev;
	}
	dockbar = m_next;
	while(dockbar)
	{
		dockbar->Show(nState);
		dockbar = dockbar->m_next;
	}	
}

//LRESULT RingSizeBar::OnNCPaint(RINGPARAMS param)
//{
/*	RECT rc;
		
	HDC hDC = GetWindowDC(m_hWnd);
	ExcludeClipRect(hDC,m_rcClient.left,m_rcClient.top,m_rcClient.right,m_rcClient.bottom);

/*	if(!DrawBkg(hDC))
	{
		GetWindowRect(m_hWnd,&rc);
		OffsetRect(&rc,-rc.left,-rc.top);
		FillRect(hDC,&rc,GetSysColorBrush(COLOR_BTNFACE));
	}
*/
/*	if(m_ExStyle != TBS_EX_NONE && m_State != TBS_FLAG_FLY)
	{
		HDC hMemDC = CreateCompatibleDC(hDC);
		SelectObject(hMemDC,m_hbmCtrl);

		//DrawFrameControl(hDC,&m_rcClose,DFC_CAPTION,DFCS_CAPTIONCLOSE|DFCS_FLAT|DFCS_TRANSPARENT);
		BitBlt(hMemDC,60,0,12,12,hDC,m_rcClose.left,m_rcClose.top,SRCCOPY);
		BitBlt(hDC,m_rcClose.left,m_rcClose.top,12,12,hMemDC,0,0,SRCAND);
		
		if(m_ExStyle == TBS_EX_SIZEABLE)
		{
			char title[40];
			memset(title,0,40);
			GetWindowText(m_hWnd,title,40);
			SetBkMode(hDC,TRANSPARENT);
			if(m_State > TBS_FLAG_DOCKBOTTOM)
			{
				SelectObject(hDC,m_font);
				TextOut(hDC,5,3,title,strlen(title));
			}	
			else
			{
				SelectObject(hDC,m_vfont);
				TextOut(hDC,3,m_winpos.cy - 5,title,strlen(title));
			}		
			DrawEdge(hDC,&m_rcCaption,BDR_RAISEDINNER,BF_RECT);
		}
		else
		{		
			BitBlt(hMemDC,72,0,12,12,hDC,m_rcMinMax.left,m_rcMinMax.top,SRCCOPY);
			BitBlt(hDC,m_rcMinMax.left,m_rcMinMax.top,12,12,hMemDC,m_nZoom,0,SRCAND);
			if(m_State > TBS_FLAG_DOCKBOTTOM)
			{			
				rc.left = 5;
				rc.top = 5;
				rc.right = m_winpos.cx - 35;
				rc.bottom = 8;
				DrawEdge(hDC,&rc,BDR_RAISEDINNER,BF_RECT);
				OffsetRect(&rc,0,5);
			}
			else
			{
				rc.left = 5;
				rc.top = 35;
				rc.right = 8;
				rc.bottom = m_winpos.cy - 3;
				DrawEdge(hDC,&rc,BDR_RAISEDINNER,BF_RECT);
				OffsetRect(&rc,5,0);
			}
			DrawEdge(hDC,&m_rcMinMax,BDR_RAISEDINNER,BF_RECT);
			DrawEdge(hDC,&rc,BDR_RAISEDINNER,BF_RECT);
			DrawEdge(hDC,&m_rcClose,BDR_RAISEDINNER,BF_RECT);
		}	
		
		CopyRect(&rc,(LPRECT)&m_winpos);
		OffsetRect(&rc,-rc.left,-rc.top);
		DrawEdge(hDC,&rc,BDR_SUNKENOUTER,BF_RECT);

		DeleteDC(hMemDC);
	}
	ReleaseDC(m_hWnd,hDC);
*/
//	return 1;	
//}

//LRESULT RingSizeBar::OnNCLButtonDown(RINGPARAMS param)
//{	
/*	LPRECT lprc;
	m_CurWhere = param.wParam;
	int x,x1;

	if(param.wParam == HTCLOSE)
	{
		lprc = &m_rcClose;
		x = 0;
		x1 = 60;
	}
	else if(param.wParam == HTZOOM)
	{
		lprc = &m_rcMinMax;
		x = m_nZoom;
		x1 = 72;
	}
	else
	{
		if(m_ExStyle == TBS_EX_DOCKABLE)
		{
			m_bNCBtDown = 2;
			POINT pt = {param.mousept.x,param.mousept.y};
			ScreenToClient(m_hWnd,&pt);
			RECT rc;
			CopyRect(&rc,(LPRECT)&m_winpos);
			OffsetRect(&rc,-m_winpos.x,-m_winpos.y);
			if(m_State > TBS_FLAG_DOCKBOTTOM)
			{
				OffsetRect(&rc,-3,-18);
				CopyRect(&m_vrc,&rc);
				m_hrc.left = 0;
				m_hrc.right = m_parent->Width();
				m_hrc.top = m_vrc.top;
				m_hrc.bottom = m_hrc.top + m_winpos.cx;
			}
			else
			{
				OffsetRect(&rc,-18,-3);
				CopyRect(&m_hrc,&rc);
				((RingWnd*)m_parent)->GetClientRect(&m_vrc);
				OffsetRect(&m_vrc,0,-m_vrc.top);
				m_vrc.left = m_hrc.left;
				m_vrc.right = m_vrc.left + m_winpos.cy;
			}			
			m_frc.bottom = m_hrc.top + m_winpos.fy;
			m_frc.left = m_hrc.left;
			m_frc.right = m_frc.left + m_winpos.fx;
			m_frc.top = m_hrc.top - BARTOOLY;
			
			//			InitFlyRect();
			m_currState = m_State;
			if(param.wParam == (WPARAM)m_flyBar)
				m_dragger.StartDrag(m_hWnd,pt.x,pt.y,&m_frc,3);
			else
				m_dragger.StartDrag(m_hWnd,pt.x,pt.y,&rc,1);			
		}
		return 0;
	}
	if(m_ExStyle != TBS_EX_NONE)
	{
		m_bNCBtDown = 2;

		HDC hDC = GetWindowDC(m_hWnd);

		HDC hMemDC = CreateCompatibleDC(hDC);
		SelectObject(hMemDC,m_hbmCtrl);

		BitBlt(hDC,lprc->left,lprc->top,12,12,hMemDC,x1,0,SRCCOPY);
		BitBlt(hDC,lprc->left+1,lprc->top+1,12,12,hMemDC,x,0,SRCAND);
		DeleteDC(hMemDC);

		DrawEdge(hDC,lprc,BDR_SUNKENINNER,BF_RECT);
		
		ReleaseDC(m_hWnd,hDC);
			
		SetCapture(m_hWnd);
	}
*/
//	return 0;
//}

//LRESULT RingSizeBar::OnNCMouseMove(RINGPARAMS param)
//{
/*	if(m_ExStyle == TBS_EX_SIZEABLE)
	{
		HDC hDC = GetWindowDC(m_hWnd);
		
		if(param.wParam == HTCLOSE)
		{
			if(m_CurWhere != HTCLOSE)
				DrawEdge(hDC,&m_rcClose,BDR_RAISEDINNER,BF_RECT);

			m_CurWhere = param.wParam;
		}
		else if(param.wParam == HTCAPTION)
			if(m_CurWhere != HTCAPTION)
			{
				m_CurWhere = param.wParam;
				HDC hMemDC = CreateCompatibleDC(hDC);
				SelectObject(hMemDC,m_hbmCtrl);
				BitBlt(hDC,m_rcClose.left,m_rcClose.top,12,12,hMemDC,60,0,SRCCOPY);
				BitBlt(hDC,m_rcClose.left,m_rcClose.top,12,12,hMemDC,0,0,SRCAND);
				DeleteDC(hMemDC);				
			}		
		ReleaseDC(m_hWnd,hDC);		
	}
*/
//	return 0;
//}

//LRESULT RingSizeBar::OnMouseMove(RINGPARAMS param)
//{
/*	if(m_bNCBtDown)
	{
		//因为客户区位置偏移的关系，必须调整鼠标位置
		POINT pt = {param.mousept.x,param.mousept.y};
		
		pt.x += m_rcClient.left;
		pt.y += m_rcClient.top;

		LPRECT lprc;
		int x,x1;
		
		if(m_CurWhere == HTCLOSE)
		{
			lprc = &m_rcClose;
			x = 0;
			x1 = 60;
		}
		else if(m_CurWhere == HTZOOM)
		{
			lprc = &m_rcMinMax;
			x = m_nZoom;
			x1 = 72;
		}
		else
		{
			//拖动矩形框
			return RingDockBar::OnMouseMove(param);
			//return 0;
		}

		HDC hDC = GetWindowDC(m_hWnd);
		HDC hMemDC = CreateCompatibleDC(hDC);
		SelectObject(hMemDC,m_hbmCtrl);

		if(PtInRect(lprc,pt))
		{
			if(m_bNCBtDown != 2)
			{
				m_bNCBtDown = 2;
				BitBlt(hDC,lprc->left,lprc->top,12,12,hMemDC,x1,0,SRCCOPY);
				BitBlt(hDC,lprc->left+1,lprc->top+1,12,12,hMemDC,x,0,SRCAND);
				DrawEdge(hDC,lprc,BDR_SUNKENOUTER,BF_RECT);
			}
		}
		else
		{
			if(m_bNCBtDown != 4)
			{
				m_bNCBtDown = 4;
				BitBlt(hDC,lprc->left,lprc->top,12,12,hMemDC,x1,0,SRCCOPY);
				BitBlt(hDC,lprc->left,lprc->top,12,12,hMemDC,x,0,SRCAND);
				if(m_ExStyle != TBS_EX_NONE)
					DrawEdge(hDC,lprc,BDR_RAISEDINNER,BF_RECT);
			}
		}
		DeleteDC(hMemDC);
		ReleaseDC(m_hWnd,hDC);
	}
*/
//	return 0;
//}

//LRESULT RingSizeBar::OnLButtonUp(RINGPARAMS param)
//{
/*	ReleaseCapture();
	if(m_bNCBtDown == 2)
		if(m_CurWhere == HTCLOSE)
			SendMessage(m_hWnd,WM_SYSCOMMAND,SC_CLOSE,param.lParam);
		else if(m_CurWhere == HTZOOM)
		{
			if(m_nZoom % 24 == 0)
				m_nZoom = RSMAXMIN_ZOOMUP + m_State/2 * 24;
				//SendMessage(m_hWnd,WM_SYSCOMMAND,SC_RESTORE,param.lParam);
			else
				m_nZoom = RSMAXMIN_ZOOMDOWN + m_State/2 * 24;
				//SendMessage(m_hWnd,WM_SYSCOMMAND,SC_MAXIMIZE,param.lParam);
			SetPos(m_winpos.x,m_winpos.y,m_winpos.cx,m_winpos.cy);
		}
		else
			RingDockBar::OnLButtonUp(param);
			//if(m_dragger.IsDragging())
			//	m_dragger.StopDrag(m_hWnd,param.mousept.x,param.mousept.y);
	
	m_bNCBtDown = 0;
*/
//	return 0;
//}

//LRESULT RingSizeBar::OnNCHitTest(RINGPARAMS param)
//{
	//因为客户区位置偏移的关系，必须调整鼠标位置
/*	POINT pt = {param.mousept.x,param.mousept.y};
	ScreenToClient(m_pSite->Handle(),&pt);
	pt.x -= m_winpos.x;
	pt.y -= m_winpos.y;
	
	if(m_State == TBS_FLAG_FLY)
		return HTCLIENT;
	if(PtInRect(&m_rcClose,pt))
		return HTCLOSE;
	if(PtInRect(&m_rcCaption,pt))
		if(m_ExStyle != TBS_EX_NONE && PtInRect(&m_rcMinMax,pt))
			return HTZOOM;
		else
			return HTCAPTION;
	//"标题栏"内的情况判断完毕
	//RECT rc;
	//::GetClientRect(m_hWnd,&rc);
	//InflateRect(&rc,-3,-3);
	if(PtInRect(&m_rcClient,pt))
		return HTCLIENT;
	else
		return HTCAPTION;
*/
//	return 0;
//}

