#include <iostream>

#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

void flipVert_8u(const Mat & src, Mat & dst){
	assert(src.type() == CV_8UC3 || src.type() == CV_8UC1);

	if(dst.empty()){
		dst.create(src.rows, src.cols, dst.type());
	}

	if(src.channels() == 3){
		for(int i = 0; i < src.rows; ++i)
			for(int j = 0; j < src.cols; ++j){
				//dst[i][src.cols - j] = src[i][j];
				dst.at<Vec3b>(i, src.cols - j)[0] = src.at<Vec3b>(i, j)[0];
				dst.at<Vec3b>(i, src.cols - j)[1] = src.at<Vec3b>(i, j)[1];
				dst.at<Vec3b>(i, src.cols - j)[2] = src.at<Vec3b>(i, j)[2];
			}
			return;
	}
	//single chanel
	for(int i = 0; i < src.rows; ++i)
		for(int j = 0; j < src.cols; ++j){
			//dst[i][src.cols - j] = src[i][j];
			dst.at<uchar>(i, src.cols - j) = src.at<uchar>(i, j);
		}
}

void flipHoriz_8u(const Mat & src, Mat & dst){
	assert(src.type() == CV_8UC3 || src.type() == CV_8UC1);

	if(dst.empty()){
		dst.create(src.rows, src.cols, dst.type());
	}
	
	if(src.channels() == 3){
		for(int i = 0; i < src.rows; ++i)
			for(int j = 0; j < src.cols; ++j){
				//dst[src.rows - i][j] = src[i][j];
				dst.at<Vec3b>(src.rows - i, j)[0] = src.at<Vec3b>(i, j)[0];
				dst.at<Vec3b>(src.rows - i, j)[1] = src.at<Vec3b>(i, j)[1];
				dst.at<Vec3b>(src.rows - i, j)[2] = src.at<Vec3b>(i, j)[2];
			}
	}
	//single chanel
	for(int i = 0; i < src.rows; ++i)
		for(int j = 0; j < src.cols; ++j){
			//dst[src.rows - i][j] = src[i][j];
			dst.at<uchar>(src.rows - i, j) = src.at<uchar>(i, j);
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