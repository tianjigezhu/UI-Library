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
//							RingSDK多媒体类库 ringdib.lib
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
//原文件名：rdib.cpp
//
//说明：图象载入部分RingDIB类实现代码
//
**********************************************************************/
#define MAKE_SELF_LIB
#include "ringdib.h"

#define MAX_PIC_FORMAT		20		//最多支持文件格式（外挂图象解码支持）

static struct stdibext{
	char ext[8];
	stdibext(){memset(ext,0,8);}
}dib_load_ext[MAX_PIC_FORMAT];

static struct stdibfunc{
	DECODER_FUNC ext_func;
	stdibfunc(){ext_func=NULL;}
}dib_load_funcs[MAX_PIC_FORMAT];

//滤镜插件类纯虚析构函数体
DibFilter::~DibFilter()
{
}

int RingDIB::pluged_ext = 0;
HDRAWDIB RingDIB::m_hDrawDib = NULL;
RingDIB* RingDIB::m_dibCanvas = NULL;
LONG RingDIB::m_nObjCount = 0;
RingDataGuard RingDIB::m_guard;

RingDIB::RingDIB()
{
	init();
}

RingDIB::RingDIB(HWND hWnd)
{
	init();
	if(IsWindow(hWnd))
		m_hWnd = hWnd;
}

void RingDIB::init()
{
   m_bmif.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	m_bmif.bmiHeader.biWidth = 0;
	m_bmif.bmiHeader.biHeight = 0;
	m_bmif.bmiHeader.biPlanes = 1;
	m_bmif.bmiHeader.biBitCount = 32;
	m_bmif.bmiHeader.biCompression = BI_RGB;
	m_bmif.bmiHeader.biClrImportant = 0;
	m_bmif.bmiHeader.biClrUsed = 0;
	m_bmif.bmiHeader.biSizeImage = 0;
	m_bmif.bmiHeader.biXPelsPerMeter = 0;
	m_bmif.bmiHeader.biYPelsPerMeter = 0;
	m_width=0;
   m_height=0;
   m_Bits=NULL;
   m_aniGifPic = NULL;
	m_lGifInfo = NULL;
	m_backup = m_actbak = m_target = NULL;
   m_crKey = 0xFFFFFFFF;
   
   m_hWnd = GetMainWnd();
   m_hrgn = NULL;
	m_size = 0;

   SetRect(&m_rcSrc,0,0,0,0);
   SetRect(&m_rcDest,0,0,0,0);

   m_currFunc = NULL;
	pluged_filter = 0;
   ZeroMemory(&m_Filter,sizeof(DIBFILTERINFO));
	dib_load_funcs[0].ext_func = LoadBmp;

   m_guard.Enter();
	if(m_hDrawDib == NULL)
		m_hDrawDib = DrawDibOpen();
	m_nObjCount ++;
	m_guard.Leave();
}

LPLOCALGIFINFO RingDIB::GetLocalGifInfo()
{
	if(m_lGifInfo == NULL)
	{
		m_lGifInfo = (LPLOCALGIFINFO)New(sizeof(LOCALGIFINFO));
		if(m_lGifInfo)
		{
			m_lGifInfo->AniThread = NULL;
			m_lGifInfo->bNeedBak = FALSE;
			m_lGifInfo->bgC = 0;
			m_lGifInfo->bActive = FALSE;
			m_lGifInfo->funcFilter = NoneFilter;
		}
	}
	return m_lGifInfo;
}


RingDIB::~RingDIB()
{
	Destroy();
	NewSize(0);

   for(int i=0;i<pluged_filter;i++)
   {
   	if(m_Filter.m_filter[i]!=NULL)
      {
			delete (LPDIBFILTER)m_Filter.m_filter[i];
			m_Filter.m_filter[i] = NULL;
		}
      else
      	break;
   }
   
   if(m_hrgn)
   {
		DeleteObject(m_hrgn);
		m_hrgn = NULL;
	}

	try
	{
		delete m_lGifInfo;
		delete m_actbak;
		delete m_backup;
		m_lGifInfo = NULL;
		m_actbak = m_backup = NULL;
	}
	catch(...)
	{
	}
	//析构：这里不能用m_guard.Enter()，因为如果是静态产生对象(RingDIB dibobjname;方式产生)，
	//静态的m_guard有可能先于本对象析构。
   LONG ncnt = InterlockedDecrement(&m_nObjCount);
	if(ncnt == 0)
   {
		//同样不能FreeCanvas();因该函数调用m_guard.Enter()
		delete m_dibCanvas;
		m_dibCanvas = NULL;
		DrawDibClose(m_hDrawDib);
      m_hDrawDib = NULL;
   }
	else if(ncnt < 0)
	{
		//FreeCanvas()会继续m_nObjCount-1，使m_nObjCount=-1
		InterlockedIncrement(&m_nObjCount);
	}	
}

void RingDIB::Destroy()												//析构函数调用;
{
   EnableAniGif();

	if(m_lGifInfo)
	{
		delete m_lGifInfo->AniThread;
		m_lGifInfo->AniThread = NULL;
		m_lGifInfo->bNeedBak = FALSE;
		m_lGifInfo->bgC = 0;
		m_lGifInfo->bActive = FALSE;
		m_lGifInfo->funcFilter = NoneFilter;
	}
	
   if(m_aniGifPic)
   {
      //删除循环链表
		LPANIGIFPIC mark,save,temp;
      mark = m_aniGifPic;
      m_aniGifPic = NULL;
      save = mark->next;

      while(save && save != mark)
      {
      	Del(save->curr);
         temp=save;
			save = save->next;
         Del(temp);
      }
		Del(mark->curr);
		Del(mark);
   }
}

BOOL RingDIB::NewSize(int nNewsize)
{	
	if(nNewsize > 0)
	{
		if(nNewsize <= m_size)
		{
			RtlZeroMemory(m_Bits,nNewsize);
			return TRUE;
		}
		else
		{
			m_Bits = (COLORREF*)Del(m_Bits);
			m_Bits = (COLORREF*)New(nNewsize);
			if(m_Bits != NULL)
			{
				m_size = nNewsize;
				return TRUE;
			}
			else
				m_size = 0;
		}
	}
	else if(nNewsize == 0)
	{
		m_Bits = (COLORREF*)Del(m_Bits);
		m_size = 0;
		return TRUE;
	}
	return FALSE;
}

//注册图象解码程序
BOOL RingDIB::RegFormat(LPCSTR ext,DECODER_FUNC ext_func)
{
   if(pluged_ext >= MAX_PIC_FORMAT)
   	return FALSE;
	if(ext && *ext != '\0')
   {
      int extlen = strlen(ext);
      int max = extlen > 7?7:extlen;
      strncpy(dib_load_ext[pluged_ext].ext,ext,max);
		dib_load_ext[pluged_ext].ext[max] = '\0';
      pluged_ext++;
      if(ext_func)
		{
			//dib_load_funcs[0]为LoadBmp
			for(int i=1;i<MAX_PIC_FORMAT;i++)
			{
				if(dib_load_funcs[i].ext_func == NULL)
				{
					dib_load_funcs[i].ext_func = ext_func;
					break;
				}
				else if(dib_load_funcs[i].ext_func == ext_func)
					break;
			}
		}
		return TRUE;
   }
   return FALSE;
}

//运行/停止GIF动画
void RingDIB::EnableAniGif(BOOL bActive)
{
	if(m_aniGifPic && m_lGifInfo)
   	if(m_lGifInfo->bActive && !bActive)
      {
      	if(m_lGifInfo->AniThread)
				m_lGifInfo->AniThread->Pause();
			m_lGifInfo->bActive = FALSE;
      }
      else if(!m_lGifInfo->bActive && bActive)
      {
			if(m_lGifInfo->AniThread)
				m_lGifInfo->AniThread->Resume();
         m_lGifInfo->bActive = TRUE;
      }
}

BOOL RingDIB::Import(LPBYTE lpdata,int width,int height)
{
	if(lpdata && Create(width,height))
	{
		memcpy(m_Bits,lpdata,width * height * sizeof(COLORREF));
		return TRUE;
	}
	return FALSE;
}

BOOL RingDIB::Select(BOOL bAllFile,HWND hWnd/*=m_hWnd*/,LPCTSTR dlgEx/*=NULL*/,LPOFNHOOKPROC hookproc/*=NULL*/)
{
	ringFile rf;
   char str[200];
   ZeroMemory(str,200);
   wsprintf(str,"图象文件\0");

   int len = 9,extlen,j;
   str[len++] = '*';
   str[len++] = '.';
   str[len++] = 'b';
   str[len++] = 'm';
   str[len++] = 'p';

   for(int i=0;i<pluged_ext;i++)
   {
      str[len++] = ';';
      str[len++] = '*';
      str[len++] = '.';
      extlen = strlen(dib_load_ext[i].ext);
      for(j=0;j<extlen;j++)
			str[len++] = dib_load_ext[i].ext[j];
   }

   if(bAllFile)
   {
   	str[len++] = ';';
      str[len++] = '*';
      str[len++] = '.';
      str[len++] = '*';
      str[len++] = '\0';
      str[len] = '\0';
	}
   else
   {
   	str[len++] = '\0';
      str[len] = '\0';
	}

   if(rf.Select(str,NULL,FALSE,hWnd,dlgEx,hookproc))
   	return Load(rf.Fullname());
   else
   	return FALSE;
}

LPSTR RingDIB::GetFormat(LPSTR lpszFormat)
{
	if(m_currFunc && m_currFunc(NULL,this,DIB_GETFORMAT,lpszFormat))
		return lpszFormat;
	else
		return NULL;
}

BOOL RingDIB::IsImage(LPSTR szPicFilename)
{
	for(int i=0;i<pluged_ext;i++)
	{
		if(dib_load_funcs[i].ext_func && dib_load_funcs[i].ext_func(szPicFilename,this,DIB_QUERYSUPPORT,NULL))
			return TRUE;
	}
	return FALSE;
}

BOOL RingDIB::Load(UINT uBmpId)
{
   Destroy();
   return LoadBmp(MAKEINTRESOURCE(uBmpId),this);
}

BOOL RingDIB::Load(LPCTSTR szPicFilename)
{
   Destroy();

   if(IS_INTRESOURCE(szPicFilename))
      return LoadBmp(szPicFilename,this);
   else
   {
      for(int i=0;i<MAX_PIC_FORMAT;i++)
      {
      	if(dib_load_funcs[i].ext_func)
		   {
   	     	if(dib_load_funcs[i].ext_func(szPicFilename,this,DIB_QUERYSUPPORT,NULL) && 
					dib_load_funcs[i].ext_func(szPicFilename,this,DIB_LOADDIB,NULL))
				{
					m_currFunc = dib_load_funcs[i].ext_func;
					return TRUE;
				}
         }
			else
				break;
		}
   }
	return FALSE;
}

BOOL RingDIB::Create(int width,int height)
{
	Destroy();

   if(NewSize(width*height*sizeof(COLORREF)))
   {
		m_width=width;
		m_height=height;
		
      SetSrc(0,0);
      SetDest(0,0);
   	return TRUE;
   }
   else
   	return FALSE;
}

void RingDIB::ClipRect(int& x,int& y,int& width,int& height)
{
	int n;
	//绘制区域处理，限定在图象内部
	if(width == 0)
		n = m_width;
	else
		n = min(m_width,x + width);
	x = max(0,x);
	width = max(0,n - x);
	
	if(height == 0)
		n = m_height;
	else
		n = min(m_height,y + height);
	
	y = max(0,y);
	height = max(0,n - y);
}

void RingDIB::SetSrc(int x,int y,int width,int height)
{	
   ClipRect(x,y,width,height);
	SetRect(&m_rcSrc,x,y,x + width,y + height);
}

void RingDIB::SetDest(int x,int y,int width,int height)
{
   if(height == 0)
   	height = m_height;
	else if(height < 0)
		height = 0;
   if(width == 0)
   	width = m_width;
	else if(width < 0)
		width = 0;

	SetRect(&m_rcDest,x,y,x + width,y + height);
}

BOOL RingDIB::DIBDraw(HDC hDC,int dx,int dy,int dw,int dh,int sx,int sy,int sw,int sh,UINT wFlags)
{
	m_bmif.bmiHeader.biWidth = m_width;
	m_bmif.bmiHeader.biHeight = m_height;
		
	return DrawDibDraw(m_hDrawDib,hDC,dx,dy,dw,dh,&(m_bmif.bmiHeader),m_Bits,
								sx,sy,sw,sh,wFlags);   
}

//根据源，目的坐标绘制
BOOL RingDIB::Draw(HDC hDC,BOOL bStretch/*=TRUE*/,UINT wFlags/*=DDF_HALFTONE*/)
{
	if(m_Bits)
	{
		int w,h;
		if(!bStretch)
		{
			w = m_rcSrc.right - m_rcSrc.left;
			h = m_rcSrc.bottom - m_rcSrc.top;
		}
		else
		{
			w = m_rcDest.right - m_rcDest.left;
			h = m_rcDest.bottom-m_rcDest.top;
		}
		return DIBDraw(hDC,m_rcDest.left,m_rcDest.top,w,h,m_rcSrc.left,m_rcSrc.top,
							m_rcSrc.right-m_rcSrc.left,m_rcSrc.bottom-m_rcSrc.top,wFlags);
	}
	return FALSE;
}

BOOL RingDIB::Draw(BOOL bStretch,UINT wFlags)
{
   BOOL bOK = FALSE;
	if(m_Bits)
	{
		HDC hDC = GetDC(m_hWnd);
		bOK = Draw(hDC,bStretch,wFlags);
		ReleaseDC(m_hWnd,hDC);		
	}
	return bOK;
}

//指定目标与源位置绘制
//目标与源的宽度不指定，则均采用源起始位置后的整个图象宽高
//指定目标宽高，则为拉伸绘制
BOOL RingDIB::Draw(HDC hDC,int dx,int dy,int sx,int sy,int dw,int dh,int sw,int sh,UINT wFlags)
{
   if(m_Bits)
	{
		//源绘制区域处理，限定在图象内部
		ClipRect(sx,sy,sw,sh);
		
		//目标绘制区域处理
		if(dw == 0)
			dw = sw;
		if(dh == 0)
			dh = sh;

		return DIBDraw(hDC,dx,dy,dw,dh,sx,sy,sw,sh,wFlags);
	}
	return FALSE;
}

//指定目标与源位置绘制，
BOOL RingDIB::Draw(int dx,int dy,int sx,int sy,int dw,int dh,int sw,int sh,UINT wFlags)
{
	BOOL bOK = FALSE;
	if(m_Bits)
	{
		HDC hDC = GetDC(m_hWnd);
		bOK = Draw(hDC,dx,dy,sx,sy,dw,dh,sw,sh,wFlags);
		ReleaseDC(m_hWnd,hDC);
	}
	return bOK;
}

BOOL RingDIB::Paint(BOOL bStretch,UINT wFlags)
{
	BOOL bOK = FALSE;
	if(m_Bits)
   {
		PAINTSTRUCT ps;
		BeginPaint(m_hWnd,&ps);
		bOK = Draw(ps.hdc,bStretch,wFlags);		
		EndPaint(m_hWnd,&ps);
   }
   return bOK;
}

BOOL RingDIB::Paint(int dx,int dy,int sx,int sy,int dw,int dh,int sw,int sh,UINT wFlags)
{
	BOOL bOK = FALSE;
	if(m_Bits)
   {
		PAINTSTRUCT ps;
		BeginPaint(m_hWnd,&ps);
		bOK = Draw(ps.hdc,dx,dy,sx,sy,dw,dh,sw,sh,wFlags);		
		EndPaint(m_hWnd,&ps);
   }
   return bOK;
}

//数据COPY操作绘制，无拉伸，整个源图象数据复制（根据目标图象大小剪裁）
BOOL RingDIB::DrawTo(RingDIB* Dest,int x,int y,BOOL bTrans)
{
	return DrawTo(Dest,x,y,0,0,m_width,m_height,bTrans);
}

//数据COPY操作绘制，无拉伸
BOOL RingDIB::DrawTo(RingDIB* Dest,int dx,int dy,int sx,int sy,int w,int h,BOOL bTrans)
{
	if(m_Bits && Dest && Dest->m_Bits)
	{
		ClipRect(sx,sy,w,h);
		if(w == 0 || h == 0)		//源区域为空
			return FALSE;

		Dest->ClipRect(dx,dy,w,h);
		if(w == 0 || h == 0)		//目的区域为空
			return FALSE;

		int i,j,a,t;
		COLORREF *Src,*Dst,*srcbak,*dstbak;
		
		//DIB图象数据上下颠倒，因此图象偏移量须上下颠倒计算
		a = (m_height - (sy + h)) * m_width + sx;
		t = (Dest->m_height - (dy + h)) * Dest->m_width + dx;
		
		srcbak = Src = m_Bits + a;
		dstbak = Dst = Dest->m_Bits + t;
		
		if(m_crKey == 0xffffffff || !bTrans)		//不透明
		{
			for(j=0;j<h;j++)
			{
				for(i=0;i<w;i++)
				{
					*Dst++ = *Src++;           	
				}
				srcbak += m_width;
				dstbak += Dest->m_width;
				Src = srcbak;
				Dst = dstbak;
			}
		}
		else       //透明
		{
			for(j=0;j<h;j++)
			{
				for(i=0;i<w;i++)
				{
					if(*Src != m_crKey)
						*Dst = *Src;
					Src++;Dst++;
				}
				srcbak += m_width;
				dstbak += Dest->m_width;
				Src = srcbak;
				Dst = dstbak;
			}
		}
		return TRUE;
	}
	return FALSE;
}

BOOL RingDIB::DrawToCanvas(int x,int y,BOOL bTrans)
{
   return DrawTo(m_dibCanvas,x,y,bTrans);
}

BOOL RingDIB::StretchTo(RingDIB* Dest,int dx,int dy,int dw,int dh,
								int sx,int sy,int sw,int sh)
{
	if(m_Bits && Dest && Dest->m_Bits)
   {
		float fsw,fsh;
		int i,j,a,t;
		COLORREF *Src,*Dst,*srcbak,*dstbak,*lpsData,*lpdData;

		ClipRect(sx,sy,sw,sh);
		if(sw == 0 || sh == 0)		//源区域为空
			return FALSE;
		
		Dest->ClipRect(dx,dy,dw,dh);
		if(dw == 0 || dh == 0)		//目的区域为空
			return FALSE;

		fsw = (float)sw;
		fsh = (float)sh;
		//DIB图象数据上下颠倒，因此图象偏移量须上下颠倒计算
		a = (m_height - (sy + sh)) * m_width + sx;
		t = (Dest->m_height - (dy + dh)) * Dest->m_width + dx;
		
		srcbak = Src = m_Bits + a;
		dstbak = Dst = Dest->m_Bits + t;
		
		float x = fsw/(float)dw;
		float y = fsh/(float)dh;
		float x_cnt,y_cnt=0.00;
		
		for(j=0;j<dh;j++)
		{
			x_cnt=0.00;
			for(i=0;i<dw;i++)
			{
				*lpdData++ = *lpsData;
				x_cnt += x;
				lpsData = Src + (int)x_cnt;
			}
			y_cnt += y;
			lpsData = Src = srcbak + m_width * (int)y_cnt;
			lpdData += Dest->m_width;
		}
		return TRUE;
	}
	return FALSE;
}

BOOL RingDIB::Fill(COLORREF crColor)
{
   if(m_Bits)
	{
		int nSize = m_width*m_height;
		COLORREF* lpd = m_Bits;
		
		while(nSize-- > 0)
			*lpd++ = crColor;
		
		return TRUE;
	}
	else
		return FALSE;
}

BOOL RingDIB::Fill(BYTE r,BYTE g,BYTE b)
{
   return Fill(RGB(r,g,b));	
}

BOOL RingDIB::FillRect(COLORREF crColor,int x,int y,int width,int height)
{
   if(m_Bits)
	{
		int i,j,a;
		COLORREF *Src,*srcbak;

		ClipRect(x,y,width,height);
		if(width == 0 || height == 0)
			return FALSE;

		a = (m_height - (y + height)) * m_width + x;
		srcbak = Src = m_Bits + a;

		for(j=0;j<height;j++)
		{
  			for(i=0;i<width;i++)
     		{
				*Src++ = crColor;
			}
			srcbak += m_width;
			Src = srcbak;
		}
		return TRUE;		
	}
	return FALSE;
}

//抓屏
BOOL RingDIB::Capture(HWND hwnd/*=NULL*/)
{
	if(hwnd == NULL && m_width != WINVAR_DESKTOP_X && m_height != WINVAR_DESKTOP_Y)
   {
		Create(WINVAR_DESKTOP_X,WINVAR_DESKTOP_Y);
		hwnd = GetDesktopWindow();
	}
   else
		Create(WinWidth(hwnd),WinHeight(hwnd));
		
   HBITMAP hbm;
   HDC hDC = GetDC(hwnd);
   HDC hMemDC = CreateCompatibleDC(hDC);
   hbm = CreateCompatibleBitmap(hDC,m_width,m_height);
   SelectObject(hMemDC,hbm);
   BitBlt(hMemDC,0,0,m_width,m_height,hDC,0,0,SRCCOPY);

   m_bmif.bmiHeader.biWidth=m_width;
   m_bmif.bmiHeader.biHeight=m_height;

   GetDIBits(hDC,hbm,0,m_height,m_Bits,&m_bmif,DIB_RGB_COLORS);

   DeleteDC(hMemDC);
   ReleaseDC(hwnd,hDC);
   DeleteObject(hbm);

   return TRUE;
}

RingDIB* RingDIB::CreateCanvas(HWND hWnd,int width,int height)
{
   if(hWnd == NULL)
   	hWnd = m_hWnd;
   if(width == 0)
   	width = m_width;
   if(height == 0)
   	height = m_height;

	m_guard.Enter();
	if(m_dibCanvas == NULL)
	{
		m_dibCanvas = new RingDIB(hWnd);
		//m_dibCanvas创建时也会将m_nObjCount+1，导致最后一个对象释放时m_nObjCount非0，
		//因此需--退回避免该情况，析构时m_nObjCount的处理在析构函数中判断
		m_nObjCount --;
		if(m_dibCanvas)
			m_dibCanvas->Create(width,height);
	}
	else
		m_dibCanvas->SetTarget(hWnd);

	m_guard.Leave();
	return m_dibCanvas;
}

void RingDIB::FreeCanvas()
{
   m_guard.Enter();
	delete m_dibCanvas;
	m_dibCanvas = NULL;
	m_guard.Leave();
}

//备份图象数据
BOOL RingDIB::Backup()
{
	return (BOOL)MakeCopy(m_backup);	
}

BOOL RingDIB::Restore(BOOL bTrans)
{
	if(m_backup)
		return m_backup->DrawTo(this,0,0,0,0,m_width,m_height,bTrans);
	else
		return FALSE;
}

//相同大小图象复制
BOOL RingDIB::CopyTo(RingDIB* Dest)
{
	if(m_Bits && Dest && Dest->m_Bits && m_width == Dest->m_width && m_height == Dest->m_height)
	{
		memcpy(Dest->m_Bits,m_Bits,m_width*m_height*sizeof(COLORREF));
		return TRUE;
	}
	else
		return FALSE;
}

//创建并复制图象，返回对象必须由调用者销毁
RingDIB* RingDIB::MakeCopy(RingDIB* Dest/*=NULL*/)
{
	if(m_Bits)
	{
		if(Dest == NULL)
			Dest = new RingDIB(m_hWnd);
		if(Dest)
		{
			Dest->Create(m_width,m_height);
			if(CopyTo(Dest))
				return Dest;
		}
	}
	return NULL;
}

//90度旋转图象，旋转后m_rcSrc为整个图象
BOOL RingDIB::Rotate(BYTE pos)
{
	SetSrc(0,0);
	
	if(m_Bits)
	{
		m_actbak = MakeCopy(m_actbak);
		if(m_actbak == NULL)
			return FALSE;
		
		int skip = 4;
		LPBYTE lpData = (LPBYTE)m_actbak->m_Bits;
		int new_width = m_width * skip;
		LPBYTE lpDest;
		int width_plus,height_plus;

		switch(pos)
		{
   		case DIB_LFPOS_UL:        //图像向左旋转90度后上下反转
   			lpDest = (LPBYTE)m_Bits + new_width * m_height - skip;
				width_plus = -new_width-skip;
				height_plus = -skip;
				break;
			case DIB_LFPOS_DL:        //图像向左旋转90度
				lpDest = (LPBYTE)m_Bits + new_width - skip;
				width_plus = new_width - skip;
				height_plus = -skip;
				break;
			case DIB_LFPOS_DR:        //图像向右旋转90度后上下反转
				lpDest = (LPBYTE)m_Bits;
				width_plus = new_width-skip;
				height_plus = skip;
				break;
			case DIB_LFPOS_UR:        //图像向右旋转90度
				lpDest = (LPBYTE)m_Bits + new_width * (m_height - 1);
				width_plus = -new_width-skip;
				height_plus = skip;
				break;
			default:
				return FALSE;
		}

		LPBYTE lpBak = lpDest;
		//宽高已调换
		for(int j=0;j<m_width;j++)
		{
   		for(int i=0;i<m_height;i++)
			{
      		for(int k=0;k<skip;k++)
				{
         		*lpDest++ = *lpData++;					
				}
				lpDest += width_plus;
			}
			lpBak += height_plus;
			lpDest = lpBak;
		}		
		return TRUE;
	}
	return FALSE;
}

//反转绘制
BOOL RingDIB::Flip(BYTE pos)
{
	if(m_Bits)
	{
		m_actbak = MakeCopy(m_actbak);
		
		if(m_actbak == NULL)
			return FALSE;
				
		int skip = 4;
		LPBYTE lpData;
		lpData = (LPBYTE)m_actbak->m_Bits;		
		int width = m_width * skip;
		LPBYTE lpDest;
		int width_plus,height_plus;

		switch(pos)
		{
			case DIB_LFPOS_UL:        //与原图像相同
      		//CopyMemory(Dest->Data(),Data(),Dest->Width() * Dest->Height() * skip);
				return TRUE;
   		case DIB_LFPOS_DL:        //图像上下反转
				lpDest = (LPBYTE)m_Bits + width * (m_actbak->m_height - 1);
				width_plus=0;
				height_plus = -(width<<1);
				break;
			case DIB_LFPOS_DR:        //图像旋转180度
				lpDest = (LPBYTE)m_Bits + width * m_actbak->m_height - skip;
				width_plus = -(skip<<1);
				height_plus=0;
				break;
			case DIB_LFPOS_UR:        //图像左右反转
				lpDest = (LPBYTE)m_Bits + width - skip;
				width_plus = -(skip<<1);
				height_plus = width<<1;
				break;
			default:
				return FALSE;
		}

		for(int j=0;j<m_height;j++)
		{
   		for(int i=0;i<m_width;i++)
			{
				for(int k=0;k<skip;k++)
				{
      			*lpDest++ = *lpData++;
				}
				lpDest += width_plus;
			}
			lpDest += height_plus;
		}
		return TRUE;
	}
	return FALSE;
}

BOOL RingDIB::DrawText(LPCTSTR szText,int x,int y,COLORREF crText/* = 0xFFFFFFFF*/,ringFont* rFont/* = NULL*/,BOOL bAutoAdjust/*=TRUE*/)
{
	if(szText && m_Bits)
	{
		HBITMAP hbm = GetBitmap();
		HDC hDC = GetDC(m_hWnd);
		HDC hMemDC = CreateCompatibleDC(hDC);
		
		SelectObject(hMemDC,hbm);
		SetBkMode(hMemDC, TRANSPARENT);

		if(crText != 0xFFFFFFFF)
			SetTextColor(hMemDC,crText);

		if(rFont && rFont->GetFont())
			if(bAutoAdjust && x < m_width && y < m_height)
				SelectObject(hMemDC,rFont->AutoAdjust(szText,m_width - x,m_height - y,hMemDC));
			else
				SelectObject(hMemDC,rFont->GetFont());

		TextOut(hMemDC,x,y,szText,strlen(szText));

		DeleteObject(hMemDC);
		//DeleteObject(hbm);
		ReleaseDC(m_hWnd,hDC);

		Import(hbm);
		DeleteObject(hbm);

		return TRUE;
	}
	return FALSE;
}

BOOL RingDIB::DrawText(LPCTSTR szText,LPRECT lprc,UINT uFormat,COLORREF crText/* = 0xFFFFFFFF*/,ringFont* rFont/* = NULL*/)
{
	if(szText && m_Bits)
	{
		HBITMAP hbm = GetBitmap();
		HDC hDC = GetDC(m_hWnd);
		HDC hMemDC = CreateCompatibleDC(hDC);
		
		SelectObject(hMemDC,hbm);
		SetBkMode(hMemDC, TRANSPARENT);
		
		if(crText != 0xFFFFFFFF)
			SetTextColor(hMemDC,crText);
		
		if(rFont && rFont->GetFont())
			SelectObject(hMemDC,rFont->GetFont());
			
		::DrawText(hMemDC,szText,strlen(szText),lprc,uFormat);
			
		DeleteObject(hMemDC);
		ReleaseDC(m_hWnd,hDC);
			
		Import(hbm);
		DeleteObject(hbm);
			
		return TRUE;
	}
	return FALSE;
}

HRGN RingDIB::CreateRgn(BOOL bForceNoneRect)
{
   int i,j,k;
   COLORREF *lpb,*lpbBak;
   BOOL bFinded = FALSE;
   RECT rt;
   HRGN hrgnS;

	if(m_Bits)
   {
      if(m_hrgn)
	      DeleteObject(m_hrgn);

      if(m_aniGifPic)
      {
      	EnableAniGif(FALSE);
         MakeBkg();
      }
      if(m_crKey == 0xffffffff && bForceNoneRect)
      	m_crKey = m_Bits[m_width * (m_height - 1)];

      lpb = lpbBak = m_Bits;

      for(j=0,k=m_height-1;j<m_height;j++,k--)
      {
         for(i=0;i<m_width;i++)
         {
         	if(*lpb != m_crKey)
            {
            	bFinded = TRUE;
               lpb++;
               i++;
               break;
            }
            lpb++;
         }
         if(bFinded)
         	break;
         lpbBak += m_width;
	      lpb = lpbBak;
      }

      m_hrgn = CreateRectRgn(i,k,i+1,k+1);

		for(;j<m_height;j++,k--)
      {
         rt.top = k;
         rt.bottom = k+1;
         rt.left = -1;
         rt.right = -1;

         while(i<m_width)
         {
	      	for(;i<m_width;i++)
				{
   	   	  	if(*lpb != m_crKey)
	        		{
   	        		rt.left = i;
                  lpb ++;
                  i++;
	            	break;
               }
               lpb ++;
	         }

            for(;i<m_width;i++)
				{
   	   	  	if(*lpb == m_crKey)
	        		{
   	        		rt.right = i;
                  lpb ++;
                  i++;
	            	break;
               }
               lpb ++;
	         }
            if(rt.left == -1)
            	break;
            if(rt.right == -1)
            	rt.right = m_width;
            hrgnS = CreateRectRgn(rt.left,rt.top,rt.right,rt.bottom);
            CombineRgn(m_hrgn,m_hrgn,hrgnS,RGN_OR);
            DeleteObject(hrgnS);

            rt.top = k;
	         rt.bottom = k+1;
   	      rt.left = -1;
      	   rt.right = -1;
         }
	      lpbBak += m_width;
   	   lpb = lpbBak;
         i = 0;
      }
      if(m_aniGifPic)
      	EnableAniGif();

      return m_hrgn;
   }
   return NULL;
}

HRGN RingDIB::CreateCoolWindow(BOOL bForceNoneRect)
{
   CreateRgn(bForceNoneRect);

   if(m_hrgn)
   {
		SetWindowPos(m_hWnd,0,0,0,m_width,m_height,SWP_NOMOVE | SWP_NOZORDER);
   	if(SetWindowRgn(m_hWnd,m_hrgn,TRUE))
     		return m_hrgn;
   }
  	return NULL;
}

//动画GIF用，绘制背景
void RingDIB::MakeBkg()
{
	LPANIGIFPIC mark,save;
   COLORREF *lpmb,*lpl,*lpmbb,*lplb;
   int i,j,r,b,x,y;

   if(!m_aniGifPic)
   	return;

   save = m_aniGifPic->next;
   mark = m_aniGifPic;

   while(save != mark && save)
   {
	   //求剪裁矩形
  		x = save->x + save->width;
		y = save->y + save->height;
		r = x;
		b = y;

   	lpmb = lpmbb = m_Bits + (m_height - y) * m_width + save->x;
	   lpl = lplb = save->curr;

      for(j=save->y;j<b;j++)
	   {
			for(i=save->x;i<r;i++)
			{
        		if(save->keycolor==0xffffffff || *lpl != save->keycolor)
					*lpmb = *lpl;
	     		lpmb++;lpl++;
   	   }
			lplb += save->width;lpmbb += m_width;
	  		lpl = lplb;
         lpmb = lpmbb;
  		}
      save = save->next;
   }
}

BOOL RingDIB::SetAniGifFilter(ANIFILTER_FUNC func)
{
	GetLocalGifInfo();
	if(m_lGifInfo)
	{
		m_lGifInfo->funcFilter = func;
		return TRUE;
	}
	else
		return FALSE;
}

LPANIGIFPIC RingDIB::AddAniGifPic(LPANIGIFPIC prev,BYTE docase,WORD delay,BOOL bTrans)
{
	LPANIGIFPIC temp;
	temp = (LPANIGIFPIC)New(sizeof(ANIGIFPIC));
	if(temp)
	{
		if(m_aniGifPic == NULL)
			m_aniGifPic = temp;
		
		temp->next = NULL;
		temp->curr = NULL;
		temp->prev = prev;
		temp->docase=docase;
		temp->delay=delay;
		if(bTrans)
			temp->keycolor=2;
		else
			temp->keycolor=0xFFFFFFFF;
		
		if(prev)
			prev->next = temp;		
	}
	return temp;
}
