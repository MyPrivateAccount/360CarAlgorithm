#include "360LookaroundMontageAlg.h"
#include <chrono>
#include <condition_variable>
#include <future>
#include <mutex>
#include <queue>
#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

C360LookAroundMontageAlg::C360LookAroundMontageAlg(  int iSourceWide , int isourceHeight )
{
	m_errorCode = NO_ERROR_INFO;
	m_bCalibrated = false;

	m_iSourceHeight = isourceHeight;
	m_iSourceWide = iSourceWide;

	// 默认为宝沃BX7的标定方案
	m_CalibrateScheme = BAOWO_BX7;

	// 初使为0
	m_fBeginOriginX = 0;
	m_fBeginOriginY = 0;
}

// 返回房车将要标定的样式
int C360LookAroundMontageAlg::OnGetTouringCarMode()
{
	int iMode = 0;

	char* sect;
	char* key;

	sect = "touringCarScheme";
	key = "touringScheme";
	iMode = iniGetInt(sect , key , 20 );

	return iMode;
}

// 前摄像头对应的角点世界坐标系
void C360LookAroundMontageAlg::OnGetFrontCameraWorldCoordinate_touringCar()
{
		// 0
	CvSize temp;//(23 , 25 );
	int    iBeginX = 0;         // 第一个角点的起点X坐标
	int    iBeginY = 0;         // 第一个角点的起点Y坐标
	int    iTemp = 0;
	char* sect;
	char* key;

	sect = "commonStyle";
	key = "Top_Bottom_wideDimension";
	temp.width = iniGetInt(sect , key , 20 ) - 1;

	key = "Top_Bottom_heightDimension";
	temp.height = iniGetInt( sect , key , 10) - 1;

	//////////////////////////////////////////////////////////////////////////
	// 获取第一个角点的起始X坐标
	key = "rectToBoundary";
	iTemp = iniGetInt( sect , key , 10 );
	iTemp = iTemp*2;

	key = "rectWide";
	iBeginX = iniGetInt( sect , key , 10 );

	iBeginX = iBeginX + iTemp;

	key = "checkboardWide";
	iTemp = iniGetInt( sect , key , 10 );
	iBeginX = iBeginX + iTemp;

	// 第一个角点的起始Y坐标
	key = "Top_Bottom_checkboardToBoundary";
	iBeginY = iniGetInt( sect , key , 10 );
	iBeginY = iBeginY + iTemp;
	//////////////////////////////////////////////////////////////////////////

	int iRectLenHeight;                          // 正方形的高度
	key = "checkboardHeightP";
	iRectLenHeight = iniGetInt(sect, key, 100);

	m_Calibrate[0].OnSetCornerWorldLocation(iBeginX, iBeginY, temp, iTemp, iRectLenHeight, TOP, true);
}

// 后摄像头对应的角点世界坐标系
void C360LookAroundMontageAlg::OnGetBottomCameraWorldCoordinate_touringCar()
{
	// 1
	CvSize temp;//(23 , 25 );
	//temp.width = 23;
	//temp.height = 25;
	int iRectLen = 0;

	int    iBeginX = 0;         // 第一个角点的起点X坐标
	int    iBeginY = 0;         // 第一个角点的起点Y坐标
	int    iTemp = 0;
	char* sect;
	char* key;

	sect = "commonStyle";
	key = "Top_Bottom_wideDimension";
	temp.width = iniGetInt(sect , key , 20 ) - 1;

	key = "Top_Bottom_heightDimension";
	temp.height = iniGetInt( sect , key , 20 ) - 1;

	//////////////////////////////////////////////////////////////////////////
	// 获取第一个角点的X坐标
	key = "rectToBoundary";
	iTemp = iniGetInt( sect , key , 20 );

	iTemp = iTemp*2;
	key = "rectWide";
	iBeginX = iniGetInt( sect , key , 200 );

	iBeginX = iBeginX + iTemp;

	key = "checkboardWide";
	iTemp = iniGetInt( sect , key , 200 );              // 棋盘格的大小，以毫米为单位
	iRectLen = iTemp;                         // 存储每个小棋盘格的大小
	iBeginX = iBeginX + iTemp;                // 第一个角点的X坐标
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	// 获取第一个角点的Y坐标
	key = "checkBoardHeight";
	iTemp = iniGetInt( sect , key , 200 );         // 获取整张棋盘的高度

	key = "Top_Bottom_checkboardToBoundary";
	iBeginY = iniGetInt( sect , key , 200 );

	iBeginY = iTemp - iBeginY;
	iBeginY = iBeginY - temp.height*iRectLen;
	//////////////////////////////////////////////////////////////////////////

	int iRectLenHeight;                          // 正方形的高度
	key = "checkboardHeightP";
	iRectLenHeight = iniGetInt(sect, key, 100);

	m_Calibrate[1].OnSetCornerWorldLocation(iBeginX, iBeginY, temp, iRectLen, iRectLenHeight, BOTTOM, true);
}

// 左摄像头对应的角点世界坐标系
void C360LookAroundMontageAlg::OnGetLeftCameraWorldCoordinate_touringCar( int iTouringCarMode )
{
	// 样式1
	if ( iTouringCarMode == 1 )
	{
		// 2
		CvSize temp;//(23 , 25 );
		//	temp.width = 23;
		//	temp.height = 25;

		int    iBeginX = 0;         // 第一个角点的起点X坐标
		int    iBeginY = 0;         // 第一个角点的起点Y坐标
		int    iTemp = 0;
		int    iRectLen = 0;		// 小棋盘格的宽度
		int    iRectLenHeight;         // 小棋盘格的高度
		char* sect;
		char* key;

		sect = "touringCar1";
		//////////////////////////////////////////////////////////////////////////
		// 获取左棋盘的维度
		key = "Left_Right_wideDimension";
		temp.width = iniGetInt( sect , key , 100 ) - 1;

		key = "Left_Right_heightDimension";
		temp.height = iniGetInt( sect , key , 100 ) - 1;

		// 获取小棋盘格的大小，以毫米为单位
		sect = "commonStyle";
		key = "checkboardWide";
		iRectLen = iniGetInt( sect , key , 100 );

		key = "checkboardHeightP";
		iRectLenHeight = iniGetInt(sect, key, 100);

		//////////////////////////////////////////////////////////////////////////
		// 获取第一个角点的X坐标
		key = "Left_Right_checkboardToBoundary";
		iBeginX = iniGetInt( sect , key , 100 );

		iBeginX = iBeginX + iRectLen;
		//////////////////////////////////////////////////////////////////////////

		//////////////////////////////////////////////////////////////////////////
		// 获取第一个角点的Y坐标
		key = "frontAxle";
		iBeginY = iniGetInt( sect , key , 100 );
		iBeginY = iBeginY + iRectLenHeight;
		//////////////////////////////////////////////////////////////////////////

		//////////////////////////////////////////////////////////////////////////
		m_Calibrate[2].OnSetCornerWorldLocation( iBeginX , iBeginY , temp , iRectLen , iRectLenHeight , LEFT ,true );
	}
	// 样式2
	if ( iTouringCarMode == 2 )
	{
		// 2
		CvSize temp;//(23 , 25 );
		//	temp.width = 23;
		//	temp.height = 25;

		int iFrontAxleT = 0;
		int iCheckBoardHeight = 0;

		int    iBeginX = 0;         // 第一个角点的起点X坐标
		int    iBeginY = 0;         // 第一个角点的起点Y坐标
		int    iTemp = 0;
		int    iRectLen = 0;
		char* sect;
		char* key;

		sect = "touringCar2";
		//////////////////////////////////////////////////////////////////////////
		// 获取左棋盘的维度
		key = "Left_Right_wideDimension";
		temp.width = iniGetInt( sect , key , 100 ) - 1;

		key = "Left_Right_heightDimension";
		temp.height = iniGetInt( sect , key , 100 ) - 1;
		iCheckBoardHeight = temp.height + 1;

		// 获取小棋盘格的大小，以毫米为单位
		sect = "commonStyle";
		key = "checkboardWide";
		iRectLen = iniGetInt( sect , key , 100 );

		//////////////////////////////////////////////////////////////////////////
		// 获取第一个角点的X坐标
		key = "Left_Right_checkboardToBoundary";
		iBeginX = iniGetInt( sect , key , 100 );

		iBeginX = iBeginX + iRectLen;
		//////////////////////////////////////////////////////////////////////////

		//////////////////////////////////////////////////////////////////////////
		// 获取第一个角点的Y坐标
		key = "frontAxle";
		iBeginY = iniGetInt( sect , key , 100 );
		iFrontAxleT = iBeginY;                          // 临时存储
		iBeginY = iBeginY + iRectLen;
		//////////////////////////////////////////////////////////////////////////

		//////////////////////////////////////////////////////////////////////////
		m_Calibrate[2].OnSetCornerWorldLocation( iBeginX , iBeginY , temp , iRectLen , iRectLen , LEFT ,true );          // 第一个棋盘

		// iBeginX不变，第二个iBeginY改变
		iBeginY = iFrontAxleT + iRectLen*iCheckBoardHeight;
		sect = "touringCar2";
		key = "middle_space";
		iTemp = iniGetInt( sect , key , 100 );

		iBeginY = iBeginY + iTemp + iRectLen;

		m_Calibrate[2].OnSetCornerWorldLocation( iBeginX , iBeginY , temp , iRectLen , iRectLen , LEFT , false );       // 第二个棋盘
	}
}

// 右摄像头对应的角点世界坐标系
void C360LookAroundMontageAlg::OnGetRightCameraWorldCoordinate_touringCar( int iTouringCarMode )
{
	// 样式1
	if ( iTouringCarMode == 1 )
	{
		// 3
		CvSize temp;//(23 , 25 );
		//temp.width = 23;
		//temp.height = 25;

		int    iBeginX = 0;         // 第一个角点的起点X坐标
		int    iBeginY = 0;         // 第一个角点的起点Y坐标
		int    iTemp = 0;
		int    iRectLen = 0;		// 小棋盘格的宽度
		int    iRectLenHeight;         // 小棋盘格的高度
		char* sect;
		char* key;

		//////////////////////////////////////////////////////////////////////////
		// 获取棋盘的维度
		sect = "touringCar1";
		key = "Left_Right_wideDimension";
		temp.width = iniGetInt( sect , key , 100 ) - 1;

		key = "Left_Right_heightDimension";
		temp.height = iniGetInt( sect , key , 100 ) - 1;
		//////////////////////////////////////////////////////////////////////////

		// 小棋盘格的大小
		sect = "commonStyle";
		key = "checkboardWide";
		iRectLen = iniGetInt( sect , key , 100 );

		key = "checkboardHeightP";
		iRectLenHeight = iniGetInt(sect, key, 100);

		//////////////////////////////////////////////////////////////////////////
		// 右棋盘的起点X
		key = "checkBoardWideX";
		iBeginX = iniGetInt( sect , key , 100 );

		key = "Left_Right_checkboardToBoundary";
		iTemp = iniGetInt( sect , key , 100 );

		iBeginX = iBeginX - iTemp;

		iBeginX = iBeginX - temp.width*iRectLen;
		//////////////////////////////////////////////////////////////////////////

		//////////////////////////////////////////////////////////////////////////
		// 左棋盘的起点Y
		key = "frontAxle";
		iBeginY = iniGetInt( sect , key , 100 );
		iBeginY = iBeginY + iRectLenHeight;
		//////////////////////////////////////////////////////////////////////////

		m_Calibrate[3].OnSetCornerWorldLocation( iBeginX , iBeginY , temp , iRectLen , iRectLenHeight , RIGHT , true );


	}
	// 样式2
	if ( iTouringCarMode == 2 )
	{
		// 3
		CvSize temp;//(23 , 25 );
		//temp.width = 23;
		//temp.height = 25;
		int iFrontAxleT = 0;
		int iCheckBoardHeight = 0;

		int    iBeginX = 0;         // 第一个角点的起点X坐标
		int    iBeginY = 0;         // 第一个角点的起点Y坐标
		int    iTemp = 0;
		int    iRectLen = 0;
		char* sect;
		char* key;

		//////////////////////////////////////////////////////////////////////////
		// 获取棋盘的维度
		sect = "touringCar2";
		key = "Left_Right_wideDimension";
		temp.width = iniGetInt( sect , key , 100 ) - 1;

		key = "Left_Right_heightDimension";
		temp.height = iniGetInt( sect , key , 100 ) - 1;
		iCheckBoardHeight = temp.height + 1;
		//////////////////////////////////////////////////////////////////////////

		// 小棋盘格的大小
		sect = "commonStyle";
		key = "checkboardWide";
		iRectLen = iniGetInt( sect , key , 100 );

		//////////////////////////////////////////////////////////////////////////
		// 右棋盘的起点X
		key = "checkBoardWideX";
		iBeginX = iniGetInt( sect , key , 100 );

		key = "Left_Right_checkboardToBoundary";
		iTemp = iniGetInt( sect , key , 100 );

		iBeginX = iBeginX - iTemp;

		iBeginX = iBeginX - temp.width*iRectLen;
		//////////////////////////////////////////////////////////////////////////

		//////////////////////////////////////////////////////////////////////////
		// 左棋盘的起点Y
		key = "frontAxle";
		iBeginY = iniGetInt( sect , key , 100 );
		iFrontAxleT = iBeginY;
		iBeginY = iBeginY + iRectLen;
		//////////////////////////////////////////////////////////////////////////

		m_Calibrate[3].OnSetCornerWorldLocation( iBeginX , iBeginY , temp , iRectLen , iRectLen , RIGHT , true );

		// iBeginX不变，第二个iBeginY改变
		iBeginY = iFrontAxleT + iRectLen*iCheckBoardHeight;
		sect = "touringCar2";
		key = "middle_space";
		iTemp = iniGetInt( sect , key , 100 );

		iBeginY = iBeginY + iTemp + iRectLen;
		m_Calibrate[3].OnSetCornerWorldLocation( iBeginX , iBeginY , temp , iRectLen , iRectLen , RIGHT , false );

	}
}

// 获取四个大正方形在世界坐标系的坐标
void C360LookAroundMontageAlg::OnGetBigRectWorldCoordinate_touringCar()
{
	//////////////////////////////////////////////////////////////////////////
	// 获取左上角正方形顶点在世界坐标系上的坐标
	char* sect;
	char* key;
	cv::Point3f pTemp;
	int iBeginX = 0;
	int iBeginY = 0;
	int iRectLen = 0;
	int iRectToBoundary = 0;

	int iTemp1 = 0;
	int iTemp2 = 0;

	sect = "commonStyle";
	key = "rectToBoundary";

	iRectToBoundary = iniGetInt( sect , key , 100 );
	//iRectToBoundary = iniGetInt( sect , key , 100 );
	iBeginX = iRectToBoundary;
	iBeginY = iRectToBoundary;
	pTemp.x = iBeginX;
	pTemp.y = iBeginY;
	pTemp.z = 0;

	m_Rect_World[0].push_back( pTemp );

	// 获取大正形的宽度
	key = "rectWide";
	iRectLen = iniGetInt( sect , key , 100 );
	pTemp.x = iBeginX + iRectLen;

	m_Rect_World[0].push_back( pTemp );

	pTemp.x = iBeginX;
	pTemp.y = iBeginY + iRectLen;
	m_Rect_World[0].push_back( pTemp );

	pTemp.x = iBeginX + iRectLen;
	pTemp.y = iBeginY + iRectLen;
	m_Rect_World[0].push_back( pTemp );
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	// 获取右上角顶点在世界坐标上的坐标
	key = "checkBoardWideX";
	iBeginX = iniGetInt( sect , key , 100 );

	iBeginX = iBeginX - iRectToBoundary;
	iBeginX = iBeginX - iRectLen;
	iBeginY = iRectToBoundary;

	iTemp1 = iBeginX;

	pTemp.x = iBeginX;
	pTemp.y = iBeginY;
	m_Rect_World[1].push_back( pTemp );

	pTemp.x = iBeginX + iRectLen;
	m_Rect_World[1].push_back( pTemp );

	pTemp.x = iBeginX;
	pTemp.y = iBeginY + iRectLen;
	m_Rect_World[1].push_back( pTemp );

	pTemp.x = iBeginX + iRectLen;
	pTemp.y = iBeginY + iRectLen;
	m_Rect_World[1].push_back( pTemp );
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	// 获取左下角顶点在世界坐标上的坐标
	key = "checkBoardHeight";
	iBeginX = iRectToBoundary;
	iBeginY = iniGetInt( sect , key , 100 );
	iBeginY = iBeginY - iRectToBoundary;
	iBeginY = iBeginY - iRectLen;

	iTemp2 = iBeginY;

	pTemp.x = iBeginX;
	pTemp.y = iBeginY;
	m_Rect_World[2].push_back( pTemp );

	pTemp.x = iBeginX + iRectLen;
	m_Rect_World[2].push_back( pTemp );

	pTemp.x = iBeginX;
	pTemp.y = iBeginY + iRectLen;
	m_Rect_World[2].push_back( pTemp );

	pTemp.x = iBeginX + iRectLen;
	pTemp.y = iBeginY + iRectLen;
	m_Rect_World[2].push_back( pTemp );
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	// 获取右下角顶点在世界坐标上的坐标
	iBeginX = iTemp1;
	iBeginY = iTemp2;

	pTemp.x = iBeginX;
	pTemp.y = iBeginY;
	m_Rect_World[3].push_back( pTemp );

	pTemp.x = iBeginX + iRectLen;
	m_Rect_World[3].push_back( pTemp);

	pTemp.x = iBeginX;
	pTemp.y = iBeginY + iRectLen;
	m_Rect_World[3].push_back( pTemp );

	pTemp.x = iBeginX + iRectLen;
	pTemp.y = iBeginY + iRectLen;
	m_Rect_World[3].push_back( pTemp );

}

// 获取车辆在世界坐标系上的起始位置
void C360LookAroundMontageAlg::OnGetCarBeginPointWorldCoordinate_touringCar()
{
		char* sect;
	char* key;
	int   iTemp1;
	int   iTemp2;

	sect = "commonStyle";
	key = "rectToBoundary";

	/*m_CarBeginPointWorld.x*/iTemp1 = iniGetInt( sect , key , 100 );

	key = "rectWide";
	iTemp2 = iniGetInt( sect , key , 100 );

	m_CarBeginPointWorld.x = 2*iTemp1 + iTemp2;

	m_CarBeginPointWorld.y = 2*iTemp1 + iTemp2;
}

// 获取图像显示在世界坐标系上的起始位置(BX7)
void C360LookAroundMontageAlg::OnGetImgBeginPointWorldCoordinate()
{
	char* sect;
	char* key;
	int   checkBoardWide;
	int   checkBoardHeight;

	sect = "checkboardINF";
	key = "checkBoardWideX";

	/*m_CarBeginPointWorld.x*/checkBoardWide = iniGetInt( sect , key , 100 );

	m_fBeginOriginX = (checkBoardWide - m_p360Param->world_show_size.width)/2;

	key = "checkBoardHeight";
	checkBoardHeight = iniGetInt( sect , key , 100 );

	m_fBeginOriginY = (checkBoardHeight - m_p360Param->world_show_size.height)/2;


}

// 获取图像显示在世界坐标系上的起始位置（房车）
void C360LookAroundMontageAlg::OnGetImgBeginPointWorldCoordinate_touringCar()
{
	char* sect;
	char* key;
	int   checkBoardWide;
	int   checkBoardHeight;

	sect = "commonStyle";
	key = "checkBoardWideX";

	/*m_CarBeginPointWorld.x*/checkBoardWide = iniGetInt( sect , key , 100 );

	m_fBeginOriginX = (checkBoardWide - m_p360Param->world_show_size.width)/2;

	key = "checkBoardHeight";
	checkBoardHeight = iniGetInt( sect , key , 100 );

	m_fBeginOriginY = (checkBoardHeight - m_p360Param->world_show_size.height)/2;
}

// 从inf配置文件中读取配置信息
bool C360LookAroundMontageAlg::OnStructWorldCoordinate( const calibrateScheme calibrateSch )
{
	int bRetrun = 0;
	const char*file = "config.ini";          // 配置文件名和路径

	int iTemp = 0;                           // 临时变量

	//const char* sect;
	//const char* key;
	bRetrun = iniFileLoad( file );

	if ( !bRetrun )
	{
		cout<<"config.ini load error!"<<endl;
		return false;
	}

	//////////////////////////////////////////////////////////////////////////
	// 房车样式选择（因为我们做了两套棋盘格）
	if ( calibrateSch == TOURING_CAR )
	{
		

		m_touringCarMode = OnGetTouringCarMode();

		m_speed_process[0]++;

		//////////////////////////////////////////////////////////////////////////
		// 获取前摄像头的角点对应的世界坐标系坐标
		OnGetFrontCameraWorldCoordinate_touringCar();
		//////////////////////////////////////////////////////////////////////////
		m_speed_process[0]++;
		//////////////////////////////////////////////////////////////////////////
		// 获取后摄像头的角点对应的世界坐标系坐标
		OnGetBottomCameraWorldCoordinate_touringCar();
		//////////////////////////////////////////////////////////////////////////
		m_speed_process[0]++;
		//////////////////////////////////////////////////////////////////////////
		// 获取左摄像头的角点对应的世界坐标系坐标
		OnGetLeftCameraWorldCoordinate_touringCar( m_touringCarMode );
		//////////////////////////////////////////////////////////////////////////
		m_speed_process[0]++;
		//////////////////////////////////////////////////////////////////////////
		// 获取右摄像头的角点对应的世界坐标系坐标
		OnGetRightCameraWorldCoordinate_touringCar( m_touringCarMode );
		//////////////////////////////////////////////////////////////////////////
		m_speed_process[0]++;
		//////////////////////////////////////////////////////////////////////////
		// 获取四个正方形的角点在世界坐标系上的坐标
		OnGetBigRectWorldCoordinate_touringCar();
		//////////////////////////////////////////////////////////////////////////
		m_speed_process[0]++;
		//////////////////////////////////////////////////////////////////////////
		// 获取车辆在世界坐标系中的起始位置
		OnGetCarBeginPointWorldCoordinate_touringCar();
		//////////////////////////////////////////////////////////////////////////
		m_speed_process[0]++;

		//////////////////////////////////////////////////////////////////////////
		// 获取图像显示在世界坐标系上的起始位置
		OnGetImgBeginPointWorldCoordinate_touringCar();
		//////////////////////////////////////////////////////////////////////////
		m_speed_process[0]++;
	}
	else
	{
		m_touringCarMode = -1;

		//////////////////////////////////////////////////////////////////////////

		//////////////////////////////////////////////////////////////////////////
		// 获取前摄像头的角点对应的世界坐标系坐标
		OnGetFrontCameraWorldCoordinate();
		//////////////////////////////////////////////////////////////////////////
		m_speed_process[0]++;

		//////////////////////////////////////////////////////////////////////////
		// 获取后摄像头的角点对应的世界坐标系坐标
		OnGetBottomCameraWorldCoordinate();
		//////////////////////////////////////////////////////////////////////////

		m_speed_process[0]++;

		//////////////////////////////////////////////////////////////////////////
		// 获取左摄像头的角点对应的世界坐标系坐标
		OnGetLeftCameraWorldCoordinate();
		//////////////////////////////////////////////////////////////////////////
		m_speed_process[0]++;

		//////////////////////////////////////////////////////////////////////////
		// 获取右摄像头的角点对应的世界坐标系坐标
		OnGetRightCameraWorldCoordinate();
		//////////////////////////////////////////////////////////////////////////
		m_speed_process[0]++;
		//////////////////////////////////////////////////////////////////////////
		// 获取四个正方形的角点在世界坐标系上的坐标
		OnGetBigRectWorldCoordinate();
		//////////////////////////////////////////////////////////////////////////
		m_speed_process[0]++;
		//////////////////////////////////////////////////////////////////////////
		// 获取车辆在世界坐标系中的起始位置
		OnGetCarBeginPointWorldCoordinate();
		//////////////////////////////////////////////////////////////////////////
		m_speed_process[0]++;

		//////////////////////////////////////////////////////////////////////////
		// 获取图像显示在世界坐标系上的起始位置
		OnGetImgBeginPointWorldCoordinate();
		m_speed_process[0]++;
	}

	iniFileFree();

	return true;
}

// 获取车辆在世界坐标系上的起始位置
void C360LookAroundMontageAlg::OnGetCarBeginPointWorldCoordinate()
{
	char* sect;
	char* key;
	int   iTemp1;
	int   iTemp2;

	sect = "checkboardINF";
	key = "rectToBoundary";

	/*m_CarBeginPointWorld.x*/iTemp1 = iniGetInt( sect , key , 100 );

	key = "rectWide";
	iTemp2 = iniGetInt( sect , key , 100 );

	m_CarBeginPointWorld.x = 2*iTemp1 + iTemp2;

	m_CarBeginPointWorld.y = 2*iTemp1 + iTemp2;
	//iRectToBoundary = iniGetInt( sect , key , 100 );
}
// 获取大正方形的四个点的坐标
void C360LookAroundMontageAlg::OnGetBigRectWorldCoordinate()
{
	//////////////////////////////////////////////////////////////////////////
	// 获取左上角正方形顶点在世界坐标系上的坐标
	const char* sect;
	const char* key;
	cv::Point3f pTemp;
	int iBeginX = 0;
	int iBeginY = 0;
	int iRectLen = 0;
	int iRectToBoundary = 0;

	int iTemp1 = 0;
	int iTemp2 = 0;

	sect = "checkboardINF";
	key = "rectToBoundary";

	iRectToBoundary = iniGetInt( sect , key , 100 );
	//iRectToBoundary = iniGetInt( sect , key , 100 );
	iBeginX = iRectToBoundary;
	iBeginY = iRectToBoundary;
	pTemp.x = iBeginX;
	pTemp.y = iBeginY;
	pTemp.z = 0;

	m_Rect_World[0].push_back( pTemp );

	// 获取大正形的宽度
	key = "rectWide";
	iRectLen = iniGetInt( sect , key , 100 );
	pTemp.x = iBeginX + iRectLen;

	m_Rect_World[0].push_back( pTemp );

	pTemp.x = iBeginX;
	pTemp.y = iBeginY + iRectLen;
	m_Rect_World[0].push_back( pTemp );

	pTemp.x = iBeginX + iRectLen;
	pTemp.y = iBeginY + iRectLen;
	m_Rect_World[0].push_back( pTemp );
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	// 获取右上角顶点在世界坐标上的坐标
	key = "checkBoardWideX";
	iBeginX = iniGetInt( sect , key , 100 );

	iBeginX = iBeginX - iRectToBoundary;
	iBeginX = iBeginX - iRectLen;
	iBeginY = iRectToBoundary;

	iTemp1 = iBeginX;

	pTemp.x = iBeginX;
	pTemp.y = iBeginY;
	m_Rect_World[1].push_back( pTemp );

	pTemp.x = iBeginX + iRectLen;
	m_Rect_World[1].push_back( pTemp );

	pTemp.x = iBeginX;
	pTemp.y = iBeginY + iRectLen;
	m_Rect_World[1].push_back( pTemp );

	pTemp.x = iBeginX + iRectLen;
	pTemp.y = iBeginY + iRectLen;
	m_Rect_World[1].push_back( pTemp );
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	// 获取左下角顶点在世界坐标上的坐标
	key = "checkBoardHeight";
	iBeginX = iRectToBoundary;
	iBeginY = iniGetInt( sect , key , 100 );
	iBeginY = iBeginY - iRectToBoundary;
	iBeginY = iBeginY - iRectLen;

	iTemp2 = iBeginY;

	pTemp.x = iBeginX;
	pTemp.y = iBeginY;
	m_Rect_World[2].push_back( pTemp );

	pTemp.x = iBeginX + iRectLen;
	m_Rect_World[2].push_back( pTemp );

	pTemp.x = iBeginX;
	pTemp.y = iBeginY + iRectLen;
	m_Rect_World[2].push_back( pTemp );

	pTemp.x = iBeginX + iRectLen;
	pTemp.y = iBeginY + iRectLen;
	m_Rect_World[2].push_back( pTemp );
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	// 获取右下角顶点在世界坐标上的坐标
	iBeginX = iTemp1;
	iBeginY = iTemp2;

	pTemp.x = iBeginX;
	pTemp.y = iBeginY;
	m_Rect_World[3].push_back( pTemp );

	pTemp.x = iBeginX + iRectLen;
	m_Rect_World[3].push_back( pTemp);

	pTemp.x = iBeginX;
	pTemp.y = iBeginY + iRectLen;
	m_Rect_World[3].push_back( pTemp );

	pTemp.x = iBeginX + iRectLen;
	pTemp.y = iBeginY + iRectLen;
	m_Rect_World[3].push_back( pTemp );
}

// 前摄像头对应的角点世界坐标系
void C360LookAroundMontageAlg::OnGetFrontCameraWorldCoordinate()
{
	// 0
	CvSize temp;//(23 , 25 );
	int    iBeginX = 0;         // 第一个角点的起点X坐标
	int    iBeginY = 0;         // 第一个角点的起点Y坐标
	int    iTemp = 0;
	const char* sect;
	const char* key;

	sect = "checkboardINF";
	key = "Top_Bottom_wideDimension";
	temp.width = iniGetInt(sect , key , 20 ) - 1;

	key = "Top_Bottom_heightDimension";
	temp.height = iniGetInt( sect , key , 10) - 1;

	//////////////////////////////////////////////////////////////////////////
	// 获取第一个角点的起始X坐标
	key = "rectToBoundary";
	iTemp = iniGetInt( sect , key , 10 );
	iTemp = iTemp*2;

	key = "rectWide";
	iBeginX = iniGetInt( sect , key , 10 );

	iBeginX = iBeginX + iTemp;

	key = "checkboardWide";
	iTemp = iniGetInt( sect , key , 10 );
	iBeginX = iBeginX + iTemp;

	// 第一个角点的起始Y坐标
	key = "Top_Bottom_checkboardToBoundary";
	iBeginY = iniGetInt( sect , key , 10 );
	iBeginY = iBeginY + iTemp;
	//////////////////////////////////////////////////////////////////////////

	

	m_Calibrate[0].OnSetCornerWorldLocation( iBeginX , iBeginY , temp , iTemp , iTemp , TOP , true );
}

// 后摄像头对应的角点世界坐标系
void C360LookAroundMontageAlg::OnGetBottomCameraWorldCoordinate()
{
	// 1
	CvSize temp;//(23 , 25 );
	//temp.width = 23;
	//temp.height = 25;
	int iRectLen = 0;

	int    iBeginX = 0;         // 第一个角点的起点X坐标
	int    iBeginY = 0;         // 第一个角点的起点Y坐标
	int    iTemp = 0;
	const char* sect;
	const char* key;

	sect = "checkboardINF";
	key = "Top_Bottom_wideDimension";
	temp.width = iniGetInt(sect , key , 20 ) - 1;

	key = "Top_Bottom_heightDimension";
	temp.height = iniGetInt( sect , key , 20 ) - 1;

	//////////////////////////////////////////////////////////////////////////
	// 获取第一个角点的X坐标
	key = "rectToBoundary";
	iTemp = iniGetInt( sect , key , 20 );

	iTemp = iTemp*2;
	key = "rectWide";
	iBeginX = iniGetInt( sect , key , 200 );

	iBeginX = iBeginX + iTemp;

	key = "checkboardWide";
	iTemp = iniGetInt( sect , key , 200 );              // 棋盘格的大小，以毫米为单位
	iRectLen = iTemp;                         // 存储每个小棋盘格的大小
	iBeginX = iBeginX + iTemp;                // 第一个角点的X坐标
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	// 获取第一个角点的Y坐标
	key = "checkBoardHeight";
	iTemp = iniGetInt( sect , key , 200 );         // 获取整张棋盘的高度

	key = "Top_Bottom_checkboardToBoundary";
	iBeginY = iniGetInt( sect , key , 200 );

	iBeginY = iTemp - iBeginY;
	iBeginY = iBeginY - temp.height*iRectLen;
	//////////////////////////////////////////////////////////////////////////

	m_Calibrate[1].OnSetCornerWorldLocation( iBeginX , iBeginY , temp , iRectLen , iRectLen , BOTTOM , true );
}

// 左摄像头对应的角点世界坐标系
void C360LookAroundMontageAlg::OnGetLeftCameraWorldCoordinate()
{
	// 2
	CvSize temp;//(23 , 25 );
//	temp.width = 23;
//	temp.height = 25;

	int    iBeginX = 0;         // 第一个角点的起点X坐标
	int    iBeginY = 0;         // 第一个角点的起点Y坐标
	int    iTemp = 0;
	int    iRectLen = 0;
	const char* sect;
	const char* key;

	sect = "checkboardINF";
	//////////////////////////////////////////////////////////////////////////
	// 获取左棋盘的维度
	key = "Left_Right_wideDimension";
	temp.width = iniGetInt( sect , key , 100 ) - 1;

	key = "Left_Right_heightDimension";
	temp.height = iniGetInt( sect , key , 100 ) - 1;

	// 获取小棋盘格的大小，以毫米为单位
	key = "checkboardWide";
	iRectLen = iniGetInt( sect , key , 100 );

	//////////////////////////////////////////////////////////////////////////
	// 获取第一个角点的X坐标
	key = "Left_Right_checkboardToBoundary";
	iBeginX = iniGetInt( sect , key , 100 );

	iBeginX = iBeginX + iRectLen;
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	// 获取第一个角点的Y坐标
	key = "frontAxle";
	iBeginY = iniGetInt( sect , key , 100 );
	iBeginY = iBeginY + iRectLen;
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	m_Calibrate[2].OnSetCornerWorldLocation( iBeginX , iBeginY , temp , iRectLen , iRectLen , LEFT , true );
}

// 右摄像头对应的角点世界坐标系
void C360LookAroundMontageAlg::OnGetRightCameraWorldCoordinate()
{
	// 3
	CvSize temp;//(23 , 25 );
	//temp.width = 23;
	//temp.height = 25;

	int    iBeginX = 0;         // 第一个角点的起点X坐标
	int    iBeginY = 0;         // 第一个角点的起点Y坐标
	int    iTemp = 0;
	int    iRectLen = 0;
	const char *sect;
	const char *key;

	//////////////////////////////////////////////////////////////////////////
	// 获取棋盘的维度
	sect = "checkboardINF";
	key = "Left_Right_wideDimension";
	temp.width = iniGetInt( sect , key , 100 ) - 1;

	key = "Left_Right_heightDimension";
	temp.height = iniGetInt( sect , key , 100 ) - 1;
	//////////////////////////////////////////////////////////////////////////

	// 小棋盘格的大小
	key = "checkboardWide";
	iRectLen = iniGetInt( sect , key , 100 );

	//////////////////////////////////////////////////////////////////////////
	// 右棋盘的起点X
	key = "checkBoardWideX";
	iBeginX = iniGetInt( sect , key , 100 );

	key = "Left_Right_checkboardToBoundary";
	iTemp = iniGetInt( sect , key , 100 );

	iBeginX = iBeginX - iTemp;

	iBeginX = iBeginX - temp.width*iRectLen;
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	// 左棋盘的起点Y
	key = "frontAxle";
	iBeginY = iniGetInt( sect , key , 100 );
	iBeginY = iBeginY + iRectLen;
	//////////////////////////////////////////////////////////////////////////

	m_Calibrate[3].OnSetCornerWorldLocation( iBeginX , iBeginY , temp , iRectLen , iRectLen , RIGHT , true );

}

// 返回进度值
int C360LookAroundMontageAlg::OnGetSpeedProcess()
{
	return m_speed_process[0];
}

void C360LookAroundMontageAlg::WriteCamParam()
{
	// 将4个鱼眼的内外参和畸变系数写到文件里面，方便车道线优化后，直接修改外参偏移值，重新生成表，而不需要再标定。
	int size = FISHEYE_COUNT * (m_Calibrate[0].m_fishInf.innerParam.channels + 2 * m_Calibrate[0].m_fishInf.RT[0].channels + m_Calibrate[0].m_fishInf.distortionParam.channels);
	double *data = new double[size];
	double *pdata = data;

	for (int i = 0; i < FISHEYE_COUNT; i++)
	{
		for (int j = 0; j < m_Calibrate[0].m_fishInf.innerParam.channels; j++)
			*pdata++ = m_Calibrate[i].m_fishInf.innerParam.val[j];

		for (int k = 0; k < 2; k++)
			for (int j = 0; j < m_Calibrate[0].m_fishInf.RT[0].channels; j++)
				*pdata++ = m_Calibrate[i].m_fishInf.RT[k].val[j];

		for (int j = 0; j < m_Calibrate[0].m_fishInf.distortionParam.channels; j++)
			*pdata++ = m_Calibrate[i].m_fishInf.distortionParam.val[j];
	}

	FILE*filex = NULL;

	if (filex = fopen("tempimg//fish_param.bin", "wb"))
	{
		fwrite(data, sizeof(cv::Point), size, filex);
		fclose(filex);
	}

	delete[]data;
}

// 标定线程
void calibrate(int i, calibrateScheme *calibScheme, CCalibrate *calibrate, 
	cv::Mat *fishEyeImg, vector<cv::Point3f> *rect_World, errorCode *retErrorCode, int *speed_process, int *hadexit)
{
	bool bReturn = false;
	vector<vector<cv::Point3f> > pRect;

	pRect.clear();

	if ( i == 0 )                                   // 前图
	{
		pRect.push_back( rect_World[0] );         // 左上
		pRect.push_back( rect_World[1] );         // 右上
	}
	else if ( i == 1 )                                   // 后图
	{
		pRect.push_back( rect_World[2] );         // 左下
		pRect.push_back( rect_World[3] );         // 右下
	}
	else if ( i == 2 )                                   // 左图
	{
		pRect.push_back( rect_World[0] );         // 左上正方形
		pRect.push_back( rect_World[2] );         // 左下正方形
	}
	else if ( i == 3 )
	{
		pRect.push_back( rect_World[1] );         // 右上正方形
		pRect.push_back( rect_World[3] );         // 右下正方形
	}

	cout<<__LINE__<<"---------------BEFORE OnCalibrateX["<<i<<"]---------------\n";
	calibrate->SetCalibrate( *calibScheme );
	bReturn = calibrate->OnCalibrateX(*fishEyeImg, pRect, *speed_process);
	if ( !bReturn )
	{
		*retErrorCode = FIND_CORNER_ERROR;
	}	
	cout<<__LINE__<<"---------------AFTER OnCalibrateX["<<i<<"]---------------\n";

	//sleep(10);

	*hadexit = 1;
}


// 初使化，若初始化成功，返回true。否则返回false
bool C360LookAroundMontageAlg::OnCalibrate( void* pParam )
{
	bool bReturn = false;
	int i=0;
	errorCode error_Code = NO_ERROR_INFO;            // 错误码
	vector<vector<cv::Point3f> > pRect;

	m_p360Param = (stu_360Param*)pParam;

	m_speed_process = m_p360Param->iProcess;

	m_speed_process[0] = 0;

//	m_speed_process = 0;
	//////////////////////////////////////////////////////////////////////////
	// 通过车辆长度来确定是房车还是宝沃BX7
	// 因为这两辆车的长度不一样
	if ( m_p360Param->world_car_size.height < CAR_DIVISION )           // 宝沃
	{
		m_CalibrateScheme = BAOWO_BX7;
	}
	else                                                               // 房车
	{
		m_CalibrateScheme = TOURING_CAR;
	}

	////////////////////////////////////////////////////////////////////////////
	//// 测试用,假设这一定是房车,到时候会删的
	//m_CalibrateScheme = TOURING_CAR;
	////////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	// 获取世界坐标系坐标及棋盘格维度
	bReturn = OnStructWorldCoordinate( m_CalibrateScheme );

	if ( !bReturn )
	{
		m_errorCode = READ_INIFILE_ERROR;

		return bReturn;
	}
	//////////////////////////////////////////////////////////////////////////
	// 先保存四幅原始的鱼眼镜头，如果标定失败，可以将原始图像拷贝出来
//#if DEBUG_SHOW_IMAGE
	// 将世界坐标系的点和图像坐标系上的点存储成文件，查看世界坐标系的点与图像坐标系的点是否是对应的
	for ( i = 0 ; i<FISHEYE_COUNT ; i++ )
	{
		std::stringstream StrStm;
		string imageFileName;
		StrStm.clear();
		StrStm<<"tempimg//"<<i<<"_source.bmp";
		StrStm>>imageFileName;
		imwrite(imageFileName,m_p360Param->fishEyeImg[i]);
	}


//#endif

	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	// 标定
#ifdef SINGLE_THREAD
	for ( i = 0 ; i<FISHEYE_COUNT ; i++ )
	{
		//////////////////////////////////////////////////////////////////////////
		// 查找图像坐标系上的角点，以及正方形的顶点
		pRect.clear();

		if ( i == 0 )                                   // 前图
		{
			pRect.push_back( m_Rect_World[0] );         // 左上
			pRect.push_back( m_Rect_World[1] );         // 右上
		}

		if ( i == 1 )                                   // 后图
		{
			pRect.push_back( m_Rect_World[2] );         // 左下
			pRect.push_back( m_Rect_World[3] );         // 右下
		}

		if ( i == 2 )                                   // 左图
		{
			pRect.push_back( m_Rect_World[0] );         // 左上正方形
			pRect.push_back( m_Rect_World[2] );         // 左下正方形
		}

		if ( i == 3 )
		{
			pRect.push_back( m_Rect_World[1] );         // 右上正方形
			pRect.push_back( m_Rect_World[3] );         // 右下正方形
		}
		//////////////////////////////////////////////////////////////////////////

		//////////////////////////////////////////////////////////////////////////
		// 标定
		cout<<__LINE__<<"---------------BEFORE OnCalibrateX["<<i<<"]---------------\n";
		m_Calibrate[i].SetCalibrate( m_CalibrateScheme );
		bReturn = m_Calibrate[i].OnCalibrateX( m_p360Param->fishEyeImg[i] , pRect , m_speed_process[0] );
		cout<<__LINE__<<"---------------AFTER OnCalibrateX["<<i<<"]---------------\n";
		if ( !bReturn )
		{
			m_errorCode = FIND_CORNER_ERROR;
			return bReturn;
		}

		// 记录图像坐标与世界坐标的对应
		pRectS[i] = pRect;         // 世界坐标
		pRectSImg[i] = m_Calibrate[i].OnGetRect_Img();
		//////////////////////////////////////////////////////////////////////////
	}
	
	#else
	int	iCalThreadHadExit[4] = { -1, -1, -1, -1 };

	// 创建并运行线程
	std::thread calibrate1(calibrate, 0, &m_CalibrateScheme, m_Calibrate, m_p360Param->fishEyeImg, m_Rect_World, &error_Code, m_speed_process, iCalThreadHadExit);
	// 将线程设置为分离模式
	calibrate1.detach();

	std::thread calibrate2(calibrate, 1, &m_CalibrateScheme, m_Calibrate + 1, m_p360Param->fishEyeImg + 1, m_Rect_World, &error_Code, m_speed_process, iCalThreadHadExit + 1);
	calibrate2.detach();

	std::thread calibrate3(calibrate, 2, &m_CalibrateScheme, m_Calibrate + 2, m_p360Param->fishEyeImg + 2, m_Rect_World, &error_Code, m_speed_process, iCalThreadHadExit + 2);
	calibrate3.detach();

	std::thread calibrate4(calibrate, 3, &m_CalibrateScheme, m_Calibrate + 3, m_p360Param->fishEyeImg + 3, m_Rect_World, &error_Code, m_speed_process, iCalThreadHadExit + 3);
	calibrate4.detach();

	// 标定成功退出循环；如果标定时间超过10s，则认为标定失败。
	// 对于比较复杂的标定环境，需要较多的标定时间，特别是对于嵌入式而言
	int timeout = 25;
	while (--timeout)
	{
		if (1 == iCalThreadHadExit[0])
		{
			iCalThreadHadExit[0] = 0;
		}
		
		if (1 == iCalThreadHadExit[1])
		{
			iCalThreadHadExit[1] = 0;
		}
		
		if (1 == iCalThreadHadExit[2])
		{
			iCalThreadHadExit[2] = 0;
		}
		
		if (1 == iCalThreadHadExit[3])
		{
			iCalThreadHadExit[3] = 0;
		}

		if (0 == iCalThreadHadExit[3] && 0 == iCalThreadHadExit[2] && 0 == iCalThreadHadExit[1] && 0 == iCalThreadHadExit[0])
		{
			iCalThreadHadExit[0] = -1;
			iCalThreadHadExit[1] = -1;
			iCalThreadHadExit[2] = -1;
			iCalThreadHadExit[3] = -1;		
			break;
		}

#ifdef _WIN32
		Sleep(1 * 1000);
#else
		sleep(1);
#endif
	}

	m_errorCode = error_Code;
	if (0 == timeout)
	{
		m_errorCode = CABICATE_TIMEOUT;
		printf("calibrate time out!\n");
		return false;
	}

	if(m_errorCode == FIND_CORNER_ERROR) 
	{
		return false;
	}
		
	#endif

#if 0
	std::stringstream StrStm;
	string storegePath;

	StrStm.clear();
	StrStm << "tempimg//Calibrate_Param.txt";
	StrStm >> storegePath;
	ofstream foutmm(storegePath);

	for (int i = 0; i < 2; i++)
	{
		foutmm << m_Calibrate[i].m_fishInf.innerParam << endl;
		foutmm << m_Calibrate[i].m_fishInf.distortionParam/*distortion_coeffs*/ << endl;

		for (int j = 0; j < 2; j++)
		{
			foutmm << m_Calibrate[i].m_fishInf.RT[j] << endl;			
		}
	}
#endif

	m_bCalibrated = true;                                        // 若调用了OnCalibrate的话，说明已经标定完成且成功

	WriteCamParam();

	return bReturn;
}

// 不仅生成查找表，而且也生成一张俯视图片模板
bool C360LookAroundMontageAlg::OnGenerateTable( int wide , int height , int iSpreadWide , int iSpreadHeight )
{
	bool bReturn = false;

	if ( !m_bCalibrated )          // 未完成标定，所以不能生成查找表
	{
		m_errorCode = NOT_CABICATE;

		return bReturn;
	}

	// 8*m_iCheckboardWide表示模型车放的起点
	if ( ((m_CarBeginPointWorld.x - m_fBeginOriginX)<=10)||((m_CarBeginPointWorld.y - m_fBeginOriginY)<=10 ) )
	{
		m_errorCode = SHOW_ORIGIN_ERROR;
		return bReturn;
	}

	//m_Calibrate[0].m_fishInf.RT[0].val[3] += 150;
	//m_Calibrate[1].m_fishInf.RT[0].val[3] -= 150;
	//m_Calibrate[1].m_fishInf.RT[1].val[3] += 120;

    cout<<__LINE__<<"---------------BEFORE C360LookAroundMontageAlg::OnGenerateTable() -> CGenerateTable generateTable( -----------\n";
	CGenerateTable generateTable( m_Calibrate , FISHEYE_COUNT ,m_iSourceWide , m_iSourceHeight );
    cout<<__LINE__<<"---------------BEFORE C360LookAroundMontageAlg::OnGenerateTable() -> CGenerateTable generateTable( -----------\n";
	//////////////////////////////////////////////////////////////////////////
	// 从文件中读取车辆模型,生成俯视图模板
	// 生成俯视图模板
	// 200代表每个小正方形在世界坐标系上的大小，以毫米为单位。下一步会从配置文件中读取
    cout<<__LINE__<<"---------------BEFORE C360LookAroundMontageAlg::OnGenerateTable() -> bReturn = OnGenerateModelImg( -----------\n";
	bReturn = OnGenerateModelImg( wide , height , m_p360Param->world_show_size  , m_p360Param->world_car_size , m_CarBeginPointWorld.x/*200*/ -m_fBeginOriginX , m_CarBeginPointWorld.y/*200*/ - m_fBeginOriginY );
    cout<<__LINE__<<"---------------AFTER C360LookAroundMontageAlg::OnGenerateTable() -> bReturn = OnGenerateModelImg( -----------\n";

	if ( !bReturn )
	{
		return bReturn;
	}

	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	// 生成查找表
	stu2dTable* p2dTable = new stu2dTable[wide * height];
    cout<<__LINE__<<"---------------BEFORE OnGenerateInTable-----------\n";
	bReturn = generateTable.OnGenerateInTable( p2dTable, m_p360Param->world_show_size , m_fBeginOriginX , m_fBeginOriginY , m_p360Param->world_car_size , m_CarBeginPointWorld.x - m_fBeginOriginX , m_CarBeginPointWorld.y - m_fBeginOriginY  , wide , height );
    cout<<__LINE__<<"---------------AFTER OnGenerateInTable-----------\n";
	if ( !bReturn )
	{
		m_errorCode = GENERATE_TABLE_ERROR;
		return bReturn;
	}
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	// 对前后的图像生成展开图
	cv::Size tSize;
	tSize.width = iSpreadWide;
	tSize.height = iSpreadHeight;
	stu_spreadImg* pImg = new stu_spreadImg[iSpreadHeight*iSpreadWide];
//#ifdef 1 //SPREED_VIEW
	memset( pImg , 0 , iSpreadWide*iSpreadHeight*sizeof(stu_spreadImg) );
	generateTable.OnGenerateCorrectionImg( TOP , tSize , pImg);              // 对前图像进行展开生成查找表

	memset( pImg , 0 , iSpreadWide*iSpreadHeight*sizeof(stu_spreadImg) );
	generateTable.OnGenerateCorrectionImg( BOTTOM , tSize , pImg );           // 对后图像进行展开生成查找表
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	// 对后视的图像生成倒车辅助点
//	OnGetSpreedBackAssistPoint(height, wide, p2dTable, pImg);
//#else
	//OnGetRawBackAssistPoint(height, wide, p2dTable);
//#endif
	//////////////////////////////////////////////////////////////////////////	

	//////////////////////////////////////////////////////////////////////////
	// 将俯视表转换为三张表，供openGL使用
    cout<<__LINE__<<"---------------BEFORE writing new-format table-----------\n";
	convert_tbl(p2dTable);
    cout<<__LINE__<<"---------------AFTER writing new-format table-----------\n";
    //////////////////////////////////////////////////////////////////////////

	delete []p2dTable;
	p2dTable = NULL;
	delete []pImg;
	pImg = NULL;
	
	return bReturn;
}

// 不仅生成查找表，而且也生成一张俯视图片模板
bool C360LookAroundMontageAlg::OnGenerate3DTable(int nx, int ny)
{
	bool bReturn = false;

	CGenerate3Dtable generate3DTable;
	G3DtableParam p3DtableParam;

	p3DtableParam.CalibrateParam = m_Calibrate;
	p3DtableParam.world_show_size = m_p360Param->world_show_size;
	p3DtableParam.worldcarSize = m_p360Param->world_car_size;
	p3DtableParam.fWorldCarBeginX = m_CarBeginPointWorld.x;
	p3DtableParam.fWorldCarBeginY = m_CarBeginPointWorld.y;
//////////////////////////////////////////////////////////////////////////
	// 老版本生成3D的代码
#if NEW3D
	p3DtableParam.pRectW = pRectS;
	p3DtableParam.pRectWImg = pRectSImg;
	generate3DTable.OnGenerateNew3Dtable( &p3DtableParam );
#else
	generate3DTable.OnGenerate3Dtable(&p3DtableParam);
#endif
	//////////////////////////////////////////////////////////////////////////

	return bReturn;
}

bool C360LookAroundMontageAlg::OnGenerateModelImg( int wide , int height , CvSize2D32f worldShowSize ,  CvSize2D32f worldCarSize  , float fWorldCarBeginX , float fWorldCarBeginY )
{
//	bool bReturn = true;
	cv::Mat ModelImg;
	m_widePerPixel = worldShowSize.width/wide;               // 每一像素点代表世界坐标系多长的宽度，下同
	m_heightPerPixel = worldShowSize.height/height;
	m_iCarWide = (int)(worldCarSize.width/m_widePerPixel);
	m_iCarHeight = (int)(worldCarSize.height/m_heightPerPixel );

	//////////////////////////////////////////////////////////////////////////
	// 小车在世界坐标系中的起点
	m_iCarBeginX = (int)(fWorldCarBeginX/m_widePerPixel);
	m_iCarBeginY = (int)(fWorldCarBeginY/m_heightPerPixel);

	if ( ((m_iCarBeginX + m_iCarWide)>=wide)||((m_iCarBeginY+m_iCarHeight)>=height)  )         // 小车的大小超出界限
	{
		m_errorCode = CAR_REGION_ERROR;

		return false;
	}

	ModelImg.create( height , wide , CV_8UC3 );
	ModelImg.setTo(cv::Scalar(0, 0, 0));
	cout<<__LINE__<<"---------------BEFORE 360LookAroundMontageAlg::OnGenerateModelImg() ->Mat Car = imread( \"car.bmp\" );-----------\n";

	cv::Rect rect(m_iCarBeginX, m_iCarBeginY, m_iCarWide, m_iCarHeight);

#if 1
	cv::Mat Car = cv::imread("car.bmp");
	cv::Mat smallCar;
	resize(Car, smallCar, cv::Size(m_iCarWide, m_iCarHeight));

	
	smallCar.copyTo( ModelImg(rect) );
	//imshow("zhou" , ModelImg );
	//waitKey(0);

	imwrite( "modelCar.bmp" , ModelImg );
    cout<<__LINE__<<"---------------AFTER 360LookAroundMontageAlg::OnGenerateModelImg() -> imwrite( \"modelCar.bmp\" , ModelImg );-----------\n";
#endif

	ofstream outf; 
	outf.open("car.txt");
	outf<<m_iCarBeginX<<','<<m_iCarBeginY<<','<<m_iCarWide<<','<<m_iCarHeight<<endl;
	outf.close();

	return true;
}

// 释放资源
bool C360LookAroundMontageAlg::OnRelease( void* pParam )
{
	return true;
}

// fx,fy,cx,cy和k都是返回的参数
bool C360LookAroundMontageAlg::GetInnerParam( double&fx, double&fy , double&cx , double&cy , double k[KNUM] , cameraLocation cameraIndex )
{
	return true;
}

// 获取外参，返回
// 返回的单个摄像机外参个数，rRTLen最长个数为16个（一维存储，连续）
bool C360LookAroundMontageAlg::GetOuterParam( double RT0[12] , double RT1[12]  , cameraLocation cameraIndex )
{
	return true;
}

// 获取平面俯视图，生成查找表
// 对应的是鱼眼原始图像上的哪一个点坐标
// pCoords生成的是图像俯视的坐标表
// lut_w查找表的宽，以像素为单位
// lut_h查找表的高，以像素为单位
bool C360LookAroundMontageAlg::GenTable2d(float* pAlpha, float* pCoords, int lut_w, int lut_h )
{
	return true;
}
// 获取错误码
errorCode C360LookAroundMontageAlg::Get360AlgError()
{
	return m_errorCode;
}

// 获取展开图
void C360LookAroundMontageAlg::OnGetSpreadImg(cv::Mat src, stu_spreadImg*pTable, cv::Mat&dst)
{
	for ( int i = 0 ; i<dst.rows; i++ )
	{
		for ( int j = 0 ; j<dst.cols ; j++ )
		{

			if ( ((pTable[i*dst.cols + j].iWidex < 0)||(pTable[i*dst.cols + j].iHeighty<0))||(pTable[i*dst.cols + j].iWidex>=src.cols)||(pTable[i*dst.cols + j].iHeighty>=src.rows) )
			{
				continue;
			}

			for ( int z = 0 ; z<3 ; z++ )
			{
				dst.data[i*dst.cols*3 + j*3 + z] = src.data[pTable[i*dst.cols + j].iHeighty*src.cols*3 + pTable[i*dst.cols + j].iWidex*3 + z];
			}

		}
	}
}

// 获取带双线性插值的展开图
void C360LookAroundMontageAlg::OnGetInterpSpreadImg(cv::Mat src, interpSpreadTable* pTable, cv::Mat &dst)
{
	double value;
	ushort x1, y1;
	ushort x2, y2;

	for (int i = 0; i<dst.rows; i++)
	{
		for (int j = 0; j<dst.cols; j++)
		{
			x1 = pTable[i*dst.cols + j].LTpoint.iWidex;
			y1 = pTable[i*dst.cols + j].LTpoint.iHeighty;

			x2 = x1 + 1;
			y2 = y1 + 1;

			for (int z = 0; z<3; z++)
			{
				value = pTable[i*dst.cols + j].ucLTWeight * src.data[y1 * src.cols * 3 + x1 * 3 + z] / 255;
				value += pTable[i*dst.cols + j].ucRTWeight * src.data[y1 * src.cols * 3 + x2 * 3 + z] / 255;
				value += pTable[i*dst.cols + j].ucLBWeight * src.data[y2 * src.cols * 3 + x1 * 3 + z] / 255;
				value += pTable[i*dst.cols + j].ucRBWeight * src.data[y2 * src.cols * 3 + x2 * 3 + z] / 255;
				dst.data[i*dst.cols * 3 + j * 3 + z] = value;
			}

		}
	}
}

#define PI 3.1415926

// 计算直线拟合度
float myLinearity(CvSeq *seq)

{

  int i;

  CvPoint2D32f *p;

  float *x = new float[seq->total];

  float *y = new float[seq->total];

  float x_bar=0.0, y_bar=0.0;

  float u11=0.0, u20=0.0, u02=0.0;

  float linearity=0.0;



  for (i=0; i < seq->total; i++){

    p=(CvPoint2D32f*)cvGetSeqElem(seq,i);

    x[i]=p->x;

    y[i]=p->y;

  }

  //x_bar, y_bar

  for (i=0; i < seq->total; i++){

    x_bar+=x[i];

    y_bar+=y[i];

  }

  x_bar/=seq->total;

  y_bar/=seq->total;



  for (i=0; i < seq->total; i++){

    u11+=((x[i]-x_bar)*(y[i]-y_bar));

    u20+=pow(x[i]-x_bar,2.0f);

    u02+=pow(y[i]-y_bar,2.0f);

  }

  u11/=seq->total;

  u20/=seq->total;

  u02/=seq->total;



  // 计算直线拟合度
  linearity = sqrt(4*pow(u11,2.0f)+pow(u20-u02,2.0f))/(u20+u02);

  return linearity;

}

void drawLine(cv::Mat &image, double theta, double rho, cv::Scalar color)
{
    if (theta < PI/4. || theta > 3.*PI/4.)// ~vertical line
    {
        cv::Point pt1(rho/cos(theta), 0);
        cv::Point pt2((rho - image.rows * sin(theta))/cos(theta), image.rows);
        cv::line( image, pt1, pt2, cv::Scalar(255), 1);
    }
    else
    {
        cv::Point pt1(0, rho/sin(theta));
        cv::Point pt2(image.cols, (rho - image.cols * cos(theta))/sin(theta));
        cv::line(image, pt1, pt2, color, 1);
    }
}

bool polynomial_curve_fit(std::vector<cv::Point>& key_point, int n, cv::Mat& A)
{
	//Number of key points
	int N = key_point.size();

	//构造矩阵X
	cv::Mat X = cv::Mat::zeros(n + 1, n + 1, CV_64FC1);
	for (int i = 0; i < n + 1; i++)
	{
		for (int j = 0; j < n + 1; j++)
		{
			for (int k = 0; k < N; k++)
			{
				X.at<double>(i, j) = X.at<double>(i, j) +
					std::pow(key_point[k].x, i + j);
			}
		}
	}

	//构造矩阵Y
	cv::Mat Y = cv::Mat::zeros(n + 1, 1, CV_64FC1);
	for (int i = 0; i < n + 1; i++)
	{
		for (int k = 0; k < N; k++)
		{
			Y.at<double>(i, 0) = Y.at<double>(i, 0) +
				std::pow(key_point[k].x, i) * key_point[k].y;
		}
	}

	A = cv::Mat::zeros(n + 1, 1, CV_64FC1);
	//求解矩阵A
	cv::solve(X, Y, A, cv::DECOMP_LU);
	return true;
}

#define WARN_LINE_DISTANCE		500		// 警告线：50cm
#define NOTICE_LINE_DISTANCE	1000	// 提示线：100cm
#define SAFE_LINE_DISTANCE		1500	// 安全线：150cm

void C360LookAroundMontageAlg::OnGetRawBackAssistPoint( int width, int height, stu2dTable *p2dTable)
{
	ifstream inf;
	char c;
	inf.open("car.txt");
	inf >> m_iCarBeginX >> c >> m_iCarBeginY >> c >> m_iCarWide >> c >> m_iCarHeight;
	inf.close();

	m_heightPerPixel = m_p360Param->world_show_size.height / width;

	printf("%d, %d, %d, %d\n", m_iCarBeginX, m_iCarBeginY, m_iCarWide, m_iCarHeight);

	// 一、在俯视图取关键节点
	cv::Point carTailBegin = { m_iCarBeginX, m_iCarBeginY + m_iCarHeight };
	cv::Point carTailEnd = { m_iCarBeginX + m_iCarWide, m_iCarBeginY + m_iCarHeight };
	cv::Point WarnLineBegin = { carTailBegin.x, carTailBegin.y + (int)(WARN_LINE_DISTANCE / m_heightPerPixel) };
	cv::Point WarnLineEnd = { carTailEnd.x, carTailEnd.y + (int)(WARN_LINE_DISTANCE / m_heightPerPixel) };
	cv::Point NoticeLineBegin = { carTailBegin.x, carTailBegin.y + (int)(NOTICE_LINE_DISTANCE / m_heightPerPixel) };
	cv::Point NoticeLineEnd = { carTailEnd.x, carTailEnd.y + (int)(NOTICE_LINE_DISTANCE / m_heightPerPixel) };
	cv::Point SafeLineBegin = { carTailBegin.x, carTailBegin.y + (int)(SAFE_LINE_DISTANCE / m_heightPerPixel) };
	cv::Point SafeLineEnd = { carTailEnd.x, carTailEnd.y + (int)(SAFE_LINE_DISTANCE / m_heightPerPixel) };
	
	cv::Point rawPoint;
	cv::Point spreadPoint;
	vector<cv::Point> rawLeft1Points;
	vector<cv::Point> rawLeft2Points;
	vector<cv::Point> rawLeft3Points;

	vector<cv::Point> rawRight1Points;
	vector<cv::Point> rawRight2Points;
	vector<cv::Point> rawRight3Points;

	vector<cv::Point> rawWarnPoints;
	vector<cv::Point> rawNoticePoints;
	vector<cv::Point> rawSafePoints;
	
	rawLeft1Points.clear();
	rawLeft2Points.clear();
	rawLeft3Points.clear();

	rawRight1Points.clear();
	rawRight2Points.clear();
	rawRight3Points.clear();

	rawWarnPoints.clear();
	rawNoticePoints.clear();
	rawSafePoints.clear();
	// 二、通过俯视表找到原始图对应的点
	// 左下
	for (int i = carTailBegin.y; i < WarnLineBegin.y; i++)
	{
		if (p2dTable[i * height + carTailBegin.x].img1Idx == 1)
		{
			rawPoint.x = p2dTable[i * height + carTailBegin.x].img1Location.x;
			rawPoint.y = p2dTable[i * height + carTailBegin.x].img1Location.y;
			//printf("left index %d: (%d, %d)\n", i, p2dTable[i * 720 + carTailBegin.x].iLocation.x, p2dTable[i * 720 + carTailBegin.x].iLocation.y);
			//printf("left idx: %d(%d, %d)\n", p2dTable[i * 720 + carTailBegin.x].img1Idx, rawPoint.x, rawPoint.y);

			rawLeft1Points.push_back(rawPoint);
		}
	}

	// 左中
	for (int i = WarnLineBegin.y; i < NoticeLineBegin.y; i++)
	{
		if (p2dTable[i * height + carTailBegin.x].img1Idx == 1)
		{
			rawPoint.x = p2dTable[i * height + carTailBegin.x].img1Location.x;
			rawPoint.y = p2dTable[i * height + carTailBegin.x].img1Location.y;
			//printf("left index %d: (%d, %d)\n", i, p2dTable[i * 720 + carTailBegin.x].iLocation.x, p2dTable[i * 720 + carTailBegin.x].iLocation.y);
			//printf("left idx: %d(%d, %d)\n", p2dTable[i * 720 + carTailBegin.x].img1Idx, rawPoint.x, rawPoint.y);

			rawLeft2Points.push_back(rawPoint);
		}
	}

	// 左上
	for (int i = NoticeLineBegin.y; i < SafeLineBegin.y; i++)
	{
		if (p2dTable[i * height + carTailBegin.x].img1Idx == 1)
		{
			rawPoint.x = p2dTable[i * height + carTailBegin.x].img1Location.x;
			rawPoint.y = p2dTable[i * height + carTailBegin.x].img1Location.y;
			//printf("left index %d: (%d, %d)\n", i, p2dTable[i * 720 + carTailBegin.x].iLocation.x, p2dTable[i * 720 + carTailBegin.x].iLocation.y);
			//printf("left idx: %d(%d, %d)\n", p2dTable[i * 720 + carTailBegin.x].img1Idx, rawPoint.x, rawPoint.y);

			rawLeft3Points.push_back(rawPoint);
		}
	}

	// 连接衔接处，将线连起来
	rawLeft1Points.push_back(rawLeft2Points.front());
	rawLeft2Points.push_back(rawLeft3Points.front());


	// 右下
	for (int i = carTailEnd.y; i < WarnLineEnd.y; i++)
	{
		if (p2dTable[i * height + carTailEnd.x].img1Idx == 1)
		{
			rawPoint.x = p2dTable[i * height + carTailEnd.x].img1Location.x;
			rawPoint.y = p2dTable[i * height + carTailEnd.x].img1Location.y;
			//printf("right index %d: (%d, %d)\n", i, p2dTable[i * 720 + carTailEnd.x].iLocation.x, p2dTable[i * 720 + carTailEnd.x].iLocation.y);
			//printf("right idx: %d(%d, %d)\n", p2dTable[i * 720 + carTailEnd.x].img1Idx, rawPoint.x, rawPoint.y);

			rawRight1Points.push_back(rawPoint);
		}
	}

	// 右中
	for (int i = WarnLineEnd.y; i < NoticeLineEnd.y; i++)
	{
		if (p2dTable[i * height + carTailEnd.x].img1Idx == 1)
		{
			rawPoint.x = p2dTable[i * height + carTailEnd.x].img1Location.x;
			rawPoint.y = p2dTable[i * height + carTailEnd.x].img1Location.y;
			//printf("right index %d: (%d, %d)\n", i, p2dTable[i * 720 + carTailEnd.x].iLocation.x, p2dTable[i * 720 + carTailEnd.x].iLocation.y);
			//printf("right idx: %d(%d, %d)\n", p2dTable[i * 720 + carTailEnd.x].img1Idx, rawPoint.x, rawPoint.y);

			rawRight2Points.push_back(rawPoint);
		}
	}

	// 右上
	for (int i = NoticeLineEnd.y; i < SafeLineEnd.y; i++)
	{
		if (p2dTable[i * height + carTailEnd.x].img1Idx == 1)
		{
			rawPoint.x = p2dTable[i * height + carTailEnd.x].img1Location.x;
			rawPoint.y = p2dTable[i * height + carTailEnd.x].img1Location.y;
			//printf("right index %d: (%d, %d)\n", i, p2dTable[i * 720 + carTailEnd.x].iLocation.x, p2dTable[i * 720 + carTailEnd.x].iLocation.y);
			//printf("right idx: %d(%d, %d)\n", p2dTable[i * 720 + carTailEnd.x].img1Idx, rawPoint.x, rawPoint.y);

			rawRight3Points.push_back(rawPoint);
		}
	}

	// 连接衔接处，将线连起来
	rawRight1Points.push_back(rawRight2Points.front());
	rawRight2Points.push_back(rawRight3Points.front());


	// 警告线
	for(int i = WarnLineBegin.x; i < WarnLineEnd.x; i++)
	{
		if(p2dTable[WarnLineEnd.y * height + i].img1Idx == 1)
		{
			rawPoint.x = p2dTable[WarnLineEnd.y * height + i].img1Location.x;
			rawPoint.y = p2dTable[WarnLineEnd.y * height + i].img1Location.y;
			//printf("index %d: (%d, %d)\n", i, p2dTable[WarnLineEnd.y * height + i].iLocation.x, p2dTable[WarnLineEnd.y * height + i].iLocation.y);
			//printf("idx: %d(%d, %d)\n", p2dTable[WarnLineEnd.y * height + i].img1Idx, rawPoint.x, rawPoint.y);

			rawWarnPoints.push_back(rawPoint);
		}
	}

	// 提示线
	for(int i = NoticeLineBegin.x; i < NoticeLineEnd.x; i++)
	{
		if(p2dTable[NoticeLineEnd.y * height + i].img1Idx == 1)
		{
			rawPoint.x = p2dTable[NoticeLineEnd.y * height + i].img1Location.x;
			rawPoint.y = p2dTable[NoticeLineEnd.y * height + i].img1Location.y;
			//printf("index %d: (%d, %d)\n", i, p2dTable[NoticeLineEnd.y * height + i].iLocation.x, p2dTable[NoticeLineEnd.y * height + i].iLocation.y);
			//printf("idx: %d(%d, %d)\n", p2dTable[NoticeLineEnd.y * height + i].img1Idx, rawPoint.x, rawPoint.y);

			rawNoticePoints.push_back(rawPoint);
		}
	}
	
	// 安全线
	for(int i = SafeLineBegin.x; i < SafeLineEnd.x; i++)
	{
		if(p2dTable[SafeLineEnd.y * height + i].img1Idx == 1)
		{
			rawPoint.x = p2dTable[SafeLineEnd.y * height + i].img1Location.x;
			rawPoint.y = p2dTable[SafeLineEnd.y * height + i].img1Location.y;
			//printf("safe index %d: (%d, %d)\n", i, p2dTable[SafeLineEnd.y * height + i].iLocation.x, p2dTable[SafeLineEnd.y * height + i].iLocation.y);
			//printf("safe idx: %d(%d, %d)\n", p2dTable[SafeLineEnd.y * height + i].img1Idx, rawPoint.x, rawPoint.y);

			rawSafePoints.push_back(rawPoint);
		}
	}

	cv::Point *data = new cv::Point[2048];
	cv::Point *pdata = data;
	int data_index = 0;

	// 三、曲线拟合
	cv::Mat A;
	std::vector<cv::Point> points_fitted;
	cv::Point prePoint;
	int index = 0;
	int maxIndex = 0;

	cv::Mat backAssistImg;
	backAssistImg.create(720, 1280, CV_8UC4);
	backAssistImg.setTo(cv::Scalar(0, 0, 0, 0));


	//左上
	polynomial_curve_fit(rawLeft3Points, 3, A);
	std::cout << "A = " << A << std::endl;

	points_fitted.clear();
	for (int i = 0; i < rawLeft3Points.size(); i++)
	{
		int x = rawLeft3Points.at(i).x;
		double y = A.at<double>(0, 0) + A.at<double>(1, 0) * x +
			A.at<double>(2, 0)*std::pow(x, 2) + A.at<double>(3, 0)*std::pow(x, 3);

		points_fitted.push_back(cv::Point(x, y));
	}

	// 过滤相同的数据
	prePoint = points_fitted.front();
	for (vector<cv::Point>::iterator it = points_fitted.begin() + 1; it != points_fitted.end();)
	{
		if ((*it) == prePoint)
		{
			it = points_fitted.erase(it);
			continue;
		}
		prePoint = (*it);
		it++;
	}

	// 曲线拟合，即画很多段折线
	cv::polylines(backAssistImg, points_fitted, false, cv::Scalar(0, 255, 0, 255), 3, CV_AA, 0);

	// 存点
	data[data_index++].x = points_fitted.size();
	for (vector<cv::Point>::iterator it = points_fitted.begin(); it != points_fitted.end(); it++)
	{
		data[data_index++] = *it;
	}


	// 左中
	polynomial_curve_fit(rawLeft2Points, 3, A);
	std::cout << "A = " << A << std::endl;

	points_fitted.clear();
	for (int i = 0; i < rawLeft2Points.size(); i++)
	{
		int x = rawLeft2Points.at(i).x;
		double y = A.at<double>(0, 0) + A.at<double>(1, 0) * x +
			A.at<double>(2, 0)*std::pow(x, 2) + A.at<double>(3, 0)*std::pow(x, 3);

		points_fitted.push_back(cv::Point(x, y));
	}

	// 过滤相同的数据
	prePoint = points_fitted.front();
	for (vector<cv::Point>::iterator it = points_fitted.begin() + 1; it != points_fitted.end();)
	{
		if ((*it) == prePoint)
		{
			it = points_fitted.erase(it);
			continue;
		}
		prePoint = (*it);
		it++;
	}

	// 曲线拟合，即画很多段折线
	cv::polylines(backAssistImg, points_fitted, false, cv::Scalar(0, 255, 255, 255), 3, CV_AA, 0);

	data[data_index++].x = points_fitted.size();
	for (vector<cv::Point>::iterator it = points_fitted.begin(); it != points_fitted.end(); it++)
	{
		data[data_index++] = *it;
	}


	// 左下
	polynomial_curve_fit(rawLeft1Points, 3, A);
	std::cout << "A = " << A << std::endl;

	points_fitted.clear();
	for (int i = 0; i < rawLeft1Points.size(); i++)
	{
		int x = rawLeft1Points.at(i).x;
		double y = A.at<double>(0, 0) + A.at<double>(1, 0) * x +
			A.at<double>(2, 0)*std::pow(x, 2) + A.at<double>(3, 0)*std::pow(x, 3);

		points_fitted.push_back(cv::Point(x, y));
	}

	// 过滤相同的数据
	prePoint = points_fitted.front();
	//prePoint = points_fitted.at(1);
	for (vector<cv::Point>::iterator it = points_fitted.begin() + 1; it != points_fitted.end();)
	{
		if ((*it) == prePoint)
		{
			it = points_fitted.erase(it);
			continue;
		}
		prePoint = (*it);
		it++;
	}

	// 曲线拟合，即画很多段折线
	cv::polylines(backAssistImg, points_fitted, false, cv::Scalar(0, 0, 255, 255), 3, CV_AA, 0);

	data[data_index++].x = points_fitted.size();
	for (vector<cv::Point>::iterator it = points_fitted.begin(); it != points_fitted.end(); it++)
	{
		data[data_index++] = *it;
	}
	

	// 右上
	polynomial_curve_fit(rawRight3Points, 3, A);
	std::cout << "A = " << A << std::endl;

	points_fitted.clear();
	for (int i = 0; i < rawRight3Points.size(); i++)
	{
		int x = rawRight3Points.at(i).x;
		double y = A.at<double>(0, 0) + A.at<double>(1, 0) * x +
			A.at<double>(2, 0)*std::pow(x, 2) + A.at<double>(3, 0)*std::pow(x, 3);

		points_fitted.push_back(cv::Point(x, y));
	}

	// 过滤相同的数据
	prePoint = points_fitted.front();
	for (vector<cv::Point>::iterator it = points_fitted.begin() + 1; it != points_fitted.end();)
	{
		if ((*it) == prePoint)
		{
			it = points_fitted.erase(it);
			continue;
		}
		prePoint = (*it);
		it++;
	}

	// 曲线拟合，即画很多段折线
	cv::polylines(backAssistImg, points_fitted, false, cv::Scalar(0, 255, 0, 255), 3, CV_AA, 0);

	data[data_index++].x = points_fitted.size();
	for (vector<cv::Point>::iterator it = points_fitted.begin(); it != points_fitted.end(); it++)
	{
		data[data_index++] = *it;
	}


	// 右中
	polynomial_curve_fit(rawRight2Points, 3, A);
	std::cout << "A = " << A << std::endl;

	points_fitted.clear();
	for (int i = 0; i < rawRight2Points.size(); i++)
	{
		int x = rawRight2Points.at(i).x;
		double y = A.at<double>(0, 0) + A.at<double>(1, 0) * x +
			A.at<double>(2, 0)*std::pow(x, 2) + A.at<double>(3, 0)*std::pow(x, 3);

		points_fitted.push_back(cv::Point(x, y));
	}

	// 过滤相同的数据
	prePoint = points_fitted.front();
	for (vector<cv::Point>::iterator it = points_fitted.begin() + 1; it != points_fitted.end();)
	{
		if ((*it) == prePoint)
		{
			it = points_fitted.erase(it);
			continue;
		}
		prePoint = (*it);
		it++;
	}

	// 曲线拟合，即画很多段折线
	cv::polylines(backAssistImg, points_fitted, false, cv::Scalar(0, 255, 255, 255), 3, CV_AA, 0);

	// 存点
	data[data_index++].x = points_fitted.size();
	for (vector<cv::Point>::iterator it = points_fitted.begin(); it != points_fitted.end(); it++)
	{
		data[data_index++] = *it;
	}



	// 右下
	polynomial_curve_fit(rawRight1Points, 3, A);
	std::cout << "A = " << A << std::endl;

	points_fitted.clear();
	for (int i = 0; i < rawRight1Points.size(); i++)
	{
		int x = rawRight1Points.at(i).x;
		double y = A.at<double>(0, 0) + A.at<double>(1, 0) * x +
			A.at<double>(2, 0)*std::pow(x, 2) + A.at<double>(3, 0)*std::pow(x, 3);

		points_fitted.push_back(cv::Point(x, y));
	}

	// 过滤相同的数据
	prePoint = points_fitted.front();
	for (vector<cv::Point>::iterator it = points_fitted.begin() + 1; it != points_fitted.end();)
	{
		if ((*it) == prePoint)
		{
			it = points_fitted.erase(it);
			continue;
		}
		prePoint = (*it);
		it++;
	}

	// 曲线拟合，即画很多段折线
	cv::polylines(backAssistImg, points_fitted, false, cv::Scalar(0, 0, 255, 255), 3, CV_AA, 0);

	data[data_index++].x = points_fitted.size();
	for (vector<cv::Point>::iterator it = points_fitted.begin(); it != points_fitted.end(); it++)
	{
		data[data_index++] = *it;
	}


	// 警告线
	polynomial_curve_fit(rawWarnPoints, 3, A);
	std::cout << "A = " << A << std::endl;

	points_fitted.clear();
	for (int i = 0; i < rawWarnPoints.size(); i++)
	{
		int x = rawWarnPoints.at(i).x;
		double y = A.at<double>(0, 0) + A.at<double>(1, 0) * x +
			A.at<double>(2, 0)*std::pow(x, 2) + A.at<double>(3, 0)*std::pow(x, 3);

		points_fitted.push_back(cv::Point(x, y));
	}

	// 抽点，减少画折线的工作量
	index = 0;
	for (vector<cv::Point>::iterator it = points_fitted.begin(); it != points_fitted.end() - 1;)
	{
		if (index++ % 5)
		{
			it = points_fitted.erase(it);
			continue;
		}
		prePoint = (*it);
		it++;
	}

	// 曲线拟合，即画很多段折线
	cv::polylines(backAssistImg, points_fitted, false, cv::Scalar(0, 0, 255, 255), 3, CV_AA, 0);

	data[data_index++].x = points_fitted.size();
	for (vector<cv::Point>::iterator it = points_fitted.begin(); it != points_fitted.end(); it++)
	{
		data[data_index++] = *it;
	}


	// 提示线
	polynomial_curve_fit(rawNoticePoints, 3, A);
	std::cout << "A = " << A << std::endl;

	points_fitted.clear();
	for (int i = 0; i < rawNoticePoints.size(); i++)
	{
		int x = rawNoticePoints.at(i).x;
		double y = A.at<double>(0, 0) + A.at<double>(1, 0) * x +
			A.at<double>(2, 0)*std::pow(x, 2) + A.at<double>(3, 0)*std::pow(x, 3);

		points_fitted.push_back(cv::Point(x, y));
	}

	// 抽点，减少画折线的工作量
	index = 0;
	for (vector<cv::Point>::iterator it = points_fitted.begin(); it != points_fitted.end() - 1;)
	{
		if (index++ % 5)
		{
			it = points_fitted.erase(it);
			continue;
		}
		prePoint = (*it);
		it++;
	}

	// 曲线拟合，即画很多段折线
	cv::polylines(backAssistImg, points_fitted, false, cv::Scalar(0, 255, 255, 255), 3, CV_AA, 0);

	data[data_index++].x = points_fitted.size();
	for (vector<cv::Point>::iterator it = points_fitted.begin(); it != points_fitted.end(); it++)
	{
		data[data_index++] = *it;
	}


	// 安全线
	polynomial_curve_fit(rawSafePoints, 3, A);
	std::cout << "A = " << A << std::endl;

	points_fitted.clear();
	for (int i = 0; i < rawSafePoints.size(); i++)
	{
		int x = rawSafePoints.at(i).x;
		double y = A.at<double>(0, 0) + A.at<double>(1, 0) * x +
			A.at<double>(2, 0)*std::pow(x, 2) + A.at<double>(3, 0)*std::pow(x, 3);

		points_fitted.push_back(cv::Point(x, y));
	}

	// 抽点，减少画折线的工作量
	index = 0;
	for (vector<cv::Point>::iterator it = points_fitted.begin(); it != points_fitted.end() - 1;)
	{
		if (index++ % 5)
		{
			it = points_fitted.erase(it);
			continue;
		}
		prePoint = (*it);
		it++;
	}

	// 曲线拟合，即画很多段折线
	cv::polylines(backAssistImg, points_fitted, false, cv::Scalar(0, 255, 0, 255), 3, CV_AA, 0);

	data[data_index++].x = points_fitted.size();
	for (vector<cv::Point>::iterator it = points_fitted.begin(); it != points_fitted.end(); it++)
	{
		data[data_index++] = *it;
	}


	// 四、写成表和图像
	imwrite("back_raw.png", backAssistImg);
	//imshow("back_raw", backAssistImg);
	//waitKey(0);

	FILE*file = NULL;

	if (file = fopen("tempimg//back_point.bin", "wb"))
	{
		fwrite(data, sizeof(cv::Point), data_index, file);
		fclose(file);
	}

	delete[]data;
}

void C360LookAroundMontageAlg::OnGetSpreedBackAssistPoint(int width, int height, stu2dTable *p2dTable, stu_spreadImg *pSpreadTable)
{
	ifstream inf;
	char c;
	inf.open("car.txt");
	inf >> m_iCarBeginX >> c >> m_iCarBeginY >> c >> m_iCarWide >> c >> m_iCarHeight;
	inf.close();

	m_heightPerPixel = 18400.0 / 1280;

	printf("%d, %d, %d, %d\n", m_iCarBeginX, m_iCarBeginY, m_iCarWide, m_iCarHeight);

	// 一、在俯视图取关键节点
	cv::Point carTailBegin = { m_iCarBeginX, m_iCarBeginY + m_iCarHeight };
	cv::Point carTailEnd = { m_iCarBeginX + m_iCarWide, m_iCarBeginY + m_iCarHeight };
	cv::Point WarnLineBegin = { carTailBegin.x, carTailBegin.y + (int)(WARN_LINE_DISTANCE / m_heightPerPixel) };
	cv::Point WarnLineEnd = { carTailEnd.x, carTailEnd.y + (int)(WARN_LINE_DISTANCE / m_heightPerPixel) };
	cv::Point NoticeLineBegin = { carTailBegin.x, carTailBegin.y + (int)(NOTICE_LINE_DISTANCE / m_heightPerPixel) };
	cv::Point NoticeLineEnd = { carTailEnd.x, carTailEnd.y + (int)(NOTICE_LINE_DISTANCE / m_heightPerPixel) };
	cv::Point SafeLineBegin = { carTailBegin.x, carTailBegin.y + (int)(SAFE_LINE_DISTANCE / m_heightPerPixel) };
	cv::Point SafeLineEnd = { carTailEnd.x, carTailEnd.y + (int)(SAFE_LINE_DISTANCE / m_heightPerPixel) };

	cv::Point rawPoint;
	cv::Point spreadPoint;
	vector<cv::Point> rawLeftPoints;
	vector<cv::Point> rawRightPoints;

	vector<cv::Point> rawWarnPoints;
	vector<cv::Point> rawNoticePoints;
	vector<cv::Point> rawSafePoints;

	vector<cv::Point> spreadLeftPoints;
	vector<cv::Point> spreadRightPoints;
	vector<cv::Point> spreadNoticePoints;
	vector<cv::Point> spreadSafePoints;

	rawLeftPoints.clear();
	rawRightPoints.clear();
	rawWarnPoints.clear();
	rawNoticePoints.clear();
	rawSafePoints.clear();

	// 左边
	for (int i = carTailBegin.y; i < SafeLineBegin.y; i++)
	{
		if (p2dTable[i * 720 + carTailBegin.x].img1Idx == 1)
		{
			rawPoint.x = p2dTable[i * 720 + carTailBegin.x].img1Location.x;
			rawPoint.y = p2dTable[i * 720 + carTailBegin.x].img1Location.y;
			//printf("right index %d: (%d, %d)\n", i, p2dTable[i * 720 + carTailEnd.x].iLocation.x, p2dTable[i * 720 + carTailEnd.x].iLocation.y);
			//printf("right idx: %d(%d, %d)\n", p2dTable[i * 720 + carTailEnd.x].img1Idx, rawPoint.x, rawPoint.y);

			rawLeftPoints.push_back(rawPoint);
		}
	}

	// 右边
	for (int i = carTailEnd.y; i < SafeLineEnd.y; i++)
	{
		if (p2dTable[i * 720 + carTailEnd.x].img1Idx == 1)
		{
			rawPoint.x = p2dTable[i * 720 + carTailEnd.x].img1Location.x;
			rawPoint.y = p2dTable[i * 720 + carTailEnd.x].img1Location.y;
			//printf("right index %d: (%d, %d)\n", i, p2dTable[i * 720 + carTailEnd.x].iLocation.x, p2dTable[i * 720 + carTailEnd.x].iLocation.y);
			//printf("right idx: %d(%d, %d)\n", p2dTable[i * 720 + carTailEnd.x].img1Idx, rawPoint.x, rawPoint.y);

			rawRightPoints.push_back(rawPoint);
		}
	}

	// 警告线
	for (int i = WarnLineBegin.x; i < WarnLineEnd.x; i++)
	{
		if (p2dTable[WarnLineEnd.y * height + i].img1Idx == 1)
		{
			rawPoint.x = p2dTable[WarnLineEnd.y * height + i].img1Location.x;
			rawPoint.y = p2dTable[WarnLineEnd.y * height + i].img1Location.y;
			//printf("index %d: (%d, %d)\n", i, p2dTable[WarnLineEnd.y * height + i].iLocation.x, p2dTable[WarnLineEnd.y * height + i].iLocation.y);
			//printf("idx: %d(%d, %d)\n", p2dTable[WarnLineEnd.y * height + i].img1Idx, rawPoint.x, rawPoint.y);

			rawWarnPoints.push_back(rawPoint);
		}
	}

	// 提示线
	for (int i = NoticeLineBegin.x; i < NoticeLineEnd.x; i++)
	{
		if (p2dTable[NoticeLineEnd.y * height + i].img1Idx == 1)
		{
			rawPoint.x = p2dTable[NoticeLineEnd.y * height + i].img1Location.x;
			rawPoint.y = p2dTable[NoticeLineEnd.y * height + i].img1Location.y;
			//printf("index %d: (%d, %d)\n", i, p2dTable[NoticeLineEnd.y * height + i].iLocation.x, p2dTable[NoticeLineEnd.y * height + i].iLocation.y);
			//printf("idx: %d(%d, %d)\n", p2dTable[NoticeLineEnd.y * height + i].img1Idx, rawPoint.x, rawPoint.y);

			rawNoticePoints.push_back(rawPoint);
		}
	}

	// 安全线
	for (int i = SafeLineBegin.x; i < SafeLineEnd.x; i++)
	{
		if (p2dTable[SafeLineEnd.y * height + i].img1Idx == 1)
		{
			rawPoint.x = p2dTable[SafeLineEnd.y * height + i].img1Location.x;
			rawPoint.y = p2dTable[SafeLineEnd.y * height + i].img1Location.y;
			//printf("safe index %d: (%d, %d)\n", i, p2dTable[SafeLineEnd.y * height + i].iLocation.x, p2dTable[SafeLineEnd.y * height + i].iLocation.y);
			//printf("safe idx: %d(%d, %d)\n", p2dTable[SafeLineEnd.y * height + i].img1Idx, rawPoint.x, rawPoint.y);

			rawSafePoints.push_back(rawPoint);
		}
	}


	spreadLeftPoints.clear();
	spreadRightPoints.clear();
	spreadNoticePoints.clear();
	spreadSafePoints.clear();

	vector<cv::Point>::iterator it;
	ushort x;
	ushort y;

	// 三、通过展开表找到展开图的点
	// 房车的视野较大，2M的倒车辅助线不到展开图的一半，但是对于宝沃BX7而言，2M已经超出了俯视图
	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			x = pSpreadTable[i * width + j].iWidex;
			y = pSpreadTable[i * width + j].iHeighty;

			for (it = rawLeftPoints.begin(); it != rawLeftPoints.end();)
			{
				if ((*it).x == x && (*it).y == y)
				{
					//printf("left spread idx: (%d, %d)\n", j, i);
					spreadPoint.x = j;
					spreadPoint.y = i;
					spreadLeftPoints.push_back(spreadPoint);
					it = rawLeftPoints.erase(it);
					continue;
				}
				it++;
			}

			for (it = rawRightPoints.begin(); it != rawRightPoints.end();)
			{
				if ((*it).x == x && (*it).y == y)
				{
					//printf("spread idx: %d(%d, %d)\n", k, i, j);
					spreadPoint.x = j;
					spreadPoint.y = i;
					spreadRightPoints.push_back(spreadPoint);
					it = rawRightPoints.erase(it);
					continue;
				}
				it++;
			}

			for (it = rawNoticePoints.begin(); it != rawNoticePoints.end();)
			{
				if ((*it).x == x && (*it).y == y)
				{
					//printf("spread idx: %d(%d, %d)\n", k, i, j);
					spreadPoint.x = j;
					spreadPoint.y = i;
					spreadNoticePoints.push_back(spreadPoint);
					it = rawNoticePoints.erase(it);
					continue;
				}
				it++;
			}

			for (it = rawSafePoints.begin(); it != rawSafePoints.end();)
			{
				if ((*it).x == x && (*it).y == y)
				{
					//printf("safe spread idx: (%d, %d)\n", j, i);
					spreadPoint.x = j;
					spreadPoint.y = i;
					spreadSafePoints.push_back(spreadPoint);
					it = rawSafePoints.erase(it);
					continue;
				}
				it++;
			}

			if (rawLeftPoints.empty() && rawRightPoints.empty() && rawNoticePoints.empty() && rawSafePoints.empty())
				break;
		}
	}


	// 四、使用openCV将离散的点拟和成直线
	cv::Vec4f line;
	float cos_theta;
	float sin_theta;
	float x0, y0;
	float k;
	float b;

	// 拟合左边的点
	if (!spreadLeftPoints.empty())
	{
		cv::fitLine(spreadLeftPoints, line, CV_DIST_HUBER, 0, 0.01, 0.01);

		cos_theta = line[0];
		sin_theta = line[1];
		x0 = line[2], y0 = line[3];

		k = sin_theta / cos_theta;
		b = y0 - k * x0;
		fprintf(stdout, "k=%f,b=%f\r\n", k, b);
	}

	float k1;
	float b1;
	// 拟合右边的点
	if (!spreadRightPoints.empty())
	{
		cv::fitLine(spreadRightPoints, line, CV_DIST_HUBER, 0, 0.01, 0.01);

		cos_theta = line[0];
		sin_theta = line[1];
		x0 = line[2], y0 = line[3];

		k1 = sin_theta / cos_theta;
		b1 = y0 - k1 * x0;
		fprintf(stdout, "k1=%f,b1=%f\r\n", k1, b1);
	}

	float k2;
	float b2;
	// 拟合提示线的点
	if (!spreadNoticePoints.empty())
	{
		cv::fitLine(spreadNoticePoints, line, CV_DIST_HUBER, 0, 0.01, 0.01);

		cos_theta = line[0];
		sin_theta = line[1];
		x0 = line[2], y0 = line[3];

		k2 = sin_theta / cos_theta;
		b2 = y0 - k2 * x0;
		fprintf(stdout, "k2=%f,b2=%f\r\n", k1, b1);
	}

	float k3;
	float b3;
	// 拟合安全线的点
	if (!spreadSafePoints.empty())
	{
		cv::fitLine(spreadSafePoints, line, CV_DIST_HUBER, 0, 0.01, 0.01);

		cos_theta = line[0];
		sin_theta = line[1];
		x0 = line[2], y0 = line[3];

		k3 = sin_theta / cos_theta;
		b3 = y0 - k3 * x0;
		fprintf(stdout, "k3=%f,b3=%f\r\n", k3, b3);
	}
	// 五、求出各直线的交叉点
	cv::Point backAssist[6];
	float tempx, tempy;

	// 求出左边的线起始点
	tempx = (height - b) / k;
	backAssist[0].x = tempx;
	backAssist[0].y = height;

	// 求出右边的线起始点
	tempx = (height - b1) / k1;
	backAssist[1].x = tempx;
	backAssist[1].y = height;

	// 求出提示线左边起始点
	tempx = (b - b2) / (k2 - k);
	tempy = k * tempx + b;

	backAssist[2].x = (int)tempx;
	backAssist[2].y = (int)tempy;

	// 求出提示线右边起始点
	tempx = (b1 - b2) / (k2 - k1);
	tempy = k1 * tempx + b1;

	backAssist[3].x = (int)tempx;
	backAssist[3].y = (int)tempy;

	// 求出安全线左边起始点
	tempx = (b - b3) / (k3 - k);
	tempy = k * tempx + b;

	backAssist[4].x = (int)tempx;
	backAssist[4].y = (int)tempy;

	// 求出安全线右边起始点
	tempx = (b1 - b3) / (k3 - k1);
	tempy = k1 * tempx + b1;

	backAssist[5].x = (int)tempx;
	backAssist[5].y = (int)tempy;


	//printf("notice (%d, %d), (%d, %d)\n", noticeBegin.x, noticeBegin.y, noticeEnd.x, noticeEnd.y);
	//printf("safe (%d, %d), (%d, %d)\n", safeBegin.x, safeBegin.y, safeEnd.x, safeEnd.y);

	// 六、写表
	FILE*filex = NULL;

	if (filex = fopen("tempimg//back_assist.bin", "wb"))
	{
		fwrite(backAssist, sizeof(cv::Point), 6, filex);
		fclose(filex);
	}

}


// 车道线检测
// 原理：检测车道线在拼接缝处是否有断层
// 关键：拼接缝处的车道线两边都可以检测，或者拼接缝处的车道线上下同边，也就是上下都是内边，或者外边
// 方法：在CANY边缘的图上，从车轮往车道线扫描，直到找到第一个白色的点
int C360LookAroundMontageAlg::LaneLineDetect(void* pParam, cv::Mat imgSrc, float& xbiasLup, float& xbiasRup, float& xbiasLdown, float& xbiasRdown)
{
	ifstream inf;
	char c;
	inf.open("car.txt");
	inf >> m_iCarBeginX >> c >> m_iCarBeginY >> c >> m_iCarWide >> c >> m_iCarHeight;
	inf.close();

	printf("%d, %d, %d, %d\n", m_iCarBeginX, m_iCarBeginY, m_iCarWide, m_iCarHeight);

	m_p360Param = (stu_360Param*)pParam;
	float widePerPixel = 720 / m_p360Param->world_show_size.width;               // 每一像素点代表世界坐标系多长的宽度，下同
	int minDiff = widePerPixel * 30;
	int maxDiff = widePerPixel * 150;

	float carLup_x = m_iCarBeginX;
	float carLup_y = m_iCarBeginY;

	float carRup_x = carLup_x + m_iCarWide;
	float carRup_y = carLup_y;

	float carLdown_x = carLup_x;
	float carLdown_y = carLup_y + m_iCarHeight;

	float carRdown_x = carLdown_x + m_iCarWide;
	float carRdown_y = carLdown_y;

	xbiasLup = -1.0;
	xbiasRup = -1.0;
	xbiasLdown = -1.0;
	xbiasRdown = -1.0;
	int srcHeight = imgSrc.rows;
	int srcWidth = imgSrc.cols;
	int itmp = imgSrc.channels();
	if (imgSrc.data == 0)
	{
		cout << "error, the src image is not built!" << endl;
		return -1;
	}

	cv::Mat grayMat;
	if (itmp = 3)
	{
		cvtColor(imgSrc, grayMat, CV_RGB2GRAY);
	}
	else
	{
		grayMat = imgSrc;
	}

	cv::Mat gauss_Mat;
	GaussianBlur(grayMat, gauss_Mat, cv::Size(7, 7), 3, 3);
	imwrite("tempimg//GaussianFilter.bmp", gauss_Mat);

	cv::Mat canny_Mat;
	Canny(gauss_Mat, canny_Mat, 60, 128);
	uchar* pImgData = canny_Mat.data;
	int   srcStep = canny_Mat.step;
	vector<float> recordRup1;
	vector<float> recordRup2;
	int num0 = 0;

	//double carRup_x = 489.0;
	//double carRup_y = 257.0;
	double Krup = (carRup_y - 0) / (carRup_x - (srcWidth - 1));
	double brup = carRup_y - Krup*carRup_x;

	cv::Point pt1Rup, pt2Rup;
	pt1Rup.x = carRup_x;
	pt1Rup.y = carRup_y;
	pt2Rup.x = srcWidth - 1;
	pt2Rup.y = 0;

	double KLup = (carLup_y - 1) / (carLup_x - 1);
	double bLup = carLup_y - KLup*carLup_x;
	cv::Point pt1Lup, pt2Lup;
	pt1Lup.x = carLup_x;
	pt1Lup.y = carLup_y;
	pt2Lup.x = 0;
	pt2Lup.y = 0;


	double KRdown = (carRdown_y - srcHeight) / (carRdown_x - srcWidth);
	double bRdown = carRdown_y - KRdown*carRdown_x;
	cv::Point pt1Rdown, pt2Rdown;
	pt1Rdown.x = carRdown_x;
	pt1Rdown.y = carRdown_y;
	pt2Rdown.x = srcWidth - 1;
	pt2Rdown.y = srcHeight - 1;


	double KLdown = (carLdown_y - srcHeight) / (carLdown_x - 1);
	double bLdown = carLdown_y - KLdown*carLdown_x;
	cv::Point pt1Ldown, pt2Ldown;
	pt1Ldown.x = carLdown_x;
	pt1Ldown.y = carLdown_y;
	pt2Ldown.x = 0;
	pt2Ldown.y = srcHeight - 1;

	// 擦除车道线断裂处的连接点（噪声），以防干扰检测（特别是偏移较大的情况下）
	cv::line(canny_Mat, pt1Lup, pt2Lup, cv::Scalar(0, 0, 0), 2);
	cv::line(canny_Mat, pt1Rup, pt2Rup, cv::Scalar(0, 0, 0), 2);
	cv::line(canny_Mat, pt1Ldown, pt2Ldown, cv::Scalar(0, 0, 0), 2);
	cv::line(canny_Mat, pt1Rdown, pt2Rdown, cv::Scalar(0, 0, 0), 2);
	imwrite("tempimg//CannyEdgeDetect.bmp", canny_Mat);


	//俯视图右上角,上图车道线右边
	for (int i = 10; i<carRup_y - 1; i++)
	{
		uchar* pImgYdata = pImgData + i*srcStep;
		//for (int j = srcWidth - 10; j>carRup_x; j--)
		for (int j = carRup_x; j < srcWidth - 10; j++)
		{
			if (pImgYdata[j]>0)
			{
				if (i*1.0 < Krup*j + brup)
				{
					recordRup1.push_back(j);
					num0 = num0 + 1;
				}
				break;		// 找到第一个点后返回
			}
		}

		// 如果上下相邻的两个点像素差大于3，则认为已经找到了断层
		if (num0 > 2)
		{
			float xdifftmp = recordRup1.at(num0 - 1) - recordRup1.at(num0 - 2);
			if (fabs(xdifftmp) > maxDiff)
			{
				// 防止拼接缝处检测不到内线的白点
				if (recordRup1.size() > 80)
				{
					recordRup1.pop_back();
					break;
				}
				cout << "right top corner 1 error, current scene not meet the requirement or camera move too large!" << endl;
				return -1;
			}
			if (fabs(xdifftmp) > minDiff)//10
			{
				break;
			}
		}

	}

	//俯视图右上角,右图车道线右边
	//访问i行j列的数据 1
	//for(int i=carRup_y-1; i>10; i--)
	//{
	//	uchar* pImgYdata = pImgData + i*srcStep;
	//	for(int j=srcWidth-10; j>carRup_x; j--)
	//	{
	//		if(pImgYdata[j]>0)
	//		{
	//			if(i*1.0 > Krup*j + brup)
	//			{
	//				recordRup2.push_back(j);
	//				break;
	//			}
	//		}
	//	}

	//}

	//访问i行j列的数据 2 指针数组
	//uchar* imgdata = (uchar*)canny_Mat.data;
	//for(int i=carRup_y-1; i>10; i--)
	//{
	//	for(int j=srcWidth-10; j>carRup_x; j--)
	//	{
	//		int index = i*srcWidth + j;//*cols
	//		if(imgdata[index]>0)
	//		{
	//			if(i*1.0 > Krup*j + brup)
	//			{
	//				recordRup2.push_back(j);
	//				break;
	//			}
	//		}
	//	}
	//}

	/* .ptr with [] */
	num0 = 0;
	for (int i = carRup_y - 1; i>10; i--)
	{
		uchar* imgdata = canny_Mat.ptr<uchar>(i);
		//for (int j = srcWidth - 10; j>carRup_x; j--)
		for (int j = carRup_x; j < srcWidth - 10; j++)
		{
			if (imgdata[j]>0)
			{
				if (i*1.0 > Krup*j + brup)
				{
					recordRup2.push_back(j);
					num0 = num0 + 1;
				}
				break;		// 找到第一个点后返回
			}
		}

		// 如果上下相邻的两个点像素差大于3，则认为已经找到了断层
		if (num0>2)
		{
			float xdifftmp = recordRup2.at(num0 - 1) - recordRup2.at(num0 - 2);
			if (fabs(xdifftmp) > maxDiff)
			{
				// 防止拼接缝处检测不到内线的白点
				if (recordRup2.size() > 80)
				{
					recordRup2.pop_back();
					break;
				}
				cout << "right top corner 2 error, current scene not meet the requirement or camera move too large!" << endl;
				return -1;
			}

			if (fabs(xdifftmp) > minDiff)//10
			{
				break;
			}
		}
	}

	//.at
	//for(int i=carRup_y-1; i>10; i--)
	//{
	//	for(int j=srcWidth-10; j>carRup_x; j--)
	//	{
	//		//if(canny_Mat.at<uchar>(i,j)>0)
	//		if(canny_Mat.at<uchar>(cv::Point(j,i))>0)
	//		{
	//			if(i*1.0 > Krup*j + brup)
	//			{
	//				recordRup2.push_back(j);
	//				break;
	//			}
	//		}
	//	}
	//}

	int RupNum1 = recordRup1.size();
	int RupNum2 = recordRup2.size();
	//float xindRup1 = accumulate( recordRup1.at(RupNum1-20-1),recordRup1.at(RupNum1-2),0)*1.0/20.0;
	//float xindRup2 = accumulate( recordRup2.at(RupNum2-20-1),recordRup2.at(RupNum2-2),0)*1.0/20.0;
	float sumxindRup1 = 0;
	float sumNumRup1 = 0;
	if (RupNum1>30)
	{
		for (int i = RupNum1 - 20 - 1; i<RupNum1 - 1; i++)
		{
			sumxindRup1 = sumxindRup1 + recordRup1.at(i);
			sumNumRup1 = sumNumRup1 + 1;
		}
	}
	float xindRup1 = sumxindRup1 / sumNumRup1;

	float sumxindRup2 = 0;
	float sumNum2 = 0;
	if (RupNum2>30)
	{
		for (int i = RupNum2 - 20 - 1; i<RupNum2 - 1; i++)
		{
			sumxindRup2 = sumxindRup2 + recordRup2.at(i);
			sumNum2 = sumNum2 + 1;
		}
	}
	float xindRup2 = sumxindRup2 / sumNum2;

	//TOP 加左减右, xindRup2代表右图,xindRup1代表上图右边
	xbiasRup = xindRup1 - xindRup2;//正值代表左移动,负值代表右移动
	if (fabs(xbiasRup) > maxDiff)
	{
		//xbiasRup = 0.0;
		cout << "right top corner error, current scene not meet the requirement or camera move too large!" << endl;
		cout << "diff: " << fabs(xbiasRup) << endl;
		return -1;
	}
	else if (fabs(xbiasRup)<1)
	{
		xbiasRup = 0.0;
	}
	else
	{
		xbiasRup = xbiasRup;
	}

	//俯视图左上角
	//double carLup_x = 232;
	//double carLup_y = 257;



	vector<float> recordLup1;
	vector<float> recordLup2;
	int num1 = 0;
	//俯视图左上角,上图车道线右边
	for (int i = 10; i<carLup_y; i++)
	{
		uchar* pImgYdata = pImgData + i*srcStep;
		for (int j = carLup_x; j>10; j--)
		{
			if (pImgYdata[j]>0)
			{
				if (i*1.0 < KLup*j + bLup)
				{
					recordLup1.push_back(j);
					num1 = num1 + 1;
				}
				break;		// 找到第一个点后返回
			}
		}

		if (num1>2)
		{
			float xdifftmp = recordLup1.at(num1 - 1) - recordLup1.at(num1 - 2);
			if (fabs(xdifftmp) > maxDiff)
			{
				// 防止拼接缝处检测不到内线的白点
				if (recordLup1.size() > 80)
				{
					recordLup1.pop_back();
					break;
				}
				cout << "left top corner 1 error, current scene not meet the requirement or camera move too large!" << endl;
				return -1;
			}
			if (fabs(xdifftmp) > minDiff)//10
			{
				break;
			}
		}
	}

	num1 = 0;
	for (int i = carLup_y - 1; i>10; i--)
	{
		uchar* pImgYdata = pImgData + i*srcStep;
		for (int j = carLup_x - 1; j>10; j--)
		{
			if (pImgYdata[j]>0)
			{
				if (i*1.0 > KLup*j + bLup)
				{
					recordLup2.push_back(j);
					num1 = num1 + 1;
				}
				break;		// 找到第一个点后返回
			}
		}

		if (num1>2)
		{
			float xdifftmp = recordLup2.at(num1 - 1) - recordLup2.at(num1 - 2);
			if (fabs(xdifftmp) > maxDiff)
			{
				// 防止拼接缝处检测不到内线的白点
				if (recordLup2.size() > 80)
				{
					recordLup2.pop_back();
					break;
				}
				cout << "left top corner 2 error, current scene not meet the requirement or camera move too large!" << endl;
				return -1;
			}
			if (fabs(xdifftmp) > minDiff)//10
			{
				break;
			}
		}
	}

	int LupNum1 = recordLup1.size();
	int LupNum2 = recordLup2.size();
	float sumxindLup1 = 0;
	float sumNumLup1 = 0;

	if (LupNum1>30)
	{
		for (int i = LupNum1 - 20 - 1; i<LupNum1 - 1; i++)
		{
			sumxindLup1 = sumxindLup1 + recordLup1.at(i);
			sumNumLup1 = sumNumLup1 + 1;
		}
	}

	float xindLup1 = sumxindLup1 / sumNumLup1;

	float sumxindLup2 = 0;
	float sumNumLup2 = 0;

	if (LupNum2>30)
	{
		for (int i = LupNum2 - 20 - 1; i<LupNum2 - 1; i++)
		{
			sumxindLup2 = sumxindLup2 + recordLup2.at(i);
			sumNumLup2 = sumNumLup2 + 1;
		}
	}
	float xindLup2 = sumxindLup2 / sumNumLup2;

	//TOP 加左减右, xindRup2代表左图,xindRup2代表上图
	xbiasLup = xindLup1 - xindLup2;//正值代表左移动,负值代表右移动
	if (fabs(xbiasLup)>maxDiff)
	{
		//xbiasLup = 0.0;
		cout << "left top corner error, current scene not meet the requirement or camera move too large!" << endl;
		cout << "diff: " << fabs(xbiasLup) << endl;
		return -1;
	}
	else if (fabs(xbiasLup)<1)
	{
		xbiasLup = 0.0;
	}
	else
	{
		xbiasLup = xbiasLup;
	}

	//俯视图右下角
	//double carRdown_x = 489;
	//double carRdown_y = 996;


	vector<int> recordRdown1;
	vector<int> recordRdown2;
	int num2 = 0;
	
	recordRdown1.clear();
	for (int i = carRdown_y; i<srcHeight - 10; i++)
	{
		uchar* pImgYdata = pImgData + i*srcStep;
		//for (int j = srcWidth - 10; j>carRdown_x; j--)
		for (int j = carRdown_x; j < srcWidth - 10; j++)
		{
			if (pImgYdata[j]>0)
			{
				if (i*1.0 < KRdown*j + bRdown)
				{
					recordRdown1.push_back(j);
					num2++;
				}
				break;		// 找到第一个点后返回
			}
		}

		if (num2>2)
		{
			float xdifftmp = recordRdown1.at(num2 - 1) - recordRdown1.at(num2 - 2);
			if (fabs(xdifftmp) > maxDiff)
			{
				// 防止拼接缝处检测不到内线的白点
				if (recordRdown1.size() > 80)
				{
					recordRdown1.pop_back();
					break;
				}
				printf("%f, %d, (%d, %d)\n", carRdown_x, num2, recordRdown1.at(num2 - 1), recordRdown1.at(num2 - 2));
				cout << "right botton corner 1 error, current scene not meet the requirement or camera move too large!" << endl;
				return -1;
			}
			if (fabs(xdifftmp) > minDiff)//10
			{
				break;
			}
		}
	}

	num2 = 0;
	for (int i = srcHeight - 10; i>carRdown_y; i--)
	{
		uchar* pImgYdata = pImgData + i*srcStep;
		//for (int j = srcWidth - 10; j>carRdown_x; j--)
		for (int j = carRdown_x; j < srcWidth - 10; j++)
		{
			if (pImgYdata[j]>0)
			{
				if (i*1.0 > KRdown*j + bRdown)
				{
					recordRdown2.push_back(j);
					num2 = num2 + 1;
				}
				break;		// 找到第一个点后返回
			}
		}

		if (num2>2)
		{
			float xdifftmp = recordRdown2.at(num2 - 1) - recordRdown2.at(num2 - 2);
			if (fabs(xdifftmp) > maxDiff)
			{
				// 防止拼接缝处检测不到内线的白点
				if (recordRdown2.size() > 80)
				{
					recordRdown2.pop_back();
					break;
				}
				cout << "right botton corner 2 error, current scene not meet the requirement or camera move too large!" << endl;
				return -1;
			}
			if (fabs(xdifftmp) > minDiff)//10
			{
				break;
			}
		}
	}

	int RdownNum1 = recordRdown1.size();
	int RdownNum2 = recordRdown2.size();
	float sumxindRdown1 = 0;
	float sumNumRdown1 = 0;

	if (RdownNum1>30)
	{
		for (int i = RdownNum1 - 20 - 1; i<RdownNum1 - 1; i++)
		{
			sumxindRdown1 = sumxindRdown1 + recordRdown1.at(i);
			sumNumRdown1 = sumNumRdown1 + 1;
		}
	}

	float xindRdown1 = sumxindRdown1 / sumNumRdown1;

	float sumxindRdown2 = 0;
	float sumNumRdown2 = 0;

	if (RdownNum2>30)
	{
		for (int i = RdownNum2 - 20 - 1; i<RdownNum2 - 1; i++)
		{
			sumxindRdown2 = sumxindRdown2 + recordRdown2.at(i);
			sumNumRdown2 = sumNumRdown2 + 1;
		}
	}
	float xindRdown2 = sumxindRdown2 / sumNumRdown2;

	//BOTTOM 左减右加 , xindRdown1 代表右图, xindRdown2 代表下图

	xbiasRdown = xindRdown1 - xindRdown2;//负值代表左移动,正值代表右移动
	if (fabs(xbiasRdown) > maxDiff)
	{
		//xbiasRdown = 0.0;
		cout << "right botton corner error, current scene not meet the requirement or camera move too large!" << endl;
		cout << "diff: " << fabs(xbiasRdown) << endl;
		return -1;
	}
	else if (fabs(xbiasRdown)<1)
	{
		xbiasRdown = 0.0;
	}
	else
	{
		xbiasRdown = xbiasRdown;
	}


	//俯视图左下角
	//double carLdown_x = 232;
	//double carLdown_y = 996;

	vector<float> recordLdown1;
	vector<float> recordLdown2;
	int num3 = 0;

	for (int i = carLdown_y; i<srcHeight - 10; i++)
	{
		uchar* pImgYdata = pImgData + i*srcStep;
		for (int j = carLdown_x; j>10; j--)
		{
			if (pImgYdata[j]>0)
			{
				if (i*1.0 < KLdown*j + bLdown)
				{
					recordLdown1.push_back(j);
					num3 = num3 + 1;
				}
				break;		// 找到第一个点后返回
			}
		}

		if (num3>2)
		{
			float xdifftmp = recordLdown1.at(num3 - 1) - recordLdown1.at(num3 - 2);
			if (fabs(xdifftmp) > maxDiff)
			{
				// 防止拼接缝处检测不到内线的白点
				if (recordLdown1.size() > 80)
				{
					recordLdown1.pop_back();
					break;
				}
				cout << "left botton corner 1 error, current scene not meet the requirement or camera move too large!" << endl;
				return -1;
			}
			if (fabs(xdifftmp) > minDiff)//10
			{
				break;
			}
		}
	}

	num3 = 0;
	for (int i = srcHeight - 10; i>carLdown_y; i--)
	{
		uchar* pImgYdata = pImgData + i*srcStep;
		for (int j = carLdown_x; j>10; j--)
		{
			if (pImgYdata[j]>0)
			{
				if (i*1.0 > KLdown*j + bLdown)
				{
					recordLdown2.push_back(j);
					num3++;
				}
				break;		// 找到第一个点后返回
			}
		}

		if (num3>2)
		{
			float xdifftmp = recordLdown2.at(num3 - 1) - recordLdown2.at(num3 - 2);
			if (fabs(xdifftmp) > maxDiff)
			{
				// 防止拼接缝处检测不到内线的白点
				if (recordLdown2.size() > 80)
				{
					recordLdown2.pop_back();
					break;
				}
				cout << "left botton corner 2 error, current scene not meet the requirement or camera move too large!" << endl;
				return -1;
			}
			if (fabs(xdifftmp) > minDiff)//10
			{
				break;
			}
		}
	}

	int LdownNum1 = recordLdown1.size();
	int LdownNum2 = recordLdown2.size();

	float sumxindLdown1 = 0;
	float sumNumLdown1 = 0;

	if (LdownNum1>30)
	{
		for (int i = LdownNum1 - 20 - 1; i<LdownNum1 - 1; i++)
		{
			sumxindLdown1 = sumxindLdown1 + recordLdown1.at(i);
			sumNumLdown1 = sumNumLdown1 + 1;
		}
	}
	float xindLdown1 = sumxindLdown1 / sumNumLdown1;

	float sumxindLdown2 = 0;
	float sumNumLdown2 = 0;

	if (LdownNum2>30)
	{
		for (int i = LdownNum2 - 20 - 1; i<LdownNum2 - 1; i++)
		{
			sumxindLdown2 = sumxindLdown2 + recordLdown2.at(i);
			sumNumLdown2 = sumNumLdown2 + 1;
		}
	}

	float xindLdown2 = sumxindLdown2 / sumNumLdown2;

	//BOTTOM 左减右加 , xindLdown1 代表左图, xindLdown2 代表下图

	xbiasLdown = xindLdown1 - xindLdown2;
	if (fabs(xbiasLdown)>maxDiff)
	{
		//xbiasLdown = 0.0;
		cout << "left botton corner error, current scene not meet the requirement or camera move too large!" << endl;
		cout << "diff: " << fabs(xbiasLdown) << endl;
		return -1;
	}
	else if (fabs(xbiasLdown)<1)
	{
		xbiasLdown = 0.0;
	}
	else
	{
		xbiasLdown = xbiasLdown;
	}

	cv::line(canny_Mat, pt1Lup, pt2Lup, cv::Scalar(255, 255, 255), 1);
	cv::line(canny_Mat, pt1Rup, pt2Rup, cv::Scalar(255, 255, 255), 1);
	cv::line(canny_Mat, pt1Ldown, pt2Ldown, cv::Scalar(255, 255, 255), 1);
	cv::line(canny_Mat, pt1Rdown, pt2Rdown, cv::Scalar(255, 255, 255), 1);

	imwrite("tempimg//AfterCanny.bmp", canny_Mat);

	return 0;
};

// 车道线检测
// 原理：检测车道线在拼接缝处是否有断层
// 关键：拼接缝处的车道线两边都可以检测，或者拼接缝处的车道线上下同边，也就是上下都是内边，或者外边
// 方法：在CANY边缘的图上，从车轮往车道线扫描，直到找到第一个白色的点
int C360LookAroundMontageAlg::EachLaneLineDetect(void* pParam, cv::Mat *imgSrc, float& xbiasLup, float& xbiasRup, float& xbiasLdown, float& xbiasRdown)
{
	ifstream inf;
	char c;
	inf.open("car.txt");
	inf >> m_iCarBeginX >> c >> m_iCarBeginY >> c >> m_iCarWide >> c >> m_iCarHeight;
	inf.close();

	printf("%d, %d, %d, %d\n", m_iCarBeginX, m_iCarBeginY, m_iCarWide, m_iCarHeight);

	m_p360Param = (stu_360Param*)pParam;

	float widePerPixel = 720 / m_p360Param->world_show_size.width;               // 每一像素点代表世界坐标系多长的宽度，下同
	int minDiff = widePerPixel * 30;
	int maxDiff = widePerPixel * 150;

	float carLup_x = m_iCarBeginX;
	float carLup_y = m_iCarBeginY;

	float carRup_x = carLup_x + m_iCarWide;
	float carRup_y = carLup_y;

	float carLdown_x = carLup_x;
	float carLdown_y = carLup_y + m_iCarHeight;

	float carRdown_x = carLdown_x + m_iCarWide;
	float carRdown_y = carLdown_y;

	xbiasLup = -1.0;
	xbiasRup = -1.0;
	xbiasLdown = -1.0;
	xbiasRdown = -1.0;
	int srcHeight = imgSrc[0].rows;
	int srcWidth = imgSrc[0].cols;
	int itmp = imgSrc[0].channels();
	for (int i = 0; i < FISHEYE_COUNT; i++)
	{
		if (imgSrc[i].data == 0)
		{
			cout << "error, the src image is not built!" << endl;
			return -1;
		}
	}

	cv::Mat grayMat;
	cv::Mat gauss_Mat;
	cv::Mat canny_Mat[FISHEYE_COUNT];
	uchar* pImgData[FISHEYE_COUNT];
	int srcStep[FISHEYE_COUNT];

	for (int i = 0; i < FISHEYE_COUNT; i++)
	{
		if (itmp = 3)
		{
			cvtColor(imgSrc[i], grayMat, CV_RGB2GRAY);
		}
		else
		{
			grayMat = imgSrc[i];
		}

		GaussianBlur(grayMat, gauss_Mat, cv::Size(7, 7), 3, 3);
		//imwrite("tempimg//GaussianFilter.bmp", gauss_Mat);

		Canny(gauss_Mat, canny_Mat[i], 60, 128);
		//imwrite("tempimg//CannyEdgeDetect.bmp", canny_Mat[i]);
		//imshow("canny", canny_Mat[i]);
		//waitKey(0);

		pImgData[i] = canny_Mat[i].data;
		srcStep[i] = canny_Mat[i].step;
	}

	imwrite("tempimg//CannyEdgeDetect_pre.bmp", canny_Mat[3]);

	vector<float> recordRup1;
	vector<float> recordRup2;
	int num0 = 0;

	//double carRup_x = 489.0;
	//double carRup_y = 257.0;
	double Krup = (carRup_y - 0) / (carRup_x - (srcWidth - 1));
	double brup = carRup_y - Krup*carRup_x;

	cv::Point pt1Rup, pt2Rup;
	pt1Rup.x = carRup_x;
	pt1Rup.y = carRup_y;
	pt2Rup.x = srcWidth - 1;
	pt2Rup.y = 0;


	double KLup = (carLup_y - 1) / (carLup_x - 1);
	double bLup = carLup_y - KLup*carLup_x;
	cv::Point pt1Lup, pt2Lup;
	pt1Lup.x = carLup_x;
	pt1Lup.y = carLup_y;
	pt2Lup.x = 0;
	pt2Lup.y = 0;


	double KRdown = (carRdown_y - srcHeight) / (carRdown_x - srcWidth);
	double bRdown = carRdown_y - KRdown*carRdown_x;
	cv::Point pt1Rdown, pt2Rdown;
	pt1Rdown.x = carRdown_x;
	pt1Rdown.y = carRdown_y;
	pt2Rdown.x = srcWidth - 1;
	pt2Rdown.y = srcHeight - 1;


	double KLdown = (carLdown_y - srcHeight) / (carLdown_x - 1);
	double bLdown = carLdown_y - KLdown*carLdown_x;
	cv::Point pt1Ldown, pt2Ldown;
	pt1Ldown.x = carLdown_x;
	pt1Ldown.y = carLdown_y;
	pt2Ldown.x = 0;
	pt2Ldown.y = srcHeight - 1;

	// 擦除车道线断裂处的连接点（噪声），以防干扰检测（特别是偏移较大的情况下）
	cv::line(canny_Mat[0], pt1Lup, pt2Lup, cv::Scalar(0, 0, 0), 2);
	cv::line(canny_Mat[2], pt1Lup, pt2Lup, cv::Scalar(0, 0, 0), 2);

	cv::line(canny_Mat[0], pt1Rup, pt2Rup, cv::Scalar(0, 0, 0), 2);
	cv::line(canny_Mat[3], pt1Rup, pt2Rup, cv::Scalar(0, 0, 0), 2);

	cv::line(canny_Mat[1], pt1Ldown, pt2Ldown, cv::Scalar(0, 0, 0), 2);
	cv::line(canny_Mat[2], pt1Ldown, pt2Ldown, cv::Scalar(0, 0, 0), 2);

	cv::line(canny_Mat[1], pt1Rdown, pt2Rdown, cv::Scalar(0, 0, 0), 2);
	cv::line(canny_Mat[3], pt1Rdown, pt2Rdown, cv::Scalar(0, 0, 0), 2);

	for (int i = 0; i < FISHEYE_COUNT; i++)
	{
		imshow("canny", canny_Mat[i]);
		cv::waitKey(0);
	}

	imwrite("tempimg//CannyEdgeDetect.bmp", canny_Mat[3]);

	//俯视图右上角,上图车道线右边
	for (int i = 10; i<carRup_y - 1; i++)
	{
		uchar* pImgYdata = pImgData[0] + i*srcStep[0];
		//for (int j = srcWidth - 10; j>carRup_x; j--)
		for (int j = carRup_x; j < srcWidth - 10; j++)
		{
			if (pImgYdata[j]>0)
			{
				if (i*1.0 < Krup*j + brup)
				{
					recordRup1.push_back(j);
					num0 = num0 + 1;
				}
				break;		// 找到第一个点后返回
			}
		}

		// 如果上下相邻的两个点像素差大于3，则认为已经找到了断层
		if (num0 > 2)
		{
			float xdifftmp = recordRup1.at(num0 - 1) - recordRup1.at(num0 - 2);
			if (fabs(xdifftmp) > maxDiff)
			{
				cout << "right top corner 1 error, current scene not meet the requirement or camera move too large!" << endl;
				return -1;
			}
			if (fabs(xdifftmp) > minDiff)//10
			{
				break;
			}
		}

	}

	//俯视图右上角,右图车道线右边
	//访问i行j列的数据 1
	//for(int i=carRup_y-1; i>10; i--)
	//{
	//	uchar* pImgYdata = pImgData + i*srcStep;
	//	for(int j=srcWidth-10; j>carRup_x; j--)
	//	{
	//		if(pImgYdata[j]>0)
	//		{
	//			if(i*1.0 > Krup*j + brup)
	//			{
	//				recordRup2.push_back(j);
	//				break;
	//			}
	//		}
	//	}

	//}

	//访问i行j列的数据 2 指针数组
	//uchar* imgdata = (uchar*)canny_Mat.data;
	//for(int i=carRup_y-1; i>10; i--)
	//{
	//	for(int j=srcWidth-10; j>carRup_x; j--)
	//	{
	//		int index = i*srcWidth + j;//*cols
	//		if(imgdata[index]>0)
	//		{
	//			if(i*1.0 > Krup*j + brup)
	//			{
	//				recordRup2.push_back(j);
	//				break;
	//			}
	//		}
	//	}
	//}

	/* .ptr with [] */
	num0 = 0;
	for (int i = carRup_y - 1; i>10; i--)
	{
		uchar* imgdata = canny_Mat[3].ptr<uchar>(i);
		//for (int j = srcWidth - 10; j>carRup_x; j--)
		for (int j = carRup_x; j < srcWidth - 10; j++)
		{
			if (imgdata[j]>0)
			{
				if (i*1.0 > Krup*j + brup)
				{
					recordRup2.push_back(j);
					num0 = num0 + 1;
				}
				break;		// 找到第一个点后返回
			}
		}

		// 如果上下相邻的两个点像素差大于3，则认为已经找到了断层
		if (num0>2)
		{
			float xdifftmp = recordRup2.at(num0 - 1) - recordRup2.at(num0 - 2);
			//if (fabs(xdifftmp) > maxDiff)
			//{
			//	cout << "right top corner 2 error, current scene not meet the requirement or camera move too large!" << endl;
			//	return -1;
			//}

			if (fabs(xdifftmp) > minDiff)//10
			{
				break;
			}
		}
	}

	//.at
	//for(int i=carRup_y-1; i>10; i--)
	//{
	//	for(int j=srcWidth-10; j>carRup_x; j--)
	//	{
	//		//if(canny_Mat.at<uchar>(i,j)>0)
	//		if(canny_Mat.at<uchar>(cv::Point(j,i))>0)
	//		{
	//			if(i*1.0 > Krup*j + brup)
	//			{
	//				recordRup2.push_back(j);
	//				break;
	//			}
	//		}
	//	}
	//}

	int RupNum1 = recordRup1.size();
	int RupNum2 = recordRup2.size();
	//float xindRup1 = accumulate( recordRup1.at(RupNum1-20-1),recordRup1.at(RupNum1-2),0)*1.0/20.0;
	//float xindRup2 = accumulate( recordRup2.at(RupNum2-20-1),recordRup2.at(RupNum2-2),0)*1.0/20.0;
	float sumxindRup1 = 0;
	float sumNumRup1 = 0;
	if (RupNum1>30)
	{
		for (int i = RupNum1 - 20 - 1; i<RupNum1 - 1; i++)
		{
			sumxindRup1 = sumxindRup1 + recordRup1.at(i);
			sumNumRup1 = sumNumRup1 + 1;
		}
	}
	float xindRup1 = sumxindRup1 / sumNumRup1;

	float sumxindRup2 = 0;
	float sumNum2 = 0;
	if (RupNum2>30)
	{
		for (int i = RupNum2 - 20 - 1; i<RupNum2 - 1; i++)
		{
			sumxindRup2 = sumxindRup2 + recordRup2.at(i);
			sumNum2 = sumNum2 + 1;
		}
	}
	float xindRup2 = sumxindRup2 / sumNum2;

	//TOP 加左减右, xindRup2代表右图,xindRup1代表上图右边
	xbiasRup = xindRup1 - xindRup2;//正值代表左移动,负值代表右移动
	if (fabs(xbiasRup) > maxDiff)
	{
		//xbiasRup = 0.0;
		cout << "right top corner error, current scene not meet the requirement or camera move too large!" << endl;
		cout << "diff: " << fabs(xbiasRup) << endl;
		return -1;
	}
	else if (fabs(xbiasRup)<1)
	{
		xbiasRup = 0.0;
	}
	else
	{
		xbiasRup = xbiasRup;
	}

	//俯视图左上角
	//double carLup_x = 232;
	//double carLup_y = 257;


	vector<float> recordLup1;
	vector<float> recordLup2;
	int num1 = 0;
	//俯视图左上角,上图车道线右边
	for (int i = 10; i<carLup_y; i++)
	{
		uchar* pImgYdata = pImgData[0] + i*srcStep[0];
		for (int j = carLup_x; j>10; j--)
		{
			if (pImgYdata[j]>0)
			{
				if (i*1.0 < KLup*j + bLup)
				{
					recordLup1.push_back(j);
					num1 = num1 + 1;
				}
				break;		// 找到第一个点后返回
			}
		}

		if (num1>2)
		{
			float xdifftmp = recordLup1.at(num1 - 1) - recordLup1.at(num1 - 2);
			if (fabs(xdifftmp) > maxDiff)
			{
				cout << "left top corner 1 error, current scene not meet the requirement or camera move too large!" << endl;
				return -1;
			}
			if (fabs(xdifftmp) > minDiff)//10
			{
				break;
			}
		}
	}

	num1 = 0;
	for (int i = carLup_y - 1; i>10; i--)
	{
		uchar* pImgYdata = pImgData[2] + i*srcStep[2];
		for (int j = carLup_x - 1; j>10; j--)
		{
			if (pImgYdata[j]>0)
			{
				if (i*1.0 > KLup*j + bLup)
				{
					recordLup2.push_back(j);
					num1 = num1 + 1;
				}
				break;		// 找到第一个点后返回
			}
		}

		if (num1>2)
		{
			float xdifftmp = recordLup2.at(num1 - 1) - recordLup2.at(num1 - 2);
			if (fabs(xdifftmp) > maxDiff)
			{
				cout << "left top corner 2 error, current scene not meet the requirement or camera move too large!" << endl;
				return -1;
			}
			if (fabs(xdifftmp) > minDiff)//10
			{
				break;
			}
		}
	}

	int LupNum1 = recordLup1.size();
	int LupNum2 = recordLup2.size();
	float sumxindLup1 = 0;
	float sumNumLup1 = 0;

	if (LupNum1>30)
	{
		for (int i = LupNum1 - 20 - 1; i<LupNum1 - 1; i++)
		{
			sumxindLup1 = sumxindLup1 + recordLup1.at(i);
			sumNumLup1 = sumNumLup1 + 1;
		}
	}

	float xindLup1 = sumxindLup1 / sumNumLup1;

	float sumxindLup2 = 0;
	float sumNumLup2 = 0;

	if (LupNum2>30)
	{
		for (int i = LupNum2 - 20 - 1; i<LupNum2 - 1; i++)
		{
			sumxindLup2 = sumxindLup2 + recordLup2.at(i);
			sumNumLup2 = sumNumLup2 + 1;
		}
	}
	float xindLup2 = sumxindLup2 / sumNumLup2;

	//TOP 加左减右, xindRup2代表左图,xindRup2代表上图
	xbiasLup = xindLup1 - xindLup2;//正值代表左移动,负值代表右移动
	if (fabs(xbiasLup)>maxDiff)
	{
		//xbiasLup = 0.0;
		cout << "left top corner error, current scene not meet the requirement or camera move too large!" << endl;
		cout << "diff: " << fabs(xbiasLup) << endl;
		return -1;
	}
	else if (fabs(xbiasLup)<1)
	{
		xbiasLup = 0.0;
	}
	else
	{
		xbiasLup = xbiasLup;
	}

	//俯视图右下角
	//double carRdown_x = 489;
	//double carRdown_y = 996;


	vector<int> recordRdown1;
	vector<int> recordRdown2;
	int num2 = 0;

	recordRdown1.clear();
	for (int i = carRdown_y; i<srcHeight - 10; i++)
	{
		uchar* pImgYdata = pImgData[3] + i*srcStep[3];
		//for (int j = srcWidth - 10; j>carRdown_x; j--)
		for (int j = carRdown_x; j < srcWidth - 10; j++)
		{
			if (pImgYdata[j]>0)
			{
				if (i*1.0 < KRdown*j + bRdown)
				{
					recordRdown1.push_back(j);
					num2++;
				}
				break;		// 找到第一个点后返回
			}
		}

		if (num2>2)
		{
			float xdifftmp = recordRdown1.at(num2 - 1) - recordRdown1.at(num2 - 2);
			if (fabs(xdifftmp) > maxDiff)
			{
				printf("%f, %d, (%d, %d)\n", carRdown_x, num2, recordRdown1.at(num2 - 1), recordRdown1.at(num2 - 2));
				cout << "right botton corner 1 error, current scene not meet the requirement or camera move too large!" << endl;
				return -1;
			}
			if (fabs(xdifftmp) > minDiff)//10
			{
				break;
			}
		}
	}

	num2 = 0;
	for (int i = srcHeight - 10; i>carRdown_y; i--)
	{
		uchar* pImgYdata = pImgData[1] + i*srcStep[1];
		//for (int j = srcWidth - 10; j>carRdown_x; j--)
		for (int j = carRdown_x; j < srcWidth - 10; j++)
		{
			if (pImgYdata[j]>0)
			{
				if (i*1.0 > KRdown*j + bRdown)
				{
					recordRdown2.push_back(j);
					num2 = num2 + 1;
				}
				break;		// 找到第一个点后返回
			}
		}

		if (num2>2)
		{
			float xdifftmp = recordRdown2.at(num2 - 1) - recordRdown2.at(num2 - 2);
			if (fabs(xdifftmp) > maxDiff)
			{
				cout << "right botton corner 2 error, current scene not meet the requirement or camera move too large!" << endl;
				return -1;
			}
			if (fabs(xdifftmp) > minDiff)//10
			{
				break;
			}
		}
	}

	int RdownNum1 = recordRdown1.size();
	int RdownNum2 = recordRdown2.size();
	float sumxindRdown1 = 0;
	float sumNumRdown1 = 0;

	if (RdownNum1>30)
	{
		for (int i = RdownNum1 - 20 - 1; i<RdownNum1 - 1; i++)
		{
			sumxindRdown1 = sumxindRdown1 + recordRdown1.at(i);
			sumNumRdown1 = sumNumRdown1 + 1;
		}
	}

	float xindRdown1 = sumxindRdown1 / sumNumRdown1;

	float sumxindRdown2 = 0;
	float sumNumRdown2 = 0;

	if (RdownNum2>30)
	{
		for (int i = RdownNum2 - 20 - 1; i<RdownNum2 - 1; i++)
		{
			sumxindRdown2 = sumxindRdown2 + recordRdown2.at(i);
			sumNumRdown2 = sumNumRdown2 + 1;
		}
	}
	float xindRdown2 = sumxindRdown2 / sumNumRdown2;

	//BOTTOM 左减右加 , xindRdown1 代表右图, xindRdown2 代表下图

	xbiasRdown = xindRdown1 - xindRdown2;//负值代表左移动,正值代表右移动
	if (fabs(xbiasRdown) > maxDiff)
	{
		//xbiasRdown = 0.0;
		cout << "right botton corner error, current scene not meet the requirement or camera move too large!" << endl;
		cout << "diff: " << fabs(xbiasRdown) << endl;
		return -1;
	}
	else if (fabs(xbiasRdown)<1)
	{
		xbiasRdown = 0.0;
	}
	else
	{
		xbiasRdown = xbiasRdown;
	}


	//俯视图左下角
	//double carLdown_x = 232;
	//double carLdown_y = 996;

	vector<float> recordLdown1;
	vector<float> recordLdown2;
	int num3 = 0;

	for (int i = carLdown_y; i<srcHeight - 10; i++)
	{
		uchar* pImgYdata = pImgData[2] + i*srcStep[2];
		for (int j = carLdown_x; j>10; j--)
		{
			if (pImgYdata[j]>0)
			{
				if (i*1.0 < KLdown*j + bLdown)
				{
					recordLdown1.push_back(j);
					num3 = num3 + 1;
				}
				break;		// 找到第一个点后返回
			}
		}

		if (num3>2)
		{
			float xdifftmp = recordLdown1.at(num3 - 1) - recordLdown1.at(num3 - 2);
			if (fabs(xdifftmp) > maxDiff)
			{
				cout << "left botton corner 1 error, current scene not meet the requirement or camera move too large!" << endl;
				return -1;
			}
			if (fabs(xdifftmp) > minDiff)//10
			{
				break;
			}
		}
	}

	num3 = 0;
	for (int i = srcHeight - 10; i>carLdown_y; i--)
	{
		uchar* pImgYdata = pImgData[1] + i*srcStep[1];
		for (int j = carLdown_x; j>10; j--)
		{
			if (pImgYdata[j]>0)
			{
				if (i*1.0 > KLdown*j + bLdown)
				{
					recordLdown2.push_back(j);
					num3++;
				}
				break;		// 找到第一个点后返回
			}
		}

		if (num3>2)
		{
			float xdifftmp = recordLdown2.at(num3 - 1) - recordLdown2.at(num3 - 2);
			if (fabs(xdifftmp) > maxDiff)
			{
				cout << "left botton corner 2 error, current scene not meet the requirement or camera move too large!" << endl;
				return -1;
			}
			if (fabs(xdifftmp) > minDiff)//10
			{
				break;
			}
		}
	}

	int LdownNum1 = recordLdown1.size();
	int LdownNum2 = recordLdown2.size();

	float sumxindLdown1 = 0;
	float sumNumLdown1 = 0;

	if (LdownNum1>30)
	{
		for (int i = LdownNum1 - 20 - 1; i<LdownNum1 - 1; i++)
		{
			sumxindLdown1 = sumxindLdown1 + recordLdown1.at(i);
			sumNumLdown1 = sumNumLdown1 + 1;
		}
	}
	float xindLdown1 = sumxindLdown1 / sumNumLdown1;

	float sumxindLdown2 = 0;
	float sumNumLdown2 = 0;

	if (LdownNum2>30)
	{
		for (int i = LdownNum2 - 20 - 1; i<LdownNum2 - 1; i++)
		{
			sumxindLdown2 = sumxindLdown2 + recordLdown2.at(i);
			sumNumLdown2 = sumNumLdown2 + 1;
		}
	}

	float xindLdown2 = sumxindLdown2 / sumNumLdown2;

	//BOTTOM 左减右加 , xindLdown1 代表左图, xindLdown2 代表下图

	xbiasLdown = xindLdown1 - xindLdown2;
	if (fabs(xbiasLdown)>maxDiff)
	{
		//xbiasLdown = 0.0;
		cout << "left botton corner error, current scene not meet the requirement or camera move too large!" << endl;
		cout << "diff: " << fabs(xbiasLdown) << endl;
		return -1;
	}
	else if (fabs(xbiasLdown)<1)
	{
		xbiasLdown = 0.0;
	}
	else
	{
		xbiasLdown = xbiasLdown;
	}

	//cv::line(canny_Mat, pt1Lup, pt2Lup, Scalar(255, 255, 255), 1);
	//cv::line(canny_Mat, pt1Rup, pt2Rup, Scalar(255, 255, 255), 1);
	//cv::line(canny_Mat, pt1Ldown, pt2Ldown, Scalar(255, 255, 255), 1);
	//cv::line(canny_Mat, pt1Rdown, pt2Rdown, Scalar(255, 255, 255), 1);

	//imwrite("tempimg//AfterCanny.bmp", canny_Mat);

	return 0;
};

bool C360LookAroundMontageAlg::OnGenLaneLineOptimizedTable(void* pParam, float& xbiasLup, float& xbiasRup, float& xbiasLdown, float& xbiasRdown)
{
	bool bReturn = false;
	int i = 0;
	vector<vector<cv::Point3f> > pRect;

	m_p360Param = (stu_360Param*)pParam;

	m_speed_process = m_p360Param->iProcess;

	m_speed_process[0] = 0;

	//	m_speed_process = 0;
	//////////////////////////////////////////////////////////////////////////
	// 通过车辆长度来确定是房车还是宝沃BX7
	// 因为这两辆车的长度不一样
	if (m_p360Param->world_car_size.height < CAR_DIVISION)           // 宝沃
	{
		m_CalibrateScheme = BAOWO_BX7;
	}
	else                                                               // 房车
	{
		m_CalibrateScheme = TOURING_CAR;
	}

	//////////////////////////////////////////////////////////////////////////
	// 获取世界坐标系坐标及棋盘格维度
	bReturn = OnStructWorldCoordinate(m_CalibrateScheme);

	if (!bReturn)
	{
		m_errorCode = READ_INIFILE_ERROR;

		return bReturn;
	}

	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	// 标定
	m_bCalibrated = true;

	int size = FISHEYE_COUNT * (m_Calibrate[0].m_fishInf.innerParam.channels + 2 * m_Calibrate[0].m_fishInf.RT[0].channels + m_Calibrate[0].m_fishInf.distortionParam.channels);
	double *data = new double[size];
	double *pdata = data;

	// 写表
	FILE*file = NULL;
	if (file = fopen("tempimg//fish_param.bin", "rb"))
	{
		fread(data, sizeof(double), size, file);
		fclose(file);
	}

	for (int i = 0; i < FISHEYE_COUNT; i++)
	{
		for (int j = 0; j < m_Calibrate[0].m_fishInf.innerParam.channels; j++)
			m_Calibrate[i].m_fishInf.innerParam.val[j] = *pdata++;

		for (int k = 0; k < 2; k++)
			for (int j = 0; j < m_Calibrate[0].m_fishInf.RT[0].channels; j++)
				m_Calibrate[i].m_fishInf.RT[k].val[j] = *pdata++;

		for (int j = 0; j < m_Calibrate[0].m_fishInf.distortionParam.channels; j++)
			m_Calibrate[i].m_fishInf.distortionParam.val[j] = *pdata++;
	}

	float widePerPixel = m_p360Param->world_show_size.width / 720;               // 每一像素点代表世界坐标系多长的宽度，下同
	m_Calibrate[0].m_fishInf.RT[0].val[3] += widePerPixel * xbiasLup;
	m_Calibrate[0].m_fishInf.RT[1].val[3] += widePerPixel * xbiasRup;

	m_Calibrate[1].m_fishInf.RT[0].val[3] += widePerPixel * xbiasLdown;
	m_Calibrate[1].m_fishInf.RT[1].val[3] += widePerPixel * xbiasRdown;
	return bReturn;
}

void C360LookAroundMontageAlg::OnGetBackAssistSpreedImg(cv::Mat spread_img, cv::Point *pbackAssistPoints)
{
	cv::line(spread_img, pbackAssistPoints[0], pbackAssistPoints[2], cv::Scalar(0, 255, 255), 1, 8 );	
	cv::line(spread_img, pbackAssistPoints[1], pbackAssistPoints[3], cv::Scalar(0, 255, 255), 1, 8 );	
	cv::line(spread_img, pbackAssistPoints[2], pbackAssistPoints[3], cv::Scalar(0, 255, 255), 1, 8 );	
	cv::line(spread_img, pbackAssistPoints[2], pbackAssistPoints[4], cv::Scalar( 0, 255, 0 ), 1, 8 );	
	cv::line(spread_img, pbackAssistPoints[3], pbackAssistPoints[5], cv::Scalar( 0, 255, 0 ), 1, 8 );	
	cv::line(spread_img, pbackAssistPoints[4], pbackAssistPoints[5], cv::Scalar( 0, 255, 0 ), 1, 8 );

	imwrite( "tempimg//back_spread.jpg" , spread_img );

	//imshow("back_spread", spread_img);
	//waitKey(0);
}

void PolyLines1(cv::Mat *raw_img, vector<cv::Point> *rawLeft1Points, vector<cv::Point> *rawLeft2Points, vector<cv::Point> *rawLeft3Points,
	vector<cv::Point> *rawRight1Points, vector<cv::Point> *rawRight2Points, vector<cv::Point> *rawRight3Points, vector<cv::Point> *rawWarnPoints)
{
	cv::polylines(*raw_img, *rawLeft3Points, false, cv::Scalar(0, 255, 0, 255), 3, CV_AA, 0);
	cv::polylines(*raw_img, *rawLeft2Points, false, cv::Scalar(0, 255, 255, 255), 3, CV_AA, 0);
	cv::polylines(*raw_img, *rawLeft1Points, false, cv::Scalar(0, 0, 255, 255), 3, CV_AA, 0);

	cv::polylines(*raw_img, *rawRight3Points, false, cv::Scalar(0, 255, 0, 255), 3, CV_AA, 0);
	cv::polylines(*raw_img, *rawRight2Points, false, cv::Scalar(0, 255, 255, 255), 3, CV_AA, 0);
	cv::polylines(*raw_img, *rawRight1Points, false, cv::Scalar(0, 0, 255, 255), 3, CV_AA, 0);

	cv::polylines(*raw_img, *rawWarnPoints, false, cv::Scalar(0, 0, 255, 255), 3, CV_AA, 0);
}

void PolyLines2(cv::Mat *raw_img, vector<cv::Point> *rawNoticePoints, vector<cv::Point> *rawSafePoints)
{
	cv::polylines(*raw_img, *rawNoticePoints, false, cv::Scalar(0, 255, 255, 255), 3, CV_AA, 0);
	cv::polylines(*raw_img, *rawSafePoints, false, cv::Scalar(0, 255, 0, 255), 3, CV_AA, 0);
}

void C360LookAroundMontageAlg::OnGetBackAssistRawImg(cv::Mat raw_img, cv::Point *pbackAssistPoints)
{
	cv::Point rawPoint;
	int size = 0;
	int index = 0;
	vector<cv::Point> rawLeft1Points;
	vector<cv::Point> rawLeft2Points;
	vector<cv::Point> rawLeft3Points;

	vector<cv::Point> rawRight1Points;
	vector<cv::Point> rawRight2Points;
	vector<cv::Point> rawRight3Points;

	vector<cv::Point> rawWarnPoints;
	vector<cv::Point> rawNoticePoints;
	vector<cv::Point> rawSafePoints;

	rawLeft1Points.clear();
	rawLeft2Points.clear();
	rawLeft3Points.clear();

	rawRight1Points.clear();
	rawRight2Points.clear();
	rawRight3Points.clear();

	rawWarnPoints.clear();
	rawNoticePoints.clear();
	rawSafePoints.clear();

	size = pbackAssistPoints[index++].x;
	for (int i = 0; i < size; i++)
	{
		rawLeft3Points.push_back(pbackAssistPoints[index++]);
	}

	size = pbackAssistPoints[index++].x;
	for (int i = 0; i < size; i++)
	{
		rawLeft2Points.push_back(pbackAssistPoints[index++]);
	}

	size = pbackAssistPoints[index++].x;
	for (int i = 0; i < size; i++)
	{
		rawLeft1Points.push_back(pbackAssistPoints[index++]);
	}


	size = pbackAssistPoints[index++].x;
	for (int i = 0; i < size; i++)
	{
		rawRight3Points.push_back(pbackAssistPoints[index++]);
	}

	size = pbackAssistPoints[index++].x;
	for (int i = 0; i < size; i++)
	{
		rawRight2Points.push_back(pbackAssistPoints[index++]);
	}

	size = pbackAssistPoints[index++].x;
	for (int i = 0; i < size; i++)
	{
		rawRight1Points.push_back(pbackAssistPoints[index++]);
	}

	size = pbackAssistPoints[index++].x;
	for (int i = 0; i < size; i++)
	{
		rawWarnPoints.push_back(pbackAssistPoints[index++]);
	}

	size = pbackAssistPoints[index++].x;
	for (int i = 0; i < size; i++)
	{
		rawNoticePoints.push_back(pbackAssistPoints[index++]);
	}

	size = pbackAssistPoints[index++].x;
	for (int i = 0; i < size; i++)
	{
		rawSafePoints.push_back(pbackAssistPoints[index++]);
	}

	//cv::polylines(raw_img, rawLeft1Points, false, cv::Scalar(0, 0, 255, 255), 3, CV_AA, 0);
	//cv::polylines(raw_img, rawLeft2Points, false, cv::Scalar(0, 255, 255, 255), 3, CV_AA, 0);
	//cv::polylines(raw_img, rawLeft3Points, false, cv::Scalar(0, 255, 0, 255), 3, CV_AA, 0);

	//cv::polylines(raw_img, rawRight1Points, false, cv::Scalar(0, 0, 255, 255), 3, CV_AA, 0);
	//cv::polylines(raw_img, rawRight2Points, false, cv::Scalar(0, 255, 255, 255), 3, CV_AA, 0);
	//cv::polylines(raw_img, rawRight3Points, false, cv::Scalar(0, 255, 0, 255), 3, CV_AA, 0);

	//cv::polylines(raw_img, rawWarnPoints, false, cv::Scalar(0, 0, 255, 255), 3, CV_AA, 0);
	//cv::polylines(raw_img, rawNoticePoints, false, cv::Scalar(0, 255, 255, 255), 3, CV_AA, 0);
	//cv::polylines(raw_img, rawSafePoints, false, cv::Scalar(0, 255, 0, 255), 3, CV_AA, 0);

	std::thread line1(PolyLines1, &raw_img, &rawLeft1Points, &rawLeft2Points, &rawLeft3Points, &rawRight1Points, &rawRight2Points, &rawRight3Points, &rawWarnPoints);
	std::thread line2(PolyLines2, &raw_img, &rawNoticePoints, &rawSafePoints);

	line1.join();
	line2.join();

	//imwrite("tempimg//back_raw.jpg", raw_img);
	//imshow("back_raw", raw_img);
	//waitKey(0);
}

void C360LookAroundMontageAlg::OnTestGetBackAssistImg(cv::Mat src, stu2dTable *p2dTable, stu_spreadImg *pSpreadTable, cv::Mat &dst)
{
	ifstream inf;
	char c;
	inf.open("car.txt");
	inf >> m_iCarBeginX >> c >> m_iCarBeginY >> c >> m_iCarWide >> c >> m_iCarHeight;
	inf.close();

	m_heightPerPixel = 18400.0 / 1280;

	printf("%d, %d, %d, %d\n", m_iCarBeginX, m_iCarBeginY, m_iCarWide, m_iCarHeight);

	cv::Point carTailBegin = { m_iCarBeginX, m_iCarBeginY + m_iCarHeight };
	cv::Point carTailEnd = { m_iCarBeginX + m_iCarWide, m_iCarBeginY + m_iCarHeight };
	cv::Point WarnLineBegin = { carTailBegin.x, carTailBegin.y + (int)(WARN_LINE_DISTANCE / m_heightPerPixel) };
	cv::Point WarnLineEnd = { carTailEnd.x, carTailEnd.y + (int)(WARN_LINE_DISTANCE / m_heightPerPixel) };
	cv::Point NoticeLineBegin = { carTailBegin.x, carTailBegin.y + (int)(NOTICE_LINE_DISTANCE / m_heightPerPixel) };
	cv::Point NoticeLineEnd = { carTailEnd.x, carTailEnd.y + (int)(NOTICE_LINE_DISTANCE / m_heightPerPixel) };
	cv::Point SafeLineBegin = { carTailBegin.x, carTailBegin.y + (int)(SAFE_LINE_DISTANCE / m_heightPerPixel) };
	cv::Point SafeLineEnd = { carTailEnd.x, carTailEnd.y + (int)(SAFE_LINE_DISTANCE / m_heightPerPixel) };

	int x;
	int y;

	cv::Point rawPoint;
	cv::Point spreadPoint;
	vector<cv::Point> rawLeftPoints;
	vector<cv::Point> rawLeft1Points;
	vector<cv::Point> rawLeft2Points;
	vector<cv::Point> rawLeft3Points;

	vector<cv::Point> rawRightPoints;
	vector<cv::Point> rawRight1Points;
	vector<cv::Point> rawRight2Points;
	vector<cv::Point> rawRight3Points;

	vector<cv::Point> rawWarnPoints;
	vector<cv::Point> rawNoticePoints;
	vector<cv::Point> rawSafePoints;
	
	vector<cv::Point> spreadLeftPoints;
	vector<cv::Point> spreadRightPoints;
	vector<cv::Point> spreadNoticePoints;
	vector<cv::Point> spreadSafePoints;

	rawLeftPoints.clear();
	rawLeft1Points.clear();
	rawLeft2Points.clear();
	rawLeft3Points.clear();

	rawRight1Points.clear();
	rawRight2Points.clear();
	rawRight3Points.clear();

	rawWarnPoints.clear();
	rawNoticePoints.clear();
	rawSafePoints.clear();
	//for (int i = carTailBegin.y; i < SafeLineBegin.y; i++)
	//{
	//	if (p2dTable[i * 720 + carTailBegin.x].img1Idx == 1)
	//	{
	//		rawPoint.x = p2dTable[i * 720 + carTailBegin.x].img1Location.x;
	//		rawPoint.y = p2dTable[i * 720 + carTailBegin.x].img1Location.y;
	//		//printf("left index %d: (%d, %d)\n", i, p2dTable[i * 720 + carTailBegin.x].iLocation.x, p2dTable[i * 720 + carTailBegin.x].iLocation.y);
	//		//printf("left idx: %d(%d, %d)\n", p2dTable[i * 720 + carTailBegin.x].img1Idx, rawPoint.x, rawPoint.y);

	//		rawLeftPoints.push_back(rawPoint);
	//	}
	//}

	for(int i = carTailBegin.y; i < WarnLineBegin.y; i++)
	{
		if(p2dTable[i * 720 + carTailBegin.x].img1Idx == 1)
		{
			rawPoint.x = p2dTable[i * 720 + carTailBegin.x].img1Location.x;
			rawPoint.y = p2dTable[i * 720 + carTailBegin.x].img1Location.y;
			//printf("left index %d: (%d, %d)\n", i, p2dTable[i * 720 + carTailBegin.x].iLocation.x, p2dTable[i * 720 + carTailBegin.x].iLocation.y);
			//printf("left idx: %d(%d, %d)\n", p2dTable[i * 720 + carTailBegin.x].img1Idx, rawPoint.x, rawPoint.y);

			rawLeft1Points.push_back(rawPoint);
		}
	}

	for (int i = WarnLineBegin.y; i < NoticeLineBegin.y; i++)
	{
		if (p2dTable[i * 720 + carTailBegin.x].img1Idx == 1)
		{
			rawPoint.x = p2dTable[i * 720 + carTailBegin.x].img1Location.x;
			rawPoint.y = p2dTable[i * 720 + carTailBegin.x].img1Location.y;
			//printf("left index %d: (%d, %d)\n", i, p2dTable[i * 720 + carTailBegin.x].iLocation.x, p2dTable[i * 720 + carTailBegin.x].iLocation.y);
			//printf("left idx: %d(%d, %d)\n", p2dTable[i * 720 + carTailBegin.x].img1Idx, rawPoint.x, rawPoint.y);

			rawLeft2Points.push_back(rawPoint);
		}
	}

	for (int i = NoticeLineBegin.y; i < SafeLineBegin.y; i++)
	{
		if (p2dTable[i * 720 + carTailBegin.x].img1Idx == 1)
		{
			rawPoint.x = p2dTable[i * 720 + carTailBegin.x].img1Location.x;
			rawPoint.y = p2dTable[i * 720 + carTailBegin.x].img1Location.y;
			//printf("left index %d: (%d, %d)\n", i, p2dTable[i * 720 + carTailBegin.x].iLocation.x, p2dTable[i * 720 + carTailBegin.x].iLocation.y);
			//printf("left idx: %d(%d, %d)\n", p2dTable[i * 720 + carTailBegin.x].img1Idx, rawPoint.x, rawPoint.y);

			rawLeft3Points.push_back(rawPoint);
		}
	}

	for (vector<cv::Point>::iterator it = rawLeft1Points.begin(); it != rawLeft1Points.end(); it++)
	{
		rawLeftPoints.push_back(*it);
	}

	for (vector<cv::Point>::iterator it = rawLeft2Points.begin(); it != rawLeft2Points.end(); it++)
	{
		rawLeftPoints.push_back(*it);
	}

	for (vector<cv::Point>::iterator it = rawLeft3Points.begin(); it != rawLeft3Points.end(); it++)
	{
		rawLeftPoints.push_back(*it);
	}

	//for (int i = carTailEnd.y; i < SafeLineEnd.y; i++)
	//{
	//	if (p2dTable[i * 720 + carTailEnd.x].img1Idx == 1)
	//	{
	//		rawPoint.x = p2dTable[i * 720 + carTailEnd.x].img1Location.x;
	//		rawPoint.y = p2dTable[i * 720 + carTailEnd.x].img1Location.y;
	//		//printf("right index %d: (%d, %d)\n", i, p2dTable[i * 720 + carTailEnd.x].iLocation.x, p2dTable[i * 720 + carTailEnd.x].iLocation.y);
	//		//printf("right idx: %d(%d, %d)\n", p2dTable[i * 720 + carTailEnd.x].img1Idx, rawPoint.x, rawPoint.y);

	//		rawRightPoints.push_back(rawPoint);
	//	}
	//}

	for(int i = carTailEnd.y; i < WarnLineEnd.y; i++)
	{
		if(p2dTable[i * 720 + carTailEnd.x].img1Idx == 1)
		{
			rawPoint.x = p2dTable[i * 720 + carTailEnd.x].img1Location.x;
			rawPoint.y = p2dTable[i * 720 + carTailEnd.x].img1Location.y;
			//printf("right index %d: (%d, %d)\n", i, p2dTable[i * 720 + carTailEnd.x].iLocation.x, p2dTable[i * 720 + carTailEnd.x].iLocation.y);
			//printf("right idx: %d(%d, %d)\n", p2dTable[i * 720 + carTailEnd.x].img1Idx, rawPoint.x, rawPoint.y);

			rawRight1Points.push_back(rawPoint);
		}
	}

	for (int i = WarnLineEnd.y; i < NoticeLineEnd.y; i++)
	{
		if (p2dTable[i * 720 + carTailEnd.x].img1Idx == 1)
		{
			rawPoint.x = p2dTable[i * 720 + carTailEnd.x].img1Location.x;
			rawPoint.y = p2dTable[i * 720 + carTailEnd.x].img1Location.y;
			//printf("right index %d: (%d, %d)\n", i, p2dTable[i * 720 + carTailEnd.x].iLocation.x, p2dTable[i * 720 + carTailEnd.x].iLocation.y);
			//printf("right idx: %d(%d, %d)\n", p2dTable[i * 720 + carTailEnd.x].img1Idx, rawPoint.x, rawPoint.y);

			rawRight2Points.push_back(rawPoint);
		}
	}

	for (int i = NoticeLineEnd.y; i < SafeLineEnd.y; i++)
	{
		if (p2dTable[i * 720 + carTailEnd.x].img1Idx == 1)
		{
			rawPoint.x = p2dTable[i * 720 + carTailEnd.x].img1Location.x;
			rawPoint.y = p2dTable[i * 720 + carTailEnd.x].img1Location.y;
			//printf("right index %d: (%d, %d)\n", i, p2dTable[i * 720 + carTailEnd.x].iLocation.x, p2dTable[i * 720 + carTailEnd.x].iLocation.y);
			//printf("right idx: %d(%d, %d)\n", p2dTable[i * 720 + carTailEnd.x].img1Idx, rawPoint.x, rawPoint.y);

			rawRight3Points.push_back(rawPoint);
		}
	}

	for (vector<cv::Point>::iterator it = rawRight1Points.begin(); it != rawRight1Points.end(); it++)
	{
		rawRightPoints.push_back(*it);
	}

	for (vector<cv::Point>::iterator it = rawRight2Points.begin(); it != rawRight2Points.end(); it++)
	{
		rawRightPoints.push_back(*it);
	}

	for (vector<cv::Point>::iterator it = rawRight3Points.begin(); it != rawRight3Points.end(); it++)
	{
		rawRightPoints.push_back(*it);
	}

	for(int i = WarnLineBegin.x; i < WarnLineEnd.x; i++)
	{
		if(p2dTable[WarnLineEnd.y * 720 + i].img1Idx == 1)
		{
			rawPoint.x = p2dTable[WarnLineEnd.y * 720 + i].img1Location.x;
			rawPoint.y = p2dTable[WarnLineEnd.y * 720 + i].img1Location.y;
			//printf("index %d: (%d, %d)\n", i, p2dTable[WarnLineEnd.y * 720 + i].iLocation.x, p2dTable[WarnLineEnd.y * 720 + i].iLocation.y);
			//printf("idx: %d(%d, %d)\n", p2dTable[WarnLineEnd.y * 720 + i].img1Idx, rawPoint.x, rawPoint.y);

			rawWarnPoints.push_back(rawPoint);
		}
	}

	for(int i = NoticeLineBegin.x; i < NoticeLineEnd.x; i++)
	{
		if(p2dTable[NoticeLineEnd.y * 720 + i].img1Idx == 1)
		{
			rawPoint.x = p2dTable[NoticeLineEnd.y * 720 + i].img1Location.x;
			rawPoint.y = p2dTable[NoticeLineEnd.y * 720 + i].img1Location.y;
			//printf("index %d: (%d, %d)\n", i, p2dTable[NoticeLineEnd.y * 720 + i].iLocation.x, p2dTable[NoticeLineEnd.y * 720 + i].iLocation.y);
			//printf("idx: %d(%d, %d)\n", p2dTable[NoticeLineEnd.y * 720 + i].img1Idx, rawPoint.x, rawPoint.y);

			rawNoticePoints.push_back(rawPoint);
		}
	}
	
	for(int i = SafeLineBegin.x; i < SafeLineEnd.x; i++)
	{
		if(p2dTable[SafeLineEnd.y * 720 + i].img1Idx == 1)
		{
			rawPoint.x = p2dTable[SafeLineEnd.y * 720 + i].img1Location.x;
			rawPoint.y = p2dTable[SafeLineEnd.y * 720 + i].img1Location.y;
			//printf("index %d: (%d, %d)\n", i, p2dTable[SafeLineEnd.y * 720 + i].iLocation.x, p2dTable[SafeLineEnd.y * 720 + i].iLocation.y);
			//printf("idx: %d(%d, %d)\n", p2dTable[SafeLineEnd.y * 720 + i].img1Idx, rawPoint.x, rawPoint.y);

			rawSafePoints.push_back(rawPoint);
		}
	}

#if 0
	cv::polylines(src, rawLeftPoints, false, cv::Scalar(0, 255, 0), 1, 8, 0);
	cv::polylines(src, rawRightPoints, false, cv::Scalar(0, 255, 0), 1, 8, 0);
	cv::polylines(src, rawWarnPoints, false, cv::Scalar(0, 0, 255), 1, 8, 0);
	cv::polylines(src, rawNoticePoints, false, cv::Scalar(255, 0, 255), 1, 8, 0);
	cv::polylines(src, rawSafePoints, false, cv::Scalar(0, 255, 0), 1, 8, 0);
#else

	// 曲线拟合
	cv::Mat A;
	std::vector<cv::Point> points_fitted;
	cv::Point prePoint;
	int index = 0;

	cv::Mat backAssistImg;
	backAssistImg.create(720, 1280, CV_8UC4);
	backAssistImg.setTo(cv::Scalar(0, 0, 0, 0));

	polynomial_curve_fit(rawLeft1Points, 3, A);
	std::cout << "A = " << A << std::endl;

	points_fitted.clear();
	for (int i = 0; i < rawLeft1Points.size(); i++)
	{
		int x = rawLeft1Points.at(i).x;
		double y = A.at<double>(0, 0) + A.at<double>(1, 0) * x +
			A.at<double>(2, 0)*std::pow(x, 2) + A.at<double>(3, 0)*std::pow(x, 3);

		points_fitted.push_back(cv::Point(x, y));
	}

	prePoint = points_fitted.front();
	for (vector<cv::Point>::iterator it = points_fitted.begin() + 1; it != points_fitted.end();)
	{
		if ((*it) == prePoint)
		{
			it = points_fitted.erase(it);
			continue;
		}
		prePoint = (*it);
		it++;
	}

	cv::polylines(backAssistImg, points_fitted, false, cv::Scalar(0, 0, 255, 255), 3, CV_AA, 0);

	polynomial_curve_fit(rawLeft2Points, 3, A);
	std::cout << "A = " << A << std::endl;

	points_fitted.clear();
	for (int i = 0; i < rawLeft2Points.size(); i++)
	{
		int x = rawLeft2Points.at(i).x;
		double y = A.at<double>(0, 0) + A.at<double>(1, 0) * x +
			A.at<double>(2, 0)*std::pow(x, 2) + A.at<double>(3, 0)*std::pow(x, 3);

		points_fitted.push_back(cv::Point(x, y));
	}

	prePoint = points_fitted.front();

	for (vector<cv::Point>::iterator it = points_fitted.begin() + 1; it != points_fitted.end();)
	{
		if ((*it) == prePoint)
		{
			it = points_fitted.erase(it);
			continue;
		}
		prePoint = (*it);
		it++;
	}

	cv::polylines(backAssistImg, points_fitted, false, cv::Scalar(0, 255, 255, 255), 3, CV_AA, 0);


	polynomial_curve_fit(rawLeft3Points, 3, A);
	std::cout << "A = " << A << std::endl;

	points_fitted.clear();
	for (int i = 0; i < rawLeft3Points.size(); i++)
	{
		int x = rawLeft3Points.at(i).x;
		double y = A.at<double>(0, 0) + A.at<double>(1, 0) * x +
			A.at<double>(2, 0)*std::pow(x, 2) + A.at<double>(3, 0)*std::pow(x, 3);

		points_fitted.push_back(cv::Point(x, y));
	}

	prePoint = points_fitted.front();

	for (vector<cv::Point>::iterator it = points_fitted.begin() + 1; it != points_fitted.end();)
	{
		if ((*it) == prePoint)
		{
			it = points_fitted.erase(it);
			continue;
		}
		prePoint = (*it);
		it++;
	}

	cv::polylines(backAssistImg, points_fitted, false, cv::Scalar(0, 255, 0, 255), 3, CV_AA, 0);



	polynomial_curve_fit(rawRight1Points, 3, A);
	std::cout << "A = " << A << std::endl;

	points_fitted.clear();
	for (int i = 0; i < rawRight1Points.size(); i++)
	{
		int x = rawRight1Points.at(i).x;
		double y = A.at<double>(0, 0) + A.at<double>(1, 0) * x +
			A.at<double>(2, 0)*std::pow(x, 2) + A.at<double>(3, 0)*std::pow(x, 3);

		points_fitted.push_back(cv::Point(x, y));
	}

	prePoint = points_fitted.front();

	for (vector<cv::Point>::iterator it = points_fitted.begin() + 1; it != points_fitted.end();)
	{
		if ((*it) == prePoint)
		{
			it = points_fitted.erase(it);
			continue;
		}
		prePoint = (*it);
		it++;
	}

	cv::polylines(backAssistImg, points_fitted, false, cv::Scalar(0, 0, 255, 255), 3, CV_AA, 0);


	polynomial_curve_fit(rawRight2Points, 3, A);
	std::cout << "A = " << A << std::endl;

	points_fitted.clear();
	for (int i = 0; i < rawRight2Points.size(); i++)
	{
		int x = rawRight2Points.at(i).x;
		double y = A.at<double>(0, 0) + A.at<double>(1, 0) * x +
			A.at<double>(2, 0)*std::pow(x, 2) + A.at<double>(3, 0)*std::pow(x, 3);

		points_fitted.push_back(cv::Point(x, y));
	}

	prePoint = points_fitted.front();

	for (vector<cv::Point>::iterator it = points_fitted.begin() + 1; it != points_fitted.end();)
	{
		if ((*it) == prePoint)
		{
			it = points_fitted.erase(it);
			continue;
		}
		prePoint = (*it);
		it++;
	}

	cv::polylines(backAssistImg, points_fitted, false, cv::Scalar(0, 255, 255, 255), 3, CV_AA, 0);

	polynomial_curve_fit(rawRight3Points, 3, A);
	std::cout << "A = " << A << std::endl;

	points_fitted.clear();
	for (int i = 0; i < rawRight3Points.size(); i++)
	{
		int x = rawRight3Points.at(i).x;
		double y = A.at<double>(0, 0) + A.at<double>(1, 0) * x +
			A.at<double>(2, 0)*std::pow(x, 2) + A.at<double>(3, 0)*std::pow(x, 3);

		points_fitted.push_back(cv::Point(x, y));
	}

	prePoint = points_fitted.front();

	for (vector<cv::Point>::iterator it = points_fitted.begin() + 1; it != points_fitted.end();)
	{
		if ((*it) == prePoint)
		{
			it = points_fitted.erase(it);
			continue;
		}
		prePoint = (*it);
		it++;
	}

	cv::polylines(backAssistImg, points_fitted, false, cv::Scalar(0, 255, 0, 255), 3, CV_AA, 0);


	polynomial_curve_fit(rawWarnPoints, 3, A);
	std::cout << "A = " << A << std::endl;

	points_fitted.clear();
	for (int i = 0; i < rawWarnPoints.size(); i++)
	{
		int x = rawWarnPoints.at(i).x;
		double y = A.at<double>(0, 0) + A.at<double>(1, 0) * x +
			A.at<double>(2, 0)*std::pow(x, 2) + A.at<double>(3, 0)*std::pow(x, 3);

		points_fitted.push_back(cv::Point(x, y));
	}

	index = 0;
	for (vector<cv::Point>::iterator it = points_fitted.begin(); it != points_fitted.end() - 1;)
	{
		if (index++ % 3)
		{
			it = points_fitted.erase(it);
			continue;
		}
		prePoint = (*it);
		it++;
	}

	cv::polylines(backAssistImg, points_fitted, false, cv::Scalar(0, 0, 255, 255), 3, CV_AA, 0);


	polynomial_curve_fit(rawNoticePoints, 3, A);
	std::cout << "A = " << A << std::endl;

	points_fitted.clear();
	for (int i = 0; i < rawNoticePoints.size(); i++)
	{
		int x = rawNoticePoints.at(i).x;
		double y = A.at<double>(0, 0) + A.at<double>(1, 0) * x +
			A.at<double>(2, 0)*std::pow(x, 2) + A.at<double>(3, 0)*std::pow(x, 3);

		points_fitted.push_back(cv::Point(x, y));
	}

	index = 0;
	for (vector<cv::Point>::iterator it = points_fitted.begin(); it != points_fitted.end() - 1;)
	{
		if (index++ % 3)
		{
			it = points_fitted.erase(it);
			continue;
		}
		prePoint = (*it);
		it++;
	}

	cv::polylines(backAssistImg, points_fitted, false, cv::Scalar(0, 255, 255, 255), 3, CV_AA, 0);

	
	polynomial_curve_fit(rawSafePoints, 3, A);
	std::cout << "A = " << A << std::endl;

	points_fitted.clear();
	for (int i = 0; i < rawSafePoints.size(); i++)
	{
		int x = rawSafePoints.at(i).x;
		double y = A.at<double>(0, 0) + A.at<double>(1, 0) * x +
			A.at<double>(2, 0)*std::pow(x, 2) + A.at<double>(3, 0)*std::pow(x, 3);

		points_fitted.push_back(cv::Point(x, y));
	}

	index = 0;
	for (vector<cv::Point>::iterator it = points_fitted.begin(); it != points_fitted.end() - 1;)
	{
		if (index++ % 3)
		{
			it = points_fitted.erase(it);
			continue;
		}
		prePoint = (*it);
		it++;
	}

	cv::polylines(backAssistImg, points_fitted, false, cv::Scalar(0, 255, 0, 255), 3, CV_AA, 0);
#endif
	
	imwrite("back_raw.png", backAssistImg);
	imshow("back_raw", backAssistImg);
	cv::waitKey(0);


	spreadLeftPoints.clear();
	spreadRightPoints.clear();
	spreadNoticePoints.clear();
	for(int i = 0; i < 720; i++)
	{
		for(int j = 0; j < 1280; j++)
		{
			for(int k = 0; k < rawLeftPoints.size(); k++)
			{
				if(rawLeftPoints.at(k).x == pSpreadTable[i * 1280 + j].iWidex && rawLeftPoints.at(k).y == pSpreadTable[i * 1280 + j].iHeighty)
				{
					//printf("raw spread idx: %d(%d, %d)\n", k, i, j);
					spreadPoint.x = j;
					spreadPoint.y = i;
					spreadLeftPoints.push_back(spreadPoint);
				}
			}

			for(int k = 0; k < rawRightPoints.size(); k++)
			{
				if(rawRightPoints.at(k).x == pSpreadTable[i * 1280 + j].iWidex && rawRightPoints.at(k).y == pSpreadTable[i * 1280 + j].iHeighty)
				{
					//printf("spread idx: %d(%d, %d)\n", k, i, j);
					spreadPoint.x = j;
					spreadPoint.y = i;
					spreadRightPoints.push_back(spreadPoint);
				}
			}

			for(int k = 0; k < rawNoticePoints.size(); k++)
			{
				if(rawNoticePoints.at(k).x == pSpreadTable[i * 1280 + j].iWidex && rawNoticePoints.at(k).y == pSpreadTable[i * 1280 + j].iHeighty)
				{
					//printf("spread idx: %d(%d, %d)\n", k, i, j);
					spreadPoint.x = j;
					spreadPoint.y = i;
					spreadNoticePoints.push_back(spreadPoint);
				}
			}

			for(int k = 0; k < rawSafePoints.size(); k++)
			{
				if(rawSafePoints.at(k).x == pSpreadTable[i * 1280 + j].iWidex && rawSafePoints.at(k).y == pSpreadTable[i * 1280 + j].iHeighty)
				{
					//printf("spread idx: %d(%d, %d)\n", k, i, j);
					spreadPoint.x = j;
					spreadPoint.y = i;
					spreadSafePoints.push_back(spreadPoint);
				}
			}				
		}
	}	

	cv::Mat spread_img = cv::imread("tempimg//img1.bmp");


	// 四、使用openCV将离散的点拟和成直线
	cv::Vec4f line;
	float cos_theta;
	float sin_theta;
	float x0, y0;
	float k;
	float b;

	// 拟合左边的点
	if (!spreadLeftPoints.empty())
	{
		cv::fitLine(spreadLeftPoints, line, CV_DIST_HUBER, 0, 0.01, 0.01);

		cos_theta = line[0];
		sin_theta = line[1];
		x0 = line[2], y0 = line[3];

		k = sin_theta / cos_theta;
		b = y0 - k * x0;
		fprintf(stdout, "k=%f,b=%f\r\n", k, b);
	}

	float k1;
	float b1;
	// 拟合右边的点
	if (!spreadRightPoints.empty())
	{
		cv::fitLine(spreadRightPoints, line, CV_DIST_HUBER, 0, 0.01, 0.01);

		cos_theta = line[0];
		sin_theta = line[1];
		x0 = line[2], y0 = line[3];

		k1 = sin_theta / cos_theta;
		b1 = y0 - k1 * x0;
		fprintf(stdout, "k1=%f,b1=%f\r\n", k1, b1);
	}

	float k2;
	float b2;
	// 拟合提示线的点
	if (!spreadNoticePoints.empty())
	{
		cv::fitLine(spreadNoticePoints, line, CV_DIST_HUBER, 0, 0.01, 0.01);

		cos_theta = line[0];
		sin_theta = line[1];
		x0 = line[2], y0 = line[3];

		k2 = sin_theta / cos_theta;
		b2 = y0 - k2 * x0;
		fprintf(stdout, "k2=%f,b2=%f\r\n", k1, b1);
	}

	float k3;
	float b3;
	// 拟合安全线的点
	if (!spreadSafePoints.empty())
	{
		cv::fitLine(spreadSafePoints, line, CV_DIST_HUBER, 0, 0.01, 0.01);

		cos_theta = line[0];
		sin_theta = line[1];
		x0 = line[2], y0 = line[3];

		k3 = sin_theta / cos_theta;
		b3 = y0 - k3 * x0;
		fprintf(stdout, "k3=%f,b3=%f\r\n", k3, b3);
	}
	// 五、求出各直线的交叉点
	cv::Point backAssist[6];
	cv::Point leftLineBegin;
	cv::Point rightLineBegin;
	cv::Point noticeBegin;
	cv::Point noticeEnd;
	cv::Point safeBegin;
	cv::Point safeEnd;
	float tempx, tempy;
	int height = 720;

	// 求出左边的线起始点
	tempx = (height - b) / k;
	leftLineBegin.x = tempx;
	leftLineBegin.y = height;

	// 求出右边的线起始点
	tempx = (height - b1) / k1;
	rightLineBegin.x = tempx;
	rightLineBegin.y = height;

	// 求出提示线左边起始点
	tempx = (b - b2) / (k2 - k);
	tempy = k * tempx + b;

	noticeBegin.x = (int)tempx;
	noticeBegin.y = (int)tempy;

	// 求出提示线右边起始点
	tempx = (b1 - b2) / (k2 - k1);
	tempy = k1 * tempx + b1;

	noticeEnd.x = (int)tempx;
	noticeEnd.y = (int)tempy;

	// 求出安全线左边起始点
	tempx = (b - b3) / (k3 - k);
	tempy = k * tempx + b;

	safeBegin.x = (int)tempx;
	safeBegin.y = (int)tempy;

	// 求出安全线右边起始点
	tempx = (b1 - b3) / (k3 - k1);
	tempy = k1 * tempx + b1;

	safeEnd.x = (int)tempx;
	safeEnd.y = (int)tempy;


	printf("notice (%d, %d), (%d, %d)\n", noticeBegin.x, noticeBegin.y, noticeEnd.x, noticeEnd.y);
	printf("safe (%d, %d), (%d, %d)\n", safeBegin.x, safeBegin.y, safeEnd.x, safeEnd.y);
	cv::line(spread_img, leftLineBegin, noticeBegin, cv::Scalar(0, 255, 255), 1, 8 );	
	cv::line(spread_img, rightLineBegin, noticeEnd, cv::Scalar(0, 255, 255), 1, 8 );	
	cv::line(spread_img, noticeBegin, noticeEnd, cv::Scalar(0, 255, 255), 1, 8 );	
	cv::line(spread_img, noticeBegin, safeBegin, cv::Scalar( 0, 255, 0 ), 1, 8 );	
	cv::line(spread_img, noticeEnd, safeEnd, cv::Scalar( 0, 255, 0 ), 1, 8 );	
	cv::line(spread_img, safeBegin, safeEnd, cv::Scalar( 0, 255, 0 ), 1, 8 );
	

	imwrite( "tempimg//back_spread.jpg" , spread_img );
	
}

// src为输入的鱼眼图原图像，是个指针，有四幅图像，0代表上，1代表后，2代表左，3代表右
// pTable是输入内存查找表指针
// dst为输出的2D俯视图
void C360LookAroundMontageAlg::OnGet2DImg(cv::Mat*src, stu2dTable*pTable, cv::Mat&dst)
{
	int indexx = 0;
	int x1;
	int x2;
	int y1;
	int y2;
	unsigned char cTemp;
	double minus_x;            // x插值
	double minus_y;            // y插值
	double dTempx1;
	double dTempx2;
	double dTempx3;
	double dTempx4;;
	int    iImgWide = m_iSourceWide;
	int    iImgHeight = m_iSourceHeight;

	int xx1;
	int xx2;
	int yy1;
	int yy2;
	double minus_x1;            // x插值
	double minus_y1;            // y插值
	double dTempx11;
	double dTempx22;
	double dTempx33;
	double dTempx44;

	for ( int i = 0 ; i<dst.rows ; i++  )
	{
		for (int j = 0 ; j<dst.cols ; j++ )
		{
			if ( pTable[indexx].img1Idx != -1 )
			{
				// 双线性插值
				// 找到它的四个点

				//////////////////////////////////////////////////////////////////////////
				// 第一幅图像
				x1 = (int)pTable[indexx].img1Location.x;
				y1 = (int)pTable[indexx].img1Location.y;
				minus_x = pTable[indexx].img1Location.x - x1;
				minus_y = pTable[indexx].img1Location.y - y1;

				x2 = x1 + 1;        // 列
				y2 = y1 + 1;        // 行

				if ( x2>= iImgWide )
				{
					x2 = iImgWide - 1;
				}
				if ( y2 >= iImgHeight)
				{
					y2 = iImgHeight - 1;
				}
				//////////////////////////////////////////////////////////////////////////
				// 看看第二幅图像是否存在
				if ( pTable[indexx].img2Idx != -1 )
				{
					xx1 = (int)pTable[indexx].img2Location.x;
					yy1 = (int)pTable[indexx].img2Location.y;
					minus_x1 = pTable[indexx].img2Location.x - xx1;
					minus_y1 = pTable[indexx].img2Location.y - yy1;

					xx2 = xx1 + 1;        // 列
					yy2 = yy1 + 1;        // 行

					if ( xx2>= iImgWide )
					{
						xx2 = iImgWide - 1;
					}
					if ( yy2 >= iImgHeight)
					{
						yy2 = iImgHeight - 1;
					}
				}


				for ( int z = 0 ; z<3 ; z++ )
				{
					// 鱼眼图中对应的图
					//////////////////////////////////////////////////////////////////////////
					dTempx1 = src[pTable[indexx].img1Idx].at<cv::Vec3b>(y1, x1)[z];
					dTempx1 = dTempx1*(1-minus_x);
					dTempx1 = dTempx1*(1-minus_y);

					dTempx2 = src[pTable[indexx].img1Idx].at<cv::Vec3b>(y1, x2)[z];
					dTempx2 = dTempx2*(1-minus_x);
					dTempx2 = dTempx2*minus_y;

					dTempx3 = src[pTable[indexx].img1Idx].at<cv::Vec3b>(y2, x1)[z];
					dTempx3 = dTempx3*minus_x;
					dTempx3 = dTempx3*(1-minus_y);

					dTempx4 = src[pTable[indexx].img1Idx].at<cv::Vec3b>(y2, x2)[z];
					dTempx4 = dTempx4*minus_x;
					dTempx4 = dTempx4*minus_y;

					dTempx1 = dTempx1 + dTempx2;
					dTempx1 = dTempx1 + dTempx3;
					dTempx1 = dTempx1 + dTempx4;
					dTempx1 = dTempx1*pTable[indexx].img1Weight;

					dTempx11 = 0;

					// 如果第二幅图像存在
					if ( pTable[indexx].img2Idx != -1 )
					{
						dTempx11 = src[pTable[indexx].img2Idx].at<cv::Vec3b>(yy1, xx1)[z];
						dTempx11 = dTempx11*(1-minus_x1);
						dTempx11 = dTempx11*(1-minus_y1);

						dTempx22 = src[pTable[indexx].img2Idx].at<cv::Vec3b>(yy1, xx2)[z];
						dTempx22 = dTempx22*(1-minus_x1);
						dTempx22 = dTempx22*minus_y1;

						dTempx33 = src[pTable[indexx].img2Idx].at<cv::Vec3b>(yy2, xx1)[z];
						dTempx33 = dTempx33*minus_x1;
						dTempx33 = dTempx33*(1-minus_y1);

						dTempx44 = src[pTable[indexx].img2Idx].at<cv::Vec3b>(yy2, xx2)[z];
						dTempx44 = dTempx44*minus_x1;
						dTempx44 = dTempx44*minus_y1;

						dTempx11 = dTempx11 + dTempx22;
						dTempx11 = dTempx11 + dTempx33;
						dTempx11 = dTempx11 + dTempx44;

						dTempx11 = dTempx11*pTable[indexx].img2Weight;
					}

					cTemp = (unsigned char)(dTempx1+dTempx11);

					dst.at<cv::Vec3b>(i, j)[z] = cTemp;
				}
			}
			indexx = indexx + 1;
		}

		//if ( i == 100 )
		//{
		//	imwrite( "dstIma.bmp" , dst );
		//	int kkk;
		//	kkk = 300;
		//}
	}

}

// src为输入的鱼眼图原图像，是个指针，有四幅图像，0代表上，1代表后，2代表左，3代表右
// pTable是输入内存查找表指针
// dst为输出的2D俯视图
void C360LookAroundMontageAlg::OnGetJointImg(cv::Mat*src, stu2dTable*pTable, cv::Mat&dst)
{
	int indexx = 0;
	int x1;
	int x2;
	int y1;
	int y2;
	unsigned char cTemp;
	double minus_x;            // x插值
	double minus_y;            // y插值
	double dTempx1;
	double dTempx2;
	double dTempx3;
	double dTempx4;
	int    iImgWide = m_iSourceWide;
	int    iImgHeight = m_iSourceHeight;


	for (int i = 0; i<dst.rows; i++)
	{
		for (int j = 0; j<dst.cols; j++)
		{
			if (pTable[indexx].img1Idx != -1)
			{
				// 双线性插值
				// 找到它的四个点

				//////////////////////////////////////////////////////////////////////////
				// 第一幅图像
				x1 = (int)pTable[indexx].img1Location.x;
				y1 = (int)pTable[indexx].img1Location.y;
				minus_x = pTable[indexx].img1Location.x - x1;
				minus_y = pTable[indexx].img1Location.y - y1;

				x2 = x1 + 1;        // 列
				y2 = y1 + 1;        // 行

				if (x2 >= iImgWide)
				{
					x2 = iImgWide - 1;
				}
				if (y2 >= iImgHeight)
				{
					y2 = iImgHeight - 1;
				}
				//////////////////////////////////////////////////////////////////////////

				for (int z = 0; z<3; z++)
				{
					// 鱼眼图中对应的图
					//////////////////////////////////////////////////////////////////////////
					dTempx1 = src[pTable[indexx].img1Idx].at<cv::Vec3b>(y1, x1)[z];
					dTempx1 = dTempx1*(1 - minus_x);
					dTempx1 = dTempx1*(1 - minus_y);

					dTempx2 = src[pTable[indexx].img1Idx].at<cv::Vec3b>(y1, x2)[z];
					dTempx2 = dTempx2*(1 - minus_x);
					dTempx2 = dTempx2*minus_y;

					dTempx3 = src[pTable[indexx].img1Idx].at<cv::Vec3b>(y2, x1)[z];
					dTempx3 = dTempx3*minus_x;
					dTempx3 = dTempx3*(1 - minus_y);

					dTempx4 = src[pTable[indexx].img1Idx].at<cv::Vec3b>(y2, x2)[z];
					dTempx4 = dTempx4*minus_x;
					dTempx4 = dTempx4*minus_y;

					dTempx1 = dTempx1 + dTempx2;
					dTempx1 = dTempx1 + dTempx3;
					dTempx1 = dTempx1 + dTempx4;			

					cTemp = (unsigned char)dTempx1;

					dst.at<cv::Vec3b>(i, j)[z] = cTemp;
				}
			}
			indexx = indexx + 1;
		}

	}

}

// src为输入的鱼眼图原图像，是个指针，有四幅图像，0代表上，1代表后，2代表左，3代表右
// pTable是输入内存查找表指针
// dst为输出的2D俯视图
void C360LookAroundMontageAlg::OnGet3DJointImg(cv::Mat *src, stu3dTable *pTable, cv::Mat &dst)
{
	int indexx = 0;
	int x1;
	int x2;
	int y1;
	int y2;
	unsigned char cTemp;
	double minus_x;            // x插值
	double minus_y;            // y插值
	double dTempx1;
	double dTempx2;
	double dTempx3;
	double dTempx4;
	int    iImgWide = m_iSourceWide;
	int    iImgHeight = m_iSourceHeight;


	for (int i = 0; i < dst.rows / 2; i++)
	{
		for (int j = 0; j<dst.cols; j++)
		{
			if (pTable[indexx].img1Idx != -1)
			{
				// 双线性插值
				// 找到它的四个点

				//////////////////////////////////////////////////////////////////////////
				// 第一幅图像
				x1 = (int)pTable[indexx].img1Location.x;
				y1 = (int)pTable[indexx].img1Location.y;
				minus_x = pTable[indexx].img1Location.x - x1;
				minus_y = pTable[indexx].img1Location.y - y1;

				x2 = x1 + 1;        // 列
				y2 = y1 + 1;        // 行

				if (x2 >= iImgWide)
				{
					x2 = iImgWide - 1;
				}
				if (y2 >= iImgHeight)
				{
					y2 = iImgHeight - 1;
				}
				//////////////////////////////////////////////////////////////////////////

				for (int z = 0; z<3; z++)
				{
					// 鱼眼图中对应的图
					//////////////////////////////////////////////////////////////////////////
					dTempx1 = src[pTable[indexx].img1Idx].at<cv::Vec3b>(y1, x1)[z];
					dTempx1 = dTempx1*(1 - minus_x);
					dTempx1 = dTempx1*(1 - minus_y);

					dTempx2 = src[pTable[indexx].img1Idx].at<cv::Vec3b>(y1, x2)[z];
					dTempx2 = dTempx2*(1 - minus_x);
					dTempx2 = dTempx2*minus_y;

					dTempx3 = src[pTable[indexx].img1Idx].at<cv::Vec3b>(y2, x1)[z];
					dTempx3 = dTempx3*minus_x;
					dTempx3 = dTempx3*(1 - minus_y);

					dTempx4 = src[pTable[indexx].img1Idx].at<cv::Vec3b>(y2, x2)[z];
					dTempx4 = dTempx4*minus_x;
					dTempx4 = dTempx4*minus_y;

					dTempx1 = dTempx1 + dTempx2;
					dTempx1 = dTempx1 + dTempx3;
					dTempx1 = dTempx1 + dTempx4;

					cTemp = (unsigned char)dTempx1;

					dst.at<cv::Vec3b>(i, j)[z] = cTemp;
				}
			}
			indexx = indexx + 1;
		}

	}

}

// src为输入的鱼眼图原图像，是个指针，有四幅图像，0代表上，1代表后，2代表左，3代表右
// pTable是输入内存查找表指针
// dst为输出的2D俯视图
void C360LookAroundMontageAlg::OnGet3DImg(cv::Mat *src, stu3dTable *pTable, cv::Mat &dst)
{
	int indexx = 0;
	int x1;
	int x2;
	int y1;
	int y2;
	unsigned char cTemp;
	double minus_x;            // x插值
	double minus_y;            // y插值
	double dTempx1;
	double dTempx2;
	double dTempx3;
	double dTempx4;;
	int    iImgWide = m_iSourceWide;
	int    iImgHeight = m_iSourceHeight;

	int xx1;
	int xx2;
	int yy1;
	int yy2;
	double minus_x1;            // x插值
	double minus_y1;            // y插值
	double dTempx11;
	double dTempx22;
	double dTempx33;
	double dTempx44;

	for (int i = 0; i<dst.rows / 2; i++)
	{
		for (int j = 0; j<dst.cols; j++)
		{
			if (pTable[indexx].img1Idx != -1)
			{
				// 双线性插值
				// 找到它的四个点

				//////////////////////////////////////////////////////////////////////////
				// 第一幅图像
				x1 = (int)pTable[indexx].img1Location.x;
				y1 = (int)pTable[indexx].img1Location.y;
				minus_x = pTable[indexx].img1Location.x - x1;
				minus_y = pTable[indexx].img1Location.y - y1;

				x2 = x1 + 1;        // 列
				y2 = y1 + 1;        // 行

				if (x2 >= iImgWide)
				{
					x2 = iImgWide - 1;
				}
				if (y2 >= iImgHeight)
				{
					y2 = iImgHeight - 1;
				}
				//////////////////////////////////////////////////////////////////////////
				// 看看第二幅图像是否存在
				if (pTable[indexx].img2Idx != -1)
				{
					xx1 = (int)pTable[indexx].img2Location.x;
					yy1 = (int)pTable[indexx].img2Location.y;
					minus_x1 = pTable[indexx].img2Location.x - xx1;
					minus_y1 = pTable[indexx].img2Location.y - yy1;

					xx2 = xx1 + 1;        // 列
					yy2 = yy1 + 1;        // 行

					if (xx2 >= iImgWide)
					{
						xx2 = iImgWide - 1;
					}
					if (yy2 >= iImgHeight)
					{
						yy2 = iImgHeight - 1;
					}
				}


				for (int z = 0; z<3; z++)
				{
					// 鱼眼图中对应的图
					//////////////////////////////////////////////////////////////////////////
					dTempx1 = src[pTable[indexx].img1Idx].at<cv::Vec3b>(y1, x1)[z];
					dTempx1 = dTempx1*(1 - minus_x);
					dTempx1 = dTempx1*(1 - minus_y);

					dTempx2 = src[pTable[indexx].img1Idx].at<cv::Vec3b>(y1, x2)[z];
					dTempx2 = dTempx2*(1 - minus_x);
					dTempx2 = dTempx2*minus_y;

					dTempx3 = src[pTable[indexx].img1Idx].at<cv::Vec3b>(y2, x1)[z];
					dTempx3 = dTempx3*minus_x;
					dTempx3 = dTempx3*(1 - minus_y);

					dTempx4 = src[pTable[indexx].img1Idx].at<cv::Vec3b>(y2, x2)[z];
					dTempx4 = dTempx4*minus_x;
					dTempx4 = dTempx4*minus_y;

					dTempx1 = dTempx1 + dTempx2;
					dTempx1 = dTempx1 + dTempx3;
					dTempx1 = dTempx1 + dTempx4;
					dTempx1 = dTempx1*pTable[indexx].img1Weight;

					dTempx11 = 0;

					// 如果第二幅图像存在
					if (pTable[indexx].img2Idx != -1)
					{
						dTempx11 = src[pTable[indexx].img2Idx].at<cv::Vec3b>(yy1, xx1)[z];
						dTempx11 = dTempx11*(1 - minus_x1);
						dTempx11 = dTempx11*(1 - minus_y1);

						dTempx22 = src[pTable[indexx].img2Idx].at<cv::Vec3b>(yy1, xx2)[z];
						dTempx22 = dTempx22*(1 - minus_x1);
						dTempx22 = dTempx22*minus_y1;

						dTempx33 = src[pTable[indexx].img2Idx].at<cv::Vec3b>(yy2, xx1)[z];
						dTempx33 = dTempx33*minus_x1;
						dTempx33 = dTempx33*(1 - minus_y1);

						dTempx44 = src[pTable[indexx].img2Idx].at<cv::Vec3b>(yy2, xx2)[z];
						dTempx44 = dTempx44*minus_x1;
						dTempx44 = dTempx44*minus_y1;

						dTempx11 = dTempx11 + dTempx22;
						dTempx11 = dTempx11 + dTempx33;
						dTempx11 = dTempx11 + dTempx44;

						dTempx11 = dTempx11*pTable[indexx].img2Weight;
					}

					cTemp = (unsigned char)(dTempx1 + dTempx11);

					dst.at<cv::Vec3b>(i, j)[z] = cTemp;
				}
			}
			indexx = indexx + 1;
		}

		//if ( i == 100 )
		//{
		//	imwrite( "dstIma.bmp" , dst );
		//	int kkk;
		//	kkk = 300;
		//}
	}

}

// src为输入的鱼眼图原图像，是个指针，有四幅图像，0代表上，1代表后，2代表左，3代表右
// pTable是输入内存查找表指针
// dst为输出的2D俯视图
void C360LookAroundMontageAlg::OnGetEachJointImg(cv::Mat *src, stu2dTable*pTable, cv::Mat *dst)
{
	int indexx = 0;
	int x1;
	int x2;
	int y1;
	int y2;
	unsigned char cTemp;
	double minus_x;            // x插值
	double minus_y;            // y插值
	double dTempx1;
	double dTempx2;
	double dTempx3;
	double dTempx4;
	int    iImgWide = m_iSourceWide;
	int    iImgHeight = m_iSourceHeight;


	for (int i = 0; i<dst[0].rows; i++)
	{
		for (int j = 0; j<dst[0].cols; j++)
		{
			if (pTable[indexx].img1Idx != -1)
			{
				// 双线性插值
				// 找到它的四个点

				//////////////////////////////////////////////////////////////////////////
				// 第一幅图像
				x1 = (int)pTable[indexx].img1Location.x;
				y1 = (int)pTable[indexx].img1Location.y;
				minus_x = pTable[indexx].img1Location.x - x1;
				minus_y = pTable[indexx].img1Location.y - y1;

				x2 = x1 + 1;        // 列
				y2 = y1 + 1;        // 行

				if (x2 >= iImgWide)
				{
					x2 = iImgWide - 1;
				}
				if (y2 >= iImgHeight)
				{
					y2 = iImgHeight - 1;
				}
				//////////////////////////////////////////////////////////////////////////

				for (int z = 0; z<3; z++)
				{
					// 鱼眼图中对应的图
					//////////////////////////////////////////////////////////////////////////
					dTempx1 = src[pTable[indexx].img1Idx].at<cv::Vec3b>(y1, x1)[z];
					dTempx1 = dTempx1*(1 - minus_x);
					dTempx1 = dTempx1*(1 - minus_y);

					dTempx2 = src[pTable[indexx].img1Idx].at<cv::Vec3b>(y1, x2)[z];
					dTempx2 = dTempx2*(1 - minus_x);
					dTempx2 = dTempx2*minus_y;

					dTempx3 = src[pTable[indexx].img1Idx].at<cv::Vec3b>(y2, x1)[z];
					dTempx3 = dTempx3*minus_x;
					dTempx3 = dTempx3*(1 - minus_y);

					dTempx4 = src[pTable[indexx].img1Idx].at<cv::Vec3b>(y2, x2)[z];
					dTempx4 = dTempx4*minus_x;
					dTempx4 = dTempx4*minus_y;

					dTempx1 = dTempx1 + dTempx2;
					dTempx1 = dTempx1 + dTempx3;
					dTempx1 = dTempx1 + dTempx4;

					cTemp = (unsigned char)dTempx1;

					dst[pTable[indexx].img1Idx].at<cv::Vec3b>(i, j)[z] = cTemp;
				}
			}
			indexx = indexx + 1;
		}

	}

}


C360LookAroundMontageAlg::~C360LookAroundMontageAlg()
{
	int k;
	k = 30;
}
