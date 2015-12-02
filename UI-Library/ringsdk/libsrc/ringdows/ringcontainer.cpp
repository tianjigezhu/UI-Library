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
//							RingSDK类库 ringmain.lib
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
//原文件名：ringmain.h
//
//说明：对WinMain的封装，程序界面库将基于本类库，包含本文件无须实现
//      WinMain，主函数为RingMain，必须自己实现。
//
**********************************************************************/
#define MAKE_SELF_LIB
//#ifndef __BORLANDC__
#include "ringcontainer.h"
//#else
//#include "include\ringmain.h"
//#endif

// Required ATL module
CComModule _Module;

#include <atlimpl.cpp>

void InitRingContainer()
{
	CoInitialize(0);
	//CoInitializeEx(NULL,COINIT_MULTITHREADED);
    // Initialize the ATL module
   _Module.Init(0, GetInstance());
    // Initialize support for control containment
   AtlAxWinInit();
}

int FreeRingContainer()
{
	_Module.Term();    
    // Uninitialize COM
   CoUninitialize();
	return 0;
}

