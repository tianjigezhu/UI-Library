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
//原文件名：ringtree.cpp
//
//说明：树型控件控制
//
**********************************************************************/
#define MAKE_SELF_LIB
#include "ringdows.h"

RingTree::~RingTree()
{
	if(m_hImageDef)
		ImageList_Destroy(m_hImageDef);		
	
	if(m_hImageExp)
		ImageList_Destroy(m_hImageExp);
}

BOOL RingTree::Create(DWORD dwStyle,DWORD dwExStyle,LPCTSTR szInitText,UINT uId,int x,int y,int width,int height)
{
	InitCommonControls();

	RingControls::Create(dwStyle,dwExStyle,szInitText,uId,x,y,width,height);

	//InitImageList();
	return TRUE;
}

void RingTree::InitImageList()
{
	if(m_hImageDef == NULL)
	{
		m_hImageDef = ImageList_Create(16,16,ILC_COLOR32|ILC_MASK,5,5);
		TreeView_SetImageList(m_hWnd, m_hImageDef,TVSIL_NORMAL);
	}
}

int RingTree::AddIcon(UINT uidIcon,HINSTANCE hInst/*=GetInstance()*/)
{
	HICON hicon = LoadIcon(hInst,MAKEINTRESOURCE(uidIcon));
	int i = AddIcon(hicon);
	DestroyIcon(hicon);
		
	return i;
}
	
int RingTree::AddIcon(HICON hicon)
{
	if(hicon == NULL)
		return -1;
	
	InitImageList();

	if(m_hImageDef)
		return ImageList_AddIcon(m_hImageDef,hicon);
	else
		return -1;
}

HTREEITEM RingTree::AddRoot(LPCTSTR szText,HTREEITEM hAfter,LPARAM lParam,UINT idIcon,UINT idSelIcon)
{
	return AddItem(szText,NULL,hAfter,lParam,idIcon,idSelIcon);
}

HTREEITEM RingTree::AddItem(LPCTSTR szText,HTREEITEM hParent,HTREEITEM hAfter,LPARAM lParam,UINT idIcon,UINT idSelIcon)
{
	TV_ITEM tvItem;
	TV_INSERTSTRUCT tvInsert;
		
	tvItem.mask = TVIF_TEXT | TVIF_PARAM;
	tvItem.pszText = (LPSTR)szText;
	tvItem.cchTextMax = lstrlen(szText);
	tvItem.lParam = lParam;
	
	if(IS_INTRESOURCE(idIcon))
	{
		tvItem.mask |= TVIF_IMAGE|TVIF_SELECTEDIMAGE;
		tvItem.iImage = idIcon;
		tvItem.iSelectedImage = idIcon;
	}
	if(IS_INTRESOURCE(idSelIcon))
		tvItem.iSelectedImage = idSelIcon;
	
	tvInsert.item = tvItem;
	tvInsert.hInsertAfter = hAfter;
	tvInsert.hParent = hParent;
	
	return (HTREEITEM)SendMessage(m_hWnd,TVM_INSERTITEM,0,
											(LPARAM)(LPTV_INSERTSTRUCT)&tvInsert);
}

LPARAM RingTree::GetData(HTREEITEM hItem)
{
	TVITEM tvi;

	memset(&tvi,0,sizeof(TVITEM));
	tvi.mask = TVIF_PARAM | TVIF_HANDLE;
	tvi.hItem = hItem;

	if(TreeView_GetItem(m_hWnd,&tvi))
		return tvi.lParam;
	else
		return 0;
}

BOOL RingTree::SetData(HTREEITEM hItem,LONG data)
{
	TVITEM tvi;
	
	memset(&tvi,0,sizeof(TVITEM));
	tvi.mask = TVIF_PARAM | TVIF_HANDLE;
	tvi.hItem = hItem;
	tvi.lParam = data;

	return TreeView_SetItem(m_hWnd,&tvi);
}

HTREEITEM RingTree::HitTest(LPPOINT lpt,LPDWORD lpFlag/*=NULL*/)
{
	if(lpt)
	{
		TVHITTESTINFO ht = {{lpt->x,lpt->y},0,NULL};
		HTREEITEM hitem = TreeView_HitTest(m_hWnd,&ht);
		if(lpFlag)
			*lpFlag = ht.flags;
		return hitem;
	}
	else
		return NULL;
}

//设置子项文字，最后的参数为HTREEITEM句柄，如为INDEX_CURSEL则是当前选择项
BOOL RingTree::SetValue(LPCTSTR lpszVal,int index/*=INDEX_CURSEL*/)
{
	if(lpszVal && *lpszVal != '\0')
	{
		HTREEITEM ht;
		if(index == INDEX_CURSEL)
			ht = GetSelection();
		else
			ht = (HTREEITEM)index;

		if(ht)
		{
			TVITEM tvi;
			tvi.cchTextMax = strlen(lpszVal);
			tvi.hItem = ht;
			tvi.pszText = (LPTSTR)lpszVal;
			tvi.mask = TVIF_TEXT;
			
			return TreeView_SetItem(m_hWnd,&tvi);
		}
	}
	return FALSE;
}

LPCTSTR RingTree::GetItemString(HTREEITEM hItem,int size/*=RBUF_SIZE*/,LPSTR lpBuf/*=NULL*/)
{
	if(lpBuf == NULL && GetBuffer(size))
		lpBuf = m_buf;

	if(lpBuf)
	{
		memset(lpBuf,0,size);
		
		TVITEM tvi;
		tvi.cchTextMax = size;
		tvi.hItem = hItem;
		tvi.pszText = lpBuf;
		tvi.mask = TVIF_TEXT;

		if(TreeView_GetItem(m_hWnd,&tvi))
		{
			if(tvi.pszText != m_buf)	//tvi.pszText的值有可能改变
				strncpy(lpBuf,tvi.pszText,size);
			return lpBuf;
		}
	}
	return "";
}

//index:仅支持TVGN_CARET,TVGN_CHILD,TVGN_NEXT,TVGN_PARENT,TVGN_PREVIOUS,TVGN_ROOT
int RingTree::GetItemText(int index,LPTSTR lpBuf,int size)
{
	HTREEITEM hitem = NULL;

	if(index == TVGN_ROOT)
		hitem = TreeView_GetRoot(m_hWnd);
	else
	{		
		hitem = TreeView_GetSelection(m_hWnd);
		if(hitem && index != INDEX_CURSEL && index != TVGN_CARET)
			hitem = TreeView_GetNextItem(m_hWnd,hitem,index);
	
		if(hitem)
			return lstrlen(GetItemString(hitem,size,lpBuf));
	}
	return 0;
}

//获取hItem下名为lpszName的子节点，仅搜索一层。hItem为NULL则仅搜索根目录
HTREEITEM RingTree::SearchItem(LPCTSTR lpszName,HTREEITEM hItem/*=NULL*/)
{
	if(hItem == NULL)
		hItem = TreeView_GetRoot(m_hWnd);
	else
		hItem = TreeView_GetChild(m_hWnd,hItem);

	if(hItem)
	{
		if(lpszName == NULL)
			return hItem;

		while(hItem)
		{
			if(strcmp(lpszName,GetItemString(hItem)) == 0)
				return hItem;
			else
				hItem = TreeView_GetNextSibling(m_hWnd,hItem);
		}
	}
	return NULL;
}

//获取鼠标当前位置下的节点(一般用在NM_CLICK之类的响应处理中)
HTREEITEM RingTree::GetMouseSelection()
{
	POINT pt;
	GetCursorPos(&pt);
	ScreenToClient(m_hWnd,&pt);
	return HitTest(&pt);
}

BOOL RingTree::SortChildren(HTREEITEM hParent,PFNTVCOMPARE lpfn,LPARAM lParam)
{
	TV_SORTCB tvs;
	tvs.hParent = hParent;
	tvs.lParam = lParam;
	tvs.lpfnCompare = lpfn;
	return TreeView_SortChildrenCB(m_hWnd,&tvs,FALSE);
}
