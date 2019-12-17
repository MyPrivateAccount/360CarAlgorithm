#ifndef GENERATE_TABLE_H
#define GENERATE_TABLE_H

#include "../common.h"
#include "../calibrate/CCalibrate.h"

struct stu2dTable               // 俯视图的查找表
{
	cv::Point iLocation;            // 当前俯视图像的坐标值

	int/*cameraLocation*/ img1Idx;     // 第一幅鱼眼图像的ID值。如果此值为-1，那么就代表不会从任何图像上映射值
	cv::Point2d img1Location;         // iLocation对应到鱼眼图像上的点
	float img1Weight;           // 第一幅图像的权重

	int/*cameraLocation*/ img2Idx;     // 第二幅鱼眼图像的ID值。如果此值为-1，那么就代表不会从任何图像上映射值
	cv::Point2d img2Location;         // iLocation对应到当前鱼眼图像上的点
	float img2Weight;           // 第二幅图像上的权重
};

struct stu_spreadImg
{
	ushort iWidex;                 // 宽度
	ushort iHeighty;               // 高度
};

struct Bilinear_Interpolation		// 双线性插值点以及该点权重
{
	cv::Point pLocation;
	float fWeight;       
};

struct interpSpreadTable               // 包含双线性插值的展开表
{
	stu_spreadImg LTpoint;	// 左上角的点

	uchar ucLTWeight;	// 左上角的点的权重
	uchar ucRTWeight;	// 右上角的点的权重
	uchar ucLBWeight;	// 左下角的点的权重
	uchar ucRBWeight;	// 右下角的点的权重
};

// 生成查找表
class CGenerateTable
{
protected:
	CCalibrate* m_pCalibrate;           // 标定类对象，在本类中只是使用它已经标定出来的内外参数
	int         m_iCalibrateNum;        // 有几个标定对象

	//////////////////////////////////////////////////////////////////////////
	// 拼接缝的直线斜率和b值
	float                   m_K[FISHEYE_COUNT];        // 拼接缝的斜率，0，1，2，3分别表示左上拼接缝，右上拼接缝，左下拼接缝，右下拼接缝
	float                   m_B[FISHEYE_COUNT];        // 拼接缝b值，同上

	float                   m_BMix[2*FISHEYE_COUNT];   // 融合线，共八条融合线，分成了四个部分，每两个1组。奇数为融合线在拼接线的下方，偶数（包括0）为融合线在拼接线的上方     

	//////////////////////////////////////////////////////////////////////////
	// 将图像分区
	int                     m_wide_area[3];        // 下标为0、1、2分别表示为左中右
	int                     m_height_area[3];      // 下标为0、1、2分别表示为上中下
	cv::Matx44d                 m_RT_DvalueAverage[4];         // 差均值，只有中间的渐变区域。分别对应着四个棋盘格中间渐变部分。从左渐变到右，从上渐变到下。下标表示上、下、左和右

	int                     m_iSourceWide;          // 原鱼眼图像的宽度
	int                     m_iSourceHeight;        // 原鱼眼图像的高度
public:
	// sourceWide为输入的原图像的宽度
	// sourceHeight为输入的原图像的高度
	CGenerateTable( CCalibrate* pCalibrate , int iNum , int sourceWide = 1280 , int sourceHeight = 720 );

	//////////////////////////////////////////////////////////////////////////
	// 从外部调用生成查找表的参数
	// 通过棋盘的大小和车的大小设计拼接缝
	// worldShowSize是显示的棋盘格区域大小
	// fBeginOriginX表示在世界坐标系中的起点X
	// fBeginOriginY表示在世界坐标系中的起点Y
	// worldCarSize是在这个棋盘格中车的大小
	// fWorldCarBeginX表示小车在棋盘格区域的起点X
	// fWorldCarBeginY表示小车在棋盘格区域的起点Y
	// 如何表生成成功，那么就返回true，否则返回false
	// iWidePixel是将要生成的显示的图像宽度，以像素为单位
	// iHeightPixel是将要生成的显示的图像高度，以像素为单位
	bool OnGenerateInTable( stu2dTable* p2dTable, CvSize2D32f worldShowSize , float fBeginOriginX , float fBeginOriginY , CvSize2D32f worldCarSize , float fWorldCarBeginX , float fWorldCarBeginY , int iWidePixel , int iHeightPixel );
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	// 生成展开图
	// iLocation表示对副图像进行校正，生成查找表
	void OnGenerateCorrectionImg(int iLocation, cv::Size SpreadSize, stu_spreadImg*pBuffer);
	//////////////////////////////////////////////////////////////////////////
	
	~CGenerateTable();
private:
	//////////////////////////////////////////////////////////////////////////
	// 设计拼接缝
	void OnSetSpliceJoint( CvSize2D32f worldShowSize , float fBeginOriginX , float fBeginOriginY , CvSize2D32f worldCarSize , float fWorldCarBeginX , float fWorldCarBeginY , double PixelPerWide , double PixelPerHeight );

	//////////////////////////////////////////////////////////////////////////
	// 生成二进制查找表
	bool OnGenerateBinaryTable( stu2dTable* p2dTable, float fBeginX , float fBeginY , double pixel_wide , double pixel_height  , int iWidePixel , int iHeightPixel );

	void GetRTvalue(cv::Matx44d&ReturnRT, int NowIndex, int*pIArrea, const fishEyeInf&fishEye, cv::Matx44d&DValueRT);

	// 从鱼眼图上映射
	void mappingFromFisheye(cv::Mat&WorldLocation, cv::Matx44d*RT, stu2dTable& tableInf, int isrcWide, int isrcHeight);

	// 生成展开图的查找表
	void initUndistortRectifyMapSpread(cv::InputArray K, cv::InputArray D, cv::InputArray R, cv::InputArray P, const cv::Size& size, int m1type, stu_spreadImg*pBuffer, interpSpreadTable* pinterpSpreadTable);
	
};


bool convert_tbl(stu2dTable * pLookAtTable);

#endif      // GENERATE_TABLE_H
