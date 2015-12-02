/**********************************************************************
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
//原文件名：ringmenu.cpp
//
//说明：菜单类实现
//
**********************************************************************/
#define MAKE_SELF_LIB
#define OEMRESOURCE
#include "ringdows.h"

////////////////////////////////////////////////
//
//RingMenu
//
//说明：
//		本类为弹出菜单作了专门处理，有时候右键菜单可能要求只是窗口菜单的一个子菜单，
//		因此加了m_hCurrMenu的设定，大多数情况情况下m_hMenu和m_hCurrMenu是相同的，
//		只有有该需求的情况下两个值才会不同且m_hCurrMenu是m_hMenu的子菜单(LoadPopup)
//		所有操作均针对m_hCurrMenu，m_hMenu只为释放而用。
//
////////////////////////////////////////////////
RingMenu::RingMenu()
{
   m_hMenu = m_hCurrMenu = NULL;
   bNeedDestroy = FALSE;
}

RingMenu::RingMenu(LPCTSTR lpMenuname,HINSTANCE hInst/*=GetInstance()*/)
{
   m_hMenu = m_hCurrMenu = NULL;
   Load(lpMenuname,hInst);
}

RingMenu::~RingMenu()
{
   if(m_hMenu && bNeedDestroy)
   	DestroyMenu(m_hMenu);
}

BOOL RingMenu::Load(LPCTSTR lpMenuname,HINSTANCE hInst/*=GetInstance()*/)
{
	if(m_hMenu == NULL)
   {
   	m_hMenu = LoadMenu(hInst,lpMenuname);
      if(m_hMenu)
      {
      	m_hCurrMenu = m_hMenu;
			bNeedDestroy = TRUE;
      	return TRUE;
      }
   }
   return FALSE;
}

BOOL RingMenu::LoadPopup(LPCTSTR lpMenuname,UINT nIndex,HINSTANCE hInst/*=GetInstance()*/)
{	
	if(m_hMenu == NULL)
   {
		m_hMenu = LoadMenu(hInst,lpMenuname);
		if(m_hMenu)
		{
			m_hCurrMenu = GetSubMenu(m_hMenu,nIndex);
			if(m_hCurrMenu == NULL)
				m_hCurrMenu = m_hMenu;
			
			bNeedDestroy = TRUE;
			return TRUE;
		}
	}
	else
	{
		HMENU hTemp = GetSubMenu(m_hMenu,nIndex);
		if(hTemp)
		{
			m_hCurrMenu = hTemp;
			return TRUE;
		}
	}
   return FALSE;
}

BOOL RingMenu::Attach(HMENU hMenu)
{
	if(m_hMenu)
	{
		if(m_hMenu == hMenu)
			return TRUE;
      else
			return FALSE;
	}
	else
	{
		m_hMenu = hMenu;
		m_hCurrMenu = m_hMenu;
		return TRUE;
	}
}

HMENU RingMenu::Detach()
{
   HMENU hTemp= m_hMenu;
	m_hCurrMenu = m_hMenu = NULL;
   return hTemp;
}

BOOL RingMenu::Create()
{
	if(m_hMenu == NULL)
   {
   	m_hMenu = CreateMenu();
      if(m_hMenu)
      {
      	m_hCurrMenu = m_hMenu;
			bNeedDestroy = TRUE;
      	return TRUE;
      }
		else
			return FALSE;
   }
	return TRUE;
}

BOOL RingMenu::CreatePopup()
{
	if(m_hMenu == NULL)
	{
		m_hMenu = CreatePopupMenu();
      if(m_hMenu)
      {
			m_hCurrMenu = m_hMenu;
			bNeedDestroy = TRUE;
			return TRUE;
      }
		else
			return FALSE;
   }
   return TRUE;
}

BOOL RingMenu::Popup(HWND hWnd,int x,int y)
{
	if(m_hCurrMenu != NULL)
   {
      //以下两行语句防止菜单弹出后鼠标在菜单外点击菜单不消失
      SetForegroundWindow(hWnd);
      SetActiveWindow(hWnd);

      if(x == -999 && y == -999)
      {
      	POINT pt;

         GetCursorPos(&pt);
         x = pt.x;
         y = pt.y;
      }
   	TrackPopupMenuEx(m_hCurrMenu,
                         TPM_LEFTALIGN | TPM_TOPALIGN |     // default values
                         TPM_HORIZONTAL | TPM_LEFTBUTTON | // equivalent to 0
                         TPM_RIGHTBUTTON,                   // Right selection
                         x,y,
                         hWnd,
                         NULL);
      return TRUE;
   }
   return FALSE;
}

//弹出，不覆盖lpRectExclude指定的区域，
//bHorzAlign：当需要调整位置避开lpRectExclude指定区域时优先考虑横向位置
BOOL RingMenu::PopupEx(HWND hWnd,int x,int y,BOOL bHorzAlign,LPRECT lpRectExclude)
{
	if(m_hCurrMenu != NULL)
   {
      //以下两行语句防止菜单弹出后鼠标在菜单外点击菜单不消失
      SetForegroundWindow(hWnd);
      SetActiveWindow(hWnd);
		
		UINT uFlag;
		TPMPARAMS param,*lpParam;

		param.cbSize = sizeof(TPMPARAMS);
		if(lpRectExclude)
		{
			CopyRect(&param.rcExclude,lpRectExclude);
			lpParam = &param;
		}
		else
			lpParam = NULL;

		if(bHorzAlign)
			uFlag = TPM_LEFTALIGN | TPM_TOPALIGN | TPM_HORIZONTAL | TPM_LEFTBUTTON | 
						TPM_RIGHTBUTTON;
		else
			uFlag = TPM_LEFTALIGN | TPM_TOPALIGN | TPM_VERTICAL | TPM_LEFTBUTTON | 
						TPM_RIGHTBUTTON;

		if(x == -999 && y == -999)
      {
			POINT pt;
         GetCursorPos(&pt);
         x = pt.x;
         y = pt.y;
      }
      return TrackPopupMenuEx(m_hCurrMenu,uFlag,x,y,hWnd,lpParam);
   }
   return FALSE;
}

//加入子菜单
BOOL RingMenu::Insert(HMENU hSubMenu,UINT nIndex,LPSTR szText,UINT flag)
{
   if(m_hCurrMenu == NULL || hSubMenu == NULL)
     	return FALSE;
	
	MENUITEMINFO mi;
   mi.cbSize = sizeof(mi);
   mi.fMask = MIIM_TYPE | MIIM_SUBMENU;
   mi.hSubMenu = hSubMenu;
	mi.fType = MFT_STRING;
	mi.dwTypeData = szText;
   mi.cch = strlen(szText);

  	return InsertMenuItem(m_hMenu,nIndex,flag,&mi);
}

BOOL RingMenu::Insert(UINT uid,UINT nIndex,LPSTR szText,UINT flag)
{
	if(m_hCurrMenu == NULL)
     	return FALSE;

	MENUITEMINFO mi;
   mi.cbSize = sizeof(mi);
   mi.fMask = MIIM_TYPE | MIIM_ID;
   mi.wID = uid;
   mi.fType = MFT_STRING;
   mi.fState = MFS_DEFAULT;
   mi.dwTypeData = szText;
   mi.cch = strlen(szText);

  	return InsertMenuItem(m_hCurrMenu,nIndex,flag,&mi);
}

BOOL RingMenu::InsertSep(UINT nIndex,UINT flag)
{
	if(m_hCurrMenu == NULL)
     	return FALSE;

	MENUITEMINFO mi;
   mi.cbSize=sizeof(mi);
   mi.fMask=MIIM_TYPE | MIIM_ID;
   mi.fType=MFT_SEPARATOR;
   mi.fState=MFS_DEFAULT;

  	return InsertMenuItem(m_hCurrMenu,nIndex,flag,&mi);
}

BOOL RingMenu::Delete(UINT nIndex,UINT uPos)
{
	if(m_hCurrMenu == NULL)
     	return FALSE;

   return DeleteMenu(m_hCurrMenu,nIndex,uPos);
}

//根据菜单项ID得到序号
int RingMenu::GetItemIndex(UINT uIndex)
{
	MENUITEMINFO mi;
   mi.cbSize=sizeof(mi);
   mi.fMask = MIIM_ID;
   
   int nItem = GetMenuItemCount(m_hCurrMenu);
   while((--nItem)>=0)
   {
   	if(GetMenuItemInfo(m_hCurrMenu,nItem,TRUE,&mi) && mi.wID == uIndex)
			return nItem;
   }
   return -1;
}

//根据菜单项序号得到ID
int RingMenu::GetItemId(UINT uIndex)
{
	MENUITEMINFO mi;
   mi.cbSize=sizeof(mi);
   mi.fMask = MIIM_ID;
   if(!GetMenuItemInfo(m_hCurrMenu,uIndex,TRUE,&mi))
   	return -1;
   else
   	return mi.wID;
}

HMENU RingMenu::GetItem(UINT uIndex)
{
	return GetSubMenu(m_hCurrMenu,uIndex);
}

void RingMenu::Update(HWND hWnd)
{
	DrawMenuBar(hWnd);
}

void RingMenu::SetCheckBmp(UINT uIndex,UINT uFlag,HBITMAP hbmCheck,HBITMAP hbmUncheck)
{
	SetMenuItemBitmaps(m_hCurrMenu,uIndex,uFlag,hbmUncheck,hbmCheck);
}

UINT RingMenu::GetItemCheckState(UINT uIndex,UINT uFlag/*=BY_COMMAND*/)
{
	MENUITEMINFO mi;

	memset(&mi,0,sizeof(MENUITEMINFO));
	mi.cbSize = sizeof(MENUITEMINFO);
	mi.fMask = MIIM_STATE;

	if(GetMenuItemInfo(m_hCurrMenu,uIndex,(uFlag == BY_POSITION),&mi))
		return (mi.fState & MF_CHECKED)?MF_CHECKED:MF_UNCHECKED;
	else
		return MF_ERRORS;
}

DWORD RingMenu::CheckItem(UINT uIndex,BOOL bChecked,UINT uFlag)
{
   UINT flag;
   if(bChecked)
   	flag = MF_CHECKED | uFlag;
   else
   	flag = MF_UNCHECKED | uFlag;

	return CheckMenuItem(m_hCurrMenu,uIndex,flag);
}

BOOL RingMenu::CheckRadioItem(UINT idFirst,UINT idEnd,UINT uIndex,UINT uFlag)
{
	return CheckMenuRadioItem(m_hCurrMenu,idFirst,idEnd,uIndex,uFlag);
}

void RingMenu::EnableItem(UINT uIndex,BOOL bEnable,UINT uFlag)
{
	UINT flag;
   if(bEnable)
   	flag = MF_ENABLED | uFlag;
   else
   	flag = MF_DISABLED | uFlag;

	EnableMenuItem(m_hCurrMenu,uIndex,flag);
}

BOOL RingMenu::SetDefaultItem(UINT uIndex,UINT uFlag/*=BY_COMMAND*/)
{
	return SetMenuDefaultItem(m_hCurrMenu,uIndex,uFlag);
}

BOOL RingMenu::SetItemText(UINT uIndex,LPCTSTR szText,UINT uFlag/*=BY_COMMAND*/)
{
	if(szText)
	{
		BOOL bFlag = (uFlag == BY_POSITION);
		MENUITEMINFO mi;
		memset(&mi,0,sizeof(MENUITEMINFO));
		mi.cbSize=sizeof(mi);
		mi.fMask = MIIM_TYPE;
		if(GetMenuItemInfo(m_hCurrMenu,uIndex,bFlag,&mi))
		{
			//保存原属性
			UINT ftype = mi.fType;
			LPSTR lps = mi.dwTypeData;
			mi.fType = MFT_STRING;
			//mi.fState = MFS_DEFAULT;
			mi.dwTypeData = (LPSTR)szText;
			mi.cch = strlen(szText);

			if(SetMenuItemInfo(m_hCurrMenu,uIndex,bFlag,&mi))
			{
				//设置完成，恢复原属性
				mi.fType = ftype;
				mi.dwTypeData = lps;
				mi.cch = 0;
				return SetMenuItemInfo(m_hCurrMenu,uIndex,bFlag,&mi);
			}
		}
	}
	return FALSE;
}

///////////////////////////////////////////////////
//
//RingCoolMenu
//
//说明：
//		RingCoolMenu初始化时（当Load菜单或Attach时）会初始化所有子菜单的菜单项
//		设置菜单项为自绘形式，为每一个子菜单构造一个对象。
//
//////////////////////////////////////////////////

HBITMAP RingCoolMenu::m_hbmCheck = NULL;
HBITMAP RingCoolMenu::m_hbmRadio = NULL;
int RingCoolMenu::m_id = 0;
int RingCoolMenu::m_cyFont = 0;
HFONT RingCoolMenu::m_hFont = NULL;

RingCoolMenu::RingCoolMenu()
{
	Init();
}

RingCoolMenu::RingCoolMenu(LPCTSTR lpMenuname,HINSTANCE hInst/*=GetInstance()*/)
{
   Init();
	Load(lpMenuname,hInst);
}

//参数bParent由InitSubMenu调用，手工调用除非有目的，否则请使用默认参数
RingCoolMenu::RingCoolMenu(HMENU hMenu,BOOL bParent)
{
   Init();
	Attach(hMenu);
   if(bParent)
		m_nInitFlag = RINGMENU_ISPARENT;
	//Remap();
}

BOOL RingCoolMenu::Load(LPCTSTR lpMenuname,HINSTANCE hInst/*=GetInstance()*/)
{
   if(RingMenu::Load(lpMenuname,hInst))
	{
   	InitSubMenu();
		//m_nInitFlag = RINGMENU_ISPARENT;
		Remap();
	   return TRUE;
   }
	return FALSE;
}

BOOL RingCoolMenu::Attach(HMENU hMenu)
{
   if(m_hMenu)
   {
		if(m_hMenu == hMenu)
      	return TRUE;
      else
	   	return FALSE;
	}
   else
   {
		m_hMenu = m_hCurrMenu = hMenu;
      InitSubMenu();
		Remap();
   }
   return TRUE;
}

HMENU RingCoolMenu::Detach()
{
   if(!m_hMenu)
   	return NULL;

   HMENU hTemp = m_hMenu;
   Reset();
   if(m_lprms != NULL)
   	m_lprms = (LPRINGMENUSTYLE)Del(m_lprms);
   m_hMenu = NULL;
   m_hCurrMenu = NULL;
	m_nInitFlag = 0;
	m_nSubMenuCnt = 0;
	m_IconCnt = 0;
   return hTemp;
}

BOOL RingCoolMenu::LoadPopup(LPCTSTR lpMenuname,UINT nIndex,HINSTANCE hInst)
{
   if(RingMenu::LoadPopup(lpMenuname,nIndex,hInst))
   {
		InitSubMenu();
		Remap();
		return TRUE;
	}
   return FALSE;
}

RingCoolMenu::~RingCoolMenu()
{
   if(m_Style.hbrBkg != (HBRUSH)(COLOR_MENU + 1))
   	DeleteObject(m_Style.hbrBkg);
   if(m_Style.hbrHilight != (HBRUSH)(COLOR_HIGHLIGHT + 1))
   	DeleteObject(m_Style.hbrHilight);
	if(m_Style.hPen)
		DeleteObject(m_Style.hPen);

   m_Style.crText = GetSysColor(COLOR_HIGHLIGHTTEXT);
   m_Style.crTextDef = GetSysColor(COLOR_MENUTEXT);
   m_Style.uSelectRect = 0;
   m_Style.uMarkerX = 0;
   m_Style.uMarkerY = 0;
//   m_Style.uBkgY = 0;
   m_ItemIconPos = 0;

   if(m_lprms != NULL)
   	m_lprms = (LPRINGMENUSTYLE)Del(m_lprms);

   if(m_SubMenu)
   {
   	for(int i=0;i<m_nSubMenuCnt;i++)
      	if(m_SubMenu[i])
         	delete m_SubMenu[i];
      m_SubMenu = (RingCoolMenu**)Del(m_SubMenu);
   }

   m_nInitFlag = 0;
   m_nSubMenuCnt = 0;

   m_id --;
   if(m_id == 0)
   {
   	DeleteObject(m_hbmCheck);
      DeleteObject(m_hbmRadio);
		DeleteObject(m_hFont);
   }
}

void RingCoolMenu::Free()
{
//	if(m_Style.hbmBkg != NULL)
//   	DeleteObject(m_Style.hbmBkg);
   if(m_Style.hbmMarker != NULL)
   	DeleteObject(m_Style.hbmMarker);
   if(m_Style.hbmHilight != NULL)
   	DeleteObject(m_Style.hbmHilight);
   if(bNeedDestroy)
		DestroyMenu(m_hMenu);
}

void RingCoolMenu::Init()
{
   m_nInitFlag = 0;
	m_Style.hPen = NULL;
   m_Style.hbmMarker = NULL;
   m_Style.hbmHilight = NULL;
   m_Style.hbrBkg = (HBRUSH)(COLOR_MENU + 1);
   m_Style.hbrHilight = (HBRUSH)(COLOR_HIGHLIGHT + 1);
   m_Style.crText = GetSysColor(COLOR_HIGHLIGHTTEXT);
   m_Style.crTextDef = GetSysColor(COLOR_MENUTEXT);
   m_Style.uSelectRect = 0;
   m_Style.uMarkerX = 0;
   m_Style.uMarkerY = 0;
	m_Style.uHilX = m_Style.uHilY = 0;

   m_lprms = NULL;
   m_SubMenu = NULL;
   m_nSubMenuCnt = 0;
	m_IconCnt = 0;

//初始化菜单项默认Check图案
   if(m_id == 0)
   {
		HDC hDC = GetDC(NULL);
		NONCLIENTMETRICS nm;
		nm.cbSize = sizeof (NONCLIENTMETRICS);
		SystemParametersInfo(SPI_GETNONCLIENTMETRICS,nm.cbSize,&nm,0);
		m_hFont = CreateFontIndirect(&nm.lfMenuFont);
		
		HFONT hft = (HFONT)SelectObject(hDC,m_hFont);
		SIZE s;
		GetTextExtentPoint32(hDC,"A",1,&s);
		m_cyFont = s.cy;
   	if(m_cyFont < 20)
   		m_cyFont = 20;
		SelectObject(hDC,hft);

   	HBITMAP hbmCheck = LoadBitmap((HINSTANCE) NULL,(LPTSTR)OBM_CHECK);
      HBITMAP hbmRadio = LoadBitmap((HINSTANCE) NULL,(LPTSTR)OBM_BTNCORNERS);
      BITMAP bm;
      HDC hDstDC = CreateCompatibleDC(hDC);
      HDC hSrcDC = CreateCompatibleDC(hDC);
      m_hbmCheck = CreateCompatibleBitmap(hDC,m_cyFont,m_cyFont);
      m_hbmRadio = CreateCompatibleBitmap(hDC,m_cyFont,m_cyFont);

      GetObject(hbmCheck,sizeof(BITMAP),&bm);

      SelectObject(hDstDC,m_hbmCheck);
      SelectObject(hSrcDC,hbmCheck);
      BitBlt(hDstDC,0,0,m_cyFont,m_cyFont,hSrcDC,0,0,WHITENESS);
      BitBlt(hDstDC,(m_cyFont - bm.bmWidth) >> 1,(m_cyFont - bm.bmHeight) >> 1,bm.bmWidth,bm.bmHeight,hSrcDC,0,0,SRCCOPY);

      ZeroMemory(&bm,sizeof(BITMAP));
      GetObject(hbmRadio,sizeof(BITMAP),&bm);

      SelectObject(hDstDC,m_hbmRadio);
      SelectObject(hSrcDC,hbmRadio);
      BitBlt(hDstDC,0,0,m_cyFont,m_cyFont,hSrcDC,0,0,WHITENESS);
      BitBlt(hDstDC,(m_cyFont - bm.bmWidth) >> 1,(m_cyFont - bm.bmHeight) >> 1,bm.bmWidth,bm.bmHeight,hSrcDC,0,0,SRCCOPY);

      DeleteDC(hSrcDC);
      DeleteDC(hDstDC);
      DeleteObject(hbmCheck);
      DeleteObject(hbmRadio);

   	ReleaseDC(NULL,hDC);
   }
   m_id ++;

   m_DrawItemFunc = RingCoolMenu_DrawItem_def;
   m_ItemIconPos = 0;
   m_SepData.hIcon = NULL;
   m_SepData.uID = 0;
   m_SepData.pMenu = this;	
}

void RingCoolMenu::InitSubMenu()
{
	if(m_hCurrMenu && m_nInitFlag == 0)
   {
   	int nItem = GetMenuItemCount(m_hCurrMenu);
      int i,j;
      HMENU hTemp;

      m_nInitFlag = RINGMENU_INITED;

      for(i=0;i<nItem;i++)
      {
      	if(GetSubMenu(m_hCurrMenu,i))
         	m_nSubMenuCnt ++;
      }

      if(m_nSubMenuCnt > 0)
      {
      	m_SubMenu = (RingCoolMenu**)New(m_nSubMenuCnt * sizeof(RingCoolMenu*));
         if(m_SubMenu)
         	for(i=0,j=0;i<nItem;i++)
      		{
               hTemp = GetSubMenu(m_hCurrMenu,i);
		      	if(hTemp)
               {
               	try
                  {
                  	m_SubMenu[j] = new RingCoolMenu(hTemp,FALSE);
                     j++;
                  }
						catch(...)
      				{
                  }
               }
		      }
      }
   }
}

RingCoolMenu* RingCoolMenu::SubMenu(int i)
{
	if(m_SubMenu && i < m_nSubMenuCnt && i >=0)
   	if(m_SubMenu[i])
      	return m_SubMenu[i];
   return this;
}

BOOL RingCoolMenu::Draw(LPDRAWITEMSTRUCT lps)
{
	if(lps->CtlType == ODT_MENU)
   {
      LPRINGMENUSTYLE lprms = (LPRINGMENUSTYLE)lps->itemData;

   	if(lprms)
	   {
		   if(lprms->pMenu == this || lprms->pMenu == NULL)
           	m_DrawItemFunc(this,lps);
		   else
				lprms->pMenu->m_DrawItemFunc(lprms->pMenu,lps);
	   }
   }
   return TRUE;
}

BOOL RingCoolMenu::MeasureItem(LPMEASUREITEMSTRUCT pmis)
{
   if(pmis->CtlType == ODT_MENU)
   {
      LPRINGMENUSTYLE lprms = (LPRINGMENUSTYLE)pmis->itemData;
	   if(lprms)
	   {
		   if(lprms->pMenu && lprms->pMenu != this)
   	   	return lprms->pMenu->MeasureItem(pmis);
	   }
		
	   MENUITEMINFO mi;

      mi.cbSize=sizeof(mi);
	   mi.fMask = MIIM_TYPE;

   	GetMenuItemInfo(m_hCurrMenu,pmis->itemID,FALSE,&mi);

	   if((mi.fType & MFT_SEPARATOR))
   	{
   		pmis->itemWidth = 0;
	      pmis->itemHeight = MENUSEP_HEIGHT;
   	}
	   else
   	{
         char txt[MAX_MENUTEXT];
			memset(txt,0,MAX_MENUTEXT);
			GetMenuString(m_hCurrMenu,pmis->itemID,txt,MAX_MENUTEXT,MF_BYCOMMAND);

			if(!(mi.fType & MFT_BITMAP))
	      {
				HDC hDC = GetDC(NULL);
				HFONT hft = (HFONT)SelectObject(hDC,m_hFont);
				SIZE s;
				GetTextExtentPoint32(hDC,txt,strlen(txt),&s);
				SelectObject(hDC,hft);
				ReleaseDC(NULL,hDC);
				
				if(m_nInitFlag == RINGMENU_ISPARENT)
				{
					pmis->itemWidth = s.cx;
					
					if(lprms->hIcon != NULL)
						pmis->itemWidth += 16;
				}
				else
					pmis->itemWidth = s.cx + 32 + m_Style.uMarkerX + (m_cyFont << 1);
				pmis->itemHeight = m_cyFont;
	      }
   	}
   }
   return TRUE;
}

////////////////////////////////////////////////////////
//
//设置菜单项图标
//
//BUG：CheckItem会检测m_IconCnt，如果有图标则设置m_ItemIconPos
//		 以使XP类型菜单可空出位置绘制并列图标（选中的图标和设置的图标），
//		 但本函数未检测是否有CHECK的菜单项，导致如果菜单资源编辑时
//		 设置有CHECK，调用本函数添加图标，则XP类型会重叠绘制图标。
//
//解决方法：如果在本函数内检测CHECK菜单项，则需要遍历全部菜单项，
//				设置图标比较多的情况下做无用功比较多，特别是无CHECK的
//				菜单，因此这里准备不修改代码，而让调用者设置图标后手工
//				调用CheckItem或CheckRadioItem解决。
//
////////////////////////////////////////////////////////
HICON RingCoolMenu::SetItemIcon(UINT nIndex,HICON hicon)
{
   if(m_lprms == NULL)
		return hicon;

	int nItem = GetMenuItemCount(m_hCurrMenu);
   int i;
   HICON hTemp = hicon;
	
   for(i=0;i<=nItem;i++)
   {
      if(m_lprms[i].uID == nIndex)
	   {
         hTemp = m_lprms[i].hIcon;
      	m_lprms[i].hIcon = hicon;
         m_IconCnt ++;
         break;
      }
   }
   return hTemp;
}

HICON RingCoolMenu::SetSubMenuIcon(UINT nIndex,HICON hicon)
{
   int nItem = GetMenuItemCount(m_hCurrMenu);
   HICON hTemp;
	int i = 0;

   if(m_lprms == NULL)
		return hicon;
	
	RingCoolMenu* tmp = SubMenu(nIndex);
	if(tmp == this)
		return hicon;
		
	MENUITEMINFO mi;	
	mi.cbSize=sizeof(mi);
	mi.fMask = MIIM_SUBMENU | MIIM_TYPE;
	
	HMENU htmp = tmp->m_hCurrMenu;
	//寻找tmp匹配的m_lprms，循环中必须剔除分隔线
   while(--nItem >= 0)
   {
		GetMenuItemInfo(m_hCurrMenu,nItem,TRUE,&mi);
      if(mi.hSubMenu == htmp)
		{
         hTemp = m_lprms[i].hIcon;
			m_lprms[i].hIcon = hicon;
         m_IconCnt ++;
         break;
      }
		if(mi.fType != MFT_SEPARATOR)
			i++;
   }
   return hTemp;
}

DWORD RingCoolMenu::CheckItem(UINT uIndex,BOOL bChecked,UINT uFlag)
{
	UINT id;

   if(uFlag == BY_POSITION)
   	id = GetItemId(uIndex);
   else
   	id = uIndex;

	SetCheckItem(id);
   if(m_IconCnt > 0)
		m_ItemIconPos = m_cyFont;
   return RingMenu::CheckItem(uIndex,bChecked,uFlag);
}

BOOL RingCoolMenu::CheckRadioItem(UINT idFirst,UINT idEnd,UINT uIndex,UINT uFlag)
{
	MENUITEMINFO mi;

   for(UINT i=idFirst;i<=idEnd;i++)
   {
	   ZeroMemory(&mi,sizeof(mi));
   	mi.cbSize=sizeof(mi);
	  	mi.fMask = MIIM_ID | MIIM_DATA;

		GetMenuItemInfo(m_hCurrMenu,i,uFlag,&mi);

	   SetCheckItem(mi.wID,MENUCHECK_RADIO);
   }
   if(m_IconCnt > 0)
		m_ItemIconPos = m_cyFont;
	return CheckMenuRadioItem(m_hCurrMenu,idFirst,idEnd,uIndex,uFlag);
}

BOOL RingCoolMenu::SetCheckItem(UINT uIndex,int nType)
{
	MENUITEMINFO mi;

   ZeroMemory(&mi,sizeof(mi));

   mi.cbSize=sizeof(mi);
   mi.fMask = MIIM_CHECKMARKS;

	GetMenuItemInfo(m_hCurrMenu,uIndex,FALSE,&mi);

   if(!mi.hbmpChecked && !mi.hbmpUnchecked)
      if(nType == MENUCHECK_CHECKBOX)
	      mi.hbmpChecked = m_hbmCheck;
      else
      	mi.hbmpChecked = m_hbmRadio;

   SetMenuItemInfo(m_hCurrMenu,uIndex,FALSE,&mi);

   return TRUE;
}

BOOL RingCoolMenu::SetBkg(HBITMAP hbmBkg,BOOL bToAllSubMenu)
{
   if(hbmBkg)
   {
		if(m_Style.hbrBkg != (HBRUSH)(COLOR_MENU + 1))
			DeleteObject(m_Style.hbrBkg);

		m_Style.hbrBkg = CreatePatternBrush(hbmBkg);

		if(bToAllSubMenu && m_SubMenu)
			for(int i=0;i<m_nSubMenuCnt;i++)
				if(m_SubMenu[i])
					m_SubMenu[i]->SetBkg(hbmBkg);
	}	
   return (BOOL)hbmBkg;
}

BOOL RingCoolMenu::SetBkg(UINT idBmp,BOOL bToAllSubMenu,HINSTANCE hInst/*=GetInstance()*/)
{
	HBITMAP hbm = LoadBitmap(hInst,MAKEINTRESOURCE(idBmp));
	if(SetBkg(hbm,bToAllSubMenu))
	{
		DeleteObject(hbm);
		return TRUE;
	}
	return FALSE;
}

void RingCoolMenu::SetBkgColor(COLORREF crBkg,COLORREF crHilight,BOOL bToAllSubMenu)
{
	if(crBkg != MENUCOLOR_NOCHANGE && crBkg != MENUCOLOR_TRANSPARENT)
   {
		if(m_Style.hbrBkg != (HBRUSH)(COLOR_MENU + 1))
			DeleteObject(m_Style.hbrBkg);
		
      if(crBkg == MENUCOLOR_DEF)
			m_Style.hbrBkg = (HBRUSH)(COLOR_MENU + 1);
      else
			m_Style.hbrBkg = CreateSolidBrush(crBkg);
   }
	
   if(crHilight != MENUCOLOR_NOCHANGE)
   {
		if(m_Style.hbrHilight != (HBRUSH)(COLOR_HIGHLIGHT + 1))
			DeleteObject(m_Style.hbrHilight);
		
		if(crHilight == MENUCOLOR_TRANSPARENT)
			m_Style.hbrHilight = (HBRUSH)GetStockObject(NULL_BRUSH);
		else if(crHilight == MENUCOLOR_DEF)
			m_Style.hbrHilight = (HBRUSH)(COLOR_HIGHLIGHT + 1);
      else
			m_Style.hbrHilight = CreateSolidBrush(crHilight);
   }
	if(bToAllSubMenu && m_SubMenu)
     	for(int i=0;i<m_nSubMenuCnt;i++)
        	if(m_SubMenu[i])
				m_SubMenu[i]->SetBkgColor(crBkg,crHilight);
}

void RingCoolMenu::SetTextColor(COLORREF crText,COLORREF crSelText,BOOL bToAllSubMenu)
{
	if(crText != MENUCOLOR_TRANSPARENT && crText != MENUCOLOR_NOCHANGE)
		if(crText == MENUCOLOR_DEF)
			m_Style.crTextDef = GetSysColor(COLOR_MENUTEXT);
      else
			m_Style.crTextDef = crText;
		
	if(crSelText != MENUCOLOR_TRANSPARENT && crSelText != MENUCOLOR_NOCHANGE)
		if(crSelText == MENUCOLOR_DEF)
			m_Style.crText = GetSysColor(COLOR_HIGHLIGHTTEXT);
		else
			m_Style.crText = crSelText;

	if(bToAllSubMenu && m_SubMenu)
		for(int i=0;i<m_nSubMenuCnt;i++)
			if(m_SubMenu[i])
				m_SubMenu[i]->SetTextColor(crText,crSelText);
}

void RingCoolMenu::SetColor(COLORREF crBkg,COLORREF crHilight,COLORREF crText,COLORREF crSelText,BOOL bToAllSubMenu)
{
	SetBkgColor(crBkg,crHilight,FALSE);
	SetTextColor(crText,crSelText,FALSE);

   if(bToAllSubMenu && m_SubMenu)
     	for(int i=0;i<m_nSubMenuCnt;i++)
        	if(m_SubMenu[i])
         	m_SubMenu[i]->SetColor(crBkg,crHilight,crText,crSelText);
}

HBITMAP RingCoolMenu::SetSelectBkg(HBITMAP hbmHilight,COLORREF crText,BOOL bToAllSubMenu)
{
   HBITMAP hbmtemp = m_Style.hbmHilight;
	m_Style.hbmHilight = hbmHilight;
   
   if(hbmHilight)
	{
		BITMAP bm;
		GetObject(hbmHilight,sizeof(BITMAP),&bm);
		m_Style.uHilX = bm.bmWidth;
		m_Style.uHilY = bm.bmHeight;
	}
	else
		m_Style.uHilX = m_Style.uHilY = 0;
		
   if(crText != MENUCOLOR_TRANSPARENT && crText != MENUCOLOR_NOCHANGE)
   	if(crText == MENUCOLOR_DEF)
      	m_Style.crText = GetSysColor(COLOR_HIGHLIGHTTEXT);
      else
	   	m_Style.crText = crText;

   if(bToAllSubMenu && m_SubMenu)
     	for(int i=0;i<m_nSubMenuCnt;i++)
        	if(m_SubMenu[i])
         	m_SubMenu[i]->SetSelectBkg(hbmHilight,crText);
   return hbmtemp;
}

void RingCoolMenu::SetSelectRectType(UINT uRectStyle,BOOL bToAllSubMenu)
{
   m_Style.uSelectRect = uRectStyle;
   if(bToAllSubMenu && m_SubMenu)
     	for(int i=0;i<m_nSubMenuCnt;i++)
        	if(m_SubMenu[i])
         	m_SubMenu[i]->SetSelectRectType(uRectStyle);
}

HBITMAP RingCoolMenu::SetMarker(HBITMAP hbmMarker)
{
   HBITMAP hbmtemp = m_Style.hbmMarker;
	m_Style.hbmMarker = hbmMarker;

   if(hbmMarker)
   {
   	BITMAP bm;
      GetObject(hbmMarker,sizeof(BITMAP),&bm);
      m_Style.uMarkerX = (UINT)bm.bmWidth;
      m_Style.uMarkerY = (UINT)bm.bmHeight;
   }
   else
   {
   	m_Style.uMarkerX = 0;
      m_Style.uMarkerY = 0;
   }
   return hbmtemp;
}

BOOL RingCoolMenu::Remap()
{
	int nItem = GetMenuItemCount(m_hCurrMenu);
   MENUITEMINFO mi;
   int i = 0;

   if(m_lprms == NULL)
   {
      m_nItemCnt = nItem + 1;
   	m_lprms = (LPRINGMENUSTYLE)New(m_nItemCnt * sizeof(RINGMENUSTYLE));
   	if(m_lprms == NULL)
   		return FALSE;
      for(int i=0;i<=nItem;i++)
      	m_lprms[i].uID = 0xffffffff;
   }
   else
   	return TRUE;

   m_Style.nMenuHeight = 0;

	while((--nItem)>=0)
	{
      memset(&mi,0,sizeof(mi));

   	mi.cbSize = sizeof(mi);
	   mi.fMask = MIIM_DATA | MIIM_ID | MIIM_TYPE | MIIM_STATE | MIIM_CHECKMARKS | MIIM_SUBMENU;

		GetMenuItemInfo(m_hCurrMenu,nItem,TRUE,&mi);

      if((mi.fType & MFT_SEPARATOR))
      {
      	m_Style.nMenuHeight += MENUSEP_HEIGHT;
         mi.dwItemData = (DWORD)&m_SepData;
      }
      else
      {
      	m_Style.nMenuHeight += m_cyFont;
         mi.dwItemData = (DWORD)&m_lprms[i];
      }

     	if(!(mi.fType & MFT_BITMAP))
      {
         m_lprms[i].uID = mi.wID;
         m_lprms[i].pMenu = this;
			
			if((mi.fType & MFT_RADIOCHECK) == 0)
				mi.hbmpChecked = m_hbmCheck;
			else
				mi.hbmpChecked = m_hbmRadio;

         mi.fType |= MFT_OWNERDRAW;
         SetMenuItemInfo(m_hCurrMenu,nItem,TRUE,&mi);
         if(!(mi.fType & MFT_SEPARATOR))
	         i++;
		}
	}
   return TRUE;
}

//////////////////////////////////////////////////////////////
//
//重新扫描菜单项，设置自绘模式，适用于直接调用InsertMenuItem
//而不是Insert的情况，如MDI创建子窗口系统会在菜单中加入子窗口的
//菜单项，但这时系统不知道要调用RingCoolMenu的Insert，因此需要
//将新加入的菜单设置为自绘。
//
///////////////////////////////////////////////////////////////
BOOL RingCoolMenu::ReScan()
{
	int nItem = GetMenuItemCount(m_hCurrMenu);
   MENUITEMINFO mi;
	
   int i=0,j=0;
	
   if(m_lprms == NULL)
   {
      m_nItemCnt = nItem + 1;
		m_lprms = (LPRINGMENUSTYLE)New(m_nItemCnt * sizeof(RINGMENUSTYLE));
		if(m_lprms == NULL)
			return FALSE;
      for(int i=0;i<=nItem;i++)
			m_lprms[i].uID = 0xffffffff;
   }
	else if(nItem >= m_nItemCnt)
		ResetItemData();
	
	m_Style.nMenuHeight = 0;
	
	//mi.cbSize = sizeof(mi);
	//mi.fMask = MIIM_DATA | MIIM_ID | MIIM_TYPE | MIIM_STATE | MIIM_CHECKMARKS;
//char txt[80];
//BOOL b;	
	while((--nItem)>=0)
	{
      memset(&mi,0,sizeof(mi));
		
		mi.cbSize = sizeof(mi);
		mi.fMask = MIIM_DATA | MIIM_ID | MIIM_TYPE | MIIM_STATE | MIIM_CHECKMARKS;

		GetMenuItemInfo(m_hCurrMenu,j,TRUE,&mi);
//memset(txt,0,80);
//GetMenuString(m_hCurrMenu,j,txt,MAX_MENUTEXT,MF_BYPOSITION);	
     	if(!(mi.fType & MFT_OWNERDRAW) && !(mi.fType & MFT_BITMAP))
      {
         if((mi.fType & MFT_SEPARATOR))
			{
				m_Style.nMenuHeight += MENUSEP_HEIGHT;
				mi.dwItemData = (DWORD)&m_SepData;
			}
			else
			{
				m_Style.nMenuHeight += m_cyFont;

				for(;i<m_nItemCnt;i++)
					if(m_lprms[i].uID == 0xffffffff || m_lprms[i].uID == mi.wID)
						break;
				mi.dwItemData = (DWORD)&m_lprms[i];
			}

			m_lprms[i].uID = mi.wID;
         m_lprms[i].pMenu = this;
			
			if((mi.fType & MFT_RADIOCHECK) == 0)
				mi.hbmpChecked = m_hbmCheck;
			else
				mi.hbmpChecked = m_hbmRadio;

         mi.fType |= MFT_OWNERDRAW;
         SetMenuItemInfo(m_hCurrMenu,j,TRUE,&mi);
         if(!(mi.fType & MFT_SEPARATOR))
				i++;
		}
		j++;
	}
	
	if(i > 0)
	{
		memset(m_lprms+i,0,sizeof(RINGMENUSTYLE) * (m_nItemCnt-i));
		for(;i<m_nItemCnt;i++)
			m_lprms[i].uID = 0xffffffff;
	}

   return TRUE;
}

void RingCoolMenu::Reset()
{
	int nItem = GetMenuItemCount(m_hCurrMenu);
   MENUITEMINFO mi;
   char txt[MAX_MENUTEXT];

   m_Style.nMenuHeight = 0;

	while ((--nItem)>=0)
	{
      ZeroMemory(&mi,sizeof(mi));

   	mi.cbSize = sizeof(mi);
	   mi.fMask = MIIM_TYPE | MIIM_DATA | MIIM_STATE | MIIM_CHECKMARKS;
      txt[0] = '\0';
		GetMenuString(m_hCurrMenu,nItem,txt,MAX_MENUTEXT,MF_BYPOSITION);

		GetMenuItemInfo(m_hCurrMenu,nItem,TRUE,&mi);

     	if((mi.fType & MFT_OWNERDRAW))
      {
         mi.fType ^= MFT_OWNERDRAW;
         mi.dwItemData = NULL;
         mi.dwTypeData = txt;
         if(mi.hbmpChecked == m_hbmCheck || mi.hbmpChecked == m_hbmRadio)
         	mi.hbmpChecked = NULL;
         SetMenuItemInfo(m_hCurrMenu,nItem,TRUE,&mi);
		}
	}

   if(m_SubMenu)
     	for(int i=0;i<m_nSubMenuCnt;i++)
        	if(m_SubMenu[i])
         	m_SubMenu[i]->Reset();
}

BOOL RingCoolMenu::Insert(UINT uid,UINT nIndex,LPSTR szText,UINT flag)
{
   if(!m_hCurrMenu)
     	return FALSE;

   if(m_lprms == NULL)
   	Remap();

   MENUITEMINFO mi;

   BOOL bOK;

   mi.cbSize = sizeof(mi);

   mi.fMask = MIIM_TYPE | MIIM_ID;
   mi.wID = uid;
   mi.fType = MFT_STRING;
   mi.fState = MFS_DEFAULT;
   mi.dwTypeData = szText;
   mi.cch = strlen(szText);

  	bOK = InsertMenuItem(m_hCurrMenu,nIndex,flag,&mi);
   if(bOK)
   {
      int nItem = GetMenuItemCount(m_hCurrMenu);
      int i;
      for(i=0;i<=nItem;i++)
      	if(m_lprms[i].uID == 0xffffffff)
         {
         	m_lprms[i].uID = uid;
            m_lprms[i].pMenu = this;
            break;
         }

      mi.fType |= MFT_OWNERDRAW;
      mi.fMask = MIIM_DATA | MIIM_TYPE | MIIM_ID;
      mi.dwItemData = (DWORD)&m_lprms[i];

      SetMenuItemInfo(m_hCurrMenu,uid,FALSE,&mi);
   	m_Style.nMenuHeight += m_cyFont;
      if(i >= m_nItemCnt -1)
      	ResetItemData();
   }
   return bOK;
}

BOOL RingCoolMenu::InsertSep(UINT nIndex,UINT flag)
{
   MENUITEMINFO mi;
   BOOL bOK;

   if(!m_hCurrMenu)
     	return FALSE;

   mi.cbSize=sizeof(mi);

   mi.fMask = MIIM_DATA | MIIM_TYPE | MIIM_ID;
   mi.fType=MFT_SEPARATOR | MFT_OWNERDRAW;
   mi.fState=MFS_DEFAULT;
   mi.dwItemData = (DWORD)&m_SepData;

  	bOK = InsertMenuItem(m_hCurrMenu,nIndex,flag,&mi);
   if(bOK)
   	m_Style.nMenuHeight += MENUSEP_HEIGHT;

   return bOK;
}

//每个菜单项绑定一个RINGMENUSTYLE,当菜单项超出m_lprms长度时，
//重新调整该缓冲区，并重新设置菜单项的itemData
void RingCoolMenu::ResetItemData()
{
   int nItem = GetMenuItemCount(m_hCurrMenu);
   int i = 0;
   m_nItemCnt += MENU_PER_INSERT + 1;
	LPRINGMENUSTYLE temp = (LPRINGMENUSTYLE)New(m_nItemCnt * sizeof(RINGMENUSTYLE));
   LPRINGMENUSTYLE temp1;

   if(temp == NULL)
  		return;

   for(int j=0;j<m_nItemCnt;j++)
   	temp[j].uID = 0xffffffff;

   MENUITEMINFO mi;
   ZeroMemory(&mi,sizeof(mi));

   mi.cbSize = sizeof(mi);
	mi.fMask = MIIM_ID | MIIM_CHECKMARKS | MIIM_STATE |MIIM_TYPE | MIIM_DATA | MIIM_SUBMENU;

   while ((--nItem)>=0)
	{
		GetMenuItemInfo(m_hCurrMenu,nItem,TRUE,&mi);

     	if((mi.fType & MFT_OWNERDRAW) && !(mi.fType & MFT_SEPARATOR))
      {
         temp1 = (LPRINGMENUSTYLE)mi.dwItemData;
         temp[i] = *temp1;
         mi.dwItemData = (DWORD)&temp[i];
         SetMenuItemInfo(m_hCurrMenu,nItem,TRUE,&mi);
         i ++;
		}
	}
   Del(m_lprms);
   m_lprms = temp;
}

BOOL RingCoolMenu::Delete(UINT nIndex,UINT uPos)
{
	MENUITEMINFO mi;
   BOOL bOK;

   ZeroMemory(&mi,sizeof(mi));

 	mi.cbSize = sizeof(mi);
   mi.fMask = MIIM_DATA;

	GetMenuItemInfo(m_hCurrMenu,nIndex,uPos,&mi);

   bOK = RingMenu::Delete(nIndex,uPos);
   if(bOK)
   {
   	if((mi.fType & MFT_SEPARATOR))
     		m_Style.nMenuHeight -= MENUSEP_HEIGHT;
	   else
      {
     		m_Style.nMenuHeight -= m_cyFont;
      	if(mi.dwItemData)
	      {
   	   	LPRINGMENUSTYLE lprms;
      	   lprms = (LPRINGMENUSTYLE)mi.dwItemData;
         	lprms->hIcon = NULL;
	         lprms->uID = 0xffffffff;
   	   }
      }
   }

   return bOK;
}

void RingCoolMenu::SetXPStyle()
{
	SetItemDrawFunc(FUNC_DRAWMENU_XP,TRUE,FUNC_DRAWPARENT_XP);
	SetBkgColor(0x00ffffff,0x00ff8080);
}

void RingCoolMenu::SetItemDrawFunc(FUNC_COOLMENU_DRAWITEM drawer,BOOL bAll/*=FALSE*/,FUNC_COOLMENU_DRAWITEM parent_drawer/*=NULL*/)
{
	if(drawer)
   	m_DrawItemFunc = drawer;

	if(m_nInitFlag == RINGMENU_ISPARENT && parent_drawer)
		m_DrawItemFunc = parent_drawer;

	if(bAll && m_SubMenu)
		for(int i=0;i<m_nSubMenuCnt;i++)
			if(m_SubMenu[i])
				m_SubMenu[i]->SetItemDrawFunc(drawer,TRUE);	
}

//绘制版权条
void RingCoolMenu::DrawMark(HDC hDC,HDC hMemDC,LPRECT lprc)
{
	if(m_Style.hbmMarker != NULL)
	{
		SelectObject(hMemDC,m_Style.hbmMarker);
		if((m_Style.nMenuHeight - lprc->top) > (int)m_Style.uMarkerY)
			BitBlt(hDC,lprc->left,lprc->top,m_Style.uMarkerX,lprc->bottom - lprc->top,
					hMemDC,0,0,SRCCOPY);
		else
			BitBlt(hDC,lprc->left,lprc->top,m_Style.uMarkerX,lprc->bottom - lprc->top,
					hMemDC,0,m_Style.uMarkerY - m_Style.nMenuHeight + lprc->top,SRCCOPY);
	}
}

//绘制分隔线
void RingCoolMenu::DrawSeparator(HDC hDC,LPRECT lprc)
{
	if(lprc)
	{
		lprc->top += 2;
		lprc->bottom -= 2;
		lprc->left += 1;
		lprc->right -= 1;
		DrawEdge(hDC,lprc,BDR_SUNKENOUTER,BF_RECT);
	}
}

//绘制背景图案
void RingCoolMenu::DrawBkg(HDC hDC,LPRECT lprc)
{
	if(lprc)
	{
		SetBrushOrgEx(hDC,m_Style.uMarkerX,0,NULL);
		FillRect(hDC,lprc,m_Style.hbrBkg);
	}
}

//绘制图标及选中符号，XP菜单将符号及图标并排绘制，其他类型则合并绘制，
//即有图标绘制图标，以突起或凹下矩形区分。hPen标记是否XP菜单
int RingCoolMenu::DrawIconCheck(HDC hDC,HDC hMemDC,HICON hIcon,int x,int y,BOOL bSelect,
											BOOL bCheck,HPEN hPen,UINT idMenu)
{	
	int ret = 0,ext = (m_cyFont - 16) >> 1;
	MENUITEMINFO mi;
	RECT rc = {x,y,x + m_cyFont,y + m_cyFont};

	ZeroMemory(&mi,sizeof(mi));
	mi.cbSize=sizeof(mi);
	mi.fMask = MIIM_CHECKMARKS;
	GetMenuItemInfo(m_hCurrMenu,idMenu,FALSE,&mi);

	if(bCheck)
	{		
		//绘制选中图案
		if(hPen)
		{
			//XP菜单，用hPen画一框
			if(bSelect)
				SelectObject(hDC,m_Style.hbrHilight);
			else
				SelectObject(hDC,(HBRUSH)GetStockObject(NULL_BRUSH));

			SelectObject(hDC,hPen);
			Rectangle(hDC,x+1,y+1,x + m_cyFont-1,y + m_cyFont-1);
		}
		else
		{
			if(mi.hbmpChecked != m_hbmRadio)
			   DrawEdge(hDC,&rc,BDR_SUNKENOUTER,BF_RECT);
		}
		if(hPen || hIcon == NULL)
		{
			SelectObject(hMemDC,mi.hbmpChecked);
			BitBlt(hDC,x,y,m_cyFont,m_cyFont,hMemDC,0,0,SRCAND);
			if(hPen)
				x += m_cyFont;
		}
	}
	else
	{
		if(hPen == NULL && hIcon && bSelect)
			DrawEdge(hDC,&rc,BDR_RAISEDINNER,BF_RECT);

		if(mi.hbmpUnchecked && (hPen || hIcon == NULL))
		{
			SelectObject(hMemDC,mi.hbmpUnchecked);
			BitBlt(hDC,x,y,m_cyFont,m_cyFont,hMemDC,0,0,SRCAND);
		}
	}
   
	if(hPen)
	{
		if(bSelect)
		{
			x = m_ItemIconPos + ext - 1;
			y += ext -1;
		}
		else
		{
			x = m_ItemIconPos + ext;
			y += ext;
		}
	}
	else
	{
		x += ext;
		y += ext;
	}

	if(hIcon)
		DrawIconEx(hDC,x,y,hIcon,16,16,0,NULL,DI_NORMAL);

	if(hPen)
		ret = m_cyFont + m_ItemIconPos;
	else if(hIcon || bCheck || mi.hbmpUnchecked)
		ret += m_cyFont;
	return ret;
}

//绘制高亮条
void RingCoolMenu::DrawHilight(HDC hDC,HDC hMemDC,HPEN hPen,LPRECT lprc)
{
	if(m_Style.hbmHilight)
	{
		SelectObject(hMemDC,m_Style.hbmHilight);
		StretchBlt(hDC,lprc->left,lprc->top,lprc->right - lprc->left,
						lprc->bottom - lprc->top,hMemDC,
						0,0,m_Style.uHilX,m_Style.uHilY,SRCCOPY);
		//BitBlt(hDC,lprc->left,lprc->top,lprc->right - lprc->left,
		//		lprc->bottom - lprc->top,hMemDC,lprc->left,0,SRCCOPY);
	}
	else
	{
		if(hPen)
		{
			SelectObject(hDC,m_Style.hbrHilight);
			SelectObject(hDC,hPen);
			Rectangle(hDC,lprc->left,lprc->top,lprc->right,lprc->bottom);
		}
		else
			FillRect(hDC,lprc,m_Style.hbrHilight);
	}
			
	if(m_Style.uSelectRect)
		DrawEdge(hDC,lprc,m_Style.uSelectRect,BF_RECT);
}

//绘制菜单文字
void RingCoolMenu::DrawText(HDC hDC,HDC hMemDC,UINT idMenu,LPRECT lprc,BOOL bGray,BOOL bSelect)
{
	char txt[MAX_MENUTEXT];
	memset(txt,0,MAX_MENUTEXT);
	GetMenuString(m_hCurrMenu,idMenu,txt,MAX_MENUTEXT,MF_BYCOMMAND);

	LPSTR lpc = txt;
	int n = 0;
	while(*lpc != '\t' && *lpc != '\0')
		lpc ++;
	if(*lpc == '\t')
	{
		*lpc = '\0';
		n = lpc - txt;
	}

	if(bGray)
     	::SetTextColor(hDC,GetSysColor(COLOR_GRAYTEXT));
	else if(bSelect)
		::SetTextColor(hDC,m_Style.crText);
	else
		::SetTextColor(hDC,m_Style.crTextDef);

   if(n > 0)
	{
		::DrawText(hDC,txt,-1,lprc,DT_LEFT|DT_VCENTER|DT_SINGLELINE);
		*lpc = '\t';
		while(*lpc == '\t')
			lpc ++;
		if(*lpc != '\0')
			::DrawText(hDC,lpc,-1,lprc,DT_RIGHT|DT_VCENTER|DT_SINGLELINE);
	}
	else
		::DrawText(hDC,txt,-1,lprc,DT_LEFT|DT_VCENTER|DT_SINGLELINE|DT_EXPANDTABS);   
}

//默认自绘菜单函数
void RingCoolMenu_DrawItem_def(RingCoolMenu* rm,LPDRAWITEMSTRUCT lps)
{
	RECT rc = {lps->rcItem.left,lps->rcItem.top,lps->rcItem.right,lps->rcItem.bottom};
	HDC hMemDC = CreateCompatibleDC(lps->hDC);

	//绘制版权条
	rm->DrawMark(lps->hDC,hMemDC,&lps->rcItem);
	//绘制背景图象
   rc.left += rm->m_Style.uMarkerX;
	rm->DrawBkg(lps->hDC,&rc);
	//判断是否分隔线
	if((rc.bottom - rc.top) == MENUSEP_HEIGHT)
	{
		//绘制分隔线
		rm->DrawSeparator(lps->hDC,&rc);
	}
	else
	{
		LPRINGMENUSTYLE lprms = (LPRINGMENUSTYLE)lps->itemData;
		BOOL bSel = (lps->itemState & ODS_SELECTED);
		BOOL bChecked = (lps->itemState & ODS_CHECKED);
		int ext = 0;

		SetBkMode(lps->hDC,TRANSPARENT);

		if(lprms)
		{
			ext = rm->DrawIconCheck(lps->hDC,hMemDC,lprms->hIcon,rc.left,rc.top,bSel,
									bChecked,NULL,lps->itemID);
			rc.left += ext;
		}
		//绘制高亮条
		if(bSel)
			rm->DrawHilight(lps->hDC,hMemDC,NULL,&rc);

		if(ext == 0)
			rc.left += rm->m_cyFont + 2;
		else
			rc.left += 2;
		
		rc.right -= 20;

		//绘制文字
		rm->DrawText(lps->hDC,hMemDC,lps->itemID,&rc,
			((lps->itemState & ODS_DISABLED) || (lps->itemState & ODS_GRAYED)),bSel);
	}
   DeleteDC(hMemDC);
}

void RingCoolMenu_DrawItem_XP(RingCoolMenu* rm,LPDRAWITEMSTRUCT lps)
{	
   RECT rc = {lps->rcItem.left,lps->rcItem.top,lps->rcItem.right,lps->rcItem.bottom};
	HDC hMemDC = CreateCompatibleDC(lps->hDC);
	
	//绘制版权条
	rm->DrawMark(lps->hDC,hMemDC,&lps->rcItem);
	//绘制背景图象
   rc.left += rm->m_Style.uMarkerX;
	rm->DrawBkg(lps->hDC,&rc);

	RECT rrc = {rc.left,rc.top,rc.left + rm->m_cyFont + rm->m_ItemIconPos + 2,rc.bottom};
	FillRect(lps->hDC,&rrc,(HBRUSH)(COLOR_BTNFACE+1));

	//判断是否分隔线
	if((rc.bottom - rc.top) == MENUSEP_HEIGHT)
	{
		//绘制分隔线
		rc.left += rm->m_cyFont + rm->m_ItemIconPos + 2;
      rm->DrawSeparator(lps->hDC,&rc);
	}
	else
	{
		LPRINGMENUSTYLE lprms = (LPRINGMENUSTYLE)lps->itemData;
		BOOL bSel = (lps->itemState & ODS_SELECTED);
		BOOL bChecked = (lps->itemState & ODS_CHECKED);
		int ext = 0;

		SetBkMode(lps->hDC,TRANSPARENT);
		
		if(!rm->m_Style.hPen)
			rm->m_Style.hPen = CreatePen(PS_SOLID,1,0x00ff0000);

		//绘制高亮条
		if(bSel)
			rm->DrawHilight(lps->hDC,hMemDC,rm->m_Style.hPen,&rc);
		
		if(lprms)
		{
			ext = rm->DrawIconCheck(lps->hDC,hMemDC,lprms->hIcon,rc.left,rc.top,bSel,
													bChecked,rm->m_Style.hPen,lps->itemID);
			rc.left += ext;
		}
		rc.left += 5;
		rc.right -= 20;
		
		//绘制文字
		rm->DrawText(lps->hDC,hMemDC,lps->itemID,&rc,
			((lps->itemState & ODS_DISABLED) || (lps->itemState & ODS_GRAYED)),bSel);
	}
   DeleteDC(hMemDC);
}

void RingCoolMenu_DrawParent_XP(RingCoolMenu* rm,LPDRAWITEMSTRUCT lps)
{
	HDC hMemDC = CreateCompatibleDC(lps->hDC);
   RECT rc = {lps->rcItem.left,lps->rcItem.top,lps->rcItem.right,lps->rcItem.bottom};
	BOOL bSel = ((lps->itemState & ODS_SELECTED) ||(lps->itemState & 0x40/*ODS_HOTLIGHT*/));

   SetBkMode(lps->hDC,TRANSPARENT);	
   if(bSel)
   {
     	if(!rm->m_Style.hPen)
			rm->m_Style.hPen = CreatePen(PS_SOLID,1,0x00ff0000);

		rm->DrawHilight(lps->hDC,hMemDC,rm->m_Style.hPen,&lps->rcItem);
   }
	else
		FillRect(lps->hDC,&lps->rcItem,(HBRUSH)(COLOR_BTNFACE + 1));

   LPRINGMENUSTYLE lprms = (LPRINGMENUSTYLE)lps->itemData;   	
   
	if(lprms)
   {
      if(lprms->hIcon)
      {
         int ext = (rm->m_cyFont - 16) >> 1;			
         if(bSel)
				DrawIconEx(lps->hDC,rc.left + ext - 1,rc.top + ext - 1,lprms->hIcon,16,16,0,NULL,DI_NORMAL);
         else
				DrawIconEx(lps->hDC,rc.left + ext,rc.top + ext,lprms->hIcon,16,16,0,NULL,DI_NORMAL);
			rc.left += rm->m_cyFont - 5;
      }
   }	
	rc.left += 5;
	
   rm->DrawText(lps->hDC,hMemDC,lps->itemID,&rc,
					((lps->itemState & ODS_DISABLED) || (lps->itemState & ODS_GRAYED)),bSel);
   DeleteDC(hMemDC);
}

void RingCoolMenu_DrawParent_def(RingCoolMenu* rm,LPDRAWITEMSTRUCT lps)
{
	HDC hMemDC = CreateCompatibleDC(lps->hDC);
   RECT rc = {lps->rcItem.left,lps->rcItem.top,lps->rcItem.right,lps->rcItem.bottom};
	BOOL bSel = ((lps->itemState & ODS_SELECTED) ||(lps->itemState & 0x40/*ODS_HOTLIGHT*/));
	
   SetBkMode(lps->hDC,TRANSPARENT);

	FillRect(lps->hDC,&rc,(HBRUSH)(COLOR_BTNFACE + 1));

	if((lps->itemState & 0x40/*ODS_HOTLIGHT*/))
		DrawEdge(lps->hDC,&rc,BDR_RAISEDINNER,BF_RECT);
	else if((lps->itemState & ODS_SELECTED))
   	DrawEdge(lps->hDC,&rc,BDR_SUNKENOUTER,BF_RECT);		
   
   LPRINGMENUSTYLE lprms = (LPRINGMENUSTYLE)lps->itemData;   	
   
	if(lprms)
   {
      if(lprms->hIcon)
      {
         int ext = (rm->m_cyFont - 16) >> 1;			
         if(bSel)
				DrawIconEx(lps->hDC,rc.left + ext - 1,rc.top + ext - 1,lprms->hIcon,16,16,0,NULL,DI_NORMAL);
         else
				DrawIconEx(lps->hDC,rc.left + ext,rc.top + ext,lprms->hIcon,16,16,0,NULL,DI_NORMAL);
			rc.left += rm->m_cyFont - 5;
      }
   }	
	rc.left += 5;
	if((lps->itemState & ODS_SELECTED))
	{
		rc.left ++;
		rc.top ++;
	}
	//最后的参数bSel恒为FALSE，不需要改变字体颜色
   rm->DrawText(lps->hDC,hMemDC,lps->itemID,&rc,
		((lps->itemState & ODS_DISABLED) || (lps->itemState & ODS_GRAYED)),FALSE);
   DeleteDC(hMemDC);
}
