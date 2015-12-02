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
//							Shell操作类库 ringShell.lib
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
//原文件名：ringshell.c
//
//说明：Shell操作实现（创建快捷方式，运行控制面板）
//
**********************************************************************/
#define MAKE_SELF_LIB
#include "ringshell.h"

//创建快捷方式（C++版本）
//用法：CreateShortcut(目标文件,运行参数（一般为NULL）,运行目录,快捷方式文件名（eg:"c:\\temp\\test.lnk"),快捷方式名称);
HRESULT CreateShortcut(LPCSTR pszPathObj,LPSTR pszParam,LPSTR pszPath,LPSTR pszPathLink,LPSTR pszDesc)
{
    HRESULT hres ;
    IShellLink * psl ;
    IPersistFile* ppf ;
    WORD wsz[MAX_PATH] ;

    CoInitialize(NULL);
    hres = (HRESULT)CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER,IID_IShellLink,(void**)&psl) ;
    if( FAILED(hres))
	 {
    	CoUninitialize();
     	return FALSE ;
    }
    // set the path to the shortcut target, and add the description
	 psl -> SetPath(pszPathObj);
    psl -> SetArguments(pszParam) ;
    psl -> SetDescription(pszDesc);
    psl -> SetWorkingDirectory(pszPath);

	 // query IShellLink for the IPersistFile interface for saving the shortcut in persistent storage
    hres = (HRESULT)(psl -> QueryInterface( IID_IPersistFile, (void**)&ppf)) ;
    if( FAILED( hres))
    {
		CoUninitialize();
    	return FALSE ;
    }

    // ensure that that string is ANSI
    MultiByteToWideChar( CP_ACP, 0, pszPathLink, -1, (LPWSTR)wsz, MAX_PATH) ;

    // save the link by calling IPersistFile::Save
    hres = ppf -> Save((LPWSTR)wsz, STGM_READWRITE) ;

	// release the IPersistFile interface
    ppf -> Release() ;
	// release the IShellLink interface
    psl -> Release() ;

    CoUninitialize();

	return hres ;
}

//创建快捷方式（C版本），已废弃
/*
HRESULT CreateShortcut(LPCSTR pszPathObj,LPSTR pszParam,LPSTR pszPath,LPSTR pszPathLink,LPSTR pszDesc)
{
    HRESULT hres ;
    IShellLink * psl ;
    IPersistFile* ppf ;
    WORD wsz[ MAX_PATH] ;

    CoInitialize(NULL);
    hres = (HRESULT)CoCreateInstance(&CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER,&IID_IShellLink,(LPVOID)&psl) ;
    if( FAILED(hres))
	 {
    	CoUninitialize();
     	return FALSE ;
    }
    // set the path to the shortcut target, and add the description
	 psl -> lpVtbl -> SetPath( psl, pszPathObj) ;
    psl -> lpVtbl -> SetArguments( psl, pszParam) ;
    psl -> lpVtbl -> SetDescription( psl, pszDesc);
    psl -> lpVtbl -> SetWorkingDirectory(psl,pszPath);

	 // query IShellLink for the IPersistFile interface for saving the shortcut in persistent storage
    hres = (HRESULT)(psl -> lpVtbl -> QueryInterface( psl, &IID_IPersistFile, &ppf)) ;
    if( FAILED( hres))
    {
		CoUninitialize();
    	return FALSE ;
    }

    // ensure that that string is ANSI
    MultiByteToWideChar( CP_ACP, 0, pszPathLink, -1, wsz, MAX_PATH) ;

    // save the link by calling IPersistFile::Save
    hres = ppf -> lpVtbl -> Save( ppf, wsz, STGM_READWRITE) ;

	// release the IPersistFile interface
    ppf -> lpVtbl -> Release( ppf) ;
	// release the IShellLink interface
    psl -> lpVtbl -> Release( psl) ;

    CoUninitialize();

	return hres ;
}
*/

BOOL RunControlPanel(LPSTR szCplName)
{
	STARTUPINFO si={0};
   PROCESS_INFORMATION pi ;
   char szCmd[600],szWinPath[MAX_PATH],szSysPath[MAX_PATH];

   si.cb = sizeof( STARTUPINFO) ;
	si.lpReserved = NULL ;
	si.lpDesktop = NULL ;
   si.lpTitle = NULL ;
	si.dwX = 0 ;
   si.dwY = 0 ;
   si.dwXSize = 0 ;
   si.dwYSize = 0 ;
   si.dwXCountChars = 0 ;
   si.dwYCountChars = 0 ;
   si.dwFillAttribute = 0 ;
   si.dwFlags = STARTF_USESHOWWINDOW;
   si.wShowWindow = SW_SHOW ;
   si.cbReserved2 = 0 ;
   si.lpReserved2 = NULL ;
   si.hStdInput = NULL;
   si.hStdOutput = NULL;
   si.hStdError = NULL;

   if(OSType() == NT)
		GetSystemDirectory(szWinPath,MAX_PATH);
   else
   	GetWindowsDirectory(szWinPath,MAX_PATH);

   GetSystemDirectory(szSysPath,MAX_PATH);
   wsprintf(szCmd,"%s\\control.exe %s\\%s",szWinPath,szSysPath,szCplName);

   if( !CreateProcess(	0,//szWinPath,
								szCmd,//"control.exe c:pwin97systemtimedate.cpl",//szSysPath,
                        (LPSECURITY_ATTRIBUTES)NULL,
                        (LPSECURITY_ATTRIBUTES)NULL,
                        FALSE,
                        NORMAL_PRIORITY_CLASS,
                        NULL,
                        NULL,
                        &si,
                        &pi))
   {
   	return FALSE;
   }
   return TRUE;
}
/*
void KillTaskButton(void)
{
   //DEFINE_MYGUID(CLSID_TaskbarList, 0x56fdf344, 0xfd6d, 0x11d0, 0x95, 0x8a, 0x0, 0x60, 0x97, 0xc9, 0xa0, 0x90);
	//DEFINE_MYGUID(IID_ITaskbarList, 0x56fdf342, 0xfd6d, 0x11d0, 0x95, 0x8a, 0x0, 0x60, 0x97, 0xc9, 0xa0, 0x90);

   LPITaskbarList pobj;
   HRESULT hres;

	CoInitialize(NULL);
   hres = (HRESULT)CoCreateInstance(&CLSID_TaskbarList,0,1,&IID_ITaskbarList,(LPVOID)&pobj);
   if( FAILED(hres))
	{
   	CoUninitialize();
     	return;
   }
   Errmsg("will init");
   pobj->lpVtbl->HrInit();
   Errmsg("inited");
	pobj->lpVtbl->DeleteTab(m_hWnd);
   Errmsg("deled");
   CoUninitialize();
}
*/
