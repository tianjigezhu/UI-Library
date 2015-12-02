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
//原文件名：ringcombo.h
//
//说明：对下拉列表控件的封装
//
**********************************************************************/
#ifndef _INC_RINGCOMBO
#define _INC_RINGCOMBO

class RingCombo:public RingControls
{
public:
	RingCombo(){m_windowType = ITIS_COMBO;}
	virtual ~RingCombo(){}
	
   LPCTSTR WndClassName(){return "ComboBox";}

	BOOL Clear(){SendMessage(m_hWnd,CB_RESETCONTENT,0,0);return TRUE;}

	BOOL SetValue(LPCTSTR lpszVal,int index=INDEX_CURSEL);

	//加入多个子项，格式：item1|item2|item3|...
	int AddStrings(LPCTSTR lpstr);

	virtual int AddString(LPCTSTR lpString,LONG lData=0);
	virtual int InsertString(LPCTSTR lpString,int index,LONG lData=0);
	int DelString(int index){return SendMessage(m_hWnd,CB_DELETESTRING,index,0);}
	int DelString(LPCTSTR lpString){return DelString(FindString(lpString));}
	int FindString(LPCTSTR lpszSearch,int nStart=-1){return SendMessage(m_hWnd,CB_FINDSTRING,nStart,(LPARAM)lpszSearch);}
	int FindMatchString(LPCTSTR lpszSearch,int nStart=-1)
	{
		return SendMessage(m_hWnd,CB_FINDSTRINGEXACT,nStart,(LPARAM)lpszSearch);
	}

	int Select(int index){return SendMessage(m_hWnd,CB_SETCURSEL,index,0);}
	int Select(LPCTSTR lpStr,int index=-1){return SendMessage(m_hWnd,CB_SELECTSTRING,index,(LPARAM)lpStr);}
	int GetCurSel(){return SendMessage(m_hWnd,CB_GETCURSEL,0,0);}
	LPCTSTR GetLBText(int index=INDEX_CURSEL,int size=RBUF_SIZE,LPTSTR lpBuf=NULL);
   int GetData(int index){return SendMessage(m_hWnd,CB_GETITEMDATA,index,0);}
	BOOL SetData(int index,LONG data){return (SendMessage(m_hWnd,CB_SETITEMDATA,index,data)!=CB_ERR);}

protected:
	//int GetItemText(int index,LPTSTR lpBuf,int size);

private:
	BOOL bPainted;

};

#endif