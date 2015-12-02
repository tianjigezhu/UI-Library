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
//本文件说明：演示按钮类的使用。
//
**********************************************************************/
#include "ringmain.h"
#include "resource.h"

BEGIN_SETUPMAINWINDOW(IS_DIALOG)	//主窗口是对话框
	BEGIN_DEF_VAR(NO_INIT)
		RingButton m_btIcon,m_btBmp,m_btCust,m_btDef;
		RingButton* m_lpbtn;
	END_DEF_VAR

	BEGIN_DEFMSG
		DEFMSG(WM_INITDIALOG)
		DEFCMD(OnBtBmpClick)
		DEFCMD(OnAlignClick)
		DEFCMD(OnIconPosClick)
		DEFCMD(OnExit)		
	END_DEFMAINMSG
END_SETUPMAINWINDOW

BEGIN_REGMAINMSG
	REGMSG(WM_INITDIALOG)
	REGCMD(IDC_BTNBMP,OnBtBmpClick)
	REGCMD(IDC_BTNCUST,OnAlignClick)
	REGCMD(IDC_BTNOWNIMG,OnIconPosClick)
	REGCMD(IDOK,OnExit)
	REGCMD(IDCANCEL,OnExit)
END_REGMSG

int RingMain()
{
	APP_CREATEDIALOG(MAKEINTRESOURCE(IDD_MAIN));
	return RingApp.Run();
}

RINGMAINMSG(WM_INITDIALOG)
{
	//直接生成的对象用AssignItem关联
	AssignItem(m_btDef,IDC_STANDARD);
	AssignItem(m_btIcon,IDC_BTNICON);
	AssignItem(m_btBmp,IDC_BTNBMP);
	AssignItem(m_btCust,IDC_BTNCUST);
	//关联对象指针用GetItem
	m_lpbtn = GetItem(itemof(RingButton),IDC_BTNOWNIMG);
	//或m_lpbtn = GetItem(m_lpbtn,IDC_BTNOWNIMG);
	
	//设置图标
	m_btIcon.SetIcon(IDI_ICON1,128,128);
	//设置类型为无焦点矩形框
	m_btIcon.SetExStyle(RINGBTN_EX_NOFOCUSRECT);

	//设置图象
	m_btBmp.SetBitmap(IDB_BEER);
	
	m_btCust.AddIcon(IDI_ICON1,ATBTN_LEFT,16,16);
	m_btCust.SetBkColor(0x007f7fff);
	m_btCust.SetTextColor(0x004f4eFF);
	m_lpbtn->AddBitmap(IDB_TIME,ATBTN_LEFT);
	m_lpbtn->SetExStyle(RINGBTN_EX_FLAT);
	
	m_btDef.SetBkColor(0x00FF0000);
	m_btDef.SetTextColor(0x000000FF);
	return FALSE;
}

RINGMAINCMD(OnAlignClick)
{
	//event为本函数的参数，为WM_COMMAND消息里的HIWORD(wParam)，RINGCMD宏隐藏了该参数.
	//设置过背景和字体颜色，BUTTON为OWNERDRAW类型，会发送BN_PUSHED等消息，因此需判断
	//是否点击了按钮产生的消息。
	if(event == BN_CLICKED)	
	{
		if(strcmp(m_btCust.GetText(),"文字左对齐") == 0)
		{
			m_btCust.ReplaceStyle(BS_LEFT,BS_CENTER);
			m_btCust.SetValue("文字居中");
		}
		else if(strcmp(m_btCust.GetText(),"文字居中") == 0)
		{
			m_btCust.ReplaceStyle(BS_CENTER,BS_RIGHT);
			m_btCust.SetValue("文字右对齐");
		}
		else		
		{
			m_btCust.ReplaceStyle(BS_RIGHT,BS_LEFT);
			m_btCust.SetValue("文字左对齐");
		}
	}
}

RINGMAINCMD(OnIconPosClick)
{
	if(m_lpbtn && event == BN_CLICKED)
	{		
		if(strcmp(m_lpbtn->GetText(),"图象在左") == 0)
		{
			m_lpbtn->AddBitmap(IDB_TIME,ATBTN_TOP);
			m_lpbtn->SetValue("图象在上");
		}
		else if(strcmp(m_lpbtn->GetText(),"图象在上") == 0)
		{
			m_lpbtn->AddBitmap(IDB_TIME,ATBTN_RIGHT);
			m_lpbtn->SetValue("图象在右");
		}
		else if(strcmp(m_lpbtn->GetText(),"图象在右") == 0)
		{
			m_lpbtn->AddBitmap(IDB_TIME,ATBTN_BOTTOM);
			m_lpbtn->SetValue("图象在下");
		}
		else if(strcmp(m_lpbtn->GetText(),"图象在下") == 0)
		{
			m_lpbtn->AddBitmap(IDB_TIME,ATBTN_LEFT);
			m_lpbtn->SetValue("图象在左");
		}		
	}
}

RINGMAINCMD(OnBtBmpClick)
{
	if(MASK_MATCH(m_btBmp.GetStyle(),BS_BITMAP))
	{		
		m_btBmp.SetIcon(IDI_ICON1,16,16);
		m_btBmp.SetPos(0,0,32,32,NULL,SWP_NOMOVE);
	}
	else
	{
		m_btBmp.SetBitmap(IDB_BEER);
		m_btBmp.SetPos(0,0,42,128,NULL,SWP_NOMOVE);
	}
}

RINGMAINCMD(OnExit)
{
	Close(0);
}
