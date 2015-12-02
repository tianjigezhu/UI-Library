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
//原文件名：ringcombo.cpp
//
//说明：下拉列表控件封装实现
//
**********************************************************************/
#define MAKE_SELF_LIB
#include "ringdows.h"

#if(WINVER < 0x0500)
typedef struct tagCOMBOBOXINFO
{
	DWORD cbSize;
	RECT  rcItem;
	RECT  rcButton;
	DWORD stateButton;
	HWND  hwndCombo;
	HWND  hwndItem;
	HWND  hwndList;
} COMBOBOXINFO, *PCOMBOBOXINFO, *LPCOMBOBOXINFO;

BOOL WINAPI GetComboBoxInfo(HWND hwndCombo,PCOMBOBOXINFO pcbi);
#endif

int RingCombo::InsertString(LPCTSTR lpString,int index,LONG lData/*=0*/)
{
	if(lpString)
	{
		int nIndex = SendMessage(m_hWnd,CB_INSERTSTRING,index,(LPARAM)lpString);
		if(nIndex != CB_ERR && lData != 0)
			SetData(nIndex,lData);
		
		return nIndex;
	}
	return CB_ERR;
}

int RingCombo::AddString(LPCTSTR lpString,LONG lData/*=0*/)
{
	if(lpString)
	{
		int nIndex = SendMessage(m_hWnd,CB_ADDSTRING,0,(LPARAM)lpString);
		if(nIndex != CB_ERR && lData != 0)
			SetData(nIndex,lData);

		return nIndex;
	}
	return CB_ERR;
}

//加入多个子项，格式：item1|item2|item3|...
int RingCombo::AddStrings(LPCTSTR lpstr)
{
	if(m_hWnd == NULL)
		return CB_ERR;
	
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
	return index - cnt - 1;
}

LPCTSTR RingCombo::GetLBText(int index/*=INDEX_CURSEL*/,
									  int size/*=RBUF_SIZE*/,
									  LPTSTR lpBuf/*=NULL*/)
{
	if(lpBuf == NULL)
	{
		if(GetBuffer(size))
		{
			memset(m_buf,0,m_bufsize);
			GetItemText(index,m_buf,m_bufsize);
			return m_buf;
		}
		else
			return "";
	}
	else
	{
		memset(lpBuf,0,size);
		if(index == INDEX_CURSEL)
			index = GetCurSel();
		SendMessage(m_hWnd,CB_GETLBTEXT,index,(LPARAM)lpBuf);
		return lpBuf;
	}
}

/*
int RingCombo::GetItemText(int index,LPTSTR lpBuf,int size)
{
	if(index == -1)
		index = GetCurSel();
	return SendMessage(m_hWnd,CB_GETLBTEXT,index,(LPARAM)lpBuf);
}
*/

//选中lpszVal指定的项，如果失败且允许输入，设置为lpszVal，index参数无用
BOOL RingCombo::SetValue(LPCTSTR lpszVal,int index/*=INDEX_CURSEL*/)
{	
	if(lpszVal)
	{
		if(Select(lpszVal) == CB_ERR)
			return SetWindowText(m_hWnd,lpszVal);			
		else
			return TRUE;
	}
	return FALSE;
}

/*
LRESULT RingCOMBO::RingdowProc(HWND hWnd,UINT uMsg,UINT wParam,LONG lParam)
{
//wsprintf(d,"msg:%x\r\n",uMsg);
//rf.Write(d,strlen(d));	
	if(uMsg == WM_ENABLE)
	{
		RingCtrlWnd::RingdowProc(hWnd,uMsg,wParam,lParam);
		currStyle = wParam?RINGITEMSTYLE_FLAT:RINGITEMSTYLE_NORMAL;
		DrawItem();
		return 0;
	}

	if(m_ExStyle == RINGITEMSTYLE_FLAT && IsWindowEnabled(hWnd))
	{
		LRESULT res = RingCtrlWnd::RingdowProc(hWnd,uMsg,wParam,lParam);
		switch(uMsg)
   	{
      	case WM_NCPAINT:
	         return OnDrawItem(hWnd,wParam,lParam);				
      	case WM_TIMER:
	         return OnTimer(hWnd,wParam,lParam);
			case WM_LBUTTONDOWN:
			{
				if((m_crBorder.crFlag & CR_FOCUSBORDERSET))	//已设置边框色
				{
					GetWindowRect(m_hWnd,&rcItem);

					rcItem.right -= (rcItem.left);
					rcItem.left = 0;
					rcItem.bottom -= (rcItem.top);
					rcItem.top = 0;

					InflateRect(&rcItem,-2,-2);
					rcItem.left = rcItem.right - GetSystemMetrics(SM_CXHSCROLL);

					HDC hDC = GetWindowDC(m_hWnd);
					DrawRect(hDC,&rcItem,m_crBorder.crFocus_border);
					ReleaseDC(m_hWnd,hDC);
				}
				return res;
			}
			case WM_LBUTTONUP:      // For kill focus
				if((m_crBorder.crFlag & CR_FOCUSBORDERSET))	//已设置边框色
				//if(lParam == -1)
					OnDrawItem(hWnd,wParam,lParam);
				return res;
			case WM_COMMAND:
				if(HIWORD(wParam) == EN_SETFOCUS)
					OnDrawItem(hWnd,wParam,lParam);
				return res;
			//case WM_CTLCOLORLISTBOX:
			//	return (LRESULT)CreateSolidBrush(0x00ff0000);
			default:
				return res;
   	}
	}
   return RingCtrlWnd::RingdowProc(hWnd,uMsg,wParam,lParam);
}

LRESULT RingCOMBO::OnDrawItem(HWND hWnd,UINT,LONG)
{
	DrawItem();
		      
	InflateRect(&rcItem,-1,-1);
	rcItem.left = rcItem.right - GetSystemMetrics(SM_CXHSCROLL);

	hDC = GetWindowDC(m_hWnd);

	if(currStyle == RINGITEMSTYLE_FLAT)
	{
		if((m_crBorder.crFlag & CR_FLATBORDERSET))	//已设置边框色
			DrawRect(hDC,&rcItem,m_crBorder.crFlat_border);
		else if((m_crBorder.crFlag & CR_FLATSET) == 0)	//未设置浮动边框
		{
			DrawRect(hDC,&rcItem,/*GetSysColor(COLOR_3DHILIGHT)/0X00FFFFFF,RC_LEFTTOP);
			DrawRect(hDC,&rcItem,m_crSys.cr3DSHADOW,RC_RIGHTBOTTOM);
		}
		else
		{
			DrawRect(hDC,&rcItem,m_crBorder.crFlat_lefttop,RC_LEFTTOP);
			DrawRect(hDC,&rcItem,m_crBorder.crFlat_rightbottom,RC_RIGHTBOTTOM);
		}
		//绘制内边框
		InflateRect(&rcItem,-1,-1);
		DrawRect(hDC,&rcItem,m_crSys.crBTNFACE);		
	}
	else
	{
		if((m_crBorder.crFlag & CR_FOCUSBORDERSET))	//已设置边框色
			DrawRect(hDC,&rcItem,m_crBorder.crFocus_border);
		else if((m_crBorder.crFlag & CR_FOCUSSET) == 0)	//未设置浮动边框
		{
			DrawEdge(hDC,&rcItem,EDGE_RAISED,BF_RECT);
			//DrawRect(hDC,&rcItem,0x00ff);
			ReleaseDC(m_hWnd,hDC);
			bPainted = TRUE;
			return 0;
		}
		else
		{
			DrawRect(hDC,&rcItem,m_crBorder.crFocus_lefttop,RC_LEFTTOP);
			DrawRect(hDC,&rcItem,m_crBorder.crFocus_rightbottom,RC_RIGHTBOTTOM);
		}
		InflateRect(&rcItem,-1,-1);
		DrawRect(hDC,&rcItem,m_crSys.crBTNFACE);
	}
	
	ReleaseDC(m_hWnd,hDC);

  	return 0;
}

BOOL RingCOMBO::IsFocused()
{
	if((GetWindowLong(m_hWnd,GWL_STYLE) & 0x03) == CBS_DROPDOWN)
   {
      HWND hWnd = GetTopWindow(m_hWnd);
      if (hWnd && hWnd == GetFocus())
         return TRUE;
		else
			return FALSE;
   }
	return RingControls::IsFocused();
}

*/
