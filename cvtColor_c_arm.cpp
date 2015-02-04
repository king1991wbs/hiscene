

void cvtRGB2Gray( const Mat & src, Mat & dst ){
	assert( src.channels() == 3 && src.type == CV_8UC3 );
	if( dst.empty() )
		dst.create( src.rows, src.cols, CV_8UC1 );
	uchar * psrcData, pdstData;
	int b, g, r;
	for( int i=0 ; i<src.rows ; i++ ){
		psrcData = src.ptr<uchar>(i);
		pdstData = dst.ptr<uchar>(i);
		for( int j=0 ; j<src.cols ; j++ ){
			b = (int)(*psrcData++);
			g = (int)(*psrcData++);
			r = (int)(*psrcData++);
			int y = r*77 + g*151 + b*28;
			*pdstData++ = (y>>8);
		}
	}
}

void cvtRGB2Gray_neon( const Mat & src, Mat & dst ){
	assert( src.channels() == 3 && src.type == CV_8UC3 );
	if( dst.empty() )
		dst.create( src.rows, src.cols, CV_8UC1 );
	uint8_t * psrc = (uint8_t*)src.data;
	uint8_t * pdst = (uint8_t*)dst.data;
	neon_convert( psrc, pdst, src.total() );
}

void neon_convert (uint8_t * __restrict dest, uint8_t * __restrict src, int n)
{
  int i;
  uint8x8_t rfac = vdup_n_u8 (77);
  uint8x8_t gfac = vdup_n_u8 (151);
  uint8x8_t bfac = vdup_n_u8 (28);
  n/=8;

  for (i=0; i<n; i++)
  {
    uint16x8_t  temp;
    uint8x8x3_t rgb  = vld3_u8 (src);
    uint8x8_t result;

    temp = vmull_u8 (rgb.val[0],      rfac);
    temp = vmlal_u8 (temp,rgb.val[1], gfac);
    temp = vmlal_u8 (temp,rgb.val[2], bfac);

    result = vshrn_n_u16 (temp, 8);
    vst1_u8 (dest, result);
    src  += 8*3;
    dest += 8;
  }
}