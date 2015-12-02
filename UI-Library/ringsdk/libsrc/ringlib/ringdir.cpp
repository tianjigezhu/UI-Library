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
//							RingSDK类库 ringlib.lib
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
//原文件名：file.cpp
//
//说明：文件操作类实现代码
//
**********************************************************************/
#define MAKE_SELF_LIB

#include "ringlib.h"

ringDir::ringDir()
{
	m_handle = NULL;
	memset(m_szDir,0,MAX_PATH*sizeof(TCHAR));
	memset(m_szCurrDir,0,MAX_PATH*sizeof(TCHAR));
}

ringDir::~ringDir()
{
	FindClose();
}

ringDir::ringDir(LPCTSTR szDir)
{
	m_handle = NULL;
	memset(m_szDir,0,MAX_PATH*sizeof(TCHAR));
	memset(m_szCurrDir,0,MAX_PATH*sizeof(TCHAR));

	if(szDir)
		SetDir(szDir);
}

void ringDir::SetDir(LPCTSTR format,...)
{
	va_list ap;
	memset(m_szDir,0,MAX_PATH*sizeof(TCHAR));
	
	if(format)
	{
		va_start(ap, format);
		_vsntprintf((char*)m_szDir,MAX_PATH-2,format,ap);
		//vsprintf((char*)m_szDir,format,ap);
		va_end(ap);
	}
	//去掉最后的"\\";
	TCHAR *lps = m_szDir + lstrlen(m_szDir) - 1;
	while(lps > m_szDir && *lps == '\\')
		*lps-- = '\0';
}

BOOL ringDir::IsExist()
{
	if(*m_szDir != '\0')
	{
		DWORD attr = GetFileAttributes(m_szDir);
		
		if((attr == 0xffffffff) ||
			(attr & FILE_ATTRIBUTE_DIRECTORY) != FILE_ATTRIBUTE_DIRECTORY)
			return FALSE;
		else
			return TRUE;
	}
	return FALSE;
}

BOOL ringDir::BeginFindFile(LPCTSTR szFilename,LPTSTR lpBuf)
{
	FindClose();

	memset(m_szCurrDir,0,MAX_PATH*sizeof(TCHAR));
	GetCurrentDirectory(MAX_PATH,m_szCurrDir);

	TCHAR szTmp[MAX_PATH];
	if(m_szDir[lstrlen(m_szDir)-1] != '\\')
		wsprintf(szTmp,"%s\\\0",m_szDir);
	else
		wsprintf(szTmp,"%s\0",m_szDir);

	if(IsExist())
	{
		SetCurrentDirectory(szTmp);
		if(szFilename == NULL)
			szFilename = "*.*";
		//wsprintf(szTmp,"%s\\%s\0",m_szDir,szFilename);
		m_handle = FindFirstFile(szFilename,&m_data);
		if(m_handle == INVALID_HANDLE_VALUE)
		{
			m_handle = NULL;
			SetCurrentDirectory(m_szCurrDir);
			return FALSE;
		}
		else if(lpBuf)
			wsprintf(lpBuf,"%s\0",m_data.cFileName);
		return TRUE;
	}
	return FALSE;
}

BOOL ringDir::FindNext(LPTSTR lpBuf)
{
	if(m_handle)
	{
		if(FindNextFile(m_handle,&m_data))
		{
			if(lpBuf)
				wsprintf(lpBuf,"%s\0",m_data.cFileName);
			return TRUE;
		}
	}
	return FALSE;
}

BOOL ringDir::FindClose()
{
	if(m_handle)
	{
		::FindClose(m_handle);
		m_handle = NULL;
	}
	if(*m_szCurrDir != '\0')
	{
		SetCurrentDirectory(m_szCurrDir);
		memset(m_szCurrDir,0,MAX_PATH*sizeof(TCHAR));
	}
	return TRUE;
}

LPCTSTR ringDir::Select(LPCTSTR lpInitDir/*=NULL*/,LPTSTR lpDir/*=NULL*/)
{
	if(lpDir == NULL)
		lpDir = m_szDir;

	BFFCALLBACK lpfn = NULL;
	LPARAM lParam = 0;

	if(lpInitDir && *lpInitDir != '\0')
	{
		//设置初始目录
		lParam = (LPARAM)lpInitDir;
		lpfn = (BFFCALLBACK)SelDirSpecInitProc;
	}
	if(SelectDir(lpDir,LANSTR_SELDIR,BIF_RETURNONLYFSDIRS,lpfn,lParam))
	{
		FindClose();
		return lpDir;
	}
	else
		return NULL;
}

INT CALLBACK SelDirSpecInitProc(HWND hwnd,UINT uMsg,LPARAM lp,LPARAM pData)
{
	if(uMsg == BFFM_INITIALIZED)
      SendMessage(hwnd,BFFM_SETSELECTION,TRUE,pData);

   return 0;
}

//相对移动到指定目录，m_szDir必须已有数据且以驱动器名开头，即"x:"格式
//支持\\和..，同DOS的CD命令
void ringDir::cd(LPCTSTR lpDir)
{
	if(*m_szDir != '\0' && m_szDir[1] == ':')
	{
		TCHAR *lps;
		while(lpDir && *lpDir != '\0')
		{
			if(*lpDir == '\\')
			{
				//退回到根目录
				memset(m_szDir + 2,0,sizeof(TCHAR)*(MAX_PATH-2));
				lpDir ++;
			}
			else if(*lpDir == '.' && lpDir[1] == '.')
			{
				//退回到上一级目录
				lps = m_szDir + lstrlen(m_szDir) - 1;
				switch(*lps)
				{
					case '\\':
						*lps = '\0';
						//don't break;
					case ':':
						lpDir += 2;
						break;
					default:
						lps --;
						while(*lps != '\\' && *lps != ':')
							*lps-- = '\0';
						if(*lps == '\\')
							*lps-- = '\0';
						lpDir += 2;
				}
				if(*lpDir == '.')
					return;			//非法路径，回退结束
				else if(*lpDir == '\\')
					lpDir ++;		//"..\",移动到"..\"后面一个字符，继续循环
			}
			else
			{
				lps = m_szDir + lstrlen(m_szDir);
				*lps++ = '\\';
				memset(lps,0,sizeof(TCHAR)*(MAX_PATH - (lps - m_szDir)));
				lstrcat(m_szDir,lpDir);
				return;
			}
		}
	}
}
