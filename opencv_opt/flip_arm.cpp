void flipVert_8u(const Mat & src, Mat & dst){
	assert(src.type() == CV_8UC3 || src.type() == CV_8UC1);

	if(dst.empty()){
		dst.create(src.rows, src.cols, dst.type());
	}

	const uchar *psrcData;
	uchar       *pdstData;

	//single chanel
	for(int i = 0; i < src.rows; ++i){
		psrcData = src.ptr<uchar>(i);
		pdstData = dst.ptr<uchar>(i) + src.cols;//refer to the next position of last element

		uint8x8_t pixData;
		for(int k = 0; k < src.cols/8; ++k){
			pixData = vld1_u8(psrcData);
			//pixData = vrev64_u8(vld1_u8(psrcData));
			pixData = vrev64_u8(pixData);
			vst1_u8(pdstData-8, pixData);

			psrcData += 8;
			pdstData -= 8;
		}

		for(int j = 0; j < src.cols%8; ++j){
			*( --pdstData ) = *(psrcData++);
		}
	}
}

void flipHoriz_8u(const Mat & src, Mat & dst){
	assert(src.type() == CV_8UC3 || src.type() == CV_8UC1);

	if(dst.empty()){
		dst.create(src.rows, src.cols, dst.type());
	}
	
	const uchar *psrcData;
	uchar       *pdstData;
	
	//single chanel
	for(int i = 0; i < src.rows; ++i){
		psrcData = src.ptr<uchar>(i);
		pdstData = dst.ptr<uchar>(src.rows - i);

		uint8x8_t  pixData;
		for(int k = 0; k < src.cols/8; ++k){
			pixData = vld1_u8(psrcData);
			//pixData = vrev64_u8(vld1_u8(psrcData));
			pixData = vrev64_u8(pixData);
			vst1_u8(pdstData, pixData);

			psrcData += 8;
			pdstData += 8;
		}

		for(int j = 0; j < src.cols%8; ++j){
			*pdstData++ = *psrcData++;
		}
	}
}