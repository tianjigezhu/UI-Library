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
//说明：不知大家是否还记得网上曾有个开源项目ISee图象浏览器，当初的目标
//		  是做个超越ACDSee的浏览器出来，后来由于种种原因，变成只提供图象
//		  编解码的项目，最后这个项目终止了。临风当时任这个项目的图象处理
//		  组的负责人，ISee项目的夭折是很可惜的，在这里我想把这个图象浏览
//		  器作为演示程序做出来，虽然比不上ACDSee，图象编解码的部分也没有
//		  使用这个项目的成果代码，但也算是表达一下对项目组成员的敬意，完
//		  成一个未了的心愿吧。
//
//本程序在上一个演示程序基础上演示：
//		创建菜单栏
//		停靠窗口/对话框
//		展现RingDIB的威力，在帮助信息对话框上演示水波效果
//		线程类的调用
//		ListView控件使用
//		文件夹列表树使用
//		模拟ACDSee4.0界面，因ACDSee功能太多，因此仅实现部分功能及保留部分
//		界面，有兴趣的可以添加代码完成这些功能，记得到时发一份代码给我
//
**********************************************************************/
#include "global.h"

//主窗口消息绑定
BEGIN_REGMAINMSG
	REGCMD(CM_OPEN,OnOpen)
	REGCMD(CM_EXIT,OnExit)
	REGCMD(CM_ABOUT,OnHelp)
	REGNOTIFY(TVN_SELCHANGED)
	REGNOTIFY(TCN_SELCHANGE)	
	REGMSG(WM_MENUSELECT)
	REGMSG(WM_DRAWITEM)
	REGMSG(WM_ACTIVATEAPP)
	REGCMDRANGE(CM_VIEWTD,CM_VIEWDETAIL,OnSetViewMode)
	REGCMDRANGE(CM_SHOWFOLDER,CM_SHOWHIDDEN,OnSetFileFilter)
	REGCMDRANGE(CM_COLSIZE,CM_COLTIME,OnSetColumn)
	REGCMDRANGE(CM_SORTEXT,CM_SORTDESC,OnSortColumn)
	REGMSG(WM_DESTROY)	
	REGMSG(WM_CREATE)	
END_REGMSG

int RingMain()
{
	//初始化OLE库，因为后面要调用系统SHELL
	CoInitialize(NULL);
	//设置快捷键
	RingApp.SetAccelTable(MAKEINTRESOURCE(IDR_ACCEL));
	APP_CREATEWINDOW("Isee浏览器[演示版本]","ISeeBrowser",WS_OVERLAPPEDWINDOW|WS_CLIPCHILDREN|WS_CLIPSIBLINGS,
							MAKEINTRESOURCE(IDR_MAINMENU));
	return RingApp.Run();
}

RINGMAINMSG(WM_CREATE)
{	
	//设定支持的图象格式，目前支持以下四种格式。BMP为默认，不需要设定。
	DIBREGFORMAT(GIF);
	DIBREGFORMAT(JPG);
	DIBREGFORMATS("jpeg",JPG);	//设定多格式支持解码
	DIBREGFORMAT(PNG);
	DIBREGFORMAT(PCX);

	//设置窗口图标
	SetIcon(LoadIcon(GetInstance(),MAKEINTRESOURCE(IDI_APP)));
	
	//设置XP样式菜单
	RingCoolMenu* rm = InstallXPMenu();

	//创建菜单栏
	RingMenuBar *menubar = NewRingObj(itemof(RingMenuBar));
	menubar->LoadMenu(rm);
	
	//设置程序背景
	SetDockBackGround(MAKEINTRESOURCE(IDB_BKG),TRUE);

	//设置菜单项图标
	rm->SubMenu(2)->SubMenu(3)->SetItemIcon(CM_SORTASEC,LoadIcon(GetInstance(),MAKEINTRESOURCE(IDI_COLMARKUP)));
	rm->SubMenu(2)->SubMenu(3)->SetItemIcon(CM_SORTDESC,LoadIcon(GetInstance(),MAKEINTRESOURCE(IDI_COLMARKDOWN)));
	//CHECK一下，否则图标与选中标志图标会重叠绘制
	rm->SubMenu(2)->SubMenu(3)->CheckRadioItem(CM_SORTASEC,CM_SORTDESC,CM_SORTASEC,BY_COMMAND);
	//设置停靠栏菜单(在工具栏停靠位置空白处的右键弹出菜单)为XP样式
	rm = GetDockMenu();
	if(rm)
   {		
		rm->SetItemDrawFunc(FUNC_DRAWMENU_XP);
		rm->SetBkgColor(0x00ffffff,0x00ff8080);
   }

	//设置文件过滤标志
	m_dwFilter = FILTER_FOLDER|FILTER_PIC|FILE_ATTRIBUTE_NORMAL|FILE_ATTRIBUTE_ARCHIVE;

	//创建状态栏
	m_iconFile = NULL;
	m_statbar = NewRingObj(itemof(RingStatusBar));
	m_statbar->Create();
	int np[5] = {
		MAKESBPINT(0,SBTS_BORDER,SBPW_FITTEXT),	 //状态栏格子宽度自适应文字宽度
		MAKESBPINT(0,SBTS_BORDER,SBPW_FITTEXT),
		MAKESBPINT(0,SBTS_BORDER,SBPW_FITTEXT),
		MAKESBPINT(0,SBTS_BORDER,SBPW_FITTEXT),
		MAKESBPINT(0,SBTS_BORDER,SBPW_STRETCH)		 //状态栏格子宽度自适应窗口宽度
	};
	m_statbar->SetParts(5,np);

	//创建进度条
	m_progress = NewRingObj(itemof(RingProgress));
	m_progress->Create(PBS_SMOOTH,RTV_FITPARENTSIZE,NULL,IDC_PROGRESS,0,0,0,0);
	//加入到状态栏
	m_statbar->InsertItem(m_progress,4);

	ringDateTime rdt;
	char szText[40];
	wsprintf(szText,"%d-%02d-%02d %02d:%02d",rdt.year(),rdt.month(),rdt.day(),
				rdt.hour(),rdt.minute());
	m_statbar->SetValue(szText,1);

	//创建导航栏，RTV_FITPARENTSIZE扩展类型表示自适应父窗口尺寸
	m_navbar = NewRingObj(itemof(RingTab));
	m_navbar->Create(TCS_MULTILINE|TCS_HOTTRACK|TCS_OWNERDRAWFIXED,
							RTV_FITPARENTSIZE,NULL,IDC_NAVBAR,0,0,300,400);
	m_navbar->InsertTabs("文件夹|相册|收藏夹|搜索");

	//创建文件夹列表树，父窗口为m_navbar，因此需调用m_navbar->NewRingObj
	m_dirtree = m_navbar->NewRingObj(itemof(RingDirTree));
	m_dirtree->Create(IDC_FOLDERTREE);
	//自适应父窗口尺寸
	m_dirtree->SetAutoSize();
	m_dirtree->ListDir();
	//绑定文件夹列表树到第一个标签
	m_navbar->SetTabToWnd(0,m_dirtree);
	
	//创建侧边栏
	m_navbar->EnableDockSize("导航栏",200,400,TBS_FLAG_DOCKLEFT);

	//创建导航子窗口
	m_tab = NewRingObj(itemof(RingTabEx));
	m_tab->Create(TCS_HOTTRACK|TCS_OWNERDRAWFIXED,
						RTV_FITPARENTSIZE,NULL,IDC_NAVPANE,0,0,300,400);
	m_tab->InsertTabs("文件|查看|属性");
	
	NavPane = m_tab->NewRingObj(NavPane,TRUE);
	NavPane->Create("导航窗口","ISeeNavPane",WS_CHILD,0,400,400,200,200);
	m_tab->SetTabToWnd(0,NavPane);
	m_tab->SetTabToWnd(1,NavPane);
	m_tab->Select(0);
	
	//获取文件列表栏
	m_fileview = NavPane->m_fileview;
			
	//创建图片浏览器
	child = NewRingObj(child,TRUE);
	child->Create("图片浏览器","ISeeViwer",WS_CHILD,0,0,0,200,200);
	child->EnableDockSize("图片浏览器",200,200);
	//停靠到左下方
	RECT rc;
	GetWindowRect(m_navbar->Handle(),&rc);
	ScreenToClient(m_hWnd,(LPPOINT)&rc);
	ScreenToClient(m_hWnd,(LPPOINT)&rc+1);
	rc.top = (rc.top + rc.bottom)/2;
	child->DockTo(TBS_FLAG_DOCKLEFT,&rc);
	
	return DefaultProc(param);
}

RINGMAINMSG(WM_MENUSELECT)
{
	//在状态栏显示菜单帮助信息
	return m_statbar->OnMenuHelp(param,GetMenu());
}

RINGMAINMSG(WM_ACTIVATEAPP)
{
	if(param.wParam && child && child->m_bFullScreen)
	{
		//全屏方式下设置图片窗口为活动窗口，否则该窗口接收不到按键
		child->SetFocus();
	}
	return DefaultProc(param);
}

RINGMAINNOTIFY(TCN_SELCHANGE)
{
	//标签页通知消息
	if(param.lpnmhdr->hwndFrom == m_navbar->Handle())
		return m_navbar->OnTabSelect(param);	//交由控件自己处理
	else if(param.lpnmhdr->hwndFrom == m_tab->Handle())
	{
		NavPane->SetViewMode(m_tab->GetCurSel());
		return m_tab->OnTabSelect(param);
	}
	return DefaultProc(param);
}

RINGMAINNOTIFY(TVN_SELCHANGED)
{
	//用户选择目录的处理，先交由控件响应
	m_dirtree->OnSelectChanged(param);
	//填充状态拦信息
	SetStatusFilePart(m_dirtree->GetSelFolderName());
	m_statbar->SetValue("",3);
	ListFile();
	return 0;
}

void RINGMAINMETHOD(SetStatusFilePart)(LPCTSTR lpszFile)
{
	SHFILEINFO sfi;
	if(lpszFile)
	{
		SHGetFileInfo(lpszFile, 
							0,
							&sfi, 
							sizeof(SHFILEINFO), 
							SHGFI_ICON|SHGFI_SMALLICON|SHGFI_DISPLAYNAME);
	
		if(m_iconFile)
		{
			m_statbar->SetIcon(2,NULL);
			DestroyIcon(m_iconFile);
		}
		m_iconFile = sfi.hIcon;
		m_statbar->SetIcon(2,m_iconFile);
		window->m_statbar->SetValue(sfi.szDisplayName,2);

		ringFile rf = lpszFile;
		FILETIME fTime;
		ringDateTime rdt;
		rf.GetFileTime(NULL,NULL,&fTime);
		rdt = &fTime;
		char szText[40];
		wsprintf(szText,"%d-%02d-%02d %02d:%02d",rdt.year(),rdt.month(),rdt.day(),
			rdt.hour(),rdt.minute());
		m_statbar->SetValue(szText,1);
	}
}

void RINGMAINMETHOD(ListFile)()
{
	//获取选择的目录后列出图象文件
	ringDir dir(m_dirtree->GetSelFolderName());
	ringFile rf;
	ringStr str;
	char szFile[MAX_PATH],szSize[40];
	int index,nSubIdx,nSize,nData;
	FILETIME fTime;
	ringDateTime rdt;
	DWORD dwAttr,dwCheck;
	int nFileCnt=0,nTotalSize=0;

	if(IsWindowPtr(m_fileview))		//检测m_fileview是否有效及已创建
		m_fileview->Clear();

	if(dir.BeginFindFile())
	{
		SHFILEINFO sfi;
		while(dir.FindNext(szFile))
		{
			if(strcmp(szFile,"..") == 0)
				continue;

			rf.SetFile("%s\\%s",dir.Getdir(),szFile);
			str = rf.Extname();
			str.toLower();
			dwAttr = rf.GetAttrib() & (FILTER_FOLDER|FILTER_HIDDEN|FILE_ATTRIBUTE_ARCHIVE);
			dwCheck = m_dwFilter & dwAttr;
			if(MASK_MATCH(dwAttr,FILTER_HIDDEN) && !MASK_MATCH(dwCheck,FILTER_HIDDEN))
				dwCheck = 0;
			if((dwAttr != 0xFFFFFFFF && dwCheck > 0) && (MASK_MATCH(dwCheck,FILTER_FOLDER) ||
				(MASK_MATCH(m_dwFilter,FILTER_PIC) && (str == "bmp" || str == "jpg" || 
				str == "jpeg" || str == "gif" || str == "pcx" || str == "png")) ||
				MASK_MATCH(m_dwFilter,FILTER_OTHER)))
			{				
				index = m_fileview->GetCount();				
				SHGetFileInfo((LPCTSTR)rf.Fullname(), 
					0,
					&sfi, 
					sizeof(SHFILEINFO), 
					SHGFI_SYSICONINDEX|SHGFI_SMALLICON|
					SHGFI_DISPLAYNAME|SHGFI_ATTRIBUTES|SHGFI_TYPENAME);
				
				nData = nSize = rf.Size();
				nTotalSize += nSize;
				nFileCnt ++;
				if(nSize > 0)
					nSize = nSize/1024 + 1;

				if(!MASK_MATCH(sfi.dwAttributes,SFGAO_FOLDER))
				{
					memset(szSize,0,40);					
					int x = 1000000000;
					while(x > 1)
					{
						if(nSize/x > 0)
							wsprintf(szSize,"%s%d,",szSize,(nSize/x)%1000);
						x /= 1000;
					}
					wsprintf(szSize,"%s%dKB",szSize,nSize%1000);
				}
				else
				{
					wsprintf(szSize,"\0");
					nData = -1;
				}
				//设置绑定数据，低位图标序号，高位为尺寸后三位，避免new一个
				//结构完整记录这三个信息，按尺寸排序需要先比较字符串长度，
				//然后字符串，然后nData的高位
				nData = MAKELONG(sfi.iIcon,nData%1000);
				m_fileview->AddItem(index,sfi.szDisplayName,nData,sfi.iIcon);
				nSubIdx = 1;
				m_fileview->SetItem(index,nSubIdx++,szSize);
				m_fileview->SetItem(index,nSubIdx++,sfi.szTypeName);

				rf.GetFileTime(NULL,NULL,&fTime);
				rdt = &fTime;
				wsprintf(szSize,"%d-%02d-%02d %02d:%02d",rdt.year(),rdt.month(),rdt.day(),
							rdt.hour(),rdt.minute());
				m_fileview->SetItem(index,nSubIdx++,szSize);
			}
		}
		m_fileview->SortItemsEx(RINGCLASS(NavPane)::SortFileProc,(NavPane->m_nMarkCol==-1));
		NavPane->UpdateFileList();
	}

	int nRest=0;
	if(nTotalSize < 1000)
		wsprintf(szSize,"字节");
	else if(nTotalSize < 1000000)
	{
		nRest = (nTotalSize%1000)/100;
		nTotalSize /= 1000;
		wsprintf(szSize,"KB");
	}
	else
	{
		nRest = ((nTotalSize/1000)%1000)/100;
		nTotalSize /= 1000000;
		wsprintf(szSize,"MB");
	}
	if(nRest == 0)
		wsprintf(szFile,"总计 %d 个文件(%d%s)",nFileCnt,nTotalSize,szSize);
	else
		wsprintf(szFile,"总计 %d 个文件(%d.%d%s)",nFileCnt,nTotalSize,nRest+1,szSize);
	m_statbar->SetValue(szFile,0);
}

RINGMAINMSG(WM_DESTROY)
{
	//主窗口关闭时的清理代码，如果响应本消息，必须在最后调用Quit()，
	//即PostQuitMessage,否则窗口销毁，但程序仍滞留内存。
	if(m_iconFile)
	{
		m_statbar->SetIcon(2,NULL);
		DestroyIcon(m_iconFile);
	}
	CoUninitialize();
	return Quit();	
}

//绘制标签
RINGMAINMSG(WM_DRAWITEM)
{
	if(param.lpdrawitem->CtlType == ODT_TAB)
	{
		if(param.lpdrawitem->itemAction == ODA_DRAWENTIRE)
		{
			RingTab* tab = NULL;
			if(param.wParam == IDC_NAVBAR)
				tab = m_navbar;
			else if(param.wParam == IDC_NAVPANE)
				tab = m_tab;

			if(tab)
			{
				LPSTR lps = (LPSTR)tab->GetText(param.lpdrawitem->itemID);
				int len = strlen(lps);
				SetBkMode(param.lpdrawitem->hDC,TRANSPARENT);
				if(param.lpdrawitem->itemState == ODS_SELECTED)
					DrawText(param.lpdrawitem->hDC,lps,len,&param.lpdrawitem->rcItem,DT_SINGLELINE|DT_CENTER|DT_VCENTER);
				else
					DrawText(param.lpdrawitem->hDC,lps,len,&param.lpdrawitem->rcItem,DT_SINGLELINE|DT_CENTER|DT_BOTTOM);
			}
		}
	}
	return TRUE;
}

RINGMAINCMD(OnOpen)
{
	//这里添加打开文件代码...	
}

RINGMAINCMD(OnExit)
{
	//发送程序退出消息
	SendExit();
}

RINGMAINCMD(OnHelp)
{
	//标准对话框调用代码
	DlgAbout = NewRingObj(DlgAbout);
	DlgAbout->DoModal();
	//如果窗口类未设置自动销毁（调用SetAutoDelete），则需要销毁对象	
	DelRingObj(DlgAbout);
}

RINGMAINCMD(OnSetViewMode)
{
	//设置文件列表查看方式
	NavPane->OnSetViewMode(event);
}

RINGMAINCMD(OnSetFileFilter)
{
	//设置文件过滤
	RingCoolMenu* rm = GetCoolMenu();
	BOOL bCheck = FALSE;
	if(rm)
	{
		rm = rm->SubMenu(2)->SubMenu(1);
		bCheck = !MASK_MATCH(rm->GetItemCheckState(event),MFS_CHECKED);
		rm->CheckItem(event,bCheck);
	
		switch(event)
		{
		case CM_SHOWFOLDER:
			if(bCheck)
				m_dwFilter |= FILTER_FOLDER;
			else
				m_dwFilter &= ~FILTER_FOLDER;
			break;
		case CM_SHOWPIC:
			if(bCheck)
				m_dwFilter |= FILTER_PIC;
			else
				m_dwFilter &= ~FILTER_PIC;
			break;
		case CM_SHOWALBUM:
			if(bCheck)
				m_dwFilter |= FILTER_ALBUM;
			else
				m_dwFilter &= ~FILTER_ALBUM;
			break;
		case CM_SHOWMEDIA:
			if(bCheck)
				m_dwFilter |= FILTER_MEDIA;
			else
				m_dwFilter &= ~FILTER_MEDIA;
			break;
		case CM_SHOWOTHER:
			if(bCheck)
				m_dwFilter |= FILTER_OTHER;
			else
				m_dwFilter &= ~FILTER_OTHER;
			break;
		case CM_SHOWHIDDEN:
			if(bCheck)
				m_dwFilter |= FILTER_HIDDEN;
			else
				m_dwFilter &= ~FILTER_HIDDEN;
			break;
		}
		ListFile();
	}
}

RINGMAINCMD(OnSetColumn)
{
	//显示/隐藏文件列表列，为RingListView的扩展特性
	RingCoolMenu* rm = GetCoolMenu();
	BOOL bCheck = FALSE;
	int idx = event-CM_COLSIZE;
	int nIndex = idx + 1;
	if(rm)
	{
		rm = rm->SubMenu(2)->SubMenu(2);
		bCheck = !MASK_MATCH(rm->GetItemCheckState(event),MFS_CHECKED);
		rm->CheckItem(event,bCheck);
		m_fileview->ShowColumn(nIndex,bCheck);
	}
}

RINGMAINCMD(OnSortColumn)
{
	//选择排序
	NavPane->OnSortColumn(event);	
}

BOOL RingTabEx::OnDrawBkg(HDC hDC)
{
	//获取停靠坞背景，计算位置偏移进行绘制，保持整个窗口背景一致
	RingDockSite* tmp = ((RingWnd*)Parent())->GetDockSite(TBS_FLAG_DOCKTOP);
	RECT rc;
	SIZE offset={0,0};
	
	if(tmp)
	{		
		Parent()->GetClientRect(&rc);
		offset.cx = rc.left;
		offset.cy = rc.top;
		OffsetRect(&rc,-rc.left,-rc.top);
		return tmp->DrawBkg(hDC,&rc,&offset);
	}
	return FALSE;
}
