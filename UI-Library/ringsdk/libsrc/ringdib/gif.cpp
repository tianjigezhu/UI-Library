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
//原文件名：gif.cpp
//
//说明：GIF图象载入的实现。
//
**********************************************************************/
#define MAKE_SELF_LIB
#include "ringdib.h"

#pragma pack(push,1)
typedef struct _gifhead{
	char szID[6];		//GIF87a or GIF89a
   WORD wWidth;
   WORD wHeight;
   BYTE bFlag;
   BYTE bgColor;		//背景色
	//char w_mod_h;     //图象纵横尺寸比,=(w_mod_h+15)/64
//w_mod_h程序中未用到，因为字节对齐的问题，结构中加上该字段
//导致数据解码出错，因此去掉该字段，并简单将接下来读取的数据
//指针+1跳过该字段。
}GIFHEADER,*LPGIFHEADER;

typedef struct _localimageinfo{	//图象数据块信息
   WORD wLeft;                   //图象左位置坐标
   WORD wTop;                    //图象上位置坐标
	WORD wWidth;                  //图象宽
   WORD wHeight;                 //图象高
   BYTE bFlag;                   //压缩字段
}LOCALINFO,*LPLOCALINFO;

typedef void(*OUTDATA)();

OUTDATA outdataFunc;					//LZW解码数据输出程序指针，方便更改数据输出程序

LPLOCALINFO lInfo;
LPBYTE m_SBits=NULL;					//单个图象数据

LPBYTE gPal=NULL,lPal=NULL;		//gPal:全局调色板，lPal:局部调色板
LPBYTE lpGifData=NULL,lpDelData; //lpDelData,=最初的lpGifData,lpGifData需要变动，so 析构时Del(lpDelData)

DWORD aniGifThread(RingDIB* rdib);
void DrawLocalPic(RingDIB *rdib,COLORREF* dest,COLORREF* bits,LPANIGIFPIC lpAniGifPic);

////////////////-----以下定义为LZW解码所用---------////////////////////////////
typedef struct tagLZW_DATA
{
	short base;
	BYTE newcode;
	short length;
}LZW_DATA,*LPLZW_DATA;
#pragma pack(pop)

int first_code, curr_bit_size, bit_overflow;
int bit_pos, data_pos, data_len, entire, code;
int clear_code, data_length, cnt, bit_size;
LPLZW_DATA lpstr=NULL;
LPBYTE lpLzw_Data=NULL;
int interlace,trans_index=-1;				//隔行扫描间隔，透明色索引
int curr_scan;		//当前扫描次数
int nLeft,nTop,nWidth,nHeight;

//ringFile x;

void clear_table(void);
void get_code(void);
void get_data(int num);
void output_data(void);					//静态GIF解码数据输出程序
void output_localdata(void);        //动态GIF单个图象解码数据输出程序
void DecodeLZW(void);
void resetVar();		//重置全局变量
////////////////-----LZW解码定义结束---------////////////////////////////

BOOL LoadGIF(LPCTSTR szFilename,RingDIB* lprd,int iFlag,LPSTR lpszFormat)
{
	if(iFlag == DIB_GETFORMAT)
	{
		if(lpszFormat)
		{
			wsprintf(lpszFormat,"gif");
			return TRUE;
		}
		else
			return FALSE;
	}
	
	if(szFilename == NULL || lprd == NULL)
		return FALSE;

	GIFHEADER gifHeader;
	LPANIGIFPIC lpAniGifPic = NULL;
   LPLOCALGIFINFO lpGifInfo = lprd->GetLocalGifInfo();
	if(lpGifInfo == NULL)
		return FALSE;
	
//x="c:\\dibdebug.txt";
//x.Create(RF_NEW);

	ringFile rf(szFilename);
   rf.Open();
	outdataFunc=output_data;

   if(!rf.Read(&gifHeader,sizeof(GIFHEADER)) || gifHeader.szID[0] != 'G' || 
		gifHeader.szID[1] != 'I' || gifHeader.szID[2] != 'F' || 
		gifHeader.szID[3] != '8' || gifHeader.szID[5] != 'a')
      return FALSE;
	else if(iFlag == DIB_QUERYSUPPORT)
		return TRUE;

   if(!lprd->Create(gifHeader.wWidth,gifHeader.wHeight))
   {
     	//Errmsg("内存分配错误，不能创建图象。");
      rf.Close();
	   return FALSE;
   }
	
   nWidth=gifHeader.wWidth;
   nHeight=gifHeader.wHeight;

   DWORD dwFileSize=rf.Size()-sizeof(GIFHEADER);
   lpGifData=(LPBYTE)New(dwFileSize);
   if(lpGifData==NULL)
   {
   	rf.Close();
   	return FALSE;
   }

   if(!rf.Read(lpGifData,dwFileSize))
   {
   	rf.Close();
   	return FALSE;
   }
   rf.Close();
	
	lpDelData=lpGifData;

	lpGifData ++;
//int ps;
   
   if((gifHeader.bFlag&0x80))	//有全局调色板
   {
   	int palsize=1<<((gifHeader.bFlag&0x07)+1);
		palsize=(palsize<<1)+palsize;       //palsize*3
      gPal=(LPBYTE)lpGifData;
      lpGifData+=palsize;
//ps=palsize;
   }

   if(gPal!=NULL)
   {
   	int bg=gifHeader.bgColor*3+2;
   	lprd->Fill(gPal[bg],gPal[bg-1],gPal[bg-2]);
      lpGifInfo->bgC = lprd->Data()[0];
   }

   while((lpGifData-lpDelData)<=(int)dwFileSize)
   {
   	switch(*lpGifData)
      {
         case 0x2C:			//图象描述符
         {
         	lpGifData++;
            lInfo=(LPLOCALINFO)lpGifData;

         	lpGifData+=9;
            if((lInfo->bFlag&0x80))	//有局部调色板
			   {
			   	int lpalsize=1<<((lInfo->bFlag&0x07)+1);
					lpalsize=(lpalsize<<1)+lpalsize;       //palsize*3
			      lPal=(LPBYTE)lpGifData;
			      lpGifData+=lpalsize;
			   }
            else if(gPal!=NULL)
            	lPal=NULL;

            if(lInfo->bFlag&0x40)
            	interlace = 8;
				else
					interlace = 1;

            curr_scan=1;

            m_SBits=(LPBYTE)lprd->Data();

            if(lpAniGifPic)
            {
            	lpAniGifPic->x = lInfo->wLeft;
               lpAniGifPic->y = lInfo->wTop;
               lpAniGifPic->width = lInfo->wWidth;
               lpAniGifPic->height = lInfo->wHeight;
               if(lpAniGifPic->curr!=NULL)			//下一个图象分配出错,lprd->m_aniGifPic始终指向新图象，curr应为NULL
               {
						Del(lpDelData);
		            resetVar();
      		      return TRUE;
               }
               lpAniGifPic->curr = (COLORREF*)New(lInfo->wWidth*lInfo->wHeight*4);
               if(lpAniGifPic->curr == NULL)
               {
						Del(lpDelData);
		            resetVar();
      		      return TRUE;
               }
               m_SBits = (LPBYTE)lpAniGifPic->curr;
               outdataFunc = output_localdata;

               if(lpAniGifPic->keycolor != 0xFFFFFFFF)
               {
                  LPBYTE lpPal;
               	if(lPal!=NULL)
						 	lpPal = lPal;
						else
							lpPal = gPal;

                  if(lpAniGifPic->keycolor == 1)
                     lprd->SetColorKey((lpPal[trans_index]<<16)+(lpPal[trans_index-1]<<8)+lpPal[trans_index-2]);

                  lpAniGifPic->keycolor=(lpPal[trans_index]<<16)+(lpPal[trans_index-1]<<8)+lpPal[trans_index-2];
               }
            }
            DecodeLZW();

				//Del(lpDelData);
            //resetVar();
            //return TRUE;
            break;
			}
         case 0x21:
         {
         	lpGifData++;
            switch(*lpGifData)
            {
               case 0xFF:				//应用程序扩展
               	lpGifData+=13;
                  break;
            	case 0xF9:				//图象控制扩展
               {
                  BYTE docase;
                  BOOL trans;
                  WORD delay;

                  lpGifData+=2;
                  docase=(BYTE)(((*lpGifData)>>2)&0x07);
                  if(docase == 0x3)
						   lpGifInfo->bNeedBak = TRUE;

                  trans=(*lpGifData)&1;
                  lpGifData++;
                  delay=*(WORD*)lpGifData;
                  lpGifData+=2;

                  if(trans)
						   trans_index=(int)*lpGifData*3+2;

                  if(delay==0 && lpAniGifPic == NULL)		//无延时，单个图象
	               {
                     if(trans && gPal != NULL)
                       	lprd->SetColorKey((gPal[trans_index]<<16)+(gPal[trans_index-1]<<8)+gPal[trans_index-2]);
                  }
                  else
                  {
		         		lpAniGifPic = lprd->AddAniGifPic(lpAniGifPic,docase,delay,trans);
							if(lpAniGifPic == NULL)
                  	{
		                 	if(lpAniGifPic == NULL)
                     	{
                      		lpGifData+=2;
                       		break;
                     	}                     	
                     }                     
						}
                  lpGifData+=2;
						break;
               }
               case 0xFE:				//注释块
               	for(int i=0;i<254;i++)
                  {
                     lpGifData++;
                  	if(*lpGifData==0)
                     {
                     	lpGifData++;
                        break;
                     }
                  }
                  break;
            }
            break;
         }
         case 0x3B:						//数据流结束
         {
         	Del(lpDelData);
            resetVar();

            if(lpAniGifPic)
            {
            	//将链表头尾相连
					lpAniGifPic->next = lprd->GetAniGifPic();
					lprd->GetAniGifPic()->prev = lpAniGifPic;
					if(lpGifInfo->AniThread == NULL)
						lpGifInfo->AniThread = new RingThread;
					if(lpGifInfo->AniThread)
						lpGifInfo->AniThread->Start((THREADPROC)aniGifThread,(LONG)lprd);
            }
		      return TRUE;
         }
         default:
         	lpGifData++;
            break;
      }
   }
   Del(lpDelData);
   resetVar();
	return FALSE;
}

/////////////////////----  LZW解码  -------///////////////////////////////
void clear_table(void)
{
	first_code = clear_code + 2;
	curr_bit_size = bit_size + 1;
	bit_overflow = 0;
}

void get_code(void)
{
	if(bit_pos+curr_bit_size>8)
   {
		if(data_pos>=data_len)
      {
      	data_len=*lpGifData;
         lpGifData++;
         data_pos=0;
      }
		entire=(*lpGifData<<8)+entire;
      lpGifData++;
		data_pos ++;
	}
	if(bit_pos+curr_bit_size>16)
   {
		if(data_pos>=data_len)
      {
      	data_len=*lpGifData;
         lpGifData++;
         data_pos=0;
      }
		entire=(*lpGifData<<16)+entire;
      lpGifData++;
		data_pos ++;
	}

	code=(entire>>bit_pos)&((1<<curr_bit_size)-1);

	if(bit_pos+curr_bit_size>8)
		entire>>=8;
	if(bit_pos+curr_bit_size>16)
		entire>>=8;
	bit_pos=(bit_pos+curr_bit_size)%8;
	if(bit_pos==0)
   {
		if(data_pos>=data_len)
      {
      	data_len=*lpGifData;
         lpGifData++;
         data_pos=0;
      }
		entire=*lpGifData;
      lpGifData++;
		data_pos ++;
	}
}

void get_data(int num)
{
	if(num<clear_code)
	{
		data_length = 1;
		lpLzw_Data[0]=lpstr[num].newcode;
	}
	else
	{
		cnt=lpstr[num].length;
		data_length=cnt;
		while(cnt>0)
		{
			cnt--;
			lpLzw_Data[cnt]=lpstr[num].newcode;
			num=lpstr[num].base;
		}
	}
}

void output_data(void)
{
   LPBYTE lpPal;
   int pos;
   int temp;

   if(lPal!=NULL)
	 	lpPal = lPal;
   else
		lpPal = gPal;

   pos=(nLeft+nTop*nWidth)<<2;

//x.Write(lpPal,256*3);

	for(int i=0; i<data_length;i++)
	{
      temp=lpLzw_Data[i]*3+2;
      //if(temp!=trans_index)
      //{
			m_SBits[pos]=lpPal[temp];
   	   pos++;temp--;
      	m_SBits[pos]=lpPal[temp];
	      pos++;temp--;
   	   m_SBits[pos]=lpPal[temp];
      	pos++;
	      m_SBits[pos]=0;
   	   pos++;
      //}
      //else
      	//pos+=4;

		nLeft++;

		if(nLeft>=lInfo->wLeft+lInfo->wWidth)
		{
			nLeft = lInfo->wLeft;
			nTop-=interlace;
         pos=(nLeft+nTop*nWidth)<<2;

			if(nTop<lInfo->wTop)
			{
            switch(interlace)
            {
              	case 8:
                  if(curr_scan==1)
                  {
                    	nTop=lInfo->wTop+nHeight-5;		//-5=-1-4
                     pos=(nLeft+nTop*nWidth)<<2;
                     curr_scan++;
                  }
                  else
                  {
                     interlace=4;
                   	nTop=lInfo->wTop+nHeight-3;		//-3=-1-2
                     pos=(nLeft+nTop*nWidth)<<2;
                  }
						break;
               case 4:
                 	interlace=2;
                 	nTop=lInfo->wTop+nHeight-2;		//-2=-1-1
                  pos=(nLeft+nTop*nWidth)<<2;
                	break;
            }
         }
		}
	}
//x.Close();
}

void output_localdata(void)
{
   LPBYTE lpPal;
   int pos;
   int temp;

   if(lPal!=NULL)
	 	lpPal = lPal;
   else
		lpPal = gPal;

   pos=(nLeft+nTop*lInfo->wWidth)<<2;

	for(int i=0; i<data_length;i++)
	{
      temp=lpLzw_Data[i]*3+2;
		m_SBits[pos]=lpPal[temp];
      pos++;temp--;
      m_SBits[pos]=lpPal[temp];
      pos++;temp--;
      m_SBits[pos]=lpPal[temp];
      pos++;
      m_SBits[pos]=0;
      pos++;

		nLeft++;

		if(nLeft>=lInfo->wWidth)
		{
			nLeft = 0;
			nTop-=interlace;
         pos=(nTop*lInfo->wWidth)<<2;

			if(nTop<0)
			{
            switch(interlace)
            {
              	case 8:
                  if(curr_scan==1)
                  {
                    	nTop=lInfo->wHeight-5;	//-5=-1-4
                     pos=(nTop*lInfo->wWidth)<<2;
                     curr_scan++;
                  }
                  else
                  {
                     interlace=4;
                   	nTop=lInfo->wHeight-3;
                     pos=(nTop*lInfo->wWidth)<<2;
                  }
						break;
               case 4:
                 	interlace=2;
                 	nTop=lInfo->wHeight-2;
                  pos=(nTop*lInfo->wWidth)<<2;
                	break;
            }
         }
		}
	}
}

void DecodeLZW(void)
{
	int old;
   BYTE btData[4096];		//LZW最大码长12BIT，=0xFFF=4095
   LZW_DATA lzw_data[4096];

   lpstr=lzw_data;
   lpLzw_Data=btData;

	bit_size=*lpGifData;
   lpGifData++;
	clear_code=1<<bit_size;

	//初始化解码数据表
   for(short i=0; i<clear_code;i++)
	{
		lzw_data[i].base=-1;
		lzw_data[i].newcode=(unsigned char)i;
		lzw_data[i].length=1;
   }

	bit_pos=0;
	data_len=*lpGifData;
   lpGifData++;
   data_pos=0;
   entire=*lpGifData;
   lpGifData++;
   data_pos ++;
	data_length=0;

   if(outdataFunc==output_data)
   {
	   nLeft=lInfo->wLeft;
   	nTop=lInfo->wTop+nHeight-1;
   }
   else
   {
	   nLeft=0;
   	nTop=lInfo->wHeight-1;
   }

   //解码开始
   clear_table();
	get_code();
   if(code==clear_code)
		get_code();
   get_data(code);
	(*outdataFunc)();
	old = code;

	while(TRUE)
	{
		get_code();

		if(code == clear_code)        //清除码
		{
			clear_table();
			get_code();
			get_data(code);
         (*outdataFunc)();

			old = code;
		}
		else if(code==clear_code+1)	//结束码
      {
      	lpGifData--;
      	break;
      }
      else if(code < first_code)
		{
			get_data(code);
			(*outdataFunc)();

			if(bit_overflow == 0)
         {
				lpstr[first_code].base = (short)old;
				lpstr[first_code].newcode = lpLzw_Data[0];
            lpstr[first_code].length = (short)(lpstr[old].length+1);
            first_code++;
            if(first_code==(1<<curr_bit_size))
            	curr_bit_size ++;
            if(curr_bit_size==13)
            {
					curr_bit_size = 12;
					bit_overflow = 1;
				}
			}

			old = code;
      }
		else
		{
			get_data(old);
			lpLzw_Data[lpstr[old].length]=lpLzw_Data[0];
         data_length++;

         if(bit_overflow==0)
         {
				lpstr[first_code].base=(short)old;
            lpstr[first_code].newcode=lpLzw_Data[0];
            lpstr[first_code].length=(short)(lpstr[old].length+1);
            first_code++;
            if(first_code==(1<<curr_bit_size))
            	curr_bit_size++;
            if(curr_bit_size==13)
            {
					curr_bit_size = 12;
					bit_overflow = 1;
				}
         }
			(*outdataFunc)();
			old = code;
      }
   }
}
//////////////////////--------LZW 解码结束-------------//////////////////////

void resetVar()		//重置全局变量
{
	lInfo=NULL;
	m_SBits=NULL;

	gPal=NULL;
   lPal=NULL;
	lpGifData=NULL;
   lpDelData=NULL;

   trans_index=-1;
   outdataFunc=output_data;

	lpstr=NULL;
	lpLzw_Data=NULL;
}

DWORD aniGifThread(RingDIB* rdib)			//动画GIF显示处理程序（线程）
{
   COLORREF *ani_Bits = NULL;
   RingMSTimer timer;
	LPLOCALGIFINFO lpGifInfo = rdib->GetLocalGifInfo();
	LPANIGIFPIC lpAniGifPic = rdib->GetAniGifPic();
   if(lpGifInfo == NULL)
		return 0;

	if(lpGifInfo->bNeedBak)
   {
   	ani_Bits = (COLORREF*)New(rdib->Width() * rdib->Height() * sizeof(COLORREF));
      if(ani_Bits)
         DrawLocalPic(rdib,ani_Bits,rdib->Data(),lpAniGifPic);
   }

   lpGifInfo->bDibReady = TRUE;

	while(rdib->GetAniGifPic() && lpGifInfo->AniThread && lpGifInfo->AniThread->IsRunning())
   {
      if(lpGifInfo->bActive && lpGifInfo->bDibReady)
	   {
   	   DrawLocalPic(rdib,rdib->Data(),NULL,lpAniGifPic);
         lpGifInfo->funcFilter();
   	      
			if(rdib->GetDestDIB())
			{
				rdib->DrawTo(rdib->GetDestDIB());
				rdib->GetDestDIB()->Draw();
			}
			else
				rdib->Draw();

     		timer.Reset();
	      lpGifInfo->bDibReady = FALSE;
   	}
      else if(timer.PassedTime() >= 10 * lpAniGifPic->delay)
	   {
   	   switch(lpAniGifPic->docase)
      	{
            case 0:                //保持图象，记录为不变图象
            case 1:
              	if(ani_Bits)
	              	DrawLocalPic(rdib,ani_Bits,NULL,lpAniGifPic);
               break;
         	case 2:                //恢复成背景色
           		rdib->FillRect(lpGifInfo->bgC,lpAniGifPic->x,lpAniGifPic->y,lpAniGifPic->width,lpAniGifPic->height);
					break;
   	      case 3:						//恢复前一图象
               if(ani_Bits)
                  DrawLocalPic(rdib,rdib->Data(),ani_Bits,lpAniGifPic);
               break;
      	   default:
              	break;
	      }
   	   if(lpAniGifPic->next)
      	{
          	lpAniGifPic = lpAniGifPic->next;
            lpGifInfo->bDibReady = TRUE;
	         timer.Reset();   	      
      	}
	      timer.Count();
   	}
      Sleep(1);
   }
   Del(ani_Bits);
   return 100;
}

/////////////////////////
//
//将bits图象绘制到dest,bits=NULL,则源图象为动画图象中当前图象
//否则，dest或bits为两相同大小图象，其中之一为记录不变图象(docase=0 or 1)
//
/////////////////////////
void DrawLocalPic(RingDIB *rdib,COLORREF* dest,COLORREF* bits,LPANIGIFPIC lpAniGifPic)
{
	int i,j,y;
   COLORREF *lpmb,*lpl,*lpmbb,*lplb;

   //求剪裁矩形
  	y = lpAniGifPic->y + lpAniGifPic->height;

   lpmb = lpmbb = dest + (rdib->Height() - y) * rdib->Width() + lpAniGifPic->x;

   if(bits == NULL)
   {
      lpl = lplb = lpAniGifPic->curr;

      for(j=0;j<lpAniGifPic->height;j++)
	   {
			for(i=0;i<lpAniGifPic->width;i++)
			{
        		if(lpAniGifPic->keycolor==0xFFFFFFFF || *lpl != lpAniGifPic->keycolor)
					*lpmb = *lpl;
	     		lpmb++;lpl++;
   	   }
			lplb += lpAniGifPic->width;
			lpmbb += rdib->Width();
	  		lpl = lplb;
         lpmb = lpmbb;
  		}
   }
   else
   {
   	lpl = lplb = bits + (rdib->Height() - y) * rdib->Width() + lpAniGifPic->x;
      for(j=0;j<lpAniGifPic->height;j++)
		{
      	for(i=0;i<lpAniGifPic->width;i++)
         {
				*lpmb++ = *lpl++;	     		
         }
         lplb += rdib->Width();
			lpmbb += rdib->Width();
	  		lpl = lplb;
         lpmb = lpmbb;
      }
   }
}

void NoneFilter()
{
}


