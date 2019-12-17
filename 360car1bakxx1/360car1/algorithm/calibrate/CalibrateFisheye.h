#ifndef CALIBRATE_FISH_EYE
#define CALIBRATE_FISH_EYE

#include "../common.h"

// 内参数据结构
struct IntrinsicParams
{
	cv::Vec2d f;             // 表示图像的fx和fy
	cv::Vec2d c;             // 表示图像的中心点
	cv::Vec4d k;             // 表示畸变系数
	double alpha;
	std::vector<int> isEstimate;

	IntrinsicParams();
	IntrinsicParams(cv::Vec2d f, cv::Vec2d c, cv::Vec4d k, double alpha = 0);
	IntrinsicParams operator+(const cv::Mat& a);
	IntrinsicParams& operator =(const cv::Mat& a);
	void Init(const cv::Vec2d& f, const cv::Vec2d& c, const cv::Vec4d& k = cv::Vec4d(0,0,0,0), const double& alpha = 0);
};

struct JacobianRow
{
	cv::Vec2d df, dc;
	cv::Vec4d dk;
	cv::Vec3d dom, dT;
	double dalpha;
};

class CCalibrateFisheye
{
public:
	CCalibrateFisheye();

	// 标定外参
	bool OnCalibrateExtrinsics( cv::InputArrayOfArrays objectPoints, cv::InputArrayOfArrays imagePoints , const IntrinsicParams& param , const int check_cond , const double thresh_cond, cv::InputOutputArray omc, cv::InputOutputArray Tc );

	// 找角点
	// imageIndex值0：上
	// 1:下
	// 2:左
	// 3:右
	// Size m_board_size;              // 定标板上的每行、列的角点数
	// cornersPoint        定标板上每个角点的位置
	bool getChessboardCorners(  cv::Mat mat , int imageIndex , cv::Size m_board_size ,vector<cv::Point2f>&cornersPoint );

	// 优化外参
	bool OnOptimizeExtrinsics(  cv::InputArrayOfArrays objectPoints, cv::InputArrayOfArrays imagePoints , const IntrinsicParams& param , const int check_cond , const double thresh_cond, cv::InputOutputArray omc, cv::InputOutputArray Tc );


	~CCalibrateFisheye();

private:




	// 初使化外参
	void InitExtrinsics(const cv::Mat& _imagePoints, const cv::Mat& _objectPoints, const IntrinsicParams& param, cv::Mat& omckk, cv::Mat& Tckk);

	// 
	cv::Mat NormalizePixels(const cv::Mat& imagePoints, const IntrinsicParams& param);

	//
	void undistortPoints(cv::InputArray distorted, cv::OutputArray undistorted, cv::InputArray K, cv::InputArray D, cv::InputArray R = cv::noArray(), cv::InputArray P = cv::noArray());

	// 单应性变换
	cv::Mat ComputeHomography(cv::Mat m, cv::Mat M);

	// 
	void ComputeExtrinsicRefine(const cv::Mat& imagePoints, const cv::Mat& objectPoints, cv::Mat& rvec, cv::Mat&  tvec, cv::Mat& J, const int MaxIter, const IntrinsicParams& param, const double thresh_cond);

	//
	void projectPoints(cv::InputArray objectPoints, cv::OutputArray imagePoints,cv::InputArray _rvec,cv::InputArray _tvec,const IntrinsicParams& param, cv::OutputArray jacobian);

	void projectPoints_fishEye(cv::InputArray objectPoints, cv::OutputArray imagePoints, cv::InputArray _rvec, cv::InputArray _tvec, cv::InputArray _K, cv::InputArray _D, double alpha, cv::OutputArray jacobian);
};
#endif