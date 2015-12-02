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
//原文件名：pcx.cpp
//
//说明：PCX图象载入功能的实现。（单色及16色图象的解码尚未实现）
//
**********************************************************************/
#define MAKE_SELF_LIB
#include "ringdib.h"

#pragma pack(push,1)
typedef struct tagPCXHEAD
{
	char manufacture;	//always 0x0A
   char version;		//version number
   char encoding;		//should be 1
   char bits_per_pixel;	//color depth
   short xmin,ymin;	//图象左上角坐标
   short xmax,ymax;	//图象宽度=(pcxHead.xmax-pcxHead.xmin)+1;高度同理
   short hres,vres;	//设备分辨率，可忽略
   char palette[48];	//16色下调色板
   char reserved;
   char color_planes;	//颜色面。EGA 16:4,else ,include 单色,=1
   short bytes_per_line;	//扫描线字节数，必须偶数，且!=xmax-xmin
   short palette_type;	//灰度图象为1，全彩:2 Paintbrush后面版本忽略
   short hscreensize;   //为Paintbrush IV/IV+而定义
   short vscreensize;	//其余版本应为NULL
   char filler[54];		//null
}PCXHEAD,*LPPCXHEADER;
#pragma pack(pop)

int xWidth,xHeight;
DWORD dwDataSize;
LPBYTE lpData;
BOOL bOK=FALSE;

int Uncode1(LPBYTE lpData,LPBYTE lpDest,PCXHEAD* ph);
int Uncode16(LPBYTE lpData,LPBYTE lpDest,PCXHEAD* ph);
int Uncode24(LPBYTE lpData,LPBYTE lpDest,PCXHEAD* ph);
int Uncode256(LPBYTE lpData,LPBYTE lpDest,PCXHEAD* ph);

BOOL LoadPCX(LPCTSTR szFilename,RingDIB* lprd,int iFlag,LPSTR lpszFormat)
{
	if(iFlag == DIB_GETFORMAT)
	{
		if(lpszFormat)
		{
			wsprintf(lpszFormat,"pcx");
			return TRUE;
		}
		else
			return FALSE;
	}
   PCXHEAD pcxHead;
   ringFile rf(szFilename);

   rf.Open();
   if(!rf.Read(&pcxHead,sizeof(PCXHEAD)) || pcxHead.manufacture != 0x0A || 
		pcxHead.version < 0x01 || pcxHead.version > 0x05)
      return FALSE;
	else if(iFlag == DIB_QUERYSUPPORT)
		return TRUE;

   xWidth=pcxHead.xmax-pcxHead.xmin+1;
   xHeight=pcxHead.ymax-pcxHead.ymin+1;

   dwDataSize = rf.Size()-sizeof(PCXHEAD);

   lpData=(LPBYTE)New(dwDataSize);
   if(lpData==NULL)
   {
   	//Errmsg("内存分配错误.");
      rf.Close();
      return FALSE;
   }

   if(!rf.Read(lpData,dwDataSize))
   {
      //Errmsg("错误：不能读取文件");
      lpData=(LPBYTE)Del(lpData);
      rf.Close();
      return FALSE;
   }
   rf.Close();
   switch(pcxHead.version)
   {
   	case 0x00:
      	Errmsg("版本太旧，已不支持.");
         lpData=(LPBYTE)Del(lpData);
         return FALSE;
      case 0x02:
      	bOK=Uncode16(lpData,(LPBYTE)lprd->Data(),&pcxHead);
         lpData=(LPBYTE)Del(lpData);
         return bOK;
      case 0x03:
      	bOK=Uncode1(lpData,(LPBYTE)lprd->Data(),&pcxHead);
         lpData=(LPBYTE)Del(lpData);
         return bOK;
      case 0x04:
      case 0x05:
      	if(!lprd->Create(xWidth,xHeight))
         {
         	//Errmsg("内存分配错误.");
		      return FALSE;
         }

      	if(pcxHead.color_planes==3)
         	bOK=Uncode24(lpData,(LPBYTE)lprd->Data(),&pcxHead);
         else if(pcxHead.color_planes==1)
         	bOK=Uncode256(lpData,(LPBYTE)lprd->Data(),&pcxHead);
         lpData=(LPBYTE)Del(lpData);
         return bOK;
      default:
         //Errmsg("版本错误！");
         lpData=(LPBYTE)Del(lpData);
         return FALSE;
   }
}

#if defined  __BORLANDC__
#pragma argsused
#endif
int Uncode1(LPBYTE lpData,LPBYTE lpDest,PCXHEAD* ph)
{
	Errmsg("单色解码尚不支持");
   return TRUE;
}

#if defined  __BORLANDC__
#pragma argsused
#endif
int Uncode16(LPBYTE lpData,LPBYTE lpDest,PCXHEAD* ph)
{
	Errmsg("16色解码尚不支持");
   return TRUE;
}

int Uncode24(LPBYTE lpData,LPBYTE lpDest,PCXHEAD* ph)
{
	DWORD Index,dwScan;//fRes
   int i,j,k,l,count;

   SetCursor(LoadCursor(NULL,IDC_WAIT));

	//////////uncode date//////////
   i=j=k=Index=0;

   dwScan=(xHeight-1)*xWidth*4;
   int len=ph->bytes_per_line*4;

   while(j<xHeight)
   {
      for(l=2;l>=0;l--)
      {
         while(i<len)
         {
            if((lpData[Index]&0xC0)==0xC0)
   		   {
      				count=lpData[Index]&0x3F;
        				Index++;

            	for(k=0;k<count;k++)
	   	     	{
   	   	     	lpDest[dwScan+i+l]=lpData[Index];
      	   	  	i+=4;
	      	     	if(i>=len)
                  {
                  	Index--;
                     lpData[Index]-=(BYTE)(k+1);
                     Index--;
                     break;
                  }
		   	   }
   		   }
	   		else
	      	{
   	      	lpDest[dwScan+i+l]=lpData[Index];
	      	   i+=4;
		      }
            Index++;
            if(Index>=dwDataSize)
            	return FALSE;
         }
         i=0;
      }
      dwScan-=xWidth*4;
      i=0;
      j++;
   }

	return TRUE;
}

int Uncode256(LPBYTE lpData,LPBYTE lpDest,PCXHEAD* ph)
{
   DWORD fSize,Index,dwScan;//fRes
   LPBYTE lpPal;

   int i,j,k,count;

	fSize=dwDataSize;

   if(fSize<769)
   {
   	Errmsg("该文件不是256色图象文件.--无足够调色板信息");
      return FALSE;
   }

   if(lpData[fSize-769]!=0x000C)
   {
   	Errmsg("该文件不是256色图象文件.--错误的调色板信息");
      return FALSE;
   }

   SetCursor(LoadCursor(NULL,IDC_WAIT));

   lpPal=lpData+fSize-768;

   //////////uncode date//////////
   i=j=k=Index=0;
   dwScan=(xHeight-1)*xWidth*4;
   int len=ph->bytes_per_line*4,s;

   while(j<xHeight)
   {
      while(i<len)
      {
	      if((lpData[Index]&0xC0)==0xC0)
  		   {
     			count=lpData[Index]&0x3F;
     			Index++;

            s=lpData[Index]*3+2;
           	for(k=0;k<count;k++)
   	     	{
     	   	  	lpDest[dwScan+i]=lpPal[s];
               i++;s--;
     	   	  	lpDest[dwScan+i]=lpPal[s];
               i++;s--;
     	   	  	lpDest[dwScan+i]=lpPal[s];
               i++;s+=2;
               lpDest[dwScan+i]=0;
               i++;
     	   	  	if(i>=len)
     	   	  		k=count;
	   	   }
  		   }
   		else
      	{
            s=lpData[Index]*3+2;
  	      	lpDest[dwScan+i]=lpPal[s];
            i++;s--;
     	   	lpDest[dwScan+i]=lpPal[s];
            i++;s--;
     	   	lpDest[dwScan+i]=lpPal[s];
            i++;
            lpDest[dwScan+i]=0;
     	   	i++;
	      }
  		   Index++;
         if(Index>=dwDataSize)
           	return FALSE;
      }
      dwScan-=xWidth*4;
      i=0;
      j++;
   }
   return TRUE;
}


