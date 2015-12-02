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
//原文件名：ringlistview.cpp
//
//说明：listview控件控制
//
**********************************************************************/
#define MAKE_SELF_LIB
#include "ringdows.h"

RingListView::RingListView()
{
	m_hImageBig = m_hImageSmall = m_hImageState = NULL;
	m_dwImgFreeMask = 0;
	m_header = NULL;
	m_windowType = ITIS_LISTVIEW;
}

RingListView::~RingListView()
{
	if(m_hImageBig)
		ImageList_Destroy(m_hImageBig);		
	if(m_hImageSmall)
		ImageList_Destroy(m_hImageSmall);
	if(m_hImageState)
		ImageList_Destroy(m_hImageState);
}

BOOL RingListView::Create(DWORD dwStyle,DWORD dwExStyle,LPCTSTR szInitText,UINT uId,int x,int y,int width,int height)
{   
	InitCommonControls();
	
	RingControls::Create(dwStyle,dwExStyle,szInitText,uId,x,y,width,height);
		
	//InitImageList();
   return (BOOL)m_hWnd;
}

void RingListView::SetExStyle(DWORD dwExStyle)
{
	if(dwExStyle != 0 && m_hWnd)
	{
		//ListView专有扩展属性必须用以下方法设置，
		DWORD dwStyle = SendMessage(m_hWnd,LVM_GETEXTENDEDLISTVIEWSTYLE,0,0);
		dwStyle |= dwExStyle;
		SendMessage(m_hWnd,LVM_SETEXTENDEDLISTVIEWSTYLE,0,dwStyle);
	}
}

LRESULT RingListView::DefaultProc(RINGPARAMS& param)
{
	if(param.uMsg == WM_NCDESTROY)
	{
		//BoundCheck的奇怪BUG：窗口过程销毁了IMAGELIST,析构时调用
		//ImageList_Destroy出错，BoundCheck报资源泄漏。
		//为避免该信息，这里检测如果是内置的就把ImageList设置为空
		if(ListView_GetImageList(m_hWnd,LVSIL_NORMAL) == m_hImageBig)
			ListView_SetImageList(m_hWnd,NULL,LVSIL_NORMAL);
		if(ListView_GetImageList(m_hWnd,LVSIL_SMALL) == m_hImageSmall)
			ListView_SetImageList(m_hWnd,NULL,LVSIL_SMALL);
		if(ListView_GetImageList(m_hWnd,LVSIL_STATE) == m_hImageState)
			ListView_SetImageList(m_hWnd,NULL,LVSIL_STATE);
		CallWindowProc(m_DefProc,m_hWnd,param.uMsg,param.wParam,param.lParam);
		OnNcDestroy();
		return 0;
	}	
	return CallWindowProc(m_DefProc,m_hWnd,param.uMsg,param.wParam,param.lParam);
}

//创建IMAGELIST，如不需要可设为0
void RingListView::InitImageList(int nBigX,int nBigY,
											int nSmallX,int nSmallY,
											int nStateX,int nStateY)
{
	if(m_hImageBig == NULL && (nBigX != 0 && nBigY != 0))
	{
		m_hImageBig = ImageList_Create(nBigX,nBigY,ILC_COLOR32|ILC_MASK,5,5);
		if(m_hWnd && m_hImageBig && ListView_GetImageList(m_hWnd,LVSIL_NORMAL) == NULL)
			ListView_SetImageList(m_hWnd,m_hImageBig,LVSIL_NORMAL);
	}	
	if(m_hImageSmall == NULL && (nSmallX != 0 && nSmallY != 0))
	{
		m_hImageSmall = ImageList_Create(nSmallX,nSmallY,ILC_COLOR32|ILC_MASK,5,5);
		if(m_hWnd && m_hImageSmall && ListView_GetImageList(m_hWnd,LVSIL_SMALL) == NULL)
			ListView_SetImageList(m_hWnd,m_hImageSmall,LVSIL_SMALL);
	}
	if(m_hImageState == NULL && (nStateX != 0 && nStateY != 0))
	{
		m_hImageState = ImageList_Create(nStateX,nStateY,ILC_COLOR32|ILC_MASK,5,5);
		if(m_hWnd && m_hImageState && ListView_GetImageList(m_hWnd,LVSIL_STATE) == NULL)
			ListView_SetImageList(m_hWnd,m_hImageState,LVSIL_STATE);
	}
}

////////////////////////////////////////////////////
//
//添加图标，必须已设置过IMAGELIST或调用过InitImageList(),
//如果图标包含不同尺寸，分别把最匹配尺寸图标添加到相应的
//IMAGELIST中。
//
//返回：最大的图标序号
//
////////////////////////////////////////////////////
int RingListView::AddIcon(LPCTSTR lpszIcon,HINSTANCE hInst/*=GetInstance()*/)
{
	//因为有可能调用过SetImageList，因此不能使用内置的m_hImageXXX,
	//必须ListView_GetImageList
	HIMAGELIST himl = ListView_GetImageList(m_hWnd,LVSIL_NORMAL);
	HICON hicon;
	int x,y;
	int nIndex = -1;

	if(himl)
	{
		if(!ImageList_GetIconSize(himl,&x,&y))
			x = y = 32;
			
		hicon = (HICON)LoadImage(hInst,lpszIcon,IMAGE_ICON,x,y,LR_DEFAULTCOLOR);
		if(hicon)
			nIndex = ImageList_AddIcon(himl,hicon);
	}
	
	himl = ListView_GetImageList(m_hWnd,LVSIL_SMALL);
	if(himl)
	{
		if(!ImageList_GetIconSize(himl,&x,&y))
			x = y = 16;
		
		hicon = (HICON)LoadImage(hInst,lpszIcon,IMAGE_ICON,x,y,LR_DEFAULTCOLOR);
		if(hicon)
			nIndex = max(nIndex,ImageList_AddIcon(himl,hicon));
	}
	return nIndex;
}

int RingListView::AddIcon(UINT uidIcon,HINSTANCE hInst/*=GetInstance()*/)
{
	return AddIcon(MAKEINTRESOURCE(uidIcon),hInst);
}

//添加图标，同时添加到大小图标列表中，返回最大的图标序号
int RingListView::AddIcon(HICON hicon)
{
	if(hicon == NULL)
		return -1;

	int nIndex = -1;
	HIMAGELIST himl = ListView_GetImageList(m_hWnd,LVSIL_NORMAL);
	if(himl)
		nIndex = ImageList_AddIcon(himl,hicon);
	
	himl = ListView_GetImageList(m_hWnd,LVSIL_SMALL);
	if(himl)
		nIndex = max(nIndex,ImageList_AddIcon(himl,hicon));

	return nIndex;
}

////////////////////////////////////////////////////
//
//添加图标，必须已设置过IMAGELIST或调用过InitImageList(),
//如果图标包含不同尺寸，把最匹配尺寸图标添加到相应的
//IMAGELIST中。
//
//返回：图标序号
//
////////////////////////////////////////////////////
int RingListView::AddIcon(UINT uidIcon,UINT uFlag,HINSTANCE hInst/*=GetInstance()*/)
{
	HICON hicon;
	HIMAGELIST himl = ListView_GetImageList(m_hWnd,uFlag);
	int x,y;
	
	if(himl)
	{
		if(ImageList_GetIconSize(himl,&x,&y))
			hicon = (HICON)LoadImage(hInst,MAKEINTRESOURCE(uidIcon),IMAGE_ICON,x,y,LR_DEFAULTCOLOR);
		else
			hicon = LoadIcon(hInst,MAKEINTRESOURCE(uidIcon));
		
		if(hicon)
			return ImageList_AddIcon(himl,hicon);
	}
	return -1;	
}

//uFlag:LVSIL_NORMAL,LVSIL_SMALL,LVSIL_STATE
int RingListView::AddIcon(HICON hicon,UINT uFlag)
{
	HIMAGELIST himl = ListView_GetImageList(m_hWnd,uFlag);
	
	if(hicon == NULL || himl == NULL)
		return -1;

	return ImageList_AddIcon(himl,hicon);	
}

LONG RingListView::GetData(int index)
{
	LVITEM lvt;
	lvt.mask = LVIF_PARAM;
	lvt.iItem = index;
	lvt.iSubItem = 0;
	lvt.lParam = 0;

	if(ListView_GetItem(m_hWnd,&lvt))
		return lvt.lParam;
	else
		return 0;
}

BOOL RingListView::SetData(int index,LONG lParam)
{
	LVITEM lvt;
	lvt.mask = LVIF_PARAM;
	lvt.iItem = index;
	lvt.iSubItem = 0;
	lvt.lParam = lParam;
	
	return ListView_SetItem(m_hWnd,&lvt);
}

int RingListView::SetColumns(LPCTSTR szColumn)
{
	if(m_hWnd == NULL || szColumn == NULL)
		return 0;

	ClearColumn();

	LV_COLUMN lvc; 
	int i,c = 0;
	ringStr str;
	LPTSTR s;
	int iCol,iWidth;

	str = szColumn;
	iCol = str.count("|") + 1;
	iWidth = (Width() - GetSystemMetrics(SM_CXVSCROLL))/iCol;

		// Initialize the LV_COLUMN structure. 
	lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
	lvc.fmt = LVCFMT_LEFT; 
		
	// Add the columns. 
	for(i=0;i<iCol;i++) 
	{ 
		lvc.iSubItem = i;
		s = (LPTSTR)str.string() + c;
		c = str.indexOf("|",c);
		str[c] = '\0';
		
		lvc.cx = iWidth;
		lvc.pszText = s;
			
		if(ListView_InsertColumn(m_hWnd,i,&lvc) == -1) 
		{
			str[c] = '|';
			return i;
		}
		str[c++] = '|';
	}
	return iCol;
}

BOOL RingListView::SetColumnsWidth(LPCTSTR szWidth)
{
	if(m_hWnd == NULL || szWidth == NULL)
		return FALSE;
	
	int i,c = 0;
	ringStr str;
	LPTSTR s;
	int iCol,iWidth;
	
	str = szWidth;
	iCol = str.count("|") + 1;
	
	for(i=0;i<iCol;i++) 
	{ 
		s = (LPTSTR)str.string() + c;
		c = str.indexOf("|",c);
		if(str[c-1] == '\\')
		{
			c++;
			continue;
		}
		str[c] = '\0';
		iWidth = atoi(s);
		
		if(!ListView_SetColumnWidth(m_hWnd,i,iWidth))
		{
			str[c] = '|';
			return FALSE;
		}
		str[c++] = '|';
	}	
	return TRUE; 
}

BOOL RingListView::InsertColumn(LPCTSTR szColumn,int subindex,int width,UINT uAlign/*=LVCFMT_LEFT*/,UINT uidIcon/*=0*/)
{
	if(m_hWnd == NULL || szColumn == NULL)
		return FALSE;
	
	LV_COLUMN lvc; 
	int iconId = -1;

	if(uidIcon != 0)
	{
		iconId = AddIcon(uidIcon);
		lvc.mask = LVCF_IMAGE;
	}
	else
		lvc.mask = 0;

	// Initialize the LV_COLUMN structure. 
	lvc.mask |= LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
	lvc.fmt = uAlign;
	lvc.cx = width;
	lvc.iSubItem = subindex;
	lvc.iImage = iconId;
	lvc.pszText = (LPSTR)szColumn;
			
	return (ListView_InsertColumn(m_hWnd,subindex,&lvc) != -1);
}

int RingListView::InsertRow(int index)
{
	LV_ITEM lvi;
	
	lvi.mask = LVIF_TEXT | LVIF_STATE;
	lvi.state = LVIS_FOCUSED | LVIS_SELECTED;
	lvi.stateMask = 0;
	lvi.pszText = "";
	lvi.cchTextMax = 300;
	
	lvi.iItem = index;
	lvi.iSubItem = 0; 
	
	return ListView_InsertItem(m_hWnd,&lvi);
}

int RingListView::AddRow()
{
	return InsertRow(GetCount());
}

int RingListView::AddItem(int index,LPCTSTR szText,LPARAM lParam,int idIcon)
{
	LV_ITEM lvi; 
	lvi.mask = 0;
	
	if(idIcon >= 0)
	{
		lvi.mask = LVIF_IMAGE;
		lvi.iImage = idIcon;
	}
	
	lvi.state = 0;
	lvi.stateMask = 0;
	
	if(szText)
	{
		lvi.mask |= TVIF_TEXT;
		lvi.pszText = (LPSTR)szText;
		lvi.cchTextMax = strlen(szText);
	}
	
	lvi.iItem = index; 
	lvi.iSubItem = 0;
	
	lvi.mask |= LVIF_PARAM;
	lvi.lParam = lParam;
	return ListView_InsertItem(m_hWnd,&lvi);	
}

int RingListView::SetItem(int index,int subindex,LPCTSTR szText,LPARAM lParam,int idIcon)
{
	LV_ITEM lvi; 
	lvi.mask = 0;

	if(idIcon >= 0)
	{
		lvi.mask = LVIF_IMAGE;
		lvi.iImage = idIcon;
	}
	
	lvi.state = 0;
	lvi.stateMask = 0;
	
	if(szText)
	{
		lvi.mask |= TVIF_TEXT;
		lvi.pszText = (LPSTR)szText;
		lvi.cchTextMax = strlen(szText);
	}
	
	lvi.iItem = index; 
	lvi.iSubItem = subindex;
	
	return ListView_SetItem(m_hWnd,&lvi)?index:-1;
} 

int RingListView::AddItem(int index,int nVal,LPARAM lParam,int uiconId)
{
	char s[16];
	wsprintf(s,"%d\0",nVal);
	return AddItem(index,s,lParam,uiconId);
}

int RingListView::SetItem(int index,int subindex,int nVal,LPARAM lParam,int uiconId)
{
	char s[16];
	wsprintf(s,"%d\0",nVal);
	return SetItem(index,subindex,s,lParam,uiconId);
}

BOOL RingListView::SetViewOf(UINT uType)
{
	LONG dNotView = ~(LVS_ICON|LVS_SMALLICON|LVS_LIST|LVS_REPORT);
	return SetWindowLong(m_hWnd,GWL_STYLE,GetWindowLong(m_hWnd,GWL_STYLE)&dNotView|uType);
}

LPCTSTR RingListView::GetText(int index,int subIndex,int size/*=RBUF_SIZE*/,LPSTR lpBuf/*=NULL*/)
{
   if(lpBuf == NULL)
	{
		if(GetBuffer(size))
		{
			memset(m_buf,0,m_bufsize);
			ListView_GetItemText(m_hWnd,index,subIndex,m_buf,m_bufsize);
			return m_buf;
		}
		else
			return "";
	}
	else
	{
		memset(lpBuf,0,size);
		ListView_GetItemText(m_hWnd,index,subIndex,lpBuf,size);
		return lpBuf;
	}
}

//重载的虚函数，获取第一列的文本
int RingListView::GetItemText(int index,LPTSTR lpBuf,int size)
{
	if(index == INDEX_CURSEL)
	{
		index = GetCurSel();
		if(index == -1)
			return 0;
	}
	
	LV_ITEM lv;
	lv.pszText = lpBuf;
	lv.cchTextMax = size;
	lv.iSubItem = 0;
	return SendMessage(m_hWnd,LVM_GETITEMTEXT,index,(LPARAM)&lv);
}

int RingListView::HitTest(int x,int y,LPLVHITTESTINFO lpvInfo/*=NULL*/)
{
	LVHITTESTINFO lpv;
	if(lpvInfo == NULL)
		lpvInfo = &lpv;

	lpvInfo->pt.x = x;
	lpvInfo->pt.y = y;

	return ListView_HitTest(m_hWnd,lpvInfo);
}

int RingListView::FindString(LPCTSTR szFind,int nStart/*=-1*/)
{
	LVFINDINFO lvf;

	lvf.flags = LVFI_PARTIAL;
	lvf.psz = szFind;

	return ListView_FindItem(m_hWnd,nStart,&lvf);
}

int RingListView::FindMatchString(LPCTSTR szFind,int nStart/*=-1*/)
{
	LVFINDINFO lvf;
	
	lvf.flags = LVFI_STRING;
	lvf.psz = szFind;
	
	return ListView_FindItem(m_hWnd,nStart,&lvf);
}

BOOL RingListView::SetValue(int val,int index/*=INDEX_CURSEL*/)
{
	char szText[16];
	wsprintf(szText,"%d\0",val);
	return SetValue(szText,index);
}

BOOL RingListView::SetValue(LPCTSTR lpszVal,int index/*=INDEX_CURSEL*/)
{
	if(lpszVal == NULL)
		return FALSE;

	if(index == INDEX_CURSEL)
	{
		index = GetCurSel();
		if(index == -1)
			return FALSE;
	}
	
	LV_ITEM lv;
	lv.pszText = (LPSTR)lpszVal;
	lv.iSubItem = 0;
	return SendMessage(m_hWnd,LVM_SETITEMTEXT,index,(LPARAM)&lv);
}

RingHeader* RingListView::GetHeaderObj()
{
	if(m_header == NULL)
	{
		HWND hWnd = GetHeader();
		if(hWnd)
		{
			m_header = (RingHeader*)GetWindowPtr(hWnd);

			if(m_header == NULL)
			{
				m_header = NewRingObj(itemof(RingHeader));
				m_header->Attach(hWnd);
			}
		}
	}
	return m_header;
}

HIMAGELIST RingListView::GetColumnImageList()
{
	if(GetHeaderObj())
		return m_header->GetImageList();
	else
		return NULL;
}

HIMAGELIST RingListView::SetColumnImageList(HIMAGELIST hImage,int cx,int cy)
{
	if(GetHeaderObj())
	{
		if(hImage == NULL)
		{
			//ListView如果设置了IMAGELIST，则列头的IMAGELIST已设置过
			//如果listview用做文件列表采用系统文件IMAGELIST，则列头需要
			//设置自己的IMAGELIST，否则只能使用系统文件IMAGELIST里的图标
			//且不能自己添加图标，（如果添加会影响Listview里正常的图标）
			hImage = m_header->GetImageList();
			//列头类设计成如果其m_hImage为空，则创建并设置IMAGELIST为m_hImage
			m_header->InitImageList(cx,cy);
			return hImage;
		}
		else
			return m_header->SetImageList(hImage);
	}
	return NULL;
}

BOOL RingListView::SetColumnImage(int nIndex,int nIconId,BOOL bOnRight)
{
	LVCOLUMN lvc;
	
	lvc.mask = LVCF_FMT|LVCF_IMAGE|LVCFMT_COL_HAS_IMAGES;
	GetColumn(nIndex,&lvc);

	if(nIconId < 0)
	{
		//不显示图标
		lvc.mask = LVCF_FMT|LVCF_IMAGE;
		lvc.fmt &= ~(LVCFMT_BITMAP_ON_RIGHT|LVCFMT_IMAGE);
	}
	else
	{
		if(bOnRight)
			lvc.fmt |= LVCFMT_BITMAP_ON_RIGHT|LVCFMT_IMAGE;
		else
			lvc.fmt |= LVCFMT_IMAGE;
	}
	lvc.iImage = nIconId;
	return SetColumn(nIndex,&lvc);
}

int RingListView::AddColumnIcon(int nIconId,HINSTANCE hInst)
{
	if(GetHeaderObj())
		return m_header->AddIcon(nIconId,hInst);
	else
		return -1;
}

int RingListView::AddColumnIcon(HICON hIcon)
{
	if(GetHeaderObj())
		return m_header->AddIcon(hIcon);
	else
		return -1;
}

//显示/隐藏列，iCol为全部列显示状态下的序号
BOOL RingListView::ShowColumn(int iCol,BOOL bShow)
{
	if(GetHeaderObj())
	{
		int nWidth = m_header->ShowColumn(iCol,bShow);
		if(!bShow)
			nWidth = 0;
		BOOL bRes = SetColumnWidth(iCol,nWidth);
		InvalidateRect(m_hWnd,NULL,TRUE);
	}
	return FALSE;
}

//允许在列头右击弹出菜单选择显示/隐藏列
RingCoolMenu* RingListView::EnableColumnMenu(BOOL bEnable)
{
	if(GetHeaderObj())
		return m_header->EnableColumnMenu(bEnable); 
	else
		return NULL;
}

int RingListView::GetCurSel(int nIndex)
{
	int nCnt = GetCount();
	for(int i=nIndex;i<nCnt;i++)
	{
		if(IsSelected(i))
			return i;
	}
	return -1;
}
