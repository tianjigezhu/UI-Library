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
//版本：1.0
//
//声明：本类库可以自由使用而不须对作者作出任何回报，但作者希望能得到
//		  你的鼓励和支持。你可以对类库源码作出修改和改进，但希望你能在
//		  修改的同时给作者一份同样的副本。
//		  本类库不得用于任何商业用途，如确实需要，请与作者联系。
//
//e-mail:ringphone@sina.com
//
//原文件名：ringbutton.h
//
//本文件说明：RingButton类声明
//
//类库功能：Button控件封装
//
**********************************************************************/
#ifndef _INC_RINGBUTTON
#define _INC_RINGBUTTON

#define ATBTN_LEFT	0
#define ATBTN_TOP		1
#define ATBTN_RIGHT	2
#define ATBTN_BOTTOM	4

//按钮扩展类型，因为BUTTON没有扩展类型，因此这里可以任意设置，但必须
//注意：任何值的最低位不能为1！！！因为RTV_FITPARENTSIZE的定义值为1，
//最低位为1会被误认为自适应父窗口尺寸
#define RINGBTN_EX_FLAT				2
#define RINGBTN_EX_NOFOCUSRECT	4

class RingButton:public RingControls
{
public:
	RingButton();
	virtual ~RingButton();
	
   LPCTSTR WndClassName(){return "Button";}
	
	void Click();
	virtual BOOL Check(UINT uCheck=BST_CHECKED,UINT id=0);
	virtual UINT IsChecked(UINT uid=0);
	UINT GetState(){return ::SendMessage(m_hWnd,BM_GETSTATE,0,0);}
	//设置按下，弹起状态，自绘按钮调用Check无用，因此使用m_bPushed记录
	void SetPush(BOOL bPushed){m_bPushed = bPushed;InvalidateRect(m_hWnd,NULL,TRUE);}
	BOOL IsPushed(){return m_bPushed;}
	void SetTextColor(COLORREF crColor);
	BOOL SetBkColor(COLORREF crColor);

	HICON SetIcon(HICON hicon);
	HICON SetIcon(UINT uidIcon,int width=0,int height=0,HINSTANCE hInst=GetInstance());
	HBITMAP SetBitmap(HBITMAP hbmp);
	HBITMAP SetBitmap(UINT uidBmp,HINSTANCE hInst=GetInstance());

	//在按钮上加图标
	HICON AddIcon(HICON hicon,int nWhere = ATBTN_LEFT);
	HICON AddIcon(UINT uidIcon,int nWhere = ATBTN_LEFT,int width=0,int height=0,HINSTANCE hInst=GetInstance());
	HBITMAP AddBitmap(HBITMAP hbm,int nWhere = ATBTN_LEFT);
	HBITMAP AddBitmap(UINT uidBmp,int nWhere = ATBTN_LEFT,HINSTANCE hInst=GetInstance());

	HBITMAP GetBitmap(){return (HBITMAP)SendMessage(m_hWnd,BM_GETIMAGE,IMAGE_BITMAP,0);}
	HICON GetIcon(){return (HICON)SendMessage(m_hWnd,BM_GETIMAGE,IMAGE_ICON,0);}
	//设置自绘，参数为TRUE表示由父窗口处理自绘
	void SetOwnerDraw(BOOL bParentDraw=FALSE)
		{RingControls::SetOwnerDraw(bParentDraw,BS_OWNERDRAW);}

	void SetExStyle(DWORD dwExStyle);

protected:
	BOOL OnDrawItem(LPDRAWITEMSTRUCT lps);
	LRESULT OnBkColor(UINT,HDC);
	LRESULT OnDestroy(RINGPARAMS& param);
	
	//绘制外框
	virtual void DrawFrame(LPDRAWITEMSTRUCT lps,DWORD style);
	//检测文字包围矩形
	LPCTSTR GettextRect(LPDRAWITEMSTRUCT lps,DWORD style,LPRECT lprc);
	//绘制文字及图标或BMP
	void DrawImage_Text(LPDRAWITEMSTRUCT lps,LPCTSTR s,DWORD style,LPRECT lprc);

protected:
	BOOL m_bNeedFreeImage;
	BOOL m_bPushed;	//自绘时按钮状态
	int m_nImgWhere;
};

class RingCheckBox:public RingButton
{
public:
	RingCheckBox(){}
	virtual ~RingCheckBox(){};
	
   BOOL Create(DWORD dwStyle,DWORD dwExStyle,LPCTSTR szInitText,UINT uId,int x,int y,int width,int height);
};

class RingRadioButton:public RingButton
{
public:
	RingRadioButton():m_first(0),m_last(0){}
	virtual ~RingRadioButton(){};
	
   BOOL Create(DWORD dwStyle,DWORD dwExStyle,LPCTSTR szInitText,UINT uId,int x,int y,int width,int height);
	
	void SetGroup(UINT uFirst,UINT uLast){m_first=uFirst;m_last=uLast;}
	BOOL Check(UINT id=0,UINT first=0,UINT last=0);
	void UnCheck(UINT id=0);
	UINT IsChecked(UINT uid=0);
	
protected:
	UINT m_first,m_last;
};

#endif
