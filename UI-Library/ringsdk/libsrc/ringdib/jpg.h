#ifdef __cplusplus
extern "C" {
#endif
extern BYTE * JpegFileToRGB(LPSTR fileName,UINT *width,UINT *height);
extern BOOL GetJPGSize(LPSTR fileName,UINT *width,UINT *height);
extern BOOL BGRFromRGB(LPBYTE dest,BYTE *src, UINT widthPix, UINT height);
#ifdef __cplusplus
}
#endif

