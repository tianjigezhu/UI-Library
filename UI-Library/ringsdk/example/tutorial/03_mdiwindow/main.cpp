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
//							RingSDK 演示程序
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
//说明：
//
//本程序演示：
//		创建MDI Frame窗口
//		创建MDI子窗口
//		打开图象文件显示
//		添加工具栏及状态栏
//		在状态栏显示菜单帮助信息
//
**********************************************************************/
#include "ringmain.h"
#include "ringdib.h"
#include "resource.h"

//MDIFrame主窗口设定
BEGIN_SETUPMAINWINDOW(IS_MDIFRAME)
	BEGIN_DEF_VAR(NO_INIT)
		RingStatusBar* m_statbar;
	END_DEF_VAR
	BEGIN_DEFMSG
		DEFMSG(WM_CREATE)
		DEFMSG(WM_MENUSELECT)
		DEFCMD(OnOpen)
		DEFCMD(OnExit)
		DEFCMD(OnHelp)
	END_DEFMAINMSG
END_SETUPMAINWINDOW

//MDI子窗口设定
BEGIN_SETUPWINDOW(child,IS_MDICHILD)
	BEGIN_DEF_VAR(NO_INIT)
		RingDIB m_dib;
	END_DEF_VAR

	BEGIN_DEFMSG
		DEFMSG(WM_CREATE)
		DEFMSG(WM_PAINT)
		DEFMSG(WM_ERASEBKGND)
		DEFMSG(WM_SIZE)		
	END_DEFMSG(child)
END_SETUPWINDOW(child)

//消息绑定
BEGIN_REGMAINMSG
	REGCMD(CM_OPEN,OnOpen)
	REGCMD(CM_EXIT,OnExit)
	REGCMD(CM_ABOUT,OnHelp)
	//OnCascadeChild,OnTileChildH,OnTileChildV均为父类RingMDIFrameWnd成员函数
	REGCMD(CM_CASCADE,OnCascadeChild)	
	REGCMD(CM_TILEH,OnTileChildH)
	REGCMD(CM_TILEV,OnTileChildV)
	REGMSG(WM_MENUSELECT)
	REGMSG(WM_CREATE)	
END_REGMSG

BEGIN_REGMSG(child)
	REGMSG(WM_PAINT)
	REGMSG(WM_ERASEBKGND)
	REGMSG(WM_SIZE)	
	REGMSG(WM_CREATE)
END_REGMSG

int RingMain()
{
	RingApp.SetAccelTable(MAKEINTRESOURCE(IDR_ACCEL));
	APP_CREATEWINDOW("RingSDK多文档演示程序","RingSDKApp3",WS_OVERLAPPEDWINDOW,MAKEINTRESOURCE(IDR_MAINMENU));
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
	
	//创建工具栏对象，NewRingObj产生的控件及MDI子窗口对象只要产生窗口（调用Create或Attach）均自动释放，
	//因此如果以后不再使用，其对象指针可以采用临时变量。
	//NewRingObj的参数请使用itemof(控件类名)
	RingToolBar *toolbar = NewRingObj(itemof(RingToolBar));
	toolbar->Create(IDB_TOOLBAR);
	//创建按钮，参数：按钮序号，按钮图象在工具栏图象中序号，命令号
	toolbar->InsertSep(0);
	toolbar->InsertButton(1,1,CM_OPEN);
	toolbar->InsertSep(2);
	toolbar->InsertButton(3,11,CM_ABOUT);
	//toolbar->EnableDock("",TBS_EX_NONE|TBS_EX_ALLLINE,TBS_FLAG_DOCKLEFT);
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

RINGMAINCMD(OnOpen)
{
	ringFile rf;
	if(rf.Select("图象文件|*.bmp;*.gif;*.jpg;*.jpeg;*.png;*.pcx||"))
	{
		HWND hChild = FindWindowEx(GetClient(),NULL,GetChildClassName(),rf.Fullname());
		if(hChild)
			ActiveChild(hChild);
		else
			CreateChild(rf.Fullname(),NewRingObj(child),TRUE,WS_OVERLAPPEDWINDOW);
	}
}

RINGMAINCMD(OnExit)
{
	SendExit();
}

RINGMAINCMD(OnHelp)
{
	ShellAbout(m_hWnd,"RingSDK多文档演示程序#RingSDK多文档演示程序","",
					LoadIcon(GetInstance(),MAKEINTRESOURCE(IDI_APP)));
}

////////////MDI子窗口代码////////////////////////
RINGMSG(child,WM_CREATE)
{
	char szPic[MAX_PATH];
	GetWindowText(m_hWnd,szPic,MAX_PATH);
	m_dib.Load(szPic);
	m_dib.SetTarget(m_hWnd);
	
	SetIcon(LoadIcon(GetInstance(),MAKEINTRESOURCE(IDI_APP)));
	return DefaultProc(param);
}

RINGMSG(child,WM_SIZE)
{
	InvalidateRect(m_hWnd,NULL,FALSE);
	return DefaultProc(param);
}

RINGMSG(child,WM_ERASEBKGND)
{
	return TRUE;
}

RINGMSG(child,WM_PAINT)
{
	int w,h;
	RECT rc,rcImage;
	PAINTSTRUCT ps;
	
	GetClientRect(&rc);
	//计算图象显示宽高
	if(rc.right < m_dib.Width())
	{
		//拉伸显示
		h = rc.right*m_dib.Height()/m_dib.Width();
		if(h > rc.bottom)
		{
			h = rc.bottom;
			w = h*m_dib.Width()/m_dib.Height();
		}
		else
			w = rc.right;
	}
	else if(rc.bottom < m_dib.Height())
	{
		//拉伸显示
		w = rc.bottom*m_dib.Width()/m_dib.Height();
		if(w > rc.right)
		{
			w = rc.right;
			h = w*m_dib.Height()/m_dib.Width();
		}
		else
			h = rc.bottom;
	}
	else
	{
		w = m_dib.Width();
		h = m_dib.Height();
	}
	rcImage.left = (rc.right - w)/2;
	rcImage.right = rcImage.left + w;
	rcImage.top = (rc.bottom - h)/2;
	rcImage.bottom = rcImage.top + h;

	//绘制图象
	BeginPaint(m_hWnd,&ps);
	m_dib.Draw(ps.hdc,rcImage.left,rcImage.top,0,0,w,h);
	//排除图象区域绘制背景，这样不会产生闪烁
	ExcludeClipRect(ps.hdc,rcImage.left,rcImage.top,rcImage.right,rcImage.bottom);
	FillRect(ps.hdc,&rc,(HBRUSH)GetStockObject(BLACK_BRUSH));
	EndPaint(m_hWnd,&ps);
	return 0;
}
