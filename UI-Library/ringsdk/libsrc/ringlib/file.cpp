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
//		  在此感谢YZFree对代码做了大量的编写及修正工作，然后我又做了一些
//		  修正，所以已经分不清哪些代码是谁写的了:-P
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
#include <shlobj.h>

//因为Win2000以前的windows该结构不一样，因此这里统一起来，使这些版本的尺寸一致
typedef struct tagRingOFN
{
	OPENFILENAME m_ofn;
#if(_WIN32_WINNT < 0x0500)
	void *        pvReserved;
	DWORD         dwReserved;
	DWORD         FlagsEx;
#endif
}RINGOPENFILENAME, *LPRINGOPENFILENAME;

ringFile::ringFile()
{
	Init();
}

ringFile::ringFile(LPCTSTR szFilename)
{
	Init();
	SetFile(szFilename);
}

ringFile::~ringFile()
{
	Destroy();
}

// 初始化类内部成员变量
void ringFile::Init()
{
	memset(m_szFilename,0,MAX_PATH);
	memset(m_szDrive,0,4);
	m_lpszPath = NULL;
	m_lpszFilename = NULL;
	m_lpszTitlename = NULL;
	m_lpszExtname = NULL;
	
	m_hFile			= NULL;
	m_dwRWNum		= 0UL;
	m_bufSize		= 0;

	m_lpFilebuf		= NULL;
	m_dwFileAttr	= 0xffffffff;
	m_dwFileSize	= 0;
	m_bExtern      = FALSE;
	m_aAttrib		= RFA_VOID;
}

void ringFile::SetFile(HANDLE hFile)
{
	Close();
	ClearFileName();
	
	m_hFile   = hFile;
	m_bExtern = TRUE;
	m_aAttrib = RFA_FILE;
	
	BY_HANDLE_FILE_INFORMATION	bhfi;
	
	::GetFileInformationByHandle(hFile, &bhfi);
	
	m_dwFileAttr = bhfi.dwFileAttributes;
	m_dwFileSize = bhfi.nFileSizeLow;
}

/////////////////////////////////////////////////////////////////////////////////////////
//
//设定文件名，并转换为绝对路径
//
void ringFile::SetFile(LPCTSTR format, ...)
{
	va_list ap;
	TCHAR	szTemp[MAX_PATH],tmp[MAX_PATH];

	// 如果以前存在已打开的文件，则关闭它
	Close();
	ClearFileName();
	// 合成路径
	if(format)
	{
		va_start(ap, format);
		_vsntprintf((char*)szTemp,MAX_PATH-2,format,ap);
		//vsprintf((char*)szTemp,format,ap);
		va_end(ap);
	}

	//记录当前路径
	memset(tmp,0,MAX_PATH);
	GetCurrentDirectory(MAX_PATH,tmp);
	//设置当前路径为程序路径
	ringStr str(RSTR_CURRPATH);
	SetCurrentDirectory(str.string());
	// 转换为绝对路径
	if(!GetFullPathName(szTemp,MAX_PATH,m_szFilename,(LPSTR*)&m_lpszFilename))
	{
		*m_szFilename = '\0';
		SetCurrentDirectory(tmp);
		return;
	}
	//还原当前路径
	SetCurrentDirectory(tmp);

	m_aAttrib = check();
}

/////////////////////////////////////////////////////////////////////////////////////////
//
//只设置文件名，SetFile调用结果m_szFilename可能与输入参数不一致，本函数不改变，
//调用结束后m_szFilename可能与输入参数完全一致
//
void ringFile::SetFileName(LPCTSTR szFile)
{
	Close();
	ClearFileName();
	// 复制新的文件名串
	if(szFile)
	{
		strncpy(m_szFilename,szFile,MAX_PATH);

		m_aAttrib = check();
		//提取文件名位置
		m_lpszFilename = m_szFilename + strlen(m_szFilename);
		if(m_aAttrib != RFA_PATH)
		{
			m_lpszFilename --;
			while(m_lpszFilename != m_szFilename)
				if(*m_lpszFilename == '\\' || *m_lpszFilename == ':')
				{
					m_lpszFilename ++;
					return;
				}
				else
					m_lpszFilename --;
		}
	}
}

/////////////////////////////////////////////////////////////////////////////////////////
//
//通过文件对话框选取文件
//
//入口：
//		szFileType - 过滤串
//		szTitle    - 对话框标题串
//		bSave      - ‘打开’/‘另存为’对话框选择标签
//		hWnd       - 对话框父窗口
//		hookproc   - 对话框钩子函数
//
//返回：
//		TRUE - 关联成功、FALSE - 失败
//
///////////////////////////////////////////////////////////////////////////////////////
int ringFile::Select(LPCTSTR szFileType,LPCTSTR szTitle,BOOL bSave/*=FALSE*/, 
							HWND hWnd/*=m_hWnd*/,LPCTSTR dlgEx/*=NULL*/,LPOFNHOOKPROC hookproc/*=NULL*/,
							LONG lCustData/*=0*/)
{
	RINGOPENFILENAME	ofnTemp;
	DWORD			Errval;
	TCHAR			d[MAX_PATH],f[MAX_PATH],*p,*v;
	
	memset(d,0,MAX_PATH);

	if(szTitle == NULL)
		szTitle = LANSTR_FILEOPEN;
	
	// 规格化过滤串
	int len = MAX_PATH * sizeof(TCHAR);
	int n0cnt = 0;
	p = (LPTSTR)szFileType;
	v = f;
	memset(f,0,len);

	for(int i=0;i<len;i++)
	{
		if(*p == '|' || *p == '\0')
		{
			*v++ = '\0';
			p++;
			n0cnt ++;
			if(n0cnt > 1)
				break;
		}
		else
		{
			*v++ = *p++;
			n0cnt = 0;
		}		
	}
#if(_WIN32_WINNT < 0x0500)
	n0cnt = 0;
#else
	n0cnt = 12;
#endif
	
	if(OSType() < OST_WIN2KPRO)
		ofnTemp.m_ofn.lStructSize = sizeof( OPENFILENAME ) - n0cnt;
	else
		ofnTemp.m_ofn.lStructSize = sizeof(RINGOPENFILENAME);

	ofnTemp.m_ofn.hwndOwner         = hWnd;
	ofnTemp.m_ofn.hInstance         = GetInstance();
	ofnTemp.m_ofn.lpstrFilter       = f;
	ofnTemp.m_ofn.lpstrCustomFilter = NULL;
	ofnTemp.m_ofn.nMaxCustFilter    = 0;
	ofnTemp.m_ofn.nFilterIndex      = 1;
	ofnTemp.m_ofn.lpstrFile         = d;
	ofnTemp.m_ofn.nMaxFile          = MAX_PATH;
	ofnTemp.m_ofn.lpstrFileTitle    = NULL;
	ofnTemp.m_ofn.nMaxFileTitle     = NULL;
	ofnTemp.m_ofn.lpstrInitialDir   = NULL;//GetSpecPath(RFSP_CURRPATH);
	ofnTemp.m_ofn.lpstrTitle        = szTitle;	
	
	if(dlgEx && hookproc)
		ofnTemp.m_ofn.Flags = OFN_ENABLEHOOK | OFN_EXPLORER |OFN_ENABLETEMPLATE|OFN_ENABLESIZING;
	else
		ofnTemp.m_ofn.Flags = OFN_EXPLORER;
	
	ofnTemp.m_ofn.nFileOffset    = NULL;
	ofnTemp.m_ofn.nFileExtension = 0;
	ofnTemp.m_ofn.lpstrDefExt    = "*";
	ofnTemp.m_ofn.lCustData      = lCustData;
	ofnTemp.m_ofn.lpfnHook       = hookproc;
	ofnTemp.m_ofn.lpTemplateName = dlgEx;
	
	ofnTemp.FlagsEx = 0;

	if(!bSave)
		bSave = GetOpenFileName((LPOPENFILENAME)&ofnTemp);
	else
		bSave = GetSaveFileName((LPOPENFILENAME)&ofnTemp);
	
	if(!bSave)
	{
		Errval = CommDlgExtendedError();
		
		if(Errval != 0)
		{
			memset(f,0,len);
			if(GetErrMessage(Errval,f,MAX_PATH))
				Errmsg(f);
		}
		return FALSE;
	}
	
	SetFile(d);
	
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////////////////
//
//创建/打开文件
//
//特性：文件不存在--创建，文件存在--打开文件并把属性设为NORMAL，即可以写只读文件。关闭
//		文件时恢复文件原属性。根据参数dwCreate：
//			 = RF_NEW -- 清除文件，截断为0
//			 = RF_APPEND -- 文件指针移到文件尾
//			 = RF_EDIT -- 打开文件
//
//返回：文件不存在--FALSE，成功--TRUE
//
BOOL ringFile::Create(DWORD dwCreate,DWORD dwAttr/*=FILE_ATTRIBUTE_NORMAL*/)
{
	DWORD dwMode = RF_EDIT;
	
	if(m_hFile)
	{
      if(dwCreate == RF_APPEND)
			Seek(0,FILE_END);
      else if(dwCreate == RF_NEW)
			Seek(0);
		return TRUE;
   }

	switch(m_aAttrib)
	{
	case RFA_FILE:
		//临时文件无须设置属性
		if((m_dwFileAttr & FILE_ATTRIBUTE_TEMPORARY) != FILE_ATTRIBUTE_TEMPORARY)
			SetFileAttributes(m_szFilename,FILE_ATTRIBUTE_NORMAL);

		if(dwCreate == RF_NEW)
			dwMode = TRUNCATE_EXISTING;
		break;
	case RFA_NOTEXIST:
		MkDir(Pathname());

		m_dwFileAttr = dwAttr;
		break;
	default:
		return FALSE;
	}
	
	m_hFile = CreateFile(m_szFilename,GENERIC_READ | GENERIC_WRITE,FILE_SHARE_READ | FILE_SHARE_WRITE,NULL,dwMode,FILE_ATTRIBUTE_NORMAL,NULL);
	
	if(m_hFile==INVALID_HANDLE_VALUE)
	{
		m_hFile = NULL;
		m_dwFileAttr = 0xffffffff;
		m_aAttrib = RFA_NOTEXIST;

		return FALSE;
	}
	else
	{
		if(dwCreate == RF_APPEND)
			SeekToEnd();
		
		m_dwFileSize = ::GetFileSize(m_hFile,NULL);
		return TRUE;
	}
}

BOOL ringFile::Create(LPCTSTR lpFilename,DWORD dwCreate)
{
	SetFile(lpFilename);
	return Create(dwCreate);
}

BOOL ringFile::CreateTemp(LPCTSTR prefix)
{
	char	path[MAX_PATH];
	char	name[MAX_PATH];
	char	pfix[4];
	
	// 合成前缀串
	if(prefix)
	{
		memcpy((void*)pfix,(const void *)prefix, 3);
		pfix[3] = '\0';
	}
	else
		wsprintf(pfix,"RFT\0");

	// 获得临时文件全路径
	GetTempPath(MAX_PATH,path);
	GetTempFileName((LPCSTR)path,(LPCSTR)pfix,0,(LPSTR)name);

	if (strlen((char*)name) == 0)
		return FALSE;
	// 进行预关联
	SetFile((LPCTSTR)name);
	// 进行实关联
	return Create(RF_NEW,FILE_ATTRIBUTE_TEMPORARY);
}

////////////////////////////////////////////////
//
//打开文件
//
//返回：
//		文件不存在--FALSE，成功--TRUE
//
BOOL ringFile::Open(DWORD dwAction,DWORD dwShare,DWORD dwAttr)
{
	if(m_hFile)
	{
		Seek(0);
		return TRUE;
   }

	m_hFile = CreateFile(m_szFilename,dwAction,dwShare,NULL,OPEN_EXISTING,dwAttr,NULL);
	
	if(m_hFile == INVALID_HANDLE_VALUE)
	{
		m_hFile = NULL;
		return FALSE;
	}
	else
	{
		m_dwFileAttr = GetFileAttributes(m_szFilename);
		m_dwFileSize = ::GetFileSize(m_hFile,NULL);
		return TRUE;
	}
}

////////////////////////////////////////////////
//
//打开文件（包括预关联操作）
//
//返回：
//		文件不存在--FALSE，成功--TRUE
//
BOOL ringFile::Open(LPCTSTR lpFilename, DWORD dwAction, DWORD dwShare,DWORD dwAttr)
{
	// 预关联
	SetFile(lpFilename);
	return Open(dwAction, dwShare,dwAttr);
}

////////////////////////////////////////////////
//
//关闭文件
//
void ringFile::Close()
{
	if(m_hFile)
	{
		if(m_bExtern)
			m_hFile = NULL;	// 导入型的关联不负责关闭文件
		else
		{
			CloseHandle(m_hFile);
			m_hFile=NULL;

			// 恢复属性
			if(m_dwFileAttr != GetFileAttributes(m_szFilename))
				SetFileAttributes(m_szFilename,m_dwFileAttr);
		}
		m_bExtern = FALSE;
	}
}

void ringFile::ClearFileName()
{
	memset(m_szFilename,0,MAX_PATH);
	if(m_lpszPath)
		memset((LPSTR)m_lpszPath,0,MAX_PATH);
	m_lpszFilename = NULL;
	if(m_lpszTitlename)
		memset((LPSTR)m_lpszTitlename,0,MAX_PATH);
	m_lpszExtname = NULL;
	memset(m_szDrive,0,4);
		
	m_dwFileAttr	= 0xffffffff;
	m_dwFileSize	= 0;
	m_aAttrib		= RFA_VOID;
		
	if(m_lpFilebuf)
		memset(m_lpFilebuf,0,m_bufSize);		
}

// 解关联
void ringFile::Destroy()
{
	Close();

	memset(m_szFilename,0,MAX_PATH);
	if(m_lpszPath)
		m_lpszPath = (LPTSTR)Del(m_lpszPath);
	m_lpszFilename = NULL;
	if(m_lpszTitlename)
		m_lpszTitlename = (LPTSTR)Del(m_lpszTitlename);
	m_lpszExtname = NULL;
	memset(m_szDrive,0,4);

	m_dwFileAttr	= 0xffffffff;
	m_dwFileSize	= 0;
	m_aAttrib		= RFA_VOID;

	if(m_lpFilebuf)
		m_lpFilebuf = Del(m_lpFilebuf);
	m_bufSize = 0;
}

/////////////////////////////////////////////////////////////////////////////////////////
//
BOOL ringFile::Read(LPVOID lpBuf,DWORD dwNum,BOOL bCheckReadNum,LPDWORD lpTrueReadNum)
{
	if(lpBuf)
	{
		try
		{
			memset(lpBuf,0,dwNum);
		}
		catch(...)
		{
			return FALSE;
		}
	}

	if(ReadFile(m_hFile,lpBuf,dwNum,&m_dwRWNum,NULL))
	{
		if(lpTrueReadNum)
			*lpTrueReadNum = m_dwRWNum;
		
		if(m_dwRWNum == 0 || (bCheckReadNum && (dwNum != m_dwRWNum)))
			return FALSE;
		else
			return TRUE;		
	}
	else
	{
		m_dwRWNum = 0UL;

		if (lpTrueReadNum)
			*lpTrueReadNum = 0UL;
		
		return FALSE;
	}
}

//读取一行，文件结束返回-1，行超长（>dwNum）返回-2,其他返回-3
int ringFile::ReadLine(LPVOID lpBuf,DWORD dwNum)
{
	if(lpBuf)
	{
		try
		{
			memset(lpBuf,0,dwNum);
			
			char szline[1024];
			memset(szline,0,1024);

			int len = -1,pos = GetRWLocation();
			DWORD num;
			
			if(Read(szline,1023,FALSE,&num))
			{
				for(int i=0;i<1023;i++)
				{
					if(szline[i] == '\r')
					{
						if(szline[i+1] == '\n')
							pos += 2;
						else
							pos ++;
						
						szline[i] = '\0';
						len = i;
						break;
					}
					else if(szline[i] == '\n')
					{
						pos ++;
						szline[i] = '\0';
						len = i;
						break;
					}
				}
				
				if(len == -1 && num == 1023)
					return RFE_LINEOVERFLOW;
				else
					len = strlen(szline);
				
				if(dwNum > (DWORD)len)
				{
					memcpy(lpBuf,szline,len);
					Seek(pos + len);
					return len;
				}
			}
			else
				return RFE_EOF;
		}
		catch(...)
		{			
		}
	}
	return RFE_ERROR;
}

int ringFile::ReadAll(char **ppbuf)
{
	if(m_dwFileSize > RF_READALL_SIZE)
		return RFE_FILETOOBIG;		//文件超过预定全读尺寸，放弃
	
	// 初始化返回数据
	if(ppbuf != 0)
		*ppbuf = NULL;

	m_dwRWNum = 0UL;
	
	// 申请缓冲区
	char	*lpbuf;
	if(m_lpFilebuf && m_dwFileSize > m_bufSize)
		lpbuf = (char*)New(m_dwFileSize + 10);
	else if(m_lpFilebuf == NULL)
	{
		m_lpFilebuf = (char*)New(m_dwFileSize + 10);
		lpbuf = (char*)m_lpFilebuf;
		m_bufSize = m_dwFileSize + 10;
	}
	else
	{
		lpbuf = (char*)m_lpFilebuf;
		memset(lpbuf,0,m_bufSize);
	}

	if(lpbuf == NULL)
		return RFE_MEMERROR;
	
	DWORD	trn;
	DWORD	pos;

	// 保留读写位置
	pos = GetRWLocation();

	SeekToBegin();

	// 读入所有文件内容
	if(!ReadFile(m_hFile,lpbuf,m_dwFileSize,&trn,NULL))
	{
		if(lpbuf != m_lpFilebuf)
			Del(lpbuf);
		return RFE_ERROR;
	}

	Seek(pos);

	if(trn != m_dwFileSize)
	{
		if(lpbuf != m_lpFilebuf)
			Del(lpbuf);
		return RFE_ERROR;
	}

	// 返回数据
	if(lpbuf != m_lpFilebuf)
	{
		if(m_lpFilebuf)
			Del(m_lpFilebuf);
		m_lpFilebuf = (LPVOID)lpbuf;
		m_bufSize = m_dwFileSize;
	}

	if(ppbuf)
	{
		*ppbuf = (char*)New(m_dwFileSize + 10);
		memcpy(*ppbuf,lpbuf,m_dwFileSize);
	}		

	m_dwRWNum = m_dwFileSize;

	return m_dwFileSize;
}

BOOL ringFile::Write(const LPVOID lpBuf,DWORD dwNum,BOOL bCheckWriteNum,LPDWORD lpTrueWriteNum)
{
	if(WriteFile(m_hFile,lpBuf,dwNum,&m_dwRWNum,NULL))
	{
		if (lpTrueWriteNum)
			*lpTrueWriteNum = m_dwRWNum;
		
		// 更新文件尺寸
		if(GetRWLocation() > m_dwFileSize)
			m_dwFileSize = GetRWLocation();
		
		if(m_dwRWNum == 0 || (bCheckWriteNum && (dwNum != m_dwRWNum)))
			return FALSE;
		else
			return TRUE;		
	}
	else
	{
		m_dwRWNum = 0UL;

		if (lpTrueWriteNum)
			*lpTrueWriteNum = 0UL;
		
		return FALSE;
	}
}

void ringFile::Flush()
{
	::FlushFileBuffers(m_hFile);
}

void ringFile::LockRWArea(DWORD dwPos,DWORD dwCount)
{
	::LockFile(m_hFile,dwPos,0,dwCount,0);
}

void ringFile::UnlockRWArea(DWORD dwPos, DWORD dwCount)
{
	::UnlockFile(m_hFile,dwPos,0,dwCount,0);
}

DWORD ringFile::Seek(LONG lDistance,DWORD dwMethod) const
{
	return ::SetFilePointer(m_hFile,lDistance,NULL,dwMethod);
}

DWORD ringFile::SeekToBegin() const
{
	return ::SetFilePointer(m_hFile,0,NULL,FILE_BEGIN);
}

DWORD ringFile::SeekToEnd() const
{
	return ::SetFilePointer(m_hFile,0,NULL,FILE_END);
}

DWORD ringFile::GetRWLocation() const
{
	return ::SetFilePointer(m_hFile,0,NULL,FILE_CURRENT);
}

BOOL ringFile::SetEndOfFile(int pos/*=-1*/)
{
	if(pos != -1)
		Seek(pos);

	if(m_hFile && ::SetEndOfFile(m_hFile))
	{
		m_dwFileSize = GetRWLocation();
		return TRUE;
	}
	else
		return FALSE;
}

/////////////////////////////////////////////////////////////////////////////////////////
//
//设置文件长度（字节单位）
//
void ringFile::SetSize(DWORD dwNewLen)
{
	if(m_hFile == NULL)
		Create();

	Seek(dwNewLen,FILE_BEGIN);
	
	SetEndOfFile();
}

/////////////////////////////////////////////////////////////////////////////////////////
//
//获取文件长度（字节单位）
//
DWORD ringFile::Size()
{
   check();
	   
   if(m_hFile != NULL)
     	m_dwFileSize = GetFileSize(m_hFile,NULL);
   else
   {
		Open();
      m_dwFileSize = GetFileSize(m_hFile,NULL);
		Close();
   }
	
	if(m_dwFileSize == 0xffffffff)	//GetFileSize函数失败
		m_dwFileSize = 0;
	
  	return m_dwFileSize;
}

LPCTSTR ringFile::Fullname() const
{
	return (LPCTSTR)m_szFilename;
}

LPCTSTR ringFile::Filename() const
{
	return (LPCTSTR)m_lpszFilename;
}

LPCTSTR ringFile::Extname() const
{
	if(m_lpszExtname == NULL)
		if(m_lpszFilename)
		{
			LPSTR p = (LPSTR)m_lpszFilename + strlen(m_lpszFilename) - 1;
			LPTSTR v = p + 1;
			while(p != m_lpszFilename)
			{
				if(*p == '.')
				{
					(LPSTR)m_lpszExtname = p + 1;
					return m_lpszExtname;
				}
				p --;
			}
			(LPSTR)m_lpszExtname = (v - m_szFilename)<MAX_PATH?v:NULL;
		}
	return (LPCTSTR)m_lpszExtname;
}

LPCTSTR ringFile::Pathname() const
{
	if(m_lpszPath == NULL)
		(LPTSTR)m_lpszPath = (LPTSTR)New(MAX_PATH);

	if(m_lpszPath && m_lpszPath[0] == '\0' && m_lpszFilename)
		strncpy((LPSTR)m_lpszPath,m_szFilename,(m_lpszFilename - m_szFilename - 1));

	return (LPCTSTR)m_lpszPath;
}

LPCTSTR ringFile::Drivename() const
{
	if(m_szFilename[1] == ':')
		memcpy((LPSTR)m_szDrive,m_szFilename,2);
	return (LPCTSTR)m_szDrive;
}

LPCTSTR ringFile::Titlename() const
{
	if(m_lpszTitlename == NULL)
		(LPTSTR)m_lpszTitlename = (LPTSTR)New(MAX_PATH);

	if(m_lpszTitlename && m_lpszTitlename[0] == '\0' && m_lpszFilename)
	{
		LPTSTR p = (LPTSTR)m_lpszFilename + strlen(m_lpszFilename) - 1;
		//LPTSTR v = p + 1;
		while(p != m_lpszFilename)
		{
			if(*p == '.')
			{
				//顺便置一下扩展名
				(LPSTR)m_lpszExtname = p + 1;
				memcpy((LPSTR)m_lpszTitlename,m_lpszFilename,p - m_lpszFilename);
				return m_lpszTitlename;
			}
			p --;
		}
		strcpy((LPSTR)m_lpszTitlename,m_lpszFilename);
	}
	return (LPCTSTR)m_lpszTitlename;
}

///////////////////////////////////////////////////////////////////////////////////////
//
BOOL ringFile::isExist(LPCTSTR lpPathFilename)
{
	if(lpPathFilename == NULL)
		return (check() < RFA_NOTEXIST);
	else
		return (GetFileAttributes(lpPathFilename) != 0xffffffff);
}

BOOL ringFile::isFile(LPCTSTR lpPathFilename)
{
	if(lpPathFilename == NULL)
		return (m_aAttrib == RFA_FILE);
	else
	{
		DWORD attr = GetFileAttributes(lpPathFilename);

		if((attr == 0xffffffff) ||
			(attr & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY)
			return FALSE;
		else
			return TRUE;
	}
}

BOOL ringFile::copyTo(LPCTSTR lpNewName,BOOL bForceOverWrite)
{
	if(lpNewName == NULL || *lpNewName == '\0')
		return FALSE;

	if(m_bExtern)
	{
		LPSTR lpbuf;
		int cnt = ReadAll(&lpbuf);
		if(cnt > 0)
		{
			ringFile rf = lpNewName;
			if(rf.isExist() && !bForceOverWrite)
				return FALSE;
			
			if(rf.Create(RF_NEW))
			{
				if(rf.Write(lpbuf,cnt))
					return TRUE;
				else
					return FALSE;
			}			
		}
		return FALSE;
	}

	char	dbuf[MAX_PATH];
	DWORD	attr = GetAttrib();

	if((attr & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY)
	{
		if(lpNewName[strlen(lpNewName)-1] == '\\')
			wsprintf(dbuf,"%s%s\0\0",lpNewName,Filename());
		else
			wsprintf(dbuf,"%s\\%s\0\0",lpNewName,Filename());
	}
	else
		strcpy((char*)dbuf, (const char *)lpNewName);
	
	// 拷贝文件
	return ::CopyFile(m_szFilename,dbuf,!bForceOverWrite);
}

BOOL ringFile::delIt()
{
	if(m_bExtern)
		return FALSE;

	Close();

	return DeleteFile(m_szFilename);	
}

BOOL ringFile::moveTo(LPCTSTR lpNewName,BOOL bForceOverWrite)
{
	if(m_bExtern || lpNewName == NULL || *lpNewName == '\0')
		return FALSE;

	char	dbuf[MAX_PATH];
	DWORD	attr = GetFileAttributes(lpNewName);
	
	if((attr & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY)
	{
		if(lpNewName[strlen(lpNewName)-1] == '\\')
			wsprintf(dbuf,"%s%s\0\0",lpNewName,Filename());
		else
			wsprintf(dbuf,"%s\\%s\0\0",lpNewName,Filename());
	}
	else
		strcpy(dbuf,lpNewName);

	DWORD	how = bForceOverWrite?MOVEFILE_REPLACE_EXISTING:0;

	DWORD	loc    = 0UL;
	BOOL bOpened = (BOOL)m_hFile;
	// 如果文件处于打开状态，则先关闭它
	if(m_hFile)
	{
		loc = GetRWLocation();
		Close();
	}

	// 移动文件
	BOOL bOK;
	
	if(OSType() == OST_WINNT)
		bOK = ::MoveFileEx(m_szFilename,dbuf,how);
	else
	{
		bOK = copyTo(dbuf,how);//ringFile rf = dbuf;
		if(bOK)
			delIt();
	}

	// 如果移动失败，恢复原状态
	if(!bOK)
	{
		if(bOpened)
		{
			Open(RF_READWRITE);
			Seek(loc);
		}
		return FALSE;
	}

	// 关联到新的位置
	SetFile(dbuf);

	if(bOpened)
	{
		Open(RF_READWRITE);
		Seek(loc);		
	}

	return TRUE;
}

BOOL ringFile::reName(LPCTSTR lpNewName)
{
	if(m_bExtern || lpNewName == NULL || *lpNewName == '\0')
		return FALSE;

	char	dbuf[MAX_PATH];
	char	szTemp[MAX_PATH];
	
	strcpy(szTemp,Pathname());
	
	if(szTemp[strlen(szTemp)-1] == '\\')
		szTemp[strlen(szTemp)-1] = 0;
	
	wsprintf(dbuf,"%s\\%s\0\0",szTemp,lpNewName);

	return moveTo((LPSTR)dbuf,FALSE);
}

/****************************************************
//
//原版本使用GetFileAttributes只能得到FAT的文件信息
//现已修正可适应NTFS
//
****************************************************/
DWORD ringFile::GetAttrib(LPCTSTR lpszfilename/*=NULL*/)
{
	LPCTSTR lps = lpszfilename;
	
	if(lps == NULL)
		lps = m_szFilename;

	WIN32_FILE_ATTRIBUTE_DATA wData;
	memset(&wData,0,sizeof(WIN32_FILE_ATTRIBUTE_DATA));

	if(GetFileAttributesEx(lps,GetFileExInfoStandard,&wData))
		return wData.dwFileAttributes;
	else
		return m_dwFileAttr;
}


BOOL ringFile::SetAttrib(DWORD dwFileAttributes)
{
	if(SetFileAttributes(m_szFilename,dwFileAttributes))
	{
		m_dwFileAttr = dwFileAttributes;
		return TRUE;
	}
	return FALSE;
}

BOOL ringFile::GetFileTime(LPFILETIME lpCreate, LPFILETIME lpLast, LPFILETIME lpModify)
{
	BOOL result;

	if(m_hFile)
		result = ::GetFileTime(m_hFile,lpCreate,lpLast,lpModify);
	else
	{
		if(MASK_MATCH(GetAttrib(),FILE_ATTRIBUTE_DIRECTORY))
			result = Open(RF_READ,RF_SHARE_READ,FILE_FLAG_BACKUP_SEMANTICS);
		else
			result = Open();

		if(result)
			result = ::GetFileTime(m_hFile,lpCreate,lpLast,lpModify);

		Close();
	}

	return result;
}

//如果文件已打开且以只读方式打开，失败
BOOL ringFile::SetFileTime(LPFILETIME lpCreate, LPFILETIME lpLast, LPFILETIME lpModify)
{
	BOOL bOpen = FALSE;

	if(!m_hFile)
	{
		if(MASK_MATCH(GetAttrib(),FILE_ATTRIBUTE_DIRECTORY))
			bOpen = Create(RF_EDIT,FILE_FLAG_BACKUP_SEMANTICS);
		else
			bOpen = Create(RF_EDIT);
	}

	// 设置时间	
	BOOL result = ::SetFileTime(m_hFile,lpCreate,lpLast,lpModify);

	if(bOpen)
		Close();

	return result;
}


//////////////////////////////////////////////////////////////////////////////////////
//
BOOL ringFile::SelectDir(LPTSTR lpDir,LPCTSTR lpszTitle/*="选择路径"*/,
								 UINT uFlag/*=BIF_RETURNONLYFSDIRS*/,BFFCALLBACK lpfn/*=NULL*/,
								 LPARAM lParam/*=0*/,int iImage/*=0*/)
{
	return ::SelectDir(lpDir,lpszTitle,uFlag,lpfn,lParam,iImage);
}

BOOL ringFile::mdir(LPCTSTR szPath)
{
	return ::MkDir(szPath);
}

HINSTANCE ringFile::exec(LPCTSTR lpFile,LPCTSTR lpDir/*=NULL*/,LPCTSTR lpParam/*=NULL*/,LPCTSTR lpOp/*=NULL*/,INT nShowCmd/*=SW_SHOW*/)
{
	return ::ringExec(lpFile, lpDir, lpParam, lpOp, nShowCmd);
}
/*
//检查文件名合法性
BOOL ringFile::isValidName()
{
	char	buf[MAX_PATH];
	int	i;
	
	// 检查驱动器标识的合法性
	if(strlen(m_szDrive) > 0)
	{
		char	*p, drv=m_szDrive[0];
		DWORD	dwlog;
		
		if (m_szDrive[1] != ':')
		{
			return FALSE;
		}
		
		// 获取逻辑驱动器名称串
		dwlog = ::GetLogicalDriveStrings(_RFMAX_PATH, (LPSTR)buf);
		
		//assert(dwlog);
		
		// 连成一个单串
		for (i=3; i<(int)dwlog; i+=4)
		{
			buf[i] = ' ';
		}
		
		strlwr((char*)buf);
		drv = (char)tolower(drv);
		p = (char*)buf;
		
		// 检查该驱动器标识符是否合法
		while ((*p != drv)&&(*p != '\0'))
		{
			p += 4;
		}
		
		if (*p == 0)
		{
			return FALSE;
		}
		
		//assert(*p == drv);
	}
	
	strcpy((char*)buf, m_lpszPath);
	strcat((char*)buf, m_lpszFilename);
	
	int		len		= strlen(buf);
	LPTSTR	lpstr	= (LPTSTR)buf;
	
	// 如果不存在路径和文件名，则直接返回
	if (len == 0)
	{
		return TRUE;
	}
	
	for(i=0; i<len; i++)
	{
		if(*lpstr == ':' || *lpstr == '*' || *lpstr == '?' || *lpstr == '\"' || *lpstr == '<' || *lpstr == '>' || *lpstr == '|')
		{
			return FALSE;
		}
		else if(is_dir_sep((TCHAR)(*lpstr)))
		{
			lpstr++;
			if(is_dir_sep((TCHAR)(*lpstr)))
			{
				return FALSE;
			}
			else
			{
				continue;
			}
		}
		
		lpstr++;
	}
	
	return TRUE;
}
*/

unsigned __int64 ringFile::GetDiskFreeSize(LPSTR lpRoot)
{
	DWORD	sect, numbytes, free, clusters;

	if (::GetDiskFreeSpace(lpRoot, &sect, &numbytes, &free, &clusters) == 0)
	{
		return (__int64)0;
	}

	return (unsigned __int64)sect*(unsigned __int64)free*(unsigned __int64)numbytes;
}


//检查文件是否存在及属性
int ringFile::check()
{
	// 获取文件属性
	m_dwFileAttr = GetFileAttributes(m_szFilename);
	
	if(m_dwFileAttr != 0xffffffff)
	{
		if((m_dwFileAttr&FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY)
			return RFA_PATH;
		else
			return RFA_FILE;
	}
	else
		return RFA_NOTEXIST;	
}
/*
void ringFile::SplitFilename()
{
	// 解析路径及文件名
	_splitpath((const char *)m_szFilename, m_szDrive, m_lpszPath, m_lpszTitlename, m_lpszExtname);
	
	int	lene = strlen((const char *)m_lpszExtname);
	
	if (lene == 1)
	{
		*m_lpszExtname = '\0';
	}
	
	// 复制+拼合
	strcpy((char*)m_lpszFilename, (const char *)m_lpszTitlename);
	strcat((char*)m_lpszFilename, (const char *)m_lpszExtname);
	
	if (lene > 1)
	{
		memmove((void*)m_lpszExtname, (const void *)&m_lpszExtname[1], lene);
	}
}
*/


///////////////////////////////////////////////////////////////////////////////////////
// 异步文件类

ringAsynFile::ringAsynFile()
{
	m_hFile = NULL;
	
	m_dwLoFileSize = 0UL;
	m_dwHiFileSize = 0UL;

	m_dwTrue = 0UL;

	m_OSType = ::OSType();
}


ringAsynFile::~ringAsynFile()
{
	if (m_hFile)
	{
		::CloseHandle(m_hFile);
	}
}


BOOL ringAsynFile::Create(LPCSTR lpFilename)
{
	//assert(m_hFile == NULL);
	//assert(lpFilename);

	DWORD att = FILE_ATTRIBUTE_NORMAL|FILE_FLAG_RANDOM_ACCESS|((m_OSType==NT)? FILE_FLAG_OVERLAPPED:0);

	::SetLastError(ERROR_SUCCESS);

	// 创建异步文件（同名文件将被销毁）
	m_hFile = ::CreateFile(lpFilename, 
							GENERIC_READ|GENERIC_WRITE, 
							FILE_SHARE_READ, 
							NULL, 
							OPEN_ALWAYS|TRUNCATE_EXISTING, 
							att, NULL);

	if (m_hFile == INVALID_HANDLE_VALUE)
	{
		m_hFile = NULL;
		return FALSE;									// 创建文件失败
	}

	m_dwLoFileSize = m_dwHiFileSize = 0UL;				// 刚创建的文件尺寸为0字节

	return TRUE;
}


BOOL ringAsynFile::Open(LPCSTR lpFilename)
{
	//assert(m_hFile == NULL);
	//assert(lpFilename);
	
	DWORD att = FILE_ATTRIBUTE_NORMAL|FILE_FLAG_RANDOM_ACCESS|((m_OSType==NT)? FILE_FLAG_OVERLAPPED:0);

	::SetLastError(ERROR_SUCCESS);

	// 打开异步文件
	m_hFile = ::CreateFile(lpFilename, 
							GENERIC_READ|GENERIC_WRITE, 
							FILE_SHARE_READ, 
							NULL, 
							OPEN_EXISTING, 
							att, NULL);
	
	if (m_hFile == INVALID_HANDLE_VALUE)
	{
		m_hFile = NULL;
		return FALSE;									// 打开文件失败（或指定文件不存在）
	}

	m_dwLoFileSize = ::GetFileSize(m_hFile, &m_dwHiFileSize);
	
	return TRUE;
}


void ringAsynFile::Close()
{
	//assert(m_hFile);

	::CloseHandle(m_hFile);

	m_hFile = NULL;
	m_dwLoFileSize = m_dwHiFileSize = 0UL;
}


int ringAsynFile::Read(LPVOID lpBuf,DWORD dwNum,DWORD offset)
{
	//assert(m_hFile&&lpBuf&&dwNum);

	if (m_OSType == NT)
	{
		// 等待，直到上一个异步操作完成（以让出OVERLAPPED结构空间）
		::WaitForSingleObject(m_hFile, INFINITE);

		memset((void*)&m_sOverLapped, 0, sizeof(OVERLAPPED));
		m_sOverLapped.Offset = offset;
	}

	::SetLastError(ERROR_SUCCESS);

	// 读数据
	if (::ReadFile(m_hFile, lpBuf, dwNum, &m_dwTrue, (m_OSType==NT) ? &m_sOverLapped:NULL))
	{
		return 1;										// 文件数据已经读出
	}
	else
	{
		if (::GetLastError() == ERROR_IO_PENDING)
		{
			return 0;									// 操作系统开始准备异步读取
		}
		else
		{
			return -1;									// ReadFile()函数执行失败
		}
	}
}


int ringAsynFile::Write(const LPVOID lpBuf,DWORD dwNum,DWORD offset)
{
	//assert(m_hFile&&lpBuf&&dwNum);
	
	if (m_OSType == NT)
	{
		// 等待上一个异步操作完成
		::WaitForSingleObject(m_hFile, INFINITE);
	
		memset((void*)&m_sOverLapped, 0, sizeof(OVERLAPPED));
		m_sOverLapped.Offset = offset;
	}

	::SetLastError(ERROR_SUCCESS);

	// 写数据
	if (::WriteFile(m_hFile, lpBuf, dwNum, &m_dwTrue, (m_OSType==NT) ? &m_sOverLapped:NULL))
	{
		return 1;										// 数据已写入文件
	}
	else
	{
		if (::GetLastError() == ERROR_IO_PENDING)
		{
			return 0;									// 操作系统开始准备异步写入
		}
		else
		{
			return -1;									// WriteFile()函数执行失败
		}
	}
}


BOOL ringAsynFile::ConfirmOver(BOOL wait)
{
	//assert(m_hFile);

	if (m_OSType == NT)
	{
		if (::WaitForSingleObject(m_hFile, (wait)?INFINITE:0) == WAIT_OBJECT_0)
		{
			return TRUE;									// 异步操作完成
		}
		else
		{
			return FALSE;
		}
	}
	else
	{
		return TRUE;
	}
}


BOOL ringAsynFile::QuestionResult(LPDWORD dwRWTure, LPDWORD errCode)
{
	//assert(m_hFile);
	
	BOOL	rc;

	if (m_OSType == NT)
	{
		// 获取操作结果（如果异步操作还没有已完成，否则控制将被阻塞在这）
		rc = ::GetOverlappedResult(m_hFile, &m_sOverLapped, &m_dwTrue, TRUE);
	}
	else
	{
		rc = (::GetLastError() == ERROR_SUCCESS) ? TRUE : FALSE;
	}

	if (dwRWTure)
	{
		*dwRWTure = m_dwTrue;
	}
	
	if (errCode)
	{
		*errCode = ::GetLastError();
	}
	
	return rc;				// TRUE-读写操作成功完成，FALSE-操作失败
}

DWORD ringAsynFile::GetFileLength()
{
	//assert(m_hFile);

	::SetLastError(ERROR_SUCCESS);

	return m_dwLoFileSize;
}

//////////////////////////////////////////////////////////
//
//内存映像文件
//
//////////////////////////////////////////////////////////
ringMappingFile::~ringMappingFile()
{
	if(m_hMapping)
		CloseHandle(m_hMapping);
}

HANDLE ringMappingFile::CreateMapping(DWORD dwProtectFlag,DWORD dwMaxSizeLow,
												  DWORD dwMaxSizeHigh,LPCTSTR lpName)
{
	HANDLE hMap = NULL;
	if(m_hFile && Size() > 0)
	{
		hMap = CreateFileMapping(m_hFile,NULL,dwProtectFlag,dwMaxSizeHigh,dwMaxSizeLow,lpName);
		if(m_hMapping == NULL)
			m_hMapping = hMap; 
	}
	return hMap;
}

LPBYTE ringMappingFile::CreateView(DWORD dwAccess,DWORD dwNumber,HANDLE hMap,
											  DWORD dwOffsetLow,DWORD dwOffsetHigh)
{
	if(hMap == NULL)
		hMap = m_hMapping;
	if(hMap)
		return (LPBYTE)MapViewOfFile(hMap,dwAccess,dwOffsetHigh,dwOffsetLow,dwNumber);
	else
		return NULL;
}

BOOL ringMappingFile::DeleteView(LPVOID lpv)
{
	return UnmapViewOfFile(lpv);
}

BOOL ringMappingFile::UnMap(HANDLE hMap)
{
	if(hMap == NULL)
		hMap = m_hMapping;

	BOOL bOK = CloseHandle(hMap);

	if(hMap == m_hMapping)
		m_hMapping = NULL;
	return bOK;
}
