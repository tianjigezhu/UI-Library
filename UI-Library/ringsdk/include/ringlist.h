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
//原文件名：ringlist.h
//
//说明：对列表控件的封装
//
**********************************************************************/
#ifndef _INC_RINGLIST
#define _INC_RINGLIST

class RingListBox:public RingControls
{
public:
	RingListBox();
	~RingListBox();
		
	//virtual LRESULT OnDrawItem(HWND,UINT,LONG);

   LPCTSTR WndClassName(){return "ListBox";}

	//字符串操作
	virtual int AddString(LPCTSTR lpString,LONG lData=0);
	//加入多个子项，格式：item1|item2|item3|...
	int AddStrings(LPCTSTR lpstr);
	virtual int InsertString(LPCTSTR lpString,int index,LONG lData=0);		
	BOOL SetValue(int val,int index=INDEX_CURSEL);
	BOOL SetValue(LPCTSTR lpszVal,int index=INDEX_CURSEL);

	void SetOwnerDraw(BOOL bParentDraw=FALSE)
			{RingControls::SetOwnerDraw(bParentDraw,LBS_OWNERDRAWFIXED);}

	//消息封装(不完全，以后逐步添加)
	int Select(int index){return SendMessage(m_hWnd,LB_SETCURSEL,index,0);}
	int Select(LPCTSTR lpStr,int index=-1){return SendMessage(m_hWnd,LB_SELECTSTRING,index,(LPARAM)lpStr);}
	int GetCurSel(){return SendMessage(m_hWnd,LB_GETCURSEL,0,0);}
	BOOL SetCurSel(int index){return (SendMessage(m_hWnd,LB_SETCURSEL,index,0) != LB_ERR);}

	BOOL IsSelected(int index){return (BOOL)(SendMessage(m_hWnd,LB_GETSEL,index,0) > 0);}

	int GetTextLen(int index){return SendMessage(m_hWnd,LB_GETTEXTLEN,index,0);}
	
	virtual LONG GetData(int index){return SendMessage(m_hWnd,LB_GETITEMDATA,index,0);}
	virtual BOOL SetData(int index,LPARAM lpData){return (SendMessage(m_hWnd,LB_SETITEMDATA,index,lpData) != LB_ERR);}
	
	BOOL Clear(){SendMessage(m_hWnd,LB_RESETCONTENT,0,0);return TRUE;}
	int DelString(int index){return SendMessage(m_hWnd,LB_DELETESTRING,index,0);}
	int DelString(LPCTSTR lpString){return DelString(FindString(lpString));}
	int FindString(LPCTSTR lpszSearch,int nStart=-1){return SendMessage(m_hWnd,LB_FINDSTRING,nStart,(LPARAM)lpszSearch);}
	int FindMatchString(LPCTSTR lpszSearch,int nStart=-1)
			{return SendMessage(m_hWnd,LB_FINDSTRINGEXACT,nStart,(LPARAM)lpszSearch);}
	int GetCount(){return SendMessage(m_hWnd,LB_GETCOUNT,0,0);}
	int GetTopIndex(){return SendMessage(m_hWnd,LB_GETTOPINDEX,0,0);}
	int SetTopIndex(int index){return SendMessage(m_hWnd,LB_SETTOPINDEX,index,0);}
	int GetItemHeight(int index=0){return SendMessage(m_hWnd,LB_GETITEMHEIGHT,index,0);}
	BOOL SetItemHeight(int height,int index=0){return (SendMessage(m_hWnd,LB_SETITEMHEIGHT,index,height) != LB_ERR);}
	BOOL GetItemRect(int index,LPRECT lprc);
	int ItemFromPoint(int x,int y){return SendMessage(m_hWnd,LB_ITEMFROMPOINT,0,MAKELONG(x,y));}
	int Dir(UINT flag,LPCTSTR lpszFile){return SendMessage(m_hWnd,LB_DIR,flag,(LPARAM)lpszFile);}
	
protected:
	LRESULT OnBkColor(UINT,HDC);
	int GetItemText(int index,LPTSTR lpBuf,int size);
   //int m_style;
	//virtual LRESULT RingdowProc(HWND,UINT,UINT,LONG);

private:
	//BOOL bPainted;
};

#endif
