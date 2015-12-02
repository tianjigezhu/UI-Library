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
//							RingSDK演示程序
//作者：临风
//
//e-mail:ringphone@sina.com
//
//原文件：coolmenu.exe
//
//演示类OICQ，WindowsXP风格的菜单
//
**********************************************************************/
#include "ringmain.h"
#include "resource.h"

BEGIN_SETUPMAINWINDOW(IS_WINDOW)
	BEGIN_DEF_VAR(NO_INIT)
		RingCoolMenu *rm,*rmXP;
		int count;
		HBITMAP hbmMarker,hbmBkg,hbmHilight,hbmTemp;
		HICON hicoC,hicoD;
		BOOL bInShowBkg,bShowRect,bPopupXP;
		COLORREF crText;
	END_DEF_VAR

	BEGIN_DEFMSG
		DEFMSG(WM_DESTROY)
		DEFMSG(WM_CREATE)
		DEFMSG(WM_RBUTTONDOWN)
		DEFCMD(OnShowMarker)
		DEFCMD(OnShowBkgBmp)
		DEFCMD(OnShowBkgColor)
		DEFCMD(OnShowSelBmp)
		DEFCMD(OnShowIcon)
		DEFCMD(OnAddItem)
		DEFCMD(OnDelItem)
		DEFCMD(OnExit)
	END_DEFMAINMSG
END_SETUPMAINWINDOW

BEGIN_REGMAINMSG	
	REGCMD(CM_SHOWCOPYRIGHT,OnShowMarker)
	REGCMD(CM_SHOWBKG,OnShowBkgBmp)
	REGCMD(CM_SHOWBKGCOLOR,OnShowBkgColor)
	REGCMD(CM_SHOWSELECTBKG,OnShowSelBmp)
	REGCMD(CM_SHOWICON,OnShowIcon)
	REGCMD(CM_ADDMENUITEM,OnAddItem)
	REGCMD(CM_DELMENUITEM,OnDelItem)
	REGMSG(WM_RBUTTONDOWN)
	REGCMD(CM_EXIT,OnExit)
	REGMSG(WM_DESTROY)
	REGMSG(WM_CREATE)
END_REGMSG

int RingMain()
{
	APP_CREATEWINDOW("RingSDK菜单演示","RingSDKApp",WS_OVERLAPPEDWINDOW,MAKEINTRESOURCE(IDM_MENU));
   return RingApp.Run();
}

RINGMAINMSG(WM_DESTROY)
{
	if(hbmMarker == NULL)
		hbmMarker = rm->SubMenu(0)->SetMarker(NULL);
	if(hbmMarker)
		DeleteObject(hbmMarker);
	if(hbmHilight == NULL)
		hbmHilight = rm->SetSelectBkg(NULL);
	if(hbmHilight)
		DeleteObject(hbmHilight);
	DeleteObject(hbmBkg);

	delete rmXP;
	return Quit();
}

RINGMAINMSG(WM_RBUTTONDOWN)
{
	rm->SubMenu(0)->Popup(hWnd);
	return 0;
}

RINGMAINCMD(OnExit)
{
	SendExit();   
}

RINGMAINMSG(WM_CREATE)
{
	//获取菜单对象
	rm = InstallCoolMenu();
	//设置顶级菜单为XP式样绘制
   rm->SetItemDrawFunc(FUNC_DRAWMENU_DEF,TRUE,FUNC_DRAWPARENT_XP);
	//设置菜单前景，背景色
	rm->SetBkgColor(GetSysColor(COLOR_HIGHLIGHT),0x00ff8080);
	
	//设置版权条
	hbmMarker = rm->SubMenu(0)->SetMarker(LoadBitmap(GetInstance(),MAKEINTRESOURCE(IDB_BITMAP2)));
   hbmBkg = LoadBitmap(GetInstance(),MAKEINTRESOURCE(IDB_BITMAP1));
	rm->SubMenu(0)->SetBkg(hbmBkg,FALSE);
   hicoC = rm->SubMenu(0)->SetItemIcon(CM_SHOWCOPYRIGHT,LoadIcon(GetInstance(),MAKEINTRESOURCE(IDI_ICON1)));
   hicoD = rm->SubMenu(0)->SetItemIcon(CM_EXIT,LoadIcon(GetInstance(),MAKEINTRESOURCE(IDI_ICON2)));
   rm->SubMenu(0)->CheckItem(CM_SHOWCOPYRIGHT);
   rm->SubMenu(0)->CheckRadioItem(1,2,1);
   rm->SubMenu(0)->CheckItem(CM_SHOWICON);
   rm->SubMenu(0)->CheckItem(CM_SHOWRECT);
   rm->SubMenu(0)->SetSelectRectType(MENURECT_DOWN);
	
	rm->SubMenu(0)->SubMenu(0)->SetColor(0x00a8a83d,MENUCOLOR_DEF,MENUCOLOR_DEF,MENUCOLOR_DEF);
	rm->SubMenu(0)->SubMenu(0)->SetBkg(IDB_TYJX);
	
	rm->SubMenu(1)->SetBkg(IDB_ZBZ);
	
	rm->SubMenu(2)->SetItemDrawFunc(FUNC_DRAWMENU_XP);
	rm->SubMenu(2)->SetItemIcon(CM_POPUPITEM1,LoadIcon(GetInstance(),MAKEINTRESOURCE(IDI_ICON1)));
	rm->SubMenu(2)->SetItemIcon(CM_POPUPITEM2,LoadIcon(GetInstance(),MAKEINTRESOURCE(IDI_ICON2)));
	rm->SubMenu(2)->SetItemIcon(CM_POPUPITEM3,LoadIcon(GetInstance(),MAKEINTRESOURCE(IDI_ICON3)));
	rm->SubMenu(2)->SetItemIcon(CM_POPUPITEM4,LoadIcon(GetInstance(),MAKEINTRESOURCE(IDI_ICON4)));
	rm->SubMenu(2)->SetItemIcon(CM_POPUPITEM5,LoadIcon(GetInstance(),MAKEINTRESOURCE(IDI_ICON5)));
	rm->SubMenu(2)->SetItemIcon(CM_POPUPITEM6,LoadIcon(GetInstance(),MAKEINTRESOURCE(IDI_ICON6)));
	rm->SubMenu(2)->SetBkgColor(0x00ffffff,0x00ff8080);
	rm->SubMenu(2)->CheckItem(CM_POPUPITEM1);
	rm->SubMenu(2)->CheckRadioItem(2,3,2);
	
   hbmHilight = LoadBitmap(GetInstance(),MAKEINTRESOURCE(IDB_BITMAP3));
	
	rmXP = new RingCoolMenu;
   rmXP->LoadPopup(MAKEINTRESOURCE(IDM_XP));
   rmXP->SetItemIcon(CM_POPUPITEM1,LoadIcon(GetInstance(),MAKEINTRESOURCE(IDI_ICON1)));
   rmXP->SetItemIcon(CM_POPUPITEM2,LoadIcon(GetInstance(),MAKEINTRESOURCE(IDI_ICON2)));
   rmXP->SetItemIcon(CM_POPUPITEM3,LoadIcon(GetInstance(),MAKEINTRESOURCE(IDI_ICON3)));
   rmXP->SetItemIcon(CM_POPUPITEM4,LoadIcon(GetInstance(),MAKEINTRESOURCE(IDI_ICON4)));
	rmXP->SetItemIcon(CM_POPUPITEM5,LoadIcon(GetInstance(),MAKEINTRESOURCE(IDI_ICON5)));
   rmXP->SetItemIcon(CM_POPUPITEM6,LoadIcon(GetInstance(),MAKEINTRESOURCE(IDI_ICON6)));
   rmXP->SetBkgColor(0x00ffffff,0x00ff8080);
   rmXP->SetItemDrawFunc(FUNC_DRAWMENU_XP);

	count = 0;
	return 0;
}

RINGMAINCMD(OnShowMarker)
{
	hbmMarker = rm->SubMenu(0)->SetMarker(hbmMarker);
	rm->CheckItem(CM_SHOWCOPYRIGHT,(BOOL)!hbmMarker);
}

RINGMAINCMD(OnShowBkgBmp)
{
	if(rm->SubMenu(0)->GetItemCheckState(CM_SHOWBKG) != MF_CHECKED)
	{
		rm->SubMenu(0)->SetBkg(hbmBkg,FALSE);
		rm->SubMenu(0)->CheckRadioItem(1,2,1);
	}
}

RINGMAINCMD(OnShowBkgColor)
{
	if(rm->SubMenu(0)->GetItemCheckState(CM_SHOWBKGCOLOR) != MF_CHECKED)
	{
		rm->SubMenu(0)->SetBkgColor(0x00a8a83d,MENUCOLOR_NOCHANGE,FALSE);
		rm->SubMenu(0)->CheckRadioItem(1,2,2);
	}
}

RINGMAINCMD(OnShowSelBmp)
{
	hbmHilight = rm->SetSelectBkg(hbmHilight);
	rm->CheckItem(CM_SHOWSELECTBKG,!hbmHilight);
}

RINGMAINCMD(OnShowIcon)
{
	hicoC = rm->SubMenu(0)->SetItemIcon(CM_SHOWCOPYRIGHT,hicoC);
	hicoD = rm->SubMenu(0)->SetItemIcon(CM_EXIT,hicoD);
	rm->CheckItem(CM_SHOWICON,(BOOL)!hicoC);
}

RINGMAINCMD(OnAddItem)
{
	char x[16];
	count ++;
	wsprintf(x,"测试菜单[%d]",count);
	rm->SubMenu(0)->Insert(count,6 + count,x,BY_POSITION);
}

RINGMAINCMD(OnDelItem)
{
	count --;
	if(count >= 0)
		rm->SubMenu(0)->Delete(7 + count,BY_POSITION);
	else
		count = 0;
}