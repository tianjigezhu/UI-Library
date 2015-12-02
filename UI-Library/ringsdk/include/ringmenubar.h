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
//原文件名：ringmenubar.h
//
//本文件说明：RingMenuBar类声明
//
//类库功能：菜单栏窗口封装
//
**********************************************************************/
#ifndef _INC_RINGMENUBAR
#define _INC_RINGMENUBAR

//是否选中菜单栏上的系统图标，最小化，还原，关闭按钮
//警告：以下定义值不可更改
#define MBAR_SYSICONSEL		996
#define MBAR_SYSMINSEL		997
#define MBAR_SYSRESTORESEL	998
#define MBAR_SYSCLOSESEL	999
//警告结束

typedef struct tagRingMenuBarItem
{
	RECT m_rect;
	int m_state;
	LPTSTR m_text;
	int m_id;
	DWORD m_data;
}RINGMENUBARITEM,*LPRINGMENUBARITEM;

class RingMenuBar:public RingCtrlWnd
{
public:
	RingMenuBar();
	~RingMenuBar();
	
	BOOL Create(LPCTSTR szTitle,int dockStyle=TBS_EX_DOCKABLE,
					int dockState=TBS_FLAG_DOCKTOP);
	BOOL LoadMenu(HMENU hMenu=NULL,BOOL bRemove=TRUE);
	BOOL LoadMenu(RingCoolMenu* rm,BOOL bRemove=TRUE);
	//void PressButton(UINT uID,BOOL bPress,BOOL bCheckSysBtn);
	
	void PressButton(int index,BOOL bPress=TRUE);
	int GetButtonCount(){return m_nBtnCount;}
	BOOL GetButtonRect(int index,LPRECT lprect);

	void SetSysControlInfo(BOOL m_bChildMax);
	RingCoolMenu* GetCoolMenu(){return m_rm;}

	//HDWP DefDeferWindowPos(HDWP,int,int,int,int,UINT uAppendFlag=0);

protected:
	BOOL AddButton(LPCTSTR szText,int state,DWORD dwData);
	BOOL InsertButton(int index,LPCTSTR szText,int state,DWORD dwData);

	BOOL AllocMenuItems(int cnt);
	void FreeMenuItems();
	
	void Update(int state=-1);
	BOOL SetHotButton(int index);

	LRESULT RingdowProc(HWND,RINGPARAMS);
	LRESULT OnLButtonDown(RINGPARAMS& param);
	LRESULT OnMouseMove(RINGPARAMS& param);
	LRESULT OnLButtonUp(RINGPARAMS& param);
	LRESULT OnRButtonDown(RINGPARAMS& param);
	LRESULT OnTimer(RINGPARAMS& param);
	LRESULT OnPaint(RINGPARAMS& param);
	LRESULT OnNotify(RINGPARAMS& param);
	LRESULT OnHitTest(RINGPARAMS& param);
	//LRESULT OnWinPosChanged(RINGPARAMS& param);
	LRESULT OnSizing(RINGPARAMS& param);

	BOOL OnParentNotify(HWND,UINT,UINT,LONG);
	
	BOOL OnGetDragRects(int state,LPRECT lprc);
	//停靠状态，响应停靠状态改变事件，返回-1：未改变，TRUE：已处理，0：未处理
	int OnDockStateChange(int stateOld,int stateNew);
	//void ChangeState(int state,RingDockSite* docksite=NULL);

	BOOL MakeMenuButtons();
		
	void DrawExtButton(HDC hdc);
	//bCheckAll：是否仅判断系统图标,该参数默认值不可轻易更改
	int CheckInExtButton(LPPOINT lpMousePt,BOOL bCheckAll = FALSE);
	void PressExtButton(UINT uID,BOOL bPress);
	
	int GetBtnsWidth();
	int GetBtnsHeight();
	
	//按钮组的前后扩展尺寸（与图标，系统按钮是否显示有关）
	int EndSize(){return m_bShowIcon?50:0;}
	int PreSize(){return m_bShowIcon?18:0;}
		
private:
	BOOL InterLoadMenu(BOOL bRemove);
	//获取单列菜单栏宽度
	int GetVertLineWidth();
	//获取单行菜单栏高度
	int GetHorzLineHeight();

private:
	RingCoolMenu* m_rm;
	RingMenu* m_rmsys;
	ringFont m_font,m_vfont;
	HICON m_sysIcon;
	HBITMAP m_hbmSys;
	int m_nPopupedMenu,m_nHotButton;
	BOOL m_bShowIcon;
	LPRINGMENUBARITEM m_item;
	int m_nBtnCount,m_BtnAllocCnt;
	//坐标：关闭按钮，最小化最大化按钮，标题栏
	RECT m_rcSysBtn,m_rcRestore,m_rcIcon;
	//停靠/浮动状态下的宽高,按钮组排列时的最大宽高,实际窗口宽高，单行/列宽
	SIZE m_BarSize,m_fSize,m_BtnsSize,m_WndSize,m_SingleSize;
	int m_nMinWidth;	//最小宽度
	//鼠标是否在关闭按钮，最小化最大化按钮，标题栏区域按下
	DWORD m_nCapedExtButton;
	RingDockBar* m_dockbar;
};


#endif
