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
//							Shell操作类库 ringShell.lib
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
//原文件名：browser.cpp
//
//说明：浏览器控件操作实现
//
**********************************************************************/
#define MAKE_SELF_LIB
#include "ringshell.h"

typedef struct AAA
{
	LPITEMIDLIST	lpi;
	LPSHELLFOLDER	lpsfParent;
	LPITEMIDLIST	lpifq;
}TVITEMDATA,*LPTVITEMDATA;

typedef struct _STRRET1
{
    UINT uType; // One of the STRRET_* values
    union
    {
        LPWSTR          pOleStr;        // OLESTR that will be freed
        LPSTR           pStr;           // ANSI string that will be freed (needed?)
        UINT            uOffset;        // Offset into SHITEMID
        char            cStr[MAX_PATH]; // Buffer to fill in (ANSI)
    } ;//DUMMYUNIONNAME;
} STRRET1, *LPSTRRET1;

HWND hTreeWnd;

char szFoldername[MAX_PATH];
//NM_TREEVIEW *pNmTv=NULL;

void FillTreeView(HWND hwndTreeView,LPSHELLFOLDER lpsf,LPITEMIDLIST lpifq,	HTREEITEM hParent);
BOOL GetName( LPSHELLFOLDER lpsf, LPITEMIDLIST lpi, DWORD dwFlags, LPSTR lpFriendlyName) ;
LPITEMIDLIST CopyITEMID( LPMALLOC lpMalloc, LPITEMIDLIST lpi) ;
LPITEMIDLIST ConcatPidls( LPCITEMIDLIST pidl1, LPCITEMIDLIST pidl2) ;
LPITEMIDLIST GetFullyQualPidl( LPSHELLFOLDER lpsf, LPITEMIDLIST lpi) ;
void GetNormalAndSelectedIcons(LPITEMIDLIST lpifq,LPTV_ITEM lptvitem);
int GetIcon( LPITEMIDLIST lpi, UINT uFlags) ;
UINT GetSize(LPCITEMIDLIST pidl);
LPITEMIDLIST Create(UINT cbSize);
LPITEMIDLIST Next(LPCITEMIDLIST pidl);
void ExpandTree(void);

//初始化浏览器控件（nID为资源文件中树型控件的id）
BOOL Init_Browser(HWND hWnd,UINT nID)
{
	HIMAGELIST hImageList;
   LPSHELLFOLDER lpsf = 0 ;
   SHFILEINFO    sfi;
	HRESULT hr ;
	BOOL bOK;

	memset(szFoldername,0,MAX_PATH);
   hTreeWnd=GetDlgItem(hWnd,nID);

   hImageList = (HIMAGELIST)SHGetFileInfo((LPCSTR)"C:\\",
                                           0,
                                           &sfi,
                                           sizeof(SHFILEINFO),
                                           SHGFI_SYSICONINDEX | SHGFI_SMALLICON) ;

  	if(hImageList)
   	TreeView_SetImageList(hTreeWnd,hImageList,0);

	hr=SHGetDesktopFolder(&lpsf) ;

	if( SUCCEEDED(hr))
	{
	   TreeView_DeleteAllItems(hTreeWnd);
	   FillTreeView(hTreeWnd,lpsf,NULL,TVI_ROOT) ;
      ExpandTree();
      TreeView_SelectItem(hTreeWnd,TreeView_GetRoot(hTreeWnd));//,TVGN_FIRSTVISIBLE);
      bOK = TRUE;
	}
   else
   	bOK = FALSE;

	if(lpsf)
		lpsf->Release();
	return bOK;
}

//响应浏览器控件操作(在WM_NOTIFY消息中调用)
UINT Response_Browser(LONG lParam)
{
   NM_TREEVIEW *pnmtv = (NM_TREEVIEW *)lParam;
   LPTVITEMDATA lptvid ;
   HRESULT hr ;
   LPSHELLFOLDER lpsf2 = 0 ;

   switch( pnmtv -> hdr.code)
	{
		case TVN_SELCHANGED:
			lptvid = (LPTVITEMDATA)pnmtv -> itemNew.lParam;
         GetName(lptvid -> lpsfParent, lptvid->lpi, SHGDN_FORPARSING, szFoldername);

         if(szFoldername[1]!=':' || szFoldername[2]!='\\')
			{
		   	GetWindowsDirectory(szFoldername,MAX_PATH);
  				strcat(szFoldername,"\\desktop\0");
            TreeView_SelectItem(hTreeWnd,TreeView_GetRoot(hTreeWnd));
		   }
			return TVN_SELCHANGED;

		case TVN_ITEMEXPANDING:
		   if(( pnmtv -> itemNew.state & TVIS_EXPANDEDONCE))
				break ;

		   lptvid = (LPTVITEMDATA)pnmtv -> itemNew.lParam ;
		   hr=lptvid -> lpsfParent->BindToObject(lptvid -> lpi,
																   0,
																   IID_IShellFolder,
																   (LPVOID *)&lpsf2) ;

		   if( SUCCEEDED(hr))
		   {
			  FillTreeView(hTreeWnd,
								   lpsf2,
								   lptvid -> lpifq,
								   pnmtv -> itemNew.hItem) ;
		   }

		   TreeView_SortChildren(hTreeWnd, pnmtv -> itemNew.hItem, FALSE) ;
			return FALSE;//TVN_ITEMEXPANDING;

		default:
		   break;
   }
   return FALSE;
}

//得到在浏览器控件中用户选择的目录
LPSTR Get_Browser_selFoldername(void)
{
	return szFoldername;
}

LPITEMIDLIST Next(LPCITEMIDLIST pidl)
{
   LPSTR lpMem=(LPSTR)pidl;

   lpMem+=pidl -> mkid.cb;

   return (LPITEMIDLIST)lpMem;
}

UINT GetSize(LPCITEMIDLIST pidl)
{
    UINT cbTotal = 0;
    if( pidl)
    {
        cbTotal += sizeof(pidl -> mkid.cb) ;       // Null terminator
        while (pidl -> mkid.cb)
        {
            cbTotal += pidl -> mkid.cb;
            pidl = Next(pidl) ;
        }
    }

    return cbTotal;
}

LPITEMIDLIST Create(UINT cbSize)
{
    LPMALLOC lpMalloc;
    HRESULT  hr;
    LPITEMIDLIST pidl=NULL;

    hr=SHGetMalloc(&lpMalloc) ;

    if( FAILED(hr))
       return 0;

    pidl=(LPITEMIDLIST)lpMalloc->Alloc(cbSize) ;

    if(pidl)
        memset(pidl, 0, cbSize) ;      // zero-init for external task   alloc

    if(lpMalloc)
		 lpMalloc->Release() ;

    return pidl;
}

LPITEMIDLIST ConcatPidls(LPCITEMIDLIST pidl1, LPCITEMIDLIST pidl2)
{
    LPITEMIDLIST pidlNew;
    UINT cb1;
    UINT cb2;

    if( pidl1)  //May be NULL
       cb1 = GetSize(pidl1) - sizeof(pidl1 -> mkid.cb) ;
    else
       cb1 = 0;

    cb2 = GetSize(pidl2) ;

    pidlNew = Create(cb1 + cb2) ;
    if( pidlNew)
    {
        if( pidl1)
           memcpy(pidlNew, pidl1, cb1) ;
        memcpy(((LPSTR)pidlNew) + cb1, pidl2, cb2) ;
    }
    return pidlNew;
}

LPITEMIDLIST CopyITEMID(LPMALLOC lpMalloc, LPITEMIDLIST lpi)
{
   LPITEMIDLIST lpiTemp;

   lpiTemp=(LPITEMIDLIST)lpMalloc->Alloc(lpi->mkid.cb+sizeof(lpi->mkid.cb));
   CopyMemory((PVOID)lpiTemp, (CONST VOID *)lpi, lpi -> mkid.cb+sizeof(lpi -> mkid.cb)) ;

   return lpiTemp;
}

BOOL GetName(LPSHELLFOLDER lpsf,LPITEMIDLIST  lpi,DWORD dwFlags,LPSTR lpFriendlyName)
{
   BOOL   bSuccess=TRUE;
   STRRET1 str;

   if(lpsf->GetDisplayNameOf(lpi,dwFlags,(LPSTRRET)&str)==NOERROR)
   {
      switch (str.uType)
      {
         case STRRET_WSTR:
            WideCharToMultiByte(CP_ACP,                 // CodePage
                                0,		               // dwFlags
                                str.pOleStr,            // lpWideCharStr
                                -1,                     // cchWideChar
                                lpFriendlyName,         // lpMultiByteStr
                                300,//sizeof(lpFriendlyName), // cchMultiByte,
                                NULL,                   // lpDefaultChar,
                                NULL) ;                  // lpUsedDefaultChar
             break;

         case STRRET_OFFSET:
             lstrcpy(lpFriendlyName, (LPSTR)lpi+str.uOffset) ;
             break;

         case STRRET_CSTR:
             lstrcpy(lpFriendlyName, (LPSTR)str.cStr) ;
             break;

         default:
             bSuccess = FALSE;
             break;
      }
   }
   else
      bSuccess = FALSE;

   return bSuccess;
}

LPITEMIDLIST GetFullyQualPidl(LPSHELLFOLDER lpsf, LPITEMIDLIST lpi)
{
   char szBuff[MAX_PATH];
   OLECHAR szOleChar[MAX_PATH];
   LPSHELLFOLDER lpsfDeskTop;
   LPITEMIDLIST  lpifq;
   ULONG ulEaten, ulAttribs;
   HRESULT hr;

   if( !GetName(lpsf, lpi, SHGDN_FORPARSING, szBuff))
      return NULL;

   hr=SHGetDesktopFolder(&lpsfDeskTop) ;

   if( FAILED(hr))
      return NULL;

   MultiByteToWideChar(CP_ACP,
					   MB_PRECOMPOSED,
					   szBuff,
					   -1,
					   szOleChar,
					   sizeof(szOleChar)) ;

   hr=lpsfDeskTop->ParseDisplayName(NULL,NULL,szOleChar,&ulEaten,&lpifq,&ulAttribs) ;

   lpsfDeskTop->Release() ;

   if( FAILED(hr))
      return NULL;

   return lpifq;
}

int GetIcon(LPITEMIDLIST lpi, UINT uFlags)
{
   SHFILEINFO    sfi;

   SHGetFileInfo((LPCSTR)lpi,
                 0,
                 &sfi,
                 sizeof(SHFILEINFO),
                 uFlags) ;

   return sfi.iIcon;
}

HICON GetFileIcon(LPSTR lpi)
{
   SHFILEINFO    sfi;

   SHGetFileInfo((LPCSTR)lpi,
                 0,
                 &sfi,
                 sizeof(SHFILEINFO),
                 SHGFI_ICON) ;

   return sfi.hIcon;
}

void GetNormalAndSelectedIcons(LPITEMIDLIST lpifq,
                               LPTV_ITEM lptvitem)
{
   lptvitem -> iImage = GetIcon(lpifq, SHGFI_PIDL |
                              SHGFI_SYSICONINDEX |
                              SHGFI_SMALLICON) ;

   lptvitem -> iSelectedImage = GetIcon(lpifq, SHGFI_PIDL |
                                      SHGFI_SYSICONINDEX |
                                      SHGFI_SMALLICON |
                                      SHGFI_OPENICON) ;
}


void FillTreeView(HWND hwndTreeView,LPSHELLFOLDER lpsf,LPITEMIDLIST lpifq,HTREEITEM hParent)
{
    TV_ITEM         tvi;                          // TreeView Item.
    TV_INSERTSTRUCT tvins;                        // TreeView Insert Struct.
    HTREEITEM       hPrev = NULL;                 // Previous Item Added.
    LPSHELLFOLDER   lpsf2=0;
    LPENUMIDLIST    lpe=0;
    LPITEMIDLIST    lpi=0,lpifqThisItem;// lpiTemp=0, lpifqThisItem;
    LPTVITEMDATA    lptvid=0;
    LPMALLOC        lpMalloc=0;
    ULONG           ulFetched;
    HRESULT         hr;
    char            szBuff[256];
    HWND            hwnd=GetParent(hwndTreeView);

    hr=SHGetMalloc(&lpMalloc);
    if(FAILED(hr))
       return;

    SetCapture(GetParent(hwndTreeView));
    SetCursor(LoadCursor(NULL,IDC_WAIT));

    hr=lpsf->EnumObjects(hwnd,
								SHCONTF_FOLDERS | SHCONTF_NONFOLDERS | SHCONTF_INCLUDEHIDDEN,
                        &lpe) ;

    if(SUCCEEDED(hr))
    {
	    while (S_OK==lpe->Next(1,&lpi,&ulFetched))
       {
          ULONG ulAttrs = SFGAO_HASSUBFOLDER | SFGAO_FOLDER;

          lpsf->GetAttributesOf(1,(LPCITEMIDLIST*)&lpi, &ulAttrs) ;

          if( ulAttrs & (SFGAO_HASSUBFOLDER | SFGAO_FOLDER))
          {
             if( ulAttrs & SFGAO_FOLDER)
             {
                tvi.mask = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_PARAM;

                if( ulAttrs & SFGAO_HASSUBFOLDER)
                {
                   tvi.cChildren=1;
                   tvi.mask |= TVIF_CHILDREN;
                }

                lptvid = (LPTVITEMDATA)lpMalloc->Alloc(sizeof(TVITEMDATA));
                if( !lptvid)
                   goto Done;

                if(!GetName(lpsf, lpi, SHGDN_NORMAL, szBuff))
                   goto Done;
                tvi.pszText    = szBuff;
                tvi.cchTextMax = MAX_PATH;

                lpifqThisItem=ConcatPidls(lpifq, lpi) ;

                lptvid -> lpi=CopyITEMID(lpMalloc, lpi) ;

                GetNormalAndSelectedIcons(lpifqThisItem, &tvi) ;

                lptvid -> lpsfParent=lpsf;    //Store the parent folders SF
                lpsf->AddRef() ;

                if( hParent==TVI_ROOT)
                   lptvid -> lpifq=ConcatPidls(lpifq, lpi) ;
                else
                   lptvid -> lpifq=GetFullyQualPidl(lpsf, lpi) ;

                tvi.lParam = (LPARAM)lptvid;

                tvins./*DUMMYUNIONNAME.*/item = tvi;
                tvins.hInsertAfter = hPrev;
                tvins.hParent      = hParent;

                hPrev = TreeView_InsertItem(hwndTreeView, &tvins) ;

//////the next code expand all item but CD-ROM in Tree view/////////
//
//TreeView_Expand(hwndTreeView,hPrev,TVE_TOGGLE);
//
/////code end//////////////////////////////////////////////////////
				}

            lpMalloc->Free(lpifqThisItem) ;
            lpifqThisItem=0;
			}

         lpMalloc->Free(lpi) ;  //Finally, free the pidl that the shell gave us...
         lpi=0;
		}
	}

Done:

	ReleaseCapture() ;
   SetCursor( LoadCursor( NULL, IDC_ARROW)) ;

   if(lpe)
		lpe->Release() ;

   if(lpi && lpMalloc)
		lpMalloc->Free(lpi) ;
   if(lpifqThisItem && lpMalloc)
		lpMalloc->Free(lpifqThisItem) ;

   if(lpMalloc)
		lpMalloc->Release() ;
}

void ExpandTree(void)
{
   HTREEITEM hTree;

   hTree=TreeView_GetRoot(hTreeWnd);

   if(hTree)
   	TreeView_Expand(hTreeWnd,hTree,TVE_EXPAND);
}
