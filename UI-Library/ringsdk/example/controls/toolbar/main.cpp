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
//							RingSDK程序界面类库演示程序 toolbar
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
//原文件名：main.cpp
//
//说明：	本程序演示RingToolBar的使用：
//			1、菜单栏的使用
//			2、创建原始状态（按钮）工具栏
//			3、从资源载入扁平工具栏
//			4、创建真彩+热敏效果工具栏
//			5、在工具栏上插入静态文本及控件
//
**********************************************************************/
#include "global.h"

#define CM_CONNECT			12001
#define CM_DISCONNECT		12002
#define IDC_HOST				201
#define IDC_PORT				202

BEGIN_REGMAINMSG
	REGCMD(CM_NEW,OnFileNew)
	REGCMD(CM_OPEN,OnFileOpen)
	REGCMD(CM_ABOUT,OnAbout)
	REGCMD(CM_EXIT,OnExit)
	REGCMD(CM_CASCADE,OnCascadeChild)
	REGCMD(CM_TILEH,OnTileChildH)
	REGCMD(CM_TILEV,OnTileChildV)
	REGCMD(CM_CLOSEALL,MaxChild)
	REGCMD(CM_CONNECT,OnConnect)
	REGCMD(CM_DISCONNECT,OnDisConnect)

	REGMSG(WM_CREATE)
	REGMSG(WM_DESTROY)
END_REGMSG

BEGIN_REGMSG(tooldlg)
	REGMSG(WM_INITDIALOG)
	REGMSG(WM_GETDRAGRECTS)
	REGMSG(WM_DOCKSTATECHANGE)
END_REGMSG

int RingMain()
{
	APP_CREATEWINDOW("工具栏演示程序","RINGSDK_TOOLBAR_DEMO",WS_OVERLAPPEDWINDOW,
		MAKEINTRESOURCE(IDM_MAIN));
	return RingApp.Run();
}

RINGMAINMSG(WM_CREATE)
{
	//设置程序图标
	SetIcon(LoadIcon(GetInstance(),MAKEINTRESOURCE(IDI_MAIN)));

	//初始化菜单
	rm = InstallXPMenu();
	//设置菜单图标
	rm->SubMenu(0)->SetItemIcon(CM_NEW,LoadIcon(GetInstance(),MAKEINTRESOURCE(IDI_NEW)));
	rm->SubMenu(0)->SetItemIcon(CM_OPEN,LoadIcon(GetInstance(),MAKEINTRESOURCE(IDI_OPEN)));
	rm->SubMenu(0)->SetItemIcon(CM_SAVE,LoadIcon(GetInstance(),MAKEINTRESOURCE(IDI_SAVE)));
	rm->SubMenu(0)->SetItemIcon(CM_SAVEALL,LoadIcon(GetInstance(),MAKEINTRESOURCE(IDI_SAVEALL)));
	rm->SubMenu(0)->SetItemIcon(CM_PRINT,LoadIcon(GetInstance(),MAKEINTRESOURCE(IDI_PRINT)));
	rm->SubMenu(0)->SetItemIcon(CM_EXIT,LoadIcon(GetInstance(),MAKEINTRESOURCE(IDI_EXIT)));
	rm->SubMenu(1)->SetItemIcon(CM_UNDO,LoadIcon(GetInstance(),MAKEINTRESOURCE(IDI_UNDO)));
	rm->SubMenu(1)->SetItemIcon(CM_REDO,LoadIcon(GetInstance(),MAKEINTRESOURCE(IDI_REDO)));
	rm->SubMenu(1)->SetItemIcon(CM_COPY,LoadIcon(GetInstance(),MAKEINTRESOURCE(IDI_COPY)));
	rm->SubMenu(1)->SetItemIcon(CM_PASTE,LoadIcon(GetInstance(),MAKEINTRESOURCE(IDI_PASTE)));
	rm->SubMenu(1)->SetItemIcon(CM_CUT,LoadIcon(GetInstance(),MAKEINTRESOURCE(IDI_CUT)));
	rm->SubMenu(1)->SetItemIcon(CM_DELETE,LoadIcon(GetInstance(),MAKEINTRESOURCE(IDI_DEL)));
	rm->SubMenu(3)->SetItemIcon(CM_CLOSEWINDOW,LoadIcon(GetInstance(),MAKEINTRESOURCE(IDI_CLOSE)));
	rm->SubMenu(3)->SetItemIcon(CM_NEXT,LoadIcon(GetInstance(),MAKEINTRESOURCE(IDI_NEXT)));
	rm->SubMenu(3)->SetItemIcon(CM_PREV,LoadIcon(GetInstance(),MAKEINTRESOURCE(IDI_PREV)));
	rm->SubMenu(3)->SetItemIcon(CM_CASCADE,LoadIcon(GetInstance(),MAKEINTRESOURCE(IDI_CASCADE)));
	rm->SubMenu(3)->SetItemIcon(CM_TILEH,LoadIcon(GetInstance(),MAKEINTRESOURCE(IDI_TILEH)));
	rm->SubMenu(3)->SetItemIcon(CM_TILEV,LoadIcon(GetInstance(),MAKEINTRESOURCE(IDI_TILEV)));
	rm->SubMenu(4)->SetItemIcon(CM_ABOUT,LoadIcon(GetInstance(),MAKEINTRESOURCE(IDI_HELP)));

	menubar = NewRingObj(itemof(RingMenuBar));
	menubar->LoadMenu(rm);

	SetDockBackGround(MAKEINTRESOURCE(IDB_BKG),TRUE);

	toolbar = NewRingObj(toolbar);
	toolbar->Create(IDB_TOOL);//,TBSTYLE_FLAT);
	
	toolbar->InsertButton(1,0,CM_NEW);
	toolbar->InsertButton(2,1,CM_OPEN);
	toolbar->InsertButton(3,2,3);
	toolbar->InsertButton(4,3,4);
	toolbar->InsertSep(5);
	toolbar->InsertButton(7,4,5);
	toolbar->InsertButton(8,5,6);

	toolbar = NewRingObj(itemof(RingToolBar));
	toolbar->Create(IDC_DEFTOOLBAR,TBSTYLE_FLAT);
	toolbar->LoadToolBar(IDC_DEFTOOLBAR);

	toolbar = NewRingObj(toolbar);
	toolbar->Create(IDB_COOLGRAY,TBSTYLE_FLAT);
	toolbar->InsertButton(1,0,1);
	toolbar->InsertButton(2,1,2);
	toolbar->InsertButton(3,2,3);
	toolbar->InsertButton(4,3,4);
	toolbar->InsertSep(5);
	toolbar->InsertButton(6,4,5);
	toolbar->InsertButton(7,5,6);

	toolbar->InsertSep(8);
	toolbar->InsertButton(9,12,7,0,TBSTYLE_BUTTON | TBSTYLE_CHECK);
	toolbar->InsertButton(10,13,8);
	toolbar->InsertSep(11);
	toolbar->InsertButton(12,6,9);
	toolbar->InsertButton(13,7,10);
	toolbar->InsertButton(14,8,11);
	toolbar->InsertButton(15,9,12);
	toolbar->InsertButton(16,10,13);
	toolbar->InsertButton(17,14,14);
	toolbar->InsertSep(18);
	toolbar->InsertButton(19,11,15);
	//加入热敏图象
	toolbar->AddHotImage(IDB_COOL,20,20,RGB(192,192,192));
	toolbar->EnableDock("真彩工具栏");

	//创建控件
	m_ipAddr = NewRingObj(itemof(RingIPAddr));
	m_ipAddr->Create(0,0,NULL,IDC_HOST,0,0,120,20);
	m_edPort = NewRingObj(itemof(RingEdit));
	m_edPort->Create(ES_NUMBER,WS_EX_CLIENTEDGE,NULL,IDC_PORT,0,0,40,20);
	
	toolbar = NewRingObj(toolbar);
	toolbar->Create(IDB_NET,TBSTYLE_FLAT);
	//加入文本及已创建的控件
	toolbar->InsertText(0,"服务器地址:");
	toolbar->InsertItem(1,m_ipAddr);
	toolbar->InsertText(2,"端口:");
	toolbar->InsertItem(3,m_edPort);
	toolbar->InsertButton(4,1,CM_CONNECT);
	toolbar->InsertButton(5,0,CM_DISCONNECT);
	toolbar->InsertButton(6,2,1);
	toolbar->InsertButton(7,3,2);
	toolbar->InsertButton(8,5,3,NULL,TBSTYLE_CHECK);
	toolbar->EnableDock("SOCKET工具栏");
	toolbar->EnableButton(CM_DISCONNECT,FALSE);
	
	return DefaultProc(param);
}

RINGMAINMSG(WM_DESTROY)
{
	return Quit();
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
}

RINGMAINCMD(OnFileOpen)
{
}

RINGMAINCMD(OnAbout)
{
	ShowHelpInfo("RingSDK 工具栏演示程序 v1.0\n(C)CopyRight 2005\n临风 ringphone@njwx.com","工具栏演示程序",MAKEINTRESOURCE(IDI_MAIN));
}

RINGMAINCMD(OnExit)
{
	SendExit();
}

RINGMAINCMD(OnConnect)
{
	int val = m_ipAddr->GetInt();
	MsgBox("正在连接","连接到%d.%d.%d.%d:%d\n\n程序仅演示界面，并不实际连接。",
				FIRST_IPADDRESS(val),SECOND_IPADDRESS(val),THIRD_IPADDRESS(val),
				FOURTH_IPADDRESS(val),m_edPort->GetInt());
	toolbar->EnableButton(CM_CONNECT,FALSE);
	toolbar->EnableButton(CM_DISCONNECT);
}

RINGMAINCMD(OnDisConnect)
{
	toolbar->EnableButton(CM_CONNECT);
	toolbar->EnableButton(CM_DISCONNECT,FALSE);
}

INITWINDOW(tooldlg)()
{
	SetRectEmpty(&m_vrc);
	SetRectEmpty(&m_hrc);
}

RINGMSG(tooldlg,WM_INITDIALOG)
{
	return FALSE;
}

RINGMSG(tooldlg,WM_GETDRAGRECTS)
{	
	RECT rc,rcS;
	
	if(param.wParam == TBS_FLAG_HORZ)
	{
		if(IsRectEmpty(&m_hrc))
		{
			GetWindowRect(GetDlgItem(m_hWnd,IDC_LIST1),&rc);
			GetWindowRect(GetDlgItem(m_hWnd,IDC_TEXT),&rcS);
			ScreenToClient(m_hWnd,(LPPOINT)&rcS);
			ScreenToClient(m_hWnd,(LPPOINT)&rcS+1);
			m_hrc.right = rcS.right + 20 + rc.right - rc.left;
			
			GetWindowRect(GetDlgItem(m_hWnd,IDC_ICON1),&rcS);
			ScreenToClient(m_hWnd,(LPPOINT)&rcS);
			ScreenToClient(m_hWnd,(LPPOINT)&rcS+1);

			m_hrc.bottom = max((rcS.bottom + rcS.top),(rc.bottom - rc.top + rcS.top * 2));
		}
		if(param.lParam)
			CopyRect((LPRECT)param.lParam,&m_hrc);
		return 1;
	}
	else
	{
		if(IsRectEmpty(&m_vrc))
		{
			GetWindowRect(GetDlgItem(m_hWnd,IDC_LIST1),&rc);
			
			m_vrc.right = rc.right - rc.left + 4;
			
			GetWindowRect(GetDlgItem(m_hWnd,IDC_ICON1),&rcS);
			ScreenToClient(m_hWnd,(LPPOINT)&rcS);
			ScreenToClient(m_hWnd,(LPPOINT)&rcS+1);
			
			m_vrc.bottom = rcS.bottom + rc.bottom - rc.top + rcS.top * 2;
		}
		if(param.lParam)
			CopyRect((LPRECT)param.lParam,&m_vrc);
	}
	return 1;
}

RINGMSG(tooldlg,WM_DOCKSTATECHANGE)
{
	if(param.shortval.high == param.shortval.low)
		return 0;

	RingListBox* list = GetItem(list,IDC_LIST1);
	RECT rc;
	
	if(m_param.wParam > TBS_FLAG_DOCKBOTTOM || m_param.wParam == TBS_FLAG_FLY)
	{
		GetWindowRect(GetDlgItem(m_hWnd,IDC_ICON1),&rc);
		ScreenToClient(m_hWnd,(LPPOINT)&rc);
		ScreenToClient(m_hWnd,(LPPOINT)&rc+1);

		SetPos(0,0,m_vrc.right,m_vrc.bottom);
		list->SetPos(2,rc.bottom + 10,0,0,0,SWP_NOSIZE);
	}
	else
	{
		GetWindowRect(GetDlgItem(m_hWnd,IDC_TEXT),&rc);
		ScreenToClient(m_hWnd,(LPPOINT)&rc);
		ScreenToClient(m_hWnd,(LPPOINT)&rc+1);

		SetPos(0,0,m_hrc.right,m_hrc.bottom);
		list->SetPos(rc.right + 10,rc.top,0,0,0,SWP_NOSIZE);
	}

	return 1;
}

