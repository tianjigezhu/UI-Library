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
//本文件说明：演示属性栏OptionView的使用。
//
//属性栏说明：	属性栏有名称和值两项显示，三种数值编辑方式：编辑框输入
//					（编辑方式），列表框选择或输入（选择方式），编辑框输入
//					及点击旁边的按钮进行更详细设置（浏览输入方式）。
//					输入值分显示值和设置值，一般情况下如果值是字符串，显示
//					值和设置值是一致的，如果是数值，则可能需要设置其设置值，
//					比如[禁止修改|允许]这种选项（"禁止修改"为显示名称，"允
//					许"为显示值，则可能需要设置1为设置值以方便后续的结果处
//					理。因此，获取选项值有以下几种方法（以[禁止修改|允许]，
//					设置值1为例）：
//					GetName:获取选项名称，返回"禁止修改"
//					GetText:获取显示值，返回"允许"
//					GetInt:获取设置值，返回1
//
**********************************************************************/
#include "ringmain.h"
#include "resource.h"

#define IOP_SELFILE	0
#define IOP_SELFONT	1
#define IOP_SHOWMSG	2

#define IDX_NAME				0
#define IDX_CANEDITNAME		1
#define IDX_TYPE				2
#define IDX_DATATYPE			3
#define IDX_DEFDATA			4
#define IDX_PACKLIST			5

BEGIN_SETUPMAINWINDOW(IS_DIALOG)
	BEGIN_DEF_VAR(NO_INIT)
		RingOptionView m_list,m_option;
		RingOptionItemSel m_itemSel;
		RingOptionItemList m_itemType,m_itemPackList,m_itemBro;
		RingOptionItemEdit m_itemVal;
		int m_index;
	END_DEF_VAR
	
	BOOL OnValChange(RingOptionItem* item)
	{
		int nIdx = m_option.GetSelectIndex(IDX_CANEDITNAME);
		if(nIdx == 1)
			//返回FALSE为限制修改
			return FALSE;
		else
			return TRUE;
	}

	BEGIN_DEFMSG
		DEFMSG(WM_INITDIALOG)
		DEFCMD(OnAdd)
		DEFCMD(OnDel)
		DEFCMD(OnClear)
		DEFNOTIFY(ROPVN_SELCHANGE)
		DEFNOTIFY(ROPVN_BUTTONCLICK)
		DEFNOTIFY(ROPVN_VALUECHANGE)
		DEFCMD(OnClose)
	END_DEFMAINMSG
END_SETUPMAINWINDOW

BEGIN_REGMAINMSG
	REGCMD(IDOK,OnAdd)
	REGCMD(IDCANCEL,OnDel)
	REGCMD(IDC_CLEAR,OnClear)
	REGCMD(IDC_CLOSE,OnClose)
	REGMSG(WM_INITDIALOG)
	REGNOTIFY(ROPVN_VALUECHANGE)
	REGNOTIFY(ROPVN_SELCHANGE)
	REGNOTIFY(ROPVN_BUTTONCLICK)
END_REGMSG

BEGIN_SETUPWINDOW(listdlg,IS_DIALOG)
	BINDID(IDD_LISTOPTION)
	BEGIN_DEF_VAR(NO_INIT)
		ringStr m_str;
		RingEdit* m_edit;
	END_DEF_VAR

	BEGIN_DEFMSG
		DEFMSG(WM_INITDIALOG)
		DEFCMD(OnOK)
	END_DEFMSG(listdlg)
END_SETUPWINDOW(listdlg)

BEGIN_REGMSG(listdlg)
	REGMSG(WM_INITDIALOG)
	REGCMD(IDOK,OnOK)
	REGCMD(IDCANCEL,Close)
END_REGMSG

int RingMain()
{
	APP_CREATEDIALOG(MAKEINTRESOURCE(IDD_MAIN));
	return RingApp.Run();
}

RINGMAINMSG(WM_INITDIALOG)
{
	AssignItem(m_list,IDC_LISTFILE);
	AssignItem(m_option,IDC_OPTION);
	m_list.SetBkColor(0xFFDEDA);

	m_index = 1;
	//添加设置项
	m_option.AddOption("子项名称","Item1");
	//如果是列表项，设置OPTYPE_READONLY则只可选择不可输入
	m_option.AddOption("名称修改","允许",0,OPTYPE_LIST|OPTYPE_READONLY,(LPARAM)"允许|禁止|只读");
	m_option.AddOption("子项类型","独立",0,OPTYPE_LIST|OPTYPE_READONLY,(LPARAM)"独立|数据包|数据包子项");
	//项目数据设置
	m_itemType.SetName("数据类型");
	//同样设置为只可选择不可输入，该选项应该在加入前设置
	m_itemType.SetReadOnly(TRUE);
	m_itemType.SetSelection("默认|列表|浏览");
	m_itemType.SetSelectIndex(0);
	m_option.AddOption(&m_itemType);

	m_itemVal.SetName("默认数据");
	m_option.AddOption(&m_itemVal);
	
	//项目数据设置，但不添加到列表中
	m_itemPackList.SetName("加入到数据包");
	m_itemPackList.SetReadOnly(TRUE);
	m_itemSel.SetName("列表选项");
	m_itemSel.SetValue("真|假");
	m_itemBro.SetName("浏览选项");
	m_itemBro.SetReadOnly(TRUE);
	m_itemBro.SetSelection("浏览文件|浏览字体|显示信息|不处理");
	m_itemBro.SetSelectIndex(3);

	//设置子项名称项数值改变的处理函数
	m_option.SetItemValChangeFunc(0,(RINGVCLASS*)this,(ROPV_CHECKVALFUNC)OnValChange);
	
	return FALSE;
}

RINGMAINCMD(OnAdd)
{
	ringStr strName = m_option.GetText(IDX_NAME);
	char s[20];
	int index;

	if(strName.len() > 0)
	{
		index = m_option.GetSelectIndex(IDX_TYPE);
		
		if(index == 1)
		{
			//数据包
			m_list.AddOptionPack(strName.string());				
				
			m_index ++;				
			wsprintf(s,"数据包%d",m_index);
			m_option.SetValue(s,0);
			
			m_itemPackList.AddSelection(strName.string());
			if(strlen(m_itemPackList.GetText()) == 0)
				m_itemPackList.SetValue(strName.string());				
			return;
		}
		else
		{
			ringStr strDef = m_option.GetText(IDX_DEFDATA);
			LONG lVal = 0,lOption = 0;
			int type = OPTYPE_EDIT;

			switch(m_option.GetSelectIndex(IDX_DATATYPE))
			{
			case 1://列表
				type = OPTYPE_LIST;
				lOption = (LPARAM)m_itemSel.GetText();
				break;
			case 2://浏览
				type = OPTYPE_BROWSE;
				lVal = m_itemBro.GetSelectIndex();
				break;				
			}
			if(index == 0)		//独立
				m_list.AddOption(strName.string(),strDef.string(),lVal,type,lOption);
			else	//数据包子项
				m_list.AddSubOption(m_option.GetText(IDX_PACKLIST),strName.string(),strDef.string(),lVal,type,lOption);
		}
		m_index ++;
		wsprintf(s,"item%d",m_index);
		m_option.SetValue(s,0);
	}
}

RINGMAINCMD(OnDel)
{
	RingOptionItem* item = m_list.GetOptionItem();
	if(item)
	{
		if(item->IsPackItem())
			m_itemPackList.DelSelection(item->GetName());
		m_list.DelOption(item);
	}
}

RINGMAINCMD(OnClear)
{
	m_itemBro.GetInt();
	m_list.Clear();
	m_itemPackList.Clear();
}

RINGMAINNOTIFY(ROPVN_SELCHANGE)
{
	if(param.lpropvnm->hdr.idFrom == IDC_OPTION)
	{		
		int index = param.lpropvnm->item->GetSelectIndex();

		switch(m_option.GetCurSel())
		{
			case IDX_TYPE:
			{	
				//子项类型变更
				char szName[40];
				if(index == 1)
				{
					//数据包
					wsprintf(szName,"数据包%d",m_index);
					//其他设置都不需要，删除
					m_option.DelOption(&m_itemType);
					m_option.DelOption(&m_itemVal);
					m_option.DelOption(&m_itemPackList);
					m_option.DelOption(&m_itemSel);
					m_option.DelOption(&m_itemBro);
				}
				else
				{
					//独立或数据包子项
					wsprintf(szName,"Item%d",m_index);
					m_option.SetValue(szName,IDX_NAME);
					//AddOption会自动检测是否已加入过
					m_option.AddOption(&m_itemType);
					m_option.AddOption(&m_itemVal);
					
					if(index == 0)
						//独立数据，删除数据包列表
						m_option.DelOption(&m_itemPackList);
					else
					{
						//这两个子项必须跟在数据包列表后面，先删除，否则顺序会乱
						m_option.DelOption(&m_itemSel);
						m_option.DelOption(&m_itemBro);
						//数据包子项，添加数据包列表						
						m_option.AddOption(&m_itemPackList);
					}
						
					index = m_itemType.GetSelectIndex();
					if(index == 1)
						m_option.AddOption(&m_itemSel);	 //列表数据类型
					else if(index == 2)
						m_option.AddOption(&m_itemBro);	 //浏览数据类型
				}
				//设置子项名称
				m_option.SetValue(szName,IDX_NAME);
				break;
			}
			case IDX_DATATYPE:
			{
				//数据类型变更
				if(index == 1)
				{
					//列表，增加列表选项，删除浏览选项
					m_option.AddOption(&m_itemSel);
					m_option.DelOption(&m_itemBro);
				}
				else if(index == 2)
				{
					//浏览，增加浏览选项，删除列表选项
					m_option.DelOption(&m_itemSel);
					m_option.AddOption(&m_itemBro);
				}
				else
				{
					//默认，删除不需要的选项
					m_option.DelOption(&m_itemSel);
					m_option.DelOption(&m_itemBro);
				}
				break;
			}
			case IDX_CANEDITNAME:
			{				
				RingOptionItem* item = m_option.GetOptionItem(IDX_NAME);
				if(item)
				{
					if(index == 2)
						item->SetReadOnly(TRUE);	//只读
					else
						//可修改或使修改无效，设置为非只读，使修改无效在
						//OnValChange函数中判断实现
						item->SetReadOnly(FALSE);
				}
				break;
			}
		}
	}
	return 0;
}

RINGMAINNOTIFY(ROPVN_BUTTONCLICK)
{
	if(param.lpropvnm->hdr.idFrom == IDC_OPTION)
	{
		if(strcmp(param.lpropvnm->item->GetName(),"列表选项")==0)
		{
			listdlg = NewRingObj(listdlg);
			if(Popup(listdlg,NULL,(LPARAM)param.lpropvnm->item->GetText()))
				param.lpropvnm->item->SetValue(listdlg->m_str.string());
			DelRingObj(listdlg);
		}
	}
	else if(param.lpropvnm->hdr.idFrom == IDC_LISTFILE)
	{
		switch(param.lpropvnm->item->GetData())
		{
			case IOP_SELFILE:
			{
				ringFile rf;
				if(rf.Select("所有文件(*.*)|*.*||"))
					param.lpropvnm->item->SetValue(rf.Fullname());
				break;
			}
			case IOP_SELFONT:
			{
				ringFont font;
				if(font.Select())
				{
					LOGFONT lf;
					font.GetLogFont(&lf);
					param.lpropvnm->item->SetValue(lf.lfFaceName);
				}
				break;
			}
			case IOP_SHOWMSG:
			{
				LPSTR lpstr = (LPSTR)param.lpropvnm->item->GetText();
				if(lpstr)
					MsgBoxEx(param.lpropvnm->item->GetName(),MB_OK,strlen(lpstr)+1,lpstr);
				break;
			}
		}
	}
	return 0;
}

RINGMAINNOTIFY(ROPVN_VALUECHANGE)
{	
	return 0;
}

RINGMAINCMD(OnClose)
{
	Close();
}

RINGMSG(listdlg,WM_INITDIALOG)
{
	m_str = (LPSTR)param.lParam;
	m_str = m_str.replace("|","\r\n");
	
	m_edit = GetItem(m_edit,IDC_SLIST);
	if(m_edit)
		m_edit->SetValue(m_str.string());
	return 0;
}

RINGCMD(listdlg,OnOK)
{
	if(m_edit)
	{
		m_str = m_edit->GetText();
		m_str = m_str.replace("\r\n","|");
		Close(TRUE);
	}
	else
		Close();
}
