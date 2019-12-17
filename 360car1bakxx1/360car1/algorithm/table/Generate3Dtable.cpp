#include "Generate3Dtable.h"

CGenerate3Dtable::CGenerate3Dtable()
{
	// 通过调整摄像机的高度和视场角，调整怎样映射到一个圆上；
	// 摄像机的高度影响球的弧度。视场角影响摄像机能看到范围。

	// 设光轴垂直朝下，夹角与z轴负方向，范围为[-theta , theta]，在此默认中，theta=60度
	//m_vCameraParam.cameraMaxFieldView = (double)(180.0f/180.0f)*PI;                     // 摄像机视场角设置为120度

	// 手动设置圆半径，如果半径设置得比较小的话，那么就手动修改半径大小
	//m_vCameraParam.circleRadius       = 8000;                    // 默认为4000毫米

	// 地面离圆心的高度
	m_CircleCenter = 0;
}



bool CGenerate3Dtable::OnGetCheckBoardSize(struct virtualCameraParam &vCameraParam, int&wide, int&height)
{
	int bRetrun = 0;
	const char*file = "config.ini";          // 配置文件名和路径

	//const char* sect;
	//const char* key;
	bRetrun = iniFileLoad( file );

	if ( !bRetrun )
	{
		cout<<"config.ini load error!"<<endl;
		return false;
	}

	char* sect;
	char* key;

	if (TOURING_CAR == m_CalibrateScheme)          // 房车
	{
		sect = "commonStyle";
		key = "checkBoardWideX";
		wide = iniGetInt(sect, key, 20);

		key = "checkBoardHeight";
		height = iniGetInt(sect, key, 20);
	}
	else			// 宝沃
	{
		sect = "checkboardINF";
		key = "checkBoardWideX";
		wide = iniGetInt(sect, key, 20);

		key = "checkBoardHeight";
		height = iniGetInt(sect, key, 20);
	}

	sect = "virtualCameraParam";
	key = "cameraMaxFieldView";
	vCameraParam.cameraMaxFieldView = iniGetDouble(sect, key, 20);

	key = "circleRadius";
	vCameraParam.circleRadius = iniGetDouble(sect, key, 20);

	key = "longRes";
	vCameraParam.longRes = iniGetInt(sect, key, 20);

	key = "latRes";
	vCameraParam.latRes = iniGetInt(sect, key, 20);

	cout << "cameraMaxFieldView: " << vCameraParam.cameraMaxFieldView << endl;
	cout << "circleRadius: " << vCameraParam.circleRadius << endl;
	cout << "longRes: " << vCameraParam.longRes << endl;
	cout << "latRes: " << vCameraParam.latRes << endl;

	sect = "fuse";
	key = "Fusion_Area_Size";
	m_doubleAreaSize = iniGetInt(sect, key, 20);

	iniFileFree();

	return true;
}

// 获取点在小车的位置
int CGenerate3Dtable::OnGetPointLocation(double xLocation , double yLocation ,  G3DtableParam* p3DtableParam )
{
	int Loca = 0;                  // 初使化表示啥都还没有
//	Loca = P_RIGHT;

#if 0
	Loca = Loca|P_CAR;
	Loca = Loca|P_RIGHT;
	Loca = Loca|P_TOP;


	int t;
	t = Loca&P_TOP;
	int rrr;
	rrr = Loca&P_CAR;
	int ttt;
	ttt = Loca&P_RIGHT;
	int tr;
	tr = Loca&P_LEFT;
#endif


	// 检查是否是在车辆区域
	Loca |= OnCheckisInCarRange( xLocation , yLocation , p3DtableParam->worldcarSize , p3DtableParam->fWorldCarBeginX , p3DtableParam->fWorldCarBeginY );

	// 检查是否是在上区域
	Loca |= OnCheckisInCarTop( xLocation , yLocation , p3DtableParam->worldcarSize , p3DtableParam->fWorldCarBeginX , p3DtableParam->fWorldCarBeginY , p3DtableParam->m_K , p3DtableParam->m_B );

	// 检查是否是在下区域
	Loca |= OnCheckisInCarBottom( xLocation , yLocation , p3DtableParam->worldcarSize , p3DtableParam->fWorldCarBeginX, p3DtableParam->fWorldCarBeginY , p3DtableParam->m_K , p3DtableParam->m_B );

	// 检查是否是在左区域
	Loca |= OnCheckisInCarLeft( xLocation , yLocation , p3DtableParam->worldcarSize , p3DtableParam->fWorldCarBeginX, p3DtableParam->fWorldCarBeginY , p3DtableParam->m_K , p3DtableParam->m_B );

	// 检查是否是在右区域
	Loca |= OnCheckisInCarRight( xLocation , yLocation , p3DtableParam->worldcarSize , p3DtableParam->fWorldCarBeginX, p3DtableParam->fWorldCarBeginY , p3DtableParam->m_K , p3DtableParam->m_B );
	//if ( Loca  P_CAR )
	//{
	//	Loca = Loca|P_CAR;
	//}

	return Loca;
}

// 车区域
PointLocation CGenerate3Dtable::OnCheckisInCarRange( double xLocation , double yLocation , CvSize2D32f worldCarSize , float fWorldCarBeginX , float fWorldCarBeginY )
{
	PointLocation PReturn = P_NO;

	if ( (xLocation >= fWorldCarBeginX)&&(xLocation < (fWorldCarBeginX + worldCarSize.width) )&&(yLocation >= fWorldCarBeginY )&&(yLocation < (fWorldCarBeginY + worldCarSize.height)) )
	{
		PReturn = P_CAR;
	}

	//if ( (xLocation >= 10000)&&(xLocation < (10000 + 10000) )&&(yLocation >= 20000 )&&(yLocation < (20000 + 20000)) )
	//{
	//	PReturn = P_CAR;
	//}

	return PReturn;
}

// 上区域
// pK和pB分别是0、1、2、3，代表左上拼接缝，右上拼接缝，左下拼接缝和右下拼接缝
PointLocation CGenerate3Dtable::OnCheckisInCarTop( double xLocation , double yLocation , CvSize2D32f worldCarSize , float fWorldCarBeginX , float fWorldCarBeginY , float* pK , float* pB )
{
	PointLocation PReturn = P_NO;
	float countY1 = 0.0;
	float countY2 = 0.0;

	countY1 = m_K[0] * xLocation + m_B[0];
	countY2 = m_K[1] * xLocation + m_B[1];
	if ( (yLocation < countY1) && (yLocation < countY2) && (yLocation < fWorldCarBeginY) )
	{
		PReturn = P_TOP;
	}

	return PReturn;
}

// 下区域
PointLocation CGenerate3Dtable::OnCheckisInCarBottom( double xLocation , double yLocation , CvSize2D32f worldCarSize , float fWorldCarBeginX , float fWorldCarBeginY , float* pK , float* pB )
{
	PointLocation PReturn = P_NO;
	float countY1 = 0.0;
	float countY2 = 0.0;

	countY1 = m_K[2] * xLocation + m_B[2];
	countY2 = m_K[3] * xLocation + m_B[3];
	if ( (yLocation > countY1) && (yLocation > countY2) && (yLocation > worldCarSize.height + fWorldCarBeginY) )
	{
		PReturn = P_BOTTOM;
	}

	return PReturn;
}

// 左区域
PointLocation CGenerate3Dtable::OnCheckisInCarLeft( double xLocation , double yLocation , CvSize2D32f worldCarSize , float fWorldCarBeginX , float fWorldCarBeginY , float* pK , float* pB )
{
	PointLocation PReturn = P_NO;
	float countY1 = 0.0;
	float countY2 = 0.0;

	countY1 = m_K[0] * xLocation + m_B[0];
	countY2 = m_K[2] * xLocation + m_B[2];
	if ( (yLocation > countY1) && (yLocation < countY2) && (xLocation < fWorldCarBeginX) )
	{
		PReturn = P_LEFT;
	}

	return PReturn;
}

// 右区域
PointLocation CGenerate3Dtable::OnCheckisInCarRight( double xLocation , double yLocation , CvSize2D32f worldCarSize , float fWorldCarBeginX , float fWorldCarBeginY , float* pK , float* pB )
{
	PointLocation PReturn = P_NO;
	float countY1 = 0.0;
	float countY2 = 0.0;

	countY1 = m_K[1] * xLocation + m_B[1];
	countY2 = m_K[3] * xLocation + m_B[3];
	if ( (yLocation > countY1) && (yLocation < countY2) && (xLocation > fWorldCarBeginX + worldCarSize.width) )
	{
		PReturn = P_RIGHT;
	}

	return PReturn;
}

void CGenerate3Dtable::OnGenerateRTAverage( G3DtableParam* p3DtableParam )
{
	int i = 0;
	cv::Matx44d p1;
	cv::Matx44d p2;
	int iTemp2 = 0;

	for ( i = 0 ; i<FISHEYE_COUNT ; i++ )
	{	
		p1 = p3DtableParam->CalibrateParam[i].OnGetRTParam(0);//.OnGetRTParam( 0 );
		p2 = p3DtableParam->CalibrateParam[i].OnGetRTParam(1);

		if ( i<2 )
		{
			iTemp2 = p3DtableParam->worldcarSize.width;
		}
		else
		{
			iTemp2 = p3DtableParam->worldcarSize.height;
		}
		

		for ( int iRow = 0 ; iRow<m_RT_3DvalueAverage[i].rows ; iRow++ )
		{		
			for ( int jCol = 0 ; jCol<m_RT_3DvalueAverage[i].cols ; jCol++ )
			{

				m_RT_3DvalueAverage[i](iRow,jCol) = p2(iRow,jCol) - p1(iRow,jCol);
				m_RT_3DvalueAverage[i](iRow,jCol) = m_RT_3DvalueAverage[i](iRow,jCol)/(double)iTemp2;
			}
		}
	}

}

void CGenerate3Dtable::OnGet3DRTvalue(int x, int y, PointLocation pLocation, cv::Matx44d &RT, G3DtableParam *p3DtableParam)
{
	if ( pLocation == P_TOP )        // 前摄像头
	{
		// 左
		if ( x < p3DtableParam->fWorldCarBeginX )
		{
			RT = p3DtableParam->CalibrateParam[TOP].OnGetRTParam( 0 );
		}
		// 右
		if( x > (p3DtableParam->fWorldCarBeginX + p3DtableParam->worldcarSize.width) )
		{
			RT = p3DtableParam->CalibrateParam[TOP].OnGetRTParam( 1 );
		}
		// 中
		if ( (x>= p3DtableParam->fWorldCarBeginX)&&(x<=p3DtableParam->fWorldCarBeginX + p3DtableParam->worldcarSize.width) )
		{
			for ( int i = 0 ; i<RT.rows ; i++ )
			{
				for ( int j = 0 ; j<RT.cols ; j++ )
				{
				//	iTemp = NowIndex - pIArrea[0];
				//	ReturnRT(i,j) = iTemp*DValueRT(i,j);
				//	ReturnRT(i,j) = ReturnRT(i,j) + fishEye.RT[0](i,j);

					RT(i,j) = (x - p3DtableParam->fWorldCarBeginX)*m_RT_3DvalueAverage[TOP](i , j) + p3DtableParam->CalibrateParam[TOP].OnGetRTParam( 0 )(i,j);
				}
			}
		}
		
	}

	if ( pLocation == P_BOTTOM )    // 后摄像头
	{
		if ( x < p3DtableParam->fWorldCarBeginX )
		{
			RT = p3DtableParam->CalibrateParam[BOTTOM].OnGetRTParam( 0 );
		}
		if( x > (p3DtableParam->fWorldCarBeginX + p3DtableParam->worldcarSize.width) )
		{
			RT = p3DtableParam->CalibrateParam[BOTTOM].OnGetRTParam( 1 );
		}
		if ( (x>= p3DtableParam->fWorldCarBeginX)&&(x<=p3DtableParam->fWorldCarBeginX + p3DtableParam->worldcarSize.width) )
		{
			for ( int i = 0 ; i<RT.rows ; i++ )
			{
				for ( int j = 0 ; j<RT.cols ; j++ )
				{
					//	iTemp = NowIndex - pIArrea[0];
					//	ReturnRT(i,j) = iTemp*DValueRT(i,j);
					//	ReturnRT(i,j) = ReturnRT(i,j) + fishEye.RT[0](i,j);

					RT(i,j) = (x - p3DtableParam->fWorldCarBeginX)*m_RT_3DvalueAverage[BOTTOM](i , j) + p3DtableParam->CalibrateParam[BOTTOM].OnGetRTParam( 0 )(i,j);
				}
			}
		}
	}

	if ( pLocation == P_LEFT )      // 左摄像头
	{
		if ( y < p3DtableParam->fWorldCarBeginY )
		{
			RT = p3DtableParam->CalibrateParam[LEFT].OnGetRTParam( 0 );
		}
		if( y > (p3DtableParam->fWorldCarBeginY + p3DtableParam->worldcarSize.height) )
		{
			RT = p3DtableParam->CalibrateParam[LEFT].OnGetRTParam( 1 );
		}
		if ( (y>= p3DtableParam->fWorldCarBeginY)&&(y<=p3DtableParam->fWorldCarBeginY + p3DtableParam->worldcarSize.height) )
		{
			for ( int i = 0 ; i<RT.rows ; i++ )
			{
				for ( int j = 0 ; j<RT.cols ; j++ )
				{
					//	iTemp = NowIndex - pIArrea[0];
					//	ReturnRT(i,j) = iTemp*DValueRT(i,j);
					//	ReturnRT(i,j) = ReturnRT(i,j) + fishEye.RT[0](i,j);

					RT(i,j) = (y - p3DtableParam->fWorldCarBeginY)*m_RT_3DvalueAverage[LEFT](i , j) + p3DtableParam->CalibrateParam[LEFT].OnGetRTParam( 0 )(i,j);
				}
			}
		}
	}

	if ( pLocation == P_RIGHT )    // 右摄像头
	{
		if ( y < p3DtableParam->fWorldCarBeginY )
		{
			RT = p3DtableParam->CalibrateParam[RIGHT].OnGetRTParam( 0 );
		}
		if( y > (p3DtableParam->fWorldCarBeginY + p3DtableParam->worldcarSize.height) )
		{
			RT = p3DtableParam->CalibrateParam[RIGHT].OnGetRTParam( 1 );
		}
		if ( (y>= p3DtableParam->fWorldCarBeginY)&&(y<=p3DtableParam->fWorldCarBeginY + p3DtableParam->worldcarSize.height) )
		{
			for ( int i = 0 ; i<RT.rows ; i++ )
			{
				for ( int j = 0 ; j<RT.cols ; j++ )
				{
					//	iTemp = NowIndex - pIArrea[0];
					//	ReturnRT(i,j) = iTemp*DValueRT(i,j);
					//	ReturnRT(i,j) = ReturnRT(i,j) + fishEye.RT[0](i,j);

					RT(i,j) = (y - p3DtableParam->fWorldCarBeginY)*m_RT_3DvalueAverage[RIGHT](i , j) + p3DtableParam->CalibrateParam[RIGHT].OnGetRTParam( 0 )(i,j);
				}
			}
		}
	}
}


void CGenerate3Dtable::OnSetSpliceJoint(CvSize2D32f worldShowSize, float fBeginOriginX, float fBeginOriginY, CvSize2D32f worldCarSize, float fWorldCarBeginX, float fWorldCarBeginY, CCalibrate *pCameraParam)
{
	//////////////////////////////////////////////////////////////////////////
	// 设计拼接缝
	// 左上角拼接缝的三条拼接线在世界坐标上的值
	float fTemp1;
	float fTemp2;

	//int iTempxx = -2000;
	int iTempxx = 0;
	//int iTempxx1 = 0;

	int bRetrun = 0;

	// 左上角
	CvSize fBeginPoint_topLeft;
	fBeginPoint_topLeft.width = fWorldCarBeginX;          // x
	fBeginPoint_topLeft.height = fWorldCarBeginY;        // y

	CvSize fEndPoint_topLeft;
	fEndPoint_topLeft.width = fBeginOriginX + iTempxx;
	fEndPoint_topLeft.height = fBeginOriginY;

	fTemp1 = (float)(fEndPoint_topLeft.height - fBeginPoint_topLeft.height);
	fTemp2 = (float)(fEndPoint_topLeft.width - fBeginPoint_topLeft.width);
	m_K[0] = fTemp1 / fTemp2;
	m_B[0] = fEndPoint_topLeft.height - m_K[0] * fEndPoint_topLeft.width;

#if 1
	// 左上角融合区域
	// 融合区域融合线计算
	CvSize fPointBottom_topLeft;            // 在拼接线的下方
	fPointBottom_topLeft.width = fEndPoint_topLeft.width - m_doubleAreaSize;//(m_isquareToGap-1)*CHECK_BOARD_PATTERN_LEN + m_ixOffset;
	fPointBottom_topLeft.height = fEndPoint_topLeft.height;
	CvSize fPointTop_topLeft;               // 在拼接线的上方
	fPointTop_topLeft.width = fEndPoint_topLeft.width + m_doubleAreaSize;
	fPointTop_topLeft.height = fEndPoint_topLeft.height;

	m_BMix[0] = fPointTop_topLeft.height - m_K[0] * fPointTop_topLeft.width;
	m_BMix[1] = fPointBottom_topLeft.height - m_K[0] * fPointBottom_topLeft.width;
#endif

	//////////////////////////////////////////////////////////////////////////
	// 右上角拼接缝在世界坐标系上值
	CvSize fBeginPoint_topRight;
	fBeginPoint_topRight.width = fWorldCarBeginX + worldCarSize.width;         // x
	fBeginPoint_topRight.height = fWorldCarBeginY;                             // y
	CvSize fEndPoint_topRight;
	fEndPoint_topRight.width = fBeginOriginX + worldShowSize.width - iTempxx;              // x
	fEndPoint_topRight.height = fBeginOriginY;                                 // y

	fTemp1 = (float)(fEndPoint_topRight.height - fBeginPoint_topRight.height);
	fTemp2 = (float)(fEndPoint_topRight.width - fBeginPoint_topRight.width);
	m_K[1] = fTemp1 / fTemp2;
	m_B[1] = fEndPoint_topRight.height - m_K[1] * fEndPoint_topRight.width;

#if 1
	// 融合区域融合线
	CvSize fPointBottom_topRight;        // 拼接线的下方
	fPointBottom_topRight.width = fBeginOriginX + worldShowSize.width - iTempxx + m_doubleAreaSize;
	fPointBottom_topRight.height = fEndPoint_topRight.height;
	CvSize fPointTop_topRight;           // 拼接线的上方
	fPointTop_topRight.width = fBeginOriginX + worldShowSize.width - iTempxx - m_doubleAreaSize;
	fPointTop_topRight.height = fEndPoint_topRight.height;

	m_BMix[2] = fPointTop_topRight.height - m_K[1] * fPointTop_topRight.width;
	m_BMix[3] = fPointBottom_topRight.height - m_K[1] * fPointBottom_topRight.width;
#endif
	//////////////////////////////////////////////////////////////////////////

	// 左下角拼接缝在世界坐标系上的值
	CvSize fBeginPoint_bottomLeft;
	fBeginPoint_bottomLeft.width = fBeginPoint_topLeft.width;
	fBeginPoint_bottomLeft.height = fWorldCarBeginY + worldCarSize.height;

	CvSize fEndPoint_BottomLeft;
	fEndPoint_BottomLeft.width = fEndPoint_topLeft.width + iTempxx;
	fEndPoint_BottomLeft.height = fBeginOriginY + worldShowSize.height;

	fTemp1 = (float)(fEndPoint_BottomLeft.height - fBeginPoint_bottomLeft.height);
	fTemp2 = (float)(fEndPoint_BottomLeft.width - fBeginPoint_bottomLeft.width);
	m_K[2] = fTemp1 / fTemp2;
	m_B[2] = fEndPoint_BottomLeft.height - m_K[2] * fEndPoint_BottomLeft.width;

#if 1
	// 融合区域拼接线
	CvSize fPointBottom_bottomLeft;           // 拼接线下方
	fPointBottom_bottomLeft.width = fPointTop_topLeft.width + iTempxx;
	fPointBottom_bottomLeft.height = fEndPoint_BottomLeft.height;
	CvSize fPointTop_bottomLeft;              // 拼接线的上方
	fPointTop_bottomLeft.width = fPointBottom_topLeft.width + iTempxx;
	fPointTop_bottomLeft.height = fPointBottom_bottomLeft.height;

	m_BMix[4] = fPointTop_bottomLeft.height - m_K[2] * fPointTop_bottomLeft.width;
	m_BMix[5] = fPointBottom_bottomLeft.height - m_K[2] * fPointBottom_bottomLeft.width;
#endif


	// 右下角拼接缝在世界坐标系上的值
	CvSize fBeginPoint_bottomRight;
	fBeginPoint_bottomRight.width = fBeginPoint_topRight.width;
	fBeginPoint_bottomRight.height = fBeginPoint_bottomLeft.height;

	CvSize fEndPoint_bottomRight;
	fEndPoint_bottomRight.width = fEndPoint_topRight.width - iTempxx;
	fEndPoint_bottomRight.height = fEndPoint_BottomLeft.height;

	fTemp1 = (float)(fEndPoint_bottomRight.height - fBeginPoint_bottomRight.height);
	fTemp2 = (float)(fEndPoint_bottomRight.width - fBeginPoint_bottomRight.width);
	m_K[3] = fTemp1 / fTemp2;
	m_B[3] = fEndPoint_bottomRight.height - m_K[3] * fEndPoint_bottomRight.width;

#if 1
	// 融合区域拼接线
	CvSize fPointBottom_bottomRight;        // 拼接线下方
	fPointBottom_bottomRight.width = fPointTop_topRight.width - iTempxx;
	fPointBottom_bottomRight.height = fEndPoint_bottomRight.height;

	CvSize fPointTop_bottomRight;           // 拼接线上方
	fPointTop_bottomRight.width = fPointBottom_topRight.width - iTempxx;
	fPointTop_bottomRight.height = fEndPoint_bottomRight.height;

	m_BMix[6] = fPointTop_bottomRight.height - m_K[3] * fPointTop_bottomRight.width;
	m_BMix[7] = fPointBottom_bottomRight.height - m_K[3] * fPointBottom_bottomRight.width;
#endif


	//////////////////////////////////////////////////////////////////////////
	// 将图像分区
	//////////////////////////////////////////////////////////////////////////
	// 接下来就是对棋盘进行分区，求中间的渐变区域
	// x方向上分区
	int iTemp1;
	int iTemp2;
	int i;
	int j;
	iTemp1 = fWorldCarBeginX;// - fBeginOriginX;

	m_wide_area[0] = (int)(iTemp1);          // 第一个区域
	//	iTemp1 = worldCarSize.width;
	m_wide_area[1] = (int)(worldCarSize.width);

	m_wide_area[2] = worldShowSize.width - fWorldCarBeginX;// + fBeginOriginX ;
	m_wide_area[2] = (m_wide_area[2] - worldCarSize.width);

	// y方向上分区
	iTemp1 = fWorldCarBeginY; // - fBeginOriginY;
	m_height_area[0] = (int)(iTemp1);

	//iTemp1 = worldCarSize.height ;
	m_height_area[1] = (int)(worldCarSize.height);

	m_height_area[2] = worldShowSize.height - fWorldCarBeginY; // + fBeginOriginY ;
	m_height_area[2] = (m_height_area[2] - worldCarSize.height);
	//	heightLen[2] = heightLen[2] - heightLen[1];

	cv::Matx44d p1;
	cv::Matx44d p2;
	int iCount = 0;


	for (iTemp1 = 0; iTemp1 < FISHEYE_COUNT; iTemp1++)
	{
		if (iTemp1 <= 1)            // 使用wideLen[1]
		{
			iTemp2 = m_wide_area[1];
		}
		else                         // 使用heightLen[1]
		{
			iTemp2 = m_height_area[1];
		}

		for (i = 0; i<m_RT_DvalueAverage[iTemp1].rows; i++)
		{
			for (j = 0; j<m_RT_DvalueAverage[iTemp1].cols; j++)
			{
				p1 = pCameraParam[iTemp1].OnGetRTParam(0);
				p2 = pCameraParam[iTemp1].OnGetRTParam(1);
				m_RT_DvalueAverage[iTemp1](i, j) = p2(i, j) - p1(i, j);
				m_RT_DvalueAverage[iTemp1](i, j) = m_RT_DvalueAverage[iTemp1](i, j) / (double)iTemp2;
				//m_pCalibrate[iTemp1](i,j) = 

				//	Dvalue_average[iTemp1]( i , j ) = m_fishEyeInf[iTemp1].RT[1](i,j) - m_fishEyeInf[iTemp1].RT[0](i,j);
				//	Dvalue_average[iTemp1]( i , j) = Dvalue_average[iTemp1](i,j)/(double)iTemp2;
			}
		}
	}
}

#if NEW3D
void CGenerate3Dtable::OnGenerateNew3Dtable(G3DtableParam*p3DtableParam)
{
	//int cation;
	cv::Matx44d TRT;          // 外参
	vector<vector<cv::Point3f> >*pRectW = p3DtableParam->pRectW;
	vector<vector<cv::Point2f> >*pRectWImg = p3DtableParam->pRectWImg;
	
	//////////////////////////////////////////////////////////////////////////
	// 使用前和右两个镜头的图像来做试验
	// 下标值分别为0和3

	//--前镜头左右两个正方形在世界坐标系中的位置
	vector<cv::Point3f> topWleft = pRectW[0].at(0);
	vector<cv::Point3f> topWright = pRectW[0].at(1);

	//--前镜头左右两个正方形在图像坐标系中的位置
	vector<cv::Point2f> topIleft = pRectWImg[0].at(0);
	vector<cv::Point2f> topIright = pRectWImg[0].at(1);

	//--右镜头上下两个正方形在世界坐标系中的位置
	vector<cv::Point3f> rightWtop = pRectW[3].at(0);
	vector<cv::Point3f> rightWbottom = pRectW[3].at(1);

	//--右镜头上下两个正方形在图像坐标系中的位置
	vector<cv::Point2f> rightItop = pRectWImg[3].at(0);
	vector<cv::Point2f> rightIbottom = pRectWImg[3].at(1);

	cv::Mat topImg = cv::imread("capture00.bmp");

	cv::Mat botImg = cv::imread("capture01.bmp");

	cv::Mat leftImg = cv::imread("capture02.bmp");

	cv::Mat rightImg = cv::imread("capture03.bmp");

	float imgx, imgy;
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	/*
	Mat RTt(RT[i]);          // 图像1的外参

	camera_c = RTt*WorldLocation;            // 转成摄像机坐标系
	*/
#if 0
	// 一、生成拼接缝
	// 二、将图像分区域
	OnSetSpliceJoint(p3DtableParam->world_show_size, 0, 0, p3DtableParam->worldcarSize, p3DtableParam->fWorldCarBeginX, p3DtableParam->fWorldCarBeginY, p3DtableParam->CalibrateParam);
	//////////////////////////////////////////////////////////////////////////
	// 计算每个镜头中的渐变步长的外参
	OnGenerateRTAverage(p3DtableParam);
	// 求三维坐标之间的旋转矩阵函数
	vector<Point3f> topCRight;
	OnWorldmaptoCamera(topWright, topCRight, P_TOP, p3DtableParam);

	vector<Point3f> rightCtop;
	OnWorldmaptoCamera(rightWtop, rightCtop, P_RIGHT, p3DtableParam);

#endif
	// 使用topCRight和rightCtop求旋转矩阵

#if 0      // 验证
	Point3f pTempp1;
	for(int r = 0; r < rightCtop.size(); r++ )
	{
		pTempp1 = rightCtop.at(r);
		OnCameraCoordinate(pTempp1.x, pTempp1.y, pTempp1.z, rightImg, p3DtableParam->CalibrateParam[RIGHT], imgx, imgy);
	}
#endif
	// cvRodriues2       // 旋转变量变换为旋转矩阵
	// 判断当前的点属于哪个区域
	//cation = OnGetPointLocation(x, y, p3DtableParam);
	//	int rrr = cation&P_CAR;
	// 动态获取RT值
	//Point3f pTempPoint;
	//pTempPoint = topWleft.at(0);
	//OnGet3DRTvalue(pTempPoint.x, pTempPoint.y, P_TOP, TRT, p3DtableParam);

	//////////////////////////////////////////////////////////////////////////

#if 1
	//////////////////////////////////////////////////////////////////////////
	// 通过车辆长度来确定是房车还是宝沃BX7
	// 因为这两辆车的长度不一样
	if (p3DtableParam->worldcarSize.height < CAR_DIVISION)           // 宝沃
	{
		m_CalibrateScheme = BAOWO_BX7;
	}
	else                                                               // 房车
	{
		m_CalibrateScheme = TOURING_CAR;
	}

	// 获取棋盘格大小及起始参数
	OnGetCheckBoardSize(m_vCameraParam, m_iWide, m_iHeight);

	int latRes = m_vCameraParam.latRes;
	int longRes = m_vCameraParam.longRes;


	// 将地面的部分全部投影到一个圆上
	double per_alph = (double)((2 * PI) / latRes);
	double per_theta = (double)(m_vCameraParam.cameraMaxFieldView / longRes);
	double alph = 0;
	double theta = 0;
	double x, y;          // 映射到地面的x和y世界坐标系值

	cv::Point imgLocation;


	double y_top_left;                  // 左上角
	double yTop_topLeft;                // 左上拼接线的上方，融合线
	double yBottom_topLeft;             // 左上拼接线的下方，融合线

	double y_top_right;                 // 右上角
	double yTop_topRight;               // 右上拼接线的上方，融合线
	double yBottom_topRight;            // 右上拼接线的下方，融合线

	double y_bottom_left;               // 左下角
	double yTop_bottomleft;             // 左下拼接线的上方，融合线
	double yBottom_bottomLeft;          // 左下拼接线的下方，融合线

	double y_bottom_right;              // 右下角
	double yTop_bottomright;            // 右下拼接线的上方，融合线
	double yBottom_bottomright;         // 右下拼接线的下方，融合线

//	Matx44d TRT[2];               // 外参值

	double inCarArea;
	vector<cv::Point> contour;
	contour.clear();
	cv::Point car[4] = { { m_iWide / 2, m_iHeight / 2 }, { (-1) * m_iWide / 2, m_iHeight / 2 }, { (-1) * m_iWide / 2, (-1) * m_iHeight / 2 }, { m_iWide / 2, (-1) * m_iHeight / 2 } };
	for (int i = 0; i < 4; i++)
	{
		contour.push_back(car[i]);
	}

	int iTemp = 0;
	cv::Point2f pTemp;
	double dTemp;

	cv::Mat M(latRes, longRes, CV_8UC3, cv::Scalar(0, 0, 0));//其实是2*6的矩阵，因为每个元素有3个通道。

	//////////////////////////////////////////////////////////////////////////
	// 生成查找表
	stu3dTable* p3dTable = new stu3dTable[latRes * longRes];

#ifdef MOTOR_HOME

	Mat topImg = imread("cam_cap1.jpg");

	Mat botImg = imread("cam_cap2.jpg");

	Mat leftImg = imread("cam_cap3.jpg");

	Mat rightImg = imread("cam_cap4.jpg");

#else
	//Mat topImg = imread("capture00.bmp");

	//Mat botImg = imread("capture01.bmp");

	//Mat leftImg = imread("capture02.bmp");

	//Mat rightImg = imread("capture03.bmp");
#endif

	int cation;       // 地面映射到哪个镜头

	//////////////////////////////////////////////////////////////////////////
	// 世界坐标系的值
	cv::Mat worldLocation;                 // 在世界坐标系上的值
	worldLocation.create(4, 1, CV_64F);
	worldLocation.at<double>(2, 0) = 0;
	worldLocation.at<double>(3, 0) = (double)1;

	// 一、生成拼接缝
	// 二、将图像分区域
	OnSetSpliceJoint(p3DtableParam->world_show_size, 0, 0, p3DtableParam->worldcarSize, p3DtableParam->fWorldCarBeginX, p3DtableParam->fWorldCarBeginY, p3DtableParam->CalibrateParam);

	//////////////////////////////////////////////////////////////////////////
	// 计算每个镜头中的渐变步长的外参
//	OnGenerateRTAverage(p3DtableParam);
	// m_vCameraParam.circleRadius // 抽象圆的半径

	cv::Matx44d RTMatx;        // 获取RT外参

	float x1, y1, z1;
	int i = 0;
	int j = 0;
	float x2, y2, z2;
	int imgx1, imgy1;
	
	for (int jx = 0; jx < longRes; jx++)
	{
		//// 计算theta值
		theta = jx*per_theta;
		for (int c = 0; c < latRes; c++)
		{
#if 1
			alph = c*per_alph;
			//////////////////////////////////////////////////////////////////////////
			// 球面对地面的区域
			//x = m_vCameraParam.circleRadius*tan(theta)*cos(alph);
			//y = m_vCameraParam.circleRadius*tan(theta)*sin(alph);
			//////////////////////////////////////////////////////////////////////////

			//////////////////////////////////////////////////////////////////////////
			// 只是球面坐标系展开
			x1 = sin(theta)*cos(alph);
			y1 = sin(theta)*sin(alph);
			z1 = cos(theta);
			//////////////////////////////////////////////////////////////////////////
			x2 = -0.5825*x1 + 0.1564*y1 - 0.6985*z1;
			y2 = -0.6185*x1 + 0.1212*y1 + 0.7115*z1;
			z2 = 0.5274*x1 + 0.3147*y1 + 0.0763*z1;
			//////////////////////////////////////////////////////////////////////////
			OnCameraCoordinate(x2, y2, z2, rightImg, p3DtableParam->CalibrateParam[RIGHT] , imgx1 , imgy1 );

			if ((imgy1 >= 0) && (imgx1 >= 0) && (imgy1 < 720) && (imgx1 < 1280))
			{
				M.data[jx*latRes * 3 + c * 3 + 0] = rightImg.data[imgy1 * 1280 * 3 + imgx1 * 3 + 0];
				M.data[jx*latRes * 3 + c * 3 + 1] = rightImg.data[imgy1 * 1280 * 3 + imgx1 * 3 + 1];
				M.data[jx*latRes * 3 + c * 3 + 2] = rightImg.data[imgy1 * 1280 * 3 + imgx1 * 3 + 2];
			}
			//////////////////////////////////////////////////////////////////////////
#endif
		}
	}

	imwrite( "tempimg//3Dright.bmp" , M );
//	imshow("fff", M);
#endif

#if 0
	for (int i = 0; i < FISHEYE_COUNT; i++ )
	{
	}
	int isize = pRectW[0].size();
	int isize1 = pRectWImg[0].size();

	vector<Point3f> xx = pRectW[0].at(0);
	vector<Point3f> xx1 = pRectW[0].at(1);

	vector<Point2f> yy = pRectWImg[1].at(0);
	vector<Point2f> yy1 = pRectWImg[1].at(1);
#endif
}
#endif
void CGenerate3Dtable::OnWorldmaptoCamera(vector<cv::Point3f> worldpoint1, vector<cv::Point3f>&cameraPoint1, PointLocation pLocation1, G3DtableParam*paramC)
{
	// 动态获取RT值
	cv::Matx44d RT;
	cv::Point3f temp;
	double dTemp;
	cv::Mat camera_c;
	cv::Mat WorldLocation;
	WorldLocation.create( 4 , 1 , CV_64F );
	double*pDtemp;
	cv::Mat RTt;
	RTt.create(4, 4, CV_64F);
	for (int j = 0; j < worldpoint1.size(); j++)
	{

		OnGet3DRTvalue(worldpoint1.at(j).x, worldpoint1.at(j).y, pLocation1, RT, paramC);
		//////////////////////////////////////////////////////////////////////////
		// 世界坐标往相机坐标转换
		WorldLocation.at<double>(0, 0) = worldpoint1.at(j).x;
		WorldLocation.at<double>(1, 0) = worldpoint1.at(j).y;
		WorldLocation.at<double>(2, 0) = 0;
		WorldLocation.at<double>(3, 0) = (double)1;

		pDtemp = (double*)RTt.data;
		for (int i = 0; i < 16; i++)
		{
			pDtemp[i] = RT.val[i];
		}

		camera_c = RTt*WorldLocation;            // 转成摄像机坐标系

		pDtemp = (double*)camera_c.data;

		// 归一化坐标系
		dTemp = sqrt(pDtemp[0] * pDtemp[0] + pDtemp[1] * pDtemp[1] + pDtemp[2] * pDtemp[2]);
		temp.x = pDtemp[0] / dTemp;
		temp.y = pDtemp[1] / dTemp;
		temp.z = pDtemp[2] / dTemp;
		cameraPoint1.push_back(temp);
		//////////////////////////////////////////////////////////////////////////
	}
}

void CGenerate3Dtable::OnCameraCoordinate(float x1, float y1, float z1, cv::Mat&Img, CCalibrate&pCameraParam, int&iSrcX, int&iSrcY)
{
	double dTemp, dTemp1, thrta_d;
	double*pDtemp1;
	double r,scale;
	double thrta, thrta2, thrta4, thrta6, thrta8;
	cv::Mat cameraLocation;
	cameraLocation.create(4, 1, CV_64F);
	cv::Mat A1(pCameraParam.OnGetFishEyeInf().innerParam);
	cv::Mat Dst;
	if (z1 > 0)
	{
		//////////////////////////////////////////////////////////////////////////
		// 摄像机坐标系归一化
		x1 = x1 / z1;
		y1 = y1 / z1;

		dTemp = x1 * x1;
		dTemp = dTemp + y1 * y1;
		r = sqrt(dTemp);
		thrta = atan(r);

		thrta2 = thrta*thrta;
		thrta4 = thrta2*thrta2;
		thrta6 = thrta4*thrta2;
		thrta8 = thrta4*thrta4;

		//dTemp = 0;

		dTemp = thrta2*pCameraParam.OnGetFishEyeInf().distortionParam.val[0];
		dTemp = dTemp + 1;

		dTemp1 = thrta4*pCameraParam.OnGetFishEyeInf().distortionParam.val[1];
		dTemp = dTemp + dTemp1;

		dTemp1 = thrta6*pCameraParam.OnGetFishEyeInf().distortionParam.val[2];
		dTemp = dTemp + dTemp1;

		dTemp1 = thrta8*pCameraParam.OnGetFishEyeInf().distortionParam.val[3];
		dTemp = dTemp + dTemp1;

		thrta_d = thrta*dTemp;

		if (r == 0)
		{
			scale = 1;
		}
		else
		{
			scale = thrta_d / r;
		}

		//////////////////////////////////////////////////////////////////////////
		// 组合成摄像机坐标系向量
		pDtemp1 = (double*)cameraLocation.data;
		pDtemp1[0] = x1 * scale;
		pDtemp1[1] = y1 * scale;
		pDtemp1[2] = 1;
		pDtemp1[3] = 0;


		Dst = A1*cameraLocation;

		pDtemp1 = (double*)Dst.data;
		//		j = pDtemp1[0];         // 获取宽度
		//////////////////////////////////////////////////////////////////////////
		// 临近插值法(四舍五入方法)
		//dTemp = pDtemp1[0] - j;
		//if ( dTemp >= 0.5 )
		//{
		//	j = j + 1;
		//}
		//		i = pDtemp1[1];         // 获取高度
		//dTemp = pDtemp1[1] - i;
		//if ( dTemp >= 0.5 )
		//{
		//	i = i + 1;
		//}
		//////////////////////////////////////////////////////////////////////////

		//if ((pDtemp1[1] >= 0) && (pDtemp1[0] >= 0) && (pDtemp1[1] < (double)720) && (pDtemp1[0] < (double)1280))
		//{
			iSrcX = pDtemp1[0];
			iSrcY = pDtemp1[1];
		//}
	}
}

void CGenerate3Dtable::OnGenerate3Dtable( G3DtableParam* p3DtableParam )
{
	//////////////////////////////////////////////////////////////////////////
	// 通过车辆长度来确定是房车还是宝沃BX7
	// 因为这两辆车的长度不一样
	if (p3DtableParam->worldcarSize.height < CAR_DIVISION)           // 宝沃
	{
		m_CalibrateScheme = BAOWO_BX7;
	}
	else                                                               // 房车
	{
		m_CalibrateScheme = TOURING_CAR;
	}

	// 获取棋盘格大小及起始参数
	OnGetCheckBoardSize(m_vCameraParam, m_iWide, m_iHeight);

	int latRes = m_vCameraParam.latRes;
	int longRes = m_vCameraParam.longRes;


	// 将地面的部分全部投影到一个圆上
	double per_alph = (double)((2*PI)/latRes);
	double per_theta = (double)(m_vCameraParam.cameraMaxFieldView / longRes);
	double alph = 0;
	double theta = 0;
	double x,y;          // 映射到地面的x和y世界坐标系值

	cv::Point imgLocation;


	double y_top_left;                  // 左上角
	double yTop_topLeft;                // 左上拼接线的上方，融合线
	double yBottom_topLeft;             // 左上拼接线的下方，融合线

	double y_top_right;                 // 右上角
	double yTop_topRight;               // 右上拼接线的上方，融合线
	double yBottom_topRight;            // 右上拼接线的下方，融合线

	double y_bottom_left;               // 左下角
	double yTop_bottomleft;             // 左下拼接线的上方，融合线
	double yBottom_bottomLeft;          // 左下拼接线的下方，融合线

	double y_bottom_right;              // 右下角
	double yTop_bottomright;            // 右下拼接线的上方，融合线
	double yBottom_bottomright;         // 右下拼接线的下方，融合线

	cv::Matx44d TRT[2];               // 外参值

	double inCarArea;
	vector<cv::Point> contour;
	contour.clear();
	cv::Point car[4] = { { m_iWide / 2, m_iHeight / 2 }, { (-1) * m_iWide / 2, m_iHeight / 2 }, { (-1) * m_iWide / 2, (-1) * m_iHeight / 2 }, { m_iWide / 2, (-1) * m_iHeight / 2 } };
	for (int i = 0; i < 4; i++)
	{
		contour.push_back(car[i]);
	}

	int iTemp = 0;
	cv::Point2f pTemp;
	double dTemp;

	cv::Mat M(latRes, longRes, CV_8UC3, cv::Scalar(0, 0, 0));//其实是2*6的矩阵，因为每个元素有3个通道。

	//////////////////////////////////////////////////////////////////////////
	// 生成查找表
	stu3dTable* p3dTable = new stu3dTable[latRes * longRes];

//#ifdef 1/*MOTOR_HOME*/

	cv::Mat topImg = cv::imread("capture00.bmp");

	cv::Mat botImg = cv::imread("capture01.bmp");

	cv::Mat leftImg = cv::imread("capture02.bmp");

	cv::Mat rightImg = cv::imread("capture03.bmp");

//#else
//	cv::Mat topImg = cv::imread("capture00.bmp");

//	cv::Mat botImg = cv::imread("capture01.bmp");

//	cv::Mat leftImg = cv::imread("capture02.bmp");

//	cv::Mat rightImg = cv::imread("capture03.bmp");
//#endif

	int cation;       // 地面映射到哪个镜头

	//////////////////////////////////////////////////////////////////////////
	// 世界坐标系的值
	cv::Mat worldLocation;                 // 在世界坐标系上的值
	worldLocation.create( 4 , 1 , CV_64F );
	worldLocation.at<double>(2 , 0) = 0;
	worldLocation.at<double>(3 , 0) = (double)1;

	// 一、生成拼接缝
	// 二、将图像分区域
	OnSetSpliceJoint(p3DtableParam->world_show_size, 0, 0, p3DtableParam->worldcarSize, p3DtableParam->fWorldCarBeginX, p3DtableParam->fWorldCarBeginY, p3DtableParam->CalibrateParam);

	//////////////////////////////////////////////////////////////////////////
	// 计算距离
	float fDistance[4];      // 四条融合线的的距离，0为左上角，1为右上角，2为左下角，3为右下角
	for (int i = 0; i < FISHEYE_COUNT; i++)
	{
		dTemp = abs(m_BMix[2 * i + 1] - m_BMix[2 * i]);
		fDistance[i] = sqrt(m_K[i] * m_K[i] + 1);
		fDistance[i] = dTemp / fDistance[i];
	}

	//////////////////////////////////////////////////////////////////////////
	// 计算每个镜头中的渐变步长的外参
	OnGenerateRTAverage( p3DtableParam );

	// 这是原来的
	double r = sqrt((double)m_iWide*m_iWide + (double)m_iHeight*m_iHeight) / 2;//m_iHeight/3;//   棋盘格
	
//	double r = sqrt((double)1911 * 1911 + (double)4800*4800) / 2;
	// 这是车的宽度
	//double r = sqrt((double)1911*1911 + (double)4800*4800) / 2;;
	double r1 = 0;

	double minRadius = sqrt(m_vCameraParam.circleRadius * m_vCameraParam.circleRadius - r * r );

	// double theta1 = asinf(minRadius / m_vCameraParam.circleRadius);
	double theta1 = asin(r/m_vCameraParam.circleRadius);
	int k = m_vCameraParam.circleRadius*sin(theta1);

	cv::Matx44d RTMatx;        // 获取RT外参


	int i, j;
	for (j = 0 ; j < longRes ; j++ )
	{
		//// 计算theta值
		theta = j*per_theta;
		if ( theta >= PI/2 )
		{
			break;
		}
		for (i = 0; i < latRes; i++ )
		{
			// 计算alph值
			alph = i*per_alph;

			pTemp.x = alph;
			pTemp.y = theta;
			x = minRadius*tan(theta)*cos(alph);
			y = minRadius*tan(theta)*sin(alph);

			iTemp = j * latRes + i;
			p3dTable[iTemp].iLocation = pTemp;

			p3dTable[iTemp].img1Idx = -1;
			p3dTable[iTemp].img2Idx = -1;


			// 计算投影到地面的世界坐标系值
			x = m_vCameraParam.circleRadius*tan(theta)*cos(alph);
			y = m_vCameraParam.circleRadius*tan(theta)*sin(alph);

			// 判断x和y在世界坐标系中的哪个位置，以及车辆所在的位置
			r1 = sqrt(x*x + y*y);

			x = x + m_iWide/2;
			y = y + m_iHeight/2;

//r1 = sqrt(x*x + y*y);

			// 左上角
			y_top_left = m_K[0] * x + m_B[0];
			yTop_topLeft = m_K[0] * x + m_BMix[0];
			yBottom_topLeft = m_K[0] * x + m_BMix[1];

			// 右上角
			y_top_right = m_K[1] * x + m_B[1];
			yTop_topRight = m_K[1] * x + m_BMix[2];
			yBottom_topRight = m_K[1] * x + m_BMix[3];

			// 左下角
			y_bottom_left = m_K[2] * x + m_B[2];
			yTop_bottomleft = m_K[2] * x + m_BMix[4];
			yBottom_bottomLeft = m_K[2] * x + m_BMix[5];

			// 右下角
			y_bottom_right = m_K[3] * x + m_B[3];
			yTop_bottomright = m_K[3] * x + m_BMix[6];
			yBottom_bottomright = m_K[3] * x + m_BMix[7];

			// 判断当前的点属于哪个区域
			cation = OnGetPointLocation( x , y , p3DtableParam );
		//	int rrr = cation&P_CAR;
			
#if 1
			if ( PointLocation(cation&P_NO) == P_NO )
			{
				M.data[(0 + j)*latRes*3 + 3*i ] = 255;
				M.data[(0 + j)*latRes*3 + 3 * i + 1] = 255;
				M.data[(0 + j)*latRes*3 + 3 * i + 2] = 0;
			}

			if ( PointLocation(cation&P_CAR) == P_CAR )
			{

				M.data[(0 + j)*latRes * 3 + 3 * i] = 0;
				M.data[(0 + j)*latRes * 3 + 3 * i + 1] = 255;
				M.data[(0 + j)*latRes * 3 + 3 * i + 2] = 0;
			}
#endif

			if ( PointLocation(cation&P_TOP) == P_TOP )
			{
				//////////////////////////////////////////////////////////////////////////
				// 将当前的(x,y)世界坐标系的值映射到图像坐标系，
				// 并取出图像坐标系相对应的像素点和其位置坐标


				// 判断当前点是否在2D俯视图区域内
				inCarArea = pointPolygonTest(contour, cv::Point2f(x, y), false);

				// 如果当前点在2D俯视图区域内，高度为0，否则人为添加高度
				if( r1<r )
				//if (inCarArea)
				{
					worldLocation.at<double>(0 , 0) = (double)x;
					worldLocation.at<double>(1 , 0) = (double)y;
					worldLocation.at<double>(2 , 0) = 0;
				}
				else
				{

					worldLocation.at<double>(0 , 0) = (double)x;
					worldLocation.at<double>(1 , 0) = (double)y;
					worldLocation.at<double>(2, 0) = (double) (r - r1) / PER_HEIGHT;

//                    Rxx = Rxx/1.02;
					//;
					//worldLocation.at<double>(0 , 0) = (double)m_vCameraParam.circleRadius*sin(theta)*cos(alph);
					//worldLocation.at<double>(1 , 0) = (double)m_vCameraParam.circleRadius*sin(theta)*sin(alph);
					//worldLocation.at<double>(2 , 0) = (double)m_vCameraParam.circleRadius*cos(theta) - minRadius;//(r - r1)/PER_HEIGHT;
				}

				p3dTable[iTemp].img1Idx = TOP;
				if (y > yTop_topLeft)                       // 说明上区域在左上角的重合区域
				{
					p3dTable[iTemp].img2Idx = LEFT;
					TRT[1] = p3DtableParam->CalibrateParam[LEFT].OnGetRTParam(0);

					//////////////////////////////////////////////////////////////////////////
					// 求当前i,j点到拼接缝1的距离
					dTemp = abs((m_K[0] * x - y + m_B[0]) / sqrt(m_K[0] * m_K[0] + 1));
					p3dTable[iTemp].img1Weight = dTemp / fDistance[0];              // 归一化
					p3dTable[iTemp].img1Weight = p3dTable[iTemp].img1Weight + 0.5;
					p3dTable[iTemp].img2Weight = 1 - p3dTable[iTemp].img1Weight;
				}
				else if (y > yTop_topRight)                     // 说明上区域在右上角的重合区域
				{
					p3dTable[iTemp].img2Idx = RIGHT;
					TRT[1] = p3DtableParam->CalibrateParam[RIGHT].OnGetRTParam(0);

					//////////////////////////////////////////////////////////////////////////
					// 求当前i,j到拼接缝2的距离
					dTemp = abs((m_K[1] * x - y + m_B[1]) / sqrt(m_K[1] * m_K[1] + 1));
					p3dTable[iTemp].img1Weight = dTemp / fDistance[1];              // 归一化
					p3dTable[iTemp].img1Weight = p3dTable[iTemp].img1Weight + 0.5;
					p3dTable[iTemp].img2Weight = 1 - p3dTable[iTemp].img1Weight;
				}
				else	// 没有在重合区域
				{
					p3dTable[iTemp].img1Weight = 1;          // 上边的图像为权重为1
					p3dTable[iTemp].img2Weight = 0;
				}

				// 动态获取RT值
				OnGet3DRTvalue(x, y, PointLocation(cation&P_TOP), TRT[0], p3DtableParam);
				mappingFromFisheye_3D(worldLocation, TRT, p3dTable[iTemp], 1280, 720, p3DtableParam->CalibrateParam);

				//////////////////////////////////////////////////////////////////////////
				// 填充M
				imgLocation = p3dTable[iTemp].img1Location;
				M.data[j*latRes * 3 + 3 * i] = topImg.data[imgLocation.y*topImg.cols * 3 + imgLocation.x * 3];
				M.data[j*latRes * 3 + 3 * i + 1] = topImg.data[imgLocation.y*topImg.cols * 3 + imgLocation.x * 3 + 1];
				M.data[j*latRes * 3 + 3 * i + 2] = topImg.data[imgLocation.y*topImg.cols * 3 + imgLocation.x * 3 + 2];
				//////////////////////////////////////////////////////////////////////////
			}


			if ( PointLocation(cation&P_BOTTOM) == P_BOTTOM )
			{
				//////////////////////////////////////////////////////////////////////////
				// 将当前的(x,y)世界坐标系的值映射到图像坐标系，
				// 并取出图像坐标系相对应的像素点和其位置坐标

				// 判断当前点是否在2D俯视图区域内
				inCarArea = pointPolygonTest(contour, cv::Point2f(x, y), false);

				// 如果当前点在2D俯视图区域内，高度为0，否则人为添加高度
				if( r1<r )
				//if (inCarArea)
				{
					worldLocation.at<double>(0 , 0) = (double)x;
					worldLocation.at<double>(1 , 0) = (double)y;
					worldLocation.at<double>(2 , 0) = 0;
				}
				else                         // 在棋盘格区域外部
				{
					worldLocation.at<double>(0 , 0) = x;
					worldLocation.at<double>(1 , 0) = y;
					worldLocation.at<double>(2, 0) = (r - r1) / PER_HEIGHT;
//					Rxx = Rxx/1.02;
				}

				p3dTable[iTemp].img1Idx = BOTTOM;
				if (y < yBottom_bottomLeft)                       // 说明上区域在左上角的重合区域
				{
					p3dTable[iTemp].img2Idx = LEFT;
					TRT[1] = p3DtableParam->CalibrateParam[LEFT].OnGetRTParam(1);//m_fishEyeInf[LEFT].RT[1];

					//////////////////////////////////////////////////////////////////////////
					// 求当前i,j点到拼接缝2的距离
					dTemp = abs((m_K[2] * x - y + m_B[2]) / sqrt(m_K[2] * m_K[2] + 1));
					p3dTable[iTemp].img1Weight = dTemp / fDistance[2];              // 归一化
					p3dTable[iTemp].img1Weight = p3dTable[iTemp].img1Weight + 0.5;
					p3dTable[iTemp].img2Weight = 1 - p3dTable[iTemp].img1Weight;

				}
				else if (y < yBottom_bottomright)                      // 说明上区域在右上角的重合区域
				{
					p3dTable[iTemp].img2Idx = RIGHT;
					TRT[1] = p3DtableParam->CalibrateParam[RIGHT].OnGetRTParam(1);//m_fishEyeInf[RIGHT].RT[1];

					//////////////////////////////////////////////////////////////////////////
					// 求当前i,j到拼接缝4的距离
					dTemp = abs((m_K[3] * x - y + m_B[3]) / sqrt(m_K[3] * m_K[3] + 1));
					p3dTable[iTemp].img1Weight = dTemp / fDistance[3];              // 归一化
					p3dTable[iTemp].img1Weight = p3dTable[iTemp].img1Weight + 0.5;
					p3dTable[iTemp].img2Weight = 1 - p3dTable[iTemp].img1Weight;
				}
				else										// 没有在重合区域
				{
					p3dTable[iTemp].img1Weight = 1;          // 上边的图像为权重为1
					p3dTable[iTemp].img2Weight = 0;
				}

				// 动态获取RT值
				OnGet3DRTvalue(x, y, PointLocation(cation&P_BOTTOM), TRT[0], p3DtableParam);
				mappingFromFisheye_3D(worldLocation, TRT, p3dTable[iTemp], 1280, 720, p3DtableParam->CalibrateParam);

				//////////////////////////////////////////////////////////////////////////
				// 填充M
				imgLocation = p3dTable[iTemp].img1Location;
				M.data[j*latRes * 3 + 3 * i] = botImg.data[imgLocation.y*botImg.cols * 3 + imgLocation.x * 3];
				M.data[j*latRes * 3 + 3 * i + 1] = botImg.data[imgLocation.y*botImg.cols * 3 + imgLocation.x * 3 + 1];
				M.data[j*latRes * 3 + 3 * i + 2] = botImg.data[imgLocation.y*botImg.cols * 3 + imgLocation.x * 3 + 2];
				//////////////////////////////////////////////////////////////////////////
			}
			
			if ( PointLocation(cation&P_LEFT) == P_LEFT )
			{
				//////////////////////////////////////////////////////////////////////////
				// 将当前的(x,y)世界坐标系的值映射到图像坐标系，
				// 并取出图像坐标系相对应的像素点和其位置坐标

				inCarArea = pointPolygonTest(contour, cv::Point2f(x, y), false);

				if( r1<r )
				//if (inCarArea)
				{
					worldLocation.at<double>(0,0) = (double)x;
					worldLocation.at<double>(1,0) = (double)y;
					worldLocation.at<double>(2,0) = 0;
				}
				else
				{
					worldLocation.at<double>(0,0) = x;
					worldLocation.at<double>(1,0) = y;
					worldLocation.at<double>(2, 0) = (r - r1) / PER_HEIGHT;
//					Rxx = Rxx/1.02;
				}

				p3dTable[iTemp].img1Idx = LEFT;

				if (y < yBottom_topLeft)                       // 说明上区域在左上角的重合区域
				{
					p3dTable[iTemp].img2Idx = TOP;
					TRT[1] = p3DtableParam->CalibrateParam[TOP].OnGetRTParam(0);

					//////////////////////////////////////////////////////////////////////////
					// 求当前i,j点到拼接缝1的距离
					dTemp = abs((m_K[0] * x - y + m_B[0]) / sqrt(m_K[0] * m_K[0] + 1));
					p3dTable[iTemp].img1Weight = dTemp / fDistance[0];              // 归一化
					p3dTable[iTemp].img1Weight = p3dTable[iTemp].img1Weight + 0.5;
					p3dTable[iTemp].img2Weight = 1 - p3dTable[iTemp].img1Weight;
				}
				else if (y > yTop_bottomleft)                      // 说明上区域在右上角的重合区域
				{
					p3dTable[iTemp].img2Idx = BOTTOM;
					TRT[1] = p3DtableParam->CalibrateParam[BOTTOM].OnGetRTParam(0);

					//////////////////////////////////////////////////////////////////////////
					// 求当前i,j到拼接缝3的距离
					dTemp = abs((m_K[2] * x - y + m_B[2]) / sqrt(m_K[2] * m_K[2] + 1));
					p3dTable[iTemp].img1Weight = dTemp / fDistance[2];              // 归一化
					p3dTable[iTemp].img1Weight = p3dTable[iTemp].img1Weight + 0.5;
					p3dTable[iTemp].img2Weight = 1 - p3dTable[iTemp].img1Weight;
				}
				else  // 没有在重合区域
				{
					p3dTable[iTemp].img1Weight = 1;          // 上边的图像为权重为1
					p3dTable[iTemp].img2Weight = 0;
				}

				// 动态获取RT值
				OnGet3DRTvalue(x, y, PointLocation(cation&P_LEFT), TRT[0], p3DtableParam);
				mappingFromFisheye_3D(worldLocation, TRT, p3dTable[iTemp], 1280, 720, p3DtableParam->CalibrateParam);

				//////////////////////////////////////////////////////////////////////////
				// 填充M
				imgLocation = p3dTable[iTemp].img1Location;
				M.data[j*latRes * 3 + 3 * i] = leftImg.data[imgLocation.y*leftImg.cols * 3 + imgLocation.x * 3];
				M.data[j*latRes * 3 + 3 * i + 1] = leftImg.data[imgLocation.y*leftImg.cols * 3 + imgLocation.x * 3 + 1];
				M.data[j*latRes * 3 + 3 * i + 2] = leftImg.data[imgLocation.y*leftImg.cols * 3 + imgLocation.x * 3 + 2];
				//////////////////////////////////////////////////////////////////////////
			}

			
			if ( PointLocation(cation&P_RIGHT) == P_RIGHT )
			{
				//////////////////////////////////////////////////////////////////////////
				// 将当前的(x,y)世界坐标系的值映射到图像坐标系，
				// 并取出图像坐标系相对应的像素点和其位置坐标

				inCarArea = pointPolygonTest(contour, cv::Point2f(x, y), false);

				if( r1<r )
				//if (inCarArea)
				{
					worldLocation.at<double>(0,0) = (double)x;
					worldLocation.at<double>(1,0) = (double)y;
					worldLocation.at<double>(2,0) = 0;
				}
				else
				{
					worldLocation.at<double>(0,0) = (double)x;
					worldLocation.at<double>(1,0) = (double)y;
					worldLocation.at<double>(2, 0) = (double)(r - r1) / PER_HEIGHT;
	//				Rxx = Rxx/1.02;
				}

				p3dTable[iTemp].img1Idx = RIGHT;
				if (y < yBottom_topRight)                       // 说明上区域在左上角的重合区域
				{
					p3dTable[iTemp].img2Idx = TOP;
					TRT[1] = p3DtableParam->CalibrateParam[TOP].OnGetRTParam(1);

					//////////////////////////////////////////////////////////////////////////
					// 求当前i,j点到拼接缝2的距离
					dTemp = abs((m_K[1] * x - y + m_B[1]) / sqrt(m_K[1] * m_K[1] + 1));
					p3dTable[iTemp].img1Weight = dTemp / fDistance[1];              // 归一化
					p3dTable[iTemp].img1Weight = p3dTable[iTemp].img1Weight + 0.5;
					p3dTable[iTemp].img2Weight = 1 - p3dTable[iTemp].img1Weight;
				}
				else if (y > yTop_bottomright)                      // 说明上区域在右上角的重合区域
				{
					p3dTable[iTemp].img2Idx = BOTTOM;
					TRT[1] = p3DtableParam->CalibrateParam[BOTTOM].OnGetRTParam(1);

					//////////////////////////////////////////////////////////////////////////
					// 求当前i,j到拼接缝4的距离
					dTemp = abs((m_K[3] * x - y+ m_B[3]) / sqrt(m_K[3] * m_K[3] + 1));
					p3dTable[iTemp].img1Weight = dTemp / fDistance[3];              // 归一化
					p3dTable[iTemp].img1Weight = p3dTable[iTemp].img1Weight + 0.5;
					p3dTable[iTemp].img2Weight = 1 - p3dTable[iTemp].img1Weight;
				}
				else								// 没有在重合区域
				{
					p3dTable[iTemp].img1Weight = 1;          // 上边的图像为权重为1
					p3dTable[iTemp].img2Weight = 0;
				}

				// 动态获取RT值
				OnGet3DRTvalue(x, y, PointLocation(cation&P_RIGHT), TRT[0], p3DtableParam);
				mappingFromFisheye_3D(worldLocation, TRT, p3dTable[iTemp], 1280, 720, p3DtableParam->CalibrateParam);

				//////////////////////////////////////////////////////////////////////////
				// 填充M
				imgLocation = p3dTable[iTemp].img1Location;
				M.data[j*latRes * 3 + 3 * i] = rightImg.data[imgLocation.y*rightImg.cols * 3 + imgLocation.x * 3];
				M.data[j*latRes * 3 + 3 * i + 1] = rightImg.data[imgLocation.y*rightImg.cols * 3 + imgLocation.x * 3 + 1];
				M.data[j*latRes * 3 + 3 * i + 2] = rightImg.data[imgLocation.y*rightImg.cols * 3 + imgLocation.x * 3 + 2];
				//////////////////////////////////////////////////////////////////////////
			}
			
			
		}
	}

	FILE *file = NULL;

	if (file = fopen("tempimg//Table3D.bin", "wb"))
	{
		fwrite(p3dTable, sizeof(stu3dTable), j * latRes, file);
		fclose(file);
	}

	ofstream outf; 
	outf.open("tempimg//3DParam.txt");
	outf << m_vCameraParam.circleRadius << ',' << m_vCameraParam.cameraMaxFieldView << ',' << theta1 << ',' << theta << ',' << longRes << ',' << latRes << endl;
	outf.close();

#if 1/*SHOW_3DIMG*/
	imwrite("kkk3.bmp" , M);
	//imshow("fff" , M);
	//waitKey(0);
#endif

	delete[]p3dTable;
}

void CGenerate3Dtable::mappingFromFisheye_3D(cv::Mat &WorldLocation, cv::Matx44d *RT, stu3dTable &tableInf, int isrcWide, int isrcHeight, CCalibrate *pCameraParam)
{
	cv::Mat camera_c;         // 摄像机坐标系
	//Mat RTt(RT[0]);          // 图像1的外参
	cv::Mat cameraLocation;      // 摄像机坐标系的矩阵
	//Mat A1(m_fishEyeInf[tableInf.img1Idx].innerParam);
//	Mat A1( p3DtableParam->CalibrateParam[tableInf.img1Idx].OnGetFishEyeInf().innerParam );
	cv::Mat A1(pCameraParam[tableInf.img1Idx].OnGetFishEyeInf().innerParam);

	int iTemp = 0;
	if ( tableInf.img2Idx == -1 )
	{
		iTemp = tableInf.img1Idx;
	}
	else
	{
		iTemp = tableInf.img2Idx;
	}


	cv::Mat A2(pCameraParam[iTemp].OnGetFishEyeInf().innerParam);

	// Mat RTt1( RT[1] );        // 图像2的外参

	int i;

	cv::Mat Dst;
	double* pDtemp;
	double* pDtemp1;
	double r;
	double thrta;
	double thrta2;
	double thrta4;
	double thrta6;
	double thrta8;
	double thrta_d;

	double dTemp = 0;
	double dTemp1 = 0;
	double scale;

	int idx[2];
	//	int iTemp;
	idx[0] = tableInf.img1Idx;
	idx[1] = tableInf.img2Idx;


	//int i;         // 高
	//int j;         // 宽

	cameraLocation.create( 4 , 1 , CV_64F );

	for ( i = 0 ; i<2 ; i++ )
	{
		iTemp = idx[i];

		if ( iTemp == -1 )
		{
			break;
		}

		cv::Mat RTt(RT[i]);          // 图像1的外参

		camera_c = RTt*WorldLocation;            // 转成摄像机坐标系

		pDtemp = (double*)camera_c.data;

		if ( pDtemp[2]>0 )
		{
			//////////////////////////////////////////////////////////////////////////
			// 摄像机坐标系归一化
			pDtemp[0] = pDtemp[0]/pDtemp[2];
			pDtemp[1] = pDtemp[1]/pDtemp[2];

			dTemp = pDtemp[0]*pDtemp[0];
			dTemp = dTemp + pDtemp[1]*pDtemp[1];
			r = sqrt( dTemp );
			thrta = atan( r );

			thrta2 = thrta*thrta;
			thrta4 = thrta2*thrta2;
			thrta6 = thrta4*thrta2;
			thrta8 = thrta4*thrta4;

			//dTemp = 0;

			dTemp = thrta2*pCameraParam[iTemp].OnGetFishEyeInf().distortionParam.val[0];
			dTemp = dTemp + 1;

			dTemp1 = thrta4*pCameraParam[iTemp].OnGetFishEyeInf().distortionParam.val[1];
			dTemp = dTemp + dTemp1;

			dTemp1 = thrta6*pCameraParam[iTemp].OnGetFishEyeInf().distortionParam.val[2];
			dTemp = dTemp + dTemp1;

			dTemp1 = thrta8*pCameraParam[iTemp].OnGetFishEyeInf().distortionParam.val[3];
			dTemp = dTemp + dTemp1;

			thrta_d = thrta*dTemp;

			if ( r == 0 )
			{
				scale = 1;
			}
			else
			{
				scale = thrta_d/r;
			}

			//////////////////////////////////////////////////////////////////////////
			// 组合成摄像机坐标系向量
			pDtemp1 = (double*)cameraLocation.data;
			pDtemp1[0] = pDtemp[0]*scale;
			pDtemp1[1] = pDtemp[1]*scale;
			pDtemp1[2] = 1;
			pDtemp1[3] = 0;

			if ( i == 0 )
			{
				Dst = A1*cameraLocation;
			}
			else
			{
				Dst = A2*cameraLocation;
			}

			pDtemp1 = (double*)Dst.data;
			//		j = pDtemp1[0];         // 获取宽度
			//////////////////////////////////////////////////////////////////////////
			// 临近插值法(四舍五入方法)
			//dTemp = pDtemp1[0] - j;
			//if ( dTemp >= 0.5 )
			//{
			//	j = j + 1;
			//}
			//		i = pDtemp1[1];         // 获取高度
			//dTemp = pDtemp1[1] - i;
			//if ( dTemp >= 0.5 )
			//{
			//	i = i + 1;
			//}
			//////////////////////////////////////////////////////////////////////////

			if ( (pDtemp1[1]>=0)&&(pDtemp1[0]>=0)&&(pDtemp1[1]<(double)isrcHeight)&&(pDtemp1[0]<(double)isrcWide) )
			{
				if ( i == 0 )
				{
					tableInf.img1Location.x = pDtemp1[0];           // 宽度
					tableInf.img1Location.y = pDtemp1[1];           // 高度
				}
				if ( i == 1 )
				{
					tableInf.img2Location.x = pDtemp1[0];           // 宽度
					tableInf.img2Location.y = pDtemp1[1];           // 高度
				}
			}
		}
	}
}


CGenerate3Dtable::~CGenerate3Dtable()
{

}