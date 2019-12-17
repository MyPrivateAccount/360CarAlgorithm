#ifndef COMMON_H
#define COMMON_H

#include <opencv2/opencv.hpp>
//#include <opencv2/calib3d.hpp>
//using namespace cv;

#include <vector>
#include <cmath>
using namespace std;

#include <fstream>
using namespace std;

enum errorCode
{
	NO_ERROR_INFO,          // 无错误
	NOT_CABICATE,      // 没有标定
	READ_INIFILE_ERROR,// 读取配置文件错误
	SHOW_ORIGIN_ERROR,  // 世界坐标中的原点设置错误
	CABICATE_INNER_PARAM_ERROR, // 内参标定错误
	FIND_CORNER_ERROR, // 查找角点错误
	FIND_SQUARE_CORNER_ERROR,   // 查找正方形的四个角点的位置错误
	CABICATE_TIMEOUT,	// 标定超时
	CAR_REGION_ERROR,  // 小车的区域设置错误，超出了显示的俯视图大小
	GENERATE_TABLE_ERROR,
	ERROR_UNKNOWN      // 未知错误
};

enum calibrateScheme
{
	BAOWO_BX7,          // 宝沃BX7的标定方案
	TOURING_CAR         // 房车的标定方案
};

#define DEBUG_SHOW_IMAGE 1      // 在调试阶段，为1的话表示要显示和存储图像。为0就表示不需要输出中间结果进行查看了

#define KNUM             4      // 单个鱼眼摄像机有4个畸变系数
#define RTNUM            16     // 单个鱼眼摄像机有16个外部参数

#define FISHEYE_COUNT           4          // 有四个鱼眼镜头

#define CAR_DIVISION      7000      // （单位为毫米）长度小于CAR_DIVISION的车辆是宝沃车或其他类型的小车。大于CAR_DIVISION的车辆是房车或其他类型的大车

//#define MOTOR_HOME		// 如果定义该宏，则为房车，否则为宝沃BX7

#define SINGLE_THREAD	// 单线程标定，用于调试

//#define SPREED_VIEW		// 如果定义该宏，则生成展开表和展开图

enum cameraLocation       // camera在车辆周围的安装位置
{
	TOP,           // 前
	BOTTOM ,       // 后
	LEFT ,         // 左
	RIGHT          // 右
};

// 原点在角点二维坐标中的位置
enum OriginInImage
{
	LEFT_TOP,     // 左上
	RIGHT_TOP ,   // 右上
	LEFT_BOTTOM , // 左下
	RIGHT_BOTTOM  // 右下
};
// 角点的排列位置
enum ArrayModeInImage
{
	ROW_MODE , // 行的排列方式
	COLOMN_MODE // 列的排列方式
};

struct fishEyeInf        // 鱼眼图像信息
{
	cv::Matx34d innerParam;      // 内参为3*4的矩阵
	cv::Matx44d RT[2];             // 两个外参，第一个外参表示优化的左边或上边，第二个外参表示优化的右边或下边
	cv::Matx14d distortionParam; // 畸变系数为1*4的矩阵
};

#define NEW3D 0              // 新3D算法

#endif