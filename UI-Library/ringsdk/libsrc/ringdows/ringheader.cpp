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
//原文件名：ringheader.cpp
//
//说明：header控件控制
//
//功能扩展：作为ListView的Header允许隐藏/显示列，为避免ListView删除和插入
//				列需要重新更新每一项的代价，采用设置列宽为0的方式，但是用户
//				拖动列头会显示出隐藏的列，因此在窗口过程做了处理，除第一列外
//				（第一列不可删除）其他列头均设置一个LONG[2]数组，LONG[0]为
//				用户设置的数据，LONG[1]为[隐藏标志1：隐藏，0：显示（最高位）]
//				[原序号（ORDER序号，3BIT）][原宽度（低4位）]，因为用户拖动列
//				时并不发送HDM_HITTEST消息来检测鼠标位于什么位置，所以不能通过
//				处理HDM_HITTEST消息改变返回值来达到目的。只能处理为隐藏列全部
//				移动到最前面，这样鼠标只有在最左边时才会变为拉开状态，此时处理
//				WM_LBUTTONDOWN和WM_LBUTTONDBCLK消息，通过HDM_HITTEST消息检测
//				如果是HHT_ONDIVOPEN且该列隐藏则直接返回，这样就达到了目的。
//
**********************************************************************/
#define MAKE_SELF_LIB
#include "ringdows.h"
#include <malloc.h>

RingHeader::RingHeader()
{
	m_hImage = NULL;
	m_CoolMenu = NULL;
	m_bCanHideColumn = FALSE;
	m_nHideColCnt = 0;
}

RingHeader::~RingHeader()
{
	if(m_hImage)
		ImageList_Destroy(m_hImage);
	delete m_CoolMenu;
}

void RingHeader::InitImageList(int cx,int cy)
{
	if(m_hImage == NULL)
	{
		m_hImage = ImageList_Create(cx,cy,ILC_COLOR32|ILC_MASK,5,5);
		Header_SetImageList(m_hWnd,m_hImage);
	}
}

////////////////////////////////////////////////////
//
//添加图标，必须已设置过IMAGELIST或调用过InitImageList(),
//
//返回：图标序号
//
////////////////////////////////////////////////////
int RingHeader::AddIcon(LPCTSTR lpszIcon,HINSTANCE hInst)
{
	//因为有可能调用过SetImageList，因此不能使用内置的m_hImage,
	//必须Header_GetImageList
	HIMAGELIST himl = Header_GetImageList(m_hWnd);
	HICON hicon;
	int x,y;
	int nIndex = -1;
	
	if(himl == NULL)
		InitImageList();

	if(himl)
	{
		if(!ImageList_GetIconSize(himl,&x,&y))
			x = y = 16;
		
		hicon = (HICON)LoadImage(hInst,lpszIcon,IMAGE_ICON,x,y,LR_DEFAULTCOLOR);
		if(hicon)
			nIndex = ImageList_AddIcon(himl,hicon);
	}
	
	return nIndex;
}

int RingHeader::AddIcon(UINT uidIcon,HINSTANCE hInst)
{
	return AddIcon(MAKEINTRESOURCE(uidIcon),hInst);
}

int RingHeader::AddIcon(HICON hicon)
{
	if(hicon == NULL)
		return -1;
	
	int nIndex = -1;
	HIMAGELIST himl = Header_GetImageList(m_hWnd);
	if(himl == NULL)
		InitImageList();

	if(himl)
		nIndex = ImageList_AddIcon(himl,hicon);
	
	return nIndex;
}

BOOL RingHeader::SetItemWidth(int iCol,int nWidth)
{
	HDITEM hdi;
	hdi.mask = HDI_WIDTH;
	hdi.cxy = nWidth;
	return Header_SetItem(m_hWnd,iCol,&hdi);
}

LONG RingHeader::GetData(int index)
{
	HDITEM hdi;

	hdi.mask = HDI_LPARAM;
	hdi.lParam = 0;

	Header_GetItem(m_hWnd,index,&hdi);
	return hdi.lParam;
}

BOOL RingHeader::SetData(int index,LONG lParam)
{
	HDITEM hdi;
	
	hdi.mask = HDI_LPARAM;
	hdi.lParam = lParam;
	
	return Header_SetItem(m_hWnd,index,&hdi);
}

LPLONG RingHeader::GetRealData(int index)
{
	HDITEM hdi;
	
	hdi.mask = HDI_LPARAM;
	hdi.lParam = 0;
	
	CallWindowProc(m_DefProc,m_hWnd,HDM_GETITEM,index,(LPARAM)&hdi);
	return (LPLONG)hdi.lParam;
}

BOOL RingHeader::SetRealData(int index,LONG lParam)
{
	HDITEM hdi;
	
	hdi.mask = HDI_LPARAM;
	hdi.lParam = lParam;
	
	return CallWindowProc(m_DefProc,m_hWnd,HDM_SETITEM,index,(LPARAM)&hdi);
}

LPLONG RingHeader::GetHDInfo(int index)
{
	LPLONG lpl = GetRealData(index);
	if(lpl)
		return lpl + 1;
	else
		return NULL;
}

//子项操作
int RingHeader::AddItem(int index,LPCTSTR szText,LPARAM lParam,int idIcon)
{
	HDITEM hdi;
	
	hdi.mask = 0;
	
	if(idIcon >= 0)
	{
		hdi.mask = HDI_IMAGE;
		hdi.iImage = idIcon;
	}
	
	if(szText)
	{
		hdi.mask |= HDI_TEXT;
		hdi.pszText = (LPSTR)szText;
		hdi.cchTextMax = strlen(szText);
	}	
	return Header_InsertItem(m_hWnd,index,&hdi);
}

int RingHeader::AddItem(int index,int nVal,LPARAM lParam,int idIcon)
{
	char s[16];
	wsprintf(s,"%d\0",nVal);
	return AddItem(index,s,lParam,idIcon);
}

void RingHeader::Select(int index,int idIcon)
{
	HDITEM hdi;
	
	hdi.mask = HDI_IMAGE;
	hdi.iImage = idIcon;
	
	Header_SetItem(m_hWnd,index,&hdi);
}

//虚函数重载
BOOL RingHeader::SetValue(int val,int index)
{
	char szText[16];
	wsprintf(szText,"%d\0",val);
	return SetValue(szText,index);
}

BOOL RingHeader::SetValue(LPCTSTR lpszVal,int index)
{
	HDITEM hdi;
	
	hdi.mask = 0;
	
	if(lpszVal)
	{
		hdi.mask = HDI_TEXT;
		hdi.pszText = (LPSTR)lpszVal;
		hdi.cchTextMax = strlen(lpszVal);
	}	
	return Header_SetItem(m_hWnd,index,&hdi);
}

int RingHeader::GetOrder(int index)
{
	HDITEM hdi;
	
	hdi.mask = HDI_ORDER;
	
	if(Header_GetItem(m_hWnd,index,&hdi))
		return hdi.iOrder;
	else
		return -1;
}

int RingHeader::GetItemText(int index,LPTSTR lpBuf,int size)
{
	HDITEM hdi;
	
	hdi.mask = HDI_TEXT;
	hdi.pszText = (LPSTR)lpBuf;
	hdi.cchTextMax = size;

	if(Header_GetItem(m_hWnd,index,&hdi))
		return lstrlen(lpBuf);
	else
		return 0;
}

BOOL RingHeader::IsColumnHide(int iCol)
{
	LPLONG lpl = GetRealData(iCol);
	if(lpl)
		return HDCOL_ISHIDDEN(lpl[1]);
	else
		return FALSE;
}

//获取列头宽度，nMode:如果列头未隐藏，返回实际宽度，
//如果列头已隐藏且nMode=SW_HIDE，返回未隐藏前的宽度
int RingHeader::GetItemWidth(int iCol,int nMode/*=SW_SHOW*/)
{
	HDITEM hd;
	hd.mask = HDI_WIDTH|HDI_LPARAM;

	if(CallWindowProc(m_DefProc,m_hWnd,HDM_GETITEM,iCol,(LPARAM)&hd))
	{
		if(m_bCanHideColumn && nMode == SW_HIDE)
		{
			LPLONG lpl = (LPLONG)hd.lParam;
			if(lpl)
				return LOWORD(lpl[1]);
		}
		else
			return hd.cxy;
	}
	return -1;
}

///////////////////////////////////////////////////////////
//
//本窗口过程主要对绑定数据进行处理，由于允许显示/隐藏列头，
//因此每个列头的DATA都是一个LONG[2]数组，第一个为用户设置
//的数据，第二个为记录列头宽度和显示与否标记
//
//第一列不可隐藏
//
///////////////////////////////////////////////////////////
LRESULT RingHeader::RingdowProc(HWND hWnd,RINGPARAMS param)
{
	LRESULT res;
	switch(param.uMsg)
	{
		case HDM_DELETEITEM:
			if(m_bCanHideColumn && param.wParam != 0)
			{
				//销毁列头宽度和显示与否标记，恢复用户数据
				LPLONG lpl = GetRealData(param.wParam);
				if(lpl)
				{
					LONG lUserData = *lpl;
					Del(lpl);
					SetRealData(param.wParam,lUserData);
				}
			}
			break;
		case HDM_INSERTITEM:
			res = CallWindowProc(m_DefProc,m_hWnd,param.uMsg,param.wParam,param.lParam);
			if(res != -1 && m_bCanHideColumn)
			{
				//初始化用户数据
				LPLONG lpl = (LPLONG)New(sizeof(LONG)*2);
				if(lpl)
				{
					LPHDITEM phdi = (LPHDITEM)param.lParam;
					if(MASK_MATCH(phdi->mask,HDI_LPARAM))
						*lpl = phdi->lParam;
					phdi->mask = HDI_LPARAM;
					phdi->lParam = (LONG)lpl;
					lpl[1] = MAKELONG(GetItemWidth(res),res);
					CallWindowProc(m_DefProc,m_hWnd,HDM_SETITEM,res,(LPARAM)phdi);
				}
			}
			return res;
		case HDM_GETITEM:
			res = CallWindowProc(m_DefProc,m_hWnd,param.uMsg,param.wParam,param.lParam);
			if(m_bCanHideColumn && param.wParam != 0)
			{
				//设置用户数据返回
				LPHDITEM lphd = (LPHDITEM)param.lParam;
				if(res && lphd && MASK_MATCH(lphd->mask,HDI_LPARAM) && lphd->lParam)
					lphd->lParam = *((LPLONG)lphd->lParam);
			}
			return res;			
		case HDM_SETITEM:
			if(m_bCanHideColumn && param.wParam != 0)
			{
				LPHDITEM lphd = (LPHDITEM)param.lParam;
				if(lphd && (lphd->mask & (HDI_LPARAM|HDI_WIDTH)))
				{
					//设置数据，需要获取原数据数组
					BOOL bHide = FALSE;
					HDITEM hd;
					hd.mask = HDI_LPARAM|HDI_WIDTH;
					hd.lParam = 0;
					if(CallWindowProc(m_DefProc,m_hWnd,HDM_GETITEM,param.wParam,(LONG)&hd))
					{
						if(hd.lParam)
						{
							LPLONG lpl = (LPLONG)hd.lParam;
							if(MASK_MATCH(lphd->mask,HDI_LPARAM))
								*lpl = lphd->lParam;
							//设置宽度，记录
							if(MASK_MATCH(lphd->mask,HDI_WIDTH))
							{
								bHide = HDCOL_ISHIDDEN(lpl[1]);
								lpl[1] &= 0xFFFF0000;
								lpl[1] |= lphd->cxy;
							}
						}						
					}
					memcpy(&hd,lphd,sizeof(HDITEM));
					//数据已设置，取消该标志
					hd.mask &= ~HDI_LPARAM;
					if(bHide && hd.cxy != 0)	//该列隐藏且设置宽度不为0，仅在数据中设置宽度，不实际设置
						hd.mask &= ~HDI_WIDTH;					
					return CallWindowProc(m_DefProc,m_hWnd,param.uMsg,param.wParam,(LPARAM)&hd);
				}
			}
			break;
		case HDM_ENABLECOLUMNHIDE:
			//自定义消息，允许/禁止隐藏列头，wParam为允许/禁止标志
			if((m_bCanHideColumn && !param.wParam) || (param.wParam && !m_bCanHideColumn))
			{
				HDITEM hdi;				
				hdi.mask = HDI_LPARAM;
				int i,nCnt = GetCount();
				LPLONG lpl;
				//状态转变
				if(m_bCanHideColumn)
				{
					//原允许，删除所有列头内部信息数据
					for(i=1;i<nCnt;i++)
					{
						if(CallWindowProc(m_DefProc,m_hWnd,HDM_GETITEM,i,(LPARAM)&hdi))
						{
							if(hdi.lParam)
							{
								lpl = (LPLONG)hdi.lParam;
								hdi.lParam = *lpl;
								CallWindowProc(m_DefProc,m_hWnd,HDM_SETITEM,i,(LPARAM)&hdi);
								Del(lpl);
							}
						}
					}
				}
				else
				{
					//原禁止，创建所有列头内部信息数据
					for(i=1;i<nCnt;i++)
					{
						if(CallWindowProc(m_DefProc,m_hWnd,HDM_GETITEM,i,(LPARAM)&hdi))
						{
							lpl = (LPLONG)New(sizeof(LONG)*2);
							if(lpl)
							{
								*lpl = hdi.lParam;
								hdi.lParam = (LONG)lpl;
								lpl[1] = GetItemWidth(i) | (i << 16);
								CallWindowProc(m_DefProc,m_hWnd,HDM_SETITEM,i,(LPARAM)&hdi);
							}
						}
					}
				}
				m_bCanHideColumn = param.wParam;
			}
			return 0;
		case HDM_SHOWCOLUMN:
			//置指定列头显示/隐藏标记,第一列不可隐藏
			if(param.wParam > 0)
			{
				if(!m_bCanHideColumn)
					SendMessage(m_hWnd,HDM_ENABLECOLUMNHIDE,TRUE,0);
				
				HDITEM hd;
				hd.mask = HDI_LPARAM;
				hd.lParam = 0;
				if(CallWindowProc(m_DefProc,m_hWnd,HDM_GETITEM,param.wParam,(LPARAM)&hd))
				{
					LPLONG lpl = (LPLONG)hd.lParam+1;
					if(lpl)
					{
						if(m_CoolMenu)
							m_CoolMenu->CheckItem(param.wParam,HDCOL_ISHIDDEN(*lpl),BY_POSITION);

						res = LOWORD(*lpl);
						//获取序号
						hd.mask = HDI_ORDER;
						hd.iOrder = param.wParam;
						CallWindowProc(m_DefProc,m_hWnd,HDM_GETITEM,param.wParam,(LPARAM)&hd);

						//显示lParam为TRUE，但记录时隐藏为TRUE，因此需反一下
						if(param.lParam)
						{
							if(HDCOL_ISHIDDEN(*lpl))
							{
								//显示,移动到原位置
								int pre=m_nHideColCnt,cnt = GetCount();
								LPINT lpi = (LPINT)_alloca(cnt*sizeof(int));
								GetOrderArray(cnt,lpi);
								hd.iOrder = HDCOL_EXTRACTORDER(*lpl);
								for(int i=m_nHideColCnt;i<cnt;i++)
								{
									if(hd.iOrder > lpi[i])
										pre = i;
									else
									{
										i = pre;
										break;
									}
								}
								m_nHideColCnt --;
								//hd.iOrder = HDCOL_EXTRACTORDER(*lpl) + m_nHideColCnt;
								*lpl = MAKELONG(res,hd.iOrder);
								hd.iOrder = min(i,cnt-1);								
							}
						}
						else
						{
							if(!HDCOL_ISHIDDEN(*lpl))
							{
								//隐藏
								*lpl = res | HDCOL_HIDEMASK | (HDCOL_EXTRACTORDER(*lpl)<<16);//((max(0,hd.iOrder-m_nHideColCnt)<<16)&0x0FFF0000);
								//移到最前面
								hd.iOrder = 0;
								m_nHideColCnt++;
							}
						}
						CallWindowProc(m_DefProc,m_hWnd,HDM_SETITEM,param.wParam,(LPARAM)&hd);
						return res;
					}
				}
			}
			return 0;
		case WM_LBUTTONDOWN:
		case WM_LBUTTONDBLCLK:
			if(IsMouseOnHiddenCol(param.mousept.x,param.mousept.y))
				return 0;
			break;
		case WM_RBUTTONDOWN:
			if(m_bCanHideColumn && m_CoolMenu)
				m_CoolMenu->Popup(m_hWnd);
			break;
		case WM_COMMAND:
			if(m_bCanHideColumn && m_CoolMenu && param.shortval.high == 0)
			{
				//菜单消息
				NMHEADER nmhd;
				int nIndex = param.shortval.low;
				BOOL bShow = (m_CoolMenu->GetItemCheckState(nIndex) == MF_UNCHECKED);
				HWND hWndP;
				if(Parent()->GetWindowType() == ITIS_LISTVIEW)
				{
					((RingListView*)Parent())->ShowColumn(nIndex - HDMENU_BASEID,bShow);
					nmhd.hdr.code = LVN_SHOWCOLUMN;
					nmhd.hdr.hwndFrom = Parent()->Handle();
					nmhd.hdr.idFrom = ((RingListView*)Parent())->GetId();
					hWndP = Parent()->GetParent();
				}
				else
				{
					ShowColumn(nIndex - HDMENU_BASEID,bShow);
					nmhd.hdr.code = HDN_SHOWCOLUMN;
					nmhd.hdr.hwndFrom = m_hWnd;
					nmhd.hdr.idFrom = GetId();
					hWndP = GetParent();
				}
				nmhd.iItem = nIndex - HDMENU_BASEID;
				nmhd.iButton = bShow;
				nmhd.pitem = NULL;
				SendMessage(hWndP,WM_NOTIFY,nmhd.hdr.idFrom,(LPARAM)&nmhd);
				return 0;
			}
			break;
		case WM_MEASUREITEM:      	
			if(m_CoolMenu)
				m_CoolMenu->MeasureItem(param.lpmeasureitem);
         break;
      case WM_DRAWITEM:
			if(m_CoolMenu)
				m_CoolMenu->Draw(param.lpdrawitem);
         break;
		case WM_DESTROY:
			SendMessage(m_hWnd,HDM_ENABLECOLUMNHIDE,FALSE,0);
			break;
	}
	return RingControls::DefaultProc(param);
}

BOOL RingHeader::IsMouseOnHiddenCol(int x,int y)
{
	if(m_bCanHideColumn)
	{
		HDHITTESTINFO hdii;
		hdii.pt.x = x;
		hdii.pt.y = y;
		LRESULT res = CallWindowProc(m_DefProc,m_hWnd,HDM_HITTEST,0,(LPARAM)&hdii);
		if(res != -1 && MASK_MATCH(hdii.flags,HHT_ONDIVOPEN))
		{
			LPLONG lpl = GetHDInfo(res);
			if(lpl && HDCOL_ISHIDDEN(*lpl))
				return TRUE;
		}
	}
	return FALSE;
}

//显示/隐藏列头，iCol为全部列头显示状态下的序号,显示/隐藏并不立即生效，
//必须随后设置宽度才可以，这样做是因为ListView设置列宽会同时设置本HEADER的列宽，
//函数返回原显示状态下列宽
int RingHeader::ShowColumn(int iCol,BOOL bShow)
{
	int nWidth = SendMessage(m_hWnd,HDM_SHOWCOLUMN,iCol,bShow);
	if(Parent()->GetWindowType() != ITIS_LISTVIEW)
	{
		if(bShow)
			SetItemWidth(iCol,nWidth);
		else
			SetItemWidth(iCol,0);
		InvalidateRect(m_hWnd,NULL,TRUE);
	}
	return nWidth;
}

//允许右键菜单选择显示/隐藏列头
RingCoolMenu* RingHeader::EnableColumnMenu(BOOL bEnable)
{
	if(bEnable)
	{
		EnableHideColumn();

		if(m_CoolMenu == NULL)
		{
			m_CoolMenu = new RingCoolMenu;
			if(m_CoolMenu)
			{
				if(m_CoolMenu->CreatePopup())
				{
					int nCnt = GetCount();
					if(nCnt > 0)
					{
						m_CoolMenu->Insert(HDMENU_BASEID,0,(LPSTR)GetText(0),BY_POSITION);
						m_CoolMenu->EnableItem(0,FALSE,BY_POSITION);
						m_CoolMenu->CheckItem(0,TRUE,BY_POSITION);

						for(int i=1;i<nCnt;i++)
						{
							m_CoolMenu->Insert(HDMENU_BASEID+i,i,(LPSTR)GetText(i),BY_POSITION);
							m_CoolMenu->CheckItem(i,!IsColumnHide(i),BY_POSITION);
						}
					}
				}
			}
		}
	}
	else
	{
		delete m_CoolMenu;
		m_CoolMenu = NULL;
	}
	return m_CoolMenu;
}
