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
//							临风程序界面类库演示
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
//本文件说明：演示编辑框类的使用。
//
//本程序演示：
//		创建窗口
//		创建编辑框
//		设置编辑框为只读，背景色为白色，自适应主窗口尺寸
//		信息类的使用，显示窗口尺寸，设置其输出为编辑框
//
//实现以上功能只需要以下45行代码（带注释），简单吧？
//
**********************************************************************/
#include "ringmain.h"

BEGIN_SETUPMAINWINDOW(IS_WINDOW)	//主窗口是普通窗口
	BEGIN_DEF_VAR(NO_INIT)
		RingEdit m_edit;
		RingMsg m_msg;
	END_DEF_VAR

	BEGIN_DEFMSG
		DEFMSG(WM_CREATE)
		DEFMSG(WM_SIZE)		
	END_DEFMAINMSG
END_SETUPMAINWINDOW

BEGIN_REGMAINMSG
	REGMSG(WM_CREATE)
	REGMSG(WM_SIZE)	
END_REGMSG

int RingMain()
{
	APP_CREATEWINDOW("RingSDK 编辑控件演示","RingSDK_EDIT_DEMO",WS_OVERLAPPEDWINDOW,(LPCTSTR)NULL);
	return RingApp.Run();
}

RINGMAINMSG(WM_CREATE)
{
	//设置EDIT控件的父对象
	m_edit.SetParentObj(this);
	//创建EDIT控件，RTV_FITPARENTSIZE为始终自适应父窗口尺寸
	m_edit.Create(WS_BORDER|ES_MULTILINE|WS_VSCROLL|ES_AUTOHSCROLL|ES_READONLY,
						RTV_FITPARENTSIZE,"",123,0,0,0,0);
	//设置背景色为白色（ES_READONLY类型默认为灰色）
	m_edit.SetBkColor(0x00FFFFFF);
	//设置消息显示到EDIT控件
	m_msg.SetMsgMessage(m_edit.Handle(),EM_REPLACESEL);
	
	return DefaultProc(param);
}

RINGMAINMSG(WM_SIZE)
{
	m_msg.MsgOut("窗口大小：%d * %d",param.mousept.x,param.mousept.y);
	return DefaultProc(param);
}
