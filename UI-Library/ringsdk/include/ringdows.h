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
//原文件名：ringdows.h
//
//
//说明：对窗口及对话框的封装，程序界面库将基于本类库，仅包含本文件并不包含
//      WinMain，必须自己实现。
//
**********************************************************************/
#ifndef _INC_RINGDOWS
#define _INC_RINGDOWS

//#define WINVER	0x0500
#if(!defined(_WIN32_IE) || (defined(_WIN32_IE) && _WIN32_IE < 0x0501))
#define _WIN32_IE 0x0501
#endif

#if (!defined(_WIN32_WINNT) || (_WIN32_WINNT < 0x0400))
#define _WIN32_WINNT 0x0400
#endif

#include "ringlib.h"
#include "ringdowbase.h"
#include "ringcontrols.h"
#include "ringimagelist.h"
#include "ringpropsheet.h"
#include "ringdockbar.h"
#include "ringmenu.h"
#include "ringedit.h"
#include "ringbutton.h"
#include "ringstatic.h"
#include "ringcombo.h"
#include "ringlist.h"
#include "ringchecklist.h"
#include "ringheader.h"
#include "ringlistview.h"
#include "ringoptionview.h"
#include "ringtree.h"
#include "ringtoolbar.h"
#include "ringcoolbar.h"
#include "ringmenubar.h"
#include "ringstatus.h"
#include "ringtab.h"
#include "ringoutlook.h"
#include "ringipaddr.h"
#include "ringtrackbar.h"
#include "ringprogress.h"
#include "ringscrollbar.h"
#include "ringspinbutton.h"
#include "ringtooltip.h"
//#include "ringdataspool.h"

#ifndef MAKE_SELF_LIB
#ifdef _DEBUG
#pragma comment(lib, "ringdowsd.lib")
#else
#pragma comment(lib, "ringdows.lib")
#endif
#pragma comment(lib, "comctl32.lib")
#endif

#endif
