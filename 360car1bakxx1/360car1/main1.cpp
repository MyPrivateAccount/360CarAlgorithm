#if 0
#include <iostream>
#include <opencv2/opencv.hpp>
#include <vector>
using namespace std;
int main()
{
	cv::Mat src = cv::imread("D://program//car360BQL//car360BQL//capture00.bmp", 0);
	//cv::imshow("fff" , src );
	//cv::waitKey(0);
	cv::Size size(9 , 6 );
	vector<cv::Point2f> corners;
	bool b = cv::findChessboardCorners(src, size, corners);
	return 0;
}
#endif