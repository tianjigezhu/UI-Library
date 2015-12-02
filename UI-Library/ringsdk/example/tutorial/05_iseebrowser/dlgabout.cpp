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
//							RingSDK 演示程序(ISee图象浏览器)
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
//说明：版权信息对话框，水波效果的实现
//
**********************************************************************/
#include "global.h"

BEGIN_REGMSG(DlgAbout)
	REGMSG(WM_MOUSEMOVE)
	REGMSG(WM_LBUTTONDOWN)
	REGMSG(WM_CLOSE)
	REGMSG(WM_INITDIALOG)
END_REGMSG

RINGMSG(DlgAbout,WM_INITDIALOG)
{
	m_dib = new RingDIB(m_hWnd);
	//载入图象
	m_dib->Load(IDB_ABOUT);
	//获取图象尺寸
	int nSize = m_dib->Width() * m_dib->Height();
	//创建波形缓冲区
	m_buf1 = (short*)New(nSize*sizeof(short));
	m_buf2 = (short*)New(nSize*sizeof(short));
	//创建画布（与图象大小相同）
	m_dib->CreateCanvas();
	//复制图象至画布
	m_dib->DrawToCanvas();

	HDC hdc = GetDC(m_hWnd);
   int cxScreen = GetDeviceCaps(hdc,HORZRES);
   int cyScreen = GetDeviceCaps(hdc,VERTRES);
   ReleaseDC(m_hWnd,hdc);
	//居中显示
	SetPos((cxScreen-m_dib->Width())/2,(cyScreen-m_dib->Height())/2,m_dib->Width(),m_dib->Height());
	
	//创建及启动水波绘制线程
	m_thread = new RingThread;
	m_thread->Start(this,(THREADFUNC)RippleThread);

	return FALSE;
}

RINGMSG(DlgAbout,WM_LBUTTONDOWN)
{
	//按左键发送退出消息，结束对话框
	SendExit();
	return FALSE;
}

RINGMSG(DlgAbout,WM_CLOSE)
{
	//关闭时的清理工作
	delete m_thread;
	Del(m_buf1);
	Del(m_buf2);
	delete m_dib;
	Close();
	return FALSE;
}

RINGMSG(DlgAbout,WM_MOUSEMOVE)
{
	//鼠标移动时往水里扔“石头”，即在波形缓冲区指定位置半径为4范围内初始化能量为96
	DropStone(param.mousept.x,param.mousept.y,4,96);
	return FALSE;
}

DWORD RINGMETHOD(DlgAbout,RippleThread)(LONG)
{
	while(m_thread && m_thread->IsRunning())
	{
		RippleSpread();					//计算波能数据缓冲区
		RenderRipple();					//页面渲染		
		m_dib->GetCanvas()->Draw();	//绘制
		Sleep(10);
	}
	return 0xDEAD;
}

void RINGMETHOD(DlgAbout,RippleSpread)()
{
	int s = m_dib->Width();
	int e = m_dib->Width() * (m_dib->Height() - 1);

	for(int i=s;i<e;i++)
	{
      if((i%s-1) < 0 || (i%s+1) > s)
			continue;
		//波能扩散,上下左右四个点的能量和/2-中间点能量
		m_buf2[i] = (short)(((m_buf1[i-1]+m_buf1[i+1]+m_buf1[i-s]+m_buf1[i+s])>>1)	- m_buf2[i]);
		//波能衰减，否则水波会无限震荡下去
		m_buf2[i] -= (short)(m_buf2[i]>>5);
	}
	//交换波能数据缓冲区
	short *ptmp = m_buf1;
	m_buf1 = m_buf2;
	m_buf2 = ptmp;
}

void RINGMETHOD(DlgAbout,RenderRipple)()
{
	//进行页面渲染，计算每个像素的偏移位置,将其移动到计算出的位置
	int xoff, yoff;
	int k = m_dib->Width();
	int h = m_dib->Height();
	int ix,iy,w = k;
	int pos1, pos2;
	RingDIB* dib = m_dib->GetCanvas();
	COLORREF *src,*dst;
	for(int i=1;i<h-1;i++)
	{
		for(int j=0;j<w;j++)
		{
			//计算偏移量
			xoff = m_buf1[k-1] - m_buf1[k+1];
			yoff = m_buf1[k-w] - m_buf1[k + w];
			
			ix = j + xoff;
			iy = i + yoff;
			//判断坐标是否在窗口范围内
			if(ix < 0 || ix >= w || iy < 0 || iy >= h)
			{
				k++;
				continue;
			}
			
			//计算出偏移象素和原始象素的内存地址偏移量
			pos1 = m_dib->Width() * iy + ix;
			pos2 = w * i + j;
			
			src = m_dib->Data() + pos1;
			dst = dib->Data() + pos2;

			//复制象素			
			*dst = *src;
			k++;
		}
	}
}

void RINGMETHOD(DlgAbout,DropStone)(int x,int y,int stonesize,int stoneweight)
{
	//判断坐标是否在屏幕范围内
	if((x + stonesize) > m_dib->Width() ||
		(y + stonesize) > m_dib->Height() ||
		(x - stonesize) < 0 || (y-stonesize) < 0)
		return;

	//波能填充
	for(int posx=x-stonesize;posx<x+stonesize;posx++)
		for(int posy=y-stonesize;posy<y+stonesize;posy++)
			if((posx-x)*(posx-x) + (posy-y)*(posy-y) < stonesize*stonesize)	//近似圆周范围判断
				m_buf1[m_dib->Width() * (m_dib->Height() - posy) + posx] = (short)(-stoneweight);
}
