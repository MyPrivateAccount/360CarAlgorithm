#ifndef GENERATE_3D_TABLE_H
#define GENERATE_3D_TABLE_H
///////////////////////////////////////
// 生成3D球的表变量

#include "../common.h"
#include "../calibrate/CCalibrate.h"
#include "../iniFile/inirw.h"
// 使用OPENCL生成查找表

#define PI 3.1415926

#define SHOW_3DIMG 0          // 是否显示和存储3D图像

#define PER_HEIGHT 1.3          // 此值调整显示的高度

struct virtualCameraParam                                        // 定义虚拟摄像机参数
{
	double cameraMaxFieldView;                                             // 摄像机最大视场角
	float circleRadius;                                                   // 抽象圆半径（以毫米为单位）
	int longRes;												// 经度分辨率
	int latRes;													// 纬度分辨率
	//int   cameraHeight;                                          
	//Point3f circleCenterLocation;                                // 圆心的位置,在Z轴上，以毫米为单位
};

struct G3DtableParam                                             // 生成此3D表的一需要的参数
{
	// 长度为FISHEYE_COUNT
	CCalibrate* CalibrateParam;                                // 这样的标定参数一共有四个

	//////////////////////////////////////////////////////////////////////////
	CvSize2D32f  world_show_size;							// 在世界坐标系中显示区域的大小（以毫米为单位）
	CvSize2D32f worldcarSize;                                       // 车辆在世界坐标系的尺寸
	float fWorldCarBeginX;
	float fWorldCarBeginY;
	//////////////////////////////////////////////////////////////////////////

#if NEW3D
	//////////////////////////////////////////////////////////////////////////
	// 将重合区域的四个正方形世界坐标系的点和图像坐标系的点记录并传递
	vector<vector<cv::Point3f> >*pRectW;
	vector<vector<cv::Point2f> >*pRectWImg;
	//////////////////////////////////////////////////////////////////////////
#endif


	int nx;
	int ny;

	//////////////////////////////////////////////////////////////////////////
	//CvSize2D32f worldshowSize;                                 // 在世界坐标系中显示的尺寸
	//float fBeginOriginX;
	//float fBeginOriginY;
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	// 拼接缝，长度为FISHEYE_COUNT
	float*m_K;        // 拼接缝的斜率，0，1，2，3分别表示左上拼接缝，右上拼接缝，左下拼接缝，右下拼接缝
	float*m_B;        // 拼接缝b值，同上
	// 融合线，长度为2*FISHEYE_COUNT
	float*m_BMix;   // 融合线，共八条融合线，分成了四个部分，每两个1组。奇数为融合线在拼接线的下方，偶数（包括0）为融合线在拼接线的上方   
	//////////////////////////////////////////////////////////////////////////
};

struct stu3dTable               // 球面的查找表
{
	cv::Point2f iLocation;            // 当前球面图像的坐标值

	short/*cameraLocation*/ img1Idx;     // 第一幅鱼眼图像的ID值。如果此值为-1，那么就代表不会从任何图像上映射值
	short/*cameraLocation*/ img2Idx;     // 第二幅鱼眼图像的ID值。如果此值为-1，那么就代表不会从任何图像上映射值

	cv::Point2f img1Location;         // iLocation对应到鱼眼图像上的点
	float img1Weight;           // 第一幅图像的权重

	
	cv::Point2f img2Location;         // iLocation对应到当前鱼眼图像上的点
	float img2Weight;           // 第二幅图像上的权重
};

// 重合区域写成，比如P_TOP|P_LEFT
enum PointLocation
{ 
	P_NO = 0,
	P_TOP = 1,               // 前区域
	P_BOTTOM = 2 ,            // 后区域
	P_LEFT = 4 ,              // 左区域
	P_RIGHT = 8 ,             // 右区域
	P_CAR = 16                 // 小车区域
};
//  
class CGenerate3Dtable
{
public:
	CGenerate3Dtable();
	~CGenerate3Dtable();

	void OnGenerate3Dtable( G3DtableParam* p3DtableParam );

	void OnGenerateNew3Dtable( G3DtableParam*p3DtableParam );
private:

	// 虚拟摄像机参数
	virtualCameraParam m_vCameraParam;

	// 地面离圆心的高度
	float m_CircleCenter;

	// 定义标定方案，默认为宝沃BX7的标定方案
	calibrateScheme m_CalibrateScheme;

	// 棋盘格区域的大小
	int m_iWide; // 宽度
	int m_iHeight; // 高度

	cv::Matx44d m_RT_3DvalueAverage[4];         // 差均值，只有中间的渐变区域。分别对应着四个棋盘格中间渐变部分。从左渐变到右，从上渐变到下。下标表示上、下、左和右

	//////////////////////////////////////////////////////////////////////////
	// 将图像分区
	int                     m_wide_area[3];        // 下标为0、1、2分别表示为左中右
	int                     m_height_area[3];      // 下标为0、1、2分别表示为上中下
	cv::Matx44d                 m_RT_DvalueAverage[4];         // 差均值，只有中间的渐变区域。分别对应着四个棋盘格中间渐变部分。从左渐变到右，从上渐变到下。下标表示上、下、左和右

	int                     m_iSourceWide;          // 原鱼眼图像的宽度
	int                     m_iSourceHeight;        // 原鱼眼图像的高度

	//////////////////////////////////////////////////////////////////////////
	// 拼接缝的直线斜率和b值
	float                   m_K[FISHEYE_COUNT];        // 拼接缝的斜率，0，1，2，3分别表示左上拼接缝，右上拼接缝，左下拼接缝，右下拼接缝
	float                   m_B[FISHEYE_COUNT];        // 拼接缝b值，同上

	float                   m_BMix[2 * FISHEYE_COUNT];   // 融合线，共八条融合线，分成了四个部分，每两个1组。奇数为融合线在拼接线的下方，偶数（包括0）为融合线在拼接线的上方     

	// 重合区域大小
	int m_doubleAreaSize;

private:
	// 获取棋盘格大小
	bool OnGetCheckBoardSize(struct virtualCameraParam &vCameraParam, int&wide, int&height);

	// 获取点在小车的位置
	int OnGetPointLocation(double xLocation , double yLocation ,  G3DtableParam* p3DtableParam );

	// 设置拼接缝和融合区域
	void OnSetSpliceJoint(CvSize2D32f worldShowSize, float fBeginOriginX, float fBeginOriginY, CvSize2D32f worldCarSize, float fWorldCarBeginX, float fWorldCarBeginY, CCalibrate *pCameraParam);

	// 车区域
	PointLocation OnCheckisInCarRange( double xLocation , double yLocation , CvSize2D32f worldCarSize , float fWorldCarBeginX , float fWorldCarBeginY );

	// 上区域
	PointLocation OnCheckisInCarTop( double xLocation , double yLocation , CvSize2D32f worldCarSize , float fWorldCarBeginX , float fWorldCarBeginY , float* pK , float* pB );

	// 下区域
	PointLocation OnCheckisInCarBottom( double xLocation , double yLocation , CvSize2D32f worldCarSize , float fWorldCarBeginX , float fWorldCarBeginY , float* pK , float* pB );

	// 左区域
	PointLocation OnCheckisInCarLeft( double xLocation , double yLocation , CvSize2D32f worldCarSize , float fWorldCarBeginX , float fWorldCarBeginY , float* pK , float* pB );

	// 右区域
	PointLocation OnCheckisInCarRight( double xLocation , double yLocation , CvSize2D32f worldCarSize , float fWorldCarBeginX , float fWorldCarBeginY , float* pK , float* pB );

	// 从鱼眼图像上映射
	// 从鱼眼图上映射
	void mappingFromFisheye_3D(cv::Mat&WorldLocation, cv::Matx44d*RT, stu3dTable& tableInf, int isrcWide, int isrcHeight, CCalibrate *pCameraParam);

	// 生成每个RT的均值
	void OnGenerateRTAverage( G3DtableParam* p3DtableParam );

	// 获取3D现在的值
	// x为世界坐标系的x值
	// y为世界坐标系的y值
	// pLocation为当前摄像头是在车辆的哪个地方
	// 获取当前的RT值
	void OnGet3DRTvalue(int x, int y, PointLocation pLocation, cv::Matx44d&RT, G3DtableParam*p3DtableParam);

	// 在摄像机坐标系的球面图
	void OnCameraCoordinate(float x1, float y1, float z1, cv::Mat&Img, CCalibrate&pCameraParam, int&iSrcX, int&iSrcY);

	// 求两个世界坐标系往摄像机坐标系的坐标
	// 将世界坐标系的worldpoint1的坐标映射到归一化的cameraPoint1（摄像机坐标系）上去
	void OnWorldmaptoCamera(vector<cv::Point3f> worldpoint1, vector<cv::Point3f>&cameraPoint1, PointLocation pLocation1, G3DtableParam*paramC);
};


#endif