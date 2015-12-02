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
//原文件名：ringctrlbar.h
//
//本文件说明：RingDockBar类声明
//
//类库功能：工具条窗口封装
//
**********************************************************************/
#ifndef _INC_RINGCTRLBAR
#define _INC_RINGCTRLBAR

#define TB_NEEDCHANGECTRLBAR	50001				//需要更换RingDockBar
#define TB_NOCHANGECTRLBAR		0					//不需要更换RingDockBar

#define RINGCTRL_IDBASE			0xC100			//工具条ID

//设置RingDockSite背景
#define RBIMG_HBITMAP			1					//是HBITMAP
#define RBIMG_IDBMP				2					//是BMP资源ID
#define RBIMG_BKCOLOR			3					//是COLORREF

//鼠标位置（关闭按钮，最大/小化按钮，其他位置）
#define RCUR_INCLOSE			(1)
#define RCUR_INMINMAX		(2)
#define RCUR_NOWHERE			(3)

//工具条ID
extern DWORD gdwDockBarId;

class RingDockBar;

//处理窗口拖动的辅助类
class RingDragger
{
public:
	RingDragger():m_bDragging(FALSE){}
	~RingDragger(){}

	void StartDrag(HWND hWnd,int xMouse,int yMouse,LPRECT lprc,int width);
	//擦除旧矩形，返回新位置新矩形（但不绘制新矩形）提供机会给
	//调用者判断如何绘制新矩形或进行处理
	//注意返回的RECT是屏幕坐标。lprc返回hWnd的相对坐标
	LPRECT EraseRect(int xMouse,int yMouse,HWND hWnd=NULL,LPRECT lprc=NULL);
	//绘制当前矩形
	//注意：如果绘制后m_DragRc位置没有变动，则再次调用为擦除矩形
	void DrawRect(int width);
	//以指定宽度绘制指定的矩形
	void DrawRect(LPRECT lprc,int width);
	void Drag(int xMouse,int yMouse);
	//返回hWnd的相对坐标
	LPRECT StopDrag(HWND hWnd,int xMouse,int yMouse);

	inline BOOL IsDragging(){return m_bDragging;}
	LPRECT GetCurrRect(HWND hWnd = NULL,LPRECT lprc = NULL);
	//设置鼠标坐标位置（并不实际改变鼠标位置）
	//该两函数作用为改变拖动时的偏移量
	void SetPointX(int x){m_pt.x = x;}
	void SetPointY(int y){m_pt.y = y;}

	//设置拖动矩形，如果hWnd非空，lprc视为相对于hWnd的坐标，
	//转换lprc到屏幕坐标
	void SetDragRect(LPRECT lprc,HWND hWnd/*=NULL*/);

private:
	BOOL m_bDragging;
	int m_width;
	HDC m_DC;
	POINT m_pt;
	RECT m_DragRc;
};

class RingFlyBar:public RingWnd
{
public:
	RingFlyBar(RingBaseWnd* parent);
   ~RingFlyBar();

   BOOL Create(LPCTSTR lpszTitle);
   
   BOOL OnParentNotify(HWND,UINT,UINT,LONG);
	void AddChild(RingDockBar* child);

	void Update();
	void UpdateDockSite();	//虚函数重载
	RingDockBar* GetDockBar(){return m_child;}

protected:
	LRESULT RingdowProc(HWND,RINGPARAMS);
	BOOL DrawBkg(HDC hDC,LPRECT lprc = NULL);

private:
	//强制只可调用复制构造函数创建对象
	RingFlyBar(){}

private:
	RingDockBar* m_child;
	RingDragger* m_dragger;
   RECT m_parentRc,m_DragRc;
   BOOL m_bDrag;
   POINT m_CurPos;
   int m_CurrState;
   int m_cy;
};

typedef struct tagRingBarLineInfo
{
  	RingDockBar* m_first;
	int m_maxSize,m_pos;//行高/宽，起始位置	
	RECT m_SplitRc;	//SizeBar的分割线
	WORD isVisible;
	short m_nSizeBarCnt;
   tagRingBarLineInfo *m_nextline;   
}RINGBARLINEINFO,*LPRINGBARLINEINFO;

typedef struct tagmBmpInfo
{
  	int cx;
   int cy;
	BOOL bSetOrg;			//是否需要背景随窗口移动
   BOOL bNeedDelete;		//析构时是否需要销毁hBrush
	HBRUSH hBrush;
	HWND hStandWnd;		//背景坐标相对与该窗口计算
}BARBMPINFO,*LPBARBMPINFO;

#define RING_SPLITERSIZE 5
	
class RingDockSite:public RingWnd
{
public:
	RingDockSite(RingBaseWnd* parent,DWORD state/*=TBS_FLAG_DOCKTOP*/);
   ~RingDockSite();

	HWND Create();
	   	
   void SetBgBmp(HBITMAP hbm,BOOL bSetOrg = FALSE);
   void SetBgBmp(LPCTSTR lpszBmp,BOOL bSetOrg = FALSE,HINSTANCE hInst=GetInstance());
	void SetBgColor(COLORREF crColor);
	//设置背景相对位置标准的窗口，即背景相对该窗口对齐
	void SetStandWindow(HWND hWnd){m_Background.hStandWnd = hWnd;}
	
	//绘制背景图案，lprc为绘制范围,lpoffset为画刷原点偏移量。
	//如果有背景图案，返回TRUE，否则FALSE
	BOOL DrawBkg(HDC hDC,LPRECT lprc=NULL,LPSIZE lpoffset=NULL);
	
	LPBARBMPINFO GetBgInfo(){return &m_Background;}

	RingDockBar* CreateDockBar(RingBaseWnd* wnd,BOOL bCanSize);
	BOOL ReleaseDockBar(RingBaseWnd* wnd);
	BOOL AddBar(RingDockBar* dockbar,LPRECT lprc);
	RingDockBar* AddBar(RingBaseWnd* wnd,LPCTSTR szTitle,UINT uDockStyle=TBS_EX_DOCKABLE,
								UINT uDockFlag=TBS_FLAG_DOCKTOP,LPRECT lprcPos=NULL,
								BOOL bCanSize = FALSE);
	BOOL DelBar(RingDockBar* dockbar,BOOL bUpdate=TRUE);
	BOOL HideBar(RingDockBar* dockbar);

	BOOL IsVert(){return (m_State > TBS_FLAG_DOCKBOTTOM);}
	BOOL IsHorz(){return (m_State < TBS_FLAG_DOCKLEFT);}

	RingDockBar* GetDockBar(RingBaseWnd* wnd);
	RingDockBar* GetDockBar(UINT uDockBarId);
	//在所有DockSite中寻找DockBar
	RingDockBar* FindDockBar(UINT uDockBarId);
	RingCoolMenu* GetDockSiteMenu();

	int GetState(){return m_State;}
	//更新所有DockSite
	void UpdateAllSite();

protected:
	BOOL OnParentNotify(HWND,UINT,UINT,LONG);
	void NotifyChild(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam);
	LRESULT RingdowProc(HWND,RINGPARAMS);
	
	//加减nAddSize窗口尺寸
	void UpdatePos(int nAddSize,BOOL bUpdate = TRUE);
	//调整位于lpline后面的边线框位置,nStep为增减的尺寸
	void UpdateLine(LPRINGBARLINEINFO lpline,int nStep,BOOL bUpdate = TRUE);
	
	LRESULT OnLButtonDown(RINGPARAMS& param);
	LRESULT OnLButtonUp(RINGPARAMS& param);
	LRESULT OnMouseMove(RINGPARAMS& param);
	LRESULT OnRButtonDown(RINGPARAMS& param);
	LRESULT OnLButtonDbClk(RINGPARAMS& param);
	
	BOOL CheckMouseInSpliter(RINGPARAMS& param);
	RingDockBar* CheckMouseInDockBar(RINGPARAMS& param);
	
	BOOL ParentReSize(int width,int height);
	//void AdjustChildren(int width,int height);
	void AdjustChildrenH(int nEdge);
	void AdjustChildrenV(int nEdge);
	
private:
	//强制只能通过复制构造函数创建对象
	RingDockSite(){}
	void Free();
	
	//更新所有DockSite背景，销毁画刷时调用
	void UpdateAllSiteBkg(HBRUSH hBrush);
	//刷新背景及刷新所有停靠窗口
	void UpdateBkg();
	//更新系统菜单（工具栏列表菜单）
	BOOL UpdateSysMenu(RingDockBar* dockbar,LPCTSTR szText);
	void RemoveSysMenu(RingDockBar* dockbar);
	//响应菜单消息，隐藏/显示工具栏
	BOOL OnDockBarHide(UINT uId);
	//由DockBar通知，更新菜单
	void OnDockBarHide(RingDockBar* dockbar);
	//拖动分隔线
	void OnDragSpliter(RINGPARAMS& param);

	//得到dockbar所在行的前一行。
	LPRINGBARLINEINFO GetPrevLine(LPRINGBARLINEINFO lpline);
	//得到dockbar停靠行，仅在拖动工具栏时判断使用。
	//bLast:是否加到最后。
	LPRINGBARLINEINFO GetDockLine(int nPos);//,BOOL &bLast);

	//dockbar加入到行
	int AddBarInLine(RingDockBar* dockbar,LPRINGBARLINEINFO lpline,LPRECT lprc=NULL);
	//插入insbar到dockbar前面，仅插入，不作任何位置调整
	void InsertDockBar(LPRINGBARLINEINFO lpline,RingDockBar* insbar,RingDockBar* dockbar);
	
	//删除工具行
	BOOL DelLine(LPRINGBARLINEINFO lpline);
	
	BOOL AddBarInLineH(RingDockBar* dockbar,LPRINGBARLINEINFO lpline,LPRECT lprc,int checkpos);
	BOOL AddBarInLineV(RingDockBar* dockbar,LPRINGBARLINEINFO lpline,LPRECT lprc,int checkpos);

	LPRINGBARLINEINFO AddLine(LPRINGBARLINEINFO prev);
	void LinkLine(LPRINGBARLINEINFO prev,LPRINGBARLINEINFO next);

	//int AdjustBarLineV(RingDockBar* bar,int nSpace,RingDockBar* dockbar,int& nSize);
	//int AdjustBarLineH(HDWP hdwp,int nSize,LPRINGBARLINEINFO lpLine,RingDockBar* dockbar,BOOL bCanWrap=FALSE);
	HDWP BarLineResizeH(HDWP hdwp,LPRINGBARLINEINFO lpLine,int nSize);
	HDWP BarLineResizeV(HDWP hdwp,LPRINGBARLINEINFO lpLine,int nSize);
	int WrapLine(RingDockBar* bar);
	
	int BarOutLine(RingDockBar* dockbar,BOOL bCalcSize,LPRINGBARLINEINFO& line);
	BOOL IsSizeBarInLine(LPRINGBARLINEINFO line){return line?(line->m_SplitRc.right>0):FALSE;}
	BOOL SetSizeSplitRect(LPRINGBARLINEINFO line);

	LPRINGBARLINEINFO GetEmptyLine();
	void SetLineEmpty(LPRINGBARLINEINFO line);
	void RegFlyBar(RingDockBar* dockbar);
	void UnregFlyBar(RingDockBar* dockbar);

	//界面刷新
	void DrawSite(HDC hDC,RINGPARAMS& param);

public:
	int m_SplitterSize;

protected:
	RingDragger m_dragger;

private:
	static LPRINGBARLINEINFO m_EmptyLine;
	static RingDockBar* m_flybar;
	LPRINGBARLINEINFO m_First,m_Curr;
	
	int m_State;
	int m_nChild;
	RECT m_rcPos;
	BARBMPINFO m_Background;
	BOOL m_bSPLimited;	//调整窗口线是否已到极限位置无法移动
	HCURSOR m_cursor,m_cursorLine;
	POINT m_ptLimit;		//调整窗口线时的两端极限位置
	RingDockBar* m_CaptureBar;
	
	friend RingFlyBar;
	friend RingDockBar;
	friend RingSizeBar;
};

class RingDockBar
{
public:
	RingDockBar();
	virtual ~RingDockBar();

	UINT GetId(){return m_id;}
	RingBaseWnd* GetChild(){return m_child;}
	RingBaseWnd* GetDockSite(){return m_pSite;}
	//返回相对于父窗口坐标
	BOOL GetDockRect(LPRECT lprc);
	void SetDockStyle(UINT style);
	void SetBarBorder(int nBorder){m_BorderSpace = nBorder;}
	BOOL SetDockTitle(LPCTSTR lpszTitle);
	LPCTSTR GetDockTitle(){return m_szTitle;}

	int GetState(){return m_State;}
	DWORD GetBarStyle(){return m_uStyle;}
	int GetBarBorder(){return m_BorderSpace;}
	BOOL IsVert(){return ::IsVert(m_State);}
	BOOL IsHorz(){return ::IsHorz(m_State);}
	BOOL IsFloat(){return ::IsFloat(m_State);}

	virtual BOOL Dockto(int state,LPRECT lprc=NULL);
	BOOL DockFly(LPRECT lprc=NULL);
	int DockBack();
	virtual void Show(int nState);

	void UpdateSize();
	void UpdateClientSize();
	void UpdateSize(int nWidth,int nHeight,BOOL bUpdate);
	virtual void UpdateFlySize(int nWidth,int nHeight);
	void UpdateFlyBkg();
	//更新所有DockBar
	void UpdateAllSite();
	//BOOL GetMaxClientRect(LPRECT lprcClient);

	virtual BOOL IsMenuBar(){return FALSE;}
	virtual BOOL IsSizeBar(){return FALSE;}
	BOOL IsStatusBar(){return m_child?(m_child->GetWindowType()==ITIS_STATUSBAR):FALSE;}
	BOOL IsVisible(BOOL *lpbUpdate=NULL);
	//是否独占一行
	BOOL IsAllLineBar(){return (m_uStyle & TBS_EX_ALLLINE);}
	BOOL CanDock(int nState){return (m_uStyle & (1 << nState));}

	//根据宽高参数返回客户区宽高
	int GetClientWidth(int width);
	int GetClientHeight(int height);

	virtual void InitDockSize(){}
	virtual UINT HitTest(POINT* lpt);

	BOOL DrawBkg(HDC hDC,LPRECT lprc=NULL);

protected:
	BOOL SetChild(RingBaseWnd* child);

	virtual LRESULT OnClose();
	virtual LRESULT OnSizing(RINGPARAMS& param);
	//virtual LRESULT OnWinPosChanged(RINGPARAMS& param);
	
	void AdjustSizingRect(int flag,LPRECT lprc,RECT& rc);
	
	virtual LRESULT StartDrag(HWND hWnd,RingDragger* dragger,RINGPARAMS& param);
	virtual LRESULT OnDrag(HWND hWnd,RingDragger* dragger,RINGPARAMS& param);
	virtual LRESULT StopDrag(HWND hWnd,RingDragger* dragger,RINGPARAMS& param);
	virtual void StartPushFrameControl(HWND hWnd,RingDragger* dragger,RINGPARAMS& param);
	virtual void OnPushHoverFrameControl(HWND hWnd,RingDragger* dragger,LPPOINT lpt);
	virtual void StopPushFrameControl(HWND hWnd,RingDragger* dragger,LPPOINT lpt);

	virtual BOOL SetDockSite(RingDockSite* docksite);
	//停靠状态改变事件的默认处理，返回-1：未改变，TRUE：已处理，0：未处理
	virtual void OnDockStateChange(int stateOld,int stateNew){}

	//DockBar拖动时判断拖动的矩形是否要改变形状
	//lprc,pt均为屏幕坐标,
	//w,h:工具栏纵向状态的宽，横向状态的高（即两种状态下宽高的最小尺寸）
	int CheckFloatRect(HWND hWnd,LPRECT lprc,POINT pt);//,int w,int h);
	//CheckFloatRect调用，设置dragger使用矩形，返回矩形边框宽度
	virtual int SetDragRect(HWND hWnd,RingDragger* dragger);

	virtual void DrawGripper(HDC hDC);
	virtual void DrawFrame(HDC hDC);
	virtual void UpdateCaption();
	BOOL Draw(HDC hDC);
	
	//得到拖动矩形
	virtual LPRECT GetDragRects(int state);
	virtual BOOL GetClientRect(int state,LPRECT lprc);
	virtual void InitDragRects(HWND hWnd,POINT pt,BOOL bInit);
	void InitAdjustRects(POINT& pt,LPRECT lprcCheck);
	void AdjustDragRects(POINT& pt,LPRECT lprcCheck);
	void AdjustRects(HWND hWnd,POINT pt,int state);
	
	BOOL SetDockPos(int x,int y,int width=-999,int height=-999);
	void SetDockPos(HDWP hdwp);
	void OffsetDockPos(int x,int y);

	virtual int GetPreSpace();
	virtual HDWP AdjustDockPosH(HDWP hdwp,int nEdge,int nPreSpace,BOOL bCanWrap);
	virtual HDWP AdjustDockPosV(HDWP hdwp,int nEdge,int nPreSpace,BOOL bCanWrap);

	virtual BOOL AdjustChildH(int);
	virtual BOOL AdjustChildV(int);

private:
	BOOL AdjustSizeChildH();
	BOOL AdjustSizeChildV();

protected:	
	UINT m_uStyle,m_id;
	int m_CurWhere;	//鼠标位于何处。
	int m_State,m_currState;
	RingBaseWnd* m_child;
   RingDockSite* m_pSite;
	RingFlyBar* m_flyBar;
	RingDockBar* m_next,*m_prev;
	int m_nSize;	//窄边长度，即停靠时横向的高度，竖向的宽度
	LPRINGBARLINEINFO m_lineInfo;
	//横向，竖向，浮动状态的矩形（拖动时用），最后的浮动矩形，停靠矩形（DockBack用）
	RECT m_hrc,m_vrc,m_frc,m_lfrc,m_ldrc;	
	int m_BorderSpace;
	int m_GripperSpace;
	//鼠标是否在关闭按钮，最小化最大化按钮，标题栏区域按下
	int m_bNCBtDown;
	//坐标：关闭按钮，客户区
	RECT m_rcClose,m_rcClient;
	RECT m_rcDockPos,m_rcCurr;
	LPSTR m_szTitle;	//窗口标题
	int m_nTitleSize;	//窗口标题缓冲区尺寸
	BOOL m_bVisible;


	friend class RingDockSite;
	friend class RingFlyBar;
	friend class RingMenuBar;
	friend class RingSizeBar;	
};

typedef DWORD (RingDockBar::*CHECKPOSFUNC)(LPRECT lprc);

class RingSizeBar:public RingDockBar
{
public:
	RingSizeBar();
	virtual ~RingSizeBar();
	
	BOOL IsSizeBar(){return TRUE;}
	void InitDockSize();
	void Show(int nState);
	UINT HitTest(POINT* lpt);

	void UpdateFlySize(int nWidth,int nHeight);

protected:
	void DrawGripper(HDC hDC);
	void UpdateCaption();
	
	BOOL GetClientRect(int state,LPRECT lprc);

	void StartPushFrameControl(HWND hWnd,RingDragger* dragger,RINGPARAMS& param);
	void OnPushHoverFrameControl(HWND hWnd,RingDragger* dragger,LPPOINT lpt);
	void StopPushFrameControl(HWND hWnd,RingDragger* dragger,LPPOINT lpt);
	void OnMinMax();

	//LRESULT OnSizing(RINGPARAMS& param);
	int GetPreSpace();
	HDWP AdjustDockPosH(HDWP hdwp,int nEdge,int nPreSpace,BOOL bCanWrap);
	HDWP AdjustDockPosV(HDWP hdwp,int nEdge,int nPreSpace,BOOL bCanWrap);

	BOOL AdjustChildH(int nAddSpace);
	BOOL AdjustChildV(int nAddSpace);

	//停靠状态改变事件的默认处理，返回-1：未改变，TRUE：已处理，0：未处理
	void OnDockStateChange(int stateOld,int stateNew);
	//重载函数，设置拖动矩形，返回矩形宽度
	int SetDragRect(HWND hWnd,RingDragger* dragger);

protected:	
	HFONT m_font,m_vfont;
	//坐标：关闭按钮，最小化最大化按钮，标题栏，拖动时整行停靠矩形
	RECT m_rcMinMax,m_rcCaption,m_rcAllLine,m_rcSpliter[2];	
	//最大/小化标志,正常状态下宽/高
	int m_nZoom,m_nRestoreSize;
};

//整行停靠工具栏（菜单栏）
class RingAllLineBar:public RingDockBar
{
public:
	RingAllLineBar();
	virtual ~RingAllLineBar(){}
	
	BOOL IsMenuBar(){return TRUE;}
	
protected:
	//重载函数，设置拖动矩形，返回矩形宽度
	int SetDragRect(HWND hWnd,RingDragger* dragger);
	LRESULT StopDrag(HWND hWnd,RingDragger* dragger,RINGPARAMS& param);
	//void DrawFrame(HDC hDC);

private:
	BOOL ResetDragRect(LPRECT lprc,int state);

protected:	
	BOOL m_bSetH,m_bSetV;
};

#endif
