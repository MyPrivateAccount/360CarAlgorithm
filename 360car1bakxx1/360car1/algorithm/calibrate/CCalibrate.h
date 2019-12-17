#ifndef CALIBRATE_CPP
#define CALIBRATE_CPP

#include "../common.h"
#include "CalibrateFisheye.h"

// 标定，以镜头为单位。一个镜头标定出来一个内参和畸变系数和外参
class CCalibrate
{
protected:
	int        m_iLocation;    // 当前标定的摄像头在哪个位置。0上，1下，2左，3右

	// 以横轴（世界坐标系宽）为X，纵轴（世界坐标系高）为Y
	vector<vector<cv::Point3f> >  m_Points_World;       // 世界坐标系中摄像头的世界坐标

	// 角点在图像坐标系上的坐标(位置存储是相对应的)
	vector<cv::Point2f>  m_Points_Img;

	// 本鱼眼图像上两个正方形共八个顶点在图像坐标系中的坐标。上下棋盘的正方形是角点是先左后右。左右棋盘的正方形的角点是先上后下
	vector<vector<cv::Point2f> > m_Rect_Img;

	// 角点维度
	cv::Size     m_cornerDimension;

	// 进行标定的变量
	CCalibrateFisheye m_onBrate;

	//////////////////////////////////////////////////////////////////////////
	// 角点y的最大值和最小值
	float yMax;
	float yMin;

	// 角点x的最大值和最小值
	float xMax;
	float xMin;

	calibrateScheme  m_CalibrateScheme;
public:
	fishEyeInf m_fishInf;      // 当前标定出来的镜头参数

public:
	CCalibrate();

	void SetCalibrate( calibrateScheme calibrates );

	//////////////////////////////////////////////////////////////////////////
	// 设置角点的世界坐标系
	// iBeginX是维度的起点X坐标
	// iBeginY是维度的起点Y坐标
	// iCheckBoardSize是小的棋盘格的大小
	// dimension是棋盘格的维度
	// bClear表示当前世界坐标系的变量是否会被清空。false表示不被清空，true表示要被清空
	void OnSetCornerWorldLocation( int iBeginX , int iBeginY , CvSize dimension , int iCheckBoardSize , int iCheckBoardSize_height , int iLocation , bool bClear );

	//////////////////////////////////////////////////////////////////////////
	// 标定
	// pMat表示当前需要标定哪幅图像
	// pRect表示在整张棋盘上哪两个正方形的世界坐标系与此次标定图像相对应
	bool OnCalibrateX(cv::Mat pMat, vector<vector<cv::Point3f> > pRect, int&iCalibrateProcess);
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	// 内参和畸变系数只有一个，外参不能确定
	// 获取当前鱼眼镜头内参
	cv::Matx34d*OnGetInnerParam();

	// 获取当前鱼眼镜头的畸变系数
	cv::Matx14d*OnGetDistortionParam();
	//////////////////////////////////////////////////////////////////////////

	// 获取外参,iCount表示获取的外参的个数
	cv::Matx44d OnGetRTParam(int iCount = 0);

	fishEyeInf OnGetFishEyeInf();

	// 获取图像坐标系定点坐标
	vector<vector<cv::Point2f> > OnGetRect_Img();

	~CCalibrate();

private:
	void TopFishEye();
	void BottomFishEye();
	void LeftFisheye();
	void RightFisheye();

	// 从起点开始，遍历二值化图像的轮廓
	bool ergodicBinaryMat(cv::Point BeginPoint, cv::Mat BinaryMat, vector<cv::Point>&edgaPoint);

	// 判断是否是二值化得边缘点；是为true，否为false
	bool CheckBinaryPoint(cv::Point PointLocation, cv::Mat BinaryMat);

	// 查找当前点是否超出图像范围,超出的部分进行处理
	void editPointRange(cv::Point&location, int rows, int cols);

	// 在二值化图像上查找4个顶点
	void GetRectPosFromBinPic(cv::Mat &ThreshMat, cv::Mat &binMatBGR, cv::Mat &erodeMat, vector<cv::Point> &Contour, vector<cv::Point> &roi_point_approx);

	// 在大正方形轮廓图上查找4个顶点
	void GetRectPosFromContourPic(cv::Mat &ThreshMat, vector<cv::Point> &Contour, bool bRight, vector<cv::Point> &roi_point_approx);

	// 棋盘格的角点在图像中的位置进行分析
	// 查看原点的位置是在左上？右上？左下还是右下？
	// CheckLen表示的是遍历的cornerLocation的长度和范围
	void OriginLocationAndErgodicMode(vector<cv::Point2f> cornerLocation, int CheckLen, OriginInImage&Location, ArrayModeInImage&ArrayMode);


	// 对车辆前面的图像进行重新排序,数据源在source中,并将排序后的结果存入到dst中
	// cornerSize是角点的维度
	void OnReSort_Top(OriginInImage originLocation, ArrayModeInImage arrayMode, vector<cv::Point2f>&source, CvSize cornerSize, vector<cv::Point2f>&dst);           // 前
	void OnResort_Bottom(OriginInImage originLocation, ArrayModeInImage arrayMode, vector<cv::Point2f>&source, CvSize cornerSize, vector<cv::Point2f>&dst);        // 后
	void OnResort_Left(OriginInImage originLocation, ArrayModeInImage arrayMode, vector<cv::Point2f>&source, CvSize cornerSize, vector<cv::Point2f>&dst);          // 左
	void OnResort_Right(OriginInImage originLocation, ArrayModeInImage arrayMode, vector<cv::Point2f>&source, CvSize cornerSize, vector<cv::Point2f>&dst);         // 右

	// 获取此鱼眼镜头大正方形在鱼眼图上的坐标
	// pMat是当前鱼眼镜头拍摄的图像
	// iLocation是当前鱼眼镜头在车上所在的位置
	// pRect是将要返回的正方形在图像坐标系上的坐标点
	bool OnGetRectPosition(const cv::Mat pMat, const int iLocation, vector<vector<cv::Point2f> >&pRect);

	// 进行标定
	void OnCalibrate_private(const vector<vector<cv::Point3f> >&position_World, const vector<vector<cv::Point3f> >&Rect_World, const vector<cv::Point2f>&position_Img, const vector<vector<cv::Point2f> >&Rect_Img, CvSize imgSize, int&iProcess);


	// 标定内参
	// innerParam是计算出来的内部参数
	bool OnCalibrateInnerParam(vector<vector<cv::Point3f> >&worldPoint, vector<vector<cv::Point2f> >&imgPoint, cv::Matx34d&innerParam/*Matx33d&innerParam*/, cv::Matx14d&distor, cv::Size image_size);


	//////////////////////////////////////////////////////////////////////////
	// 自动门限
	void AdaptiveFindThreshold(const cv::Mat src, double *low, double *high, int aperture_size = 3);

	// 仿照matlab，自适应求高低两个门限                                              
	void _AdaptiveFindThreshold(CvMat *dx, CvMat *dy, double *low, double *high);

	// 从一堆的封闭区域中检测出满足要求的正方形
	// source为在一幅图像中检测的所有的满足封闭要求的点集合
	// dst为从source中找到的正方形的顶点
	// imageWide为原图像的宽度
	// imageHeight为原图像的高度
	// iLocation表示当前镜头在车的哪个位置（0上，1下，2左，3右）
	bool OnFindRectPointInImage(vector<vector<cv::Point> >&source, vector<vector<cv::Point2f> >&dst, int imageWide, int imageHeight, int iLocation);


	// 检查当前正方形是否满足要求
	bool CheckRect(vector<cv::Point>&RectPoint, int imageWide, vector<vector<cv::Point> >&leftCorner, vector<vector<cv::Point> >&rightCorner);

	// 检测正方形
	// 正方形一定是在source中的中一个
	// bLeft为true时，表示source是左边的点，否则为右边的点
	void OnCheckRect(vector<vector<cv::Point> >&source, vector<vector<cv::Point> >&cornerLocation, bool bLeft);

	// 查找角点中y值最大和最小的值
	void onFindMaxYAndMinY(vector<cv::Point2f> corners);

	// 根据找出的四个顶点，进一步确定每幅图像的四个顶点，共8个顶点
	// leftPoint表示图像左边的那个正方形四个顶点和其他的点
	// rightPoint表示图像右边的正方形四个顶点集合和其他的点
	// dst存储的是正方形的左右八个顶点，分别为   左：左上、右上、左下、右下    右：左上、右上、左下、右下
	void OnDetermineRectInImg(vector<vector<cv::Point> >&leftPoint, vector<vector<cv::Point> >&rightPoint, vector<vector<cv::Point2f> >&dst);

	//////////////////////////////////////////////////////////////////////////
	// 检查当前点是否在棋盘的角点处
	// 如果PointLocation在Points_Img处，那么就返回false，否则返回true
	bool bCheckRectOnCorner(vector<cv::Point2f>  Points_Img, vector<cv::Point> PointLocation);

	// 对m_Points_square_Img中的数据进行重新排列
	// pTemp表示的是传递进来的每幅图像的共8个顶点（重合区域）
	// iSindex表示的是当前的点是在车的哪个位置上
	void OnReSort_Square_Img(vector<vector<cv::Point2f> >&pTemp, int iSindex);


	// 亚像素化
	void RectImgSubPix(cv::Mat grayImage, vector<vector<cv::Point2f> >&rect_Point);

	// 返回为距离最小的下标值
	// pPoint为一堆的点，fThreath为这些点到此值的最小值
	int iGetMinDis(vector<vector<cv::Point> >&pPoint, float fThreath);

	//////////////////////////////////////////////////////////////////////////
	// 求距离最小值
	int iGetMinDis2f(vector<vector<cv::Point2f> >&pPoint, float fThreath);

	//////////////////////////////////////////////////////////////////////////
	// 求一定距离范围内的均值
	void iGetAverage(vector<vector<cv::Point2f> >&pPoint, int iMinIndex, vector<cv::Point2f>&average);
};

#endif