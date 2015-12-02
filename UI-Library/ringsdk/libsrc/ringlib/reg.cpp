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
//版本：0.1
//
//声明：本类库可以自由使用而不须对作者作出任何回报，但作者希望能得到
//		  你的鼓励和支持。你可以对类库源码作出修改和改进，但希望你能在
//		  修改的同时给作者一份同样的副本。
//		  本类库不得用于任何商业用途，如确实需要，请与作者联系。
//
//e-mail:ringphone@sina.com
//
//原文件名：reg.cpp
//
//说明：注册表操作类实现代码
//
**********************************************************************/
#define MAKE_SELF_LIB

//#ifndef __BORLANDC__
//#include "ringlib.h"
//#else
#include "ringlib.h"
//#endif

ringReg::ringReg()
{
	m_dwKeyCnt=0;
   m_dwKeyMax=0;
   m_lprv=NULL;
   m_hKey=NULL;
}

ringReg::~ringReg()
{
	Destroy();
}

void ringReg::Destroy()
{
	m_dwKeyCnt=0;
   m_dwKeyMax=0;

  	m_lprv = (LPREGVALINFO)Del(m_lprv);

   Close();
}

BOOL ringReg::error(HKEY hKey,LPCTSTR szSubKey)
{
  	char err[20];

   if(hKey == HKEY_LOCAL_MACHINE)
     	wsprintf(err,"HKEY_LOCAL_MACHINE");
   else if(hKey == HKEY_CLASSES_ROOT)
     	wsprintf(err,"HKEY_CLASSES_ROOT");
   else if(hKey == HKEY_CURRENT_USER)
     	wsprintf(err,"HKEY_CURRENT_USER");
   else if(hKey == HKEY_USERS)
     	wsprintf(err,"HKEY_USERS");
   else
    	wsprintf(err,"");

	Errmsg("错误：不能创建注册表项：%s\\%s",err,szSubKey);
	return FALSE;
}

BOOL ringReg::Set(HKEY hKey,LPCTSTR szSubKey,LPCTSTR szKey,LPCTSTR szVal)
{
   HKEY hKey1;
   LONG lRetCode;
   DWORD dwDisposition;

   Close();

   if(hKey==HKEY_LOCAL_MACHINE || hKey==HKEY_CLASSES_ROOT || hKey==HKEY_CURRENT_USER || hKey==HKEY_USERS)
   {

   	if(!szSubKey)

      	return FALSE;


   	lRetCode=RegCreateKeyEx(hKey,szSubKey,0, NULL, REG_OPTION_NON_VOLATILE,

                        KEY_ALL_ACCESS,NULL,&hKey1,&dwDisposition);
   	if (lRetCode != ERROR_SUCCESS)
   		return error(hKey,szSubKey);

      lRetCode=RegSetValueEx(hKey1,szKey,NULL,REG_SZ,(LPBYTE)szVal,strlen(szVal));
      RegCloseKey(hKey1);
   }
   else
      lRetCode=RegSetValueEx(hKey,szKey,NULL,REG_SZ,(LPBYTE)szVal,strlen(szVal));

   return (lRetCode == ERROR_SUCCESS);
}

BOOL ringReg::Set(HKEY hKey,LPCTSTR szSubKey,LPCTSTR szKey,DWORD dwVal)
{
   HKEY hKey1;
   LONG lRetCode;
   DWORD dwDisposition;

   Close();

   if(hKey==HKEY_LOCAL_MACHINE || hKey==HKEY_CLASSES_ROOT || hKey==HKEY_CURRENT_USER || hKey==HKEY_USERS)
   {

   	if(!szSubKey)

      	return FALSE;

      lRetCode=RegCreateKeyEx(hKey,szSubKey,0, NULL, REG_OPTION_NON_VOLATILE,
                        KEY_ALL_ACCESS,NULL,&hKey1,&dwDisposition);
   	if (lRetCode != ERROR_SUCCESS)
   		return error(hKey,szSubKey);

      lRetCode=RegSetValueEx(hKey1,szKey,NULL,REG_DWORD,(LPBYTE)&dwVal,sizeof(DWORD));
      RegCloseKey(hKey1);
   }
   else
	   lRetCode=RegSetValueEx(hKey,szKey,NULL,REG_DWORD,(LPBYTE)&dwVal,sizeof(DWORD));

   return (lRetCode == ERROR_SUCCESS);
}

BOOL ringReg::Set(HKEY hKey,LPCTSTR szSubKey,LPCTSTR szKey,LPVOID lpData,DWORD dwType,DWORD cbData)
{
   HKEY hKey1;
   LONG lRetCode;
   DWORD dwDisposition;

   Close();

   if(hKey==HKEY_LOCAL_MACHINE || hKey==HKEY_CLASSES_ROOT || hKey==HKEY_CURRENT_USER || hKey==HKEY_USERS)
   {

   	if(!szSubKey)

      	return FALSE;

      lRetCode = RegCreateKeyEx(hKey,szSubKey,0, NULL, REG_OPTION_NON_VOLATILE,
                        KEY_ALL_ACCESS,NULL,&hKey1,&dwDisposition);
   	if (lRetCode != ERROR_SUCCESS)
   		return error(hKey,szSubKey);

      lRetCode = RegSetValueEx(hKey1,szKey,NULL,dwType,(LPBYTE)lpData,cbData);
      RegCloseKey(hKey1);
   }
   else
	   lRetCode=RegSetValueEx(hKey,szKey,NULL,dwType,(LPBYTE)lpData,cbData);

   return (lRetCode == ERROR_SUCCESS);
}

BOOL ringReg::Check(DWORD dwNum)
{
	if(!m_lprv)
   {
   	m_lprv = (LPREGVALINFO)New(dwNum*sizeof(REGVALINFO));
   	if(!m_lprv)
   		return FALSE;
      else
      {
      	m_dwKeyMax = dwNum;
         m_dwKeyCnt = 0;
      }
   }
   else
   	if(m_dwKeyCnt >= m_dwKeyMax)
      	return FALSE;
   return TRUE;
}

BOOL ringReg::Add(LPCTSTR szKey,LPVOID lpData,DWORD dwType,DWORD cbData,DWORD dwNum)
{
	if(!Check(dwNum))
   	return FALSE;

   m_lprv[m_dwKeyCnt].szValName = szKey;
	m_lprv[m_dwKeyCnt].dwType = dwType;
   m_lprv[m_dwKeyCnt].lpData = lpData;
   m_lprv[m_dwKeyCnt].cbData = cbData;

   m_dwKeyCnt++;

   return TRUE;
}

BOOL ringReg::Add(LPCTSTR szKey,DWORD dwData,DWORD dwNum)
{
	if(!Check(dwNum))
   	return FALSE;

   m_lprv[m_dwKeyCnt].szValName = szKey;
	m_lprv[m_dwKeyCnt].dwType = REG_DWORD;
   m_lprv[m_dwKeyCnt].dwData = dwData;
   m_lprv[m_dwKeyCnt].lpData = (LPBYTE)&m_lprv[m_dwKeyCnt].dwData;
   m_lprv[m_dwKeyCnt].cbData = sizeof(DWORD);

   m_dwKeyCnt++;

   return TRUE;
}

BOOL ringReg::Add(LPCTSTR szKey,LPCTSTR szData,DWORD dwNum)
{
	if(!Check(dwNum))
   	return FALSE;

   m_lprv[m_dwKeyCnt].szValName = szKey;
	m_lprv[m_dwKeyCnt].dwType = REG_SZ;
   m_lprv[m_dwKeyCnt].lpData = (LPBYTE)szData;
   m_lprv[m_dwKeyCnt].cbData = strlen(szData);

   m_dwKeyCnt++;

   return TRUE;
}

BOOL ringReg::Set(HKEY hKey,LPCTSTR szSubKey)
{
   HKEY hKey1;
   LONG   lRetCode;
   BOOL bUseKeyName;
   DWORD  dwDisposition;
   DWORD i;

   Close();

   if(hKey==HKEY_LOCAL_MACHINE || hKey==HKEY_CLASSES_ROOT || hKey==HKEY_CURRENT_USER || hKey==HKEY_USERS)
   {

   	if(!szSubKey)

      	return FALSE;

      bUseKeyName = TRUE;

   	lRetCode=RegCreateKeyEx(hKey,szSubKey,0, NULL, REG_OPTION_NON_VOLATILE,
                        KEY_ALL_ACCESS,NULL,&hKey1,&dwDisposition);
   	if (lRetCode != ERROR_SUCCESS)
   		return error(hKey,szSubKey);
   }
   else
   {
   	bUseKeyName = FALSE;
   	if(hKey)
	     	hKey1 = hKey;
      else
      	return FALSE;
   }

   for(i=0;i<m_dwKeyCnt;i++)

   {

   	lRetCode = RegSetValueEx(hKey1,m_lprv[i].szValName,NULL,m_lprv[i].dwType,(LPBYTE)m_lprv[i].lpData,m_lprv[i].cbData);

   	if (lRetCode != ERROR_SUCCESS)
     	{
      	Errmsg("错误：写注册表失败");
         if(bUseKeyName)
         	RegCloseKey(hKey1);
         Destroy();
      	return FALSE;
      }
   }

   if(bUseKeyName)
     	RegCloseKey(hKey1);

   Destroy();
   return TRUE;
}

BOOL ringReg::Get(HKEY hKey,LPCTSTR szSubKey,LPCTSTR szKey,LPVOID lpData,DWORD dwDataSize)
{
   if(!szSubKey || szSubKey[0]=='\0')
   	return FALSE;

   DWORD dwType;

   Close();

   if(RegOpenKeyEx(hKey,szSubKey,0,KEY_ALL_ACCESS,&m_hKey) != ERROR_SUCCESS)
   {
   	m_hKey = NULL;
   	return FALSE;
   }

   return(RegQueryValueEx(m_hKey,szKey,NULL,&dwType,(LPBYTE)lpData,&dwDataSize) == ERROR_SUCCESS);
}

DWORD ringReg::GetInt(HKEY hKey,LPCTSTR szSubKey,LPCTSTR szKey,DWORD dwDef)
{
   if(!szSubKey || szSubKey[0]=='\0')
   	return FALSE;

   DWORD dwType,dwDataSize,dwVal;

   Close();

   if(RegOpenKeyEx(hKey,szSubKey,0,KEY_ALL_ACCESS,&m_hKey) != ERROR_SUCCESS)
   {
   	m_hKey = NULL;
		return dwDef;
   }
   dwDataSize = sizeof(DWORD);

   if(RegQueryValueEx(m_hKey,szKey,NULL,&dwType,(LPBYTE)&dwVal,&dwDataSize) == ERROR_SUCCESS)
   	dwDef = dwVal;

  	return dwDef;
}

BOOL ringReg::Get(LPCTSTR szKey,LPVOID lpData,DWORD dwDataSize)
{
   if(!m_hKey)
   	return FALSE;

	DWORD dwType;

   return (RegQueryValueEx(m_hKey,szKey,NULL,&dwType,(LPBYTE)lpData,&dwDataSize) == ERROR_SUCCESS);
}

DWORD ringReg::GetInt(LPCTSTR szKey,DWORD dwDef)
{
   if(!m_hKey)
   	return dwDef;

   DWORD dwType,dwDataSize,dwVal;

   dwDataSize = sizeof(DWORD);

   if(RegQueryValueEx(m_hKey,szKey,NULL,&dwType,(LPBYTE)&dwVal,&dwDataSize)  == ERROR_SUCCESS)
   	dwDef = dwVal;

   return dwDef;
}

BOOL ringReg::Delete(HKEY hKey,LPCTSTR szSubKey,LPCTSTR szKey)
{
	LONG lResult;
	HKEY hKey1;
	
	Close();
	
	lResult = RegOpenKeyEx(hKey,szSubKey,0,KEY_ALL_ACCESS,&hKey1);
	if(lResult != ERROR_SUCCESS)
		return FALSE;
	
	lResult = RegDeleteValue(hKey1,szKey);
	RegCloseKey(hKey1);
	
	return (lResult == ERROR_SUCCESS);
}

BOOL ringReg::Delete(HKEY hKey,LPCTSTR szSubKey)
{
   Close();
	if(OSType()==NT)
   	return DelNT(hKey,szSubKey);
   else
   	return (RegDeleteKey(hKey,szSubKey) == ERROR_SUCCESS);
}

BOOL ringReg::DelNT(HKEY hKey,LPCTSTR szSubKey)
{
	LONG lResult;
	HKEY hKey1;
	DWORD dwIndex=0;
	char szKeyName[300];
	char szKey[500];
	DWORD dwKeySize=300;
	
	lResult = RegOpenKeyEx(hKey,szSubKey,0,KEY_ALL_ACCESS,&hKey1);
	if(lResult != ERROR_SUCCESS)
		return FALSE;
	
	while(lResult == ERROR_SUCCESS)
	{
		lResult = RegEnumKeyEx(hKey1,dwIndex,szKeyName,&dwKeySize,NULL,NULL,NULL,NULL);
		if(lResult == ERROR_NO_MORE_ITEMS)
		{
			RegCloseKey(hKey1);
			return (RegDeleteKey(hKey,szSubKey)==ERROR_SUCCESS);
		}
		else if(lResult != ERROR_SUCCESS)
		{
			RegCloseKey(hKey1);
			return FALSE;
		}
		else
		{
			wsprintf(szKey,"%s\\%s\0",szSubKey,szKeyName);
			if(!DelNT(hKey,szKey))
			{
				RegCloseKey(hKey1);
				return FALSE;
			}
		}
		dwIndex++;
	}
	return TRUE;
}

void ringReg::Close()
{
	if(m_hKey!=NULL)
		RegCloseKey(m_hKey);
	m_hKey = NULL;
}

///////////////////////////////////////////////////////////////////////
//
//配置文件操作类
//
///////////////////////////////////////////////////////////////////////

ringINI::ringINI()
{
	ZeroMemory(m_filename,MAX_PATH);
   bInited = FALSE;
}

ringINI::ringINI(LPCTSTR szFile)
{
	SetFile(szFile);
}

ringINI::~ringINI()
{
	ZeroMemory(m_filename,MAX_PATH);
   bInited = FALSE;
}

void ringINI::SetFile(LPCTSTR format,...)
{
	va_list ap;

   if(m_filename[0] != '\0')
		ZeroMemory(m_filename,MAX_PATH);

	if(format)
   {
		va_start(ap, format);
		_vsntprintf(m_filename,MAX_PATH-2,format,ap);
		//vsprintf(m_filename, format, ap);
		va_end(ap);
	}

   if(m_filename[0] != '\0')
	   bInited = TRUE;
   else
   	bInited = FALSE;
}

int ringINI::GetInt(LPCTSTR section,LPCTSTR key,int nDefault/* = 0*/)
{
	if(bInited)
   	return GetPrivateProfileInt(section,key,nDefault,m_filename);
   else
   	return nDefault;
}

DWORD ringINI::GetString(LPCTSTR section,LPCTSTR key,LPTSTR szBuf,int nBufSize,LPCTSTR szDefault/*=NULL*/)
{
   if(szDefault == NULL)
   	szDefault = "";
      
	if(bInited)
   	return GetPrivateProfileString(section,key,szDefault,szBuf,nBufSize,m_filename);
   else
   	return 0;
}

BOOL ringINI::Write(LPCTSTR section,LPCTSTR key,LPCTSTR szBuf)
{
	if(bInited)
   	return WritePrivateProfileString(section,key,szBuf,m_filename);
   else
   	return FALSE;
}

BOOL ringINI::Write(LPCTSTR section,LPCTSTR key,int nVal)
{
   char s[20];

   wsprintf(s,"%d\0\0",nVal);

	if(bInited)
   	return WritePrivateProfileString(section,key,s,m_filename);
   else
   	return FALSE;
}

BOOL ringINI::DeleteKey(LPCTSTR section,LPCTSTR szKey)
{
	if(bInited)
		return WritePrivateProfileString(section,szKey,NULL,m_filename);
   else
		return FALSE;
}

BOOL ringINI::DeleteSection(LPCTSTR section)
{
	if(bInited)
		return WritePrivateProfileString(section,NULL,NULL,m_filename);
   else
		return FALSE;
}

DWORD ringINI::GetSections(LPTSTR lpBuffer,int nSize)
{
	if(bInited)
		return GetPrivateProfileSectionNames(lpBuffer,nSize,m_filename);
   else
		return 0;
}

DWORD ringINI::GetKeys(LPCTSTR szSec,LPTSTR lpBuffer,int nSize)
{
	if(bInited)
		return GetPrivateProfileSection(szSec,lpBuffer,nSize,m_filename);
   else
		return 0;
}
