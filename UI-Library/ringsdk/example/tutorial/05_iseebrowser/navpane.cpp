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
//							RingSDK 演示程序(ISee图象浏览器)
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
//说明：导航子窗口代码
//
**********************************************************************/
#include "global.h"

BEGIN_REGMSG(NavPane)
	REGMSG(WM_SIZE)
	REGNOTIFY(NM_DBLCLK)
	REGNOTIFY(LVN_COLUMNCLICK)
	REGNOTIFY(LVN_ITEMCHANGED)
	REGNOTIFY(TBN_DROPDOWN)
	REGNOTIFY(NM_RETURN)
	REGNOTIFY(LVN_SHOWCOLUMN)
	REGCMD(CM_UPPATH,OnUpParentPath)
	REGCMD(CM_ZOOMIN,OnZoomIn)
	REGCMD(CM_ZOOMOUT,OnZoomOut)
	REGCMD(CM_NEXTPIC,OnNextImg)
	REGCMD(CM_PREVPIC,OnPrevImg)
	REGCMDRANGE(CM_VIEWTD,CM_VIEWDETAIL,OnSetViewMode)
	REGCMDRANGE(CM_SHOWFOLDER,CM_SHOWHIDDEN,OnSetFileFilter)
	REGCMDRANGE(CM_SORTEXT,CM_SORTDESC,OnSortColumn)
	REGMSG(WM_DESTROY)
	REGMSG(WM_INITDIALOG)	
END_REGMSG

RINGMSG(NavPane,WM_INITDIALOG)
{
	ReplaceStyle(0,WS_CLIPCHILDREN|WS_CLIPSIBLINGS);
	
	//创建缩略图IMAGELIST
	m_img = new RingImageList;
	m_img->Create(128,96,ILC_COLOR32,10,10);
	//创建文件列表栏
	m_fileview = NewRingObj(itemof(RingListView));
	m_fileview->Create(LVS_AUTOARRANGE|LVS_REPORT|LVS_SINGLESEL|LVS_SHOWSELALWAYS|LVS_ALIGNTOP,//|LVS_OWNERDRAWFIXED,
							 WS_EX_CLIENTEDGE|RTV_FITPARENTSIZE,NULL,IDC_FILEVIEW,0,0,300,300);
	m_fileview->SetExStyle(LVS_EX_FLATSB | LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES|LVS_EX_HEADERDRAGDROP);
	m_fileview->SetColumns("名称|大小|类型|修改时间");
	m_fileview->SetColumnsWidth("200|80|130|130");

	//获取系统文件图标IMAGELIST
	SHFILEINFO  sfi;
	m_hSysImgS = (HIMAGELIST)SHGetFileInfo((LPCSTR)"C:\\",0,&sfi,sizeof(SHFILEINFO),
		SHGFI_SYSICONINDEX|SHGFI_ICON|SHGFI_SMALLICON);
	m_fileview->SetImageList(m_hSysImgS,LVSIL_SMALL);
	m_hSysImgL = (HIMAGELIST)SHGetFileInfo((LPCSTR)"C:\\",0,&sfi,sizeof(SHFILEINFO),
		SHGFI_SYSICONINDEX|SHGFI_ICON|SHGFI_LARGEICON);
	m_fileview->SetImageList(m_hSysImgL,LVSIL_NORMAL);
	
	m_nBarBorder = 0;
	m_nMarkCol = 0;
	m_nOrder = 1;
	m_nViewMode = 0;
	m_nFileMode = CM_VIEWDETAIL;
	m_stateViewer = TBS_FLAG_DOCKLEFT;

	//设置尺寸栏文字靠右
	LVCOLUMN lvc;
	lvc.fmt = LVCFMT_RIGHT;
	lvc.mask = LVCF_FMT;
	m_fileview->SetColumn(1,&lvc);
	
	//设置列头排序标记
	m_fileview->SetColumnImageList();
	m_nMarkUp = m_fileview->AddColumnIcon(LoadIcon(GetInstance(),MAKEINTRESOURCE(IDI_COLMARKUP)));
	m_nMarkDown = m_fileview->AddColumnIcon(LoadIcon(GetInstance(),MAKEINTRESOURCE(IDI_COLMARKDOWN)));
	m_fileview->SetColumnImage(m_nMarkCol,m_nMarkUp,TRUE);
	m_hHeaderImg  = m_fileview->GetColumnImageList();
	//允许弹出列头右键菜单选择显示/隐藏列
	RingCoolMenu* rm = m_fileview->EnableColumnMenu();
	if(rm)
   {		
		rm->SetItemDrawFunc(FUNC_DRAWMENU_XP);
		rm->SetBkgColor(0x00ffffff,0x00ff8080);
   }

	//创建工具栏
	m_toolbar = NewRingObj(itemof(RingToolBar));
	m_toolbar->Create(IDB_VIEWTOOL,TBSTYLE_FLAT);
	//创建按钮，参数：按钮序号，按钮图象在工具栏图象中序号，命令号
	m_toolbar->InsertButton(0,0,CM_PREVPIC);
	m_toolbar->InsertButton(1,1,CM_NEXTPIC);
	m_toolbar->InsertSep(2);
	m_toolbar->InsertButton(3,2,CM_ZOOMOUT);
	m_toolbar->InsertButton(4,3,CM_ZOOMIN);
	m_toolbar->InsertSep(5);
	m_toolbar->InsertButton(6,5,3);
	m_toolbar->EnableDock("查看导航栏",TBS_EX_NONE);
	m_toolbar->Show(SW_HIDE);

	m_navtool = NewRingObj(itemof(RingToolBar));
	m_navtool->Create(IDB_NAVTOOL,TBSTYLE_FLAT|TBSTYLE_LIST,
		TBSTYLE_EX_DRAWDDARROWS|TBSTYLE_EX_HIDECLIPPEDBUTTONS|TBSTYLE_EX_MIXEDBUTTONS);
	//创建按钮，参数：按钮序号，按钮图象在工具栏图象中序号，命令号
	m_navtool->InsertButton(0,0,CM_UPPATH);
	m_navtool->InsertSep(1);
	m_navtool->InsertButton(2,5,CM_FILTER,"过滤器",
		TBSTYLE_BUTTON|BTNS_AUTOSIZE|BTNS_WHOLEDROPDOWN|BTNS_SHOWTEXT);
	m_navtool->InsertButton(3,6,CM_SORTER,"排序",
		TBSTYLE_BUTTON|BTNS_AUTOSIZE|BTNS_WHOLEDROPDOWN|BTNS_SHOWTEXT);
	m_navtool->InsertButton(4,7,CM_MODE,"查看",
		TBSTYLE_BUTTON|BTNS_AUTOSIZE|BTNS_WHOLEDROPDOWN|BTNS_SHOWTEXT);
	m_navtool->InsertSep(5);
	m_navtool->InsertButton(6,24,3);
	m_navtool->EnableDock("文件导航栏",TBS_EX_NONE);
	
	SetDockBackGround(MAKEINTRESOURCE(IDB_BKG),TRUE,-1,window->Handle());

	//创建线程对象
	m_thread = new RingThread;
	return FALSE;
}

RINGMSG(NavPane,WM_DESTROY)
{
	//在WM_DESTROY调用Close()会继续产生WM_DESTROY消息，因此需判断窗口是否已被销毁
	if(IsWindow())
	{
		delete m_thread;
		delete m_img;
		Close();
	}
	return FALSE;
}

RINGMSG(NavPane,WM_SIZE)
{
	//更新ListView和工具栏，否则会花
	if(IsWindowPtr(m_fileview))
		m_fileview->Update(m_fileview->GetCount()-1);
	if(IsWindowPtr(m_toolbar))
		InvalidateRect(m_toolbar->Handle(),NULL,TRUE);
	if(IsWindowPtr(m_navtool))
		InvalidateRect(m_navtool->Handle(),NULL,TRUE);
	return DefaultProc(param);
}

//父窗口调整尺寸，激活等事件会通知子窗口，子窗口如果需要可在该函数内响应事件
BOOL RINGMETHOD(NavPane,OnParentNotify)(HWND,UINT uMsg,UINT,LONG)
{
	if(uMsg == WM_SIZE)
	{
		RECT rc;
		Parent()->GetClientRect(&rc);
		MoveWindow(m_hWnd,rc.left,rc.top,rc.right-rc.left,rc.bottom-rc.top,FALSE);
	}
	return TRUE;
}

RINGNOTIFY(NavPane,TBN_DROPDOWN)
{
	//弹出工具栏按钮菜单，这里的菜单直接扒的主窗口菜单的子菜单
	LPNMTOOLBAR lpb = (LPNMTOOLBAR)param.lParam;
	if(lpb->iItem == CM_FILTER || lpb->iItem == CM_SORTER || lpb->iItem == CM_MODE)
	{
		RingCoolMenu* rm = window->GetCoolMenu();
		if(rm)
		{
			RECT rc;
			int idx,idxm;
			if(lpb->iItem == CM_FILTER)
			{
				idx = 2;
				idxm = 1;
			}
			else if(lpb->iItem == CM_SORTER)
			{
				idx = 3;
				idxm = 3;
			}
			else
			{
				idx = 4;
				idxm = 0;
			}
			m_navtool->GetButtonRect(idx,&rc);
			//转换到屏幕坐标
			ClientToScreen(m_navtool->Handle(),(LPPOINT)&rc);
			ClientToScreen(m_navtool->Handle(),(LPPOINT)&rc+1);
			//获取子菜单
			rm = rm->SubMenu(2)->SubMenu(idxm);
			//菜单弹出时避免覆盖该工具栏按钮，避开时优先考虑垂直位置
			rm->PopupEx(m_hWnd,rc.left,rc.bottom,FALSE,&rc);
		}
	}
	return TBDDRET_DEFAULT;
}

RINGNOTIFY(NavPane,LVN_ITEMCHANGED)
{
	//选中文件，显示图片
	if(param.lpnmlistview->iItem != -1 &&
		MASK_MATCH(param.lpnmlistview->uNewState,LVIS_SELECTED))
	{
		char szFile[MAX_PATH];		
		int nData = m_fileview->GetData(param.lpnmlistview->iItem);
		LPCTSTR lps = m_fileview->GetText(param.lpnmlistview->iItem,0);
		
		wsprintf(szFile,"%s\\%s\0",window->m_dirtree->GetSelFolderName(TRUE),lps);
		window->SetStatusFilePart(szFile);
		if(HIWORD(m_fileview->GetData(param.lpnmlistview->iItem)) != 0xFFFF)
		{
			//非文件夹，试图显示图片，因缩略图模式是启动线程加载目录下所有图片，
			//而RingDIB不支持多线程，因此这里需要同步
			m_guard.Enter();
			try
			{
				child->View(szFile);
			}
			catch(...)
			{
			}
			m_guard.Leave();
		}
	}
	return 0;
}

RINGNOTIFY(NavPane,NM_RETURN)
{
	//响应回车键，全屏显示图片
	int nIndex = m_fileview->GetCurSel();
	
	if(nIndex >= 0)
	{
		char szFile[MAX_PATH];
		wsprintf(szFile,"%s\\%s\0",window->m_dirtree->GetSelFolderName(),
			m_fileview->GetText(nIndex,0));
		if(m_fileview->GetData(nIndex) != -1)
		{
			child->SetFullScreen(TRUE);
			child->View(szFile);
		}
	}
	return 0;
}

RINGNOTIFY(NavPane,NM_DBLCLK)
{
	////双击事件响应，判断是进入子目录还是全屏显示图片
	if(param.lpnmhdr->hwndFrom == m_fileview->Handle())
	{
		char szFile[MAX_PATH];
		LPNMITEMACTIVATE lpv = (LPNMITEMACTIVATE)param.lParam;
		wsprintf(szFile,"%s\\%s\0",window->m_dirtree->GetSelFolderName(),
					m_fileview->GetText(lpv->iItem,0));
		if(HIWORD(m_fileview->GetData(lpv->iItem)) == 0xFFFF)
		{
			//目录
			HTREEITEM hti = window->m_dirtree->GetSelection();
			window->m_dirtree->Expand(hti);
			hti = window->m_dirtree->SearchItem(m_fileview->GetText(lpv->iItem,0),hti);
			if(hti)
				window->m_dirtree->Select(hti);
		}
		else
		{
			child->SetFullScreen(TRUE);
			child->View(szFile);
		}
	}
	return 0;
}

RINGNOTIFY(NavPane,LVN_COLUMNCLICK)
{
	//列头点击响应，选择排序
	if(param.lpnmlistview->hdr.hwndFrom == m_fileview->Handle())
	{
		RingCoolMenu* rm = window->GetCoolMenu();
		if(rm)
			rm = rm->SubMenu(2)->SubMenu(3);

		if(m_nMarkCol == param.lpnmlistview->iSubItem)
		{
			m_nOrder = -m_nOrder;
			if(rm)
				rm->CheckRadioItem(CM_SORTASEC,CM_SORTDESC,m_nOrder>0?CM_SORTASEC:CM_SORTDESC,BY_COMMAND);
		}
		else
		{
			m_fileview->SetColumnImage(m_nMarkCol,-1);
			m_nMarkCol = param.lpnmlistview->iSubItem;
			if(rm)
				rm->CheckRadioItem(CM_SORTEXT,CM_SORTTIME,m_nMarkCol + CM_SORTNAME,BY_COMMAND);
		}
		
		m_fileview->SetColumnImage(m_nMarkCol,m_nOrder>0?m_nMarkUp:m_nMarkDown,TRUE);
		m_fileview->SortItemsEx(SortFileProc,0);
	}
	return 0;
}

RINGNOTIFY(NavPane,LVN_SHOWCOLUMN)
{
	RingCoolMenu* rm = window->GetCoolMenu();
	LPNMHEADER hd = (LPNMHEADER)param.lParam;
	if(rm)
	{
		rm = rm->SubMenu(2)->SubMenu(2);
		rm->CheckItem(hd->iItem-1,hd->iButton,BY_POSITION);
	}
	return 0;
}

//回上层目录
RINGCMD(NavPane,OnUpParentPath)
{
	HTREEITEM hti = window->m_dirtree->GetSelection();
	if(hti)
		hti = window->m_dirtree->GetParentItem(hti);
	
	if(hti)
		window->m_dirtree->Select(hti);
}

RINGCMD(NavPane,OnZoomIn)
{
	child->ZoomIn();
}

RINGCMD(NavPane,OnZoomOut)
{
	child->ZoomOut();
}

RINGCMD(NavPane,OnNextImg)
{
	child->ViewNextImg();
	m_fileview->EnsureVisible(m_fileview->GetCurSel());
}

RINGCMD(NavPane,OnPrevImg)
{
	child->ViewNextImg(FALSE);
	m_fileview->EnsureVisible(m_fileview->GetCurSel());
}

RINGCMD(NavPane,OnSetFileFilter)
{
	window->OnSetFileFilter(event);
}

//排序
RINGCMD(NavPane,OnSortColumn)
{
	RingCoolMenu* rm = window->GetCoolMenu();
	if(rm)
	{
		rm = rm->SubMenu(2)->SubMenu(3);
		if(event < CM_SORTASEC)
		{
			rm->CheckRadioItem(CM_SORTEXT,CM_SORTTIME,event,BY_COMMAND);
			if(event == CM_SORTEXT)
			{
				m_fileview->SetColumnImage(m_nMarkCol,-1);
				m_nMarkCol = -1;
			}
			else
			{
				m_fileview->SetColumnImage(m_nMarkCol,-1);
				m_nMarkCol = event - CM_SORTNAME;
			}
		}
		else
		{
			rm->CheckRadioItem(CM_SORTASEC,CM_SORTDESC,event,BY_COMMAND);
			m_nOrder = (event==CM_SORTASEC)?1:-1;
		}
	}	
	m_fileview->SetColumnImage(m_nMarkCol,m_nOrder>0?m_nMarkUp:m_nMarkDown,TRUE);
	m_fileview->SortItemsEx(SortFileProc,(m_nMarkCol==-1));

}

//设置文件列表栏查看方式
RINGCMD(NavPane,OnSetViewMode)
{
	if(m_nFileMode == (int)event)
		return;

	int nOldMode = m_nFileMode;
	BOOL bUpdate = FALSE;
	RingCoolMenu* rm = window->GetCoolMenu();
	if(rm)
	{
		rm = rm->SubMenu(2)->SubMenu(0);
		rm->CheckRadioItem(CM_VIEWTD,CM_VIEWDETAIL,event,BY_COMMAND);
	}
	
	m_nFileMode = event;
	switch(event)
	{
	case CM_VIEWBIGICON:
		m_fileview->SetViewOf(LVS_ICON);
		if(nOldMode <= CM_VIEWD)
			bUpdate = TRUE;
		break;
	case CM_VIEWSMALLICON:
		m_fileview->SetViewOf(LVS_SMALLICON);
		if(nOldMode <= CM_VIEWD)
			bUpdate = TRUE;
		break;
	case CM_VIEWLIST:
		m_fileview->SetViewOf(LVS_LIST);
		if(nOldMode <= CM_VIEWD)
			bUpdate = TRUE;
		break;
	case CM_VIEWDETAIL:
		if(nOldMode == CM_VIEWTD)
			m_fileview->EnsureVisible(0);
		m_fileview->SetViewOf(LVS_REPORT);
		if(nOldMode <= CM_VIEWD)
			bUpdate = TRUE;
		break;
	case CM_VIEWD:
		m_fileview->SetViewOf(LVS_ICON);
		if(nOldMode == CM_VIEWTD)	//缩略图间切换，UPDATE即可
			m_fileview->Update(m_fileview->GetCount()-1);
		else
			bUpdate = TRUE;
		break;
	case CM_VIEWTD:
		if(nOldMode == CM_VIEWDETAIL)
			m_fileview->EnsureVisible(0);
		m_fileview->SetViewOf(LVS_REPORT);
		if(nOldMode == CM_VIEWD)	//缩略图间切换，UPDATE即可
			m_fileview->Update(m_fileview->GetCount()-1);
		else
			bUpdate = TRUE;
		break;
	}
	if(bUpdate)
	{
		//先终止可能正在运行的线程
		m_thread->Quit(1);
		//启动
		m_thread->Start(this,(THREADFUNC)ViewThumbProc,nOldMode);
	}
}

void RINGMETHOD(NavPane,UpdateFileList)()
{
	//清空缩略图
	m_img->Clear();

	if(m_nFileMode <= CM_VIEWD)
	{
		//先终止可能正在运行的线程
		m_thread->Quit(1);
		//启动
		m_thread->Start(this,(THREADFUNC)ViewThumbProc,m_nFileMode);
	}
}

/************************************************************
//
//创建并显示缩略图线程
//
//以下三种情况会启动本线程：
//1、非缩略图显示模式切换到缩略图显示模式（缩略图或缩略图+详细资料）
//2、缩略图显示模式切换到非缩略图显示模式
//3、缩略图显示模式下切换文件夹（此时nOldMode==m_nFileMode）
//非缩略图显示模式间切换和缩略图显示模式间切换不启动本线程
//
//本线程完成工作：
//如未生成缩略图（m_img->Count()==0），生成缩略图并加入到m_img
//如已生成缩略图并需要切换IMAGELIST，切换IMAGELIST并取原记录的
//IMAGE序号进行设置，同时记录每一项原IMAGE序号以备下一次切换
//
//**********************************************************/
DWORD RINGMETHOD(NavPane,ViewThumbProc)(LONG nOldMode)
{
	int cnt = m_fileview->GetCount();
	int nData,nImg;
	LVITEM lvm;

	lvm.mask = LVIF_IMAGE|LVIF_PARAM;
	lvm.iSubItem = 0;

	//非缩略图显示模式间切换和缩略图显示模式间切换不启动本线程，
	//因此可用m_nFileMode和nOldMode大小来判断
	if(m_nFileMode > nOldMode)	//缩略图显示模式切换到非缩略图显示模式
	{
		//恢复原IMAGELIST
		m_fileview->SetImageList(m_hSysImgS,LVSIL_SMALL);
		m_fileview->SetImageList(m_hSysImgL,LVSIL_NORMAL);
		//设置IMAGELIST会同时设置列头的IMAGELIST，因此需再次设置回来
		m_fileview->SetColumnImageList(m_hHeaderImg);
		m_fileview->SetColumnImage(m_nMarkCol,m_nOrder>0?m_nMarkUp:m_nMarkDown,TRUE);

		//记录当前图标，恢复原系统图标
		for(int i=0;i<cnt;i++)
		{
			lvm.iItem = i;
			m_fileview->GetItemInfo(&lvm);
			nData = lvm.lParam;
			lvm.lParam = MAKELONG(lvm.iImage,HIWORD(nData));
			lvm.iImage = LOWORD(nData);
			m_fileview->SetItem(&lvm);			
		}
		m_fileview->Update(i-1);
		m_fileview->EnsureVisible(m_fileview->GetCurSel());
		return 0x900D;
	}
	else
	{
		//缩略图显示模式
		m_fileview->SetImageList(*m_img,LVSIL_SMALL);
		m_fileview->SetImageList(*m_img,LVSIL_NORMAL);
		//设置IMAGELIST会同时设置列头的IMAGELIST，因此需再次设置回来
		m_fileview->SetColumnImageList(m_hHeaderImg);
		m_fileview->SetColumnImage(m_nMarkCol,m_nOrder>0?m_nMarkUp:m_nMarkDown,TRUE);

		if(m_img->Count() > 0 && nOldMode != m_nFileMode)
		{
			//非缩略图显示模式切换到缩略图显示模式且已生成缩略图，
			//记录当前图标，恢复原缩略图图标
			for(int i=0;i<cnt;i++)
			{
				lvm.iItem = i;
				m_fileview->GetItemInfo(&lvm);
				nData = lvm.lParam;
				lvm.lParam = MAKELONG(lvm.iImage,HIWORD(nData));
				lvm.iImage = LOWORD(nData);
				m_fileview->SetItem(&lvm);
				m_fileview->Update(i);
			}
			m_fileview->EnsureVisible(m_fileview->GetCurSel());
			return 0x900D;					
		}			

		//创建缩略图
		RingDIB dibPic;
		char szFile[MAX_PATH];
		LPSTR lps = (LPSTR)window->m_dirtree->GetSelFolderName();
		int w,h;
		HDC hDC = GetDC(NULL);
		HDC hMemDC = CreateCompatibleDC(hDC);
		HDC hBkgDC = CreateCompatibleDC(hDC);
		HBITMAP hobm,hbm = CreateCompatibleBitmap(hDC,128,96);
		HBITMAP hbmBkg = LoadBitmap(GetInstance(),MAKEINTRESOURCE(IDB_THUMBBKG));
		HBRUSH hbr = (HBRUSH)GetStockObject(GRAY_BRUSH);
		RECT rc;
		BOOL bLoadOK;

		hobm = (HBITMAP)SelectObject(hMemDC,hbm);
		SelectObject(hBkgDC,hbmBkg);

		m_img->Clear();
		
		window->m_progress->SetRange(0,cnt-1);
		window->m_progress->SetStep(1);

		for(int i=0;i<cnt;i++)
		{
			if(!m_thread->IsRunning())		  //查询线程退出标志
				break;

			nImg = -1;

			SetRect(&rc,0,0,128,96);
			BitBlt(hMemDC,0,0,128,96,hBkgDC,0,0,SRCCOPY);
			DrawEdge(hMemDC,&rc,BDR_RAISEDINNER,BF_RECT);

			wsprintf(szFile,"%s\\%s\0",lps,m_fileview->GetText(i,0));
			lvm.iItem = i;
			m_fileview->GetItemInfo(&lvm);
			nData = lvm.lParam;
			//为防用户此时点击文件查看，需要进行同步
			m_guard.Enter();
			try
			{
				bLoadOK = (HIWORD(nData) != 0xFFFF && dibPic.Load(szFile));
			}
			catch(...)
			{
				bLoadOK = FALSE;
			}
			m_guard.Leave();
			
			if(bLoadOK)
			{
				//绘制缩略图
				if(dibPic.Width() > dibPic.Height())
				{
					w = 120;
					h = 120 * dibPic.Height() / dibPic.Width();
				}
				else
				{
					h = 90;
					w = 90 * dibPic.Width() / dibPic.Height();
				}
				rc.left = (128-w)/2 - 1;
				rc.top = (96-h)/2 - 1;
				rc.right = rc.left + w + 1;
				rc.bottom = rc.top + h + 1;

				dibPic.Draw(hMemDC,rc.left + 1,rc.top + 1,0,0,w,h);
				
				lvm.lParam = MAKELONG(lvm.iImage,HIWORD(nData));
			}
			else
			{
				//目录或载入图象失败，先搜索已生成的图标缩略图
				h = lvm.iImage;

				for(int j=0;j<i;j++)
				{
					lvm.iItem = j;
					m_fileview->GetItemInfo(&lvm);
					if(h == LOWORD(lvm.lParam))
					{
						//当前IMAGE序号与前面项记录的原IMAGE序号相同，采用
						//匹配项的缩略图序号
						nImg = lvm.iImage;
						break;
					}
				}
				lvm.lParam = MAKELONG(h,HIWORD(nData));
				if(nImg == -1)
				{
					//绘制图标缩略图
					ImageList_GetIconSize(m_hSysImgL,&w,&h);
					rc.left = (128-w)/2 - 1;
					rc.top = (96-h)/2 - 1;
					rc.right = rc.left + w + 1;
					rc.bottom = rc.top + h + 1;
					ImageList_Draw(m_hSysImgL,LOWORD(nData),hMemDC,rc.left+1,rc.top+1,ILD_TRANSPARENT);
				}				
			}
			DrawEdge(hMemDC,&rc,BDR_SUNKENINNER,BF_RECT);
			
			SelectObject(hMemDC,hobm);
			if(nImg == -1)
				lvm.iImage = m_img->Add(hbm);
			else
				lvm.iImage = nImg;
			lvm.iItem = i;
			m_fileview->SetItem(&lvm);
			m_fileview->Update(i);

			hobm = (HBITMAP)SelectObject(hMemDC,hbm);
			window->m_progress->StepIt();
			Sleep(1);
		}
		window->m_progress->Clear();
		//m_fileview->Update(i-1);

		DeleteObject(hbmBkg);
		DeleteObject(hbm);
		DeleteDC(hBkgDC);
		DeleteDC(hMemDC);		
		ReleaseDC(m_hWnd,hDC);		
	}
	return 0xDEAD;
}

/*
RINGMSG(NavPane,WM_DRAWITEM)
{
	if(param.lpdrawitem->CtlType == ODT_LISTVIEW)
	{
		LPCTSTR lpc = m_fileview->GetText(param.lpdrawitem->itemID,0,MAX_PATH);
		RECT rc;
		int x=1,y;
		
		if((param.lpdrawitem->itemID & 0x1) == 1)
		{
			if((param.lpdrawitem->itemState & ODS_SELECTED))
			{
				FillRect(param.lpdrawitem->hDC,&param.lpdrawitem->rcItem,m_brsel);
				SetTextColor(param.lpdrawitem->hDC,m_crSel);
			}
			else
			{
				FillRect(param.lpdrawitem->hDC,&param.lpdrawitem->rcItem,m_br1);
				SetTextColor(param.lpdrawitem->hDC,0);
			}
			y = 1;
		}
		else
		{
			if((param.lpdrawitem->itemState & ODS_SELECTED))
			{
				FillRect(param.lpdrawitem->hDC,&param.lpdrawitem->rcItem,m_brsel);
				SetTextColor(param.lpdrawitem->hDC,m_crSel);
			}
			else
			{
				FillRect(param.lpdrawitem->hDC,&param.lpdrawitem->rcItem,m_br2);
				SetTextColor(param.lpdrawitem->hDC,0);
			}
			y = 0;
		}
		
		for(int i=0;i<20;i++)
		{
			lpc = m_fileview->GetText(param.lpdrawitem->itemID,i,MAX_PATH);
			if(lpc && strlen(lpc) > 0)
			{
				ListView_GetSubItemRect(m_fileview->Handle(),
					param.lpdrawitem->itemID,
					i,LVIR_BOUNDS,&rc);
				TextOut(param.lpdrawitem->hDC,
					rc.left+2,
					rc.top+2,
					lpc,
					strlen(lpc));
				//if(x > 0)
				InflateRect(&rc,x,y);
				if(x == 1)
					x = 0;
				else
					x = 1;
				
				FrameRect(param.lpdrawitem->hDC,&rc,m_brline);
			}
			else
				break;
		}
		return TRUE;
	}
	return DefaultProc(param);
}
*/

//////////////////////////////////////////////////////////
//
//改变查看模式，因为NavPane不是主窗口，浏览器窗口在不同窗口
//停靠需要先脱离原窗口，即不停靠，然后再停靠到目标窗口。
//
//注：同一窗口在不同窗口停靠，这是停靠窗口的非典型用法，一般
//		用不着。
//
//////////////////////////////////////////////////////////
void RINGMETHOD(NavPane,SetViewMode)(int iFlag)
{
	if(iFlag != m_nViewMode)
	{
		//改变查看模式
		if(iFlag == 0)
		{
			//文件列表模式
			m_toolbar->Show(SW_HIDE);

			if(child->Parent() == this)
			{
				child->DisableDock();
				child->SetParent(window->Handle());
				child->SetParentObj(window->Handle());

				child->EnableDockSize("图片浏览器",
											m_RectViewer.right-m_RectViewer.left-2*m_nBarBorder,
											m_RectViewer.bottom-m_RectViewer.top-2*m_nBarBorder,
											m_stateViewer);
				child->DockTo(m_stateViewer,&m_RectViewer);
			}
			m_nViewMode = iFlag;
		}
		else if(iFlag == 1)
		{
			//查看模式
			if(child->Parent() == window)
			{
				//保存浏览器窗口停靠位置，以便再次停靠回去
				RingDockBar *bar = child->GetDockBar();
				if(bar)
				{
					bar->GetDockRect(&m_RectViewer);
					m_stateViewer = bar->GetState();
					m_nBarBorder = bar->GetBarBorder();
				}
				m_toolbar->Show();
				child->DisableDock();
				child->SetParent(m_hWnd);
				child->SetParentObj(m_hWnd);				
				child->EnableDockSize("图片浏览器",200,200,TBS_FLAG_DOCKTOP,TBS_EX_NONE);
				
				//由于停靠窗口排列是按高度中线计算，图片显示窗口较高，会停靠到最后一行，因此
				//这里要调整使文件操作工具栏停靠在最后一行
				RECT rc;
				GetWindowRect(m_navtool->Handle(),&rc);
				OffsetRect(&rc,-rc.left,child->Height());
				m_navtool->DockTo(TBS_FLAG_DOCKTOP,&rc);
			}
			m_nViewMode = iFlag;
		}
	}	
}

//文件排序回调函数
int CALLBACK RINGMETHOD(NavPane,SortFileProc)(LPARAM lParam1,LPARAM lParam2,LPARAM lParamSort)
{
	char szCmp[MAX_PATH];
	int nRes;
	memset(szCmp,0,MAX_PATH);
	if(lParamSort)
	{
		//扩展名排序
		ringFile rf1 =	NavPane->m_fileview->GetText(lParam1,0,MAX_PATH);
		ringFile rf2 =	NavPane->m_fileview->GetText(lParam2,0,MAX_PATH);
		nRes = strcmpi(rf1.Extname(),rf2.Extname());
	}
	else
	{
		//其他
		NavPane->m_fileview->GetText(lParam1,NavPane->m_nMarkCol,MAX_PATH,szCmp);
		if(HIWORD(NavPane->m_fileview->GetData(lParam1)) == 0xFFFF || 
			HIWORD(NavPane->m_fileview->GetData(lParam2)) == 0xFFFF)
		{
			//文件夹
			nRes = (short)HIWORD(NavPane->m_fileview->GetData(lParam1)) - (short)HIWORD(NavPane->m_fileview->GetData(lParam2));
			if(nRes == 0)
				nRes = strcmpi(szCmp,NavPane->m_fileview->GetText(lParam2,NavPane->m_nMarkCol,MAX_PATH));
		}
		else if(NavPane->m_nMarkCol == 1)
		{
			//尺寸
			nRes = strlen(szCmp) - strlen(NavPane->m_fileview->GetText(lParam2,NavPane->m_nMarkCol,MAX_PATH));
			if(nRes == 0)
			{
				nRes = strcmpi(szCmp,NavPane->m_fileview->GetText(lParam2,NavPane->m_nMarkCol,MAX_PATH));
				if(nRes == 0)
					//data高位为尺寸余量(文件夹为-1，最小)
					nRes = (short)HIWORD(NavPane->m_fileview->GetData(lParam1)) - 
							 (short)HIWORD(NavPane->m_fileview->GetData(lParam2));
			}
		}
		else
			nRes = strcmpi(szCmp,NavPane->m_fileview->GetText(lParam2,NavPane->m_nMarkCol,MAX_PATH));
	}
	return nRes * NavPane->m_nOrder;
}

