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
//版本：0.2
//
//声明：本类库可以自由使用而不须对作者作出任何回报，但作者希望能得到
//		  你的鼓励和支持。你可以对类库源码作出修改和改进，但希望你能在
//		  修改的同时给作者一份同样的副本。
//		  本类库不得用于任何商业用途，如确实需要，请与作者联系。
//
//e-mail:ringphone@sina.com
//
//原文件名：ringlist.cpp
//
//说明：ListBox控件控制
//
**********************************************************************/
#define MAKE_SELF_LIB
#include "ringdows.h"

RingListBox::RingListBox()
{
	m_windowType = ITIS_LISTBOX;	
}

RingListBox::~RingListBox()
{
}

int RingListBox::InsertString(LPCTSTR lpString,int index,LONG lData/*=0*/)
{
	if(lpString)
	{
		int nIndex = SendMessage(m_hWnd,LB_INSERTSTRING,index,(LPARAM)lpString);
		if(nIndex != LB_ERR && lData != 0)
			SetData(nIndex,lData);
		
		return nIndex;
	}
	return LB_ERR;
}

int RingListBox::AddString(LPCTSTR lpString,LONG lData/*=0*/)
{
	if(lpString)
	{
		int nIndex = SendMessage(m_hWnd,LB_ADDSTRING,0,(LPARAM)lpString);
		if(nIndex != LB_ERR && lData != 0)
			SetData(nIndex,lData);
		
		return nIndex;
	}
	return LB_ERR;
}

LRESULT RingListBox::OnBkColor(UINT uMsg,HDC hDC)
{
	switch(uMsg)
	{
	case WM_CTLCOLORLISTBOX:
		return SetColors(hDC);			
	}
	return 0;
}

int RingListBox::GetItemText(int index,LPTSTR lpBuf,int size)
{
	if(index == INDEX_CURSEL)
		index = GetCurSel();
	int nTextLen = SendMessage(m_hWnd,LB_GETTEXTLEN,index,0);
	if(nTextLen > size)
	{
		if(lpBuf != (LPTSTR)m_buf)
			return 0;
		else
		{
			//内部缓冲区，调整大小
			if(GetBuffer(nTextLen + 1))
				lpBuf = m_buf;
			else
				return 0;
		}
	}
	return SendMessage(m_hWnd,LB_GETTEXT,index,(LPARAM)lpBuf);
}

BOOL RingListBox::GetItemRect(int index,LPRECT lprc)
{
	if(lprc == NULL)
		return FALSE;

	return (LB_ERR != SendMessage(m_hWnd,LB_GETITEMRECT,index,(LPARAM)lprc));
}

BOOL RingListBox::SetValue(int val,int index/*=INDEX_CURSEL*/)
{
	char szText[16];
	wsprintf(szText,"%d\0",val);
	return SetValue(szText,index);
}

BOOL RingListBox::SetValue(LPCTSTR lpszVal,int index/*=INDEX_CURSEL*/)
{
	if(index == INDEX_CURSEL)
		index = GetCurSel();

	if(lpszVal && index >= 0 && index < GetCount())
	{
		LONG lData = GetData(index);
		SetData(index,0);		//避免发送WM_DELETEITEM消息
		DelString(index);
		index = max(0,index-1);
		return InsertString(lpszVal,index,lData);
	}
	return FALSE;
}

//加入多个子项，格式：item1|item2|item3|...
//返回加入的第一个子项的序号（如果ListBox自动排序，该返回值不一定准确）
int RingListBox::AddStrings(LPCTSTR lpstr)
{
	if(m_hWnd == NULL)
		return LB_ERR;
	
	int index,i,cnt,c = 0;
	ringStr str;
	LPTSTR s;
	
	str = lpstr;
	cnt = str.count("|") + 1;
	
	// Add the item
	for(i=0;i<cnt;i++) 
	{ 
		s = (LPTSTR)str.string() + c;
		c = str.indexOf("|",c) + 1;
		
		//str的operator[]总是安全的
		if(str[c-2] == '\\')
			continue;
		str[c-1] = '\0';		
		if(*s != '\0')
			index = AddString(s);
		str[c-1] = '|';		
	}
	
	return index - (cnt - 1);
}

/*
UINT RingListBox::GetString(LPCTSTR buf,int len)
{
	return SendMessage(m_hWnd,WM_GETTEXT,len,(LPARAM)buf);
}
*/
/*LRESULT RingListBox::RingdowProc(HWND hWnd,RINGPARAMS param)
{
	return DefaultProc(param);
}
*/
