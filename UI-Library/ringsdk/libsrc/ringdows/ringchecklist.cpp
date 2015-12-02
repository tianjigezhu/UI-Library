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
//原文件名：ringchecklist.cpp
//
//说明：带选择框的ListBox控件控制
//
**********************************************************************/
#define MAKE_SELF_LIB
#define OEMRESOURCE
#include "ringdows.h"

RingCheckList::RingCheckList()
{
	m_hbmp = NULL;
}

RingCheckList::~RingCheckList()
{
	if(m_hbmp)
		DeleteObject(m_hbmp);
}

BOOL RingCheckList::Create(DWORD dwStyle,DWORD dwExStyle,LPCTSTR szInitText,UINT uId,int x,int y,int width,int height)
{
	dwStyle |= (LBS_HASSTRINGS|LBS_OWNERDRAWFIXED|LBS_NOTIFY|WS_VSCROLL);
	
	if(RingControls::Create(dwStyle,dwExStyle,szInitText,uId,x,y,width,height))
	{
		RingControls::SetOwnerDraw(FALSE,(UINT)0xFFFFFFFF);
		if(m_hbmp == NULL)
			CreateCheckBmp(NULL,GetItemHeight());
		return TRUE;
	}
	return FALSE;
}
	
BOOL RingCheckList::Create(UINT uId,int x/*=0*/,int y/*=0*/,int width/*=-999*/,int height/*=-999*/)
{
	DWORD dwExStyle = 0;
	if(width == -999 || height == -999)
		dwExStyle = RTV_FITPARENTSIZE;
	
	return Create(LBS_HASSTRINGS|LBS_OWNERDRAWFIXED|LBS_NOTIFY|WS_VSCROLL,
						dwExStyle,"",uId,x,y,width,height);
}

void RingCheckList::CreateCheckBmp(HDC hDC,int nSize)
{
	if(m_hbmp)
		DeleteObject(m_hbmp);

	BOOL bDelDC = FALSE;
	if(hDC == NULL)
	{
		hDC = GetDC(m_hWnd);
		bDelDC = TRUE;
	}
	m_nCheckSize = nSize;
	m_hbmp = CreateCompatibleBitmap(hDC,nSize*2,nSize);
	
	HBITMAP hbmCheck = LoadBitmap(NULL,(LPTSTR)OBM_CHECK);
	BITMAP bm;	
	HDC hDstDC = CreateCompatibleDC(hDC);
	HDC hSrcDC = CreateCompatibleDC(hDC);
	
	GetObject(hbmCheck,sizeof(BITMAP),&bm);
	
	SelectObject(hSrcDC,hbmCheck);
	SelectObject(hDstDC,m_hbmp);
	
	BitBlt(hDstDC,0,0,nSize*2,nSize,hSrcDC,0,0,WHITENESS);	
	Rectangle(hDstDC,2,2,nSize - 2,nSize - 2);
	Rectangle(hDstDC,nSize + 2,2,nSize*2 - 2,nSize - 2);

	BitBlt(hDstDC,nSize + 3,2,nSize,nSize,hSrcDC,0,0,SRCAND);
	
	DeleteDC(hSrcDC);
	DeleteDC(hDstDC);
	DeleteObject(hbmCheck);
	if(bDelDC)
		ReleaseDC(m_hWnd,hDC);
}

BOOL RingCheckList::Attach(HWND hWnd)
{
	//因为ListBox一旦创建其Style就不可改变，因此这里检测其Style
	//如果不是自绘则需要先Destroy然后再Create	
	DWORD style = GetWindowLong(hWnd,GWL_STYLE);
	style &= ~(LBS_SORT|LBS_MULTIPLESEL|LBS_MULTICOLUMN);
	DWORD st = LBS_HASSTRINGS|LBS_OWNERDRAWFIXED|LBS_NOTIFY;
	if((style & st) != st)
	{
		RECT rc;
		HWND hwnd;
		
		style |=	(st|WS_VSCROLL);
		
		DWORD exstyle = GetWindowLong(hWnd,GWL_EXSTYLE);
		
		GetWindowRect(hWnd,&rc);
		hwnd = ::GetParent(hWnd);
		
		ScreenToClient(hwnd,(LPPOINT)&rc);
		ScreenToClient(hwnd,(LPPOINT)&rc+1);
		
		m_ID = GetWindowLong(hWnd,GWL_ID);
		
		DestroyWindow(hWnd);
		
		if(RingControls::Create(style,exstyle,"",m_ID,
			rc.left,rc.top,rc.right-rc.left,rc.bottom-rc.top))
		{
			RingControls::SetOwnerDraw(FALSE,(UINT)0xFFFFFFFF);
			if(m_hbmp == NULL)
				CreateCheckBmp(NULL,GetItemHeight());
			return TRUE;
		}
	}
	else
		if(RingBaseWnd::Attach(hWnd))
		{
			RingControls::SetOwnerDraw(FALSE,(UINT)0xFFFFFFFF);
			if(m_hbmp == NULL)
				CreateCheckBmp(NULL,GetItemHeight());
			return TRUE;
		}
		
	return FALSE;
}

BOOL RingCheckList::OnDrawItem(LPDRAWITEMSTRUCT lpds)
{
	if(lpds->itemAction & ODA_FOCUS)
		return TRUE;

	RECT rc;	
	LPRINGCHECKLISTDATA lpData = (LPRINGCHECKLISTDATA)lpds->itemData;
	

	CopyRect(&rc,&lpds->rcItem);
	rc.left += m_nCheckSize;
	
	int ndc = SaveDC(lpds->hDC);
	
	SelectObject(lpds->hDC,m_brush);
	HDC hDC = CreateCompatibleDC(lpds->hDC);

	SelectObject(hDC,m_hbmp);

	if(lpData && lpds->itemData != LB_ERR)
		if(lpData->m_bChecked)
			BitBlt(lpds->hDC,0,rc.top,m_nCheckSize,m_nCheckSize,hDC,m_nCheckSize,0,SRCCOPY);
		else
			BitBlt(lpds->hDC,0,rc.top,m_nCheckSize,m_nCheckSize,hDC,0,0,SRCCOPY);

	if((lpds->itemState & ODS_SELECTED))
	{		
		::SetBkColor(lpds->hDC,GetSysColor(COLOR_HIGHLIGHT));
		::SetTextColor(lpds->hDC,GetSysColor(COLOR_HIGHLIGHTTEXT));
		ExtTextOut(lpds->hDC,0,0,ETO_OPAQUE,&rc,NULL,0,NULL);
	}
	else
		FillRect(lpds->hDC,&rc,m_brush);
	
	DrawText(lpds->hDC,GetText(lpds->itemID),GetTextLen(lpds->itemID),&rc,DT_LEFT|DT_SINGLELINE|DT_VCENTER);
	DeleteDC(hDC);
	RestoreDC(lpds->hDC,ndc);
	
	return TRUE;
}

//RingdowProc已对LB_GETITEMDATA和LB_SETITEMDATA进行处理使用户数据只能加到
//LPRINGCHECKLISTDATA的m_data字段，因此需提供原GetData和SetData的行为函数以
//使LPRINGCHECKLISTDATA能绑定到列表子项
LPRINGCHECKLISTDATA RingCheckList::GetPriData(int nIndex)
{
	LONG lpData = CallWindowProc(m_DefProc,m_hWnd,LB_GETITEMDATA,nIndex,0);
	if(lpData != LB_ERR)
		return (LPRINGCHECKLISTDATA)lpData;
	else
		return NULL;
}

BOOL RingCheckList::SetPriData(int nIndex,LPRINGCHECKLISTDATA lpData)
{
	if(lpData)
		return (BOOL)CallWindowProc(m_DefProc,m_hWnd,LB_SETITEMDATA,nIndex,(LPARAM)lpData);
	else
		return FALSE;
}

//检测选择状态
BOOL RingCheckList::IsItemChecked(int nIndex)
{
	if(nIndex == -1)
		nIndex = GetCurSel();

	LPRINGCHECKLISTDATA lpData = GetPriData(nIndex);
	if(lpData)
	{
		try
		{
			return lpData->m_bChecked;
		}
		catch(...)
		{
		}		
	}
	return FALSE;
}

//切换选择/非选择状态，ToggleItemCheck返回现状态，SetItemCheck返回原状态
BOOL RingCheckList::ToggleItemCheck(int nIndex)
{
	BOOL bRet = FALSE;

	if(nIndex == -1)
		nIndex = GetCurSel();
	
	LPRINGCHECKLISTDATA lpData = GetPriData(nIndex);
	if(lpData)
	{
		try
		{
			bRet = !lpData->m_bChecked;
			lpData->m_bChecked = bRet;
			SetPriData(nIndex,lpData);
		}
		catch(...)
		{
		}
	}
	return bRet;
}

BOOL RingCheckList::SetItemCheck(int nIndex,BOOL bCheck)
{
	BOOL bRet = FALSE;
	
	if(nIndex == -1)
		nIndex = GetCurSel();
	
	LPRINGCHECKLISTDATA lpData = GetPriData(nIndex);
	if(lpData)
	{
		try
		{
			bRet = lpData->m_bChecked;
			lpData->m_bChecked = bCheck;
			SetPriData(nIndex,lpData);
		}
		catch(...)
		{
		}
	}
	return bRet;
}

LRESULT RingCheckList::RingdowProc(HWND hWnd,RINGPARAMS param)
{
	switch(param.uMsg)
	{
		case WM_LBUTTONDOWN:
			if(param.mousept.y < GetCount() * GetItemHeight() && 
				param.mousept.x > 0 && param.mousept.x < m_nCheckSize)
			{
				int index = SendMessage(m_hWnd,LB_ITEMFROMPOINT,0,param.lParam);				
				RECT rc;
				ToggleItemCheck(index);				
				GetItemRect(index,&rc);
				InvalidateRect(m_hWnd,&rc,TRUE);
				return 0;
			}
			break;
		case WM_VSCROLL:
		case (WM_MOUSELAST+1)://=WM_MOUSEWHEEL
		{
			LRESULT res = RingControls::RingdowProc(hWnd,param);
			if(param.uMsg == (WM_MOUSELAST+1) || 
				(param.uMsg == WM_VSCROLL && param.wParam == SB_ENDSCROLL))
				InvalidateRect(m_hWnd,NULL,TRUE);
			return res;
		}
		case LB_ADDSTRING:
		case LB_INSERTSTRING:
		{
			LRESULT res = RingControls::RingdowProc(hWnd,param);
			
			if(res != LB_ERR && res != LB_ERRSPACE)
			{
				LPRINGCHECKLISTDATA lpData = (LPRINGCHECKLISTDATA)New(sizeof(RINGCHECKLISTDATA));
				if(lpData)
				{
					lpData->m_bChecked =	param.wParam;
					SetPriData(res,lpData);					
				}				
			}
			return res;
		}
		case LB_DELETESTRING:
		{			
			OnDelString(param.wParam);
			break;
		}
		case LB_RESETCONTENT:
		{
			int n = GetCount();
			for(int i=0;i<n;i++)
				OnDelString(i);
			break;
		}
		case LB_SETITEMDATA:
		{	
			LONG lpData = CallWindowProc(m_DefProc,m_hWnd,LB_GETITEMDATA,param.wParam,0);
			if(lpData != LB_ERR)
				if(lpData == 0)
					break;
				else
				{
					try
					{
						((LPRINGCHECKLISTDATA)lpData)->m_data = param.lParam;
						return TRUE;
					}
					catch(...)
					{						
					}
				}
			return LB_ERR;
		}
		case LB_GETITEMDATA:
		{	
			LONG lpData = CallWindowProc(m_DefProc,m_hWnd,LB_GETITEMDATA,param.wParam,0);
			if(lpData != LB_ERR)
			{
				if(lpData == 0)
					return 0;
				else
				{
					try
					{
						return ((LPRINGCHECKLISTDATA)lpData)->m_data;
					}
					catch(...)
					{
					}
				}
			}
			return LB_ERR;
		}
		//case WM_ERASEBKGND:
		case WM_WINDOWPOSCHANGED:
			return 0;
	}
	return RingControls::RingdowProc(hWnd,param);
}

//删除子项，检查数据RINGCHECKLISTDATA中有无用户设置的数据，有则将该数据
//替换RINGCHECKLISTDATA，释放RINGCHECKLISTDATA，函数返回后执行默认的删除
//子项操作
void RingCheckList::OnDelString(int index)
{
	LONG res = CallWindowProc(m_DefProc,m_hWnd,LB_GETITEMDATA,index,0);
	if(res != LB_ERR)
		if(res != 0)
		{
			LPRINGCHECKLISTDATA lpData = (LPRINGCHECKLISTDATA)res;
			if(lpData)
			try
			{
				if(lpData->m_data)
				{
					CallWindowProc(m_DefProc,m_hWnd,LB_SETITEMDATA,index,lpData->m_data);
					Del(lpData);
				}
			}
			catch(...)
			{
			}
		}
}

LRESULT RingCheckList::OnDestroy(RINGPARAMS& param)
{
	Clear();
	return DefaultProc(param);
}


