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
#ifndef _INC_RINGCONTROLS
#define _INC_RINGCONTROLS

#define RINGTIMER_BASE		50
#define RINGITEMSTYLE_FLAT		29001
#define RINGITEMSTYLE_NORMAL	29002
//自适应父窗口尺寸
#define RTV_FITPARENTSIZE	0x1

typedef void (RINGVCLASS::*DRAW_FUNC1)(HDC,LPRECT);

#define CAN_OWNER_DRAW()\
private:\
	void OnMouseOverDraw(HDC,LPRECT);\
	void OnMouseOutDraw(HDC,LPRECT);\
	void OnFocusDraw(HDC,LPRECT);\
	void OnDrawScrollBar(HDC,LPRECT);\
public:\
	void SetOwnerDraw()\
	{\
		m_drawFuncs.mouseoverFunc = (DRAW_FUNC1)OnMouseOverDraw;\
		m_drawFuncs.mouseoutFunc = (DRAW_FUNC1)OnMouseOutDraw;\
	}

#define CR_NORMAL				1
#define CR_FLATSET			2
#define CR_FOCUSSET			4
#define CR_FLATBORDERSET	8
#define CR_FOCUSBORDERSET	16

typedef struct _tagBorderColor
{
	COLORREF crFlat_lefttop;
	COLORREF crFlat_rightbottom;
	COLORREF crFocus_lefttop;
	COLORREF crFocus_rightbottom;
	COLORREF crFlat_border;
	COLORREF crFocus_border;
	COLORREF crBkg;
	DWORD		crFlag;
}BORDERCOLOR,*LPBORDERCOLOR;

enum
{
	RC_LEFT,
	RC_RIGHT,
	RC_TOP,
	RC_BOTTOM,
	RC_LEFTTOP,
	RC_RIGHTBOTTOM,
	RC_RECT
};

class RingOwnerDrawer
{
public:
	RingOwnerDrawer(){};
	virtual ~RingOwnerDrawer(){};
/*
	virtual void OnNcPaint(HDC,LPRECT){}
	virtual void OnMouseHover(HDC,LPRECT){}
	virtual void OnMouseLeave(HDC,LPRECT){}
	virtual void OnSetFocus(HDC,LPRECT){}
	virtual void OnKillFocus(HDC,LPRECT){}
	virtual void OnLButtonDown(RINGPARAMS& param){}
	virtual void OnLButtonUp(RINGPARAMS& param){}
	virtual void OnMouseMove(RINGPARAMS& param){}
*/
	virtual LRESULT CALLBACK DrawProc(RingControls*,RINGPARAMS& param);
};

#define DEF_OWNERDRAWFUNC()	\
	void OnNcPaint(HDC,LPRECT);\
	void OnMouseHover(HDC,LPRECT);\
	void OnMouseLeave(HDC,LPRECT);\
	void OnSetFocus(HDC,LPRECT);\
	void OnKillFocus(HDC,LPRECT);


#define OWNERDRAWER(control) RingOwnerDrawer_##control::
//#define SETOWNERDRAWER

//控件数值默认缓冲区大小，每个控件均有一个内置缓冲区（视需要创建）
//存放用户输入值，默认RBUF_SIZE长度。
#define RBUF_SIZE		80
//有些消息的参数值正好是-1，与INDEX_CURSEL一致，因此如果修改该值需要
//检查用到INDEX_CURSEL的函数，如RingCombo::Select(int index)，index
//为-1则取消选择，如果修改INDEX_CURSEL值则需要修改该Select函数。
#define INDEX_CURSEL (-1)

class RingControls:public RingCtrlWnd
{
public:
	RingControls();
   virtual ~RingControls() = 0;

	//创建/关联操作
   virtual BOOL Create(DWORD dwStyle,DWORD dwExStyle,LPCTSTR szInitText,UINT uId,int x,int y,int width,int height);
	virtual BOOL Attach(UINT uId,RingBaseWnd* parent=NULL);
	virtual BOOL Attach(HWND);

	//读取,设置数据
	virtual int GetInt(int index=INDEX_CURSEL);
	//本函数不可重载，其内部调用GetItemText，因此派生类可重载GetItemText改变该函数行为。
	LPCTSTR GetText(int index=INDEX_CURSEL,int size=RBUF_SIZE,LPTSTR lpBuf=NULL);
	virtual BOOL SetValue(int val,int index=INDEX_CURSEL);
	virtual BOOL SetValue(LPCTSTR lpszVal,int index=INDEX_CURSEL);
	virtual BOOL Clear(){return TRUE;}

	DWORD GetExStyle(){return m_ExStyle;}
	void SetAutoSize(BOOL bAuto=TRUE);
	virtual BOOL SetReadOnly(BOOL bReadOnly){return FALSE;}
	//virtual BOOL IsReadOnly(){return FALSE;}
	//按键消息发送给父窗口
	void SetKeyEventToParent(BOOL bYes=TRUE){m_bNotifiParentKeyEvent = bYes;}
	void SetSizeFitParent(BOOL bFit = TRUE){m_ExStyle |= (bFit?1:0);}

	virtual void SetTextColor(COLORREF crColor);
	virtual COLORREF GetTextColor(){return m_crTextColor;}
	
	virtual BOOL SetBkColor(COLORREF crColor);
	
	void SetFont(LPCTSTR szFontName,int size,int nStyle=0,COLORREF crText=0);
	ringFont* GetFont(){return &m_font;}

	void SetMouseTrack(RINGPARAMS& param);

	//检查是否与控件类型一致
	BOOL TypeMatched(UINT id);
	BOOL TypeMatched(HWND hWnd);

	virtual LPCTSTR WndClassName() = 0;
				
	//设置自绘，参数为TRUE表示由父窗口处理自绘
	//uItemStyle为具体控件的自绘类型
	void SetOwnerDraw(BOOL bParentDraw,UINT uItemStyle);
	
	RingOwnerDrawer* SetDrawer(RingOwnerDrawer* drawer)
	{
		RingOwnerDrawer* prev = m_drawer;
		if(drawer)
			m_drawer = drawer;
		return prev;
	}
	
/*
	template<typename tname>
	tname* SetOwnerDraw(tname* drawer)
	{
		if(m_drawer)
		{
			tname* tmp = dynamic_cast<tname*>(m_drawer);
			if(tmp)	//类别相同，视为已设置
				return (tname*)m_drawer;
			else
				delete m_drawer;
		}
		m_drawer = new tname;
		return (tname*)m_drawer;
	}
*/	
protected:
	//所有控件构造时初始化
	//virtual void InitControl(RingBaseWnd* parent,UINT uId);
	LRESULT RingdowProc(HWND hWnd,RINGPARAMS);
	BOOL OnParentNotify(HWND,UINT,UINT,LONG);
	virtual BOOL OnFitSize(HWND,WPARAM,LPARAM);
	virtual BOOL OnDrawItem(LPDRAWITEMSTRUCT lpds){return FALSE;}
	//停靠状态，响应停靠状态改变事件，返回-1：未改变，TRUE：已处理，0：未处理
	virtual int OnDockStateChange(int stateOld,int stateNew);
	virtual BOOL OnGetDragRects(int state,LPRECT lprc);
	//LRESULT DefaultProc(RINGPARAMS&);
	int GetBuffer(int size);	
	LRESULT SetColors(HDC);
	virtual LRESULT OnDestroy(RINGPARAMS& param){return DefaultProc(param);}
	virtual int GetItemText(int index,LPTSTR lpBuf,int size);

protected:
	ringFont m_font;
	COLORREF m_crTextColor,m_crBkgColor;
	BOOL m_bNotifiParentKeyEvent;
	RingOwnerDrawer* m_drawer;

	BOOL m_bOwnerDraw;
//private:
	LPSTR m_buf;
   int m_bufsize;	
};

#endif
