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
//原文件名：ringdib.h
//
//说明：RingDIB.lib的头文件，使用该类库必须包含本文件
//
**********************************************************************/
#ifndef _INC_RINGDIB
#define _INC_RINGDIB
//#pragma warn -pck
#include "ringlib.h"

#include <mmsystem.h>
#include <math.h>
#include <vfw.h>

#define RC_BMP				0x730001	//资源类型：BMP
#define RC_DATA         0x730002	//资源类型：数据

//#define DIB_BMP		6600000
//#define DIB_PCX		6600001
//#define DIB_JPG		6600002
//#define DIB_GIF		6600003
//#define DIB_MIX		6600004

#define PCX		"pcx"
#define GIF		"gif"
#define JPG		"jpg"
#define PNG		"png"

#define DIB_LOADDIB			0
#define DIB_QUERYSUPPORT	1
#define DIB_GETFORMAT		2

#define FAIL_SUCCESS	8800001
#define FAIL_OTHER	8800002

typedef struct tagTRUECOLOR{
	BYTE r;
   BYTE g;
   BYTE b;
   BYTE alpha;
}RGBCOLOR,*LPRGBCOLOR;

#pragma pack(push,1)
typedef struct tagAniGifImage{		//动画GIF图象信息
	tagAniGifImage* prev;            //上一幅图象信息
   COLORREF* curr;                  //当前图象数据
   tagAniGifImage* next;            //下一幅图象信息
   COLORREF keycolor;					//透明色索引
   int x,y,width,height;
   WORD delay;								//图象延时
   BYTE docase;							//图象处理方法
}ANIGIFPIC,*LPANIGIFPIC;
#pragma pack(pop)

typedef void (*ANIFILTER_FUNC)(void);

typedef struct tagLOCALGIFINFO{
	RingThread* AniThread;				//动画线程句柄
   COLORREF bgC;					 		//背景色
   BOOL bNeedBak;							//动画中需要记录不变图象。(docase = 0 or 1)
   BOOL bActive;							//允许动画
   BOOL bDibReady;						//图象是否准备好可绘制到窗口
   ANIFILTER_FUNC funcFilter;			//图象准备好后如需要处理可设置该函数（如继续在上面绘制图象）
}LOCALGIFINFO,*LPLOCALGIFINFO;

class RingDIB;

extern "C" {
//RingDIB外挂图象解码程序
BOOL LoadGIF(LPCTSTR szFilename,RingDIB* lprd,int iFlag=DIB_LOADDIB,LPSTR lpszFormat=NULL);
BOOL LoadPCX(LPCTSTR szFilename,RingDIB* lprd,int iFlag=DIB_LOADDIB,LPSTR lpszFormat=NULL);
BOOL LoadJPG(LPCTSTR szFilename,RingDIB* rdib,int iFlag=DIB_LOADDIB,LPSTR lpszFormat=NULL);
BOOL LoadPNG(LPCTSTR szFilename,RingDIB* rdib,int iFlag=DIB_LOADDIB,LPSTR lpszFormat=NULL);

BOOL SaveJPG(LPCTSTR szFilename,RingDIB* rdib,BOOL bRGB=TRUE,int quality=50);
BOOL SaveBMP(LPCTSTR szFilename,RingDIB* rdib);

//动画GIF单幅图象绘制好后默认的处理函数（该函数不处理图象）
void NoneFilter();

//在图象上绘制矩形用。（DragRect需要调用）
//根据在图象上的绝对位置求绘制到屏幕的位置
BOOL Get_OnDCRect(RingDIB* dib,int rate,CONST RECT* lpsrc,LPRECT lpdst);
//根据绘制到屏幕的位置求在图象上的绝对位置
BOOL Get_OnDIBRect(RingDIB* dib,int rate,CONST RECT* lpsrc,LPRECT lpdst);
BOOL Get_OnDCPt(RingDIB* dib,int rate,CONST POINT* lpsrc,LPPOINT lpdst);
BOOL Get_OnDIBPt(RingDIB* dib,int rate,CONST POINT* lpsrc,LPPOINT lpdst);

//求图象缩放比率（只返回整数）
int GetDIBShowRate(RingDIB* dib);
}

#define MAX_FILTER 	10			//最多支持滤镜插件类

//RingDIB滤镜插件类声明
class dibFilterALPHA;
class dibFilterEFFECT;

//RingDIB滤镜插件类的基类
typedef struct DibFilter
{
	DibFilter():m_rdib(NULL){}
	virtual ~DibFilter()=0;		//纯虚函数，函数体代码在rdib.cpp
protected:
	RingDIB* m_rdib;
}DIBFILTER,*LPDIBFILTER;

typedef struct _tagDibFilterInfo
{
	LPVOID m_filter[MAX_FILTER];
   //新的RingDIB滤镜插件类在下面加入
	//命名规则：dibFilterXXX* m_lpXXX
	//XXX为类名，如下面的ALPHA,EFFECT
   dibFilterALPHA* m_lpALPHA;
	dibFilterEFFECT* m_lpEFFECT;
}DIBFILTERINFO,*LPDIBFILTERINFO;

//计时器基类
class RingTimer
{
public:
	RingTimer(){}
	virtual ~RingTimer()=0{}
	
   void Reset();		//重新开始计时
   void Count();     //计时
   int PassedTime();	//自开始计时到调用该函数时的流逝时间（毫秒）
};

//RingTimer::~RingTimer(){}

//毫秒级计时器
class RingMSTimer:public RingTimer
{
public:
	RingMSTimer():m_initTime(timeGetTime()),m_passedTime(0),m_currTime(0){}
	~RingMSTimer(){}

   void Reset();		//重新开始计时
   void Count();     //计时
   int PassedTime();	//自开始计时到调用该函数时的流逝时间（毫秒）

private:
	int m_initTime,m_passedTime,m_currTime;
};

#pragma warning(disable:4035)
//钠秒级计时器,摘自《Windows图形编程》
class RingNSTimer:public RingTimer
{
public:
	RingNSTimer()
	{
		m_cpu = 1;
		m_overHead = 0;
		Reset();
		m_overHead = PassedTime();
		Reset();
		Sleep(100);
		m_cpu = PassedTime()/10000;
	}
	~RingNSTimer(){}

	inline __int64 GetCycleCount()
	{
		_asm _emit 0x0F
		_asm _emit 0x31
	}

	void Reset(){m_startCycle = GetCycleCount();}
	void Count(){}
	//返回微秒,1小时内必须Reset，否则返回值截断
	int PassedTime(){return (int)((GetCycleCount() - m_startCycle - m_overHead)/m_cpu)/100;}

private:
	unsigned __int64 m_startCycle,m_overHead,m_cpu;
};
#pragma warning(default:4035)

/*
class RingDraw
{
public:
	static HDRAWDIB Open();
	static void Close ();

   static BOOL Draw(RingDIB* rdib,UINT wFlags=DDF_HALFTONE);
	static BOOL Draw(HDC hDC,RingDIB* rdib,UINT wFlags=DDF_HALFTONE);
	static BOOL Paint(RingDIB* rdib,PAINTSTRUCT* lps,UINT wFlags=DDF_HALFTONE);
   static BOOL DrawTo(RingDIB* src,RingDIB* dest,BOOL bTrans=TRUE);
   static BOOL StretchTo(RingDIB* src,RingDIB* dest,BOOL bAll);
   static BOOL CopyToBack(RingDIB* rdib);

   static BOOL CreateBack(HWND hWnd,int x,int y);     //创建内存图象
	static void FreeBack();                			//释放内存图象

public:
	static HDRAWDIB m_hDrawDib;
   static BITMAPINFO m_bmif;
   static int m_count;
   static BOOL m_bWithBack;
};
*/
typedef BOOL (*DECODER_FUNC)(LPCTSTR,RingDIB*,int,LPSTR);
typedef void (*FILTER_FUNC)(RingDIB*);

//图象旋转/反转标志
//技巧：pos = DIB_LFPOS_UL;
//左转：pos = (BYTE)(pos+DIB_LFPOS_DL);
//右转：pos = (BYTE)(pos-DIB_LFPOS_DL);
//横向反转：pos ^= DIB_LFPOS_UR;
//纵向反转：pos ^= DIB_LFPOS_DL;
//遵照此规则不论图象旋转/反转多少次，最终原始图象调用
//RotateTo或FlipTo（当然必须知道图象是否宽高调换），
//把pos作为参数，图象旋转/反转结果始终正确。
#define DIB_LFPOS_UL	0x00			//原图像左上角旋转后在左上角
#define DIB_LFPOS_DL	0x40        //原图像左上角旋转后在左下角
#define DIB_LFPOS_DR	0x80        //原图像左上角旋转后在右下角
#define DIB_LFPOS_UR	0xc0        //原图像左上角旋转后在右上角

class RingDIB
{
public:
	RingDIB();
	RingDIB(HWND hWnd);
	~RingDIB();

	//创建，载入图象操作
   BOOL Create(int width,int height);
   BOOL Load(LPCTSTR szPicFilename);
   BOOL Load(UINT uBmpId);
	BOOL Import(HBITMAP hbm);
	BOOL Import(LPBYTE lpdata,int width,int height);
   BOOL Select(BOOL bAllFile=FALSE,HWND hWnd=GetMainWnd(),LPCTSTR dlgEx=NULL,LPOFNHOOKPROC hookproc=NULL);
	BOOL IsImage(LPSTR szPicFilename);

	//图象属性导出
	COLORREF* Data(){return m_Bits;}
   int Width(){return m_width;}
   int Height(){return m_height;}
	int GetSrcX(){return m_rcSrc.left;}
	int GetSrcY(){return m_rcSrc.top;}
	int GetDestX(){return m_rcDest.left;}
	int GetDestY(){return m_rcDest.top;}
	BOOL GetSrcRect(LPRECT lpdst){return lpdst?CopyRect(lpdst,&m_rcSrc):FALSE;}
	BOOL GetDestRect(LPRECT lpdst){return lpdst?CopyRect(lpdst,&m_rcDest):FALSE;}
	HWND GetDestWindow(){return m_hWnd;}
	RingDIB* GetDestDIB(){return m_target;}
	RingDIB* GetCanvas(){return m_dibCanvas;}
	LPANIGIFPIC GetAniGifPic(){return m_aniGifPic;}
   LPLOCALGIFINFO GetLocalGifInfo();
	//返回图象格式，如"gif","jpeg"等等
	LPSTR GetFormat(LPSTR lpszFormat);
	
	//设置
	LPANIGIFPIC AddAniGifPic(LPANIGIFPIC prev,BYTE docase,WORD delay,BOOL bTrans);
	void SetTarget(HWND hWnd){m_hWnd = hWnd;}
	void SetTarget(RingDIB* dib){m_target = dib;}
   static BOOL RegFormat(LPCSTR ext,DECODER_FUNC ext_func);
   
   void SetSrc(int x,int y,int width=0,int height=0);
   void SetDest(int x,int y,int width=0,int height=0);
	
   void SetColorKey(COLORREF crkey){m_crKey=crkey;}
	COLORREF GetColorKey(){return m_crKey;}

	//DrawDibDraw绘制操作
   BOOL Draw(BOOL bStretch=TRUE,UINT wFlags=DDF_HALFTONE);
	BOOL Draw(HDC hDC,BOOL bStretch=TRUE,UINT wFlags=DDF_HALFTONE);
   BOOL Draw(int dx,int dy,int sx,int sy,int dw=0,int dh=0,int sw=0,int sh=0,UINT wFlags=DDF_HALFTONE);
	BOOL Draw(HDC hDC,int dx,int dy,int sx,int sy,int dw=0,int dh=0,int sw=0,int sh=0,UINT wFlags=DDF_HALFTONE);
   //Paint不需要HDC参数，有该参数可调用Draw(hDC,...)
	BOOL Paint(BOOL bStretch=TRUE,UINT wFlags=DDF_HALFTONE);
   BOOL Paint(int dx,int dy,int sx,int sy,int dw=0,int dh=0,int sw=0,int sh=0,UINT wFlags=DDF_HALFTONE);
	
	//直接图象数据操作
	BOOL DrawTo(RingDIB* Dest,int x=0,int y=0,BOOL bTrans=TRUE);
   BOOL DrawTo(RingDIB* Dest,int dx,int dy,int sx,int sy,int w=0,int h=0,BOOL bTrans=TRUE);
	//相同大小图象复制
	BOOL CopyTo(RingDIB* Dest);
	//创建并复制图象，返回对象必须由调用者销毁
	RingDIB* MakeCopy(RingDIB* Dest=NULL);
   //拉伸绘制
	BOOL StretchTo(RingDIB* Dest,int dx=0,int dy=0,int dw=0,int dh=0,int sx=0,int sy=0,int sw=0,int sh=0);
	BOOL Fill(COLORREF crColor);
   BOOL Fill(BYTE r,BYTE g,BYTE b);
	BOOL FillRect(COLORREF crColor,int x=-1,int y=-1,int width=0,int height=0);
	BOOL Rotate(BYTE pos);		//旋转90度
	BOOL Flip(BYTE pos);		//反转
   
	//画布操作
	//创建内存图象(画布)
	RingDIB* CreateCanvas(HWND hWnd = NULL,int width = 0,int height = 0);
	BOOL DrawToCanvas(int x = 0,int y = 0,BOOL bTrans=TRUE);
   void FreeCanvas();								//释放内存图象
	
	//备份操作
   BOOL Backup();			 					//备份图象数据
	BOOL Restore(BOOL bTrans=FALSE);		//恢复图象数据
	
	//动画GIF图象处理
	BOOL IsAniGif(){return (BOOL)m_aniGifPic;}
	void EnableAniGif(BOOL bActive=TRUE);
   BOOL SetAniGifFilter(ANIFILTER_FUNC func);

	//扩展操作
   BOOL Capture(HWND hwnd = NULL);	//=NULL,全屏
	BOOL DrawText(LPCTSTR szText,int x,int y,COLORREF crText = 0xFFFFFFFF,ringFont* rFont = NULL,BOOL bAutoAdjust=FALSE);
	BOOL DrawText(LPCTSTR szText,LPRECT lprc,UINT uFormat,COLORREF crText = 0xFFFFFFFF,ringFont* rFont = NULL);
	//转换已有图象到HBITMAP,必须手工销毁HBITMAP（DeleteObject)
	HBITMAP GetBitmap();	

   HRGN CreateRgn(BOOL bForceNoneRect = TRUE);
   HRGN CreateCoolWindow(BOOL bForceNoneRect = TRUE);
   void MakeBkg();

public:   
   DIBFILTERINFO m_Filter;
   int pluged_filter;

private:
	static BOOL LoadBmp(LPCTSTR szFilename,RingDIB* lprdib,int iFlag=DIB_LOADDIB,LPSTR lpszFormat=NULL);
   void Destroy();
	BOOL NewSize(int nNewsize);	//分配图象数据缓冲区
	void init();
	BOOL DIBDraw(HDC hDC,int dx,int dy,int dw,int dh,int sx,int sy,int sw,int sh,UINT wFlags);
	//绘制区域调整处理，限定在图象内部
	//使用注意，不可传入成员变量！！！必须复制后传入
	void ClipRect(int& x,int& y,int& width,int& height);
   
private:
   static int pluged_ext;			//滤镜插件数量
	static HDRAWDIB m_hDrawDib;   
   static LONG m_nObjCount;
	static RingDIB* m_dibCanvas;	//全局画布
   DECODER_FUNC m_currFunc;		//当前解码程序
	BITMAPINFO m_bmif;
	RECT m_rcSrc,m_rcDest;
   COLORREF m_crKey;
	HWND m_hWnd;						//绘画目标窗口句柄
	HRGN m_hrgn;
   RingDIB* m_backup,*m_actbak,*m_target;
   COLORREF* m_Bits;
   int m_width,m_height;
	int m_size;							//分配的图象缓冲区尺寸
	LPANIGIFPIC m_aniGifPic;
   LPLOCALGIFINFO m_lGifInfo;     //动画GIF信息
	static RingDataGuard m_guard;
};

//注册图象解码程序
#define DIBREGFORMAT(pic)			RingDIB::RegFormat(pic,Load##pic)
//同一函数支持多文件格式注册，如jpg,jpeg，均调用LoadJPG
#define DIBREGFORMATS(ext,pic)	RingDIB::RegFormat(ext,Load##pic)

//注册插件类
#define DIBREGFILTER(dib,filter)\
{\
	RingDIB& ___rdib = dib;\
   if(___rdib.pluged_filter<MAX_FILTER && ___rdib.m_Filter.m_lp##filter==NULL){\
		try{___rdib.m_Filter.m_filter[___rdib.pluged_filter] = ___rdib.m_Filter.m_lp##filter = new dibFilter##filter(___rdib);\
         ___rdib.pluged_filter ++;}\
						catch(char *)\
      {Errmsg("RingDIB 挂接滤镜模块失败");abort();}\
   }\
}

#define FILTERS(filter) m_Filter.m_lp##filter

//RingDIB滤镜插件类定义
class dibFilterALPHA:public DIBFILTER
{
public:
	~dibFilterALPHA();

	dibFilterALPHA(RingDIB& dib){m_rdib = &dib;};
	void Fill(int r,int g,int b,int a);
   void FillGlass(COLORREF color);
	void Blend(RingDIB *dib,int A);
	void Darken(RingDIB *dib);
	void Difference(RingDIB *dib);
	void Lighten (RingDIB *dib);
	void Multiply (RingDIB *dib);
	void Screen(RingDIB *dib);
	void Xor(RingDIB *dib);
};

typedef struct tagHSLDATA
{
	float m_hue;         // 0.0 .. 360.0  // Winkel
	float m_saturation;  // 0.0 .. 1.0    // Prozent
	float m_luminance;   // 0.0 .. 1.0    // Prozent
	float reserve;
}HSLDATA,*LPHSLDATA;

class dibFilterEFFECT:public DIBFILTER
{
public:
	~dibFilterEFFECT(){}

	dibFilterEFFECT(RingDIB& dib){m_rdib = &dib;}
	
	//转换为256级灰度图象
	BOOL GrayScale();
	//负片效果
   BOOL Negative();
	//调整对比度，参数为百分比去掉百分号，如调整对比度
	//为70%，Percent应为70.参数范围0~200
	BOOL AdjustContrast(int Percent);
	//调整亮度，参数为正，增加亮度，为负则减小亮度，
	//范围应在-255~255之间，255图象全白，-255则全黑
	BOOL AdjustBright(int step);
	//同时调整亮度和对比度，参数规则同上，该函数目的
	//是因为单调整亮度或对比度像素颜色超出0~255会丢弃
	//多余信息，但同时调整则有可能仍在0~255范围，不会
	//丢失信息。
	BOOL AdjustBright_Contrast(int bright,int contrast);
	//调整色调，参数范围：-180~180（度）,=0不作调整
	//调整饱和度，参数范围0~200(建议，最大值可>200),=100不作调整
	//调整亮度，亮度参数范围0~200(建议，最大值可>200)
	//,=100不作调整
	BOOL AdjustHSL(int degHue,int perSaturation,int perLuminosity);
	//未完成，留待以后对图象部分区域操作用
	void SetWorkArea(int x,int y,int width=0,int height=0);

protected:
	void RGBtoHSL(BYTE R,BYTE G,BYTE B,float* H,float* S,float* L);
	BYTE HueToRGB(float rm1, float rm2, float rh);
	COLORREF HSLtoRGB(float H,float S,float L);

private:
	RECT m_rcArea;		//处理范围，暂未用到
	LPHSLDATA m_hslData;
};

//以下定义值切勿更改
#define DRC_LEFT		1
#define DRC_TOP		2
#define DRC_RIGHT		4
#define DRC_BOTTOM	8
#define DRC_LU			3
#define DRC_RU			6
#define DRC_LD			9
#define DRC_RD			12
#define DRC_INSIDE	99

#define ON_DC			1
#define ON_DIB			0
////////////////////////////////////////////////
//
//类似PhotoShop的选择矩形，可拉伸移动
//
////////////////////////////////////////////////
class	RingDragRect
{
public:
	RingDragRect();
	virtual ~RingDragRect();

	void Init(HWND hwnd,RingDIB* dib)
	{
		m_hWnd = hwnd;
		m_dib = dib;		
	}

	BOOL IsActive(){return bActived;}
	BOOL IsMoving(){return bInMove;}
	BOOL IsDraging(){return bInDrag;}

	int left(int isDC = ON_DC);
	int top(int isDC = ON_DC);
	int right(int isDC = ON_DC);
	int bottom(int isDC = ON_DC);
	BOOL GetRect(LPRECT lprc,int isDC = ON_DC);
		
	//父窗口尺寸改变
	void OnParentSize(int width,int height)
	{
		m_width = width;
		m_height = height;		
	}
	virtual void SetRate(int rate);
	void ScrollParent(int dx,int dy);

	virtual void StartDrag(int x,int y);
	virtual void Drag(int x,int y);
	virtual void StopDrag(int x,int y);
	//virtual void StartMoveSide(int x,int y);
	virtual void StartMove(int x,int y);
	virtual void Move(int x,int y);
	virtual void StopMove(int x,int y);
	//void Resize(int x,int y);
	
	virtual void OnLButtonDown(int x,int y);
	virtual void OnLButtonUp(int x,int y);
	virtual int OnMouseMove(int x,int y);
	virtual void OnPaint(int rate);
	
	virtual void Draw(/*BOOL setClear*/);

	void setDIBRect(LPPOINT lpd,LPPOINT lps);//设置图象矩形
	virtual void setDCRect(int x,int y);	//设置绘制矩形	

	int GetDCWidth(){return m_rcDC.right-m_rcDC.left;}
	int GetDCHeight(){return m_rcDC.bottom-m_rcDC.top;}
	
	virtual void EnableDraw(BOOL bDraw = TRUE);
	//擦除矩形。必须先EnableDraw(FALSE)后才有效	
	virtual void Erase();
	void Disable();

protected:
	virtual void Init();

protected:
	HANDLE handleDraw,hevent;
	RECT m_rcDIB,m_rcDC;	//rc:相对图象坐标，rectDC:相对窗口坐标
	//RECT m_dcDIBDest;	//图象
	BOOL bActived;		//是否激活
	BOOL bInDrag,bInMove,bInFirstAct;
	BOOL bShow;			//是否已显示
	HDC hDC;
	POINT pt,ptOrg;
	HWND m_hWnd;
	RingDIB* m_dib;
	//m_width,m_height:父窗口宽高，
	//m_currX,m_currY:当前图象src坐标,
	//m_w,m_h:矩形宽高
	int m_width,m_height,m_currX,m_currY,m_w,m_h;
	int m_nInSide,scrx,scry;//scrx,scry滚动条位置
	int m_rate;
	HBRUSH m_brush;
	
	BOOL bQuitDraw;
	//DragRect动画绘制线程
	friend DWORD DragRectDrawProc(RingDragRect* obj);
};

#define AVIW_NONE			0xFFFFFFFF
#define AVIW_NEW			0x00000001
#define AVIW_BAR			0x00000010
#define AVIW_MENUBAR 	0x00000100
#define AVIW_CENTER  	0x10000000
#define AVIW_DESKCENTER 0x01000000

class RingAvi
{
public:
	RingAvi();
   ~RingAvi();

   void Open(LPCTSTR szPath=NULL);
   void SetWindow(int nflag,int x=0,int y=0,int pos=0);
   HWND Play(int width=0,int height=0);
   HWND PlayFullScreen();
   void ResetWindow(int x=0,int y=0,int pos=0);
   void NextFast(int skip=10000);
   void PrevFast(int skip=10000);
   void Close();

private:
   void CentreWindow(HWND hwnd,int width,int height);

public:
	HWND m_hMCIWnd;
   char szAviFilename[MAX_PATH];
   int nCenter,nWindowStyle;
   int X,Y,m_width,m_height,w_x,w_y,w_width,w_height;

private:
	LONG lAviPos;	//AVI当前播放位置
   LONG lAviLen;	//AVI长度
};

#ifndef MAKE_SELF_LIB
#ifdef _DEBUG
#pragma comment(lib, "ringdibd.lib")
#pragma comment(lib, "jpegd.lib")
#pragma comment(lib, "zlibd.lib")
#pragma comment(lib, "pngd.lib")
#else
#pragma comment(lib, "ringdib.lib")
#pragma comment(lib, "jpegd.lib")
#pragma comment(lib, "zlib.lib")
#pragma comment(lib, "png.lib")
#endif
#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "vfw32.lib")
#endif

#endif

