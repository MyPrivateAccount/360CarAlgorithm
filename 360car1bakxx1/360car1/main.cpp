//////////////////////////////////////////////////////////////////////////
// 主函数调用算法实现过程
//////////////////////////////////////////////////////////////////////////
//#include <stdio.h>
#include <iostream>
using namespace std;
#include "algorithm/360LookaroundMontageAlg.h"
#include <fstream>
using namespace std;

#define NX		2000
#define NY		4000
// #define MOTOR_HOME 0

int main()
{
	///////
	//////////////////////////////////////////////////////////////////////////
	// fdasfafdasfadsfdas
	////////
	//////////////////////////////////////
	// 设置参数
	int i;
	stu_360Param pParam;
	string fishEyePath[4];
#ifdef MOTOR_HOME
	//fishEyePath[0] = "bak/tourCar/3/cam_cap1.jpg";    // 上
	//fishEyePath[1] = "bak/tourCar/3/cam_cap2.jpg" ;  // 下
	//fishEyePath[2] = "bak/tourCar/3/cam_cap3.jpg" ;  // 左
	//fishEyePath[3] = "bak/tourCar/3/cam_cap4.jpg" ;  // 右

	//fishEyePath[0] = "bak/1025/tempimg/cam_cap1.jpg";    // 上
	//fishEyePath[1] = "bak/1025/tempimg/cam_cap2.jpg" ;  // 下
	//fishEyePath[2] = "bak/1025/tempimg/cam_cap3.jpg" ;  // 左
	//fishEyePath[3] = "bak/1025/tempimg/cam_cap4.jpg" ;  // 右

	//fishEyePath[0] = "temp/tempimg_bad_1707/cam_cap1.jpg";    // 上
	//fishEyePath[1] = "temp/tempimg_bad_1707/cam_cap2.jpg";  // 下
	//fishEyePath[2] = "temp/tempimg_bad_1707/cam_cap3.jpg";  // 左
	//fishEyePath[3] = "temp/tempimg_bad_1707/cam_cap4.jpg";  // 右

	fishEyePath[0] = "cam_cap1.jpg";    // 上
	fishEyePath[1] = "cam_cap2.jpg" ;  // 下
	fishEyePath[2] = "cam_cap3.jpg" ;  // 左
	fishEyePath[3] = "cam_cap4.jpg" ;  // 右
#else
	fishEyePath[0] = "bak/BX7/1/cam_cap1.jpg";    // 上
	fishEyePath[1] = "bak/BX7/1/cam_cap2.jpg";  // 下
	fishEyePath[2] = "bak/BX7/1/cam_cap3.jpg";  // 左
	fishEyePath[3] = "bak/BX7/1/cam_cap4.jpg";  // 右

	fishEyePath[0] = "bak/180109/calibraw_yuv_buffer_crop_0.bmp";    // 上
	fishEyePath[1] = "bak/180109/calibraw_yuv_buffer_crop_1.bmp";  // 下
	fishEyePath[2] = "bak/180109/calibraw_yuv_buffer_crop_2.bmp";  // 左
	fishEyePath[3] = "bak/180109/calibraw_yuv_buffer_crop_3.bmp";  // 右

	fishEyePath[0] = "bak/180112/cam_cap1.jpg";    // 上
	fishEyePath[1] = "bak/180112/cam_cap2.jpg";  // 下
	fishEyePath[2] = "bak/180112/cam_cap3.jpg";  // 左
	fishEyePath[3] = "bak/180112/cam_cap4.jpg";  // 右

	fishEyePath[0] = "capture00.bmp";    // 上
	fishEyePath[1] = "capture01.bmp";  // 下
	fishEyePath[2] = "capture02.bmp";  // 左
	fishEyePath[3] = "capture03.bmp";  // 右

	//fishEyePath[0] = "bak/BX7/6/0_source.bmp";    // 上
	//fishEyePath[1] = "bak/BX7/6/1_source.bmp";  // 下
	//fishEyePath[2] = "bak/BX7/6/2_source.bmp";  // 左
	//fishEyePath[3] = "bak/BX7/6/3_source.bmp";  // 右

	//fishEyePath[0] = "bak/1102/tempimg-1518/cam_cap1.jpg";    // 上
	//fishEyePath[1] = "bak/1102/tempimg-1518/cam_cap2.jpg";  // 下
	//fishEyePath[2] = "bak/1102/tempimg-1518/cam_cap3.jpg";  // 左
	//fishEyePath[3] = "bak/1102/tempimg-1518/cam_cap4.jpg";  // 右
#if 0
	fishEyePath[0] = "bx7_pic1/calibraw_yuv_buffer_crop_0.bmp";    // 上
	fishEyePath[1] = "bx7_pic1/calibraw_yuv_buffer_crop_1.bmp";  // 下
	fishEyePath[2] = "bx7_pic1/calibraw_yuv_buffer_crop_2.bmp";  // 左
	fishEyePath[3] = "bx7_pic1/calibraw_yuv_buffer_crop_3.bmp";  // 右
#endif
#endif
	//for ( int i = 0 ; i<4 ; i++ )
	//{
	//	cv::resize(Matxx[i], Dstss[i], cv::Size(540, 720), (0, 0), (0, 0), cv::INTER_LINEAR);
	//}
	// cv::Mat cc = cv::imread("D://program//car360BQL//capture00.bmp", 0);

	for ( i = 0 ; i<FISHEYE_COUNT ; i++ )
	{
		pParam.fishEyeImg[i] = cv::imread(fishEyePath[i], 0);              // 以灰度图的形式读取
	}
#ifdef MOTOR_HOME       //房车
	//CvSize2D32f carRange = cvSize2D32f( 2800/*CHECK_BOARD_PATTERN_LEN*/ , 10500/*CHECK_BOARD_PATTERN_LEN*/ );           // 显示的车的区域大小
	//CvSize2D32f checkBoardRange = cvSize2D32f( 14400/*CHECK_BOARD_PATTERN_LEN*/ , 18400/*CHECK_BOARD_PATTERN_LEN*/ );      // 棋盘格大小
	CvSize2D32f carRange = cvSize2D32f( 2700/*CHECK_BOARD_PATTERN_LEN*/ , 10500/*CHECK_BOARD_PATTERN_LEN*/ );           // 显示的车的区域大小
	CvSize2D32f checkBoardRange = cvSize2D32f( /*9400*/6400/*CHECK_BOARD_PATTERN_LEN*/ , /*18000*/14000/*CHECK_BOARD_PATTERN_LEN*/ );      // 棋盘格大小
#else
	//CvSize2D32f carRange = cvSize2D32f( 2000/*CHECK_BOARD_PATTERN_LEN*/ , 5400/*CHECK_BOARD_PATTERN_LEN*/ );           // 显示的车的区域大小
	//CvSize2D32f checkBoardRange = cvSize2D32f( 5600/*CHECK_BOARD_PATTERN_LEN*/ , 9000/*CHECK_BOARD_PATTERN_LEN*/ );      // 棋盘格大小

	CvSize2D32f carRange = cvSize2D32f(2000/*1911*//*CHECK_BOARD_PATTERN_LEN*/, 5400/*4800*//*CHECK_BOARD_PATTERN_LEN*/);           // 显示的车的区域大小
	CvSize2D32f checkBoardRange = cvSize2D32f(5600/*7400*//*CHECK_BOARD_PATTERN_LEN*/, /*14000*/9000/*CHECK_BOARD_PATTERN_LEN*/);      // 棋盘格大小
#endif
	pParam.world_show_size = checkBoardRange;

	pParam.world_frontAxle_len = 10*60/*CHECK_BOARD_PATTERN_LEN*/;              // 前距轴长度为world_frontAxle_len
	pParam.world_tailAxle_len  = 10*60/*CHECK_BOARD_PATTERN_LEN*/;              // 尾距轴长度为world_tailAxle_len
	pParam.world_car_size = carRange;

	bool bReturn;
	errorCode errCode;

	//////////////////////////////////////////////////////////////////////////
	// 原始的鱼眼图像的宽度为1280，高度为720
	C360LookAroundMontageAlg alg(/*960*/1280 , 720/*540*/);

#if 1
	int iCalibrateProcess = 0;

	pParam.iProcess = &iCalibrateProcess;

	bReturn = alg.OnCalibrate(&pParam);             // 标定完成后，获取了每个摄像头的内参外参和畸变系数

	if (!bReturn)
	{
		errCode = alg.Get360AlgError();
		printf("OnCalibrate failed\n");
		return -1;
	}

	int iProcess = alg.OnGetSpeedProcess();

	// 生成的目标俯视图的宽度为720，高度为1280
	bReturn = alg.OnGenerateTable( 720/*540*/ , 1280/*720*//*1280*/ , 1280 , 720 );

	if ( !bReturn )
	{
		errCode = alg.Get360AlgError();
		printf("OnGenerateTable failed\n");
		return -1;
	}

	// 生成的目标俯视图的宽度为720，高度为1280
	bReturn = alg.OnGenerate3DTable(NX, NY);
#endif

#if 1
	// 获取俯视图
	cv::Mat DstMatx = cv::imread("modelCar.bmp");

	cv::Mat Matxx[FISHEYE_COUNT];

#ifdef MOTOR_HOME
	Matxx[0] = cv::imread( "cam_cap1.jpg" );
	Matxx[1] = cv::imread( "cam_cap2.jpg" );
	Matxx[2] = cv::imread( "cam_cap3.jpg" );
	Matxx[3] = cv::imread( "cam_cap4.jpg" );
//	for (int i = 0; i < FISHEYE_COUNT; i++)
//		Matxx[i] = imread(fishEyePath[i]);
#else
	//Matxx[0] = imread("linecapture00.bmp");  // 上
	//Matxx[1] = imread("linecapture01.bmp");  // 下
	//Matxx[2] = imread("linecapture02.bmp");  // 左
	//Matxx[3] = imread("linecapture03.bmp");  // 右
	for(int i = 0; i < FISHEYE_COUNT; i++)
		Matxx[i] = cv::imread(fishEyePath[i]);
#if 0
	Matxx[0] = imread("bx7_pic1/calibraw_yuv_buffer_crop_0.bmp");    // 上
	Matxx[1] = imread("bx7_pic1/calibraw_yuv_buffer_crop_1.bmp");  // 下
	Matxx[2] = imread("bx7_pic1/calibraw_yuv_buffer_crop_2.bmp");  // 左
	Matxx[3] = imread("bx7_pic1/calibraw_yuv_buffer_crop_3.bmp");  // 右
#endif
#endif
#if 1
	stu2dTable* p2dTable;
	p2dTable = new stu2dTable[DstMatx.cols*DstMatx.rows];
	FILE*file = NULL;
	if (file = fopen( "tempImg//Table.bin", "rb"))
	{
		fread(p2dTable , sizeof(stu2dTable) , DstMatx.cols*DstMatx.rows , file );
		fclose( file );
	}
	else
	{
		printf("open Table.bin failed\n");
	}

	alg.OnGet2DImg( Matxx , p2dTable , DstMatx );

//	imshow( "zhouchaoyang" , DstMatx );
//	waitKey( 0 );

	imwrite( "tempImg//20170508.bmp" , DstMatx );

	alg.OnGetJointImg(Matxx, p2dTable, DstMatx);

	imwrite("tempImg//20170508_joint.bmp", DstMatx);
#endif

#if 1    // 3D
	int size = sizeof(stu3dTable);
	cv::Mat Dst3D(NX, NY, CV_8UC3, cv::Scalar(0, 0, 0));//其实是2*6的矩阵，因为每个元素有3个通道。

	stu3dTable* p3dTable;
	p3dTable = new stu3dTable[NX * NY];
	FILE *file3D = NULL;
	if ( file3D = fopen( "tempImg//Table3D.bin", "rb"))
	{
		fread(p3dTable, sizeof(stu3dTable), NX * NY, file3D);
		fclose(file3D);
	}
	else
	{
		printf("open Table3D.bin failed\n");
	}

	alg.OnGet3DImg(Matxx, p3dTable, Dst3D);

	imwrite("tempimg//20171220_3D.bmp", Dst3D);

	alg.OnGet3DJointImg(Matxx, p3dTable, Dst3D);

	imwrite("tempimg//20171220_3Djoint.bmp", Dst3D);

#endif
	//Mat DstMatxx[FISHEYE_COUNT];

	//for (i = 0; i<FISHEYE_COUNT; i++)
	//{
	//	DstMatxx[i].create(1280, 720, CV_8UC3);
	//	DstMatxx[i].setTo(Scalar(0, 0, 0));
	//}

	//alg.OnGetEachJointImg(Matxx, p2dTable, DstMatxx);

	//imwrite("tempimg//joint_front.bmp", DstMatxx[0]);
	//imwrite("tempimg//joint_back.bmp", DstMatxx[1]);
	//imwrite("tempimg//joint_left.bmp", DstMatxx[2]);
	//imwrite("tempimg//joint_right.bmp", DstMatxx[3]);

	//delete[]p2dTable;
	//p2dTable = NULL;
#endif

//#ifdef SPREED_VIEW

	// 获取不带双线性插值的展开图
	cv::Mat spreadMat(720, 1280, CV_8UC3);
	stu_spreadImg*pspreadTable;
	pspreadTable = new stu_spreadImg[720*1280];

	FILE *files = NULL;
	if ( files = fopen( "tempimg//spread_1.bin" , "rb") )
	{
		fread(pspreadTable , sizeof(stu_spreadImg) , 1280*720 , files );
		fclose( files );
	}

	alg.OnGetSpreadImg( Matxx[1] , pspreadTable , spreadMat );

	imwrite( "tempimg//img1.bmp" , spreadMat );

	// 获取带双线性插值的展开图
	cv::Mat interpSpreadMat(720, 1280, CV_8UC3);
	interpSpreadTable *pinterpSpreadTable;
	pinterpSpreadTable = new interpSpreadTable[720 * 1280];

	if (files = fopen( "tempimg//interp_spread_0.bin", "rb"))
	{
		fread(pinterpSpreadTable, sizeof(interpSpreadTable), 1280 * 720, files);
		fclose(files);
	}

	alg.OnGetInterpSpreadImg(Matxx[0], pinterpSpreadTable, interpSpreadMat);

	imwrite("tempimg//interpSpread_0.bmp", interpSpreadMat);
	delete[]pinterpSpreadTable;
	pinterpSpreadTable = NULL;


	//// 获取带倒车辅助线的展开图
	//Point *pbackAssistPoints;
	//pbackAssistPoints = new Point[6];

	//FILE *fBackAssist = NULL;
	//if ( fBackAssist = fopen("tempimg//back_assist.bin" , "rb") )
	//{
	//	fread(pbackAssistPoints , sizeof(Point) , 6 , fBackAssist );
	//	fclose( fBackAssist );
	//}

	//alg.OnGetBackAssistSpreedImg(interpSpreadMat, pbackAssistPoints);

	//delete []pbackAssistPoints;
	//delete[]pspreadTable;
	//pspreadTable = NULL;
//#else
//
//	Point *pbackAssistRawPoints;
//	pbackAssistRawPoints = new Point[2048];
//	FILE *fBackAssistRaw = NULL;
//	if (fBackAssistRaw = fopen("tempimg//back_point.bin", "rb"))
//	{
//		fread(pbackAssistRawPoints, sizeof(Point), 2048, fBackAssistRaw);
//		fclose(fBackAssistRaw);
//	}
//
//	Mat BackImg = imread("linecapture01.bmp");  // 下
//
//	alg.OnGetBackAssistRawImg(BackImg, pbackAssistRawPoints);
//
//	delete []pbackAssistRawPoints;
//#endif

#if 0
	// 获取车道线优化后的俯视图
	Mat imgSrc = imread("tempimg//20170508_joint.bmp");

	float xbiasLup, xbiasRup, xbiasLdown, xbiasRdown;
	//int st = GetTickCount();
	//int ret = alg.EachLaneLineDetect(&pParam, DstMatxx, xbiasLup, xbiasRup, xbiasLdown, xbiasRdown);
	int ret = alg.LaneLineDetect(&pParam, imgSrc, xbiasLup, xbiasRup, xbiasLdown, xbiasRdown);
	//int ed = GetTickCount();

	if (ret < 0) return -1;

	//printf("计算LLDET time://%dms \n", ed - st);

	printf("left top corner offset: xbiasLup = %.1f \n", xbiasLup);
	printf("right top corner offset: xbiasRup = %.1f \n", xbiasRup);
	printf("left botton corner offset: xbiasLdown=%.1f \n", xbiasLdown);
	printf("right botton corner offset: xbiasRdown=%.1f \n", xbiasRdown);

	if (fabs(xbiasLup) < 1e-2 && fabs(xbiasRup) < 1e-2 && fabs(xbiasLdown) < 1e-2 && fabs(xbiasRdown) < 1e-2)
	{
		printf("Current view is OK, no need to optimise\n");
		return -1;
	}

	int calibrateProcess = 0;
	pParam.iProcess = &calibrateProcess;

	alg.OnGenLaneLineOptimizedTable(&pParam, xbiasLup, xbiasRup, xbiasLdown, xbiasRdown);

	// 生成的目标俯视图的宽度为720，高度为1280
	bReturn = alg.OnGenerateTable(720/*540*/, 1280/*720*//*1280*/, 1280, 720);

	if (!bReturn)
	{
		errCode = alg.Get360AlgError();
	}

#if 1
	p2dTable = new stu2dTable[DstMatx.cols*DstMatx.rows];
	if (file = fopen("tempimg//Table.bin", "rb"))
	{
		fread(p2dTable, sizeof(stu2dTable), DstMatx.cols*DstMatx.rows, file);
		fclose(file);
	}
	else
	{
		printf("open Table.bin failed\n");
	}

	alg.OnGet2DImg(Matxx, p2dTable, DstMatx);

	//imshow( "laneLine" , DstMatx );
	//waitKey( 0 );

	imwrite("tempimg//20170508_optimize.bmp", DstMatx);

	delete[]p2dTable;
	p2dTable = NULL;
#endif

	// 如果优化成功，则重新生成摄像机参数文件
	alg.WriteCamParam();

#endif

	return 0;
}