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
//							RingSDK VC6界面模仿演示程序
//作者：临风
//
//版本：1.0
//
//说明：本程序演示使用RingSDK界面库模仿VC6界面。
//
//e-mail:ringphone@sina.com
//
**********************************************************************/
#include "global.h"

void UpdateToolbar();

BEGIN_REGMAINMSG
	REGCMD(CM_NEW,OnFileNew)
	REGCMD(CM_OPEN,OnFileOpen)
	REGCMD(CM_ABOUT,OnAbout)
	REGCMD(CM_EXIT,OnExit)
	REGCMD(CM_CASCADE,OnCascadeChild)
	REGCMD(CM_TILEH,OnTileChildH)
	REGCMD(CM_TILEV,OnTileChildV)
	REGCMD(CM_CLOSEALL,MaxChild)
	
	REGMSG(WM_CREATE)
	REGMSG(WM_RBUTTONDOWN)
	REGNOTIFY(TBN_DROPDOWN)
	REGMSG(WM_DESTROY)
END_REGMSG

int RingMain()
{	
	APP_CREATEWINDOW("VC界面演示程序","RINGSDK_VCLIKE_GUI",WS_OVERLAPPEDWINDOW|WS_CLIPCHILDREN,
		MAKEINTRESOURCE(IDM_MAIN));
	RingApp.SetIdleProc(UpdateToolbar);
	return RingApp.Run();
}

//空闲时更新工具栏
void UpdateToolbar()
{
	window->UpdateToolbar((BOOL)!(window->GetCurrChild()));
}

RINGMAINMSG(WM_CREATE)
{
	//设置程序图标
	SetIcon(LoadIcon(GetInstance(),MAKEINTRESOURCE(IDI_MAIN)));
	//创建子窗口背景画刷
	HBITMAP hbm = LoadBitmap(GetInstance(),MAKEINTRESOURCE(IDB_LINE));
	m_brush = CreatePatternBrush(hbm);
	DeleteObject(hbm);

	//初始化菜单，XP风格
	rm = InstallXPMenu();
	//设置菜单图标
	HINSTANCE m_hInst = GetInstance();
	rm->SubMenu(0)->SetItemIcon(CM_NEW,LoadIcon(m_hInst,MAKEINTRESOURCE(IDI_NEW)));
	rm->SubMenu(0)->SetItemIcon(CM_OPEN,LoadIcon(m_hInst,MAKEINTRESOURCE(IDI_OPEN)));
	rm->SubMenu(0)->SetItemIcon(CM_SAVE,LoadIcon(m_hInst,MAKEINTRESOURCE(IDI_SAVE)));
	rm->SubMenu(0)->SetItemIcon(CM_SAVEALL,LoadIcon(m_hInst,MAKEINTRESOURCE(IDI_SAVEALL)));
	rm->SubMenu(0)->SetItemIcon(IDC_PRINT,LoadIcon(m_hInst,MAKEINTRESOURCE(IDI_PRINT)));
	rm->SubMenu(0)->SetItemIcon(CM_EXIT,LoadIcon(m_hInst,MAKEINTRESOURCE(IDI_EXIT)));
	rm->SubMenu(1)->SetItemIcon(CM_UNDO,LoadIcon(m_hInst,MAKEINTRESOURCE(IDI_UNDO)));
	rm->SubMenu(1)->SetItemIcon(CM_REDO,LoadIcon(m_hInst,MAKEINTRESOURCE(IDI_REDO)));
	rm->SubMenu(1)->SetItemIcon(CM_COPY,LoadIcon(m_hInst,MAKEINTRESOURCE(IDI_COPY)));
	rm->SubMenu(1)->SetItemIcon(CM_PASTE,LoadIcon(m_hInst,MAKEINTRESOURCE(IDI_PASTE)));
	rm->SubMenu(1)->SetItemIcon(CM_CUT,LoadIcon(m_hInst,MAKEINTRESOURCE(IDI_CUT)));
	rm->SubMenu(1)->SetItemIcon(CM_DELETE,LoadIcon(m_hInst,MAKEINTRESOURCE(IDI_DEL)));
	rm->SubMenu(1)->SetItemIcon(CM_FIND,LoadIcon(m_hInst,MAKEINTRESOURCE(IDI_FIND)));
	rm->SubMenu(1)->SetItemIcon(CM_FINDINFILE,LoadIcon(m_hInst,MAKEINTRESOURCE(IDI_FINDFILE)));
	rm->SubMenu(2)->SetItemIcon(CM_CLASSID,LoadIcon(m_hInst,MAKEINTRESOURCE(IDI_ID)));
	rm->SubMenu(2)->SetItemIcon(IDC_FULLSCREEN,LoadIcon(m_hInst,MAKEINTRESOURCE(IDI_FULLSCREEN)));
	rm->SubMenu(2)->SetItemIcon(IDC_REFRESH,LoadIcon(m_hInst,MAKEINTRESOURCE(IDI_REFRESH)));
	rm->SubMenu(2)->SetItemIcon(IDC_PROP,LoadIcon(m_hInst,MAKEINTRESOURCE(IDI_PROP)));
	rm->SubMenu(5)->SetItemIcon(IDC_COMPILE,LoadIcon(m_hInst,MAKEINTRESOURCE(IDI_COMPILE)));
	rm->SubMenu(5)->SetItemIcon(CM_REBUILD,LoadIcon(m_hInst,MAKEINTRESOURCE(IDI_REBUILD)));
	rm->SubMenu(5)->SetSubMenuIcon(0,LoadIcon(m_hInst,MAKEINTRESOURCE(IDI_RUN)));
	rm->SubMenu(5)->SubMenu(0)->SetItemIcon(IDC_RUN,LoadIcon(m_hInst,MAKEINTRESOURCE(IDI_RUN)));
	rm->SubMenu(6)->SetItemIcon(IDC_1,LoadIcon(m_hInst,MAKEINTRESOURCE(IDI_TOOL)));
	rm->SubMenu(6)->SetItemIcon(IDC_2,LoadIcon(m_hInst,MAKEINTRESOURCE(IDI_TOOL)));
	rm->SubMenu(6)->SetItemIcon(IDC_3,LoadIcon(m_hInst,MAKEINTRESOURCE(IDI_TOOL)));
	rm->SubMenu(6)->SetItemIcon(IDC_4,LoadIcon(m_hInst,MAKEINTRESOURCE(IDI_TOOL)));
	rm->SubMenu(6)->SetItemIcon(IDC_5,LoadIcon(m_hInst,MAKEINTRESOURCE(IDI_TOOL)));
	rm->SubMenu(6)->SetItemIcon(IDC_6,LoadIcon(m_hInst,MAKEINTRESOURCE(IDI_TOOL)));
	rm->SubMenu(6)->SetItemIcon(IDC_7,LoadIcon(m_hInst,MAKEINTRESOURCE(IDI_TOOL)));
	rm->SubMenu(6)->SetItemIcon(IDC_MACRO,LoadIcon(m_hInst,MAKEINTRESOURCE(IDI_MACRO)));
	rm->SubMenu(7)->SetItemIcon(IDC_NEWWINDOW,LoadIcon(m_hInst,MAKEINTRESOURCE(IDI_NEWWINDOW)));
	rm->SubMenu(7)->SetItemIcon(IDC_SPLIT,LoadIcon(m_hInst,MAKEINTRESOURCE(IDI_SPLIT)));
	rm->SubMenu(7)->SetItemIcon(CM_CLOSEWINDOW,LoadIcon(m_hInst,MAKEINTRESOURCE(IDI_CLOSE)));
	rm->SubMenu(7)->SetItemIcon(IDC_NEXT,LoadIcon(m_hInst,MAKEINTRESOURCE(IDI_NEXT)));
	rm->SubMenu(7)->SetItemIcon(IDC_PREV,LoadIcon(m_hInst,MAKEINTRESOURCE(IDI_PREV)));
	rm->SubMenu(7)->SetItemIcon(CM_CASCADE,LoadIcon(m_hInst,MAKEINTRESOURCE(IDI_CASCADE)));
	rm->SubMenu(7)->SetItemIcon(CM_TILEH,LoadIcon(m_hInst,MAKEINTRESOURCE(IDI_TILEH)));
	rm->SubMenu(7)->SetItemIcon(CM_TILEV,LoadIcon(m_hInst,MAKEINTRESOURCE(IDI_TILEV)));
	rm->SubMenu(8)->SetItemIcon(IDC_SEARCH,LoadIcon(m_hInst,MAKEINTRESOURCE(IDI_SEARCH)));
	rm->SubMenu(8)->SetItemIcon(CM_ABOUT,LoadIcon(m_hInst,MAKEINTRESOURCE(IDI_HELP)));
	
	//设置工具栏显示/隐藏菜单为XP风格
	RingCoolMenu *srm = GetDockMenu();
	if(srm)
   {		
		srm->SetItemDrawFunc(FUNC_DRAWMENU_XP);//,TRUE,FUNC_DRAWMENU_XP);
		srm->SetBkgColor(0x00ffffff,0x00ff8080);
   }
	//设置背景图片，设置后以后创建的停靠窗口及工具栏均自动采用该图片
	SetDockBackGround(MAKEINTRESOURCE(IDB_BKG),TRUE);

	//加载右键快捷菜单
	rmBtn = new RingCoolMenu();
	
	if(rmBtn)
   {
		rmBtn->LoadPopup(MAKEINTRESOURCE(IDM_POP));
		rmBtn->SetItemDrawFunc(FUNC_DRAWMENU_XP,TRUE,FUNC_DRAWPARENT_XP);
		rmBtn->SetColor(0x00ffffff,0x00ff8080,MENUCOLOR_TRANSPARENT,MENUCOLOR_TRANSPARENT);
		rmBtn->SetDefaultItem(CM_TOSEL);
   }
	//生成菜单栏
	RingMenuBar *menubar = NewRingObj(itemof(RingMenuBar));
	menubar->LoadMenu(rm);
	
	//导航栏
	wizbar = NewRingObj(itemof(RingToolBar));
	wizbar->Create(IDB_TOOL,TBSTYLE_FLAT,TBSTYLE_EX_DRAWDDARROWS);
	RingCombo* combo = wizbar->NewRingObj(itemof(RingCombo));
	if(combo)
	{		
		wizbar->InsertItem(0,combo,
			WS_CHILD|CBS_AUTOHSCROLL|CBS_DROPDOWN|CBS_HASSTRINGS|WS_BORDER|WS_VSCROLL,
			1,145,160);
		combo->Disable();
	}
	combo = wizbar->NewRingObj(itemof(RingCombo));
	if(combo)
	{		
		wizbar->InsertItem(1,combo,
			WS_CHILD|CBS_AUTOHSCROLL|CBS_DROPDOWN|CBS_HASSTRINGS|WS_BORDER|WS_VSCROLL,
			2,145,160);
		combo->Disable();
	}
	combo = wizbar->NewRingObj(itemof(RingCombo));
	if(combo)
	{		
		wizbar->InsertItem(2,combo,
			WS_CHILD|CBS_AUTOHSCROLL|CBS_DROPDOWN|CBS_HASSTRINGS|WS_BORDER|WS_VSCROLL,
			3,240,160);
		combo->Disable();
	}
	wizbar->InsertButton(3,0,CM_TOSEL,NULL,TBSTYLE_DROPDOWN,
			TBSTATE_ENABLED,(DWORD)rmBtn);
	wizbar->EnableDock("导航栏");

	//标准工具栏
	toolbar = NewRingObj(itemof(RingToolBar));
	toolbar->Create(IDB_DEFTOOLBAR,TBSTYLE_FLAT);
	toolbar->InsertButton(0,0,CM_NEW);
	toolbar->InsertSep(1);
	toolbar->InsertButton(2,1,CM_OPEN);
	toolbar->InsertButton(3,2,CM_SAVE);//,NULL,TBSTYLE_BUTTON,TBSTATE_INDETERMINATE);
	toolbar->InsertButton(4,3,CM_SAVEALL);//,NULL,TBSTYLE_BUTTON,TBSTATE_INDETERMINATE);
	toolbar->InsertSep(5);
	toolbar->InsertButton(6,4,CM_CUT);//,NULL,TBSTYLE_BUTTON,TBSTATE_INDETERMINATE);
	toolbar->InsertButton(7,5,CM_COPY);//,NULL,TBSTYLE_BUTTON,TBSTATE_INDETERMINATE);
	toolbar->InsertButton(8,6,CM_PASTE);//,NULL,TBSTYLE_BUTTON,TBSTATE_INDETERMINATE);
	toolbar->InsertSep(9);
	toolbar->InsertButton(10,7,CM_UNDO);//,NULL,TBSTYLE_DROPDOWN,TBSTATE_INDETERMINATE);
	toolbar->InsertButton(11,8,CM_REDO);//,NULL,TBSTYLE_DROPDOWN,TBSTATE_INDETERMINATE);
	toolbar->InsertSep(12);
	toolbar->InsertButton(13,9,CM_VIEWWORKSPACE);//,NULL,TBSTYLE_CHECK,TBSTATE_CHECKED|TBSTATE_ENABLED);
	toolbar->InsertButton(14,10,CM_VIEWOUTPUT);//,NULL,TBSTYLE_CHECK,TBSTATE_CHECKED|TBSTATE_ENABLED);
	toolbar->InsertButton(15,11,CM_PASTE);
	toolbar->InsertSep(16);
	toolbar->InsertButton(17,12,CM_FINDINFILE);

	combo = toolbar->NewRingObj(itemof(RingCombo));
	if(combo)
	{		
		toolbar->InsertItem(18,combo,
			WS_CHILD|CBS_AUTOHSCROLL|CBS_DROPDOWN|CBS_HASSTRINGS|WS_BORDER|WS_VSCROLL,
			4,200,160);		
	}

	toolbar->InsertSep(19);
	toolbar->InsertButton(20,13,CM_FIND);
	toolbar->EnableDock("标准工具栏");

	//生成工具栏
	toolbar = NewRingObj(itemof(RingToolBar));
	toolbar->Create(IDB_BUILDBAR,TBSTYLE_FLAT);
	//toolbar->LoadToolBar(IDR_DEFTOOLBAR);
	toolbar->InsertButton(0,0,CM_COMPILE);
	toolbar->InsertButton(1,1,CM_BUILD);
	toolbar->InsertButton(2,2,CM_STOPBUILD);
	toolbar->InsertButton(3,3,CM_EXEC);
	toolbar->InsertButton(4,4,CM_RUN);
	toolbar->InsertButton(5,5,CM_BREAKPOINT);
	toolbar->EnableDock("编译工具栏");

	//侧边栏
	m_tab = NewRingObj(m_tab);
	if(m_tab)
	{
		m_tab->Create(TCS_BOTTOM|TCS_HOTTRACK,RTV_FITPARENTSIZE,
			NULL,19921,0,0,300,400);
		
		m_tab->InsertTabs("类|资源|文件");
		m_tab->SetIcon(0,m_tab->AddIcon(IDI_NEW));
		m_tab->SetIcon(1,m_tab->AddIcon(IDI_WIN));
		m_tab->SetIcon(2,m_tab->AddIcon(IDI_TEXT));
		
		HTREEITEM htiRoot,hti;
		m_classTree = m_tab->NewRingObj(m_classTree);
		if(m_classTree)
		{
			m_classTree->Create(TVS_HASBUTTONS | TVS_HASLINES | 
				TVS_LINESATROOT,RTV_FITPARENTSIZE,NULL,19923,0,0,300,260);
			m_classTree->AddIcon(IDI_PROJ);
			m_classTree->AddIcon(IDI_CLASS);
			m_classTree->AddIcon(IDI_MEMFUNC);
			m_classTree->AddIcon(IDI_MEMDATA);
			htiRoot = m_classTree->AddRoot("Project classes",NULL,0,0);
			hti = m_classTree->AddItem("Class1",htiRoot,TVI_FIRST,0,1);
			m_classTree->AddItem("Func1",hti,TVI_FIRST,0,2);
			m_classTree->AddItem("Data1",hti,TVI_LAST,0,3);
			m_classTree->Expand(htiRoot);
			m_classTree->Expand(hti);
			m_tab->SetTabToWnd(0,m_classTree);
		}
		m_resTree = m_tab->NewRingObj(m_resTree);
		if(m_resTree)
		{
			m_resTree->Create(TVS_HASBUTTONS | TVS_HASLINES | 
				TVS_LINESATROOT,RTV_FITPARENTSIZE,NULL,19924,0,0,300,260);
			m_resTree->AddIcon(IDI_PROJ);
			m_resTree->AddIcon(IDI_FOLDER);
			m_resTree->AddIcon(IDI_BMP);
			m_resTree->AddIcon(IDI_DLG);
			m_resTree->AddIcon(IDI_OPEN);
			htiRoot = m_resTree->AddRoot("Project resources",NULL,0,0);
			hti = m_resTree->AddItem("Bitmap",htiRoot,TVI_FIRST,0,1,4);
			m_resTree->AddItem("IDB_BITMAP1",hti,TVI_FIRST,0,2);
			m_resTree->Expand(hti);
			hti = m_resTree->AddItem("Dialog",htiRoot,TVI_LAST,0,1,4);
			m_resTree->AddItem("IDD_ABOUT",hti,TVI_LAST,0,3);
			m_resTree->Expand(htiRoot);
			m_resTree->Expand(hti);
			m_tab->SetTabToWnd(1,m_resTree);
		}
		m_fileTree = m_tab->NewRingObj(m_fileTree);
		if(m_fileTree)
		{
			m_fileTree->Create(TVS_HASBUTTONS | TVS_HASLINES | 
				TVS_LINESATROOT,RTV_FITPARENTSIZE,NULL,19925,0,0,300,260);
			m_fileTree->AddIcon(IDI_PROJ);
			m_fileTree->AddIcon(IDI_FOLDER);
			m_fileTree->AddIcon(IDI_CPP);
			m_fileTree->AddIcon(IDI_TEXT);
			m_fileTree->AddIcon(IDI_OPEN);
			htiRoot = m_fileTree->AddRoot("Workspace 'Project':1 project(s)",NULL,0,0);
			hti = m_fileTree->AddItem("Project Files",htiRoot,TVI_FIRST,0);
			m_fileTree->Expand(htiRoot);
			htiRoot = hti;
			hti = m_fileTree->AddItem("Source Files",htiRoot,TVI_FIRST,0,1,4);
			m_fileTree->AddItem("demo.cpp",hti,TVI_FIRST,0,2);
			m_fileTree->Expand(hti);
			hti = m_fileTree->AddItem("Header Files",htiRoot,TVI_LAST,0,1,4);
			m_fileTree->AddItem("demo.h",hti,TVI_LAST,0,3);
			m_fileTree->Expand(htiRoot);
			m_fileTree->Expand(hti);
			m_fileTree->AddItem("Resource Files",htiRoot,TVI_LAST,0,1,4);
			m_fileTree->AddItem("External Dependencies",htiRoot,TVI_LAST,0,1,4);
			m_tab->SetTabToWnd(2,m_fileTree);
		}
	}
	m_tab->EnableDockSize("工作区",160,200,TBS_FLAG_DOCKLEFT,TBS_EX_DOCKABLE|TBS_EX_CANCLOSE);

	RingTabEx* tab = NewRingObj(itemof(RingTabEx));
	if(tab)
	{
		tab->Create(TCS_BOTTOM,RTV_FITPARENTSIZE,NULL,19925,0,0,400,300);
		
		tab->InsertTabs("编译|调试|文件查找结果1|文件查找结果2|结果|数据库调试");
		m_edit = NewRingObj(itemof(RingEdit));
		if(m_edit)
		{
			m_edit->Create(ES_MULTILINE|ES_READONLY|ES_WANTRETURN|WS_VSCROLL,
				WS_EX_CLIENTEDGE|RTV_FITPARENTSIZE,NULL,20001,0,0,0,0);
			m_edit->SetBkColor(0x00FFFFFF);
			for(int i=0;i<6;i++)
				tab->SetTabToWnd(i,m_edit);
			tab->Select(0);
		}
	}
	tab->EnableDockSize("输出栏",200,160,TBS_FLAG_DOCKBOTTOM,TBS_EX_DOCKABLE|TBS_EX_CANCLOSE);

	//状态栏
	RingStatusBar* sbar = NewRingObj(itemof(RingStatusBar));
	if(sbar->Create())
	{
		sbar->SetParts(6);
		sbar->SetValue(0,"就绪",SBPS_STRETCH);
		sbar->SetValue("就绪",MAKELONG(0,SBTS_NOBORDER));
		sbar->SetValue(1,"行,列",SBPS_FITTEXT|SBPF_UPDATE);
		sbar->SetValue(2,"录制",SBPS_FITTEXT);
		sbar->SetValue(3,"COL",SBPS_FITTEXT);
		sbar->SetValue(4,"OVR",SBPS_FITTEXT);
		sbar->SetValue(5,"READ",SBPS_FITTEXT);
	}

	return DefaultProc(param);
}

RINGMAINMSG(WM_DESTROY)
{
	delete rmBtn;
	DeleteObject(m_brush);
	return Quit();
}

RINGMAINMSG(WM_RBUTTONDOWN)
{
	rm->SubMenu(0)->Popup(m_hWnd);
	return 0;
}

RINGMAINNOTIFY(TBN_DROPDOWN)
{
	LPNMTOOLBAR nmb = (LPNMTOOLBAR)param.lParam;
	if(nmb->iItem == CM_TOSEL)
	{
		RECT rc;
		
		wizbar->GetButtonRect(3,&rc);
		
		//ClientToScreen()
		MapWindowPoints(wizbar->Handle(),HWND_DESKTOP,(LPPOINT)&rc,2);
		rmBtn->Popup(m_hWnd,rc.left + LOWORD(wizbar->GetButtonSize()),rc.bottom);
	}
	return TBDDRET_DEFAULT;
}

RINGMAINMSG(WM_RINGTOOLHELP)
{
	char s[80];
	wsprintf(s,"%d\0",param.wParam);
	SetTitle(s);
	return 0;
}

RINGMAINCMD(OnFileNew)
{
	child = NewRingObj(child);
	CreateChild("new document",child,FALSE,WS_OVERLAPPEDWINDOW);
	//child->Show(SW_SHOWMAXIMIZED);
}

void RINGMAINMETHOD(UpdateToolbar)(BOOL bNoChild)
{
	RingCombo* combo = (RingCombo*)wizbar->GetContrlolItem(1);
	
	if(combo)
		if(bNoChild && combo->IsEnabled())
			combo->Disable();
		else if(!bNoChild && !combo->IsEnabled())
		{
			combo->Enable();
			combo->AddString("(Globals)");
			combo->Select(0);
		}
	combo = (RingCombo*)wizbar->GetContrlolItem(2);
	if(combo)
		if(bNoChild && combo->IsEnabled())
			combo->Disable();
		else if(!bNoChild && !combo->IsEnabled())
		{
			combo->Enable();
			combo->AddString("(All global members)");
			combo->Select(0);
		}
	combo = (RingCombo*)wizbar->GetContrlolItem(3);
	if(combo)
		if(bNoChild && combo->IsEnabled())
			combo->Disable();
		else if(!bNoChild && !combo->IsEnabled())
			combo->Enable();	

}

RINGMAINCMD(OnFileOpen)
{
}

RINGMAINCMD(OnAbout)
{
	RingDialog(IDD_ABOUT).DoModal();	
}

RINGMAINCMD(OnExit)
{
	SendExit();
}
