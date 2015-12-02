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
//							RingSDK类库 不同语言字符串定义
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
//原文件名：lang_string.h
//
//说明：不同语言字符串定义。
//
**********************************************************************/
#ifndef _INC_LANGUAE_STRING_HEADER
#define _INC_LANGUAE_STRING_HEADER

#if defined(RINGSDK_VER_ENGLISH)
//English strings
//for RingComm:
#define LANSTR_COMM_OPENFAIL	"Open communications device[%s] fail - "
#define LANSTR_COMM_EVENTFAIL		"CreateEvent for communications device fail - "
#define LANSTR_COMM_EVENTSETFAIL	"SetCommMask fail - "
#define LANSTR_COMM_GETTIMEOUTFAIL	"GetCommTimeouts fail - "
#define LANSTR_COMM_SETTIMEOUTFAIL	"SetCommTimeouts fail - "
#define LANSTR_COMM_CLEARERRORFAIL	"ClearCommError fail - "
#define LANSTR_COMM_GETDCBFAIL	"Unable to obtain DCB information - "
#define LANSTR_COMM_SETDCBFAIL	"Unable to set DCB information - "
#define LANSTR_COMM_WRITETIMEOUT	"Write data timeout - "

#define LANSTR_WRITEFILEERROR	"WriteFile error - "

#define LANSTR_FILEOPEN "Open"
#define LANSTR_SELDIR "Choose a folder"
////界面库字符串
#define LANSTR_MENUBARNAME	"Menu bar"

//#elif defined()
//other languages string define here
//...
#else
//默认中文
//RingComm用到的字符串：
#define LANSTR_COMM_OPENFAIL	"打开串口[%s]失败 - "
#define LANSTR_COMM_EVENTFAIL		"创建串口监视事件失败 - "
#define LANSTR_COMM_EVENTSETFAIL	"设置串口事件失败 - "
#define LANSTR_COMM_GETTIMEOUTFAIL	"获取串口超时设置失败 - "
#define LANSTR_COMM_SETTIMEOUTFAIL	"设置串口超时失败 - "
#define LANSTR_COMM_CLEARERRORFAIL	"ClearCommError失败 - "
#define LANSTR_COMM_GETDCBFAIL	"获取DCB信息失败 - "
#define LANSTR_COMM_SETDCBFAIL	"设置DCB信息失败 - "
#define LANSTR_COMM_WRITETIMEOUT	"写入数据超时 - "

#define LANSTR_WRITEFILEERROR	"写入文件失败 - "

#define LANSTR_FILEOPEN "打开"
#define LANSTR_SELDIR "选择路径"

//界面库字符串
#define LANSTR_MENUBARNAME	"菜单栏"

#endif

#endif
