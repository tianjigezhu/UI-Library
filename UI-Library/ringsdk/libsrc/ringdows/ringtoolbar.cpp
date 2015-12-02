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

RingToolBar::RingToolBar()
{
	InitCommonControls();

	m_windowType = ITIS_TOOLBAR;
	m_btnSize.cx = m_btnSize.cy = 0;
	m_BarSize.cx = m_BarSize.cy = 0;
	m_fSize.cx = m_fSize.cy = 0;
	m_nMaxRow = 0;
	m_nfRows = 1;
	m_dockbar = NULL;
	m_img = m_imgHot = NULL;
}

RingToolBar::~RingToolBar()
{
	if(m_img)
		ImageList_Destroy(m_img);
	if(m_imgHot)
		ImageList_Destroy(m_imgHot);
}

void RingToolBar::ReplaceStyle(DWORD dwsRemove,DWORD dwsReplace)
{
	DWORD style = GetStyle();
   style &= ~dwsRemove;
   style |= dwsReplace;
   SetStyle(style);
}

RingDockBar* RingToolBar::EnableDock(LPCTSTR szTitle,UINT uDockStyle,UINT uDockFlag,LPRECT lprcPos)
{
	DWORD style = CCS_NORESIZE;
	if(!MASK_MATCH(uDockStyle,TBS_EX_NONE))
		style |= TBSTYLE_FLAT;
	ReplaceStyle(0,style);
	//如果是非FLAT类型，顶端会留有空间以便绘制横线，
	//必须调用一下SetButtonSize才会取消该空间
	DWORD dw = GetButtonSize();
	SetButtonSize(LOWORD(dw),HIWORD(dw));
	return RingBaseWnd::EnableDock(szTitle,uDockStyle,uDockFlag,lprcPos);
}

///////////////////////////////////////////////////////////////////////
//
//重载RingControls的Create，szInitText参数为图象资源，该资源图片
//必须在本模块资源中或是BMP文件，并且按钮图片必须宽高一致紧密排列
//成一行或是16*15标准的TOOLBAR按钮图片（同资源编辑工具栏生成的图片，但
//不局限于16色），图片0,0位置像素必须是透明色
//
///////////////////////////////////////////////////////////////////////
BOOL RingToolBar::Create(DWORD dwStyle,DWORD dwExStyle,LPCTSTR szInitText,UINT uId,
								 int x,int y,int width,int height)
{
	if(IsWindow())
		return TRUE;

	//父窗口不能是控件
	if(m_parent->IsControls())
		return FALSE;

	RingDockSite* pSite;
		
	if(m_parent->GetWindowType() != ITIS_DOCKSITE)
		pSite = ((RingWnd*)m_parent)->CreateDockSite(TBS_FLAG_DOCKTOP);
	else
		pSite = (RingDockSite*)m_parent;

	if(pSite == NULL)
		return FALSE;

	if(dwStyle & TBSTYLE_FLAT)
		dwStyle |= CCS_NORESIZE;

	dwStyle |= WS_CHILD | WS_VISIBLE | TBSTYLE_BUTTON | WS_CLIPCHILDREN | 
		WS_CLIPSIBLINGS | TBSTYLE_TOOLTIPS | CCS_NODIVIDER | CCS_NOMOVEY;

	RingControls::Create(dwStyle,0,"",uId,x,y,width,height);
	//设置WM_NOTIFY消息接收窗口为pSite
	SendMessage(m_hWnd,TB_SETPARENT,(WPARAM)pSite->Handle(),0);
	SendMessage(m_hWnd,TB_BUTTONSTRUCTSIZE,sizeof(TBBUTTON),0);
	//允许多个IMAGELIST
	SendMessage(m_hWnd,CCM_SETVERSION,5,0);
	SetExStyle(dwExStyle);

	if(szInitText)
	{
		HBITMAP hbm;
		if(IS_INTRESOURCE(szInitText))
			hbm = (HBITMAP)LoadBitmap(GetInstance(),szInitText);
		else
			hbm = (HBITMAP)LoadImage(GetInstance(),szInitText,IMAGE_BITMAP,0,0,LR_LOADFROMFILE);
		if(hbm)
		{
			SIZE size;
			int ncnt;

			GetBmpSize(hbm,&size);
			ncnt = size.cx;

			if(size.cy != 15)
				size.cx = size.cy;
			else
				size.cx = 16;
			
			m_img = ImageList_Create(size.cx,size.cy,ILC_COLOR32|ILC_MASK,ncnt/size.cx,4);
			ImageList_AddMasked(m_img,hbm,CLR_DEFAULT);

			DeleteObject(hbm);
			SetImageList(m_img);
			SetButtonSize(size.cx,size.cy);			
		}		
	}
	m_dockbar = RingBaseWnd::EnableDock(NULL,TBS_EX_NONE|TBS_EX_ALLLINE);
	//创建时为整行工具栏，如果是非FLAT类型，需要先取消CCS_NORESIZE类型，
	//EnableDock时dockbar获取到正确高度后再设置以便能控制工具栏位置
	ReplaceStyle(0,CCS_NORESIZE);
	return (BOOL)m_hWnd;
}

//idBitmap：该图象只能在本模块资源中
BOOL RingToolBar::Create(int idBitmap,DWORD dwStyle/*=0*/,DWORD dwExStyle/*=0*/)
{
   if(idBitmap > 0)
		return Create(dwStyle,dwExStyle,MAKEINTRESOURCE(idBitmap),gdwDockBarId++,0,0,0,0);
	else
		return Create(dwStyle,dwExStyle,NULL,gdwDockBarId++,0,0,0,0);
}

void RingToolBar::SetExStyle(DWORD dwExStyle)
{
	if(dwExStyle != 0 && m_hWnd)
	{
		//Toolbar专有扩展属性必须用以下方法设置，
		DWORD dwStyle = SendMessage(m_hWnd,TB_GETEXTENDEDSTYLE,0,0);
		dwStyle |= dwExStyle;
		SendMessage(m_hWnd,TB_SETEXTENDEDSTYLE,0,dwStyle);
	}
}

/*******************************************************
//
//LoadToolbar:	调入工具栏资源，因为需要知道资源格式，
//					以下代码部分摘自MFC
//
*******************************************************/
struct CToolBarData
{
	WORD wVersion;
	WORD wWidth;
	WORD wHeight;
	WORD wItemCount;
	//WORD aItems[wItemCount]
	
	WORD* items()
	{ return (WORD*)(this+1); }
};

BOOL RingToolBar::LoadToolBar(UINT uid,DWORD dwStyle,DWORD dwExStyle,HINSTANCE hInst)
{
	return LoadToolBar(MAKEINTRESOURCE(uid),dwStyle,dwExStyle,hInst);
}

BOOL RingToolBar::LoadToolBar(LPCTSTR lpszResourceName,DWORD dwStyle,DWORD dwExStyle,HINSTANCE hInst)
{
	if(lpszResourceName == NULL)
		return FALSE;
	
	if(m_hWnd == NULL)
		if(!Create(0,dwStyle,dwExStyle))
			return FALSE;
		
	HRSRC hRsrc = FindResource(hInst,lpszResourceName,RT_TOOLBAR);
	if(hRsrc == NULL)
		return FALSE;
	
	HGLOBAL hGlobal = LoadResource(hInst, hRsrc);
	if(hGlobal == NULL)
		return FALSE;
	
	CToolBarData* pData = (CToolBarData*)LockResource(hGlobal);
	if(pData == NULL || pData->wVersion != 1)
		return FALSE;
	
	BOOL bResult = FALSE;
	HBITMAP hbm = LoadBitmap(hInst,lpszResourceName);
	if(hbm)
	{
		bResult = TRUE;
		AddImage(hbm,pData->wWidth,pData->wHeight);
		DeleteObject(hbm);
	}

	DeleteAllButtons();

	TBBUTTON tb;
	memset(&tb,0,sizeof(TBBUTTON));
	tb.iString = -1;
	int iImg = 0;

	for(int i = 0; i < pData->wItemCount; i++)
	{
		tb.fsState = TBSTATE_ENABLED;
		if((tb.idCommand = pData->items()[i]) == 0)
		{
			// separator
			tb.fsStyle = TBSTYLE_SEP;
			// width of separator includes 8 pixel overlap
			
			if ((GetStyle() & TBSTYLE_FLAT))
				tb.iBitmap = 6;
			else
				tb.iBitmap = 8;
		}
		else
		{
			// a command button with image
			tb.fsStyle = TBSTYLE_BUTTON;
			tb.iBitmap = iImg ++;
		}
		if(!SendMessage(m_hWnd,TB_ADDBUTTONS,1,(LPARAM)&tb))
			return FALSE;
	}
	
	SetButtonSize(pData->wWidth + 7, pData->wHeight + 7);
	
	UnlockResource(hGlobal);
	FreeResource(hGlobal);

	Update(pData->wItemCount);

	return bResult;
}

//删除所有按钮
void RingToolBar::DeleteAllButtons()
{
	int nCount = SendMessage(m_hWnd,TB_BUTTONCOUNT,0,0);
	while(nCount--)
		SendMessage(m_hWnd,TB_DELETEBUTTON,0,0);
}

///////////////////////////////////////////////////
//
//加入或设置按钮图片，未设置，创建m_img并加入，已设置，
//加入到m_img
//
///////////////////////////////////////////////////
int RingToolBar::AddImage(UINT id,int cx/*=16*/,int cy/*=15*/,
									COLORREF crMask/*=CLR_DEFAULT*/,
									HINSTANCE hInst/*=GetInstance()*/)
{
	HBITMAP hbm = LoadBitmap(hInst,MAKEINTRESOURCE(id));
	int nIdx = AddImage(hbm,cx,cy,crMask);
	DeleteObject(hbm);
	return nIdx;
}

int RingToolBar::AddImage(HBITMAP hbm,int cx/*=15*/,int cy/*=16*/,
									COLORREF crMask/*=CLR_DEFAULT*/)
{
	return AddInterImage(hbm,cx,cy,crMask,m_img,TB_GETIMAGELIST,TB_SETIMAGELIST,TRUE);
}

int RingToolBar::AddHotImage(UINT id,int cx/*=16*/,int cy/*=15*/,
									  COLORREF crMask/*=CLR_DEFAULT*/,
									  HINSTANCE hInst/*=GetInstance()*/)
{
	HBITMAP hbm = LoadBitmap(hInst,MAKEINTRESOURCE(id));
	int nIdx = AddHotImage(hbm,cx,cy,crMask);
	DeleteObject(hbm);
	return nIdx;
}

int RingToolBar::AddHotImage(HBITMAP hbm,int cx/*=15*/,int cy/*=16*/,
								  COLORREF crMask/*=CLR_DEFAULT*/)
{
	return AddInterImage(hbm,cx,cy,crMask,m_imgHot,TB_GETHOTIMAGELIST,TB_SETHOTIMAGELIST,FALSE);
}

int RingToolBar::AddInterImage(HBITMAP hbm,int cx,int cy,COLORREF crMask,
										 HIMAGELIST& imgInter,UINT uMsgGet,UINT uMsgSet,
										 BOOL bSetButtonSize)
{
	int nIdx = -1;
	if(hbm)
	{
		HIMAGELIST himl = (HIMAGELIST)::SendMessage(m_hWnd,uMsgGet,0,0);
		
		if(himl)	  //已存在并已设置
			nIdx = ImageList_AddMasked(himl,hbm,crMask);
		else
		{
			SIZE size;
			int ncnt;
			GetBmpSize(hbm,&size);
			ncnt = size.cx/cx;
			
			if(bSetButtonSize)
				SetButtonSize(cx,cy);

			if(imgInter == NULL)
				imgInter = ImageList_Create(cx,cy,ILC_COLOR32|ILC_MASK,ncnt,4);
			//imgInter(m_img或m_imgHot有可能已创建但未设置进TOOLBAR，因此这里不能用else
			if(imgInter)
			{
				nIdx = ImageList_AddMasked(imgInter,hbm,crMask);
				himl = imgInter;
			}
		}
		::SendMessage(m_hWnd,uMsgSet,0,(LPARAM)himl);
	}
	return nIdx;
}

BOOL RingToolBar::AddButtons(int num,LPCTBBUTTON lptb)
{
	if(lptb == NULL)
		return FALSE;

	int cnt = GetButtonCount();
	
	TBBUTTON tb = *lptb;

	if(m_dockbar && m_dockbar->IsVert() && lptb)
		tb.fsState |= TBSTATE_WRAP;
	
	BOOL bOK = SendMessage(m_hWnd,TB_ADDBUTTONS,num,(LPARAM)&tb);
   Update(cnt);

   return bOK;
}

BOOL RingToolBar::InsertButton(int index,LPCTBBUTTON lptb)
{
	if(lptb == NULL)
		return FALSE;

	int cnt = GetButtonCount();

	TBBUTTON tb = *lptb;

	if(m_dockbar && m_dockbar->IsVert() && lptb)
		tb.fsState |= TBSTATE_WRAP;

	BOOL bOk = SendMessage(m_hWnd,TB_INSERTBUTTON,index,(LPARAM)&tb);
   Update(cnt);
   return bOk;
}

BOOL RingToolBar::InsertButton(int index,int iBitmap,int idCmd,
										 LPCTSTR szText/*=NULL*/,BYTE btStyle,
										 BYTE btState,DWORD dwData)
{
   int iString = -1;
	if(szText != NULL)
		iString = SendMessage(m_hWnd,TB_ADDSTRING,0,(LPARAM)szText);

	TBBUTTON tbButton;
   tbButton.iBitmap = iBitmap;
   tbButton.idCommand = idCmd;
   tbButton.fsState = btState;
   tbButton.fsStyle = (BYTE)(TBSTYLE_BUTTON | btStyle);
   tbButton.dwData = dwData;
   tbButton.iString = iString;

	if(m_dockbar && m_dockbar->IsVert())
		tbButton.fsState |= TBSTATE_WRAP;

	int cnt = GetButtonCount();
	BOOL bOk = SendMessage(m_hWnd,TB_INSERTBUTTON,index,(LPARAM)&tbButton);

	Update(cnt);
   return bOk;
}

BOOL RingToolBar::InsertSep(int index,int width/*=0*/,int id/*=0*/)
{
   int cnt = GetButtonCount();
	TBBUTTON tbButtons = {width,id,TBSTATE_ENABLED,TBSTYLE_SEP,{0,0},0,0};
	BOOL bOk;

	if(m_dockbar && m_dockbar->IsVert())
		tbButtons.fsState |= TBSTATE_WRAP;
	bOk = SendMessage(m_hWnd,TB_INSERTBUTTON,index,(LPARAM)&tbButtons);
   Update(cnt);
   return bOk;
}

RingControls* RingToolBar::InsertItem(int index,RingControls* item,DWORD dwStyle,UINT uId,int width,int height)
{
   if(item)
   {
      RECT rt;

		item->SetParentObj(this);
		
		if(item->Handle() == NULL)
			item->Create(dwStyle,0,"",uId,0,0,width,height);
		
		if(uId == 0)
			uId = item->GetId();
		
		GetWindowRect(item->Handle(),&rt);
		width = max((rt.right-rt.left),width);
		height = max((rt.bottom-rt.top),height);

		InsertButton(index,-1,uId,NULL,TBSTYLE_BUTTON,TBSTATE_INDETERMINATE);
		TBBUTTONINFO tb;
		tb.cbSize = sizeof(TBBUTTONINFO);
		tb.dwMask = TBIF_SIZE;
		tb.cx = width + 4;
		SendMessage(m_hWnd,TB_SETBUTTONINFO,uId,(LPARAM)&tb);
		//InsertSep(index,width + 4,uId);
		GetButtonRect(index,&rt);

		item->SetParent(m_hWnd);
		item->SetPos(rt.left + 2,rt.top,width,height);		
   }
   return item;
}

BOOL RingToolBar::InsertText(int index,LPCTSTR szText)
{
   if(szText == NULL)
		return FALSE;

	RingStatic *item = NewRingObj(itemof(RingStatic));
	UINT uId = gdwDockBarId++;

	item->Create(SS_CENTERIMAGE,WS_EX_TRANSPARENT,0,uId,0,0,0,0);

	if(item && szText)
   {
      item->SetValue(szText);
		item->SizeFitText();
		item->SetBkColor(CLR_INVALID);
		
		return (BOOL)InsertItem(index,item,0,0,0,HIWORD(GetButtonSize()));
	}
	return FALSE;	
}

RingControls* RingToolBar::GetContrlolItem(UINT uId)
{
	HWND hwnd = GetDlgItem(m_hWnd,uId);
	if(hwnd)
	{
		RingBaseWnd* obj = GetWindowPtr(hwnd);
		if(obj && obj->IsControls())
			return (RingControls*)obj;
	}
	return NULL;
}

LRESULT RingToolBar::RingdowProc(HWND hWnd,RINGPARAMS param)
{
	LRESULT res;
	switch(param.uMsg)
	{
		case WM_ERASEBKGND:
			if(GetStyle() & TBSTYLE_CUSTOMERASE)	//CUSTOMERASE，默认处理，不进行背景绘制
				break;
			if(m_dockbar && m_dockbar->DrawBkg(param.hdc))
				return TRUE;
			break;
		case WM_SIZING:
			if(m_dockbar)
				return OnSizing(param);
		case WM_PAINT:
			res = DefaultProc(param);
			{
				//更新所有子窗口。InsertText加入的Static控件是透明的，
				//如果不更新，当Static控件被局部覆盖引起本消息，背景会覆盖该控件
				//而Static控件的WM_PAINT消息却不能全部更新，导致文字消失。
				HWND hwnd = GetChild();
				while(hwnd)
				{
					InvalidateRect(hwnd,NULL,TRUE);
					hwnd = GetNextChild(hwnd);
				}
			}
			return res;
		/*case TB_SETBUTTONSIZE:
		{
			LRESULT res = DefaultProc(param);
			Update(GetButtonCount());
			if(m_dockbar)
				m_dockbar->UpdateAllSite();
			return res;
		}*/
		default:
		{
			LRESULT res = CtlColorProc(m_hWnd,param);
			if(res != RM_NOTPROCESS)
				return res;
			break;
		}
/*
		case WM_GETDRAGRECTS:
			return OnGETDRAGRECTS(hWnd,param);
//*/
	}
	return RingControls::RingdowProc(hWnd,param);
}

//计算工具栏横排和竖排的尺寸
void RingToolBar::CalcSize(LPSIZE lpsize/*=NULL*/)
{
	RECT rc;
	int bw;
	int n = GetButtonCount();
	TBBUTTON tb;
	//工具栏按钮有变动，最大行数置0，在WM_SIZING里重新计算
	m_nMaxRow = 0;

	m_btnSize.cx = 0;
	m_BarSize.cx = m_BarSize.cy = 0;
	
	m_btnSize.cy = HIWORD(GetButtonSize());

	for(int i=0;i<n;i++)
	{
		GetButton(i,&tb);

		if(tb.fsState & TBSTATE_HIDDEN)
			continue;

		if(GetButtonRect(i,&rc))
		{
			bw = rc.right - rc.left;
			if(bw > m_btnSize.cx)
				m_btnSize.cx = bw;
			m_BarSize.cx += bw;			
		}

		if((tb.fsState & TBSTATE_INDETERMINATE) || (tb.fsStyle & TBSTYLE_SEP))
		{
			if(tb.idCommand !=0)
			{
				HWND hwnd = GetDlgItem(m_hWnd,tb.idCommand);
				if(hwnd && hwnd != m_hWnd)
					SetWindowPos(hwnd,0,rc.left,rc.top,0,0,SWP_NOSIZE);
				m_BarSize.cy += m_btnSize.cy;
			}
			else
				m_BarSize.cy += bw;
		}
		else
			m_BarSize.cy += m_btnSize.cy;
	}	

	if(lpsize)
	{
		lpsize->cx = m_BarSize.cx;
		lpsize->cy = m_BarSize.cy;
	}
}

void RingToolBar::UpdateItems()
{
	RECT rc;
	int n = GetButtonCount();
	TBBUTTON tb;
	
	for(int i=0;i<n;i++)
	{
		GetButton(i,&tb);
		
		if(tb.fsState & TBSTATE_HIDDEN)
			continue;
		
		if(tb.fsState & TBSTATE_INDETERMINATE)
		{
			if(tb.idCommand !=0)
			{
				GetButtonRect(i,&rc);
				HWND hwnd = GetDlgItem(m_hWnd,tb.idCommand);
				if(hwnd && hwnd != m_hWnd)
					SetWindowPos(hwnd,0,rc.left,rc.top,0,0,SWP_NOSIZE);
			}
		}
	}	
}

void RingToolBar::Update(int nOrgButtonCnt,LPRECT lprc/*=NULL*/,
								 BOOL bUpdateFloatRect/*=TRUE*/,BOOL bSetPos/*=TRUE*/)
{
	RECT rc;
	
	CalcSize();
	
	if(lprc)
		CopyRect(&rc,lprc);
	else
	{		
		SendMessage(m_hWnd,TB_AUTOSIZE,0,0);
		GetWindowRect(m_hWnd,&rc);
		OffsetRect(&rc,-rc.left,-rc.top);
	}
		
	if(m_dockbar && m_dockbar->IsVert())
	{
		WrapToolBar(nOrgButtonCnt+2,&rc);		
		m_BarSize.cy = rc.bottom - rc.top;
		if(m_dockbar->IsAllLineBar())
			SetPos(0,0,rc.right - rc.left,m_dockbar->GetClientHeight(0),NULL,SWP_NOMOVE);
	}
	else
	{
		SendMessage(m_hWnd,TB_SETROWS,MAKELONG(1,TRUE),(LPARAM)&rc);
		UpdateItems();
		
		//bSetPos:初始创建时为默认式样，其高度需要控制好，以后父窗口拖动时的处理则不再需要
		if(bSetPos)
		{
			if(m_dockbar && m_dockbar->IsAllLineBar())//GetStyle() & TBSTYLE_FLAT)
				SetPos(0,0,m_BarSize.cx,m_btnSize.cy,NULL,SWP_NOMOVE|SWP_FRAMECHANGED);
			else
				SetPos(0,0,m_BarSize.cx,rc.bottom - rc.top,NULL,SWP_NOMOVE|SWP_FRAMECHANGED);
		}
	}
	if(bUpdateFloatRect)
	{
		m_fSize.cx = m_BarSize.cx;
		m_fSize.cy = m_btnSize.cy;
	}
//	UpdateClientSize(w,h);
}

//根据lprc计算最符合该矩形的窗口尺寸
LRESULT RingToolBar::OnSizing(RINGPARAMS& param)
{
	int n,h = (int)HIWORD(SendMessage(m_hWnd,TB_GETBUTTONSIZE,0,0));

	LPRECT lprc = (LPRECT)param.lParam;
	n = (lprc->bottom - lprc->top)/h;
	
	WrapToolBar(n,lprc);
	m_fSize.cx = lprc->right - lprc->left;
	m_fSize.cy = lprc->bottom - lprc->top;
	m_nfRows = n;
	return TRUE;
}

BOOL RingToolBar::OnParentNotify(HWND hWnd,UINT uMsg,UINT wParam,LONG lParam)
{	
	if(uMsg == MYWM_SIZE)
	{
		//不需要绘制，本消息由DockBar通知，稍后DockBar会通知绘制
		Update(GetButtonCount(),NULL,TRUE,FALSE);
		//CalcSize();
		
		RECT rc;
		GetWindowRect(m_hWnd,&rc);

		if(IsVert(lParam))
		{
			//SendMessage(m_hWnd,TB_SETROWS,MAKELONG(GetButtonCount(),TRUE),(LPARAM)&rc);
			//UpdateItems();
			//GetWindowRect(m_hWnd,&rc);
			return rc.right - rc.left;
			//return m_BarSize.cx;//CalcReSizeV(wParam);
		}
		else
		{
			//SendMessage(m_hWnd,TB_SETROWS,MAKELONG(1,TRUE),(LPARAM)&rc);
			//UpdateItems();
			//GetWindowRect(m_hWnd,&rc);
			return rc.bottom - rc.top;
		}
	}
	return FALSE;
}

/*
//	HWND：父窗口句柄
LRESULT RingToolBar::OnNotify(RINGPARAMS param)
{
	switch(param.lpnmhdr->code)
   {
   	case TTN_NEEDTEXT:
      {
		   ((LPTOOLTIPTEXT)param.lParam)->hinst = m_hInst;
         return SendMessage(m_parent->Handle(),WM_RINGTOOLTIP,((LPTOOLTIPTEXT)param.lParam)->hdr.idFrom,param.lParam);
      }		
		case TBN_DROPDOWN:
		{
			 //LPNMTOOLBAR nm = (LPNMTOOLBAR)param.lParam;
			Errmsg("Asd");
			return 0;//TBDDRET_NODEFAULT;
		}
	}
	return DefaultProc(param);
}
*/

void RingToolBar::ChangeState(int state,RingDockSite* docksite/*=NULL*/)
{
	if(docksite && m_dockbar && 
		(m_dockbar->GetState() == TBS_FLAG_FLY || 
		((m_dockbar->GetState() & 0x2) != (state & 0x2))))
	{
		//原始状态改变，横变竖或竖变横
		int cnt = GetButtonCount();	
		//TBBUTTON tb;
		//tb.

		//for(int i=0;i<cnt;i++)
		//{
		//	if(GetButton(i,&tb))
				if(state > TBS_FLAG_DOCKBOTTOM)
				{
					//tb.fsState |= TBSTATE_WRAP;
					RECT rc;
					SendMessage(m_hWnd,TB_SETROWS,MAKEWPARAM(cnt,TRUE),(LPARAM)&rc);
				}
				else
				{
					RECT rc;
					SendMessage(m_hWnd,TB_SETROWS,MAKEWPARAM(1,FALSE),(LPARAM)&rc);
					//tb.fsState &= ~TBSTATE_WRAP;
					//SendMessage(m_hWnd,TB_SETBUTTONINFO,i,(LPARAM)&tb);
				}
		//}
		//m_State = state;
		Update(cnt);
	}
	else
	{
/*		//m_State = state;
		if(m_dockbar && m_dockbar->GetState() == TBS_FLAG_FLY)
		{
			RECT rc;
			GetButtonRect(0,&rc);
			int row = (m_frc.bottom - m_frc.top - BARTOOLY)/((rc.bottom - rc.top)+m_BorderSpace*2);
			SendMessage(m_hWnd,TB_SETROWS,MAKEWPARAM(row,TRUE),(LPARAM)&rc);
			m_winpos.cx = rc.right - rc.left;
			m_winpos.cy = rc.bottom - rc.top;
		}
//*/
	}
}

//折行工具栏
void RingToolBar::WrapToolBar(int nRows,LPRECT lprc)
{
	if(lprc == NULL)
		return;

	if(m_nMaxRow)
		nRows = min(nRows,m_nMaxRow);

	SendMessage(m_hWnd,TB_SETROWS,MAKEWPARAM(nRows,TRUE),(LPARAM)lprc);
	while(lprc->right - lprc->left < m_btnSize.cx)
	{
		nRows --;
		m_nMaxRow = nRows;
		SendMessage(m_hWnd,TB_SETROWS,MAKEWPARAM(nRows,TRUE),(LPARAM)lprc);
	}
	UpdateItems();
}

BOOL RingToolBar::OnGetDragRects(int state,LPRECT lprc)
{
	if(lprc)
	{
		switch(state)
		{
		case TBS_FLAG_FLY:
			lprc->left = lprc->top = 0;
			lprc->right = m_fSize.cx;
			lprc->bottom = m_fSize.cy;
			break;
		case TBS_FLAG_HORZ:
			lprc->left = lprc->top = 0;
			lprc->right = m_BarSize.cx;
			lprc->bottom = m_btnSize.cy;
			break;
		case TBS_FLAG_VERT:
			lprc->left = lprc->top = 0;
			lprc->right = m_btnSize.cx;
			lprc->bottom = m_BarSize.cy;
			break;
		}
		return TRUE;
	}
	return FALSE;
}

//停靠状态，响应停靠状态改变事件，返回-1：未改变，TRUE：已处理，0：未处理
BOOL RingToolBar::OnDockStateChange(int stateOld,int stateNew)
{
	if(stateNew == stateOld)
		return -1;
	
//	m_State = param.wParam;
	if(stateNew != TBS_FLAG_FLY)
	{
		//原始状态改变，横变竖或竖变横
		int cnt = GetButtonCount();	
		RECT rc;
		int w,h;
		if(IsVert(stateNew))
		{
			WrapToolBar(cnt,&rc);
			CalcSize();
			w = m_btnSize.cx;
			h = m_BarSize.cy;
		}
		else
		{
			WrapToolBar(1,&rc);
			//SendMessage(m_hWnd,TB_SETROWS,MAKEWPARAM(1,FALSE),(LPARAM)&rc);
			//UpdateItems();
			CalcSize();
			w = m_BarSize.cx;
			h = m_btnSize.cy;
		}
		
//		m_State = param.wParam;
		//不需要Update浮动状态
		
		SetPos(0,0,w,h,NULL,SWP_NOMOVE);
	}
	else
	{
		RECT rc;
		//GetButtonRect(0,&rc);
		//int row = (m_fSize.cy)/(rc.bottom - rc.top);
		
		WrapToolBar(m_nfRows,&rc);		
	}
	return TRUE;
}


