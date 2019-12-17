#ifndef LOOK_AROUND_MONTAGE_ALG
#define LOOK_AROUND_MONTAGE_ALG

#include "common.h"
#include "calibrate/CCalibrate.h"
#include "table/Generatetable.h"
#include "table/Generate3Dtable.h"
#include "iniFile/inirw.h"

// 这是标定时所需要的参数，对于其他的参数可以从配置文件中读取
struct stu_360Param
{
	cv::Mat fishEyeImg[FISHEYE_COUNT];        // 0上，1下，2左，3右。标定时需要输入的标定图像

	CvSize2D32f  world_show_size;        // 在世界坐标系中显示区域的大小（以毫米为单位）
	CvSize2D32f  world_car_size;               // 在世界坐标系中车的宽度和长度（以毫米为单位）

	float  world_frontAxle_len;         // 世界坐标系中前摄像头对应的正方形所在的空白区域的高度(以毫米为单位)。前轴长度
	float  world_tailAxle_len;          // 世界坐标系中后摄像头对应的正方形所在的空白区域长度的高度（以毫米为单位）。尾轴长度

	int*   iProcess;                    // 记录进度
};
//struct stu2dTable               // 俯视图的查找表
//{
//	Point iLocation;            // 当前俯视图像的坐标值
//
//	int/*cameraLocation*/ img1Idx;     // 第一幅鱼眼图像的ID值。如果此值为-1，那么就代表不会从任何图像上映射值
//	Point2d img1Location;         // iLocation对应到鱼眼图像上的点
//	float img1Weight;           // 第一幅图像的权重
//
//	int/*cameraLocation*/ img2Idx;     // 第二幅鱼眼图像的ID值。如果此值为-1，那么就代表不会从任何图像上映射值
//	Point2d img2Location;         // iLocation对应到当前鱼眼图像上的点
//	float img2Weight;           // 第二幅图像上的权重
//};

class C360LookAroundMontageAlg
{
private:
	errorCode m_errorCode;            // 错误码

	stu_360Param*   m_p360Param;      // 指向参数的指针

	bool            m_bCalibrated;    // 是否已经完成标定，初使化为false
	CCalibrate      m_Calibrate[FISHEYE_COUNT];      // 对四个鱼眼镜头进行标定的变量。0为TOP，1为BOTTOM，2为LEFT，3为RIGHT

	int             m_iSourceWide;
	int             m_iSourceHeight;

	// 同上，四个大的正方形顶点在世界坐标系上的坐标
	vector<cv::Point3f> m_Rect_World[FISHEYE_COUNT];      // 棋盘上设置的有四个正方形。0为左上，1为右上，2为左下，3为右下 

	// 记录每张图像是怎么对应的
	vector<vector<cv::Point3f> > pRectS[FISHEYE_COUNT];
	vector<vector<cv::Point2f> > pRectSImg[FISHEYE_COUNT];

	//////////////////////////////////////////////////////////////////////////
	// 小棋盘格的大小
//	int             m_iCheckboardWide;
	// 大的正方形的大小
//	int             m_iRectWide;

	// 大的正方形距离边界大小
//	int             m_iRectToBoundary;

	cv::Point2f          m_CarBeginPointWorld;


	//////////////////////////////////////////////////////////////////////////
	// 定义标定方案，默认为宝沃BX7的标定方案
	calibrateScheme  m_CalibrateScheme;

	//////////////////////////////////////////////////////////////////////////
	// 房车样式。房车有两种样式，分别为样式1和样式2
	int              m_touringCarMode;


	//////////////////////////////////////////////////////////////////////////
	// 标定进度值
	// 正常情况下，此值为最大为70
	int*              m_speed_process;

	//////////////////////////////////////////////////////////////////////////
	// 图像在世界坐标系中的起始坐标
	float               m_fBeginOriginX;
	float               m_fBeginOriginY;

	//////////////////////////////////////////////////////////////////////////
	// 在x轴和y轴上每个像素对应的真实世界的大小
	float 				m_widePerPixel;	
	float 				m_heightPerPixel;

	//////////////////////////////////////////////////////////////////////////
	// 车模的起点坐标和宽高
	int 				m_iCarWide;
	int					m_iCarHeight;
	int					m_iCarBeginX;
	int					m_iCarBeginY;

public:
	C360LookAroundMontageAlg( int iSourceWide , int isourceHeight );

	// 保存摄像机参数文件，包括内参、外参和畸变系数
	void WriteCamParam();

	// 初使化，若初始化成功，返回true。否则返回false
	bool OnCalibrate( void* pParam );

	//void calibrate(int i);

	// 生成查找表
	// (生成查找表时棋盘在世界坐标系上的坐标原点在左上角处)
	// wide表示生成的图片的的宽度(默认为720)，以像素为单位
	// height表示生成的图像的长度（默认为1280），以像素为单位
	// iSpreadWide和iSpreadHeight分别表示展开图的宽度和高度
	// fBeginOriginX是起始原点X在默认0.0处，可以为负数
	// fBeginOriginY是起始原点Y在默认0.0处，可以为负数
	bool OnGenerateTable( int wide = 720 , int height = 1280 , int iSpreadWide = 1280 , int iSpreadHeight = 720  );

	bool OnGenerate3DTable(int nx, int ny);

	// 生成俯视图模板
	bool OnGenerateModelImg( int wide , int height , CvSize2D32f worldShowSize ,  CvSize2D32f worldCarSize , float fWorldCarBeginX , float fWorldCarBeginY );

	// 释放资源
	bool OnRelease( void* pParam );

	// fx,fy,cx,cy和k都是返回的参数
	bool GetInnerParam( double&fx, double&fy , double&cx , double&cy , double k[KNUM] , cameraLocation cameraIndex );

	// 获取外参，返回
	// 返回的单个摄像机外参个数，rRTLen最长个数为16个（一维存储，连续）
	bool GetOuterParam( double RT0[12] , double RT1[12]  , cameraLocation cameraIndex );

	// 获取平面俯视图，生成查找表
	// 对应的是鱼眼原始图像上的哪一个点坐标
	// pCoords生成的是图像俯视的坐标表
	// lut_w查找表的宽，以像素为单位
	// lut_h查找表的高，以像素为单位
	bool GenTable2d(float* pAlpha, float* pCoords, int lut_w, int lut_h );


	// 获取错误码
	errorCode Get360AlgError();

	// src为输入的鱼眼图原图像，是个指针，有四幅图像，0代表上，1代表后，2代表左，3代表右
	// pTable是输入内存查找表指针
	// dst为输出的2D俯视图
	void OnGet2DImg(cv::Mat*src, stu2dTable*pTable, cv::Mat&dst);

	// 获取不融合的俯视图
	void OnGetJointImg(cv::Mat*src, stu2dTable*pTable, cv::Mat&dst);

	// src为输入的鱼眼图原图像，是个指针，有四幅图像，0代表上，1代表后，2代表左，3代表右
	// pTable是输入内存查找表指针
	// dst为输出的2D俯视图
	void OnGet3DImg(cv::Mat *src, stu3dTable *pTable, cv::Mat &dst);

	// 获取不融合的3D图
	void OnGet3DJointImg(cv::Mat *src, stu3dTable *pTable, cv::Mat &dst);

	// 获取前/后/左/右的单镜头俯视图
	void OnGetEachJointImg(cv::Mat *src, stu2dTable*pTable, cv::Mat *dst);

	// 获取不带双线性插值的展开图
	void OnGetSpreadImg(cv::Mat src, stu_spreadImg*pTable, cv::Mat&dst);

	// 获取带双线性插值的展开图
	void OnGetInterpSpreadImg(cv::Mat src, interpSpreadTable* pTable, cv::Mat &dst);

	// 获取带倒车辅助线的展开图
	void OnGetBackAssistSpreedImg(cv::Mat spread_img, cv::Point *pbackAssistPoints);

	// 获取带倒车辅助线的鱼眼图
	void OnGetBackAssistRawImg(cv::Mat raw_img, cv::Point *pbackAssistPoints);

	// 测试函数，倒车辅助
	void OnTestGetBackAssistImg(cv::Mat src, stu2dTable *p2dTable, stu_spreadImg *pSpreadTable, cv::Mat &dst);

	// 获取鱼眼图像上的倒车辅助点
	void OnGetRawBackAssistPoint( int width, int height, stu2dTable *p2dTable);

	// 获取展开图上的倒车辅助点
	void OnGetSpreedBackAssistPoint(int width, int height, stu2dTable *p2dTable, stu_spreadImg *pSpreadTable);

	// 车道检测函数
	int LaneLineDetect(void* pParam, cv::Mat imgSrc, float& xbiasLup, float& xbiasRup, float& xbiasLdown, float& xbiasRdown);

	// 车道检测函数，每个镜头各检测一次
	int EachLaneLineDetect(void* pParam, cv::Mat *imgSrc, float& xbiasLup, float& xbiasRup, float& xbiasLdown, float& xbiasRdown);

	// 生成优化后的查找表
	bool OnGenLaneLineOptimizedTable(void* pParam, float& xbiasLup, float& xbiasRup, float& xbiasLdown, float& xbiasRdown);

	// 返回进度值
	int OnGetSpeedProcess();

	~C360LookAroundMontageAlg();

private:
	// 从inf配置文件中读取关于棋盘格的配置信息,前构建世界坐标系坐标
	// 打开配置文件成功，返回true；否则返回false
	//////////////////////////////////////////////////////////////////////////
	// 宝沃BX7
	bool OnStructWorldCoordinate( const calibrateScheme calibrateSch );

	// 前摄像头对应的角点世界坐标系
	void OnGetFrontCameraWorldCoordinate();

	// 后摄像头对应的角点世界坐标系
	void OnGetBottomCameraWorldCoordinate();

	// 左摄像头对应的角点世界坐标系
	void OnGetLeftCameraWorldCoordinate();

	// 右摄像头对应的角点世界坐标系
	void OnGetRightCameraWorldCoordinate();

	// 获取四个大正方形在世界坐标系的坐标
	void OnGetBigRectWorldCoordinate();

	// 获取车辆在世界坐标系上的起始位置
	void OnGetCarBeginPointWorldCoordinate();
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	// 房车
	// 返回房车将要标定的样式
	int OnGetTouringCarMode();

	// 前摄像头对应的角点世界坐标系
	void OnGetFrontCameraWorldCoordinate_touringCar();

	// 后摄像头对应的角点世界坐标系
	void OnGetBottomCameraWorldCoordinate_touringCar();

	// 左摄像头对应的角点世界坐标系
	void OnGetLeftCameraWorldCoordinate_touringCar( int iTouringCarMode );

	// 右摄像头对应的角点世界坐标系
	void OnGetRightCameraWorldCoordinate_touringCar( int iTouringCarMode );

	// 获取四个大正方形在世界坐标系的坐标
	void OnGetBigRectWorldCoordinate_touringCar();

	// 获取车辆在世界坐标系上的起始位置
	void OnGetCarBeginPointWorldCoordinate_touringCar();


	// 获取图像显示在世界坐标系上的起始位置(BX7)
	void OnGetImgBeginPointWorldCoordinate();

	// 获取图像显示在世界坐标系上的起始位置（房车）
	void OnGetImgBeginPointWorldCoordinate_touringCar();

	//////////////////////////////////////////////////////////////////////////
};

#endif      // LOOK_AROUND_MONTAGE_ALG
