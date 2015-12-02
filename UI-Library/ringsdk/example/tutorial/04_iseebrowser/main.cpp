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
//		创建XP样式的菜单
//		停靠工具栏
//		创建侧边栏
//		创建文件夹列表树
//		ListView控件使用演示
//		帮助信息对话框演示
//
**********************************************************************/
#include "global.h"

//消息绑定
BEGIN_REGMAINMSG
	REGCMD(CM_OPEN,OnOpen)
	REGCMD(CM_EXIT,OnExit)
	REGCMD(CM_ABOUT,OnHelp)
	//OnCascadeChild,OnTileChildH,OnTileChildV均为父类RingMDIFrameWnd成员函数
	REGCMD(CM_CASCADE,OnCascadeChild)	
	REGCMD(CM_TILEH,OnTileChildH)
	REGCMD(CM_TILEV,OnTileChildV)
	REGNOTIFY(TVN_SELCHANGED)
	REGNOTIFY(NM_DBLCLK)
	REGMSG(WM_MENUSELECT)
	REGMSG(WM_DESTROY)	
	REGMSG(WM_CREATE)	
END_REGMSG

int RingMain()
{
	RingApp.SetAccelTable(MAKEINTRESOURCE(IDR_ACCEL));
	APP_CREATEWINDOW("Isee浏览器[演示版本]","RingSDKApp4",WS_OVERLAPPEDWINDOW,MAKEINTRESOURCE(IDR_MAINMENU));
	return RingApp.Run();
}

RINGMAINMSG(WM_CREATE)
{
	//设定支持的图象格式，目前支持以下四种格式。BMP为默认，不需要设定。
	DIBREGFORMAT(GIF);
	DIBREGFORMAT(JPG);
	DIBREGFORMAT(PNG);
	DIBREGFORMAT(PCX);

	SetIcon(LoadIcon(GetInstance(),MAKEINTRESOURCE(IDI_APP)));
	
	//设置XP样式菜单
	InstallXPMenu();

	//创建工具栏对象，NewRingObj产生的控件及MDI子窗口对象只要产生窗口（调用Create或Attach）均自动释放，
	//因此如果以后不再使用，其对象指针可以采用临时变量。
	//NewRingObj的参数请使用itemof(控件类名)
	RingToolBar *toolbar = NewRingObj(itemof(RingToolBar));
	toolbar->Create(IDB_TOOLBAR,TBSTYLE_FLAT);
	//创建按钮，参数：按钮序号，按钮图象在工具栏图象中序号，命令号
	toolbar->InsertButton(0,1,CM_OPEN);
	toolbar->InsertSep(1);
	toolbar->InsertButton(2,11,CM_ABOUT);
	toolbar->EnableDock("工具栏");
	
	//创建文件夹列表树
	m_dirtree = NewRingObj(itemof(RingDirTree));
	m_dirtree->Create(IDC_FOLDERTREE);
	m_dirtree->ListDir();
	
	//创建侧边栏
	m_dirtree->EnableDockSize("文件夹",200,400,TBS_FLAG_DOCKLEFT,TBS_EX_NONE);	

	//创建文件列表栏
	m_fileview = NewRingObj(itemof(RingListView));
	m_fileview->Create(LVS_REPORT|LVS_SINGLESEL|LVS_SHOWSELALWAYS,WS_EX_CLIENTEDGE,NULL,IDC_FILEVIEW,0,0,300,300);
	m_fileview->SetExStyle(LVS_EX_FLATSB | LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES);
	m_fileview->SetColumns("名称|大小|修改时间");
	m_fileview->SetColumnsWidth("300|180|230");
	m_fileview->EnableDockSize("文件列表",300,200,TBS_FLAG_DOCKBOTTOM,TBS_EX_NONE);
	//创建状态栏
	m_statbar = NewRingObj(itemof(RingStatusBar));
	m_statbar->Create();
	
	return DefaultProc(param);
}

RINGMAINMSG(WM_MENUSELECT)
{
	//在状态栏显示菜单帮助信息，资源字符串ID规则：菜单项资源字符串ID必须=该菜单项命令ID，
	//子菜单资源字符串ID必须=0x1100+子菜单序号（从0开始计算）
	UINT uIds[] = {MH_BASE,MH_POPUPBASE,0,0};
	MenuHelp(WM_MENUSELECT,param.wParam,param.lParam,GetMenu(),GetInstance(),m_statbar->Handle(),uIds);
	return 0;
}

RINGMAINNOTIFY(TVN_SELCHANGED)
{
	//用户选择目录的处理，先交由控件响应
	m_dirtree->OnSelectChanged(param);

	//获取选择的目录后列出图象文件
	ringDir dir(m_dirtree->GetSelFolderName());
	ringFile rf;
	ringStr str;
	char szFile[MAX_PATH];
	int index;
	HICON hicon;

	if(IsWindowPtr(m_fileview))		//检测m_fileview是否有效及已创建
		m_fileview->Clear();

	if(dir.BeginFindFile())
	{
		while(dir.FindNext(szFile))
		{
			rf.SetFile("%s\\%s",dir.Getdir(),szFile);
			str = rf.Extname();
			str.toLower();
			if(str == "bmp" || str == "jpg" || str == "jpeg" || str == "gif" || 
				str == "pcx" || str == "png")
			{				
				index = m_fileview->GetCount();
				hicon = ExtractIcon(GetInstance(),rf.Fullname(),0);
				m_fileview->AddItem(index,rf.Filename(),(LPARAM)hicon,m_fileview->AddIcon(hicon));
				m_fileview->SetItem(index,1,rf.Size());
			}
		}
	}

	return 0;
}

RINGMAINNOTIFY(NM_DBLCLK)
{
	if(param.lpnmhdr->hwndFrom == m_fileview->Handle())
	{
		char szFile[MAX_PATH];
		LPNMITEMACTIVATE lpv = (LPNMITEMACTIVATE)param.lParam;
		wsprintf(szFile,"%s\\%s\0",m_dirtree->GetSelFolderName(),m_fileview->GetText(lpv->iItem,0));
		
		HWND hChild = FindWindowEx(GetClient(),NULL,GetChildClassName(),szFile);
		if(hChild)
			ActiveChild(hChild);
		else
			CreateChild(szFile,NewRingObj(child),TRUE,WS_OVERLAPPEDWINDOW);
	}
	return 0;
}

RINGMAINMSG(WM_DESTROY)
{
	//主窗口关闭时的清理代码，本程序不需要清理，仅退出即可。
	//如果响应本消息，必须在最后调用Quit()，即PostQuitMessage,
	//否则窗口销毁，但程序仍滞留内存。
	return Quit();	
}

RINGMAINCMD(OnOpen)
{
	ringFile rf;
	if(rf.Select("图象文件|*.bmp;*.gif;*.jpg;*.jpeg;*.png;*.pcx||"))
		CreateChild(rf.Fullname(),NewRingObj(child),TRUE,WS_OVERLAPPEDWINDOW);	
}

RINGMAINCMD(OnExit)
{
	SendExit();
}

RINGMAINCMD(OnHelp)
{
	//对话框的最简单用法，如果对话框只是显示版权信息，不需要与用户交互，
	//就可以这样使用，用对话框资源ID作为参数调用复制构造函数即可。
	RingDialog(IDD_ABOUT).DoModal();
}

