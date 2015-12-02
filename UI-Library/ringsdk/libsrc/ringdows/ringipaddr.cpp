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
//版本：1.0
//
//声明：本类库可以自由使用而不须对作者作出任何回报，但作者希望能得到
//		  你的鼓励和支持。你可以对类库源码作出修改和改进，但希望你能在
//		  修改的同时给作者一份同样的副本。
//		  本类库不得用于任何商业用途，如确实需要，请与作者联系。
//
//e-mail:ringphone@sina.com
//
//原文件名：ringipaddr.cpp
//
//说明：IP地址控件控制
//
**********************************************************************/
#define MAKE_SELF_LIB
#include "ringdows.h"

RingIPAddr::RingIPAddr()
{
	InitIPControl();
}

BOOL RingIPAddr::Create(UINT uId,int x,int y,int width,int height)
{
	return Create(0,0,NULL,uId,x,y,width,height);
}

BOOL RingIPAddr::Create(DWORD dwStyle,DWORD dwExStyle,LPCTSTR szInitText,UINT uId,int x,int y,int width,int height)
{
	return RingControls::Create(dwStyle,dwExStyle,szInitText,uId,x,y,width,height);
}

int RingIPAddr::GetInt(int)
{
	DWORD dwRes;
	SendMessage(m_hWnd,IPM_GETADDRESS,0,(LPARAM)&dwRes);
	return dwRes;
}

BOOL RingIPAddr::SetValue(LPCTSTR lpszVal,int)
{
	char szip[16];
	LPSTR lps = (LPSTR)lpszVal,lpi = lps;
	int i,j,nCnt = 0;
	int ip[4] = {0,0,0,0};

	if(lpszVal)
	{
		strncpy(szip,lpszVal,15);
		lps = szip;
	
		for(i=0;i<4;i++)
		{
			for(j=0;j<4;j++)
			{
				if(*lps == '\0' || *lps == '.')
				{
					if(*lps == '.')
						*lps++ = '\0';
					
					ip[i] = atoi(lpi);
					lpi = lps;
					break;
				}
				else
					lps ++;
			}
			if(*lps == '\0')
				break;
		}
		SendMessage(m_hWnd,IPM_SETADDRESS,0,MAKEIPADDRESS(ip[0],ip[1],ip[2],ip[3]));
		return TRUE;
	}
	return FALSE;
}

int RingIPAddr::GetItemText(int,LPTSTR lpBuf,int size)
{
	DWORD dw;
	SendMessage(m_hWnd,IPM_GETADDRESS,0,(LPARAM)&dw);
	if(size >= 16)
	{
		wsprintf(lpBuf,"%d.%d.%d.%d\0",FIRST_IPADDRESS(dw),SECOND_IPADDRESS(dw),
				THIRD_IPADDRESS(dw),FOURTH_IPADDRESS(dw));
		return strlen(lpBuf);
	}
	else
		return 0;
}
