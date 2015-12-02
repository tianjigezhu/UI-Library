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
//原文件名：ringbutton.cpp
//
//说明：Button控件控制
//
**********************************************************************/
#define MAKE_SELF_LIB
#include "ringdows.h"

RingButton::RingButton()
{
	m_windowType = ITIS_BUTTON;
	m_bNeedFreeImage = FALSE;
	m_nImgWhere = -1;
	m_bPushed = FALSE;
}

RingButton::~RingButton()
{
	
}

void RingButton::Click()
{
	SetActiveWindow(m_parent->Handle());
	SendMessage(m_hWnd,BM_CLICK,0,0);
}

void RingButton::SetExStyle(DWORD dwExStyle)
{
	m_ExStyle = dwExStyle;

	if(m_ExStyle)
		SetOwnerDraw(m_bOwnerDraw);
}

LRESULT RingButton::OnBkColor(UINT uMsg,HDC hDC)
{
	switch(uMsg)
	{
	case WM_CTLCOLORSTATIC:
	case WM_CTLCOLORBTN:
		;
		//return SetColors(hDC);			
	}
	return 0;
}

LRESULT RingButton::OnDestroy(RINGPARAMS& param)
{
	if(m_bNeedFreeImage)
	{
		HBITMAP hbm = GetBitmap();
		if(hbm)
			DeleteObject(hbm);
	}
	return DefaultProc(param);
}

BOOL RingButton::Check(UINT uCheck/*=BST_CHECKED*/,UINT id/*=0*/)
{
	if(id == 0)
		id = m_ID;
	return CheckDlgButton(m_parent->Handle(),id,uCheck);
}

UINT RingButton::IsChecked(UINT uid/*=0*/)
{
	if(uid == 0)
		uid = m_ID;
	return IsDlgButtonChecked(m_parent->Handle(),uid);
}

BOOL RingButton::SetBkColor(COLORREF crColor)
{
	SetOwnerDraw(m_bOwnerDraw);
	return RingControls::SetBkColor(crColor);
}

void RingButton::SetTextColor(COLORREF crColor)
{
	SetOwnerDraw(m_bOwnerDraw);
	RingControls::SetTextColor(crColor);
}

HICON RingButton::SetIcon(HICON hicon)
{
	if(hicon)
	{
		if(m_bNeedFreeImage)
		{
			HBITMAP hbm = GetBitmap();
			if(hbm)
				DeleteObject(hbm);
			m_bNeedFreeImage = FALSE;
		}

		if((GetStyle() & BS_ICON) == 0)
			ReplaceStyle(BS_BITMAP,BS_ICON);
		return (HICON)SendMessage(m_hWnd,BM_SETIMAGE,IMAGE_ICON,(LPARAM)hicon);
	}
	else
		return NULL;
}

HICON RingButton::SetIcon(UINT uidIcon,int width/*=0*/,int height/*=0*/,HINSTANCE hInst)
{
	HICON hicon = (HICON)LoadImage(
							hInst,
							MAKEINTRESOURCE(uidIcon),
							IMAGE_ICON,
							width,height,
							LR_DEFAULTSIZE);
	return SetIcon(hicon);//LoadIcon(m_hInst,MAKEINTRESOURCE(uidIcon)));
}

HBITMAP RingButton::SetBitmap(HBITMAP hbm)
{
	if(hbm)
	{
		if((GetStyle() & BS_BITMAP) == 0)
			ReplaceStyle(BS_ICON,BS_BITMAP);
		
		HBITMAP hbm1 = (HBITMAP)SendMessage(m_hWnd,BM_SETIMAGE,IMAGE_BITMAP,(LPARAM)hbm);
		if(m_bNeedFreeImage && hbm1)
		{
			//原来的BMP是由本类调入，必须释放
			DeleteObject(hbm1);
			hbm1 = NULL;
			m_bNeedFreeImage = FALSE;
		}
		return hbm1;
	}
	else
		return NULL;
}

HBITMAP RingButton::SetBitmap(UINT uidBmp,HINSTANCE hInst)
{
	HBITMAP hbm1,hbm = LoadBitmap(hInst,MAKEINTRESOURCE(uidBmp));
	
	hbm1 = SetBitmap(hbm);
	if(hbm)
		m_bNeedFreeImage = TRUE;

	return hbm1;
}

HICON RingButton::AddIcon(HICON hicon,int nWhere/* = ATBTN_LEFT*/)
{
	if(hicon)
	{
		HICON icon = SetIcon(hicon);
		m_nImgWhere = nWhere;

		SetOwnerDraw(m_bOwnerDraw);
		
		return icon;
	}
	return NULL;
}

HICON RingButton::AddIcon(UINT uidIcon,int nWhere/*=ATBTN_LEFT*/,
								  int width/*=0*/,int height/*=0*/,HINSTANCE hInst)
{
	HICON hicon = (HICON)LoadImage(
		hInst,
		MAKEINTRESOURCE(uidIcon),
		IMAGE_ICON,
		width,height,
		LR_DEFAULTSIZE);
	return AddIcon(hicon,nWhere);
}

HBITMAP RingButton::AddBitmap(HBITMAP hbm,int nWhere/* = ATBTN_LEFT*/)
{
	if(hbm)
	{
		HBITMAP bmp = SetBitmap(hbm);
		m_nImgWhere = nWhere;
		
		SetOwnerDraw(m_bOwnerDraw);
		
		return bmp;
	}
	return NULL;
}

HBITMAP RingButton::AddBitmap(UINT uidBmp,int nWhere/*=ATBTN_LEFT*/,HINSTANCE hInst)
{
	HBITMAP hbm1,hbm = LoadBitmap(hInst,MAKEINTRESOURCE(uidBmp));
	
	hbm1 = AddBitmap(hbm,nWhere);
	if(hbm)
		m_bNeedFreeImage = TRUE;
	
	return hbm1;	
}

BOOL RingButton::OnDrawItem(LPDRAWITEMSTRUCT lps)
{	
	if(lps->CtlType != ODT_BUTTON)
		return FALSE;

	DWORD style = GetStyle();
	
	//设置文字，背景色及绘制模式
	SetColors(lps->hDC);
	//背景
	if(m_brush == NULL)
		m_brush = CreateSolidBrush(::GetBkColor(lps->hDC));

	FillRect(lps->hDC,&lps->rcItem,m_brush);
	//边框
	DrawFrame(lps,style);
	//检测文字包围矩形
	RECT rc;
	LPCTSTR s = GettextRect(lps,style,&rc);
	//图标或BMP
	DrawImage_Text(lps,s,style,&rc);

	if(lps->itemAction == ODA_SELECT)
	{
		if(lps->itemState & ODS_SELECTED)
			SendMessage(GetParent(),WM_COMMAND,MAKELONG(m_ID,BN_PUSHED),(LPARAM)m_hWnd);
		else
			SendMessage(GetParent(),WM_COMMAND,MAKELONG(m_ID,BN_UNPUSHED),(LPARAM)m_hWnd);
	}
	return TRUE;
}

void RingButton::DrawFrame(LPDRAWITEMSTRUCT lps,DWORD style)
{
	UINT state = GetState();
	if((m_ExStyle & RINGBTN_EX_FLAT) == 0  || IsPushed() || ((m_ExStyle & RINGBTN_EX_FLAT) && (state & BST_PUSHED)))
	{		
		DrawEdge(lps->hDC,&lps->rcItem,
			((state & BST_PUSHED) == BST_PUSHED || IsPushed())?EDGE_SUNKEN:EDGE_RAISED,
			BF_RECT | ((state & BS_FLAT) == BS_FLAT?BF_FLAT:0));
	}
}

LPCTSTR RingButton::GettextRect(LPDRAWITEMSTRUCT lps,DWORD style,LPRECT lprc)
{	
	//检测字体大小
	LPCTSTR s = GetText();
	int offsetx = WINVAR_EDGE_X * 2;
	if(lps)
	{
		if(s && lprc)
		{
			CopyRect(lprc,&lps->rcItem);
			InflateRect(lprc,-offsetx,-WINVAR_BORDER_Y);

			if((style & BS_MULTILINE))
			{
				UINT flag = DT_CALCRECT | DT_WORDBREAK | DT_EDITCONTROL;
			
				if((style & BS_CENTER))
					flag |= DT_CENTER;
			
				if((style & BS_VCENTER))
					flag |= DT_VCENTER;
			
				DrawTextEx(lps->hDC,(LPSTR)s,strlen(s),lprc,flag,NULL);
			}
			else
			{
				SIZE size;
				
				GetTextExtentPoint32(lps->hDC,s,strlen(s),&size);

				InflateRect(lprc,
					(size.cx - (lprc->right - lprc->left))/2,
					(size.cy - (lprc->bottom - lprc->top))/2);

				//BS_CENTER = BS_LEFT | BS_RIGHT
				//所以拿style & BS_CENTER判断
				if((style & BS_CENTER) == BS_RIGHT)
					offsetx = lps->rcItem.right - lprc->right - offsetx;
				else if((style & BS_CENTER) == BS_LEFT)
					offsetx = lps->rcItem.left - lprc->left + offsetx;
				else
					offsetx = 0;

				OffsetRect(lprc,offsetx,0);				
			}		
		}
	}
	return s;
}

void RingButton::DrawImage_Text(LPDRAWITEMSTRUCT lps,LPCTSTR s,DWORD style,LPRECT lprc)
{
	UINT state = GetState();
	int x,y,ofx=0,ofy=0;
	int offsetx = WINVAR_EDGE_X * 2;
	BOOL bImgDraw = FALSE;

	if((style & (BS_ICON | BS_BITMAP)))
	{
		ICONINFO pi;
		HANDLE hImg;
		
		bImgDraw = TRUE;

		memset(&pi,0,sizeof(ICONINFO));

		if((style & BS_ICON))
		{
			hImg = (HANDLE)GetIcon();
			pi.fIcon = FALSE;
			GetIconInfo((HICON)hImg,&pi);
			
			pi.xHotspot <<= 1;
			pi.yHotspot <<= 1;

			if(pi.hbmColor)
				DeleteObject(pi.hbmColor);
			if(pi.hbmMask)
				DeleteObject(pi.hbmMask);			
		}
		else
		{
			hImg = (HANDLE)GetBitmap();
			pi.fIcon = FALSE;
			
			BITMAP bmp;
			GetObject((HBITMAP)hImg,sizeof(BITMAP),&bmp);
			pi.xHotspot = bmp.bmWidth;
			pi.yHotspot = bmp.bmHeight;
		}
				
		if(m_nImgWhere == ATBTN_LEFT)
		{
			ofy = 0;
			y = (int)(lps->rcItem.bottom + lps->rcItem.top - pi.yHotspot)/2;
			x = lps->rcItem.left + offsetx;

			ofx = x + pi.xHotspot + 4 - lprc->left;
			if(ofx < 0)
			{
				//文字与图标无重叠
				ofx = 0;
			}
			/*
			//图标始终跟随在文字左边的代码
			if((style & BS_CENTER) == BS_LEFT)
				ofx = pi.xHotspot * 2 + offsetx;
			else if((style & BS_CENTER) == BS_RIGHT)
				ofx = 0;
			else
				ofx = pi.xHotspot + offsetx/2;
				
			x = lprc->left + ofx - pi.xHotspot * 2 - offsetx;
			*/
		}
		else if(m_nImgWhere == ATBTN_RIGHT)
		{
			ofy = 0;
			y = (int)(lps->rcItem.bottom + lps->rcItem.top - pi.yHotspot)/2;
			//x = lps->rcItem.right - pi.xHotspot * 2 - offsetx;

			if((style & BS_CENTER) == BS_RIGHT)
				ofx = -(int)(pi.xHotspot + offsetx);
			else if((style & BS_CENTER) == BS_LEFT)
				ofx = 0;
			else
				ofx = -(int)(pi.xHotspot + offsetx)/2;
			x = lprc->right + ofx + offsetx;
		}
		else if(m_nImgWhere == ATBTN_TOP)
		{
			ofx = 0;
			x = (int)(lps->rcItem.right + lps->rcItem.left - pi.xHotspot)/2;
			ofy = pi.yHotspot/2 + WINVAR_EDGE_Y;
			y = lprc->top + ofy - pi.yHotspot - WINVAR_EDGE_Y;
		}
		else if(m_nImgWhere == ATBTN_BOTTOM)
		{
			ofx = 0;
			x = (lps->rcItem.right + lps->rcItem.left - pi.xHotspot)/2;
			ofy = -(int)(pi.yHotspot/2 + WINVAR_EDGE_Y);
			y = lprc->bottom + ofy + WINVAR_EDGE_Y;
		}
		else
		{
			switch(style & BS_CENTER)
			{
				case BS_LEFT:
					x = lps->rcItem.left;
					break;
				case BS_RIGHT:
					x = lps->rcItem.right - pi.xHotspot;
					break;
				default:
					x = (int)(lps->rcItem.right + lps->rcItem.left - pi.xHotspot)/2;
			}

			switch(style & BS_VCENTER)
			{
				case BS_TOP:
					y = lps->rcItem.top;
					break;
				case BS_BOTTOM:
					y = lps->rcItem.bottom - pi.yHotspot;
					break;
				default:
					y = (int)(lps->rcItem.bottom + lps->rcItem.top - pi.yHotspot)/2;
			}
		}
		
		if((state & BST_PUSHED) || IsPushed())
		{
			x ++;
			y ++;
		}

		if(hImg)
		{
			if(pi.fIcon)
				DrawIconEx(lps->hDC,x,y,(HICON)hImg,pi.xHotspot,pi.yHotspot,0,0,DI_NORMAL);
			else
			{
				HDC hDC = CreateCompatibleDC(lps->hDC);
				SelectObject(hDC,hImg);
				BitBlt(lps->hDC,x,y,pi.xHotspot,pi.yHotspot,hDC,0,0,SRCCOPY);
				DeleteDC(hDC);
			}
		}
	}


	if((state & BST_PUSHED))
		OffsetRect(lprc,ofx+1,ofy+1);
	else
		OffsetRect(lprc,ofx,ofy);

	if(s && (m_nImgWhere != -1 || !bImgDraw))
		TextOut(lps->hDC,lprc->left,lprc->top,s,strlen(s));

	if(lps->itemState & ODS_FOCUS && (m_ExStyle & RINGBTN_EX_NOFOCUSRECT) == 0)
	{
		RECT rc;
		CopyRect(&rc,&lps->rcItem);
		InflateRect(&rc,-4,-4);
		if(m_crTextColor != 0)
			::SetTextColor(lps->hDC,0);
		DrawFocusRect(lps->hDC,&rc);
	}
}

//////////////////////////////////////////////////////
//
//	RingCheckBox
//
//////////////////////////////////////////////////////
BOOL RingCheckBox::Create(DWORD dwStyle,DWORD dwExStyle,LPCTSTR szInitText,UINT uId,int x,int y,int width,int height)
{
	m_hWnd = RingCtrlWnd::Create(
		szInitText,
		"button",
		WS_CHILD | WS_VISIBLE | BS_CHECKBOX | dwStyle,
		0,dwExStyle,
		(HMENU)uId,x,y,width,height);
   return (BOOL)m_hWnd;
}

//////////////////////////////////////////////////////
//
//	RingRadioButton
//
//////////////////////////////////////////////////////
BOOL RingRadioButton::Create(DWORD dwStyle,DWORD dwExStyle,LPCTSTR szInitText,UINT uId,int x,int y,int width,int height)
{
	m_hWnd = RingCtrlWnd::Create(
		szInitText,
		"button",
		WS_CHILD | WS_VISIBLE | BS_RADIOBUTTON | dwStyle,
		0,dwExStyle,
		(HMENU)uId,x,y,width,height);
   return (BOOL)m_hWnd;
}
	
BOOL RingRadioButton::Check(UINT id/*=0*/,UINT first/*=0*/,UINT last/*=0*/)
{
	if(id == 0)
		id = m_ID;
	if(first !=0 && last !=0)
		SetGroup(first,last);

	return CheckRadioButton(m_parent->Handle(),m_first,m_last,id);
}

void RingRadioButton::UnCheck(UINT id/*=0*/)
{
	HWND hWnd;
	if(id == 0)
		hWnd = m_hWnd;
	else
		hWnd = GetDlgItem(m_parent->Handle(),id);

	::SendMessage(hWnd,BM_SETCHECK,BST_UNCHECKED,0);
}

UINT RingRadioButton::IsChecked(UINT uid/*=0*/)
{
	if(uid == 0)
		uid = m_ID;
	return IsDlgButtonChecked(m_parent->Handle(),uid);
}
