#include <iostream>

#include <cv.h>
#include <highgui.h>

using namespace std;
using namespace cv;

bool isPointInRotRect(const RotatedRect &rRect, const Point2f &pt){
	Point2f vertices[4];
	rRect.points(vertices);

	Point2f vecAB(vertices[1].x - vertices[0].x, vertices[1].y - vertices[0].y);
	Point2f vecAD(vertices[3].x - vertices[0].x, vertices[3].y - vertices[0].y);
	Point2f vecAP(pt.x - vertices[0].x, pt.y - vertices[0].y);

	double tem1 = vecAB.x*vecAP.x + vecAB.y*vecAP.y;
	double tem2 = vecAD.x*vecAP.x + vecAD.y*vecAP.y;
	double ABsquare = vecAB.x*vecAB.x + vecAB.y*vecAB.y;
	double ADsquare = vecAD.x*vecAD.x + vecAD.y*vecAD.y;
	if(tem1 > 0 && tem1 < ABsquare && tem2 > 0 && tem2 < ADsquare)
		return true;

	return false;
}

int main(){
	Mat image(200, 200, CV_8UC3, Scalar(0));
	RotatedRect rRect = RotatedRect(Point2f(100,100), Size2f(100,50), 60);

	Point2f pt(100,100);
	if(isPointInRotRect(rRect,pt))
		cout << "point in rectangle!" << endl;
	else
		cout << "point not in rectangle!" << endl;

	Point2f vertices[4];
	rRect.points(vertices);
	for (int i = 0; i < 4; i++)
	    line(image, vertices[i], vertices[(i+1)%4], Scalar(0,255,0));



	line(image, pt, pt, Scalar(0,255,0));

	Rect brect = rRect.boundingRect();
	rectangle(image, brect, Scalar(255,0,0));

	imshow("rectangles", image);
	waitKey(0);

	return 0;
}