#include <iostream>

#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

void flipVert_8u(const Mat & src, Mat & dst){
	assert(src.type() == CV_8UC3 || src.type() == CV_8UC1);

	if(dst.empty()){
		dst.create(src.rows, src.cols, dst.type());
	}

	const uchar *psrcData;
	uchar       *pdstData;
	if(src.channels() == 3){
		for(int i = 0; i < src.rows; ++i){
			psrcData = src.ptr<uchar>(i);
			pdstData = dst.ptr<uchar>(i);
			for(int j = 0; j < src.cols; ++j){
				*( pdstData + (src.cols-j)*3 + 0 ) = *( psrcData + j*3 + 0 );
				*( pdstData + (src.cols-j)*3 + 1 ) = *( psrcData + j*3 + 1 );
				*( pdstData + (src.cols-j)*3 + 2 ) = *( psrcData + j*3 + 2 );
			}
		}
			return;
	}
	//single chanel
	for(int i = 0; i < src.rows; ++i){
		psrcData = src.ptr<uchar>(i);
		pdstData = dst.ptr<uchar>(i);
		for(int j = 0; j < src.cols; ++j){
			*( pdstData + (src.cols-j) ) = *( psrcData + j );
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
	if(src.channels() == 3){
		for(int i = 0; i < src.rows; ++i){
			psrcData = src.ptr<uchar>(i);
			pdstData = dst.ptr<uchar>(src.rows - i);

			for(int j = 0; j < src.cols; ++j){
				*pdstData++ = *psrcData++;
				*pdstData++ = *psrcData++;
				*pdstData++ = *psrcData++;
			}
		}
		return;
	}
	//single chanel
	for(int i = 0; i < src.rows; ++i){
		psrcData = src.ptr<uchar>(i);;
		pdstData = dst.ptr<uchar>(src.rows - i);
		for(int j = 0; j < src.cols; ++j){
			*pdstData++ = *psrcData++;
		}
	}
}


int main(int argc, char **argv){
	if(argc != 2)
		cout << "please input image path:" << endl;

	Mat src = imread(argv[1], 1);
	Mat dst1(src.size(),CV_8UC3);
	Mat dst2(src.size(),CV_8UC3);

	flipHoriz_8u(src, dst1);
	flipVert_8u(src, dst2);

	namedWindow("horiz",CV_WINDOW_AUTOSIZE);
	imshow("horiz", dst1);
	namedWindow("verti",CV_WINDOW_AUTOSIZE);
	imshow("verti", dst2);
	waitKey();

	return 0;
}