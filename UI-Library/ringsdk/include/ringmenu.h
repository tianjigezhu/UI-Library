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
//版本：0.1
//
//声明：本类库可以自由使用而不须对作者作出任何回报，但作者希望能得到
//		  你的鼓励和支持。你可以对类库源码作出修改和改进，但希望你能在
//		  修改的同时给作者一份同样的副本。
//		  本类库不得用于任何商业用途，如确实需要，请与作者联系。
//
//e-mail:ringphone@sina.com
//
//原文件名：ringmenu.h
//
//本文件说明：ringmenu菜单类定义
//
//类库功能：窗口菜单类定义，实现类似OICQ和OFFICE XP的菜单。
//
**********************************************************************/

#ifndef _INC_RINGMENU
#define _INC_RINGMENU
//#include "include\ringlib.h"

class RingMenu;
class RingCoolMenu;

//以下定义为RingCoolMenu所用
typedef struct _tagRingMenuStyle
{
	HICON hIcon;				//图标
   UINT uID;					//菜单项ID
	RingCoolMenu* pMenu;		//对象指针
	LONG lData;					//菜单项绑定的数据，RingDockSite会设置该值
}RINGMENUSTYLE,*LPRINGMENUSTYLE;

#define MENURECT_UP			BDR_RAISEDINNER
#define MENURECT_DOWN		BDR_SUNKENOUTER

#define MENUMARKER_EXT		24		//标记图案中扩展图案高度，该扩展图案是为MENU高度>标记图案时
											//绘制剩余部分而设，该扩展图案必须位于标记图案顶部并高度为该数值
                                 //资源文件中的标记图案必须符合上述规则
#define MENUSEP_HEIGHT	  	6		//菜单分割线所占菜单项高度
#define MENUCHECK_CHECKBOX	0		//Menu Check图案是CHECKBOX
#define MENUCHECK_RADIO		12    //Menu Check图案是RADIO

typedef struct _tagRingMenuGlobalStyle
{
   HBRUSH hbrBkg;          //背景画刷
   HBITMAP hbmHilight;		//选中菜单图案
	int uHilX,uHilY;			//选中菜单图案宽，高
   HBRUSH hbrHilight;      //选中菜单的颜色
   HPEN hPen;					//画线用（XP式样）
   COLORREF crText;		   //选中菜单的文字颜色
   COLORREF crTextDef;		//未选中菜单颜色（默认颜色）
   UINT uSelectRect;			//选中菜单是凸起或凹下，MENURECT_UP 或 MENURECT_DOWN
	//以下标记图案（即类似OICQ菜单左边的一条图案）信息
   HBITMAP hbmMarker;		//标记图案
   int uMarkerX,uMarkerY;	//标记图案宽，高
   int nMenuHeight;			//菜单总高度
}MENUGLOBALSTYLE,*LPMENUGLOBALSTYLE;

#define BY_COMMAND 		MF_BYCOMMAND			//插入菜单位置指定方式
#define BY_POSITION 		MF_BYPOSITION

#define MAX_MENUTEXT		84							//菜单最多显示字数
#define MENU_PER_INSERT	16							//每次增加菜单项数量(RingCoolMenu用)

#define MENUCOLOR_TRANSPARENT		0xffffffff		//透明色（以下定义菜单用）
#define MENUCOLOR_DEF				0xfffffffe		//默认色
#define MENUCOLOR_NOCHANGE			0xfffffffd		//上一次的颜色（用于SetColor()中不想改变的颜色）

typedef void (*FUNC_COOLMENU_DRAWITEM)(RingCoolMenu*,LPDRAWITEMSTRUCT);
//定义结束

extern "C"
{
   void RingCoolMenu_DrawItem_def(RingCoolMenu* rm,LPDRAWITEMSTRUCT lps);
   void RingCoolMenu_DrawItem_XP(RingCoolMenu* rm,LPDRAWITEMSTRUCT lps);	//Office XP 类型菜单的绘制
	void RingCoolMenu_DrawParent_def(RingCoolMenu* rm,LPDRAWITEMSTRUCT lps);
	void RingCoolMenu_DrawParent_XP(RingCoolMenu* rm,LPDRAWITEMSTRUCT lps);	//Office XP 类型父菜单的绘制
}

class RingMenu
{
public:
   RingMenu();
	RingMenu(LPCTSTR lpMenuname,HINSTANCE hInst=GetInstance());
   virtual ~RingMenu();

   BOOL Create();
	//创建弹出菜单
   BOOL CreatePopup();
	//创建子菜单
	HMENU CreateSubMenu(){return CreatePopupMenu();}
   void Destroy(){DestroyMenu(m_hMenu);};

   virtual BOOL Load(LPCTSTR lpMenuname,HINSTANCE hInst=GetInstance());
   virtual BOOL LoadPopup(LPCTSTR lpMenuname=NULL,UINT nIndex=0,HINSTANCE hInst=GetInstance());
   virtual BOOL Attach(HMENU hMenu);
   virtual HMENU Detach();
   BOOL Popup(HWND hWnd,int x=-999,int y=-999);			//使用默认值则在鼠标当前位置弹出
	//弹出，不覆盖lpRectExclude指定的区域，
	//bHorzAlign：当需要调整位置避开lpRectExclude指定区域时优先考虑横向位置
	BOOL PopupEx(HWND hWnd,int x,int y,BOOL bHorzAlign,LPRECT lpRectExclude);

   HMENU GetMenu(){return m_hCurrMenu;};
   //HMENU GetPopup(){return m_hCurrMenu;};
   int GetItemIndex(UINT uIndex);
   int GetItemId(UINT uIndex);
	int GetItemCount(){return GetMenuItemCount(m_hCurrMenu);}
   HMENU GetItem(UINT uIndex);

   virtual BOOL Insert(HMENU hSubMenu,UINT nIndex,LPSTR szText,UINT flag = BY_POSITION);
	virtual BOOL Insert(UINT uid,UINT nIndex,LPSTR szText,UINT flag = BY_COMMAND);
   virtual BOOL InsertSep(UINT nIndex,UINT uPos=BY_COMMAND);	//插入分隔符
   virtual BOOL Delete(UINT nIndex,UINT uPos=BY_COMMAND);

	UINT GetItemCheckState(UINT uIndex,UINT uFlag = BY_COMMAND);
   virtual DWORD CheckItem(UINT uIndex,BOOL bChecked = TRUE,UINT uFlag = BY_COMMAND);
   virtual BOOL CheckRadioItem(UINT idFirst,UINT idEnd,UINT uIndex,UINT uFlag = BY_POSITION);
   void SetCheckBmp(UINT uIndex,UINT uFlag,HBITMAP hbmCheck,HBITMAP hbmUncheck = NULL);
   void EnableItem(UINT uIndex,BOOL bEnable = TRUE,UINT uFlag = BY_COMMAND);
	BOOL SetDefaultItem(UINT uIndex,UINT uFlag = BY_COMMAND);
	BOOL SetItemText(UINT uIndex,LPCTSTR szText,UINT uFlag = BY_COMMAND);

   BOOL BindToWindow(HWND hWnd){return SetMenu(hWnd,m_hMenu);};
   void Update(HWND hWnd);

protected:
   HMENU m_hMenu,m_hCurrMenu;	//m_hCurrMenu:如果是POPUPMENU，该值是m_hMenu的submenu,否则=m_hMenu
	BOOL bNeedDestroy;			//是否需要销毁MENU
};

#define FUNC_DRAWMENU_DEF	RingCoolMenu_DrawItem_def
#define FUNC_DRAWMENU_XP	RingCoolMenu_DrawItem_XP
#define FUNC_DRAWPARENT_DEF	RingCoolMenu_DrawParent_def
#define FUNC_DRAWPARENT_XP	RingCoolMenu_DrawParent_XP

#define RINGMENU_INITED		1
#define RINGMENU_ISPARENT	2

class RingCoolMenu:public RingMenu
{
public:
	RingCoolMenu();
   RingCoolMenu(LPCTSTR lpMenuname,HINSTANCE hInst=GetInstance());
   RingCoolMenu(HMENU hMenu,BOOL bParent = TRUE);
   ~RingCoolMenu();

   BOOL Load(LPCTSTR lpMenuname,HINSTANCE hInst=GetInstance());
   BOOL Attach(HMENU hMenu);
   HMENU Detach();
   BOOL LoadPopup(LPCTSTR lpMenuname=NULL,UINT nIndex=0,HINSTANCE hInst=GetInstance());
   void Free();			//释放分配的菜单图象（背景，选中图案，版权栏等）

   BOOL Draw(LPDRAWITEMSTRUCT lps);
   BOOL MeasureItem(LPMEASUREITEMSTRUCT pmis);
   BOOL Remap();			//设置菜单（包括子菜单为自绘形式）
	BOOL ReScan();			//重新扫描菜单，设置自绘
   void Reset();        //设置菜单为初始状态（不自绘，释放分配的菜单数据）

   HICON SetItemIcon(UINT nIndex,HICON hicon);	//设置菜单项图标（根据菜单项的ID）
   HICON SetSubMenuIcon(UINT nIndex,HICON hicon);
	BOOL SetBkg(HBITMAP hbmBkg,BOOL bToAllSubMenu=TRUE);
	BOOL SetBkg(UINT idBmp,BOOL bToAllSubMenu=TRUE,HINSTANCE hInst=GetInstance());
	
	void SetBkgColor(COLORREF crBkg,COLORREF crHilight,BOOL bToAllSubMenu=TRUE);
	void SetTextColor(COLORREF crBkgText,COLORREF crSelText,BOOL bToAllSubMenu=TRUE);
   void SetColor(COLORREF crBkg,COLORREF crHilight,COLORREF crBkgText,COLORREF crSelText,BOOL bToAllSubMenu=TRUE);
	
   HBITMAP SetSelectBkg(HBITMAP hbmHilight,COLORREF crText = MENUCOLOR_NOCHANGE,BOOL bToAllSubMenu=TRUE);
   void SetSelectRectType(UINT uRectStyle,BOOL bToAllSubMenu=TRUE);
   HBITMAP SetMarker(HBITMAP hbmMarker);			//设置版权条

   DWORD CheckItem(UINT uIndex,BOOL bChecked = TRUE,UINT uFlag = BY_COMMAND);
   BOOL CheckRadioItem(UINT idFirst,UINT idEnd,UINT uIndex,UINT uFlag = BY_POSITION);

   BOOL Insert(UINT uid,UINT nIndex,LPSTR szText,UINT flag = BY_COMMAND);
   BOOL InsertSep(UINT nIndex,UINT uPos=BY_COMMAND);	//插入分隔符
   BOOL Delete(UINT nIndex,UINT uPos=BY_COMMAND);

	//bAll:是否设置所有子菜单
   void SetItemDrawFunc(FUNC_COOLMENU_DRAWITEM drawer,BOOL bAll=FALSE,FUNC_COOLMENU_DRAWITEM parent_drawer = NULL);	//设置菜单自绘函数
	FUNC_COOLMENU_DRAWITEM GetItemDrawFunc(){return m_DrawItemFunc;}
   
	RingCoolMenu* SubMenu(int i);                         //得到子菜单
	inline int GetSubMenuCount(){return m_nSubMenuCnt;}

	//绘制版权条
	void DrawMark(HDC hDC,HDC hMemDC,LPRECT lprc);
	//绘制分割线
	void DrawSeparator(HDC hDC,LPRECT lprc);
	//绘制背景图案
	void DrawBkg(HDC hDC,LPRECT lprc);
	//绘制图标及选中标志
	int DrawIconCheck(HDC hDC,HDC hMemDC,HICON hIcon,int x,int y,BOOL bSelect,
								BOOL bCheck,HPEN hPen,UINT idMenu);
	//绘制高亮条
	void DrawHilight(HDC hDC,HDC hMemDC,HPEN hPen,LPRECT lprc);
	//绘制文字
	void DrawText(HDC hDC,HDC hMemDC,UINT idMenu,LPRECT lprc,BOOL bGray,BOOL bSelect);

	void SetXPStyle();

private:
   void Init();                                          //类初始化
   void InitSubMenu();                                   //初始化子菜单
   void ResetItemData();                                 //重置菜单项数据（用于增加菜单项菜单数据缓冲区已满的情况）
	BOOL SetCheckItem(UINT uIndex,int nType = MENUCHECK_CHECKBOX);

public:
   static HBITMAP m_hbmCheck,m_hbmRadio;                 //默认菜单项选中图案
	static int m_cyFont;
   int m_ItemIconPos;                                    //菜单项图标位置（与选中图案并列显示或优先显示）
   MENUGLOBALSTYLE m_Style;
	int m_IconCnt;

private:
   static HFONT m_hFont;
   LPRINGMENUSTYLE m_lprms;
   RINGMENUSTYLE m_SepData;			//分隔线数据（公用的ItemData)
   FUNC_COOLMENU_DRAWITEM m_DrawItemFunc;
   static int m_id;
   int m_nSubMenuCnt;
   RingCoolMenu** m_SubMenu;
   //RingCoolMenu* m_Popup;
   int m_nInitFlag;                 //是否已初始化
   int m_nItemCnt;						//菜单项数(非实际数量，分配DATA用)
};

#endif

