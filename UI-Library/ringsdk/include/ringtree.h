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
//原文件名：ringtree.h
//
//本文件说明：树型控件类定义
//
**********************************************************************/
#ifndef _INC_RINGTREE
#define _INC_RINGTREE
#include <commctrl.h>

class RingTree:public RingControls
{
public:
	RingTree():m_hImageDef(NULL),m_hImageExp(NULL){}
   ~RingTree();

	LPCTSTR WndClassName(){return WC_TREEVIEW;}	//"SysTreeView32"

	int AddIcon(UINT uidIcon,HINSTANCE hInst=GetInstance());
	int AddIcon(HICON hicon);

   BOOL Create(DWORD dwStyle,DWORD dwExStyle,LPCTSTR szInitText,UINT uId,int x=0,int y=0,int width=0,int height=0);
		
	HTREEITEM AddItem(LPCTSTR szText,HTREEITEM hParent,HTREEITEM hAfter,LPARAM lParam=0,UINT idIcon=-1,UINT idSelIcon=-1);
	HTREEITEM AddRoot(LPCTSTR szText,HTREEITEM hAfter,LPARAM lParam=0,UINT idIcon=-1,UINT idSelIcon=-1);
	
	//虚函数重载
	BOOL SetValue(int val,int index=INDEX_CURSEL){return FALSE;}
	//设置子项文字，最后的参数为HTREEITEM句柄，如为INDEX_CURSEL则是当前选择项
	BOOL SetValue(LPCTSTR lpszVal,int index=INDEX_CURSEL);

	HTREEITEM GetSelection(){return TreeView_GetSelection(m_hWnd);}
	//获取鼠标当前位置下的节点(一般用在NM_CLICK之类的响应处理中)
	HTREEITEM GetMouseSelection();
	HTREEITEM GetRoot(){return TreeView_GetRoot(m_hWnd);}
	HTREEITEM GetChildItem(HTREEITEM hItem){return TreeView_GetChild(m_hWnd,hItem);}
	HTREEITEM GetNext(HTREEITEM hItem){return TreeView_GetNextSibling(m_hWnd,hItem);}
	HTREEITEM GetPrev(HTREEITEM hItem){return TreeView_GetPrevSibling(m_hWnd,hItem);}
	HTREEITEM GetParentItem(HTREEITEM hItem){return TreeView_GetParent(m_hWnd,hItem);}
	//获取hItem下名为lpszName的子节点，仅搜索一层。hItem为NULL则仅搜索根目录
	HTREEITEM SearchItem(LPCTSTR lpszName,HTREEITEM hItem = NULL);
	LPCTSTR GetItemString(HTREEITEM hItem,int size=RBUF_SIZE,LPSTR lpBuf=NULL);

	LPARAM GetData(HTREEITEM hItem);
	BOOL SetData(HTREEITEM hItem,LONG data);
	HTREEITEM HitTest(LPPOINT lpt,LPDWORD lpFlag=NULL);
	BOOL Select(HTREEITEM hItem){return TreeView_SelectItem(m_hWnd,hItem);}
	BOOL Expand(HTREEITEM hItem,UINT uFlag = TVE_EXPAND){return TreeView_Expand(m_hWnd,hItem,uFlag);}
	BOOL Clear(){return TreeView_DeleteAllItems(m_hWnd);}
	BOOL SortChildren(HTREEITEM hItem){return TreeView_SortChildren(m_hWnd,hItem,FALSE);}
	BOOL SortChildren(HTREEITEM hParent,PFNTVCOMPARE lpfn,LPARAM lParam=0);

protected:
	virtual void InitImageList();
	//重载的虚函数,index:仅支持TVGN_CARET,TVGN_CHILD,TVGN_NEXT,TVGN_PARENT,TVGN_PREVIOUS,TVGN_ROOT
	int GetItemText(int index,LPTSTR lpBuf,int size);
	
protected:
	HIMAGELIST m_hImageDef,m_hImageExp;

private:
   
};

//文件夹列表树
class RingDirTree:public RingTree
{
public:
	RingDirTree():m_hImage(NULL){memset(m_szFolder,0,MAX_PATH);}
	~RingDirTree(){}

	BOOL Create(UINT uId,int x=0,int y=0,int width=0,int height=0);
	BOOL Create(DWORD dwStyle,DWORD dwExStyle,LPCTSTR szInitText,UINT uId,int x=0,int y=0,int width=0,int height=0);

	BOOL Attach(HWND);

	BOOL ListDir();
	LRESULT OnDefNotify(RingBaseWnd* w,RINGPARAMS& param);
	virtual LRESULT OnSelectChanged(RINGPARAMS& param);
	virtual LRESULT OnItemExpanding(RINGPARAMS& param);
	//获取选中的目录名，bRootNoSlash：如果选择的是根目录(比如C:)，则返回的是"C:\\"，
	//但普通目录返回最后是没有"\"的，因此为统一，设置该参数为TRUE可以去掉最后的"\"，
	//即返回"C:",默认参数则返回"C:\\"
	LPCTSTR GetSelFolderName(BOOL bRootNoSlash=FALSE);

	static int CALLBACK SortCBProc(LPARAM lparam1,LPARAM lparam2,LPARAM lparamSort);
	
protected:
	void InitImageList();
	void FillTreeView(LPSHELLFOLDER lpsf,LPITEMIDLIST lpifq,HTREEITEM hParent);

private:
	LPITEMIDLIST ConcatPidls(LPCITEMIDLIST pidl1,LPCITEMIDLIST pidl2);
	LPITEMIDLIST CopyITEMID(LPMALLOC lpMalloc,LPITEMIDLIST lpi);
	BOOL GetName(LPSHELLFOLDER lpsf,LPITEMIDLIST lpi,DWORD dwFlags,LPSTR lpFriendlyName);
	LPITEMIDLIST GetFullyQualPidl(LPSHELLFOLDER lpsf,LPITEMIDLIST lpi);
	int GetIcon(LPITEMIDLIST lpi,UINT uFlags);
	HICON GetFileIcon(LPSTR lpi);
	void GetNormalAndSelectedIcons(LPITEMIDLIST lpifq,LPTV_ITEM lptvitem);
	UINT GetSize(LPCITEMIDLIST pidl);
	LPITEMIDLIST Next(LPCITEMIDLIST pidl);
	LPITEMIDLIST CreateIDList(UINT cbSize);
	
private:	
	HIMAGELIST m_hImage;
	char m_szFolder[MAX_PATH];
};

#endif
