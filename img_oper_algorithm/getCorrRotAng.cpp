#include <iostream>
#include <cmath>
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"

#define precision 0.1

using namespace std;
using namespace cv;


const double THREDIFF = 20*CV_PI/180;

double rotRectAngle( const RotatedRect &rRect )
{
	double angle = 0.0;

	Point2f pts[4];
	rRect.points(pts);

	//Point2f lEdgeA, LEdgeB;
	int lPosA = 0, lPosB;
	for( int i = 1; i < 4; ++i )
	{
		//rect if up-right
		if( abs( pts[lPosA].x - pts[i].x ) < precision )
		{
			if( ( rRect.size.height - rRect.size.width ) > precision )////??????
				return 0;
			else
				return CV_PI/2;///+-
		}
		//find the most left point in rect
		if( pts[lPosA].x > pts[i].x )
			lPosA = i;
	}
	
	double length = ( pts[lPosA].x - pts[( lPosA + 1 ) % 4].x )*( pts[lPosA].x - pts[( lPosA + 1 ) % 4].x ) + 
					( pts[lPosA].y - pts[( lPosA + 1 ) % 4].y )*( pts[lPosA].y - pts[( lPosA + 1 ) % 4].y );
	double height = rRect.size.height > rRect.size.width ? rRect.size.height : rRect.size.width;
	if( abs( length - height*height ) < precision )
		lPosB = ( lPosA + 1 ) % 4;
	else if( lPosA == 0 )
	{
		lPosB = 3;
	}else
		lPosB = lPosA - 1;

	Point2f vecAB( pts[lPosB].x - pts[lPosA].x , pts[lPosB].y - pts[lPosA].y);
	Point2f vecUnitY( 0 , 1 );

	double abDotunit = vecAB.ddot(vecUnitY);
	double magAB = sqrt( vecAB.x*vecAB.x + vecAB.y*vecAB.y );
	int angSign =  abDotunit > 0 ? 1 : -1;//angle sign we need
	double costheta = abs( abDotunit / magAB );
	angle = angSign * acos( costheta );

	return angle;
}


void getCorrRotAng( const RotatedRect & preRect, RotatedRect & nextRect )
{
	double angle;
	double preAng = rotRectAngle(preRect);
	double nextAng = rotRectAngle(nextRect);

	double diff = abs( preAng - nextAng );
	if(diff > THREDIFF)
		nextRect.angle = ( -nextAng / abs(nextAng) )*( CV_PI - abs(nextAng) );
	else
		nextRect.angle = nextAng;
}

int main()
{
	RotatedRect p(Point2f(100, 50), Size(50, 100), 65);
	RotatedRect n(Point2f(100, 50), Size(50, 100), 75);

	Mat image(200, 200, CV_8UC3, Scalar(0));

	Point2f vertices[4];
	p.points(vertices);
	for (int i = 0; i < 4; i++)
	    line(image, vertices[i], vertices[(i+1)%4], Scalar(0,255,0));

	getCorrRotAng(p, n);
	cout << n.angle/CV_PI*180 << endl;

	imshow("p", image);
	waitKey(0);
	return 0;
}