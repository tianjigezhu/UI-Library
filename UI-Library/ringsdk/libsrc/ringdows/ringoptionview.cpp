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
//原文件名：ringoptionview.cpp
//
//说明：类似DELPHI属性控件控制
//
**********************************************************************/
#define MAKE_SELF_LIB
//#include "ringdebug.h"
#include "ringdows.h"

RingOptionView::RingOptionView()
{
	m_edit = NULL;
	m_combo = m_comboSel = NULL;
	m_button = NULL;
	m_pen = NULL;
	m_SplitPos = 0;
	m_LeftPos = 0;
	m_cursorSize = LoadCursor(NULL,IDC_SIZEWE);
	m_cursor = LoadCursor(NULL,IDC_ARROW);
	m_bCapture = FALSE;
	m_bUpdate = FALSE;
}

RingOptionView::~RingOptionView()
{
	if(m_pen)
		DeleteObject(m_pen);
}

BOOL RingOptionView::Create(DWORD dwStyle,DWORD dwExStyle,LPCTSTR szInitText,UINT uId,int x,int y,int width,int height)
{
	dwStyle |= (LBS_HASSTRINGS|LBS_OWNERDRAWFIXED|LBS_NOTIFY|WS_VSCROLL);
	
	if(RingControls::Create(dwStyle,dwExStyle,szInitText,uId,x,y,width,height))
	{
		RingControls::SetOwnerDraw(FALSE,(UINT)0xFFFFFFFF);
		OnCreate(m_hWnd);
		return TRUE;
	}
	return FALSE;
}
	
BOOL RingOptionView::Create(UINT uId,int x/*=0*/,int y/*=0*/,int width/*=-999*/,int height/*=-999*/)
{
	DWORD dwExStyle = 0;
	if(width == -999 || height == -999)
		dwExStyle = RTV_FITPARENTSIZE;
	
	return Create(LBS_HASSTRINGS|LBS_OWNERDRAWFIXED|LBS_NOTIFY|WS_VSCROLL,
						dwExStyle,"",uId,x,y,width,height);
}

void RingOptionView::OnCreate(HWND)
{
	RECT rc;
	GetClientRect(&rc);
	m_SplitPos = (rc.right - rc.left) / 2;

	if(m_pen == NULL)
		m_pen = CreatePen(PS_SOLID,1,0x00C6C6C6);
	SetItemHeight(GetItemHeight() + 2);
//StartDebug(m_hWnd);
}

BOOL RingOptionView::Attach(HWND hWnd)
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
			OnFitSize(m_parent->Handle(),0,0);
			OnCreate(m_hWnd);
			return TRUE;
		}
	}
	else
		if(RingBaseWnd::Attach(hWnd))
		{
			RingControls::SetOwnerDraw(FALSE,(UINT)0xFFFFFFFF);
			OnFitSize(m_parent->Handle(),0,0);
			OnCreate(m_hWnd);
			return TRUE;
		}
		
	return FALSE;
}

BOOL RingOptionView::OnFitSize(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	BOOL bOK = RingControls::OnFitSize(hWnd,wParam,lParam);
	RECT rc;
	GetClientRect(&rc);
	m_SplitPos = (rc.right - rc.left + m_LeftPos) / 2;
	return bOK;
}

BOOL RingOptionView::OnDrawItem(LPDRAWITEMSTRUCT lpds)
{
	if(lpds->itemAction & ODA_FOCUS)
		return TRUE;

	RECT rc;
	int cx = ClientWidth() - 1;
	CopyRect(&rc,&lpds->rcItem);
	rc.right = m_SplitPos;
	
	int ndc = SaveDC(lpds->hDC);

	HGLOBAL m_old = SelectObject(lpds->hDC,m_pen);
	SelectObject(lpds->hDC,m_brush);
	
	RingOptionItem* item = (RingOptionItem*)lpds->itemData;
	
	if((lpds->itemState & ODS_SELECTED))
	{		
		::SetBkColor(lpds->hDC,GetSysColor(COLOR_HIGHLIGHT));
		::SetTextColor(lpds->hDC,GetSysColor(COLOR_HIGHLIGHTTEXT));
		if(item->IsPackItem() || item->IsSubItem())
		{
			rc.left += m_LeftPos;
			ExtTextOut(lpds->hDC,0,0,ETO_OPAQUE,&rc,NULL,0,NULL);
			rc.left -= m_LeftPos;
		}
		else
			ExtTextOut(lpds->hDC,0,0,ETO_OPAQUE,&rc,NULL,0,NULL);
		DrawEdge(lpds->hDC,&lpds->rcItem,EDGE_SUNKEN,BF_RECT);

		if(item && !m_bUpdate)
			item->EnterEdit(rc.right,rc.top,cx-rc.right,rc.bottom-rc.top);
		m_bUpdate = FALSE;
	}
	else
	{
		if(item && item->GetCtrl())
			item->ExitEdit();
		FillRect(lpds->hDC,&lpds->rcItem,m_brush);		
	}

	rc.left += m_LeftPos;
	MoveToEx(lpds->hDC,rc.left,rc.top,NULL);
	LineTo(lpds->hDC,rc.right,rc.top);
	LineTo(lpds->hDC,rc.right,rc.bottom);
	LineTo(lpds->hDC,rc.left,rc.bottom);
	if(!(lpds->itemState & ODS_SELECTED))
		LineTo(lpds->hDC,rc.left,rc.top);
	MoveToEx(lpds->hDC,rc.right,rc.top,NULL);
	LineTo(lpds->hDC,cx,rc.top);
	LineTo(lpds->hDC,cx,rc.bottom);
	LineTo(lpds->hDC,rc.right,rc.bottom);
	LineTo(lpds->hDC,rc.right,rc.top);
	
	InflateRect(&rc,-1,-2);
	OffsetRect(&rc,1,2);
	
	if(item)
	{
		HFONT hfont = NULL;
		if(item->IsSubItem())
			hfont = (HFONT)SelectObject(lpds->hDC,m_fontSubItem.GetFont());
		else if(item->IsPackItem())
			hfont = (HFONT)SelectObject(lpds->hDC,m_fontPack.GetFont());

		DrawText(lpds->hDC,item->GetName(),strlen(item->GetName()),&rc,DT_LEFT|DT_END_ELLIPSIS);		
		rc.left = rc.right + 2;
		rc.right = cx;
		
		if(hfont)
			SelectObject(lpds->hDC,hfont);

		item->DrawValue(lpds->hDC,&rc);		
	}
	RestoreDC(lpds->hDC,ndc);

	return TRUE;
}

//////////////////////////////////////////////////////////
//
//编辑控件值改变（列表选择变动，点击按钮，编辑框失去焦点），
//执行绑定的相应处理函数，如果未绑定函数，发送相应通知消息.
//
//////////////////////////////////////////////////////////
void RingOptionView::SendNotify(int type)
{
	RingOptionItem* item = GetOptionItem();
	if(item && !item->OnBeforeNotify(type))
	{
		ROPVNOTIFY opnm;
		opnm.hdr.code = type;
		opnm.hdr.hwndFrom = m_hWnd;
		opnm.hdr.idFrom = m_ID;
		opnm.item = item;
		opnm.lpszName = item->GetName();
		opnm.lpszText = item->GetText();
		opnm.lpszOrgText = item->GetPrevText();
		opnm.lData = item->GetData();
		opnm.lValue = item->GetInt();
		opnm.uType = item->GetType();
		SendMessage(m_parent->Handle(),WM_NOTIFY,GetCurSel(),(LPARAM)&opnm);
	}
}

LRESULT RingOptionView::RingdowProc(HWND hWnd,RINGPARAMS param)
{
	switch(param.uMsg)
	{
		case LB_ADDFILE:
		case LB_DIR:
			//取消列表文件功能,不支持
			return LB_ERR;
		case WM_DRAWITEM:
			//BUTTON必须自绘
			NotifyChild(m_hWnd,param.uMsg,param.wParam,param.lParam);
			break;
		case WM_COMMAND:			
			if(param.shortval.high == CBN_SELCHANGE && 
				((m_combo && param.shortval.low == LOWORD(m_combo)) ||
				 (m_comboSel && param.shortval.low == LOWORD(m_comboSel))))
				SendNotify(ROPVN_SELCHANGE);
			else if(m_button && param.shortval.low == LOWORD(m_button)
				&& param.shortval.high == BN_CLICKED)
				SendNotify(ROPVN_BUTTONCLICK);
			else if(m_edit && m_button == NULL && param.shortval.low == LOWORD(m_edit)
				&& param.shortval.high == EN_KILLFOCUS)
				SendNotify(ROPVN_VALUECHANGE);
			break;
		case WM_MOUSEMOVE:
			if(m_bCapture)
			{
				if(param.mousept.x > m_LeftPos + 16 && 
					param.mousept.x < ClientWidth() - 32)
				{
					if(m_SplitPos != param.mousept.x)
					{
						m_SplitPos = param.mousept.x;
						InvalidateRect(m_hWnd,NULL,TRUE);
					}					
				}
				return 0;
			}
			else if(param.mousept.y < GetCount() * GetItemHeight() && 
						param.mousept.x > m_SplitPos-3 &&
						param.mousept.x < m_SplitPos+3)
				SetCursor(m_cursorSize);
			else
				SetCursor(m_cursor);
			//不支持多选。注意该段代码拿掉会导致数据包因展开需调整
			//topitem而使高亮选择变动到subitem.
			if((param.wParam & MK_LBUTTON) == MK_LBUTTON && !m_bCapture)
				return 0;
			break;
		case WM_LBUTTONDOWN:
			if(param.mousept.y < GetCount() * GetItemHeight() && 
				param.mousept.x > m_SplitPos-3 &&
				param.mousept.x < m_SplitPos+3)
			{
				SetCursor(m_cursorSize);
				m_bCapture = TRUE;
				SetCapture(m_hWnd);
				return 0;
			}
			else
			{
				RingOptionItem* item = GetOptionItem(GetCurSel());
				m_bUpdate = TRUE;
				if(item && item->GetCtrl() && !item->IsEditVisible())
				{
					//原选中项退出编辑，item->ExitEdit()会通知item值改变
					item->ExitEdit();					
				}
				
				LRESULT res = RingControls::RingdowProc(hWnd,param);
				
				item = GetOptionItem(GetCurSel());
				
				if(item)
				{
					RECT rc;
					if(GetItemRect(GetCurSel(),&rc))
					{
						rc.left = m_SplitPos;
						item->EnterEdit(rc.left,rc.top,rc.right-rc.left,rc.bottom-rc.top);
					}
					if(param.mousept.x < m_LeftPos && item->IsPackItem())
						((RingOptionItemPack*)item)->AutoExpand();					
				}
				return res;				
			}			
		case WM_LBUTTONUP:
			if(m_bCapture)
				ReleaseCapture();
			m_bCapture = FALSE;
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
			int type = 0;
			LPARAM lParam = param.lParam;
			RingOptionItem* item;
			LRESULT res = LB_ERR;

			if(param.wParam != -1)
			{
				//LB_INSERTSTRING并加到最后param.wParam为-1
				type = param.shortval.high;
				param.shortval.high = 0;
				if(MASK_MATCH(type,OPTYPE_EXTERN))//外部数据
				{
					try
					{
						param.lParam = (LPARAM)(((RingOptionItem*)lParam)->GetName());
					}
					catch (...)
					{
						return LB_ERR;
					}
				}
			}
			item = OnAddString(type,(LPCTSTR)lParam);
			if(item)
			{
				res = RingControls::RingdowProc(hWnd,param);
				if(res != LB_ERR && res != LB_ERRSPACE)
				{
					item->SetParent(this);
					SendMessage(m_hWnd,LB_SETITEMDATA,res,(LPARAM)item);
				}
				else
					if((type & OPTYPE_EXTERN) != OPTYPE_EXTERN)//外部数据
						delete item;
			}
			return res;
		}
		case LB_DELETESTRING:
		{			
			OnDelString(param.wParam,param.lParam==DELITEM_HIDE?DELITEM_HIDE:0);
			break;
		}
		case LB_RESETCONTENT:
		{
			int n = GetCount();
			for(int i=0;i<n;i++)
				OnDelString(i,DELITEM_CLEAR);
			break;
		}
		case LB_SETITEMDATA:
		{	
			LONG item = CallWindowProc(m_DefProc,m_hWnd,LB_GETITEMDATA,param.wParam,0);
			if(item != LB_ERR)
				if(item == 0)
					break;
				else
				{
					try
					{
						((RingOptionItem*)item)->SetData(param.lParam);
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
			LONG item = RingControls::RingdowProc(hWnd,param);
			if(item != LB_ERR)
			{
				if(item == 0)
					return 0;
				else
				{
					try
					{
						return ((RingOptionItem*)item)->GetData();
					}
					catch(...)
					{
					}
				}
			}
			return LB_ERR;
		}
		//case WM_ERASEBKGND:
		//case WM_WINDOWPOSCHANGED:
		//	return 0;
	}
	return RingControls::RingdowProc(hWnd,param);
}

RingOptionItem* RingOptionView::GetOptionItem(int index/*=INDEX_CURSEL*/)
{
	if(index == INDEX_CURSEL)
		index = GetCurSel();

	LONG item = CallWindowProc(m_DefProc,m_hWnd,LB_GETITEMDATA,index,0);
	if(item == LB_ERR)
		item = 0;
	return (RingOptionItem*)item;
}

RingOptionItem* RingOptionView::GetOptionItem(LPCTSTR lpName)
{
	int index = FindMatchString(lpName);
	if(index != LB_ERR)
		return GetOptionItem(index);
	else
		return NULL;
}

RingOptionItemPack* RingOptionView::GetPackItem(LPCTSTR lpName)
{
	int idx,index = FindMatchString(lpName);
	if(index != LB_ERR)
	{
		RingOptionItem* item = GetOptionItem(index);

		while(item)
		{
			//只有是PACK才返回
			if(item->GetType() == OPTYPE_PACK)
				return (RingOptionItemPack*)item;

			idx = FindMatchString(lpName,index);
			//未找到或已找了一圈
			if(idx == LB_ERR || idx <= index)
				return NULL;
			index = idx;
			item = GetOptionItem(index);
		}
	}
	return NULL;
}

RingOptionItemPack* RingOptionView::GetPackItem(int index)
{
	if(index != LB_ERR)
	{
		RingOptionItem* item = GetOptionItem(index);
		
		//只有是PACK才返回
		if(item->GetType() == OPTYPE_PACK)
			return (RingOptionItemPack*)item;
	}
	return NULL;
}

//重载GetInt为获得属性值,GetText无法重载，改变其行为需
//重载GetItemText
int RingOptionView::GetInt(int index/*=INDEX_CURSEL*/)
{
	if(index == INDEX_CURSEL)
		index = GetCurSel();

	RingOptionItem* item = GetOptionItem(index);
	if(item)
		return item->GetInt();
	else
		return 0;
}

int RingOptionView::GetInt(LPCTSTR lpName)
{
	int index = FindMatchString(lpName);

	if(index == LB_ERR)
		return 0;
	else
		return GetInt(index);
}

//获取选项DATA
int RingOptionView::GetSelectData(int index)
{
	if(index == INDEX_CURSEL)
		index = GetCurSel();
	
	RingOptionItem* item = GetOptionItem(index);
	if(item && MASK_MATCH(item->GetType(),OPTYPE_LIST))
		return ((RingOptionItemList*)item)->GetItemData(index);
	else
		return 0;
}

int RingOptionView::GetSelectData(LPCTSTR lpName)
{
	int index = FindMatchString(lpName);
	
	if(index == LB_ERR)
		return 0;
	else
		return GetSelectData(index);
}

//获取属性名称（原GetText的行为）
LPCTSTR RingOptionView::GetName(int index)
{
	RingOptionItem* item = GetOptionItem(index);
	if(item)
		return item->GetName();
	else
		return NULL;
}

int RingOptionView::GetType(int nIndex)
{
	RingOptionItem* item = GetOptionItem(nIndex);
	if(item)
		return item->GetType();
	else
		return OPTYPE_NONE;
}

//重载该函数使GetText返回属性值，真正的Item Text
//用GetName得到
int RingOptionView::GetItemText(int index,LPTSTR lpBuf,int size)
{
	if(index == INDEX_CURSEL)
		index = GetCurSel();

	RingOptionItem* item = GetOptionItem(index);
	if(item)
	{
		LPCTSTR lps = item->GetText();
		lstrcpyn(lpBuf,lps,size);
		return lstrlen(lpBuf);
	}
	return 0;
}

int RingOptionView::GetSelectIndex(int index)
{
	if(index == INDEX_CURSEL)
		index = GetCurSel();

	RingOptionItem* item = GetOptionItem(index);
	if(item)
		return item->GetSelectIndex();
	return LB_ERR;
}

int RingOptionView::GetSelectIndex(LPCTSTR lpName)
{
	int index = FindMatchString(lpName);
	
	if(index == LB_ERR)
		return LB_ERR;
	else
		return GetSelectIndex(index);
}

BOOL RingOptionView::SetSelectIndex(int index,int val)
{
	RingOptionItem* item = GetOptionItem(index);
	if(item)
		return item->SetSelectIndex(val);
	return FALSE;
}

BOOL RingOptionView::SetSelectIndex(LPCTSTR lpName,int val)
{
	int index = FindMatchString(lpName);
	
	if(index == LB_ERR)
		return FALSE;
	else
		return SetSelectIndex(index,val);
}

BOOL RingOptionView::SetSelectIndexByData(int index,LONG lData,BOOL IsString/*=FALSE*/)
{
	RingOptionItem* item = GetOptionItem(index);
	if(item)
		return item->SetSelectIndexByData(lData,IsString);
	return FALSE;
}

BOOL RingOptionView::SetSelectIndexByData(LPCTSTR lpName,LONG lData,BOOL IsString/*=FALSE*/)
{
	int index = FindMatchString(lpName);
	
	if(index == LB_ERR)
		return FALSE;
	else
		return SetSelectIndexByData(index,lData,IsString);
}

RingOptionItem* RingOptionView::OnAddString(DWORD type,LPCTSTR lpString)
{	
	RingOptionItem* item = NULL;
	
	if((type & OPTYPE_EXTERN) == OPTYPE_EXTERN)//外部数据
		item = (RingOptionItem*)lpString;

	try
	{
		if(m_fontSubItem.GetFont() == NULL && item && item->IsSubItem())
		{
			LOGFONT hf;
			BOOL bOK = m_fontSubItem.GetWindowLogFont(m_hWnd,&hf);
			if(!bOK)
			{
				m_fontSubItem.SetFont((HFONT)GetStockObject(DEFAULT_GUI_FONT));
				bOK = m_fontSubItem.GetLogFont(&hf);
			}

			if(bOK)
			{
				//hf.lfItalic = TRUE;
				hf.lfUnderline = TRUE;
				m_fontSubItem.SetFont(&hf);
			}
		}
		
		switch(type & OPTYPE_MASK)
		{
			case OPTYPE_LIST:
				if(item == NULL)
				{
					item = new RingOptionItemList;
					item->SetAutoDelete(TRUE);
					item->SetName(lpString);
				}
				if(type & OPTYPE_READONLY)
				{
					if(m_comboSel == NULL)
					{
						m_comboSel = NewRingObj(m_comboSel);
						m_comboSel->Create(WS_CHILD|CBS_AUTOHSCROLL|CBS_DROPDOWNLIST|CBS_HASSTRINGS|WS_BORDER|WS_VSCROLL,WS_EX_CLIENTEDGE,"",LOWORD(m_comboSel),0,0,0,120);
					}
				}
				else
				{
					if(m_combo == NULL)
					{
						m_combo = NewRingObj(m_combo);
						m_combo->Create(WS_CHILD|CBS_AUTOHSCROLL|CBS_DROPDOWN|CBS_HASSTRINGS|WS_BORDER|WS_VSCROLL,WS_EX_CLIENTEDGE,"",LOWORD(m_combo),0,0,0,120);
					}
				}
				break;
			case OPTYPE_BROWSE:
				if(item == NULL)
				{
					item = new RingOptionItemSel;
					item->SetAutoDelete(TRUE);
					item->SetName(lpString);
				}
				if(m_edit == NULL)
				{
					m_edit = NewRingObj(m_edit);
					m_edit->Create(WS_CHILD|ES_AUTOHSCROLL,
						WS_EX_CLIENTEDGE,"",LOWORD(m_edit),0,0,0,0);
					//初始化m_edit的数据缓冲区为1024，否则该缓冲区仅为80字节
					if(m_edit)
						m_edit->GetText(INDEX_CURSEL,1024);
				}
				
				if(m_edit && m_button == NULL)
				{
					m_button = NewRingObj(m_button);
					m_button->Create(WS_CHILD|BS_PUSHBUTTON|BS_TEXT,0,"...",LOWORD(m_button),0,0,0,0);
					m_button->SetFont("宋体",4,FW_BOLD);
					m_button->SetBkColor(GetSysColor(COLOR_BTNFACE));
					m_button->SetExStyle(RINGBTN_EX_NOFOCUSRECT);
				}
				break;
			case OPTYPE_PACK:
				if(item == NULL)
				{
					item = new RingOptionItemPack;
					item->SetAutoDelete(TRUE);
					item->SetName(lpString);
				}
				else
				{
					item->SetParent(this);
					((RingOptionItemPack*)item)->Expand();
				}
				if(m_fontPack.GetFont() == NULL)
				{
					LOGFONT hf;
					BOOL bOK = m_fontPack.GetWindowLogFont(m_hWnd,&hf);
					if(!bOK)
					{
						m_fontPack.SetFont((HFONT)GetStockObject(DEFAULT_GUI_FONT));
						bOK = m_fontPack.GetLogFont(&hf);
					}
					if(bOK)
					{
						hf.lfWeight = FW_BOLD;
						m_fontPack.SetFont(&hf);
					}
				}
				break;
			default:	//默认为OPTYPE_EDIT:
				if(item == NULL)
				{
					item = new RingOptionItemEdit;
					item->SetAutoDelete(TRUE);
					item->SetName(lpString);
				}
				else if((type & OPTYPE_MASK) != OPTYPE_EDIT)
					//外部数据不合要求
					return NULL;

				if(m_edit == NULL)
				{
					m_edit = NewRingObj(m_edit);
					m_edit->Create(WS_CHILD|ES_AUTOHSCROLL,
											WS_EX_CLIENTEDGE,"",LOWORD(m_edit),0,0,0,0);
					//初始化m_edit的数据缓冲区为1024，否则该缓冲区仅为80字节
					if(m_edit)
						m_edit->GetText(INDEX_CURSEL,1024);
				}
				break;							
		}
		//只有是READONLY才需要设置，否则无须改变值，READONLY默认FALSE，
		//如果是SubItem，会调用本函数两次，第二次已无READONLY信息。
		if(item && (type & OPTYPE_READONLY) && (type & OPTYPE_PACK) == 0)
			item->SetReadOnly(TRUE);
	}
	catch(...)
	{
		//出错情况为外部数据或new失败，无须delete item
		return NULL;
	}
	return item;
}

void RingOptionView::OnDelString(int index,DWORD dwFlag)
{
	LONG res = CallWindowProc(m_DefProc,m_hWnd,LB_GETITEMDATA,index,0);
	if(res != LB_ERR && res != 0)
	{
		RingOptionItem* item = (RingOptionItem*)res;
		if(item)
		try
		{
			item->ExitEdit();
			if(item->IsSubItem())
			{
				//数据包子项,先置DATA为0，免发送WM_DELETEITEM消息
				CallWindowProc(m_DefProc,m_hWnd,LB_SETITEMDATA,index,0);
				if(dwFlag == DELITEM_HIDE)
				{
					//数据包收缩，对象仅隐藏不析构，
					item->SetParent(NULL);
				}
				else
				{
					//删除
					RingOptionItemPack* pack = item->m_parentItem;
					if(pack)	//DelSubItem会发送WM_DELETEITEM消息
						pack->DelSubItem(item);
				}
				return;
			}
				
			if(item->IsPackItem())
			{
				((RingOptionItemPack*)item)->Collapse();

				//ResetContent调用时不能调用IsLastPack，因IsLastPack会遍历选项，
				//而此时可能已有选项处于删除状态，其绑定的DATA已变为原设置的DATA，
				//RingOptionItem已删除，无法访问。				
				if(dwFlag != DELITEM_CLEAR && IsLastPack(item))
					SetLeftMargin(0);
			}

			if(item->m_bAutoDelete)
			{
				CallWindowProc(m_DefProc,m_hWnd,LB_SETITEMDATA,index,item->GetData());
				delete item;
			}
			else
				item->SetParent(NULL);
		}
		catch(...)
		{
		}
	}
}

LRESULT RingOptionView::OnDestroy(RINGPARAMS& param)
{
	Clear();
	return DefaultProc(param);
}

BOOL RingOptionView::IsLastPack(RingOptionItem* item)
{
	if(item && item->IsPackItem())
	{
		int cnt = GetCount();
		LONG tmp;

		for(int i=0;i<cnt;i++)
		{
			tmp = CallWindowProc(m_DefProc,m_hWnd,LB_GETITEMDATA,i,0);
			if(tmp != LB_ERR && tmp != NULL && tmp != (LONG)item)
			{
				if(((RingOptionItem*)tmp)->IsPackItem())
					return FALSE;
			}
		}
		return TRUE;
	}
	return FALSE;
}

BOOL RingOptionView::SetValue(int val,int index/*=INDEX_CURSEL*/)
{
	RingOptionItem* item = GetOptionItem(index);
	if(item)
		return item->SetValue(val);
	else
		return FALSE;
}

BOOL RingOptionView::SetValue(LPCTSTR lpszVal,int index/*=INDEX_CURSEL*/)
{
	RingOptionItem* item = GetOptionItem(index);
	if(item)
		return item->SetValue(lpszVal);
	else
		return FALSE;
}

BOOL RingOptionView::SetValue(int index,LPARAM lVal,LPCTSTR szShowVal/*=NULL*/)
{
	RingOptionItem* item = GetOptionItem(index);
	if(item)
		return item->SetValue(lVal,szShowVal);
	else
		return FALSE;
}

BOOL RingOptionView::SetValue(LPCTSTR szItemName,LPARAM lVal,LPCTSTR szShowVal/*=NULL*/)
{	
	return SetValue(FindMatchString(szItemName),lVal,szShowVal);	
}

//设置数据包子项数据
BOOL RingOptionView::SetPackItemValue(int index,LPCTSTR szSubItem,LPARAM lVal,LPCTSTR szShowVal/*=NULL*/)
{
	return SetPackItemValue(GetPackItem(index),szSubItem,lVal,szShowVal);
}

BOOL RingOptionView::SetPackItemValue(LPCTSTR szPackName,LPCTSTR szSubItem,LPARAM lVal,LPCTSTR szShowVal/*=NULL*/)
{
	return SetPackItemValue(GetPackItem(szPackName),szSubItem,lVal,szShowVal);
}

BOOL RingOptionView::SetPackItemValue(RingOptionItemPack* pack,LPCTSTR szSubItem,LPARAM lVal,LPCTSTR szShowVal/*=NULL*/)
{
	if(pack)
	{
		RingOptionItem* item = GetOptionItem(szSubItem);
		if(item == NULL || !item->IsSubItem() || item->m_parentItem != pack)
			//包可能已收起，遍历寻找
			item = pack->GetSubItem(szSubItem);
		
		if(item)
			return item->SetValue(lVal,szShowVal);		
	}
	return FALSE;
}

//获取数据包子项数据，数据包有可能收起，调用GetText和GetInt会失败或得不到
//正确值，GetPackItemText和GetPackItemInt不管数据包是否收起总能获得正确值
LPCTSTR RingOptionView::GetPackItemText(int index,LPCTSTR szSubItem)
{
	return GetPackItemText(GetPackItem(index),szSubItem);
}

LPCTSTR RingOptionView::GetPackItemText(LPCTSTR szPackName,LPCTSTR szSubItem)
{
	return GetPackItemText(GetPackItem(szPackName),szSubItem);
}

LPCTSTR RingOptionView::GetPackItemText(RingOptionItemPack* pack,LPCTSTR szSubItem)
{
	if(pack)
	{
		RingOptionItem* item = pack->GetSubItem(szSubItem);
		
		if(item)
			return item->GetText();
	}
	return "";
}

int RingOptionView::GetPackItemInt(int index,LPCTSTR szSubItem)
{
	return GetPackItemInt(GetPackItem(index),szSubItem);
}

int RingOptionView::GetPackItemInt(LPCTSTR szPackName,LPCTSTR szSubItem)
{
	return GetPackItemInt(GetPackItem(szPackName),szSubItem);
}

int RingOptionView::GetPackItemInt(RingOptionItemPack* pack,LPCTSTR szSubItem)
{
	if(pack)
	{
		RingOptionItem* item = pack->GetSubItem(szSubItem);
		
		if(item)
			return item->GetInt();
	}
	return 0;
}

//加入属性设置
int RingOptionView::AddOption(LPCTSTR szOption,LPCTSTR szDefValue/*=NULL*/,
										LPARAM lParam/*=0*/,UINT uEditType/*=OPTYPE_EDIT*/,
										LONG lTypeCfg/*=0*/)
{
	int index = SendMessage(m_hWnd,LB_ADDSTRING,(uEditType<<16),(LPARAM)szOption);
	if(index >= 0)
	{
		RingOptionItem* item = GetOptionItem(index);
		if(item)
		{
			item->SetOption(lTypeCfg);
			item->InterSetValue(lParam,szDefValue);
		}
	}
	return index;
}

int RingOptionView::AddOptionPack(LPCTSTR szOption,LPARAM lParam/*=0*/)
{
	int index = SendMessage(m_hWnd,LB_ADDSTRING,(OPTYPE_PACK<<16),(LPARAM)szOption);
	if(index >= 0)
	{
		RingOptionItem* item = GetOptionItem(index);
		if(item)
			item->InterSetValue(lParam,NULL);
	}
	return index;
}

int RingOptionView::AddOption(RingOptionItem* item)
{
	//已加入到OptionView中的项目不能再加入
	if(item && item->GetParent() == NULL)
		return SendMessage(m_hWnd,LB_ADDSTRING,((item->GetType()|OPTYPE_EXTERN)<<16),(LPARAM)item);
	else
		return LB_ERR;
}

int RingOptionView::InsertOption(int index,LPCTSTR szOption,LPCTSTR szDefValue/*=NULL*/,
											LPARAM lParam/*=0*/,UINT uEditType/*=OPTYPE_EDIT*/,
											LONG lTypeCfg/*=0*/)
{
	int idx = SendMessage(m_hWnd,LB_INSERTSTRING,MAKELONG(index,uEditType),(LPARAM)szOption);
	if(index >= 0)
	{
		RingOptionItem* item = GetOptionItem(idx);
		if(item)
		{
			item->SetOption(lTypeCfg);
			item->InterSetValue(lParam,szDefValue);
		}
	}
	return idx;
}

int RingOptionView::InsertOption(RingOptionItem* item,int index)
{
	if(index == INDEX_CURSEL)
		return AddOption(item);
	else if(item && item->GetParent() == NULL)
		return SendMessage(m_hWnd,LB_INSERTSTRING,((item->GetType()|OPTYPE_EXTERN)<<16) + LOWORD(index),(LPARAM)item);
	else
		return LB_ERR;
}

int RingOptionView::InsertOption(RingOptionItem* item,LPCTSTR lpString)
{
	if(item && lpString)
	{
		int index = FindMatchString(lpString);
		return InsertOption(item,index);
	}
	else
		return LB_ERR;
}

int RingOptionView::AddSubOption(LPCTSTR szPackName,LPCTSTR szOption,
											LPCTSTR szDefValue/*=NULL*/,
											LPARAM lParam/*=0*/,
											UINT uEditType/*=OPTYPE_EDIT*/,
											LONG lTypeCfg/*=0*/)
{
	//取消包属性
	if((uEditType & OPTYPE_PACK))
		return LB_ERR;

	RingOptionItemPack* item = GetPackItem(szPackName);
	if(item)
	{
		RingOptionItem* subitem = OnAddString(uEditType,szOption);
		if(subitem)
		{
			subitem->SetOption(lTypeCfg);
			subitem->InterSetValue(lParam,szDefValue);
			return item->AddSubItem(subitem);
		}
	}
	return LB_ERR;
}

int RingOptionView::AddSubOption(int index,LPCTSTR szOption,LPCTSTR szDefValue/*=NULL*/,
											LPARAM lParam/*=0*/,UINT uEditType/*=OPTYPE_EDIT*/,
											LONG lTypeCfg/*=0*/)
{
	//取消包属性
	if((uEditType & OPTYPE_PACK))
		return LB_ERR;
	
	RingOptionItemPack* item = GetPackItem(index);
	if(item)
	{
		RingOptionItem* subitem = OnAddString(uEditType,szOption);
		if(subitem)
		{
			subitem->SetOption(lTypeCfg);
			subitem->InterSetValue(lParam,szDefValue);
			return item->AddSubItem(subitem);
		}
	}
	return LB_ERR;
}

int RingOptionView::DelOption(RingOptionItem* item)
{
	if(item)
		return DelString(item->GetName());
	else
		return LB_ERR;
}

int RingOptionView::ClearPack(int index)
{
	RingOptionItem* item = GetOptionItem(index);
	if(item->IsPackItem())
	{
		RingOptionItemPack* pack = (RingOptionItemPack*)item;
		pack->Collapse();
		pack->Clear();
		return TRUE;
	}
	return LB_ERR;
}

int RingOptionView::ClearPack(LPCTSTR lpName)
{
	return ClearPack(FindMatchString(lpName));
}

//清空数据，保留各子项，设置各项值为空，保留列表子项的列表选项
void RingOptionView::Reset()
{
	RingOptionItem* item;

	for(int i=0;i<GetCount();i++)
	{
		item = GetOptionItem(i);
		//SubItem不需要Reset，由PackItem遍历后Reset
		if(item && !item->IsSubItem())
			item->Reset();
	}
}

BOOL RingOptionView::SetItemValChangeFunc(int index,RINGVCLASS* obj,ROPV_CHECKVALFUNC func)
{
	RingOptionItem* item = GetOptionItem(index);
	if(item)
	{
		item->SetCheckValueFunc(obj,func);
		return TRUE;
	}
	return FALSE;
}

BOOL RingOptionView::SetItemValChangeFunc(LPCTSTR lpName,RINGVCLASS* obj,ROPV_CHECKVALFUNC func)
{
	int index = FindMatchString(lpName);
	
	if(index == LB_ERR)
		return FALSE;
	else
		return SetItemValChangeFunc(index,obj,func);
}

BOOL RingOptionView::SetItemValSelectFunc(int index,RINGVCLASS* obj,ROPV_CHECKVALFUNC func)
{
	RingOptionItem* item = GetOptionItem(index);
	if(item)
		return item->SetSelChangeFunc(obj,func);
	else
		return FALSE;
}

BOOL RingOptionView::SetItemValSelectFunc(LPCTSTR lpName,RINGVCLASS* obj,ROPV_CHECKVALFUNC func)
{
	int index = FindMatchString(lpName);
	
	if(index == LB_ERR)
		return FALSE;
	else
		return SetItemValSelectFunc(index,obj,func);
}

BOOL RingOptionView::SetItemBrowserFunc(int index,RINGVCLASS* obj,ROPV_CHECKVALFUNC func)
{
	RingOptionItem* item = GetOptionItem(index);
	if(item)
		return item->SetBrowserFunc(obj,func);
	else
		return FALSE;
}

BOOL RingOptionView::SetItemBrowserFunc(LPCTSTR lpName,RINGVCLASS* obj,ROPV_CHECKVALFUNC func)
{
	int index = FindMatchString(lpName);
	
	if(index == LB_ERR)
		return FALSE;
	else
		return SetItemBrowserFunc(index,obj,func);
}

/**********************************************************
//
//RingOptionItem
//
***********************************************************/
void RingOptionItem::DrawValue(HDC hDC,LPRECT lprc)
{
	if(m_edit == NULL)
		DrawText(hDC,GetText(),m_strValue.len(),lprc,DT_LEFT);
}

//得到在OptionView中的排列序号(数据包子项返回LB_ERR)
int RingOptionItem::GetIndex()
{	
	if(m_parent && !IsSubItem())
	{
		int start,index = -1;
		RingOptionItem* item = NULL;

		do
		{
			start = index;
			index = m_parent->FindMatchString(m_strName.string(),start);
			if(index != LB_ERR)
				item = m_parent->GetOptionItem(index);
			else
				return LB_ERR;
		}while(item != this && index > start);

		if(item == this)
			return index;
	}
	return LB_ERR;
}

//得到在数据包中的排列序号
int RingOptionItem::GetSubIndex(RingOptionItemPack** parentItem/*=NULL*/)
{	
	if(parentItem)
		*parentItem = m_parentItem;
	
	if(IsSubItem() && m_parentItem)
		return m_parentItem->GetSubItemIndex(m_strName.string());
	
	return LB_ERR;
}

LPCTSTR RingOptionItem::GetText()
{
	if(m_edit)
		return m_edit->GetText();
	else
		return m_strValue.string();
}

int RingOptionItem::GetInt()
{
	if(m_edit)
		return m_edit->GetInt();
	else
		return m_strValue.toInt();
}	

void RingOptionItem::ActiveOption(RingControls& edit)
{
	edit.SetReadOnly(m_bReadOnly);
}

void RingOptionItem::EnterEdit(int x,int y,int width,int height)
{	
	if(m_edit)
	{
		//MoveWindow(m_edit->Handle(),x,y,width,height,FALSE);
		ActiveOption(*m_edit);
		m_edit->SetPos(x,y,width,height);		
		
		if(!m_edit->IsVisible())
			m_edit->Show();
	}
	else
	{
		m_edit = GetEditCtrl();
		if(m_edit)
		{
			ActiveOption(*m_edit);
			m_edit->SetPos(x,y,width,height);
			if(m_strValue.isClear())
				m_edit->SetValue("");
			else
			{
				m_edit->SetValue(m_strValue.string());
				if(m_edit->GetWindowType() == ITIS_EDIT)
					((RingEdit*)m_edit)->SetSel();
			}
			m_edit->Show();
			m_edit->SetFocus();
		}
	}
}

/////////////////////////////////////////////////
//
//隐藏编辑控件，需要执行绑定的处理函数或发送
//ROPVN_VALUECHANGE消息通知值已改变，并保存处理后的结果.
//
/////////////////////////////////////////////////
BOOL RingOptionItem::ExitEdit()
{
	BOOL bok = FALSE;

	if(m_edit)
	{		
		bok = TRUE;
		try
		{
			if(m_strValue != m_edit->GetText())
			{
				if(m_obj && m_OnCheckValFunc)
					bok = (m_obj->*(m_OnCheckValFunc))(this);
				else
				{
					ROPVNOTIFY opnm;
					
					opnm.hdr.code = ROPVN_VALUECHANGE;
					opnm.hdr.hwndFrom = m_parent->Handle();
					opnm.hdr.idFrom = m_parent->GetId();
					opnm.item = this;
					opnm.lpszOrgText = m_strValue.string();
					opnm.lpszName = GetName();
					opnm.lpszText = m_edit->GetText();
					opnm.lValue = GetInt();
					opnm.lData = GetData();
					opnm.uType = GetType();
					//如果父窗口未对该事件进行处理，SendMessage返回0，因此需置
					//bok为真，允许修改值。父窗口如对该事件进行处理，认为输入值
					//非法，不允许改变必须返回-1，返回其他值则允许修改。					
					if(SendMessage(m_parent->Parent()->Handle(),WM_NOTIFY,
											m_parent->GetCurSel(),(LPARAM)&opnm) == -1)
						bok = FALSE;
					else
						bok = TRUE;
				}
			}
		}
		catch(...)
		{
		}

		if(bok)
			m_strValue = m_edit->GetText();
		else
			m_edit->SetValue(m_strValue.string());

		m_edit->Show(SW_HIDE);		
	}
	m_edit = NULL;
	return bok;
}

void RingOptionItem::UpdateValue()
{
	if(m_edit)
		m_strValue = m_edit->GetText();
}

BOOL RingOptionItem::IsEditVisible()
{
	if(m_edit)
	{
		RECT rc;
		m_edit->GetRect(&rc);
		return (rc.top >= 0 && rc.top < m_edit->Parent()->Height());
	}
	return FALSE;
}

BOOL RingOptionItem::SetValue(int nVal)
{
	m_strValue = nVal;
	if(m_edit)
		return m_edit->SetValue(m_strValue.string());
	else if(m_parent)
		InvalidateRect(m_parent->Handle(),NULL,TRUE);
	return TRUE;
}

BOOL RingOptionItem::SetValue(LPCTSTR lpszVal)
{
	if(lpszVal)
	{
		m_strValue = lpszVal;
		if(m_edit)
			return m_edit->SetValue(lpszVal);
		else if(m_parent)
			InvalidateRect(m_parent->Handle(),NULL,TRUE);
		return TRUE;
	}
	return FALSE;
}

BOOL RingOptionItem::SetValue(LPARAM lVal,LPCTSTR lpszShowVal)
{
	SetData(lVal);
	if(lpszShowVal)
		return SetValue(lpszShowVal);
	else	//lpszShowVal为空，不改变显示值，返回TRUE
		return TRUE;
}

/**********************************************************
//
//RingOptionItemEdit
//
***********************************************************/
void RingOptionItemEdit::ActiveOption(RingControls& edit)
{	
	RingOptionItem::ActiveOption(edit);
	
	if(m_uType & OPPROP_NUMBERONLY)
		edit.ReplaceStyle(0,ES_NUMBER);
	else
		edit.ReplaceStyle(ES_NUMBER,0);
	if(m_uType & OPPROP_PASSWORD)
		SendMessage(edit.Handle(),EM_SETPASSWORDCHAR,(WPARAM)'*',0);
	else
		SendMessage(edit.Handle(),EM_SETPASSWORDCHAR,0,0);
}

void RingOptionItemEdit::DrawValue(HDC hDC,LPRECT lprc)
{
	if(m_edit == NULL)
	{
		if(m_strPsw && (m_uType & OPPROP_PASSWORD))
			DrawText(hDC,m_strPsw->string(),m_strPsw->len(),lprc,DT_LEFT);
		else
			DrawText(hDC,GetText(),m_strValue.len(),lprc,DT_LEFT);
	}
}

///////////////////////////////////////////////////////
//
//ExitEdit为给用户判断是否允许改变值，并保存处理后的结果。
//而OnBeforeNotify由RingOptionView::SendNotify调用，目的是
//当编辑控件是LIST或有按钮时检测是否绑定了SelChangeFunc和
//BrowserFunc，以给机会执行这些函数，EDIT无这两个函数，但
//需要判断是否允许改变值，因为ExitEdit无法捕获所有值改变的
//情况，只有EN_KILLFOCUS消息能捕获，因此这里需要调用CheckValFunc
//或发送ROPVN_VALUECHANGE消息给用户判断是否允许改变值，并
//保存处理后的结果，完成了ExitEdit的功能。由于OnBeforeNotify
//是执行绑定函数，无绑定函数则返回FALSE以发送相应通知消息，
//而EDIT没有这两个消息，因此始终返回TRUE,不发送消息。
//
///////////////////////////////////////////////////////
BOOL RingOptionItemEdit::OnBeforeNotify(int type)
{
	if(type == ROPVN_VALUECHANGE && m_strValue != m_edit->GetText())
	{
		BOOL bok;
		
		if(m_obj && m_OnCheckValFunc)
			bok = (m_obj->*(m_OnCheckValFunc))(this);
		else
		{
			ROPVNOTIFY opnm;
			
			opnm.hdr.code = ROPVN_VALUECHANGE;
			opnm.hdr.hwndFrom = m_parent->Handle();
			opnm.hdr.idFrom = m_parent->GetId();
			opnm.item = this;
			opnm.lpszOrgText = m_strValue.string();
			opnm.lpszName = GetName();
			opnm.lpszText = m_edit->GetText();
			opnm.lValue = GetInt();
			opnm.lData = GetData();
			opnm.uType = GetType();
			//如果父窗口未对该事件进行处理，SendMessage返回0，因此需置
			//bok为真，允许修改值。父窗口如对该事件进行处理，认为输入值
			//非法，不允许改变必须返回-1，返回其他值则允许修改。					
			if(SendMessage(m_parent->Parent()->Handle(),WM_NOTIFY,
				m_parent->GetCurSel(),(LPARAM)&opnm) == -1)
				bok = FALSE;
			else
				bok = TRUE;
		}
		if(bok)
			m_strValue = m_edit->GetText();
		else
			m_edit->SetValue(m_strValue.string());
	}
	return TRUE;
}

BOOL RingOptionItemEdit::ExitEdit()
{
	//值改变判断已由OnBeforeNotify执行，这里只需简单隐藏即可
	if(m_edit)
	{		
		m_strValue = m_edit->GetText();
		m_edit->Show(SW_HIDE);		
	}
	m_edit = NULL;

	if(m_strPsw && (m_uType & OPPROP_PASSWORD))
	{
		*m_strPsw = m_strValue;
		memset((LPSTR)m_strPsw->string(),'*',m_strPsw->len());
	}
	return TRUE;
}

BOOL RingOptionItemEdit::SetValue(int nVal)
{
	if((m_uType & OPPROP_PASSWORD))
	{
		if(m_strPsw == NULL)
			m_strPsw = new ringStr;
		if(m_strPsw)
		{
			*m_strPsw = nVal;
			memset((LPSTR)m_strPsw->string(),'*',m_strPsw->len());
		}
	}
	return RingOptionItem::SetValue(nVal);
}

BOOL RingOptionItemEdit::SetValue(LPCTSTR lpszVal)
{
	if((m_uType & OPPROP_PASSWORD))
	{
		if(m_strPsw == NULL)
			m_strPsw = new ringStr;
		if(m_strPsw)
		{
			*m_strPsw = lpszVal;
			memset((LPSTR)m_strPsw->string(),'*',m_strPsw->len());
		}
	}
	return RingOptionItem::SetValue(lpszVal);
}

/**********************************************************
//
//RingOptionItemList
//
***********************************************************/
BOOL RingOptionItemList::InterSetValue(LPARAM lVal,LPCTSTR lpszShowVal)
{
	SetData(lVal);
	m_strValue = lpszShowVal;
	if(m_edit)
	{
		((RingCombo*)m_edit)->AddStrings(m_strList.string());
		m_edit->SetValue(lpszShowVal);
	}
	return TRUE;
}

//返回显示值
LPCTSTR RingOptionItemList::GetText()
{
	if(m_edit)
	{
		RingCombo* combo = (RingCombo*)m_edit;
		int index = combo->GetCurSel();
		if(index == CB_ERR)
			return combo->GetText();
		else
			return combo->GetLBText(index);
	}
	else
		return m_strValue.string();
}

//得到当前选择项对应的数据,如没有设置过隐含值，返回0
//本函数不返回显示值的转换数字值
int RingOptionItemList::GetInt()
{
	if(m_strList.isClear() || m_selData == NULL)
		return 0;
		
	if(m_edit)
	{
		RingCombo* combo = (RingCombo*)m_edit;
		if(combo->GetCurSel() != CB_ERR)
			return combo->GetData(combo->GetCurSel());
		else
			return 0;
	}
	else
		return GetItemData();		
}

BOOL RingOptionItemList::SetValue(int nVal)
{
	if(IsReadOnly())
		return SetSelectIndex(nVal);
	else
		return RingOptionItem::SetValue(nVal);
}

BOOL RingOptionItemList::SetValue(LPCTSTR lpszVal)
{
	if(IsReadOnly() && FindString(lpszVal) == LB_ERR)
		return FALSE;
	else
		return RingOptionItem::SetValue(lpszVal);
}

int RingOptionItemList::GetSelectIndex()
{
	if(m_strList.isClear())
		return LB_ERR;
				
	if(m_edit)
		return ((RingCombo*)m_edit)->GetCurSel();
	else
		return FindString(m_strValue.string());
	return LB_ERR;
}

//在选项中查找lpString,必须完全匹配，即该选项必须前后有"|"分隔符，
//但最前和最后一个选项要一些特殊判断，返回lpString所在选项序号
int RingOptionItemList::FindString(LPCTSTR lpString)
{
	if(lpString == NULL || *lpString == '\0' || m_strList.isClear())
		return LB_ERR;

	if(m_edit)
		return ((RingCombo*)m_edit)->FindMatchString(lpString);
	else
	{
		int index = 0;
		int len = lstrlen(lpString),pos = 0;

		do 
		{
			pos = m_strList.indexOf(lpString,pos) + 1;
			if(pos == 0)	 //indexOf返回-1表示未找到，+1后=0
				return LB_ERR;
		}while(!((pos == 1 || m_strList[pos - 2] == _T('|')) && 
			(m_strList[(int)(pos + len - 1)] == _T('|') || 
			m_strList.len() == pos + len - 1)));//必须全部匹配
		
		if(pos == 1)
			return 0;
		else
		{
			len = 1;
			do
			{
				len = m_strList.indexOf("|",len) + 1;
				index ++;
			}while(len != pos - 1 && len != 0);
			return index;
		}
	}
	return LB_ERR;
}

BOOL RingOptionItemList::SetSelectIndex(int index)
{
	if(m_strList.isClear() || index < 0 || index >= m_nItemCnt)
		return FALSE;
		
	if(m_edit)
		return ((RingCombo*)m_edit)->Select(index);
	else
	{
		if(index > 0)
		{
			index = m_strList.indexOf("|",0,index);
			if(index == -1)
				return FALSE;
			else
				index ++;
		}

		int e = m_strList.indexOf("|",index);
		if(e == -1)
			e = m_strList.len();
		if(e <= index)
			return FALSE;
		else
		{
			m_strValue = m_strList.SubString(index,e-index);
			if(m_parent)
				InvalidateRect(m_parent->Handle(),NULL,TRUE);
		}
	}
	return TRUE;
}

//根据绑定值设置选项
BOOL RingOptionItemList::SetSelectIndexByData(LONG lData)
{
	if(m_strList.isClear() || m_selData == NULL)
		return FALSE;
		
	int index = -1;
	for(int i=0;i<m_nItemCnt;i++)
	{
		if(m_selData[i] == lData)
		{
			index = i;
			break;
		}
	}
	return SetSelectIndex(index);
}

void RingOptionItemList::EnterEdit(int x,int y,int width,int height)
{
	//由于WM_DRAWITEM也会调用本函数，RingOptionItem::EnterEdit执行过后
	//m_edit && m_edit->IsVisible()条件会成立，因此下面的这个if...else
	//必须如此判断，不可优化为先执行EnterEdit再判断，否则判断永远成立。
	if(m_edit && m_edit->IsVisible())
	{
		RingOptionItem::EnterEdit(x,y,width,height);
		return;
	}
	else
		RingOptionItem::EnterEdit(x,y,width,height);
	
	RingCombo* combo = (RingCombo*)m_edit;
	if(combo)
	{
		combo->Clear();
		if(!m_strList.isClear())
		{
			combo->AddStrings(m_strList.string());
			//设置选项数据
			if(m_selData)
			{
				for(int i=0;i<m_nItemCnt;i++)
					combo->SetData(i,m_selData[i]);
			}
			combo->SetValue(m_strValue.string());
		}
	}
}

//设置选项，必须先清除全部选项才可调用
BOOL RingOptionItemList::SetSelection(LPCTSTR lpString)
{
	if(lpString && m_strList.isClear())
	{
		m_strList = lpString;
		m_nItemCnt = m_strList.count(_T("|")) + 1;
		if(m_edit)
		{
			((RingCombo*)m_edit)->Clear();
			((RingCombo*)m_edit)->AddStrings(lpString);
		}
		return TRUE;
	}
	else
		return FALSE;
}

BOOL RingOptionItemList::AddSelection(LPCTSTR lpString,LONG lData)
{
	if(lpString)
	{
		if(m_strList.isClear())
			m_strList = lpString;
		else
			m_strList = m_strList + _T("|") + lpString;

		m_nItemCnt ++;

		if(lData != 0)
			SetItemData(lData,m_nItemCnt-1);

		if(m_edit)
			((RingCombo*)m_edit)->AddString(lpString,lData);
		return TRUE;
	}
	else
		return FALSE;
}

void RingOptionItemList::SetItemData(LPARAM lParam,int index)
{
	LPLONG lpl = m_selData;
	int nSize = 0;
	if(m_selData == NULL || m_nItemCnt > m_nDataSize)
	{
		nSize = m_nDataSize;
		m_nDataSize = m_nItemCnt + 1;
		m_selData = (LPLONG)New(sizeof(LONG) * m_nDataSize);
	}
	if(m_selData)
	{
		if(lpl && nSize > 0)
		{
			memcpy(m_selData,lpl,nSize*sizeof(LONG));
			Del(lpl);
		}

		if(index >= 0 && index < m_nItemCnt)
			m_selData[index] = lParam;
	}
	else
		m_nDataSize = 0;
}

BOOL RingOptionItemList::DelSelection(LPCTSTR lpString)
{
	if(lpString)
	{
		int index = FindString(lpString);
		if(index != LB_ERR)
		{
			m_strList = m_strList.replace(lpString,"");
			m_strList = m_strList.replace("||","|");
			if(m_strList[m_strList.len()-1] == '|')
			{
				m_strList[m_strList.len()-1] = '\0';
				m_strList.GetLength();
			}
			if(m_edit)
				((RingCombo*)m_edit)->DelString(lpString);

			m_nItemCnt --;
			if(m_selData)
			{
				//后面的数据往前移
				for(int i=index;i<m_nItemCnt;i++)
					m_selData[i] = m_selData[i+1];
				m_selData[i] = 0;
			}
			if(IsReadOnly() && m_strValue == lpString)
			{
				m_strValue.Clear();
				if(m_parent)
					InvalidateRect(m_parent->Handle(),NULL,TRUE);
			}
			return TRUE;
		}
	}
	return FALSE;
}

void RingOptionItemList::Clear()
{
	if(m_selData && m_nDataSize > 0)
		RtlZeroMemory(m_selData,m_nDataSize);

	m_strList.Clear();

	if(m_edit)
		m_edit->Clear();
	if(m_parent)
		InvalidateRect(m_parent->Handle(),NULL,TRUE);
}

BOOL RingOptionItemList::SetOption(LONG lConfig)
{
	try
	{
		ringStr str = (LPSTR)lConfig;
		int pos = str.indexOf("||");
		if(pos > 0)
		{
			m_strList = str.SubString(0,pos);
			m_nItemCnt = m_strList.count("|") + 1;

			ringStr strData = str.SubString(pos + 2);
			int i,c = 0;
			LPTSTR s;

			for(i=0;i<m_nItemCnt;i++)
			{ 
				s = (LPTSTR)strData.string() + c;
				c = strData.indexOf("|",c) + 1;
				
				//str的operator[]总是安全的,如果是\，表示该|字符是经转义，不作为分隔符
				if(strData[c-2] == '\\')
					continue;
				strData[c-1] = '\0';
				if(*s != '\0')
					SetItemData(atoi(s),i);
			}
		}
		else
		{
			m_strList = str;
			m_nItemCnt = m_strList.count("|") + 1;
		}
		return TRUE;
	}
	catch(...)
	{
		return FALSE;
	}
}

BOOL RingOptionItemList::OnBeforeNotify(int type)
{
	if(type == ROPVN_SELCHANGE)
	{
		if(m_objSel && m_SelectFunc)
		{
			(m_objSel->*(m_SelectFunc))(this);
			return TRUE;
		}
	}
	return FALSE;
}

LONG RingOptionItemList::GetItemData(int index)
{
	if(index == INDEX_CURSEL)
		index = FindString(m_strValue.string());
			
	if(index >= 0 && index < m_nItemCnt && m_selData)
		return m_selData[index];
	else
		return 0;
}

/**********************************************************
//
//RingOptionItemSel
//
***********************************************************/
void RingOptionItemSel::EnterEdit(int x,int y,int width,int height)
{	
	RingOptionItem::EnterEdit(x,y,width-height+1,height);
	
	if(m_button)
	{
		if(!m_button->IsVisible())
			m_button->Show();
		//MoveWindow(m_button->Handle(),x+width-height+1,y+1,height-2,height-2,FALSE);
		m_button->SetPos(x+width-height+1,y+1,height-2,height-2);
	}
	else
	{
		m_button = GetParent()?GetParent()->GetButton():NULL;
		if(m_button)
		{
			//m_button->SetParent(m_edit->Handle());
			//MoveWindow(m_button->Handle(),x+width-height+1,y+1,height-2,height-2,FALSE);
			m_button->SetPos(x+width-height+1,y+1,height-2,height-2);
		}
	}
}

//返回值仅RingOptionItem::ExitEdit()有意义，供RingOptionItem::ExitEdit()判断使用
//这里返回FALSE
BOOL RingOptionItemSel::ExitEdit()
{
	RingOptionItem::ExitEdit();

	if(m_button)
	{
		m_button->Show(SW_HIDE);
	}
	m_button = NULL;
	return FALSE;
}

BOOL RingOptionItemSel::OnBeforeNotify(int type)
{
	if(type == ROPVN_BUTTONCLICK)
	{
		if(m_bIsSelectFile)
		{
			ringFile rf;
			if(rf.Select(m_szFileExt))
				SetValue(rf.Fullname());
			return TRUE;
		}
		else
		{
			if(m_objBrowser && m_BrowserFunc)
			{
				(m_objBrowser->*(m_BrowserFunc))(this);
				return TRUE;
			}
		}
	}
	return FALSE;
}
/*
BOOL RingOptionItemSel::SetValue(LPARAM lVal,LPCTSTR lpszShowVal)
{
	SetItemData(lVal);
	if(lpszShowVal)
		return SetValue(lpszShowVal);
	else
		return TRUE;
}
*/
BOOL RingOptionItemSel::SetOption(LONG lConfig)
{
	try
	{
		if(lConfig)
		{
			memset(m_szFileExt,0,80);
			lstrcpyn(m_szFileExt,(LPCTSTR)lConfig,79);
			m_bIsSelectFile = TRUE;			
		}
		else
			m_bIsSelectFile = FALSE;
		return TRUE;
	}
	catch(...)
	{
	}
	return FALSE;
}

/**********************************************************
//
//RingOptionItemPack
//
***********************************************************/
RingOptionItemPack::RingOptionItemPack()
{
	m_SubItem = NULL;
	m_bExpand = TRUE;
}

RingOptionItemPack::~RingOptionItemPack()
{
	Clear();
}

void RingOptionItemPack::Clear()
{
	LPRPDATAPACKITEM save,tmp = m_SubItem;
	HWND hWnd = NULL;
	DELETEITEMSTRUCT ds;
	int index = 0;

	if(m_parent && IsWindowPtr(m_parent->Parent()))
	{
		hWnd = m_parent->Parent()->Handle();
		ds.CtlID = GetIndex();
		ds.CtlType = ODT_LISTBOX;
		ds.hwndItem = m_parent->Handle();
	}

	while(tmp)
	{
		if(tmp->m_item && tmp->m_item->m_bAutoDelete)
		{
			if(hWnd)
			{
				ds.itemID = index;
				ds.itemData = tmp->m_item->GetData();
				if(ds.itemData != 0)
					SendMessage(hWnd,WM_DELETEITEM,m_parent->GetId(),(LPARAM)&ds);
			}
			delete tmp->m_item;
		}
		save = tmp->m_next;
		Del(tmp);
		tmp = save;
	}
	m_SubItem = NULL;
}

void RingOptionItemPack::Reset()
{
	LPRPDATAPACKITEM tmp = m_SubItem;
	
	while(tmp)
	{
		if(tmp->m_item)
			tmp->m_item->Reset();
		tmp = tmp->m_next;		
	}	
}

void RingOptionItemPack::DrawValue(HDC hDC,LPRECT lprc)
{
	if(lprc && m_SubItem)
	{		
		lprc->left = (m_parent->GetLeftMargin() - 8) / 2;
		lprc->right = lprc->left + 9;
		lprc->top = (lprc->bottom + lprc->top - 9)/2 - 1;
		lprc->bottom = lprc->top + 9;

		if(lprc->right <= 0)
			return;

		FrameRect(hDC,lprc,(HBRUSH)GetStockObject(BLACK_BRUSH));

		SelectObject(hDC,(HBRUSH)GetStockObject(BLACK_PEN));
		MoveToEx(hDC,lprc->left + 2,lprc->top + 4,NULL);
		LineTo(hDC,lprc->left + 7,lprc->top + 4);
		if(!m_bExpand)
		{
			MoveToEx(hDC,lprc->left + 4,lprc->top + 2,NULL);
			LineTo(hDC,lprc->left + 4,lprc->top + 7);
		}
	}
}

void RingOptionItemPack::Expand()
{	
	if(m_SubItem && !m_bExpand && m_parent)
	{
		LPRPDATAPACKITEM tmp = m_SubItem;
		int index,org = GetIndex();//m_parent->FindMatchString(m_strName.string());
		m_bExpand = TRUE;
		index = org;
		while(index != LB_ERR && index != LB_ERRSPACE)
		{
			index ++;
			if(tmp->m_item)
				index = m_parent->InsertOption(tmp->m_item,index);
			if(tmp->m_next)
				tmp = tmp->m_next;
			else
			{
				//调整LISTBOX使整个包可见
				int n = m_parent->Height() / m_parent->GetItemHeight();
				int h = m_parent->GetTopIndex();
				if(index - h > n)
				{
					index = (index - org) > n?org:(index - n + 1);
					m_parent->SetTopIndex(index);
					m_parent->SetCurSel(org);
				}
				break;
			}
		}
	}
}

void RingOptionItemPack::Collapse()
{
	if(m_parent && m_bExpand)
	{
		LPRPDATAPACKITEM tmp = m_SubItem;
		int index = GetIndex();
		m_bExpand = FALSE;

		if(index != LB_ERR)
		{
			index ++;
			while(tmp)
			{
				if(tmp->m_item && tmp->m_item->GetParent())
					SendMessage(m_parent->Handle(),LB_DELETESTRING,index,DELITEM_HIDE);
				tmp = tmp->m_next;
			}
		}
	}
}

void RingOptionItemPack::AutoExpand()
{
	if(m_bExpand)
		Collapse();
	else
		Expand();
}

int RingOptionItemPack::AddSubItem(RingOptionItem* item)
{
	if(item)
	{		
		//已加入到OptionView中的项目不能再加入
		if(item->GetParent())
			return LB_ERR;

		LPRPDATAPACKITEM prev = NULL,tmp = m_SubItem;
		int cnt = 0;

		while(tmp)
		{
			prev = tmp;
			tmp = tmp->m_next;
			cnt ++;
		}

		if(prev == NULL)
		{
			m_SubItem = (LPRPDATAPACKITEM)New(sizeof(RPDATAPACKITEM));
			prev = m_SubItem;
			if(m_parent)
			{
				m_parent->SetLeftMargin(ROPV_LEFTMARGIN);
				InvalidateRect(m_parent->Handle(),NULL,TRUE);
			}
		}
		else
		{
			prev->m_next = (LPRPDATAPACKITEM)New(sizeof(RPDATAPACKITEM));
			prev = prev->m_next;
		}
		
		prev->m_item = item;
		item->m_parentItem = this;

		if(m_bExpand && m_parent)
		{
			int index = GetIndex();
			if(index != LB_ERR)
				return m_parent->InsertOption(item,index + cnt + 1);
		}
		return -(1000 + cnt);
	}
	return LB_ERR;
}

BOOL RingOptionItemPack::DelSubItem(RingOptionItem* item)
{
	if(item && item->m_parentItem == this)
	{		
		LPRPDATAPACKITEM prev = NULL,tmp = m_SubItem;
		int index = 0;
		DELETEITEMSTRUCT ds;

		while(tmp && tmp->m_item != item)
		{
			prev = tmp;
			tmp = tmp->m_next;
			index ++;
		}
		if(tmp == NULL)
			return FALSE;

		if(prev == NULL)
			m_SubItem = m_SubItem->m_next;
		else
			prev->m_next = tmp->m_next;
		
		item->m_parentItem = NULL;
		tmp->m_next = NULL;
		
		if(item->m_bAutoDelete)
		{
			if(m_parent && IsWindowPtr(m_parent->Parent()))
			{
				HWND hWnd = m_parent->Parent()->Handle();
				if(hWnd)
				{
					ds.CtlID = GetIndex();
					ds.CtlType = ODT_LISTBOX;
					ds.hwndItem = m_parent->Handle();
					ds.itemID = index;
					ds.itemData = item->GetData();
					SendMessage(hWnd,WM_DELETEITEM,m_parent->GetId(),(LPARAM)&ds);
				}
			}
			delete item;
			Del(tmp);
		}
		return TRUE;
	}
	return FALSE;
}

int RingOptionItemPack::GetSubItemIndex(LPCTSTR lpszName)
{
	LPRPDATAPACKITEM tmp = m_SubItem;
	int index = -1;
	
	while(tmp && tmp->m_item)
	{
		index ++;
		if(strcmp(tmp->m_item->GetName(),lpszName) == 0)
			return index;
		tmp = tmp->m_next;		
	}
	return index;
}

int RingOptionItemPack::GetSubItemCount()
{
	int cnt = 0;
	LPRPDATAPACKITEM tmp = m_SubItem;
	
	while(tmp && tmp->m_item)
	{
		cnt ++;
		tmp = tmp->m_next;		
	}
	return cnt;
}

RingOptionItem* RingOptionItemPack::GetSubItem(int nIndex)
{
	LPRPDATAPACKITEM tmp = m_SubItem;
	int idx = 0;

	while(tmp && tmp->m_item)
	{
		if(nIndex == idx++)
			return tmp->m_item;

		tmp = tmp->m_next;		
	}
	return NULL;
}

RingOptionItem* RingOptionItemPack::GetSubItem(LPCTSTR lpszName)
{
	LPRPDATAPACKITEM tmp = m_SubItem;
	
	while(tmp && tmp->m_item)
	{
		if(strcmp(tmp->m_item->GetName(),lpszName) == 0)
			return tmp->m_item;
		tmp = tmp->m_next;		
	}
	return NULL;
}
