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
//原文件名：ringtreedir.cpp
//
//说明：文件夹列表树型控件控制
//
**********************************************************************/
#define MAKE_SELF_LIB
#include "ringdows.h"

typedef struct tagTVItemData
{
	LPITEMIDLIST	lpi;
	LPSHELLFOLDER	lpsfParent;
	LPITEMIDLIST	lpifq;
}TVITEMDATA,*LPTVITEMDATA;

BOOL RingDirTree::Create(UINT uId,int x,int y,int width,int height)
{
	if(RingTree::Create(WS_VISIBLE|WS_CHILD|TVS_HASLINES|TVS_LINESATROOT|
									TVS_HASBUTTONS|TVS_SHOWSELALWAYS/*|WS_BORDER/*CCS_NOPARENTALIGN|
									CCS_NODIVIDER|CCS_NORESIZE|*/,WS_EX_STATICEDGE,NULL,uId,x,y,width,height))
	{
		InitImageList();
		return TRUE;
	}
	else
		return FALSE;
}

BOOL RingDirTree::Create(DWORD dwStyle,DWORD dwExStyle,LPCTSTR szInitText,UINT uId,int x,int y,int width,int height)
{
	if(RingTree::Create(dwStyle|WS_VISIBLE|WS_CHILD|TVS_HASLINES|TVS_LINESATROOT|
									TVS_HASBUTTONS|TVS_SHOWSELALWAYS/*|CCS_NOPARENTALIGN|
									CCS_NODIVIDER|CCS_NORESIZE*/,dwExStyle,szInitText,
									uId,x,y,width,height))
	{
		InitImageList();
		return TRUE;
	}
	else
		return FALSE;
}

void RingDirTree::InitImageList()
{	
	if(m_hImage == NULL)
	{
		SHFILEINFO  sfi;
		m_hImage = (HIMAGELIST)SHGetFileInfo((LPCSTR)"C:\\",0,&sfi,sizeof(SHFILEINFO),
															SHGFI_SYSICONINDEX|SHGFI_ICON|SHGFI_SMALLICON);
	
  		if(m_hImage)
			TreeView_SetImageList(m_hWnd,m_hImage,0);
	}	
}

BOOL RingDirTree::Attach(HWND hWnd)
{
	BOOL bOK = RingControls::Attach(hWnd);

	if(bOK)
	{
		ReplaceStyle(GetStyle(),WS_VISIBLE|WS_CHILD|TVS_HASLINES|TVS_LINESATROOT|
										TVS_HASBUTTONS|TVS_SHOWSELALWAYS|CCS_NOPARENTALIGN);
		InitImageList();
	}
	return bOK;
}

int CALLBACK RingDirTree::SortCBProc(LPARAM lparam1,LPARAM lparam2,LPARAM lparamSort)
{
	LPTVITEMDATA lptvid1 = (LPTVITEMDATA)lparam1;
	LPTVITEMDATA lptvid2 = (LPTVITEMDATA)lparam2;
	HRESULT hr;
	
	hr = lptvid1->lpsfParent->CompareIDs(0,lptvid1->lpi,lptvid2->lpi);
	
	if (FAILED(hr))
		return 0;
	
	return (short)SCODE_CODE(GetScode(hr));
}

BOOL RingDirTree::ListDir()
{
	HRESULT	hr;
	BOOL bOK = FALSE;
	LPSHELLFOLDER lpsf = NULL;//IShellFolder 接口指针

	hr = SHGetDesktopFolder(&lpsf);

	if(SUCCEEDED(hr))
	{
		Clear();		
		FillTreeView(lpsf,NULL,TVI_ROOT);
		//SortChildren(TVI_ROOT,SortCBProc);
      Expand(GetRoot());
      Select(GetRoot());
      bOK = TRUE;
	}
   
	if(lpsf)
		lpsf->Release();

	return bOK;
}

LRESULT RingDirTree::OnDefNotify(RingBaseWnd*,RINGPARAMS& param)
{
	switch(param.lpnmhdr->code)
	{
		case TVN_SELCHANGED:
			return OnSelectChanged(param);		
		case TVN_ITEMEXPANDING:
			return OnItemExpanding(param);
		default:
			break;
   }
   return -1;
}

LPCTSTR RingDirTree::GetSelFolderName(BOOL bRootNoSlash/*=FALSE*/)
{
	if(bRootNoSlash)
	{
		int nLen = strlen(m_szFolder) - 1;
		if(nLen > 0 && m_szFolder[nLen] == '\\')
			m_szFolder[nLen] = '\0';
	}
	return m_szFolder;
}

LRESULT RingDirTree::OnSelectChanged(RINGPARAMS& param)
{
	NM_TREEVIEW *pnmtv = (NM_TREEVIEW*)param.lParam;
   LPTVITEMDATA lptvid;
   
	lptvid = (LPTVITEMDATA)pnmtv->itemNew.lParam;
	memset(m_szFolder,0,MAX_PATH);
	GetName(lptvid->lpsfParent,lptvid->lpi,SHGDN_FORPARSING,m_szFolder);
	
	
/*	
	if(m_szFolder[1]!=':' || m_szFolder[2]!='\\')
	{
		GetWindowsDirectory(m_szFolder,MAX_PATH);
		strcat(m_szFolder,"\\desktop\0");
		Select(GetRoot());
	}
*/
	return 0;
}

LRESULT RingDirTree::OnItemExpanding(RINGPARAMS& param)
{
	NM_TREEVIEW *pnmtv = (NM_TREEVIEW*)param.lParam;
   LPTVITEMDATA lptvid;
   HRESULT hr;
   LPSHELLFOLDER lpsf2 = NULL;

	if((pnmtv->itemNew.state & TVIS_EXPANDEDONCE))
		return 0;
	
	lptvid = (LPTVITEMDATA)pnmtv->itemNew.lParam;
	hr = lptvid->lpsfParent->BindToObject(lptvid->lpi,0,IID_IShellFolder,(LPVOID*)&lpsf2);
	
	if(SUCCEEDED(hr))
		FillTreeView(lpsf2,lptvid->lpifq,pnmtv->itemNew.hItem);
	
	//SortChildren(pnmtv->itemNew.hItem,SortCBProc);	
	return FALSE;
}

void RingDirTree::FillTreeView(LPSHELLFOLDER lpsf,LPITEMIDLIST lpifq,HTREEITEM hParent)
{
	TV_ITEM         tvi;                          // TreeView Item.
	TV_INSERTSTRUCT tvins;                        // TreeView Insert Struct.
	HTREEITEM       hPrev = NULL;                 // Previous Item Added.
	//LPSHELLFOLDER   lpsf2 = 0;
	LPENUMIDLIST    lpe = 0;
	LPITEMIDLIST    lpi = 0,lpifqThisItem;
	LPTVITEMDATA    lptvid = 0;
	LPMALLOC        lpMalloc = NULL;
	ULONG           ulFetched;
	HRESULT         hr;
	char            szBuff[MAX_PATH];
	HWND            hwnd = GetParent();
	ULONG				 ulAttrs;

	hr=SHGetMalloc(&lpMalloc);
	if(FAILED(hr))
		return;

	SetCursor(LoadCursor(NULL,IDC_WAIT));

	hr = lpsf->EnumObjects(hwnd,
									SHCONTF_FOLDERS|SHCONTF_NONFOLDERS|SHCONTF_INCLUDEHIDDEN,
									&lpe);
	
	if(SUCCEEDED(hr))
	{
		while (S_OK == lpe->Next(1,&lpi,&ulFetched))
		{
			ulAttrs = SFGAO_HASSUBFOLDER|SFGAO_FOLDER;
			
			lpsf->GetAttributesOf(1,(LPCITEMIDLIST*)&lpi,&ulAttrs);
			
			if(ulAttrs & (SFGAO_HASSUBFOLDER | SFGAO_FOLDER))
			{
				if(ulAttrs & SFGAO_FOLDER)
				{
					tvi.mask = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_PARAM;
					
					if(ulAttrs & SFGAO_HASSUBFOLDER)
					{
						tvi.cChildren=1;
						tvi.mask |= TVIF_CHILDREN;
					}
					
					lptvid = (LPTVITEMDATA)lpMalloc->Alloc(sizeof(TVITEMDATA));
					if(lptvid && GetName(lpsf,lpi,SHGDN_NORMAL,szBuff))
					{
						tvi.pszText    = szBuff;
						tvi.cchTextMax = MAX_PATH;
					
						lpifqThisItem = ConcatPidls(lpifq,lpi);					
						lptvid->lpi = CopyITEMID(lpMalloc,lpi);
					
						GetNormalAndSelectedIcons(lpifqThisItem,&tvi) ;
						
						lptvid->lpsfParent = lpsf;    //Store the parent folders SF
						lpsf->AddRef() ;
						
						if(hParent==TVI_ROOT)
							lptvid->lpifq = ConcatPidls(lpifq,lpi);
						else
							lptvid->lpifq = GetFullyQualPidl(lpsf,lpi);
						
						tvi.lParam = (LPARAM)lptvid;
						
						tvins.item = tvi;
						tvins.hInsertAfter = hPrev;
						tvins.hParent = hParent;
						
						hPrev = TreeView_InsertItem(m_hWnd,&tvins);
					}
					else
						break;
				}				
            lpMalloc->Free(lpifqThisItem) ;
            lpifqThisItem = 0;
			}
			
         lpMalloc->Free(lpi);  //Finally, free the pidl that the shell gave us...
         lpi=0;
		}
	}
	
   SetCursor(LoadCursor(NULL,IDC_ARROW));
	
   if(lpe)
		lpe->Release() ;
	
   if(lpi && lpMalloc)
		lpMalloc->Free(lpi);

   if(lpifqThisItem && lpMalloc)
		lpMalloc->Free(lpifqThisItem);
	
   if(lpMalloc)
		lpMalloc->Release();
}

LPITEMIDLIST RingDirTree::ConcatPidls(LPCITEMIDLIST pidl1,LPCITEMIDLIST pidl2)
{
   LPITEMIDLIST pidlNew;
   UINT cb1;
   UINT cb2;

   if(pidl1)  //May be NULL
      cb1 = GetSize(pidl1) - sizeof(pidl1->mkid.cb);
   else
      cb1 = 0;

   cb2 = GetSize(pidl2);

   pidlNew = CreateIDList(cb1 + cb2);
   if(pidlNew)
   {
      if(pidl1)
         memcpy(pidlNew,pidl1,cb1);
      memcpy(((LPSTR)pidlNew) + cb1, pidl2,cb2);
   }
   return pidlNew;
}

LPITEMIDLIST RingDirTree::CopyITEMID(LPMALLOC lpMalloc,LPITEMIDLIST lpi)
{
   LPITEMIDLIST lpiTemp;

   lpiTemp = (LPITEMIDLIST)lpMalloc->Alloc(lpi->mkid.cb+sizeof(lpi->mkid.cb));
   CopyMemory((PVOID)lpiTemp,(CONST VOID *)lpi,lpi->mkid.cb+sizeof(lpi->mkid.cb));

   return lpiTemp;
}

BOOL RingDirTree::GetName(LPSHELLFOLDER lpsf,LPITEMIDLIST lpi,DWORD dwFlags,
								  LPSTR lpFriendlyName)
{
   BOOL bSuccess = TRUE;
   STRRET str;

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
                                MAX_PATH,//sizeof(lpFriendlyName), // cchMultiByte,
                                NULL,                   // lpDefaultChar,
                                NULL);                  // lpUsedDefaultChar
             break;
         case STRRET_OFFSET:
             lstrcpy(lpFriendlyName,(LPSTR)lpi+str.uOffset);
             break;
         case STRRET_CSTR:
             lstrcpy(lpFriendlyName,(LPSTR)str.cStr);
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

LPITEMIDLIST RingDirTree::GetFullyQualPidl(LPSHELLFOLDER lpsf,LPITEMIDLIST lpi)
{
   char szBuff[MAX_PATH];
   OLECHAR szOleChar[MAX_PATH];
   LPSHELLFOLDER lpsfDeskTop;
   LPITEMIDLIST  lpifq;
   ULONG ulEaten, ulAttribs;
   HRESULT hr;

   if(!GetName(lpsf,lpi,SHGDN_FORPARSING,szBuff))
      return NULL;

   hr = SHGetDesktopFolder(&lpsfDeskTop);

   if(FAILED(hr))
      return NULL;

   MultiByteToWideChar(CP_ACP,
					   MB_PRECOMPOSED,
					   szBuff,
					   -1,
					   szOleChar,
					   sizeof(szOleChar));

   hr = lpsfDeskTop->ParseDisplayName(NULL,NULL,szOleChar,&ulEaten,&lpifq,&ulAttribs);

   lpsfDeskTop->Release();

   if(FAILED(hr))
      return NULL;

   return lpifq;
}

int RingDirTree::GetIcon(LPITEMIDLIST lpi,UINT uFlags)
{
   SHFILEINFO sfi;

   SHGetFileInfo((LPCSTR)lpi,
                 0,
                 &sfi,
                 sizeof(SHFILEINFO),
                 uFlags);

   return sfi.iIcon;
}

HICON RingDirTree::GetFileIcon(LPSTR lpi)
{
   SHFILEINFO sfi;

   SHGetFileInfo((LPCSTR)lpi,
                 0,
                 &sfi,
                 sizeof(SHFILEINFO),
                 SHGFI_ICON);

   return sfi.hIcon;
}

void RingDirTree::GetNormalAndSelectedIcons(LPITEMIDLIST lpifq,LPTV_ITEM lptvitem)
{
   lptvitem->iImage = GetIcon(lpifq,SHGFI_PIDL |
                              SHGFI_SYSICONINDEX |
                              SHGFI_SMALLICON);

   lptvitem->iSelectedImage = GetIcon(lpifq,SHGFI_PIDL |
                                      SHGFI_SYSICONINDEX |
                                      SHGFI_SMALLICON |
                                      SHGFI_OPENICON);
}

UINT RingDirTree::GetSize(LPCITEMIDLIST pidl)
{
	UINT cbTotal = 0;
	if(pidl)
	{
		cbTotal += sizeof(pidl->mkid.cb);
		while(pidl->mkid.cb)
		{
			cbTotal += pidl->mkid.cb;
			pidl = Next(pidl);
		}
	}	
	return cbTotal;
}

LPITEMIDLIST RingDirTree::Next(LPCITEMIDLIST pidl)
{
   LPSTR lpMem = (LPSTR)pidl;
	
   lpMem += pidl->mkid.cb;
	
   return (LPITEMIDLIST)lpMem;
}

LPITEMIDLIST RingDirTree::CreateIDList(UINT cbSize)
{
	LPMALLOC lpMalloc;
	HRESULT  hr;
	LPITEMIDLIST pidl=NULL;
	
	hr = SHGetMalloc(&lpMalloc);
	
	if(FAILED(hr))
		return NULL;
	
	pidl = (LPITEMIDLIST)lpMalloc->Alloc(cbSize);
	
	if(pidl)
		memset(pidl,0,cbSize);      // zero-init for external task   alloc
	
	if(lpMalloc)
		lpMalloc->Release();
	
	return pidl;
}
