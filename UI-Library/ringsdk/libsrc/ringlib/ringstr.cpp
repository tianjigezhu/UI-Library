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
//原文件名：ringstr.h
//
//说明：
//    字符串操作类 ringStr 实现
//
**********************************************************************/
#define MAKE_SELF_LIB
#include "ringlib.h"

void exwsprintf(LPSTR s,LPCTSTR format,...)
{
	va_list ap;

   if(format)
	{
		va_start(ap, format);		
		vsprintf(s,format,ap);
		va_end(ap);
	}
}

ringStr::ringStr(LPCTSTR str,DWORD size/*=0*/)
{
   if(str)
   {
      m_len = (DWORD)lstrlen(str);
      if(m_len > size)
	      if(m_len < RSTR_INIT)
   	   	m_size = RSTR_INIT;
      	else
      		m_size = m_len + sizeof(TCHAR);
      else
      	m_size = size;

   	m_data = (LPTSTR)New(m_size * sizeof(TCHAR));

      if(m_data)
      	lstrcpy(m_data,str);
      else
      	init();
   }
   else
   {
   	m_len = m_size = 0;
      m_data = NULL;
   }
	m_wData = NULL;
}

ringStr::ringStr(int size)
{
   m_wData = NULL;
	memset(x,0,4);
	
	m_data = NULL;
	m_size = m_len = 0;

   if(size <= RSTR_PCNAME && size >= RSTR_WINPATH)
   	LoadSpecial(size);
	else
	{
		if(size < 0)
			size = 0;

		m_size = size;
		m_len = 0;
 		m_data = (LPTSTR)New(size * sizeof(TCHAR));
		if(!m_data)
   		init();
	}
}

BOOL ringStr::LoadSpecial(int nFlag)
{
	if(m_data && m_size < RSTR_STEP)
		m_data = (LPTSTR)Del(m_data);
	
   if(m_data == NULL)
	{
		m_data = (LPTSTR)New(RSTR_STEP * sizeof(TCHAR));
		m_size = m_data?RSTR_STEP:0;
	}
   else
		Clear();

	m_len = 0;

	if(m_data)
	{		
		switch(nFlag)
		{
		case RSTR_WINPATH:
			m_len = GetWindowsDirectory(m_data,m_size);
			break;
		case RSTR_SYSPATH:
			m_len = GetSystemDirectory(m_data,m_size);
			break;
		case RSTR_TEMPPATH:
			m_len = GetTempPath(m_size,m_data) - 1;
			m_data[m_len] = 0;
			break;
		case RSTR_CURRPATH:
		{
			m_len = GetModuleFileName(GetInstance(),m_data,m_size);
			TCHAR c;
			for(int i=m_len-1;i>1;i--)
			{
				c = m_data[i];
				m_data[i] = 0;
				m_len --;
				if(c == '\\')
					break;
			}
			break;
		}
		case RSTR_MYFILENAME:
			m_len = GetModuleFileName(GetInstance(),m_data,m_size);
			break;
		case RSTR_PCNAME:
			GetComputerName(m_data,&m_len);
			break;
		}
	}
	return (BOOL)m_len;
}

ringStr::ringStr(const ringStr& src)
{
   m_wData = NULL;
	if(&src == this || src.m_data == NULL)
   {
		m_data = NULL;
      m_len = 0;
      m_size = 0;
   	return;
	}
   else
   {
		m_data = (LPSTR)New(src.m_size);
    	if(m_data)
      {
         lstrcpy(m_data,src.m_data);
      	m_len = src.m_len;
         m_size = src.m_size;
      }
   }
}

ringStr::~ringStr()
{
   Release();
}

void ringStr::Release()
{
	m_data = (LPSTR)Del(m_data);
   m_wData = (WCHAR*)Del(m_wData);
	m_len = m_size = 0;
}

void ringStr::Clear()
{
   if(m_data)
		memset(m_data,0,m_size * sizeof(TCHAR));
   m_len = 0;
}

void ringStr::init()
{
	m_data = (LPTSTR)New(RSTR_INIT * sizeof(TCHAR));
   if(m_data)
   {
   	m_size = RSTR_INIT;
      m_len = 0;
   }
   else
   	m_len = m_size = 0;
}

int ringStr::GetLength()
{
	if(m_data)
		m_len = lstrlen(m_data);
	else
		m_len = 0;
	return m_len;
}

TCHAR& ringStr::operator [](int index)
{
	if((DWORD)index < m_len)
		return m_data[index];
	else
	{
		x[0] = '\0';
		return x[0];
	}
}

int ringStr::Load(UINT ident,HINSTANCE hInst)
{
   if(m_data && m_size < RSTR_STEP)
   	m_data = (LPTSTR)Del(m_data);

   if(m_data == NULL)
   {
		m_data = (LPTSTR)New(RSTR_MAX * sizeof(TCHAR));
  	   m_size = RSTR_MAX;
   }
   else
   	Clear();

   m_len = LoadString(hInst,ident,m_data,m_size);

   return m_len;
}

int ringStr::printf(LPCTSTR format,...)
{
   TCHAR s[RSTR_MAX];
   va_list ap;

   if(format)
	{
		va_start(ap, format);
		_vsntprintf(s,RSTR_MAX-2,format,ap);
		//vsprintf(s, format, ap);
		va_end(ap);
	}

   m_len = lstrlen(s);
	Clear();

   if(m_len < m_size)
   	lstrcpy(m_data,s);
   else
   {
   	Del(m_data);
      if(m_len < RSTR_STEP)
      	m_size = RSTR_STEP;
      else
         m_size = RSTR_MAX;

      m_data = (LPTSTR)New(m_size * sizeof(TCHAR));
      if(m_data)
      	lstrcpy(m_data,s);
      else
      	init();
   }
   return m_len;
}

int ringStr::strcat(LPCTSTR format,...)
{
   char s[RSTR_MAX];
   va_list ap;

   if(format)
	{
		va_start(ap, format);
		_vsntprintf(s,RSTR_MAX-2,format,ap);
		//vsprintf(s, format, ap);
		va_end(ap);
	}

   m_len += lstrlen(s);

   if(m_len < m_size)
   	::lstrcat(m_data,s);
   else
   {
      LPTSTR temp = m_data;
   	//Del(m_data);
      if(m_len < RSTR_STEP)
      	m_size = RSTR_STEP;
      else if(m_len < RSTR_MAX)
         m_size = RSTR_MAX;
      else
      	m_size = m_len + sizeof(TCHAR);

      m_data = (LPTSTR)New(m_size * sizeof(TCHAR));
      if(m_data)
      	exwsprintf(m_data,"%s%s\0",temp,s);
      else
      	init();

      Del(temp);
   }
   return m_len;
}

//////////////////////////////////////////////////
//
//设置m_data长度
//		bClear:清除数据标志
//		=FALSE：不清除，如果原有m_data长度大于size，不缩减m_data长度，
//				  截断已有数据到size长度
//		=TRUE:清除已有数据
//
//
//////////////////////////////////////////////////
DWORD ringStr::SetLength(DWORD size,BOOL bClear/*=FALSE*/)
{
	if(m_data)
   {
      LPTSTR temp;

   	if(size > m_size)
      {
         temp = m_data;
      	m_data = (LPTSTR)New(size * sizeof(TCHAR));
         if(m_data)
         {
            m_size = size;
            if(!bClear)
	         	lstrcpy(m_data,temp);
            else
            	m_len = 0;

            Del(temp);
         }
         else
         	m_data = temp;
      }
      else
      {
      	if(bClear)
         	Clear();
			else
				memset(m_data + size,0,(m_size - size)*sizeof(TCHAR));			
      }
   }
   else
   {
		m_data = (LPTSTR)New(size * sizeof(TCHAR));
      if(m_data)
      {
      	m_size = size;
         m_len = 0;
      }
      else
      	init();
   }
   return m_size;
}

void ringStr::resizeBuf(LPCTSTR str)
{
   if(str)
   {
      DWORD len = (DWORD)lstrlen(str);
      if(len < m_size)
      {
         Clear();
      	lstrcpy(m_data,str);
         m_len = len;
      }
      else
      {
         LPTSTR temp = m_data;
   		m_data = (LPTSTR)New((len + 1) * sizeof(TCHAR));
      	if(m_data)
	      {
            m_len = len;
            m_size = len + 1;
            lstrcpy(m_data,str);
            Del(temp);
         }
      	else
         	m_data = temp;
   	}
   }
	else
		Clear();
}

//ANSI与UNICODE通用版本，参数类型根据版本为LPCSTR或LPCWSTR
const ringStr& ringStr::operator=(LPCTSTR str)
{
	if(str != m_data)
		resizeBuf(str);
   return *this;
}

const ringStr& ringStr::operator=(const ringStr& str)
{
	if(this == &str)
   	return *this;

	return operator=(str.m_data);
}

const ringStr& ringStr::operator=(const int number)
{
   if(!m_data)
   	init();
   else if(m_size < RSTR_INIT)
	   SetLength(RSTR_INIT,TRUE);

	wsprintf(m_data,"%d\0\0",number);
   m_len = lstrlen(m_data);
   return *this;
}

#ifndef _UNICODE
//ANSI版本专用
const ringStr& ringStr::operator=(LPCWSTR str)
{
	if(str)
	{
		int z = WideCharToMultiByte(CP_ACP,WC_SEPCHARS|WC_COMPOSITECHECK,(LPWSTR)str,-1,NULL,0,NULL,NULL);
	
		SetLength(z+1,TRUE);
		WideCharToMultiByte(CP_ACP,WC_SEPCHARS|WC_COMPOSITECHECK,(LPWSTR)str,-1,m_data,z,NULL,NULL);
		
		GetLength();
	}
   return *this;
}
#endif

const ringStr& ringStr::operator=(TCHAR ch)
{
	TCHAR s[2] = {ch,'\0'};
	resizeBuf(s);
	return *this;
}

const ringStr& ringStr::operator=(const unsigned char* psz)
{
	*this = (LPCSTR)psz;
	return *this;
}

const ringStr& ringStr::operator+=(const int number)
{
	TCHAR s[16];
	wsprintf(s,"%d\0\0",number);
	return operator+=(s);
}

const ringStr& ringStr::operator+=(LPCTSTR s)
{
   if(!s)
   	return *this;

   m_len += lstrlen(s);

   if(m_len < m_size)
   	::lstrcat(m_data,s);
   else
   {
      if(m_len < RSTR_STEP)
      	m_size = RSTR_STEP;
      else if(m_len < RSTR_MAX)
         m_size = RSTR_MAX;
      else
      	m_size = m_len + sizeof(TCHAR);

      LPTSTR temp = (LPSTR)New(m_size);
		if(m_data == NULL)
			lstrcpy(temp,s);
		else
			exwsprintf(temp,"%s%s\0",m_data,s);
		Del(m_data);
      m_data = temp;
   }
   return *this;
}

const ringStr& ringStr::operator+=(const ringStr& str)
{
   return operator+=(str.m_data);
}

const ringStr& ringStr::operator+=(TCHAR ch)
{
	TCHAR s[2] = {ch,'\0'};
	return operator+=(s);
}

BOOL ringStr::operator ==(const ringStr& str) const
{
   if(str.m_data && m_data)
		return !lstrcmp(m_data,str.m_data);
   else
		return (!str.m_data && !m_data);   	
}

BOOL ringStr::operator ==(LPCTSTR str) const
{
	if(m_data && str)
		return !lstrcmp(m_data,str);
   else
		return (!m_data && !str);
}

BOOL ringStr::operator !=(const ringStr& str) const
{
	if(str.m_data && m_data)
		return lstrcmp(m_data,str.m_data);
   else
		return (str.m_data || m_data);
}

BOOL ringStr::operator !=(LPCTSTR str) const
{
	if(m_data && str)
		return lstrcmp(m_data,str);
   else
		return (m_data || str);
}

BOOL ringStr::operator <(const ringStr& str) const
{
	if(str.m_data && m_data)
		return (lstrcmp(m_data,str.m_data) < 0);
   else
		return (BOOL)(str.m_data);   	
}

BOOL ringStr::operator >(const ringStr& str) const
{
	if(str.m_data && m_data)
		return (lstrcmp(m_data,str.m_data) > 0);
   else
		return (BOOL)(m_data);
}

BOOL ringStr::operator <=(const ringStr& str) const
{
   return !operator > (str);
}

BOOL ringStr::operator >=(const ringStr& str) const
{
	return !operator < (str);
}

ringStr ringStr::operator +(const ringStr& rhs) const
{
	ringStr temp = *this;
   temp += rhs.m_data;

   return temp;
}

ringStr ringStr::operator +(TCHAR ch) const
{
	ringStr temp = *this;
   temp += ch;
	
   return temp;
}

int ringStr::cmp(const ringStr& str) const
{
	if(str.m_data && m_data)
		return lstrcmp(m_data,str.m_data);
   else if(m_data)
   	return -1;
   else if(str.m_data)
   	return 1;
   else
   	return 0;
}

int ringStr::cmpi(const ringStr& str) const
{
	if(str.m_data && m_data)
		return lstrcmpi(m_data,str.m_data);
   else if(m_data)
   	return -1;
   else if(str.m_data)
   	return 1;
   else
   	return 0;
}

ringStr ringStr::replace(LPCTSTR str1,LPCTSTR str2,BOOL bAll/* = TRUE*/) const
{
   ringStr temp;
   replace(temp,str1,str2,bAll);
	return temp;
}

BOOL ringStr::replace(ringStr& temp,LPCTSTR str1,LPCTSTR str2,BOOL bAll/* = TRUE*/) const
{
   if(str1 == NULL || str2 == NULL || m_data == NULL || m_len == 0)
	{
		temp = m_data;
		return FALSE;
	}
	
	int l1 = lstrlen(str1);
   int l2 = lstrlen(str2);
   int addstep = l2 - l1;	//如果替换后字串超长，需增加的字串长度
	int i,pos = 0;
   LPTSTR dest,src;
   
   pos = indexOf(str1,pos);
   src = m_data;
   
	if(addstep <= 0)
		temp.SetLength(m_size,TRUE);
	else
		temp.SetLength(m_size + (bAll?count(str1):1) * addstep + 1,TRUE);
	
	dest = temp.m_data;
	
   if(bAll)
   {
		i = 0;
		while(pos >= 0)
      {
			//COPY 匹配字串前面的字符
			lstrcpyn(dest,src,pos - i + 1);
			dest += pos - i;
			src += pos - i;
			//替换 str1 为 str2
			lstrcpyn(dest,str2,l2 + 1);
			dest += l2;
			src += l1;
			i = pos + l1;
         //寻求下一次匹配
         pos = indexOf(str1,i);
      }
   }
   else
   {
		if(pos >= 0)
      {
			//COPY 匹配字串前面的字符
			lstrcpyn(dest,src,pos + 1);
			dest += pos;
			src += pos;
			//替换 str1 为 str2
			lstrcpyn(dest,str2,l2 + 1);
			dest += l2;
			src += l1;			
      }
   }
	//补齐后续字串
	lstrcpyn(dest,src,m_len - (src - m_data) + 1);
	temp.GetLength();
   return TRUE;
}

void ringStr::replace(TCHAR c1,TCHAR c2,BOOL bAll/* = TRUE*/)
{
	LPTSTR tmp = m_data;
	if(tmp)
		if(bAll)
			for(DWORD i=0;i<m_len;i++)
			{
				if(*tmp == c1)
					*tmp = c2;
				tmp ++;
			}
		else
			for(DWORD i=0;i<m_len;i++)
			{
				if(*tmp == c1)
				{
					*tmp = c2;
					return;
				}
				tmp ++;
			}
}

ringStr ringStr::SubString(int start,int count/* = 0*/) const
{
	ringStr temp;

	if(start < 0)
		start = 0;

   if(count <= 0 || (DWORD)(count + start) >= m_len)
   	count = m_len - start;

   if(count <= 0)		//如果start超出范围,m_len - start有可能 <0
   	return temp;

   temp.SetLength(count + sizeof(TCHAR));

   LPTSTR src = m_data + start;
   LPTSTR dest = temp.m_data;

   lstrcpyn(dest,src,count + 1);	
	temp.GetLength();
   return temp;
}

ringStr ringStr::SubString(LPCTSTR szStart,LPCTSTR szEnd,int nStart/*=0*/,int* nIndex/*=NULL*/) const
{
	int lens=0,s,e=0,sp;

	if(szStart)
		lens = lstrlen(szStart);
	
	s = indexOf(szStart,nStart);
	
	if(s < 0)
		sp = nStart;
	else
		sp = s + lens;

	if(szEnd)
		e = indexOf(szEnd,sp);
	
	if(e < 0)
		e = m_len;
	
	//if(e == 0 || (s == 0 && szStart) || (e - s - lens == 0))//严格判断版本
	if((s < 0 && szStart) || (e - s - lens == 0))	//szEnd找不到返回到结尾
	{
		//未找到szStart或szEnd,e只有szEnd有效且找不到才为0
		if(nIndex)
			*nIndex = e;
		return (LPTSTR)NULL;
	}
	else	//找到匹配的字串
	{
		if(nIndex)
			*nIndex = e;
		return SubString(s + lens,e - s - lens);
	}	
}

/*********************************************************
//
//搜索字符串
//
//参数：
//	str:要搜索的字符串
//	start:搜索起始位置，<0：从头开始，>=0：从start位置开始搜索
//	ntimes:第ntimes匹配（从1开始计数）
//
//返回：-1：未找到，否则>=0
//
//*******************************************************/
int ringStr::indexOf(LPCTSTR str,int start/*=-1*/,int ntimes/*=1*/) const
{
   if(!str || *str == '\0' || m_data == NULL || *m_data == '\0' || start > (int)m_len)
   	return -1;
	
	if(start < 0)
		start = 0;

	LPTSTR lpszCur = m_data + start;
	int nCnt = 1,nLen = lstrlen(str);

//修正：原始版本，工程默认定义_MBCS，_tcsstr的_MBCS版本狂慢！
	//while((lpszCur - m_data) < (int)m_len && ((lpszCur = _tcsstr(lpszCur,str)) != NULL))
#ifndef _UNICODE
	while((lpszCur - m_data) < (int)m_len && ((lpszCur = strstr(lpszCur,str)) != NULL))
#else
	while((lpszCur - m_data) < (int)m_len && ((lpszCur = wcsstr(lpszCur,str)) != NULL))
#endif
	{
		if(++nCnt > ntimes)
			return lpszCur - m_data;
		else
			lpszCur += nLen;
	}
	return -1;
}

//ntimes:第ntimes匹配（从1开始计数）
int ringStr::lastIndexOf(LPCTSTR str,int ntimes/*=1*/) const
{
	if(!str || *str == '\0' || m_data == NULL || *m_data == '\0')
		return -1;

	int nLenF = lstrlen(str);
	if(nLenF > (int)m_len)
		return -1;

   LPTSTR lpszEndF = (LPTSTR)str + nLenF -1;

   LPTSTR pS = m_data + m_len - nLenF;
   LPTSTR pFind;
   LPTSTR p;
	int cnt = 1;
   //匹配不成功，被查询串指针将向左移动的字符数
   int nMove;

   while(pS >= m_data)
   {
      pFind = (LPTSTR)str;

      while(pFind <= lpszEndF)
      {
         if(*pS != *pFind)
         {
            //被查询串的当前字符是否在查询串当前字符后面的字串里？
            if(pFind == lpszEndF)
               nMove = nLenF;
            else
            {
            	p = pFind + 1;
               while(p <= lpszEndF)
               {
               	if(*p == *pS)
                  {
		               nMove = p - str;
                     break;
                  }
                  p ++;
               }
               if(p > lpszEndF)
               	nMove = nLenF;
            }
            break;
         }
         pFind ++;
         pS ++;
      }
      if(pFind > lpszEndF)
		{
			cnt ++;
			if(cnt > ntimes)
				return((pS - m_data) - nLenF);
			else
				nMove = nLenF << 1;
		}
      pS -= nMove;
   }
   return -1;
}

//计算含有多少str子串
int ringStr::count(LPCTSTR str) const
{
   if(!str || *str == '\0' || m_data == NULL || *m_data == '\0')
		return 0;
	
	int nCnt = 0,nLen = lstrlen(str);
	LPTSTR lpszCur = m_data;

//修正：原始版本，工程默认定义_MBCS，_tcsstr的_MBCS版本狂慢！
	//while((lpszCur - m_data) < (int)m_len && ((lpszCur = _tcsstr(lpszCur,str)) != NULL))
#ifndef _UNICODE
	while((lpszCur - m_data) < (int)m_len && ((lpszCur = strstr(lpszCur,str)) != NULL))
#else
	while((lpszCur - m_data) < (int)m_len && ((lpszCur = wcsstr(lpszCur,str)) != NULL))
#endif	
	{
		nCnt ++;
		lpszCur += nLen;
	}

	return nCnt;
}

ringStr ringStr::Insert(const ringStr& str, int index)
{
   ringStr temp;
   int i,cnt = str.len();
   LPTSTR src,dest;

	if(cnt == 0 || isClear())
	{
		temp = *this;
		return temp;
	}

   if((cnt + m_len) > m_size)
   	temp.SetLength(cnt + m_len + sizeof(TCHAR),TRUE);
   else
   	temp.SetLength(m_size,TRUE);

   src = m_data;
   dest = temp.m_data;

   for(i=0;i<index;i++)
   	*dest++ = *src++;

   for(i=0;i<cnt;i++)
   	*dest++ = str.m_data[i];

   cnt = m_len - index;

   for(i=0;i<cnt;i++)
   	*dest++ = *src++;

   return temp;
}

ringStr ringStr::Delete(int index, int count)
{
	ringStr temp = *this;
   LPTSTR src,dest;
   int i;

	if(isClear())
		return temp;

   src = m_data;
   dest = temp.m_data;

   if(index > (int)m_len)
   	index = m_len;

   for(i=0;i<index;i++)
   	*dest++ = *src++;

   if((index + count) > (int)m_len)
   	count = m_len - index;

   src += count;

   for(i=index+count;i<(int)m_len;i++)
   	*dest++ = *src++;

	return temp;
}

void ringStr::toLower()
{
	if(m_data && m_len > 0)
	{
		LPTSTR tmp = m_data + m_len - 1;
		while(tmp >= m_data)
		{
			*tmp = (char)tolower(*tmp);
			tmp --;
		}
	}
}

void ringStr::toUpper()
{
	if(m_data && m_len > 0)
	{
		LPTSTR tmp = m_data + m_len - 1;
		while(tmp >= m_data)
		{
   		*tmp = (char)toupper(*tmp);
			tmp --;
		}
	}
}

void ringStr::Trim()
{
	RTrim();
	LTrim();   
}

void ringStr::LTrim()
{
   LPTSTR start = m_data,tmp = m_data,end = m_data + m_len - 1;

   while(start <= end)
   {
   	if(*start == ' ')
      {
			start ++;
			m_len --;
		}
      else
      	break;
   }
   if(start > m_data)
   {
		while(start <= end)
      {
      	*tmp++ = *start++;
      }
		memset(tmp,0,(end - (tmp - 1)) * sizeof(TCHAR));
	}
}

void ringStr::RTrim()
{
   LPTSTR tmp = m_data + m_len - 1;
	while(tmp >= m_data)
   {
   	if(*tmp == ' ')
      {
      	*tmp-- = 0;
         m_len --;
      }
      else
      	break;
   }
}

WCHAR* ringStr::wstring(int* nLen)
{
	int len = (m_len << 1) + 2;
	if(m_wData)
		Del(m_wData);

	m_wData = (WCHAR*)New(len * sizeof(WCHAR));
	if(m_wData && m_data)
	{
		len = MultiByteToWideChar(CP_ACP,MB_PRECOMPOSED,m_data,-1,m_wData,len);
		if(nLen)
			*nLen = len;
		return m_wData;
	}
	return L"";
}

ringStr operator +(LPTSTR lhs, const ringStr& rhs)
{
	ringStr temp(lhs);
   temp += rhs.string();
   return temp;
}

//添加count个c字符到串尾
void ringStr::Append(TCHAR c,int count)
{
	LPTSTR s = (LPTSTR)New((count+1) * sizeof(TCHAR));
	LPTSTR p = s;
	if(s)
	{
		for(int i=0;i<count;i++)
			*p++ = c;
		operator+=(s);
	}
	Del(s);
}

//添加lpstr和若干个c字符到串尾，总添加字符数为count
//如果lpstr长度大于count，截断lpstr为count个字符
void ringStr::AppendSize(LPCTSTR lpstr,TCHAR c,int count)
{
	if(count <= 0)
		return;

	int n = lstrlen(lpstr);
	LPTSTR p,s = (LPTSTR)New((count+2) * sizeof(TCHAR));
	
	if(s)
	{
		if(lpstr)
			lstrcpyn(s,lpstr,count+1);	//+1:最后的'\0'

		p = s + lstrlen(s);
		for(int i=p-s;i<count;i++)
			*p++ = c;		
		operator+=(s);
	}
	Del(s);
}


/***********废弃代码，但有参考价值，字符串搜索算法***********************
//ntimes:第ntimes匹配（从1开始计数）
//返回：-1：未找到，否则>=0
int ringStr::indexOf(LPCTSTR str,int start,int ntimes) const
{
   if(!str || m_len < 1)
		return -1;
	
	if(start < 0)
		start = 0;
	
	int nLenF = lstrlen(str);
	if(nLenF < 1)
		return -1;
	
   LPTSTR lpszEndF = (LPSTR)str + nLenF -1;
   LPTSTR lpszEndS = m_data + m_len -1;
	
   LPSTR pS = m_data + start + nLenF -1;
   LPSTR pFind;
   LPSTR p;
   //匹配不成功，被查询串指针将向右移动的字符数
   int nMove;
	int cnt = 1;
   BOOL bEnd = FALSE;
	
   while(pS <= lpszEndS)
   {
		//从右向左匹配。
      pFind = lpszEndF;
      //比较被查询串的当前字符是否和查询串的最右边字
      //符匹配，如果匹配则回溯比较
      if(pS == lpszEndS)
			bEnd = TRUE;
		
      while(pFind >= str)
      {
         if(*pS != *pFind)
         {
            //被查询串的当前字符是否在查询串当前字符前面的字串里？
            if(pFind == str)
               nMove = nLenF;
            else
            {
					p = pFind - 1;
               while(p >= str)
               {
						if(*p == *pS)
                  {
							nMove = lpszEndF - p;
                     break;
                  }
                  p --;
               }
               if(p < str)
						nMove = nLenF;
            }
            break;
         }
         // 匹配成功一个，向左回溯
         pFind --;
         pS --;
      }
      if(pFind < str)
      {
			cnt ++;
			if(cnt > ntimes)
				return((pS - m_data) + 2);
			else
				nMove = nLenF << 1;
		}
		
      if(bEnd)
			return 0;
		
      pS += nMove;
   }
   return 0;
}
**********************************************************/
