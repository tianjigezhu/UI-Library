
/*
////////////////////////////////////////////////////////////////
//	Reading Usage :

	UINT height;
	UINT width;
	BYTE *dataBuf;
   //read the file
   dataBuf=JpegFile::JpegFileToRGB(fileName,
								&width,
								&height);
	if (dataBuf==NULL) {
		return;
	}

	// RGB -> BGR
	JpegFile::BGRFromRGB(dataBuf, m_width, m_height);


	BYTE *buf;
	// create a DWORD aligned buffer from the JpegFile object
	buf = JpegFile::MakeDwordAlignedBuf(dataBuf,
									width,
									height,
									&m_widthDW);

	// flip that buffer
	JpegFile::VertFlipBuf(m_buf, m_widthDW, m_height);

	// you now have a buffer ready to be used as a DIB

	// be sure to delete [] dataBuf;	// !!!!!!!!!!
	//			delete [] buf;


	// Writing Usage

	BYTE *tmp=NULL;

	// assume buf is a DWORD-aligned BGR buffer, vertically flipped
	// as if read from a BMP file.

	// un-DWORD-align
	tmp=JpegFile::RGBFromDWORDAligned(buf,
									widthPix,
									widthBytes,
									height);

	// vertical flip
	JpegFile::VertFlipBuf(tmp, widthPix * 3, height);

	// reverse BGR
	JpegFile::BGRFromRGB(tmp, widthPix, height);

	if (tmp==NULL) {
		AfxMessageBox("~DWORD Memory Error");
		return;
	}

	// write it
	BOOL ok=JpegFile::RGBToJpegFile(fileName,
						tmp,
						width,
						height,
						TRUE,
						75);
	if (!ok) {
		AfxMessageBox("Write Error");
	}

	delete [] tmp;

////////////////////////////////////////////////////////////////

*/

//
//	for DWORD aligning a buffer
//

#define WIDTHBYTES(bits)    (((bits) + 31) / 32 * 4)

#ifdef __cplusplus
extern "C" {
#endif

// write a JPEG file from a 3-component, 1-byte per component buffer          //是否彩色    //压缩比率
extern BOOL RGBToJpegFile(LPSTR fileName,BYTE *dataBuf,UINT width,UINT height,BOOL bRGB,int quality);

	// allocates a DWORD-aligned buffer, copies data buffer
	// caller is responsible for delete []'ing the buffer

extern BYTE * MakeDwordAlignedBuf(BYTE *dataBuf,			// input buf
									 UINT widthPix,				// input pixels
									 UINT height,				// lines
									 UINT *uiOutWidthBytes);	// new width bytes


	////////////////////////////////////////////////////////////////
	// if you have a DWORD aligned buffer, this will copy the
	// RGBs out of it into a new buffer. new width is widthPix * 3 bytes
	// caller is responsible for delete []'ing the buffer

extern BYTE *RGBFromDWORDAligned(BYTE *inBuf,				// input buf
									UINT widthPix,				// input size
									UINT widthBytes,			// input size
									UINT height);

	////////////////////////////////////////////////////////////////
	// vertically flip a buffer - for BMPs
	// in-place

	// note, this routine works on a buffer of width widthBytes: not a
	// buffer of widthPixels.
extern BOOL VertFlipBuf(BYTE * inbuf,						// input buf
					   UINT widthBytes,							// input width bytes
					   UINT height);							// height

#ifdef __cplusplus
}
#endif
