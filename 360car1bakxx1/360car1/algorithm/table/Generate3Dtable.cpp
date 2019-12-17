#include "Generate3Dtable.h"

CGenerate3Dtable::CGenerate3Dtable()
{
	// ͨ������������ĸ߶Ⱥ��ӳ��ǣ���������ӳ�䵽һ��Բ�ϣ�
	// ������ĸ߶�Ӱ����Ļ��ȡ��ӳ���Ӱ��������ܿ�����Χ��

	// ����ᴹֱ���£��н���z�Ḻ���򣬷�ΧΪ[-theta , theta]���ڴ�Ĭ���У�theta=60��
	//m_vCameraParam.cameraMaxFieldView = (double)(180.0f/180.0f)*PI;                     // ������ӳ�������Ϊ120��

	// �ֶ�����Բ�뾶������뾶���õñȽ�С�Ļ�����ô���ֶ��޸İ뾶��С
	//m_vCameraParam.circleRadius       = 8000;                    // Ĭ��Ϊ4000����

	// ������Բ�ĵĸ߶�
	m_CircleCenter = 0;
}



bool CGenerate3Dtable::OnGetCheckBoardSize(struct virtualCameraParam &vCameraParam, int&wide, int&height)
{
	int bRetrun = 0;
	const char*file = "config.ini";          // �����ļ�����·��

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

	if (TOURING_CAR == m_CalibrateScheme)          // ����
	{
		sect = "commonStyle";
		key = "checkBoardWideX";
		wide = iniGetInt(sect, key, 20);

		key = "checkBoardHeight";
		height = iniGetInt(sect, key, 20);
	}
	else			// ����
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

// ��ȡ����С����λ��
int CGenerate3Dtable::OnGetPointLocation(double xLocation , double yLocation ,  G3DtableParam* p3DtableParam )
{
	int Loca = 0;                  // ��ʹ����ʾɶ����û��
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


	// ����Ƿ����ڳ�������
	Loca |= OnCheckisInCarRange( xLocation , yLocation , p3DtableParam->worldcarSize , p3DtableParam->fWorldCarBeginX , p3DtableParam->fWorldCarBeginY );

	// ����Ƿ�����������
	Loca |= OnCheckisInCarTop( xLocation , yLocation , p3DtableParam->worldcarSize , p3DtableParam->fWorldCarBeginX , p3DtableParam->fWorldCarBeginY , p3DtableParam->m_K , p3DtableParam->m_B );

	// ����Ƿ�����������
	Loca |= OnCheckisInCarBottom( xLocation , yLocation , p3DtableParam->worldcarSize , p3DtableParam->fWorldCarBeginX, p3DtableParam->fWorldCarBeginY , p3DtableParam->m_K , p3DtableParam->m_B );

	// ����Ƿ�����������
	Loca |= OnCheckisInCarLeft( xLocation , yLocation , p3DtableParam->worldcarSize , p3DtableParam->fWorldCarBeginX, p3DtableParam->fWorldCarBeginY , p3DtableParam->m_K , p3DtableParam->m_B );

	// ����Ƿ�����������
	Loca |= OnCheckisInCarRight( xLocation , yLocation , p3DtableParam->worldcarSize , p3DtableParam->fWorldCarBeginX, p3DtableParam->fWorldCarBeginY , p3DtableParam->m_K , p3DtableParam->m_B );
	//if ( Loca  P_CAR )
	//{
	//	Loca = Loca|P_CAR;
	//}

	return Loca;
}

// ������
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

// ������
// pK��pB�ֱ���0��1��2��3����������ƴ�ӷ죬����ƴ�ӷ죬����ƴ�ӷ������ƴ�ӷ�
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

// ������
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

// ������
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

// ������
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
	if ( pLocation == P_TOP )        // ǰ����ͷ
	{
		// ��
		if ( x < p3DtableParam->fWorldCarBeginX )
		{
			RT = p3DtableParam->CalibrateParam[TOP].OnGetRTParam( 0 );
		}
		// ��
		if( x > (p3DtableParam->fWorldCarBeginX + p3DtableParam->worldcarSize.width) )
		{
			RT = p3DtableParam->CalibrateParam[TOP].OnGetRTParam( 1 );
		}
		// ��
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

	if ( pLocation == P_BOTTOM )    // ������ͷ
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

	if ( pLocation == P_LEFT )      // ������ͷ
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

	if ( pLocation == P_RIGHT )    // ������ͷ
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
	// ���ƴ�ӷ�
	// ���Ͻ�ƴ�ӷ������ƴ���������������ϵ�ֵ
	float fTemp1;
	float fTemp2;

	//int iTempxx = -2000;
	int iTempxx = 0;
	//int iTempxx1 = 0;

	int bRetrun = 0;

	// ���Ͻ�
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
	// ���Ͻ��ں�����
	// �ں������ں��߼���
	CvSize fPointBottom_topLeft;            // ��ƴ���ߵ��·�
	fPointBottom_topLeft.width = fEndPoint_topLeft.width - m_doubleAreaSize;//(m_isquareToGap-1)*CHECK_BOARD_PATTERN_LEN + m_ixOffset;
	fPointBottom_topLeft.height = fEndPoint_topLeft.height;
	CvSize fPointTop_topLeft;               // ��ƴ���ߵ��Ϸ�
	fPointTop_topLeft.width = fEndPoint_topLeft.width + m_doubleAreaSize;
	fPointTop_topLeft.height = fEndPoint_topLeft.height;

	m_BMix[0] = fPointTop_topLeft.height - m_K[0] * fPointTop_topLeft.width;
	m_BMix[1] = fPointBottom_topLeft.height - m_K[0] * fPointBottom_topLeft.width;
#endif

	//////////////////////////////////////////////////////////////////////////
	// ���Ͻ�ƴ�ӷ�����������ϵ��ֵ
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
	// �ں������ں���
	CvSize fPointBottom_topRight;        // ƴ���ߵ��·�
	fPointBottom_topRight.width = fBeginOriginX + worldShowSize.width - iTempxx + m_doubleAreaSize;
	fPointBottom_topRight.height = fEndPoint_topRight.height;
	CvSize fPointTop_topRight;           // ƴ���ߵ��Ϸ�
	fPointTop_topRight.width = fBeginOriginX + worldShowSize.width - iTempxx - m_doubleAreaSize;
	fPointTop_topRight.height = fEndPoint_topRight.height;

	m_BMix[2] = fPointTop_topRight.height - m_K[1] * fPointTop_topRight.width;
	m_BMix[3] = fPointBottom_topRight.height - m_K[1] * fPointBottom_topRight.width;
#endif
	//////////////////////////////////////////////////////////////////////////

	// ���½�ƴ�ӷ�����������ϵ�ϵ�ֵ
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
	// �ں�����ƴ����
	CvSize fPointBottom_bottomLeft;           // ƴ�����·�
	fPointBottom_bottomLeft.width = fPointTop_topLeft.width + iTempxx;
	fPointBottom_bottomLeft.height = fEndPoint_BottomLeft.height;
	CvSize fPointTop_bottomLeft;              // ƴ���ߵ��Ϸ�
	fPointTop_bottomLeft.width = fPointBottom_topLeft.width + iTempxx;
	fPointTop_bottomLeft.height = fPointBottom_bottomLeft.height;

	m_BMix[4] = fPointTop_bottomLeft.height - m_K[2] * fPointTop_bottomLeft.width;
	m_BMix[5] = fPointBottom_bottomLeft.height - m_K[2] * fPointBottom_bottomLeft.width;
#endif


	// ���½�ƴ�ӷ�����������ϵ�ϵ�ֵ
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
	// �ں�����ƴ����
	CvSize fPointBottom_bottomRight;        // ƴ�����·�
	fPointBottom_bottomRight.width = fPointTop_topRight.width - iTempxx;
	fPointBottom_bottomRight.height = fEndPoint_bottomRight.height;

	CvSize fPointTop_bottomRight;           // ƴ�����Ϸ�
	fPointTop_bottomRight.width = fPointBottom_topRight.width - iTempxx;
	fPointTop_bottomRight.height = fEndPoint_bottomRight.height;

	m_BMix[6] = fPointTop_bottomRight.height - m_K[3] * fPointTop_bottomRight.width;
	m_BMix[7] = fPointBottom_bottomRight.height - m_K[3] * fPointBottom_bottomRight.width;
#endif


	//////////////////////////////////////////////////////////////////////////
	// ��ͼ�����
	//////////////////////////////////////////////////////////////////////////
	// ���������Ƕ����̽��з��������м�Ľ�������
	// x�����Ϸ���
	int iTemp1;
	int iTemp2;
	int i;
	int j;
	iTemp1 = fWorldCarBeginX;// - fBeginOriginX;

	m_wide_area[0] = (int)(iTemp1);          // ��һ������
	//	iTemp1 = worldCarSize.width;
	m_wide_area[1] = (int)(worldCarSize.width);

	m_wide_area[2] = worldShowSize.width - fWorldCarBeginX;// + fBeginOriginX ;
	m_wide_area[2] = (m_wide_area[2] - worldCarSize.width);

	// y�����Ϸ���
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
		if (iTemp1 <= 1)            // ʹ��wideLen[1]
		{
			iTemp2 = m_wide_area[1];
		}
		else                         // ʹ��heightLen[1]
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
	cv::Matx44d TRT;          // ���
	vector<vector<cv::Point3f> >*pRectW = p3DtableParam->pRectW;
	vector<vector<cv::Point2f> >*pRectWImg = p3DtableParam->pRectWImg;
	
	//////////////////////////////////////////////////////////////////////////
	// ʹ��ǰ����������ͷ��ͼ����������
	// �±�ֵ�ֱ�Ϊ0��3

	//--ǰ��ͷ������������������������ϵ�е�λ��
	vector<cv::Point3f> topWleft = pRectW[0].at(0);
	vector<cv::Point3f> topWright = pRectW[0].at(1);

	//--ǰ��ͷ����������������ͼ������ϵ�е�λ��
	vector<cv::Point2f> topIleft = pRectWImg[0].at(0);
	vector<cv::Point2f> topIright = pRectWImg[0].at(1);

	//--�Ҿ�ͷ������������������������ϵ�е�λ��
	vector<cv::Point3f> rightWtop = pRectW[3].at(0);
	vector<cv::Point3f> rightWbottom = pRectW[3].at(1);

	//--�Ҿ�ͷ����������������ͼ������ϵ�е�λ��
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
	Mat RTt(RT[i]);          // ͼ��1�����

	camera_c = RTt*WorldLocation;            // ת�����������ϵ
	*/
#if 0
	// һ������ƴ�ӷ�
	// ������ͼ�������
	OnSetSpliceJoint(p3DtableParam->world_show_size, 0, 0, p3DtableParam->worldcarSize, p3DtableParam->fWorldCarBeginX, p3DtableParam->fWorldCarBeginY, p3DtableParam->CalibrateParam);
	//////////////////////////////////////////////////////////////////////////
	// ����ÿ����ͷ�еĽ��䲽�������
	OnGenerateRTAverage(p3DtableParam);
	// ����ά����֮�����ת������
	vector<Point3f> topCRight;
	OnWorldmaptoCamera(topWright, topCRight, P_TOP, p3DtableParam);

	vector<Point3f> rightCtop;
	OnWorldmaptoCamera(rightWtop, rightCtop, P_RIGHT, p3DtableParam);

#endif
	// ʹ��topCRight��rightCtop����ת����

#if 0      // ��֤
	Point3f pTempp1;
	for(int r = 0; r < rightCtop.size(); r++ )
	{
		pTempp1 = rightCtop.at(r);
		OnCameraCoordinate(pTempp1.x, pTempp1.y, pTempp1.z, rightImg, p3DtableParam->CalibrateParam[RIGHT], imgx, imgy);
	}
#endif
	// cvRodriues2       // ��ת�����任Ϊ��ת����
	// �жϵ�ǰ�ĵ������ĸ�����
	//cation = OnGetPointLocation(x, y, p3DtableParam);
	//	int rrr = cation&P_CAR;
	// ��̬��ȡRTֵ
	//Point3f pTempPoint;
	//pTempPoint = topWleft.at(0);
	//OnGet3DRTvalue(pTempPoint.x, pTempPoint.y, P_TOP, TRT, p3DtableParam);

	//////////////////////////////////////////////////////////////////////////

#if 1
	//////////////////////////////////////////////////////////////////////////
	// ͨ������������ȷ���Ƿ������Ǳ���BX7
	// ��Ϊ���������ĳ��Ȳ�һ��
	if (p3DtableParam->worldcarSize.height < CAR_DIVISION)           // ����
	{
		m_CalibrateScheme = BAOWO_BX7;
	}
	else                                                               // ����
	{
		m_CalibrateScheme = TOURING_CAR;
	}

	// ��ȡ���̸��С����ʼ����
	OnGetCheckBoardSize(m_vCameraParam, m_iWide, m_iHeight);

	int latRes = m_vCameraParam.latRes;
	int longRes = m_vCameraParam.longRes;


	// ������Ĳ���ȫ��ͶӰ��һ��Բ��
	double per_alph = (double)((2 * PI) / latRes);
	double per_theta = (double)(m_vCameraParam.cameraMaxFieldView / longRes);
	double alph = 0;
	double theta = 0;
	double x, y;          // ӳ�䵽�����x��y��������ϵֵ

	cv::Point imgLocation;


	double y_top_left;                  // ���Ͻ�
	double yTop_topLeft;                // ����ƴ���ߵ��Ϸ����ں���
	double yBottom_topLeft;             // ����ƴ���ߵ��·����ں���

	double y_top_right;                 // ���Ͻ�
	double yTop_topRight;               // ����ƴ���ߵ��Ϸ����ں���
	double yBottom_topRight;            // ����ƴ���ߵ��·����ں���

	double y_bottom_left;               // ���½�
	double yTop_bottomleft;             // ����ƴ���ߵ��Ϸ����ں���
	double yBottom_bottomLeft;          // ����ƴ���ߵ��·����ں���

	double y_bottom_right;              // ���½�
	double yTop_bottomright;            // ����ƴ���ߵ��Ϸ����ں���
	double yBottom_bottomright;         // ����ƴ���ߵ��·����ں���

//	Matx44d TRT[2];               // ���ֵ

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

	cv::Mat M(latRes, longRes, CV_8UC3, cv::Scalar(0, 0, 0));//��ʵ��2*6�ľ�����Ϊÿ��Ԫ����3��ͨ����

	//////////////////////////////////////////////////////////////////////////
	// ���ɲ��ұ�
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

	int cation;       // ����ӳ�䵽�ĸ���ͷ

	//////////////////////////////////////////////////////////////////////////
	// ��������ϵ��ֵ
	cv::Mat worldLocation;                 // ����������ϵ�ϵ�ֵ
	worldLocation.create(4, 1, CV_64F);
	worldLocation.at<double>(2, 0) = 0;
	worldLocation.at<double>(3, 0) = (double)1;

	// һ������ƴ�ӷ�
	// ������ͼ�������
	OnSetSpliceJoint(p3DtableParam->world_show_size, 0, 0, p3DtableParam->worldcarSize, p3DtableParam->fWorldCarBeginX, p3DtableParam->fWorldCarBeginY, p3DtableParam->CalibrateParam);

	//////////////////////////////////////////////////////////////////////////
	// ����ÿ����ͷ�еĽ��䲽�������
//	OnGenerateRTAverage(p3DtableParam);
	// m_vCameraParam.circleRadius // ����Բ�İ뾶

	cv::Matx44d RTMatx;        // ��ȡRT���

	float x1, y1, z1;
	int i = 0;
	int j = 0;
	float x2, y2, z2;
	int imgx1, imgy1;
	
	for (int jx = 0; jx < longRes; jx++)
	{
		//// ����thetaֵ
		theta = jx*per_theta;
		for (int c = 0; c < latRes; c++)
		{
#if 1
			alph = c*per_alph;
			//////////////////////////////////////////////////////////////////////////
			// ����Ե��������
			//x = m_vCameraParam.circleRadius*tan(theta)*cos(alph);
			//y = m_vCameraParam.circleRadius*tan(theta)*sin(alph);
			//////////////////////////////////////////////////////////////////////////

			//////////////////////////////////////////////////////////////////////////
			// ֻ����������ϵչ��
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
	// ��̬��ȡRTֵ
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
		// �����������������ת��
		WorldLocation.at<double>(0, 0) = worldpoint1.at(j).x;
		WorldLocation.at<double>(1, 0) = worldpoint1.at(j).y;
		WorldLocation.at<double>(2, 0) = 0;
		WorldLocation.at<double>(3, 0) = (double)1;

		pDtemp = (double*)RTt.data;
		for (int i = 0; i < 16; i++)
		{
			pDtemp[i] = RT.val[i];
		}

		camera_c = RTt*WorldLocation;            // ת�����������ϵ

		pDtemp = (double*)camera_c.data;

		// ��һ������ϵ
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
		// ���������ϵ��һ��
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
		// ��ϳ����������ϵ����
		pDtemp1 = (double*)cameraLocation.data;
		pDtemp1[0] = x1 * scale;
		pDtemp1[1] = y1 * scale;
		pDtemp1[2] = 1;
		pDtemp1[3] = 0;


		Dst = A1*cameraLocation;

		pDtemp1 = (double*)Dst.data;
		//		j = pDtemp1[0];         // ��ȡ���
		//////////////////////////////////////////////////////////////////////////
		// �ٽ���ֵ��(�������뷽��)
		//dTemp = pDtemp1[0] - j;
		//if ( dTemp >= 0.5 )
		//{
		//	j = j + 1;
		//}
		//		i = pDtemp1[1];         // ��ȡ�߶�
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
	// ͨ������������ȷ���Ƿ������Ǳ���BX7
	// ��Ϊ���������ĳ��Ȳ�һ��
	if (p3DtableParam->worldcarSize.height < CAR_DIVISION)           // ����
	{
		m_CalibrateScheme = BAOWO_BX7;
	}
	else                                                               // ����
	{
		m_CalibrateScheme = TOURING_CAR;
	}

	// ��ȡ���̸��С����ʼ����
	OnGetCheckBoardSize(m_vCameraParam, m_iWide, m_iHeight);

	int latRes = m_vCameraParam.latRes;
	int longRes = m_vCameraParam.longRes;


	// ������Ĳ���ȫ��ͶӰ��һ��Բ��
	double per_alph = (double)((2*PI)/latRes);
	double per_theta = (double)(m_vCameraParam.cameraMaxFieldView / longRes);
	double alph = 0;
	double theta = 0;
	double x,y;          // ӳ�䵽�����x��y��������ϵֵ

	cv::Point imgLocation;


	double y_top_left;                  // ���Ͻ�
	double yTop_topLeft;                // ����ƴ���ߵ��Ϸ����ں���
	double yBottom_topLeft;             // ����ƴ���ߵ��·����ں���

	double y_top_right;                 // ���Ͻ�
	double yTop_topRight;               // ����ƴ���ߵ��Ϸ����ں���
	double yBottom_topRight;            // ����ƴ���ߵ��·����ں���

	double y_bottom_left;               // ���½�
	double yTop_bottomleft;             // ����ƴ���ߵ��Ϸ����ں���
	double yBottom_bottomLeft;          // ����ƴ���ߵ��·����ں���

	double y_bottom_right;              // ���½�
	double yTop_bottomright;            // ����ƴ���ߵ��Ϸ����ں���
	double yBottom_bottomright;         // ����ƴ���ߵ��·����ں���

	cv::Matx44d TRT[2];               // ���ֵ

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

	cv::Mat M(latRes, longRes, CV_8UC3, cv::Scalar(0, 0, 0));//��ʵ��2*6�ľ�����Ϊÿ��Ԫ����3��ͨ����

	//////////////////////////////////////////////////////////////////////////
	// ���ɲ��ұ�
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

	int cation;       // ����ӳ�䵽�ĸ���ͷ

	//////////////////////////////////////////////////////////////////////////
	// ��������ϵ��ֵ
	cv::Mat worldLocation;                 // ����������ϵ�ϵ�ֵ
	worldLocation.create( 4 , 1 , CV_64F );
	worldLocation.at<double>(2 , 0) = 0;
	worldLocation.at<double>(3 , 0) = (double)1;

	// һ������ƴ�ӷ�
	// ������ͼ�������
	OnSetSpliceJoint(p3DtableParam->world_show_size, 0, 0, p3DtableParam->worldcarSize, p3DtableParam->fWorldCarBeginX, p3DtableParam->fWorldCarBeginY, p3DtableParam->CalibrateParam);

	//////////////////////////////////////////////////////////////////////////
	// �������
	float fDistance[4];      // �����ں��ߵĵľ��룬0Ϊ���Ͻǣ�1Ϊ���Ͻǣ�2Ϊ���½ǣ�3Ϊ���½�
	for (int i = 0; i < FISHEYE_COUNT; i++)
	{
		dTemp = abs(m_BMix[2 * i + 1] - m_BMix[2 * i]);
		fDistance[i] = sqrt(m_K[i] * m_K[i] + 1);
		fDistance[i] = dTemp / fDistance[i];
	}

	//////////////////////////////////////////////////////////////////////////
	// ����ÿ����ͷ�еĽ��䲽�������
	OnGenerateRTAverage( p3DtableParam );

	// ����ԭ����
	double r = sqrt((double)m_iWide*m_iWide + (double)m_iHeight*m_iHeight) / 2;//m_iHeight/3;//   ���̸�
	
//	double r = sqrt((double)1911 * 1911 + (double)4800*4800) / 2;
	// ���ǳ��Ŀ��
	//double r = sqrt((double)1911*1911 + (double)4800*4800) / 2;;
	double r1 = 0;

	double minRadius = sqrt(m_vCameraParam.circleRadius * m_vCameraParam.circleRadius - r * r );

	// double theta1 = asinf(minRadius / m_vCameraParam.circleRadius);
	double theta1 = asin(r/m_vCameraParam.circleRadius);
	int k = m_vCameraParam.circleRadius*sin(theta1);

	cv::Matx44d RTMatx;        // ��ȡRT���


	int i, j;
	for (j = 0 ; j < longRes ; j++ )
	{
		//// ����thetaֵ
		theta = j*per_theta;
		if ( theta >= PI/2 )
		{
			break;
		}
		for (i = 0; i < latRes; i++ )
		{
			// ����alphֵ
			alph = i*per_alph;

			pTemp.x = alph;
			pTemp.y = theta;
			x = minRadius*tan(theta)*cos(alph);
			y = minRadius*tan(theta)*sin(alph);

			iTemp = j * latRes + i;
			p3dTable[iTemp].iLocation = pTemp;

			p3dTable[iTemp].img1Idx = -1;
			p3dTable[iTemp].img2Idx = -1;


			// ����ͶӰ���������������ϵֵ
			x = m_vCameraParam.circleRadius*tan(theta)*cos(alph);
			y = m_vCameraParam.circleRadius*tan(theta)*sin(alph);

			// �ж�x��y����������ϵ�е��ĸ�λ�ã��Լ��������ڵ�λ��
			r1 = sqrt(x*x + y*y);

			x = x + m_iWide/2;
			y = y + m_iHeight/2;

//r1 = sqrt(x*x + y*y);

			// ���Ͻ�
			y_top_left = m_K[0] * x + m_B[0];
			yTop_topLeft = m_K[0] * x + m_BMix[0];
			yBottom_topLeft = m_K[0] * x + m_BMix[1];

			// ���Ͻ�
			y_top_right = m_K[1] * x + m_B[1];
			yTop_topRight = m_K[1] * x + m_BMix[2];
			yBottom_topRight = m_K[1] * x + m_BMix[3];

			// ���½�
			y_bottom_left = m_K[2] * x + m_B[2];
			yTop_bottomleft = m_K[2] * x + m_BMix[4];
			yBottom_bottomLeft = m_K[2] * x + m_BMix[5];

			// ���½�
			y_bottom_right = m_K[3] * x + m_B[3];
			yTop_bottomright = m_K[3] * x + m_BMix[6];
			yBottom_bottomright = m_K[3] * x + m_BMix[7];

			// �жϵ�ǰ�ĵ������ĸ�����
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
				// ����ǰ��(x,y)��������ϵ��ֵӳ�䵽ͼ������ϵ��
				// ��ȡ��ͼ������ϵ���Ӧ�����ص����λ������


				// �жϵ�ǰ���Ƿ���2D����ͼ������
				inCarArea = pointPolygonTest(contour, cv::Point2f(x, y), false);

				// �����ǰ����2D����ͼ�����ڣ��߶�Ϊ0��������Ϊ��Ӹ߶�
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
				if (y > yTop_topLeft)                       // ˵�������������Ͻǵ��غ�����
				{
					p3dTable[iTemp].img2Idx = LEFT;
					TRT[1] = p3DtableParam->CalibrateParam[LEFT].OnGetRTParam(0);

					//////////////////////////////////////////////////////////////////////////
					// ��ǰi,j�㵽ƴ�ӷ�1�ľ���
					dTemp = abs((m_K[0] * x - y + m_B[0]) / sqrt(m_K[0] * m_K[0] + 1));
					p3dTable[iTemp].img1Weight = dTemp / fDistance[0];              // ��һ��
					p3dTable[iTemp].img1Weight = p3dTable[iTemp].img1Weight + 0.5;
					p3dTable[iTemp].img2Weight = 1 - p3dTable[iTemp].img1Weight;
				}
				else if (y > yTop_topRight)                     // ˵�������������Ͻǵ��غ�����
				{
					p3dTable[iTemp].img2Idx = RIGHT;
					TRT[1] = p3DtableParam->CalibrateParam[RIGHT].OnGetRTParam(0);

					//////////////////////////////////////////////////////////////////////////
					// ��ǰi,j��ƴ�ӷ�2�ľ���
					dTemp = abs((m_K[1] * x - y + m_B[1]) / sqrt(m_K[1] * m_K[1] + 1));
					p3dTable[iTemp].img1Weight = dTemp / fDistance[1];              // ��һ��
					p3dTable[iTemp].img1Weight = p3dTable[iTemp].img1Weight + 0.5;
					p3dTable[iTemp].img2Weight = 1 - p3dTable[iTemp].img1Weight;
				}
				else	// û�����غ�����
				{
					p3dTable[iTemp].img1Weight = 1;          // �ϱߵ�ͼ��ΪȨ��Ϊ1
					p3dTable[iTemp].img2Weight = 0;
				}

				// ��̬��ȡRTֵ
				OnGet3DRTvalue(x, y, PointLocation(cation&P_TOP), TRT[0], p3DtableParam);
				mappingFromFisheye_3D(worldLocation, TRT, p3dTable[iTemp], 1280, 720, p3DtableParam->CalibrateParam);

				//////////////////////////////////////////////////////////////////////////
				// ���M
				imgLocation = p3dTable[iTemp].img1Location;
				M.data[j*latRes * 3 + 3 * i] = topImg.data[imgLocation.y*topImg.cols * 3 + imgLocation.x * 3];
				M.data[j*latRes * 3 + 3 * i + 1] = topImg.data[imgLocation.y*topImg.cols * 3 + imgLocation.x * 3 + 1];
				M.data[j*latRes * 3 + 3 * i + 2] = topImg.data[imgLocation.y*topImg.cols * 3 + imgLocation.x * 3 + 2];
				//////////////////////////////////////////////////////////////////////////
			}


			if ( PointLocation(cation&P_BOTTOM) == P_BOTTOM )
			{
				//////////////////////////////////////////////////////////////////////////
				// ����ǰ��(x,y)��������ϵ��ֵӳ�䵽ͼ������ϵ��
				// ��ȡ��ͼ������ϵ���Ӧ�����ص����λ������

				// �жϵ�ǰ���Ƿ���2D����ͼ������
				inCarArea = pointPolygonTest(contour, cv::Point2f(x, y), false);

				// �����ǰ����2D����ͼ�����ڣ��߶�Ϊ0��������Ϊ��Ӹ߶�
				if( r1<r )
				//if (inCarArea)
				{
					worldLocation.at<double>(0 , 0) = (double)x;
					worldLocation.at<double>(1 , 0) = (double)y;
					worldLocation.at<double>(2 , 0) = 0;
				}
				else                         // �����̸������ⲿ
				{
					worldLocation.at<double>(0 , 0) = x;
					worldLocation.at<double>(1 , 0) = y;
					worldLocation.at<double>(2, 0) = (r - r1) / PER_HEIGHT;
//					Rxx = Rxx/1.02;
				}

				p3dTable[iTemp].img1Idx = BOTTOM;
				if (y < yBottom_bottomLeft)                       // ˵�������������Ͻǵ��غ�����
				{
					p3dTable[iTemp].img2Idx = LEFT;
					TRT[1] = p3DtableParam->CalibrateParam[LEFT].OnGetRTParam(1);//m_fishEyeInf[LEFT].RT[1];

					//////////////////////////////////////////////////////////////////////////
					// ��ǰi,j�㵽ƴ�ӷ�2�ľ���
					dTemp = abs((m_K[2] * x - y + m_B[2]) / sqrt(m_K[2] * m_K[2] + 1));
					p3dTable[iTemp].img1Weight = dTemp / fDistance[2];              // ��һ��
					p3dTable[iTemp].img1Weight = p3dTable[iTemp].img1Weight + 0.5;
					p3dTable[iTemp].img2Weight = 1 - p3dTable[iTemp].img1Weight;

				}
				else if (y < yBottom_bottomright)                      // ˵�������������Ͻǵ��غ�����
				{
					p3dTable[iTemp].img2Idx = RIGHT;
					TRT[1] = p3DtableParam->CalibrateParam[RIGHT].OnGetRTParam(1);//m_fishEyeInf[RIGHT].RT[1];

					//////////////////////////////////////////////////////////////////////////
					// ��ǰi,j��ƴ�ӷ�4�ľ���
					dTemp = abs((m_K[3] * x - y + m_B[3]) / sqrt(m_K[3] * m_K[3] + 1));
					p3dTable[iTemp].img1Weight = dTemp / fDistance[3];              // ��һ��
					p3dTable[iTemp].img1Weight = p3dTable[iTemp].img1Weight + 0.5;
					p3dTable[iTemp].img2Weight = 1 - p3dTable[iTemp].img1Weight;
				}
				else										// û�����غ�����
				{
					p3dTable[iTemp].img1Weight = 1;          // �ϱߵ�ͼ��ΪȨ��Ϊ1
					p3dTable[iTemp].img2Weight = 0;
				}

				// ��̬��ȡRTֵ
				OnGet3DRTvalue(x, y, PointLocation(cation&P_BOTTOM), TRT[0], p3DtableParam);
				mappingFromFisheye_3D(worldLocation, TRT, p3dTable[iTemp], 1280, 720, p3DtableParam->CalibrateParam);

				//////////////////////////////////////////////////////////////////////////
				// ���M
				imgLocation = p3dTable[iTemp].img1Location;
				M.data[j*latRes * 3 + 3 * i] = botImg.data[imgLocation.y*botImg.cols * 3 + imgLocation.x * 3];
				M.data[j*latRes * 3 + 3 * i + 1] = botImg.data[imgLocation.y*botImg.cols * 3 + imgLocation.x * 3 + 1];
				M.data[j*latRes * 3 + 3 * i + 2] = botImg.data[imgLocation.y*botImg.cols * 3 + imgLocation.x * 3 + 2];
				//////////////////////////////////////////////////////////////////////////
			}
			
			if ( PointLocation(cation&P_LEFT) == P_LEFT )
			{
				//////////////////////////////////////////////////////////////////////////
				// ����ǰ��(x,y)��������ϵ��ֵӳ�䵽ͼ������ϵ��
				// ��ȡ��ͼ������ϵ���Ӧ�����ص����λ������

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

				if (y < yBottom_topLeft)                       // ˵�������������Ͻǵ��غ�����
				{
					p3dTable[iTemp].img2Idx = TOP;
					TRT[1] = p3DtableParam->CalibrateParam[TOP].OnGetRTParam(0);

					//////////////////////////////////////////////////////////////////////////
					// ��ǰi,j�㵽ƴ�ӷ�1�ľ���
					dTemp = abs((m_K[0] * x - y + m_B[0]) / sqrt(m_K[0] * m_K[0] + 1));
					p3dTable[iTemp].img1Weight = dTemp / fDistance[0];              // ��һ��
					p3dTable[iTemp].img1Weight = p3dTable[iTemp].img1Weight + 0.5;
					p3dTable[iTemp].img2Weight = 1 - p3dTable[iTemp].img1Weight;
				}
				else if (y > yTop_bottomleft)                      // ˵�������������Ͻǵ��غ�����
				{
					p3dTable[iTemp].img2Idx = BOTTOM;
					TRT[1] = p3DtableParam->CalibrateParam[BOTTOM].OnGetRTParam(0);

					//////////////////////////////////////////////////////////////////////////
					// ��ǰi,j��ƴ�ӷ�3�ľ���
					dTemp = abs((m_K[2] * x - y + m_B[2]) / sqrt(m_K[2] * m_K[2] + 1));
					p3dTable[iTemp].img1Weight = dTemp / fDistance[2];              // ��һ��
					p3dTable[iTemp].img1Weight = p3dTable[iTemp].img1Weight + 0.5;
					p3dTable[iTemp].img2Weight = 1 - p3dTable[iTemp].img1Weight;
				}
				else  // û�����غ�����
				{
					p3dTable[iTemp].img1Weight = 1;          // �ϱߵ�ͼ��ΪȨ��Ϊ1
					p3dTable[iTemp].img2Weight = 0;
				}

				// ��̬��ȡRTֵ
				OnGet3DRTvalue(x, y, PointLocation(cation&P_LEFT), TRT[0], p3DtableParam);
				mappingFromFisheye_3D(worldLocation, TRT, p3dTable[iTemp], 1280, 720, p3DtableParam->CalibrateParam);

				//////////////////////////////////////////////////////////////////////////
				// ���M
				imgLocation = p3dTable[iTemp].img1Location;
				M.data[j*latRes * 3 + 3 * i] = leftImg.data[imgLocation.y*leftImg.cols * 3 + imgLocation.x * 3];
				M.data[j*latRes * 3 + 3 * i + 1] = leftImg.data[imgLocation.y*leftImg.cols * 3 + imgLocation.x * 3 + 1];
				M.data[j*latRes * 3 + 3 * i + 2] = leftImg.data[imgLocation.y*leftImg.cols * 3 + imgLocation.x * 3 + 2];
				//////////////////////////////////////////////////////////////////////////
			}

			
			if ( PointLocation(cation&P_RIGHT) == P_RIGHT )
			{
				//////////////////////////////////////////////////////////////////////////
				// ����ǰ��(x,y)��������ϵ��ֵӳ�䵽ͼ������ϵ��
				// ��ȡ��ͼ������ϵ���Ӧ�����ص����λ������

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
				if (y < yBottom_topRight)                       // ˵�������������Ͻǵ��غ�����
				{
					p3dTable[iTemp].img2Idx = TOP;
					TRT[1] = p3DtableParam->CalibrateParam[TOP].OnGetRTParam(1);

					//////////////////////////////////////////////////////////////////////////
					// ��ǰi,j�㵽ƴ�ӷ�2�ľ���
					dTemp = abs((m_K[1] * x - y + m_B[1]) / sqrt(m_K[1] * m_K[1] + 1));
					p3dTable[iTemp].img1Weight = dTemp / fDistance[1];              // ��һ��
					p3dTable[iTemp].img1Weight = p3dTable[iTemp].img1Weight + 0.5;
					p3dTable[iTemp].img2Weight = 1 - p3dTable[iTemp].img1Weight;
				}
				else if (y > yTop_bottomright)                      // ˵�������������Ͻǵ��غ�����
				{
					p3dTable[iTemp].img2Idx = BOTTOM;
					TRT[1] = p3DtableParam->CalibrateParam[BOTTOM].OnGetRTParam(1);

					//////////////////////////////////////////////////////////////////////////
					// ��ǰi,j��ƴ�ӷ�4�ľ���
					dTemp = abs((m_K[3] * x - y+ m_B[3]) / sqrt(m_K[3] * m_K[3] + 1));
					p3dTable[iTemp].img1Weight = dTemp / fDistance[3];              // ��һ��
					p3dTable[iTemp].img1Weight = p3dTable[iTemp].img1Weight + 0.5;
					p3dTable[iTemp].img2Weight = 1 - p3dTable[iTemp].img1Weight;
				}
				else								// û�����غ�����
				{
					p3dTable[iTemp].img1Weight = 1;          // �ϱߵ�ͼ��ΪȨ��Ϊ1
					p3dTable[iTemp].img2Weight = 0;
				}

				// ��̬��ȡRTֵ
				OnGet3DRTvalue(x, y, PointLocation(cation&P_RIGHT), TRT[0], p3DtableParam);
				mappingFromFisheye_3D(worldLocation, TRT, p3dTable[iTemp], 1280, 720, p3DtableParam->CalibrateParam);

				//////////////////////////////////////////////////////////////////////////
				// ���M
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
	cv::Mat camera_c;         // ���������ϵ
	//Mat RTt(RT[0]);          // ͼ��1�����
	cv::Mat cameraLocation;      // ���������ϵ�ľ���
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

	// Mat RTt1( RT[1] );        // ͼ��2�����

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


	//int i;         // ��
	//int j;         // ��

	cameraLocation.create( 4 , 1 , CV_64F );

	for ( i = 0 ; i<2 ; i++ )
	{
		iTemp = idx[i];

		if ( iTemp == -1 )
		{
			break;
		}

		cv::Mat RTt(RT[i]);          // ͼ��1�����

		camera_c = RTt*WorldLocation;            // ת�����������ϵ

		pDtemp = (double*)camera_c.data;

		if ( pDtemp[2]>0 )
		{
			//////////////////////////////////////////////////////////////////////////
			// ���������ϵ��һ��
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
			// ��ϳ����������ϵ����
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
			//		j = pDtemp1[0];         // ��ȡ���
			//////////////////////////////////////////////////////////////////////////
			// �ٽ���ֵ��(�������뷽��)
			//dTemp = pDtemp1[0] - j;
			//if ( dTemp >= 0.5 )
			//{
			//	j = j + 1;
			//}
			//		i = pDtemp1[1];         // ��ȡ�߶�
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
					tableInf.img1Location.x = pDtemp1[0];           // ���
					tableInf.img1Location.y = pDtemp1[1];           // �߶�
				}
				if ( i == 1 )
				{
					tableInf.img2Location.x = pDtemp1[0];           // ���
					tableInf.img2Location.y = pDtemp1[1];           // �߶�
				}
			}
		}
	}
}


CGenerate3Dtable::~CGenerate3Dtable()
{

}