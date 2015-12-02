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
//							RingSDK程序界面类库 ringdows.lib
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
//原文件名：ringdowbase.h
//
//说明：窗口基类定义，所有窗口类均派生自这些类。
//
**********************************************************************/
#ifndef _INC_RINGDOWBASE
#define _INC_RINGDOWBASE
#include "ringlib.h"
#include "ringmenu.h"

//窗口类型
enum{
ITIS_PROPSHEET = 12298,		//属性页对话框
ITIS_PROPPAGE = 12299,			//属性页
ITIS_DIALOG	= 12300,				//主窗口是DialogBox
//以上三个定义必须按现有次序紧密排列（RingBaseWnd::stdProc）
ITIS_WINDOW	= 12301,				//主窗口是Window
ITIS_MDIFRAME = 12302,
ITIS_MDICHILD = 12303,
ITIS_TOOLBAR = 12305,
ITIS_COOLBAR = 12306,
ITIS_MENUBAR = 12307,
ITIS_FLYBAR	= 12308,
ITIS_STATUSBAR = 12310,
ITIS_DOCKSITE = 12313,
ITIS_EDIT =	12320,
ITIS_COMBO =	12321,
ITIS_LISTBOX =	12322,
ITIS_BUTTON	= 12323,
ITIS_STATIC = 12324,
ITIS_TAB = 12325,
ITIS_SCROLLBAR = 12326,
ITIS_LISTVIEW = 12327,

ITIS_DLGITEM = 12400,
ITIS_MAINWINDOW =	20000
};

//停靠工具栏状态/类型定义（警告：以下定义值不可修改，否则须修改代码）
#define TBS_EX_DOCKABLE			0x0000000F		//允许改变位置（可以拖动）
#define TBS_EX_NONE				0x00000020     //不允许拖动(SizeBar可调整尺寸)
#define TBS_EX_DOCKTOP			0x00000001
#define TBS_EX_DOCKBOTTOM		0x00000002
#define TBS_EX_DOCKHORZ			0x00000003
#define TBS_EX_DOCKLEFT			0x00000004
#define TBS_EX_DOCKRIGHT		0x00000008
#define TBS_EX_DOCKVERT			0x0000000C
#define TBS_EX_CANCLOSE			0x00010000
#define TBS_EX_ALLLINE			0x00100000		//独占一行

#define TBS_FLAG_NONE			22000
#define TBS_FLAG_FLYING			22001
#define TBS_FLAG_FLOATTING		22002

#define TBS_FLAG_FLY				(0xFFFF)
#define TBS_FLAG_DOCKTOP		0
#define TBS_FLAG_DOCKBOTTOM	1
#define TBS_FLAG_DOCKLEFT		2
#define TBS_FLAG_DOCKRIGHT		3

#define TBS_FLAG_MASK			2
#define TBS_FLAG_HORZ			0
#define TBS_FLAG_VERT			2
//警告结束

class RingOptionItem;

//自定义窗口消息
#define MYWM_BASEEX				0x3000
#define MYWM_BASE					0x2000
#define MYWM_NOTIFYBASE			0x4000
#define MYWM_SIZE					(WM_SIZE + MYWM_BASEEX)
#define MYWM_ACTIVATE			(WM_ACTIVATE + MYWM_BASEEX)
#define MYWM_ENABLE				(WM_ENABLE + MYWM_BASEEX)
#define WM_RINGTOOLTIP			(WM_NOTIFY + MYWM_NOTIFYBASE)
#define WM_RINGTOOLHELP			(WM_NOTIFY + MYWM_NOTIFYBASE + 1)
//以下为增加的自定义窗口消息
//得到窗口对象指针
#define MYWM_GETCLASS			(MYWM_BASE + 1)
//MDI的MDICLIENT窗口关闭消息
#define MYWM_MDICLIENTCLOSE	(MYWM_BASE + 2)
//超链接鼠标移动上去和离开的消息
#define WM_LINKHOVER				(MYWM_BASE + 3)
#define WM_LINKLEAVE				(MYWM_BASE + 4)
//按键消息，按下键后到抬起发送该消息，参数同WM_KEYUP
#define WM_KEYPRESS				(MYWM_BASE + 5)
//停靠窗口消息
//查询得到三个拖动矩形,参数：wParam:指向三个矩形的指针，
//依次为横向，竖向，浮动的RECT，lParam参数无用
#define WM_GETDRAGRECTS			(MYWM_BASE + 6)
//窗口确定停靠位置后，显示前发送该消息，
//wParam为停靠位置标志(上下左右或浮动)
//程序根据停靠位置标志决定如何显示（竖向或横向）
//lParam为子窗口(停靠窗口)句柄。
//该消息先发送到主窗口（停靠窗口的父窗口），如主窗口未处理（返回0），
//则发送到要停靠的窗口，如该窗口未处理，则默认为窗口不管停靠
//在什么位置，均采用该窗口创建时的尺寸
#define WM_DOCKSTATECHANGE		(MYWM_BASE + 7)
//DockSite尺寸变动
#define WM_DOCKSITESIZECHANGE	(MYWM_BASE + 8)
//DockSite消息，
#define RD_ADDBAR					(MYWM_BASE + 9)
#define RD_DELETEBAR				(MYWM_BASE + 10)
#define RD_MOVEBAR				(MYWM_BASE + 11)
//header消息
#define HDM_ENABLECOLUMNHIDE	(MYWM_BASE + 12)
#define HDM_SHOWCOLUMN			(MYWM_BASE + 13)
//通知消息
#define HDN_SHOWCOLUMN			(MYWM_BASE + 13)
#define LVN_SHOWCOLUMN			HDN_SHOWCOLUMN
//RingComm串口事件通知消息(该消息在Ringcomm.h中有同样定义)
#ifndef WM_COMMEVENT
#define WM_COMMEVENT				(MYWM_BASE + 101)
#endif

#define RDERR_OK					(1)
#define RDERR_FAIL				(2)
//标记窗口消息未处理
#define RM_NOTPROCESS			(-9999)

/********************************************************
//
//菜单帮助ID，用于在状态栏显示帮助，
//程序资源字符串应该是如下格式：
//MH_BASE + 菜单项ID "菜单项说明"
//MH_POPUPBASE + n "子菜单说明，n为从0开始的连续数"
//子菜单说明是指当展开菜单上的一个子菜单，鼠标移出菜单时的帮助信息
//n指定了是第n个子菜单
//MH_POPUPBASE的值不可修改，该值为主窗口菜单的第一个子菜单的句柄值
//
********************************************************/
#define MH_BASE					0
#define MH_POPUPBASE				0x1100

//新增通知消息结构(OptionView控件)
typedef struct _PROPVNOTIFY
{ 
	NMHDR hdr;
	LPCTSTR lpszName,lpszText,lpszOrgText;
	LONG lValue,lData;
	UINT uType;
	RingOptionItem* item; 
}ROPVNOTIFY,*LPROPVNOTIFY;

//新增停靠窗口结构
typedef struct _RINGDOCKBARINFO
{ 
	UINT cbSize;
	UINT uDockStyle;
	UINT uDockFlag;
	HWND hChildWnd;	
}RINGDOCKBARINFO,*LPRINGDOCKBARINFO;

//窗口信息
typedef struct tagRindowInfo
{
   HCURSOR	hCursor;			//光标，如果程序需要设置光标，则创建主窗口时必须把系统光标设为NULL，
   HICON		hIconBig;		//否则光标一移动，系统会恢复原有光标
   HICON		hIconSmall;
   LPCTSTR	lpszMenu;
}RINDOWINFO,*LPRINDOWINFO;

typedef struct tagRingParams
{	
	UINT uMsg;				//窗口消息
	union
	{
		struct 
		{
			WORD low;		//wParam
			WORD high;		//wParam
		}shortval;			//wParam
		HDC hdc;				//wParam
		HRGN hrgn;			//WM_NCPAINT,etc.
		WPARAM wParam;
	};
	union
	{
		LPCREATESTRUCT lpcreate;				//WM_CREATE(lParam)
		LPNMHDR lpnmhdr;							//WM_NOTIFY(lParam)
		LPROPVNOTIFY lpropvnm;					//for OptionView NOTIFY(lParam)
		LPWINDOWPOS lpwindowpos;				//WM_WINDOWPOSXXX(lParam)
		LPDRAWITEMSTRUCT lpdrawitem;			//WM_DRAWITEM(lParam)
		LPMEASUREITEMSTRUCT lpmeasureitem;	//WM_MEASUREITEM(lParam)
		LPNCCALCSIZE_PARAMS lpncsp;			//WM_NCCALCSIZE(lParam)
		LPDELETEITEMSTRUCT lpdeleteitem;		//WM_DELETEITEM(lParam)
		LPNMTREEVIEW lpnmtreeview;				//WM_NOTIFY(lParam-treeview)
		LPNMLISTVIEW lpnmlistview;				//WM_NOTIFY(lParam-listview)
		POINTS mousept;							//鼠标消息(lParam)		
		HANDLE handle;								//(lParam)
		DWORD dwval;								//(lParam)
		int intval;									//(lParam)
		LPARAM lParam;
	};
}RINGPARAMS,*LPRINGPARAMS;
/*
template <class T>
inline void (T::*v_Sig(void (T::*pmf)()))()
{
	return pmf;
}
*/
template <class T>
struct RingdowMsg
{
	typedef LRESULT (T::*MSG_FUNC1)(HWND,RINGPARAMS&);
	typedef void (T::*CMD_FUNC1)(UINT);
   UINT uMsg;
	UINT uId;
   MSG_FUNC1 msgFunc;
	CMD_FUNC1 cmdFunc;
};

//RINGVCLASS类作用为转换各类的 RingdowMsg 和成员函数的指针类型，
//如该类仅声明，无任何定义和实现，在BC运行正常，VC必须有实现
/**********************************************************
//该段代码已移至ringlib.h,RingThread需要用到
#ifndef __BORLANDC__
class RINGVCLASS
{
public:
	RINGVCLASS(){}
	~RINGVCLASS(){}
};
#else
class RINGVCLASS;
#endif
**********************************************************/
typedef RingdowMsg<RINGVCLASS> RingMsgs;
typedef LRESULT (RINGVCLASS::*MSG_FUNC1)(HWND,RINGPARAMS&);
typedef void (RINGVCLASS::*CMD_FUNC1)(UINT);

typedef void (*IDLEPROC)(void);

class RingBaseWnd;
class RingControls;
class RingDialog;
class RingOFNHookDialog;
class RingWnd;
class RingMDIClient;
class RingMDIChildWnd;
class RingDockBar;
class RingSizeBar;
class RingFlyBar;
class RingDockSite;
class RingToolBar;
class RingCoolBar;
class RingMenuBar;
class RingStatusBar;
class RingdowApp;
class RingCtrlWnd;
class RingCoolMenu;
class RingEdit;
class RingCombo;
class RingListBox;
class RingPropSheet;
class RingPropPage;
class RingTab;

//控件自绘辅助类
class RingOwnerDrawer;

extern "C"
{
   int RingMain(void);
   void InitRingControls(HWND hWnd);
   //ATOM WINAPI RegRingdowClass(LPWNDCLASSEX lpwc);
	BOOL WINAPI RegRingdowClass(LPWNDCLASSEX lpwc);
   void WINAPI DrawDragRect(HDC hDC,LPRECT pRect,int nWidth);
	void WINAPI DrawLineRect(HDC hDC,LPRECT pRect);	
	RingBaseWnd* GetWindowPtr(HWND);
	BOOL IsWindowPtr(RingBaseWnd* obj);
	char * DecodeMessage(UINT msg);
	//for dockbar only
	BOOL IsVert(int state);
	BOOL IsHorz(int state);
	BOOL IsFloat(int state);
	//初始化扩展控件
	void InitIPControl();
	void InitBarControl();
	void InitCommonCtl32(DWORD dwICC);
}

//宏定义，方便编程
#define BEGIN_SETUPWINDOW(id,baseclass)	\
class RingWnd_##id:public baseclass\
{\
public:\
	RingWnd_##id(){m_msgs = (RingMsgs*)RingSDK_winmsg;InitRingSDKWindow();}

//	RingWnd_##id(RingBaseWnd* parent){SetParentObj(parent);m_msgs = (RingMsgs*)RingSDK_winmsg;InitRingSDKWindow();}\
	
#define BEGIN_DEFMSG
#define END_DEFMSG(id)	\
private:\
   typedef RingdowMsg<RingWnd_##id>::MSG_FUNC1   MyRingMsgFunc;\
	typedef RingdowMsg<RingWnd_##id>::CMD_FUNC1   MyRingCmdFunc;\
   typedef RingWnd_##id		                      RingSDKClass;\
	static RingdowMsg<RingWnd_##id> RingSDK_winmsg[];

#define END_SETUPWINDOW(id)	};\
extern RingWnd_##id* id;

#define BEGIN_REGMSG(id) \
RingWnd_##id* id = NULL;\
RingdowMsg<RingWnd_##id> RingWnd_##id::RingSDK_winmsg[] = {

#define REGMSG(wmsg)	{wmsg,0,(MyRingMsgFunc)&RingSDKClass::On_##wmsg,0},
#define REGMSGEX(wmsg,func)	{wmsg,0,(MyRingMsgFunc)&RingSDKClass::func,0},
#define REGCMD(id,func)	{0,id,(MyRingMsgFunc)&RingWnd::DispCommand,(MyRingCmdFunc)&RingSDKClass::func},
#define REGCMDRANGE(idlow,idhigh,func)	{idhigh,idlow,(MyRingMsgFunc)&RingWnd::DispCommand,(MyRingCmdFunc)&RingSDKClass::func},
#define REGNOTIFY(id)	{0,id,(MyRingMsgFunc)&RingSDKClass::On_##id,0},
#define END_REGMSG	{0,0,0,0}};

#define DEFMSG(msgname) 	LRESULT On_##msgname(HWND,RINGPARAMS&);
#define DEFMSGEX(msgname) 	LRESULT msgname(HWND,RINGPARAMS&);
#define DEFCMD(funcname) 	void funcname(UINT);
#define DEFNOTIFY(notify)		LRESULT On_##notify(HWND,RINGPARAMS&);
#define RINGMSG(id,msgname)	LRESULT RingWnd_##id::On_##msgname(HWND hWnd,RINGPARAMS& param)
#define RINGCMD(id,cmdfunc)	void RingWnd_##id::cmdfunc(UINT event)
#define RINGMSGEX(id,msgname)	LRESULT RingWnd_##id::msgname(HWND hWnd,RINGPARAMS& param)
#define RINGNOTIFY(id,notify)		LRESULT RingWnd_##id::On_##notify(HWND hWnd,RINGPARAMS& param)
#define RINGCLASS(id)	RingWnd_##id
#define RINGCLASSPTR(id,ptr)	((RingWnd_##id*)ptr)
#define RINGMETHOD(id,func) RingWnd_##id::##func

#define NEED_INIT		void InitRingSDKWindow();
#define NO_INIT      void InitRingSDKWindow(){}

#define BEGIN_DEF_VAR(setup)	setup\
	
#define END_DEF_VAR

#define INITWINDOW(id) void RingWnd_##id::InitRingSDKWindow

#define BEGIN_SETUPDIALOG(id)	BEGIN_SETUPWINDOW(id,IS_DIALOG)

#define END_SETUPDIALOG(id)	END_SETUPWINDOW(id)

#define BINDID(id)	void SetId(UINT uId=0){m_id = id;}
#define RINGDOWOBJ(id) RingWnd_##id()

#define itemof(objtype) ((objtype*)0)
#define windowof(obj) ((RingWnd_##obj*)0)

/**********************************************************
//
//	所有窗口类的基类
//
**********************************************************/
class RingBaseWnd
{
public:
	RingBaseWnd();
   virtual ~RingBaseWnd();
	
   //内部数据引出
   RingBaseWnd* Parent() const{return m_parent;}
   HWND Handle() const{return this==NULL?NULL:m_hWnd;}
	operator HWND() const{return Handle();}

	HMENU GetMenu(){return ::GetMenu(m_hWnd);}
	int GetWindowType(){return m_windowType;}
	HWND GetParent(){return ::GetParent(m_hWnd);}

	int Width(){return WinWidth(m_hWnd);}
	int Height(){return WinHeight(m_hWnd);}
	int ClientWidth();
	int ClientHeight();
	//得到窗口坐标（相对于父窗口，无父窗口则为屏幕坐标）
	BOOL GetRect(LPRECT lprc);

	HWND GetChild(){return GetWindow(m_hWnd,GW_CHILD);}
	HWND GetChild(HWND hwnd,UINT uFlag){return GetWindow(hwnd,uFlag);}
	HWND GetNextChild(HWND hwnd){return GetWindow(hwnd,GW_HWNDNEXT);}
	
	virtual DWORD GetStyle(){return GetWindowLong(m_hWnd,GWL_STYLE);}
	virtual DWORD GetExStyle(){return GetWindowLong(m_hWnd,GWL_EXSTYLE);}

	virtual BOOL GetClientRect(LPRECT lprc){return ::GetClientRect(m_hWnd,lprc);}
	COLORREF GetBkColor(HDC hDC = NULL);

	//属性设置
	void SetTitle(LPCTSTR szTitle);
	HWND SetParent(HWND hParentWnd);
   void SetParentObj(RingBaseWnd* parent);
	BOOL SetParentObj(HWND hParentWnd);
	virtual BOOL SetPos(int x,int y,int width=-999,int height=-999,HWND hPos = NULL,UINT uFlags = SWP_NOZORDER);
   virtual void SetCursor(HCURSOR hCur = NULL);
	//设置背景色（窗口存活期一直有效）
	virtual HBRUSH SetBkgColor(COLORREF crColor);
	virtual HBRUSH SetBkgBitmap(HBITMAP hbm);
	virtual HBRUSH SetBkgBitmap(UINT uId,HINSTANCE hInst = GetInstance());
	//设置退出时是否销毁自身对象
	void SetAutoDelete(BOOL bFlag = TRUE){m_bKillOnExit = bFlag;}

	virtual void SetExStyle(DWORD dwExStyle);
   virtual void SetStyle(DWORD dwStyle);
	virtual void ReplaceStyle(DWORD dwsRemove,DWORD dwsReplace);
	
	//RingOwnerDrawer* SetItemOwnerDraw(RingOwnerDrawer* drawer);

	HWND SetFocus(){return ::SetFocus(m_hWnd);}

	//操作功能
	LRESULT SendMessage(HWND hWnd,RINGPARAMS& param)
	{return ::SendMessage(hWnd,param.uMsg,param.wParam,param.lParam);}
	//因为上面的函数关系，调用全局同名函数会报找不到函数
	//的错误，因此设置该函数
	LRESULT SendMessage(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
	{return ::SendMessage(hWnd,uMsg,wParam,lParam);}
	
	BOOL CenterWindow(BOOL bWithParent = TRUE);
   virtual BOOL Show(int nCmdShow = SW_SHOW){return ::ShowWindow(m_hWnd,nCmdShow);}
	BOOL Enable(){return EnableWindow(m_hWnd,TRUE);}
	BOOL Disable(){return EnableWindow(m_hWnd,FALSE);}
	int Quit(){PostQuitMessage(0);return 0;}
	virtual int Close(int nResult=0){return DestroyWindow(m_hWnd);}

	virtual BOOL Attach(HWND);
	BOOL Detach();
	virtual WNDPROC SubClassWindow(HWND hWnd);

	virtual RingControls* InsertItem(RingControls* item,DWORD dwStyle=0,int left=0,int top=0,int width=0,int height=0);
		
	//重载 new 的目的：如果用 new 产生对象，窗口关闭时视需要自动销毁。
	//控件类默认自动销毁，应用程序窗口类（即非控件类）窗口关闭时不自动释放，
	//该行为可通过SetAutoDelete设置
	void* operator new(size_t size);

	//停靠操作，LPRECT坐标为相对父窗口
	virtual RingDockBar* EnableDock(LPCTSTR szTitle,UINT uDockStyle = TBS_EX_DOCKABLE,
		UINT uDockFlag = TBS_FLAG_DOCKTOP,LPRECT lprcPos = NULL);
	virtual RingDockBar* EnableDockSize(LPCTSTR szTitle,int nWidth,int nHeight,
			UINT uDockFlag = TBS_FLAG_DOCKLEFT,UINT uDockStyle = TBS_EX_DOCKABLE);
	virtual BOOL DockTo(UINT uDockFlag,LPRECT lprcPos = NULL);
	virtual void DisableDock();
	//浮动与停靠状态切换
	int DockBack();
	RingDockBar* GetDockBar();
	int GetDockState();

	//弹出子窗口
	BOOL Popup(RingBaseWnd* obj,LPCTSTR lpDlgOrTitle = NULL,LPARAM lParamOrClass = 0,DWORD dwStyle = WS_POPUP,DWORD dwExStyle = 0);
	//创建子窗口
	//int CreateChild(RingBaseWnd* child,LPCTSTR lpTemplate=NULL,LPARAM dwInitParam = 0);
	
	//未处理的WM_NOTIFY消息的默认处理，本函数应该由控件类继承，未处理消息必须返回-1
	virtual LRESULT OnDefNotify(RingBaseWnd* w,RINGPARAMS& param){return -1;}
	//窗口停靠时，移动位置响应，不同控件可能需要不同参数才不闪烁,
	//uAppendFlag：默认参数的附加参数
	virtual HDWP DefDeferWindowPos(HDWP,int,int,int,int,UINT uAppendFlag=0);

	//刷新，本函数为停靠而设，父窗口改变尺寸时DockBar会调用，此情况下不同控件或窗口需要有不同表现，
	//因此手工调用请弄清楚具体控件或窗口的具体表现
	//virtual void Repaint(){InvalidateRect(m_hWnd,NULL,TRUE);}

	template<typename tname>
	tname* NewRingObj(tname* obj,BOOL bAutoDelete=FALSE)
	{
		try
		{
			tname* tmp = new tname;
			if(tmp)
			{
				tmp->SetParentObj(this);
				if(bAutoDelete)
					tmp->SetAutoDelete();
			}
			return tmp;
		}
		catch(...)
		{
			return NULL;
		}		
	}

	template<typename tname>
	tname* GetItem(tname* obj,UINT id)
	{
		try
		{
			HWND hwnd = GetDlgItem(m_hWnd,id);
			if(hwnd)
			{
				RingBaseWnd* temp = GetWindowPtr(hwnd);
				tname* tmp = NULL;
				if(temp)	//已有控制对象
				{
					//bug:tmp = (tname*)temp;强制类型转换有可能
					//会返回向下映射的指针，
					//如果用dynamic_cast<tname*>(temp)则需要把
					//工程设置为“允许RTTI”，VC默认关闭，且打开
					//该选项影响效率，因此调用该函数必须注意不要
					//进行向下映射。
					if(((RingControls*)temp)->TypeMatched(hwnd))
						tmp = (tname*)temp;

					return tmp;
				}
				else
				{
					tmp = new tname;
				
					if(tmp->TypeMatched(hwnd))
					{
						tmp->SetParentObj(this);
						//类型对
						tmp->Attach(id);
						return tmp;
					}
					else
					{
						delete tmp;
						return NULL;
					}
				}
			}
		}
		catch(...)
		{
		}
		return NULL;
	}

	template<typename tname>
	BOOL AssignItem(tname& obj,UINT id)
	{
		try
		{
			HWND hwnd = GetDlgItem(m_hWnd,id);
			if(hwnd)
			{
				RingBaseWnd* temp = GetWindowPtr(hwnd);
				
				if(temp == NULL)	//尚未Attach
					if(obj.TypeMatched(hwnd))
					{
						obj.Detach();
						obj.SetParentObj(this);
						return obj.Attach(hwnd);
					}				
			}
		}
		catch(...)
		{
		}
		return FALSE;
	}

	//判定功能	
	virtual BOOL IsControls(){return FALSE;}
	virtual BOOL IsFocused(){return m_hWnd == GetFocus();}

	//窗口函数（WINAPI中第一个参数为HWND的函数）
	//说明：以下并未包括所有的WINAPI函数,以后会逐步添加	
	BOOL IsVisible(){return IsWindowVisible(m_hWnd);}
	BOOL IsWindow(){return ::IsWindow(m_hWnd);}
	BOOL IsEnabled(){return IsWindowEnabled(m_hWnd);}
	BOOL UpdateWindow(){return ::UpdateWindow(m_hWnd);}

protected:
	//窗口消息处理
	static LRESULT CALLBACK WINAPI InitWndProc(HWND hWnd,UINT uMsg,UINT wParam,LONG lParam);
	
	virtual LRESULT RingdowProc(HWND,RINGPARAMS);
   virtual LRESULT DefaultProc(RINGPARAMS&) = 0;
	LRESULT CtlColorProc(HWND hWnd,RINGPARAMS& param);
	virtual LRESULT OnBkColor(UINT uMsg,HDC hDC){return RM_NOTPROCESS;}

	//转发消息到子窗口
	virtual void NotifyChild(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam);
	BOOL NotifyChildDrawItem(HWND hParentWnd,HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam);

	virtual BOOL OnParentNotify(HWND,UINT,UINT,LONG){return FALSE;}
	void OnNcDestroy();
	
	WNDPROC GetThunk() const{return m_thunk;}
	WNDPROC InitThunk();

   virtual HWND CreateEx(LPCTSTR szTitle,
   						 LPCTSTR szClass,
                      DWORD dwStyle,
                      DWORD dwExStyle = 0,
							 HWND hParentWnd = NULL,
                      HMENU hMenu = NULL,
   						 int x = CW_USEDEFAULT,
                      int y = CW_USEDEFAULT,
                      int width = CW_USEDEFAULT,
                      int height = CW_USEDEFAULT,
							 LPARAM lParam=0);
	
   friend RingCoolBar;
	friend RingFlyBar;
	friend RingDockBar;
	friend RingSizeBar;
	friend RingWnd;
	friend RingTab;
	
protected:	
   HWND m_hWnd;
	WNDPROC m_DefProc;
   RingBaseWnd* m_parent;
	RINGPARAMS m_param;
   int m_windowType;	//窗口类型
	HBRUSH m_brush;

   friend RingdowApp;
	friend RingDialog;
	friend RingOFNHookDialog;

private:
	static LRESULT CALLBACK WINAPI stdProc(HWND hWnd,UINT uMsg,UINT wParam,LONG lParam);

private:
	WNDPROC m_thunk;
	DWORD bNeedDestroy;
	BOOL m_bKillOnExit;
	//WM_KEYPRESS消息用
	//BOOL m_bKeyDown;

	friend RingCtrlWnd;
};

typedef struct tagRingWndParam
{
	RingWnd* WndObj;
   LPARAM dwInitParam;
}RINGWNDPARAM,*LPRINGWNDPARAM;

#define CLIRECT_WITHSCROLL	TRUE
#define CLIRECT_NOSCROLL	FALSE

class RingWnd:public RingBaseWnd
{
public:
	RingWnd();
   virtual ~RingWnd()
	{
		UnInstallCoolMenu();
		Del(m_dockSite);
	}

	//内部数据引出
	BOOL GetClientRect(LPRECT lprc);//,BOOL bWithScrollSize=CLIRECT_WITHSCROLL);
	
	//属性设置	
	virtual HICON SetIcon(HICON hIconBig,HICON hIconSmall = NULL);
   BOOL SetMenu(LPCTSTR lpszMenu,HINSTANCE hInst=GetInstance());
   BOOL SetMenu(HMENU hMenu);
	void SetCursor(HCURSOR hCur = NULL);

	//操作功能
	virtual HWND Create(LPCTSTR szTitle,LPCTSTR szClass,
								DWORD dwStyle,DWORD dwExStyle = 0,
								int x=CW_USEDEFAULT,int y=CW_USEDEFAULT,
								int width=CW_USEDEFAULT,int height=CW_USEDEFAULT,
								HWND hParentWnd=NULL,HMENU hMenu=NULL,LPARAM lParam=0);
	//virtual int Exit(int nResult=0){return DestroyWindow(m_hWnd);}
	virtual void SendExit(){SendMessage(m_hWnd,WM_CLOSE,0,0);}
	
	//滚动条操作
	int GetScrollPos(int nBar){return ::GetScrollPos(m_hWnd,nBar);}
	int SetScrollPos(int nBar,int nPos,BOOL bRedraw=TRUE)
	{
		return ::SetScrollPos(m_hWnd,nBar,nPos,bRedraw);
	}
	int SetScrollPosHV(int nPosX=-1,int nPosY=-1,BOOL bRedraw=TRUE);
	BOOL GetScrollRange(int nBar,LPSIZE lpSize);
	BOOL SetScrollRange(int nBar,int nPosMin,int nPosMax,BOOL bRedraw=TRUE)
	{
		return ::SetScrollRange(m_hWnd,nBar,nPosMin,nPosMax,bRedraw);
	}
	int SetScrollRangePage(int nBar,int nMin,int nMax,int nPage,BOOL bRedraw=TRUE);
	BOOL GetScrollInfo(int nBar,LPSCROLLINFO lpsi);
	int SetScrollInfo(int nBar,int nMask,int nMin,int nMax,int nPage,int nPos,
							int nTrackPos,BOOL bRedraw=TRUE);
	BOOL SetScrollInfo(int nBar,LPSCROLLINFO lpsi,BOOL bRedraw=TRUE);	
	int GetScrollLimit(int nBar);
	BOOL EnableScrollBar(int nBar,UINT wArrows)
	{
		return ::EnableScrollBar(m_hWnd,nBar,wArrows);
	}	

   RingCoolMenu* LoadCoolMenu(LPCTSTR lpszMenu,BOOL bPopup=TRUE,HINSTANCE hInst=GetInstance());
	RingCoolMenu* InstallCoolMenu(LPCTSTR lpszMenu=NULL,BOOL bPopup=TRUE,HINSTANCE hInst=GetInstance());
	RingCoolMenu* InstallXPMenu(LPCTSTR lpszMenu=NULL,BOOL bPopup=TRUE,HINSTANCE hInst=GetInstance());
	RingCoolMenu* GetCoolMenu(){return m_pInterMenu;}
	RingCoolMenu* GetDockMenu();
   void UnInstallCoolMenu()
   {
		delete m_pInterMenu;
      m_pInterMenu = NULL;
   }
	   
	virtual void InitRingSDKWindow(){}		//初始化成员数据，由派生类实现
	virtual void UpdateToolBar(){}			//更新工具栏，由派生类实现

	BOOL IsClipBoradFormat(UINT format);
	BOOL IsClipBoradFormatText();

	//允许停靠
	RingDockSite* GetDockSite(int index);
	RingDockSite* CreateDockSite(int index);

	BOOL SetDockBackGround(LPCTSTR lpszBmp,BOOL bSetOrg = FALSE,int index = -1,
							HWND hStandWnd=(HWND)0xFFFFFFFF,HINSTANCE hInst=GetInstance());
	BOOL SetDockBackGround(HBITMAP hbmp,BOOL bSetOrg = FALSE,int index = -1,HWND hStandWnd=(HWND)0xFFFFFFFF);
	BOOL SetDockBackGround(COLORREF crColor,int index = -1,HWND hStandWnd=(HWND)0xFFFFFFFF);
	//更新停靠栏
	virtual void UpdateDockSite();

	LRESULT OnMenuSelect(HWND hStatusWnd,RINGPARAMS& param,HINSTANCE hInst=GetInstance());

	virtual DEFMSG(WM_RINGTOOLTIP)
	virtual DEFMSG(WM_MENUSELECT)

protected:
	//窗口消息处理
	//static LRESULT CALLBACK WINAPI InitWndProc(HWND hWnd,UINT uMsg,UINT wParam,LONG lParam);
	LRESULT DispMessage(RINGVCLASS*,RingMsgs*,HWND,RINGPARAMS&);
	LRESULT DispCommand(RINGVCLASS*,RingMsgs*&,HWND,RINGPARAMS&);
	LRESULT DispNotify(RINGVCLASS* obj,RingMsgs*& msgs,HWND hWnd,RINGPARAMS& param);

	void NotifyChild(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam);
	virtual LRESULT NotifyChildProc(HWND,UINT,UINT,LPARAM);	
	virtual LRESULT RingdowProc(HWND,RINGPARAMS);
	virtual LRESULT DefaultProc(RINGPARAMS& param)
	{
		//因为界面库没有用HOOK截获创建窗口事件，因此把窗口过程设置为RingdowProc的时间相对滞后，
		//如果有程序HOOK界面库创建的程序，并且把窗口过程设置为自己的窗口过程，则有可能其窗口
		//过程又被界面库的窗口过程替换，因此为兼容该情况，默认消息处理设置为CallWindowProc，
		//这样一来，对RingSDK进行HOOK的程序一个写不好就有可能导致非法操作或程序运行不正常，
		//不知道这是界面库的优点还是缺点了:)
		LRESULT res = 0;

		if(m_hWnd)
			res = CallWindowProc(m_DefProc,m_hWnd,param.uMsg,param.wParam,param.lParam);
			//DefWindowProc(m_hWnd,param.uMsg,param.wParam,param.lParam);
		
		if(param.uMsg == WM_NCDESTROY)
			OnNcDestroy();
		
		return res;
	}
	
   virtual void OnCreate(HWND);
	void OnMeasureCoolMenu(LPMEASUREITEMSTRUCT lParam);
   void OnDrawCoolMenu(LPDRAWITEMSTRUCT lParam);
	BOOL Register(LPCTSTR szClass,
                 HBRUSH hbkg,
                 LPCTSTR lpszMenu = NULL,
                 HICON hBigIcon = NULL,
                 HCURSOR hCur = LoadCursor(NULL,IDC_ARROW),
                 HICON hSmallIcon = NULL,
					  int windowType = 0);

private:
	RingDockSite* GetFirstDockSite();
		
protected:
	RingMsgs* m_msgs;
	RingDockSite** m_dockSite;
	RINDOWINFO m_windowInfo;

	friend RingFlyBar;
	friend RingCoolBar;
	friend RingDockSite;
	friend RingDockBar;
	friend RingStatusBar;
	friend RingToolBar;

private:
	RingCoolMenu* m_pInterMenu;
};

//#define ACTIVECHILDWND(id,szTitle,szClass,ringvar)	\
//ActiveChild(&id,&id##_rdinfo,szTitle,szClass,ringvar)

#define DUMPRINGDOW(classid,id)	\
typedef RingWnd_##classid	RingWnd_##id;\
RingWnd_##id* id = NULL

/*****************************************************
//
//	MDI窗口类
//
*****************************************************/
class RingMDIClient:public RingBaseWnd
{
public:
	RingMDIClient():m_extWnd(NULL),m_nFirstChildid(0),m_bChildMax(FALSE){}
	~RingMDIClient(){}
	
	HWND Create(CLIENTCREATESTRUCT* css);
	
	//设置外部监视用MDICLIENT的HWND，WinMain中窗口
	//消息循环需要TranslateMDISysAccel(hwndMDIClient,&msg)
	//但MDICLIENT窗口关闭时这个外部的hwndMDIClient无法知道
	//自己已无效，需要设置进来，MDICLIENT关闭时置该
	//hwndMDIClient为NULL。
	void SetMonExtClientWnd(HWND * extWnd)
	{m_extWnd = extWnd;}

	//更新父窗口菜单栏
	void RefreshMenu(RingCoolMenu* rm,HWND hChildWnd);
	void UpdateMenuBar(HWND hChildWnd);

protected:
	virtual LRESULT RingdowProc(HWND,RINGPARAMS);
	virtual LRESULT DefaultProc(RINGPARAMS& param)
	{
		return CallWindowProc(m_DefProc,m_hWnd,param.uMsg,param.wParam,param.lParam);
	}

private:
	HWND* m_extWnd;
	HMENU m_parentMenu;
	int m_nFirstChildid;
	BOOL m_bChildMax;
};

class RingMDIFrameWnd:public RingWnd
{
public:
	RingMDIFrameWnd();
   virtual ~RingMDIFrameWnd();

   //void RegChild(LPRINGDOWMSGINFO lprmi){m_lpChildMsgInfo = lprmi;}
   BOOL CreateChild(LPCTSTR lpsztitle,RingWnd* child,BOOL bOpen=FALSE,
							DWORD dwStyle=0,LPARAM lParam=0,LPSTR szClass=NULL);

   virtual HWND Create(LPCTSTR szTitle,LPCTSTR szClass,
								DWORD dwStyle,DWORD dwExStyle = 0,
								int x=CW_USEDEFAULT,int y=CW_USEDEFAULT,
								int width=CW_USEDEFAULT,int height=CW_USEDEFAULT,
								HWND hParentWnd=NULL,HMENU hMenu=NULL,LPARAM lParam=0);
	void NotifyChild(HWND,UINT,UINT,LPARAM);

   inline HWND GetClient(){return m_client.Handle();}
	inline RingMDIClient* GetClientObj(){return &m_client;}
	RingMDIChildWnd* GetCurrChild(BOOL* bMaxMin=NULL);
	RingMDIChildWnd* GetMDIChild();
	RingMDIChildWnd* GetNextMDIChild(RingMDIChildWnd* child);
	HWND GetCurrChildWnd(LPBOOL bMax=NULL);
	//获取默认创建的MDI子窗口类名，子窗口创建时如果指定了窗口类名本函数无效
	LPCTSTR GetChildClassName(){return m_szChildClass;}

	//以下函数参数无用，仅为保持与窗口过程中CMDFUNC类型一致，
	//以使这些函数能作为CMDFUNC加入到消息映射列表
	virtual void OnCascadeChild(UINT){CascadeChild();}
	virtual void OnTileChildH(UINT){TileChild(MDITILE_HORIZONTAL);}
	virtual void OnTileChildV(UINT){TileChild(MDITILE_VERTICAL);}
	virtual void OnTileChild(UINT){TileChild();}
	virtual void OnCloseChild(UINT){CloseChild(GetCurrChildWnd());}
	virtual void OnArrangeIcon(UINT){ArrangeIcon();}
	virtual void OnNextChild(UINT){ActiveNextChild();}
	virtual void OnPrevChild(UINT){ActivePrevChild();}
	
	void CascadeChild(UINT flag=0/*MDITILE_SKIPDISABLED*/);
	void TileChild(UINT flag = MDITILE_VERTICAL);
	void CloseChild(HWND hWnd = NULL);
	void ActiveChild(HWND);
	//以下函数参数无用，仅为保持与窗口过程中CMDFUNC类型一致，
	//以使这些函数能作为CMDFUNC加入到消息映射列表
	void ArrangeIcon(UINT event=0);
	void MaxChild(UINT event=0);
	void MinChild(UINT event=0);
	void RestoreChild(UINT event=0);
	void ActiveNextChild(UINT event=0){SendMessage(m_client.Handle(),WM_MDINEXT,NULL,0);}
	void ActivePrevChild(UINT event=0){SendMessage(m_client.Handle(),WM_MDINEXT,NULL,1);}
	//只要还有子窗口，保证有活动窗口
	BOOL KeepChildActive();

	//设置外部监视用MDICLIENT的HWND，WinMain中窗口
	//消息循环需要TranslateMDISysAccel(hwndMDIClient,&msg)
	//但MDICLIENT窗口关闭时这个外部的hwndMDIClient无法知道
	//自己已无效，需要设置进来，MDICLIENT关闭时置该
	//hwndMDIClient为NULL。
	void SetMonExtClientWnd(HWND * extWnd)
	{m_client.SetMonExtClientWnd(extWnd);}

	BOOL IsChildMax();

protected:
	void OnCreate(HWND);
	virtual LRESULT DefaultProc(RINGPARAMS& param)
	{		
		if(param.uMsg != WM_SIZE)
		{
			LRESULT res = DefFrameProc(m_hWnd,m_client.Handle(),param.uMsg,param.wParam,param.lParam);
			if(param.uMsg == WM_NCDESTROY)
				OnNcDestroy();
			return res;
		}
		else
			return CallWindowProc(m_DefProc,m_hWnd,param.uMsg,param.wParam,param.lParam);
		//DefWindowProc(m_hWnd,param.uMsg,param.wParam,param.lParam);
	}
	
private:
	void FindWindowMenu(LPCLIENTCREATESTRUCT lpccs);
	//菜单栏操作
	void SetMenuBar(RingMenuBar* menubar){m_menuBar = menubar;}
	RingMenuBar* GetMenuBar();

private:	
	RingMDIClient m_client;
	int m_nChildid;
   char m_szChildClass[80];
	RingMenuBar* m_menuBar;
	friend class RingMDIClient;	
	friend class RingMenuBar;
};

class RingMDIChildWnd:public RingWnd
{
public:
	RingMDIChildWnd(){m_windowType = ITIS_MDICHILD;SetAutoDelete();}
	~RingMDIChildWnd(){}
	
	virtual HWND Create(LPCTSTR szTitle,LPCTSTR szClass,
								DWORD dwStyle,DWORD dwExStyle = 0,								
								int x=CW_USEDEFAULT,int y=CW_USEDEFAULT,
								int width=CW_USEDEFAULT,int height=CW_USEDEFAULT,
								HWND hParentWnd=NULL,HMENU hMenu=NULL,LPARAM lParam=0);

	//virtual int Exit(int nResult=0){return DestroyWindow(m_hWnd);}
	virtual int Close(int nResult=0){return SendMessage(m_hWnd,WM_CLOSE,0,0);}

protected:
	virtual LRESULT RingdowProc(HWND,RINGPARAMS);
	friend class RingWnd;

private:
	static LRESULT CALLBACK InitChildProc(HWND,UINT,UINT,LPARAM);
};

class RingDialog:public RingWnd
{
public:
	RingDialog();
	RingDialog(UINT id){m_windowType = ITIS_DIALOG;m_id=id;}
   virtual ~RingDialog();

	WNDPROC SubClassWindow(HWND hWnd);
	   
	void InitRingControls();
   //static BOOL CALLBACK CheckControlProc(HWND hWnd,LPARAM lParam);
   //static BOOL CALLBACK AddControlProc(HWND hWnd,LPARAM lParam);
   //RingControls* GetItem(UINT uId,DWORD dwStyle = 0);

	virtual void SetId(UINT uId=0){if(uId!=0)m_id=uId;}
   virtual int DoModal(LPCTSTR lpTemplate=NULL,LPARAM dwInitParam = 0,
							HWND hParentWnd = NULL,HINSTANCE hInst=GetInstance());
	HWND Create(LPCTSTR szTitle,LPCTSTR szClass,
					DWORD dwStyle,DWORD dwExStyle = 0,
					int x=CW_USEDEFAULT,int y=CW_USEDEFAULT,
					int width=CW_USEDEFAULT,int height=CW_USEDEFAULT,
					HWND hParentWnd=NULL,HMENU hMenu=NULL,LPARAM lParam=0);
	virtual HWND Create(LPCTSTR lpTemplate=NULL,LPARAM dwInitParam = 0,
								HWND hParentWnd = NULL,HINSTANCE hInst=GetInstance());
   virtual HWND CreateIndirect(LPCTSTR szCaption,DWORD dwStyle,
											int x,int y,int width,int height,
											HWND hParentWnd=NULL,LPARAM lParam=0);
	virtual int DialogIndirect(LPCTSTR szCaption,DWORD dwStyle,
											int x,int y,int width,int height,
											HWND hParentWnd=NULL,LPARAM lParam=0);
	int Close(int nResult=0);

	//HBRUSH SetBkgColor(COLORREF crColor);
	//HBRUSH SetBkgBitmap(HBITMAP hbm);
	HBRUSH SetBkgBitmap(UINT uId,HINSTANCE hInst = GetInstance())
	{return RingBaseWnd::SetBkgBitmap(uId,hInst);}
	//设置对话框返回值
	LONG SetResult(LONG lRes){return SetWindowLong(m_hWnd,DWL_MSGRESULT,lRes);}
//	void SendExit(){PostMessage(m_hWnd,WM_CLOSE,0,0);}
	
	virtual LRESULT OnCtlColorDlg(HWND,RINGPARAMS&);
	virtual void OnClose(UINT event){Close();}

protected:	
	virtual LRESULT RingdowProc(HWND hWnd,RINGPARAMS);
	static LRESULT CALLBACK DefaultDlgProc(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
	{
		return FALSE;
	}
	
protected:
	BOOL m_modal;	//模态/非模态标记
	UINT m_id;
	//BOOL CALLBACK WINAPI OnClose(WPARAM,LPARAM);

private:
	static LRESULT CALLBACK WINAPI InitDlgProc(HWND hWnd,UINT uMsg,UINT wParam,LONG lParam);
	static void CreateTemplate(LPDLGTEMPLATE lpdt,LPCTSTR szCaption,DWORD dwStyle,int x,int y,int width,int height);
	friend class RingDockDlg;
};

//打开/保存文件对话框HOOK对话框封装
class RingOFNHookDialog:public RingDialog
{
public:
	RingOFNHookDialog(){m_thunkParent = m_OFNProc = NULL;}
	RingOFNHookDialog(UINT id){m_windowType=ITIS_DIALOG;m_id=id;m_thunkParent=m_OFNProc=NULL;}
   virtual ~RingOFNHookDialog(){m_OFNProc=m_thunkParent=(WNDPROC)Del((LPSTR)m_thunkParent);}

	LPOFNHOOKPROC GetHookProc(){return (LPOFNHOOKPROC)InitDlgProc;}

	//消息封装
	LPCTSTR GetFilePath(LPTSTR szFile,int nSize)
	{
		if(szFile && SendMessage(m_hWndParent,CDM_GETFILEPATH,nSize,(LPARAM)szFile) > 0)
			return szFile;
		else
			return NULL;
	}
	LPCTSTR GetFolderPath(LPTSTR szFolder,int nSize)
	{
		if(szFolder && SendMessage(m_hWndParent,CDM_GETFOLDERPATH,nSize,(LPARAM)szFolder) > 0)
			return szFolder;
		else
			return NULL;
	}
	LPCTSTR GetSpec(LPTSTR szFile,int nSize)
	{
		if(szFile && SendMessage(m_hWndParent,CDM_GETSPEC,nSize,(LPARAM)szFile) > 0)
			return szFile;
		else
			return NULL;
	}
	void SetControlText(UINT uId,LPSTR szText)
	{
		SendMessage(m_hWndParent,CDM_SETCONTROLTEXT,uId,(LPARAM)szText);
	}

protected:
	LRESULT RingdowProc(HWND,RINGPARAMS);

private:
	static LRESULT CALLBACK WINAPI InitDlgProc(HWND hWnd,UINT uMsg,UINT wParam,LONG lParam);
	static LRESULT CALLBACK WINAPI OFNDlgProc(HWND hWnd,UINT uMsg,UINT wParam,LONG lParam);

private:
	WNDPROC m_thunkParent,m_OFNProc;
	HWND m_hWndParent;
};

class RingCtrlWnd:public RingBaseWnd
{
public:
	RingCtrlWnd():m_ID(0),m_ExStyle(0){SetAutoDelete(TRUE);}
   virtual ~RingCtrlWnd() = 0;
	
	UINT GetId(){return m_ID;}
	UINT ReScanId(){return m_ID = GetWindowLong(m_hWnd,GWL_ID);}
	
	BOOL IsControls(){return TRUE;}

protected:
	//void OnParentNotify(HWND,UINT,UINT,LONG){}
	LRESULT DefaultProc(RINGPARAMS&);
	
	HWND Create(LPCTSTR szTitle,
		LPCTSTR szClass,
		DWORD dwStyle,
		HWND hParentWnd = NULL,
		DWORD dwExStyle = 0,		
		HMENU hMenu = NULL,
		int x = CW_USEDEFAULT,
		int y = CW_USEDEFAULT,
		int width = CW_USEDEFAULT,
		int height = CW_USEDEFAULT);

protected:
   UINT m_ID;
   DWORD m_ExStyle;

	friend RingCoolBar;
	
private:   
//   static RingdowList m_childList;
//	friend RingAppWnd;
//	friend RingdowList;
};



/******************************************************************
//
//销毁窗口类对象
//	注意：obj必须是其原始类型，即作为参数传入时不可做强制类型转换
//
******************************************************************/
template<typename tname>
void DelRingObj(tname*& obj)
{
	try
	{
		delete obj;
	}
	catch(...)
	{
	}
	obj = NULL;
}

#endif
