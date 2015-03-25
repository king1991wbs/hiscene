#include <iostream>

#include <highgui.h>
#include <cxcore.h> 
#include "opencv2/imgproc/imgproc.hpp"

#include "RotRectAngle.h"

using namespace std;
using namespace cv;

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
				return CV_PI/2;
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
	int angSign =  abDotunit > 0 ? -1 : 1;//angle sign we need
	double costheta = abs( abDotunit / magAB );
	angle = angSign * acos( costheta );

	return angle;
}

void getRotatedImageROI( const Mat & src, const RotatedRect & rRect, Mat & cropped  ){
	// matrices we'll use
	Mat M, rotated;
	// get angle and size from the bounding box
	double angle = rotRectAngle( rRect );
	
	// get the rotation matrix
	M = getRotationMatrix2D(rRect.center, angle, 1.0);

	warpAffine(src, rotated, M, src.size(), INTER_CUBIC);	

	// crop the resulting image
	getRectSubPix(rotated, rRect.size, rRect.center, cropped);
}


int main(){
	Mat image(200, 200, CV_8UC1, Scalar(0));

	Point2f center(100,100);
	int angle = 30;
	RotatedRect rRect = RotatedRect(center, Size2f(100,50), angle);

	Point2f vertices[4];
	rRect.points(vertices);
	for (int i = 0; i < 4; i++)
	    line(image, vertices[i], vertices[(i+1)%4], Scalar(255));
	
	Mat roi(rRect.size, image.type(), Scalar(0));

	getRotatedImageROI(image, rRect, roi);
	namedWindow( "roi", CV_WINDOW_AUTOSIZE );
	imshow("roi", roi);
	waitKey(0);

	return 0;
}