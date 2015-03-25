#ifndef _ROT_RECT_ANGLE
#define _ROT_RECT_ANGLE

#include <iostream>
#include <cmath>

#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"

#define precision 0.0001
//#define PI 3.14159

using namespace std;
using namespace cv;

double rotRectAngle( const RotatedRect &rRect );
#endif