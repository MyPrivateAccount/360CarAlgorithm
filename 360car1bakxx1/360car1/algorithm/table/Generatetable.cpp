#include "Generatetable.h"
#include "../iniFile/inirw.h"
#include <stdint.h>

CGenerateTable::CGenerateTable( CCalibrate* pCalibrate , int iNum , int sourceWide , int sourceHeight )
{
	m_pCalibrate = pCalibrate;
	m_iCalibrateNum = iNum;

	m_iSourceWide = sourceWide;
	m_iSourceHeight = sourceHeight;
}

bool CGenerateTable::OnGenerateInTable( stu2dTable* p2dTable, CvSize2D32f worldShowSize , float fBeginOriginX , float fBeginOriginY , CvSize2D32f worldCarSize , float fWorldCarBeginX , float fWorldCarBeginY , int iWidePixel , int iHeightPixel )
{
	bool bReturn = true;

	//////////////////////////////////////////////////////////////////////////


	double pixel_wide = worldShowSize.width/iWidePixel;
	double pixel_height = worldShowSize.height/iHeightPixel;

	// 一、生成拼接缝
	// 二、将图像分区域
	OnSetSpliceJoint( worldShowSize , 0, 0, worldCarSize , fWorldCarBeginX , fWorldCarBeginY , pixel_wide , pixel_height );

	//////////////////////////////////////////////////////////////////////////
	// 生成二进制查找表
	bReturn = OnGenerateBinaryTable( p2dTable, fBeginOriginX, fBeginOriginY , pixel_wide , pixel_height , iWidePixel , iHeightPixel );

	return bReturn;
}

void CGenerateTable::OnGenerateCorrectionImg(int iLocation, cv::Size SpreadSize, stu_spreadImg*pBuffer)
{
	cv::Matx34d*innerParam = m_pCalibrate[iLocation].OnGetInnerParam();             // 获取内参
	cv::Matx14d*distortion = m_pCalibrate[iLocation].OnGetDistortionParam();        // 获取畸变系数

	char valstr[64];

	cv::Matx33d m_intrinsic_matrix;

	cv::Vec4d m_distortion_coeffs;

	for ( int i = 0 ; i<3 ; i++ )
	{
		for ( int j = 0 ; j<3 ; j++)
		{
			m_intrinsic_matrix.val[i*3 + j] = innerParam->val[i*4 + j];
		}
	}

	for ( int i = 0 ; i<4 ; i++ )
	{
		m_distortion_coeffs.val[i] = m_distortion_coeffs.val[i];
	}
	

	cv::Mat intrinsic_mat(m_intrinsic_matrix), new_intrinsic_mat;
	cv::Mat R = cv::Mat::eye(3, 3, CV_32F);

	new_intrinsic_mat = intrinsic_mat.clone();

	new_intrinsic_mat.at<double>(0 , 0) *= 0.5;//0.7;
	new_intrinsic_mat.at<double>(1 , 1) *= 0.5;//0.85;
	new_intrinsic_mat.at<double>(0,2) = 0.5*SpreadSize.width;
	new_intrinsic_mat.at<double>(1,2) = 0.5*SpreadSize.height;

	interpSpreadTable* pinterpSpreadTable = new interpSpreadTable[SpreadSize.width*SpreadSize.height];

	initUndistortRectifyMapSpread(m_intrinsic_matrix, m_distortion_coeffs, R, new_intrinsic_mat, SpreadSize, CV_32F, pBuffer, pinterpSpreadTable);


	sprintf(valstr, "tempimg//spread_%d.bin", iLocation);
	FILE*filex = NULL;

	if ( filex = fopen( valstr , "wb" ) )
	{
		fwrite( pBuffer , sizeof(stu_spreadImg) , SpreadSize.width*SpreadSize.height , filex );
		fclose( filex );
	}

	sprintf(valstr, "tempimg//interp_spread_%d.bin", iLocation);

	if (filex = fopen(valstr, "wb"))
	{
		fwrite(pinterpSpreadTable, sizeof(interpSpreadTable), SpreadSize.width*SpreadSize.height, filex);
		fclose(filex);
	}

	delete []pinterpSpreadTable;
}

void CGenerateTable::initUndistortRectifyMapSpread(cv::InputArray K, cv::InputArray D, cv::InputArray R, cv::InputArray P, const cv::Size& size, int m1type, stu_spreadImg*pBuffer, interpSpreadTable* pinterpSpreadTable)
{
	CV_Assert( m1type == CV_16SC2 || m1type == CV_32F || m1type <=0 ); 
	//map1.create( size, m1type <= 0 ? CV_16SC2 : m1type );
	//map2.create( size, map1.type() == CV_16SC2 ? CV_16UC1 : CV_32F );

	CV_Assert((K.depth() == CV_32F || K.depth() == CV_64F) && (D.depth() == CV_32F || D.depth() == CV_64F));
	CV_Assert((P.depth() == CV_32F || P.depth() == CV_64F) && (R.depth() == CV_32F || R.depth() == CV_64F));
	CV_Assert(K.size() == cv::Size(3, 3) && (D.empty() || D.total() == 4));
	CV_Assert(R.empty() || R.size() == cv::Size(3, 3) || R.total() * R.channels() == 3);
	CV_Assert(P.empty() || P.size() == cv::Size(3, 3) || P.size() == cv::Size(4, 3));

	double x, y;
	ushort x1, y1;
	ushort x2, y2;
	

	cv::Vec2d f, c;
	if (K.depth() == CV_32F)
	{
		cv::Matx33f camMat = K.getMat();
		f = cv::Vec2f(camMat(0, 0), camMat(1, 1));
		c = cv::Vec2f(camMat(0, 2), camMat(1, 2));
	}
	else
	{
		cv::Matx33d camMat = K.getMat();
		f = cv::Vec2d(camMat(0, 0), camMat(1, 1));
		c = cv::Vec2d(camMat(0, 2), camMat(1, 2));
	}

	cv::Vec4d k = cv::Vec4d::all(0);
	if (!D.empty())
		k = D.depth() == CV_32F ? (cv::Vec4d)*D.getMat().ptr<cv::Vec4f>() : *D.getMat().ptr<cv::Vec4d>();

	cv::Matx33d RR  = cv::Matx33d::eye();
	if (!R.empty() && R.total() * R.channels() == 3)
	{
		cv::Vec3d rvec;
		R.getMat().convertTo(rvec, CV_64F);
		RR = cv::Affine3d(rvec).rotation();
	}
	else if (!R.empty() && R.size() == cv::Size(3, 3))
		R.getMat().convertTo(RR, CV_64F);

	cv::Matx33d PP = cv::Matx33d::eye();
	if (!P.empty())
		P.getMat().colRange(0, 3).convertTo(PP, CV_64F);

	cv::Matx33d iR = (PP * RR).inv(cv::DECOMP_SVD);

	for( int i = 0; i < size.height; ++i)
	{

		double _x = i*iR(0, 1) + iR(0, 2),
			_y = i*iR(1, 1) + iR(1, 2),
			_w = i*iR(2, 1) + iR(2, 2);

		for( int j = 0; j < size.width; ++j)
		{
			double u = _x/_w, v = _y/_w;

			double r = sqrt(u*u + v*v);
			double theta = atan(r);

			double theta2 = theta*theta, theta4 = theta2*theta2, theta6 = theta4*theta2, theta8 = theta4*theta4;
			double theta_d = theta * (1 + k[0]*theta2 + k[1]*theta4 + k[2]*theta6 + k[3]*theta8);

			double scale = (r == 0) ? 1.0 : theta_d / r;
			double x = f[0]*u*scale + c[0];
			double y = f[1]*v*scale + c[1];

			if( m1type == CV_16SC2 )
			{
				int iu = cv::saturate_cast<int>(x*cv::INTER_TAB_SIZE);
				int iv = cv::saturate_cast<int>(y*cv::INTER_TAB_SIZE);
	//			m1[j*2+0] = (short)(iu >> cv::INTER_BITS);
	//			m1[j*2+1] = (short)(iv >> cv::INTER_BITS);
	//			m2[j] = (ushort)((iv & (cv::INTER_TAB_SIZE-1))*cv::INTER_TAB_SIZE + (iu & (cv::INTER_TAB_SIZE-1)));
			}
			else if( m1type == CV_32FC1 )
			{
		//		m1f[j] = (float)u;
		//		m2f[j] = (float)v;

				pBuffer[i*size.width + j].iWidex = (ushort)x;
				pBuffer[i*size.width + j].iHeighty = (ushort)y;

				x1 = (ushort)x;
				y1 = (ushort)y;
				x2 = (ushort)(x + 1.0);
				y2 = (ushort)(y + 1.0);

				pinterpSpreadTable[i*size.width + j].LTpoint.iWidex = x1;
				pinterpSpreadTable[i*size.width + j].LTpoint.iHeighty = y1;

				pinterpSpreadTable[i*size.width + j].ucLTWeight = 255 * (x2 - x) * (y2 - y) / ((x2 - x1) * (y2 - y1));
				pinterpSpreadTable[i*size.width + j].ucRTWeight = 255 * (x - x1) * (y2 - y) / ((x2 - x1) * (y2 - y1));
				pinterpSpreadTable[i*size.width + j].ucLBWeight = 255 * (x2 - x) * (y - y1) / ((x2 - x1) * (y2 - y1));
				pinterpSpreadTable[i*size.width + j].ucRBWeight = 255 * (x - x1) * (y - y1) / ((x2 - x1) * (y2 - y1));
			}

			_x += iR(0, 0);
			_y += iR(1, 0);
			_w += iR(2, 0);
		}
	}
}

void EncodeStream(float* pData, int dst_width, int dst_height, int src_w, int src_h, unsigned char* pOutData)
{
	unsigned int r = 0;
	unsigned int c = 0;
	unsigned int ind = 0;
	const unsigned int nc = dst_width*dst_height;

	//double power_255 = 255. * 255.;
	double power_255 = 1.;
	double kx = power_255 / (src_w - 1);
	double ky = power_255 / (src_h - 1);
	
	uint16_t *pDstBuf = (uint16_t *)pOutData;

	for (r = 0; r < nc; r++) {
		double x = (pData[2 * r + 0])*kx;
		double y = (pData[2 * r + 1])*ky;

		//unsigned int y = (unsigned int)((double)pData[2 * r + 1] * ky);
		//unsigned int x = (unsigned int)((double)pData[2 * r + 0] * kx);
#if 1
		if (x < 0) 
			x = 0;
		else if (x > 1) 
			x = 1;

		if (y < 0) 
			y = 0;
		else if (y > 1)
			y = 1;

		int k1 = y*255.0;
		int k2 = 65025 * y - 255.0*k1;

		int k3 = x*255.0;
		int k4 = 65025 * x - 255.0*k3;

		//printf("k1>-1 && k1<256 && k2>-1 && k2<256\r\n");
		//printf("k3>-1 && k3<256 && k4>-1 && k4 < 256\r\n");

		pOutData[4 * r] = k3;
		pOutData[4 * r + 1] = k4;
		pOutData[4 * r + 2] = k1;
		pOutData[4 * r + 3] = k2;
#endif
		//pDstBuf[2 * r + 0] = (y / 255) | (y % 255) << 8;
		//pDstBuf[2 * r + 1] = (x / 255) | (x % 255) << 8;
	}
}

void write_width_height(FILE* fp, int w, int h)
{
	uint8_t a1 = (h >> 8) & 0Xff;
	uint8_t a2 = h & 0Xff;
	fwrite(&a1, 1, 1, fp);
	fwrite(&a2, 1, 1, fp);

	a1 = (w >> 8) & 0Xff;
	a2 = w & 0Xff;
	fwrite(&a1, 1, 1, fp);
	fwrite(&a2, 1, 1, fp);
}

void WriteTabl(float* pData, int lut_w, int lut_h, int img_w, int img_h, string filename)
{
	uint8_t* pBuff = new uint8_t[4 * lut_w*lut_h];
	EncodeStream(pData, lut_w, lut_h, img_w, img_h, pBuff);

	FILE* fp = fopen(filename.c_str(), "wb");
	if (!fp) 
		return;

	write_width_height(fp, lut_w, lut_h);
	fwrite((void*)pBuff, lut_w*lut_h * 4, 1, fp);
	fflush(fp);
	fclose(fp);
	delete[] pBuff;
}

void WriteAlphaTabl(uint8_t* pData, int lut_w, int lut_h, string filename)
{
	FILE* fp = fopen(filename.c_str(), "wb");

	if (!fp) 
		return;

	write_width_height(fp, lut_w, lut_h);
	fwrite((void*)pData, lut_w*lut_h * 4, 1, fp);
	fflush(fp);
	fclose(fp);
}


bool convert_tbl(stu2dTable * pLookAtTable)
{
	int w = 720;
	int h = 1280;

	const int nc = w*h;
	stu2dTable* pCurEntry = pLookAtTable;

	for (int i = 0; i < nc; i++) {

		int id1 = pCurEntry[i].img1Idx;
		int id2 = pCurEntry[i].img2Idx;

		if (id1 < 0 && id2 < 0)
			continue;

		switch (id1) {
		case TOP:
			break;

		case BOTTOM:
			pCurEntry[i].img1Location.x += 1280;
			break;

		case LEFT:
			pCurEntry[i].img1Location.y += 720;
			break;

		case RIGHT:
			pCurEntry[i].img1Location.x += 1280;
			pCurEntry[i].img1Location.y += 720;
			break;

		default:
			break;
		}

		switch (id2) {
		case TOP:
			break;
		case BOTTOM:
			pCurEntry[i].img2Location.x += 1280;
			break;
		case LEFT:
			pCurEntry[i].img2Location.y += 720;
			break;
		case RIGHT:
			pCurEntry[i].img2Location.x += 1280;
			pCurEntry[i].img2Location.y += 720;
			break;
		default:
			break;
		}
	}

	uint8_t * pAlpha	= new uint8_t[w*h * 4];
	float* pUv1			= new float[w*h * 2];
	float* pUv2			= new float[w*h * 2];

	for (int r = 0; r < h; r++) {
		uint8_t *pAlphaRow = pAlpha + r*w * 4;
		float   *pUvRow1   = pUv1 + r*w * 2;
		float   *pUvRow2   = pUv2 + r*w * 2;

		for (int c = 0; c < w; c++) {
			int id1 = pCurEntry->img1Idx;
			int id2 = pCurEntry->img2Idx;
			int ii1 = id1;
			int ii2 = id2;

			if (id1 < 0 && id2 < 0) {
				pAlphaRow[4 * c] = 100;
				pAlphaRow[4 * c + 1] = 0;
				pAlphaRow[4 * c + 2] = 100;
				pAlphaRow[4 * c + 3] = 0;

				pUvRow1[2 * c] = 0;
				pUvRow1[2 * c + 1] = 0;

				pUvRow2[2 * c] = 0;
				pUvRow2[2 * c + 1] = 0;
				pCurEntry++;
				continue;
			}

			uint8_t alpha1 = 0;
			if (pCurEntry->img1Weight < 0.0001)
				alpha1 = 0;
			else if (pCurEntry->img1Weight > 0.999)
				alpha1 = 255;
			else
				alpha1 = (uint8_t)(pCurEntry->img1Weight*254.0);

			if (id1 < 0) {
				id1 = 10;
				//printf("alpha1 == 0");
				alpha1 = 0;
			}

			if (id2 < 0) {
				id2 = 10;
				//printf("alpha1 == 255");
				alpha1 = 255;
			}

			uint8_t alpha2 = 255 - alpha1;
			int aa = alpha1;
			aa += alpha2;
			//printf("aa == 255");

			if (alpha1 >= alpha2) {
				pAlphaRow[4 * c] = id1;
				pAlphaRow[4 * c + 1] = alpha1;
				pAlphaRow[4 * c + 2] = id2;
				pAlphaRow[4 * c + 3] = alpha2;

				pUvRow1[2 * c] = pCurEntry->img1Location.x;
				pUvRow1[2 * c + 1] = /*719-*/pCurEntry->img1Location.y;

				pUvRow2[2 * c] = pCurEntry->img2Location.x;
				pUvRow2[2 * c + 1] = /*719-*/pCurEntry->img2Location.y;
			} else {
				pAlphaRow[4 * c] = id2;
				pAlphaRow[4 * c + 1] = alpha2;
				pAlphaRow[4 * c + 2] = id1;
				pAlphaRow[4 * c + 3] = alpha1;

				pUvRow1[2 * c] = pCurEntry->img2Location.x;
				pUvRow1[2 * c + 1] = /*719-*/pCurEntry->img2Location.y;

				pUvRow2[2 * c] = pCurEntry->img1Location.x;
				pUvRow2[2 * c + 1] = /*719-*/pCurEntry->img1Location.y;
			}

			pCurEntry++;
		}
	}

	int img_w = 1280 * 2;
	int img_h = 720 * 2;

	WriteTabl(pUv1, w, h, img_w, img_h, "./appres/top_lut2d.lut");
	WriteTabl(pUv2, w, h, img_w, img_h, "./appres/bot_lut2d.lut");
	WriteAlphaTabl(pAlpha, w, h, "./appres/alpha_lut2d.lut");

	delete[] pUv1;
	delete[] pUv2;
	delete[] pAlpha;

	return true;
}

// 生成二进制查找表
bool CGenerateTable::OnGenerateBinaryTable( stu2dTable* m_p2dTable, float fBeginX , float fBeginY , double pixel_wide , double pixel_height  , int iWidePixel , int iHeightPixel )
{
	bool bReturn = true;

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

	cv::Mat worldLocation;                 // 在世界坐标系上的值
	cv::Mat worldLocationT;                // 世界坐标系的转置

	int i2dBackPointCount = 0;

	//Matx44d RT;
	cv::Matx44d TRT[2];               // 外参值

	// 矩阵的原点
	cv::Mat OriginMat;
	OriginMat.create( 1 , 4 , CV_64F );
	OriginMat.setTo(cv::Scalar::all(0));
	OriginMat.at<double>(0,0) = fBeginX;
	OriginMat.at<double>(0,1) = fBeginY;

	// 内参矩阵的逆
	cv::Mat virtualCamera_innerParam;       // 虚拟摄像机内参矩阵的逆矩阵

	virtualCamera_innerParam.create( 4 , 4 , CV_64F );
	virtualCamera_innerParam.setTo(cv::Scalar::all(0));          // 全部清0

	virtualCamera_innerParam.at<double>( 0 , 0 ) = pixel_wide;
	virtualCamera_innerParam.at<double>( 1 , 1 ) = pixel_height;
	virtualCamera_innerParam.at<double>( 2 , 2 ) = 1;
	virtualCamera_innerParam.at<double>( 3 , 3 ) = 1;

	cv::Mat imgLocation;                  // 在图像坐标系上的值

	imgLocation.create( 1 , 4 , CV_64F );
	imgLocation.at<double>(0 , 2) = 0;
	imgLocation.at<double>(0 , 3) = (double)1;

	stu2dTable* m_p2dBackTable = new stu2dTable[iWidePixel*iHeightPixel];

	//stu2dTable* m_p2dTable= new stu2dTable[iWidePixel*iHeightPixel];
	int iTemp = 0;
	cv::Point pTemp;
	double dTemp;

	//////////////////////////////////////////////////////////////////////////
	// 计算距离
	float fDistance[4];      // 四条融合线的的距离，0为左上角，1为右上角，2为左下角，3为右下角
	for ( int i = 0 ; i<4 ; i++ )
	{
		dTemp = abs( m_BMix[2*i+1] - m_BMix[2*i] );
		fDistance[i] = sqrt( m_K[i]*m_K[i] + 1 );
		fDistance[i] = dTemp/fDistance[i];
	}




	for (int i = 0 ; i<iHeightPixel ; i++ )
	{
		for (int j = 0 ; j<iWidePixel ; j++ )
		{
			// i和j都是图像上的像素坐标，所以在这里要将其转换成世界坐标系
			imgLocation.at<double>(0 , 0) = (double)j;
			imgLocation.at<double>(0 , 1) = (double)i;
			//y_top_left = m_K[0]*j + m_B[0];
			//y_top_right = m_K[1]*j + m_B[1];
			y_top_left = m_K[0]*j + m_B[0];
			yTop_topLeft = m_K[0]*j + m_BMix[0];
			yBottom_topLeft = m_K[0]*j + m_BMix[1];

			y_top_right = m_K[1]*j + m_B[1];
			yTop_topRight = m_K[1]*j + m_BMix[2];
			yBottom_topRight = m_K[1]*j + m_BMix[3];

			y_bottom_left = m_K[2]*j + m_B[2];
			yTop_bottomleft = m_K[2]*j + m_BMix[4];
			yBottom_bottomLeft = m_K[2]*j + m_BMix[5];

			y_bottom_right = m_K[3]*j + m_B[3];
			yTop_bottomright = m_K[3]*j + m_BMix[6];
			yBottom_bottomright = m_K[3]*j + m_BMix[7];

			worldLocation = imgLocation*virtualCamera_innerParam;

			worldLocation = OriginMat + worldLocation;

			worldLocationT = worldLocation.t();            // 转置
			//iTemp = i*dst.cols + j;
			//pTemp.x = j;
			//pTemp.y = i;
			//m_p2dTable[iTemp].iLocation = pTemp;

			////////////////////////////////////////////////////////////////////////////
			////
			//m_p2dTable[iTemp].img1Idx = -1;
			//m_p2dTable[iTemp].img2Idx = -1;
			//////////////////////////////////////////////////////////////////////////
			iTemp = i*iWidePixel + j;
			pTemp.x = j;
			pTemp.y = i;
			m_p2dTable[iTemp].iLocation = pTemp;

			//////////////////////////////////////////////////////////////////////////
			//
			m_p2dTable[iTemp].img1Idx = -1;
			m_p2dTable[iTemp].img2Idx = -1;
			//////////////////////////////////////////////////////////////////////////

			if ( (i<y_top_left)&&(i<y_top_right)&&(i<m_height_area[0]) )          // 上区域
			{
#if 1
				m_p2dTable[iTemp].img1Idx = TOP;
				if ( i>yTop_topLeft )                       // 说明上区域在左上角的重合区域
				{
					m_p2dTable[iTemp].img2Idx = LEFT;
					TRT[1] = m_pCalibrate[LEFT].OnGetRTParam( 0 );

					//////////////////////////////////////////////////////////////////////////
					// 求当前i,j点到拼接缝1的距离
					dTemp = abs( (m_K[0]*j - i + m_B[0] )/sqrt(m_K[0]*m_K[0] + 1) );
					m_p2dTable[iTemp].img1Weight = dTemp/fDistance[0];              // 归一化
					m_p2dTable[iTemp].img1Weight = m_p2dTable[iTemp].img1Weight + 0.5;
					m_p2dTable[iTemp].img2Weight = 1 - m_p2dTable[iTemp].img1Weight;
				}
				if ( (i<=yTop_topLeft)&&(i<=yTop_topRight) )   // 没有在重合区域
				{
					m_p2dTable[iTemp].img1Weight = 1;          // 上边的图像为权重为1
					m_p2dTable[iTemp].img2Weight = 0;
				}
				if ( i>yTop_topRight )                      // 说明上区域在右上角的重合区域
				{
					m_p2dTable[iTemp].img2Idx = RIGHT;
					TRT[1] = m_pCalibrate[RIGHT].OnGetRTParam( 0 );

					//////////////////////////////////////////////////////////////////////////
					// 求当前i,j到拼接缝2的距离
					dTemp = abs( (m_K[1]*j - i + m_B[1] )/sqrt(m_K[1]*m_K[1] + 1) );
					m_p2dTable[iTemp].img1Weight = dTemp/fDistance[1];              // 归一化
					m_p2dTable[iTemp].img1Weight = m_p2dTable[iTemp].img1Weight + 0.5;
					m_p2dTable[iTemp].img2Weight = 1 - m_p2dTable[iTemp].img1Weight;
				}

				GetRTvalue(TRT[0] , j , m_wide_area , m_pCalibrate[TOP].OnGetFishEyeInf() , m_RT_DvalueAverage[TOP] );
				mappingFromFisheye( worldLocationT , TRT , m_p2dTable[iTemp] , m_iSourceWide/*sourceMat[TOP].cols*/ , m_iSourceHeight/*sourceMat[TOP].rows*/ );

#endif

#if 0
				m_p2dTable[iTemp].img1Idx = TOP;
				m_p2dTable[iTemp].img1Weight = 1;
				m_p2dTable[iTemp].img2Idx = -1;
				m_p2dTable[iTemp].img2Weight = 0;

				GetRTvalue(TRT[0] , j , m_wide_area , m_pCalibrate[TOP].OnGetFishEyeInf() , m_RT_DvalueAverage[TOP] );

//				TRT[1] = m_fishEyeInf[BOTTOM].RT[0];
				mappingFromFisheye( worldLocationT , TRT , m_p2dTable[iTemp] , m_iSourceWide , m_iSourceHeight );
#endif
			}
			//////////////////////////////////////////////////////////////////////////
			//////////////////////////////////////////////////////////////////////////
			if ( (i>= y_top_left)&&(i<y_bottom_left)&&(j<m_wide_area[0]) )     // 左区域
			{
#if 1
				m_p2dTable[iTemp].img1Idx = LEFT;
				if ( i<yBottom_topLeft )                       // 说明上区域在左上角的重合区域
				{
					m_p2dTable[iTemp].img2Idx = TOP;
					TRT[1] = m_pCalibrate[TOP].OnGetRTParam( 0 );

					//////////////////////////////////////////////////////////////////////////
					// 求当前i,j点到拼接缝1的距离
					dTemp = abs( (m_K[0]*j - i + m_B[0] )/sqrt(m_K[0]*m_K[0] + 1) );
					m_p2dTable[iTemp].img1Weight = dTemp/fDistance[0];              // 归一化
					m_p2dTable[iTemp].img1Weight = m_p2dTable[iTemp].img1Weight + 0.5;
					m_p2dTable[iTemp].img2Weight = 1 - m_p2dTable[iTemp].img1Weight;
				}
				if ( (i>=yBottom_topLeft)&&(i<=yTop_bottomleft) )   // 没有在重合区域
				{
					m_p2dTable[iTemp].img1Weight = 1;          // 上边的图像为权重为1
					m_p2dTable[iTemp].img2Weight = 0;
				}
				if ( i>yTop_bottomleft )                      // 说明上区域在右上角的重合区域
				{
					m_p2dTable[iTemp].img2Idx = BOTTOM;
					TRT[1] = m_pCalibrate[BOTTOM].OnGetRTParam( 0 );

					//////////////////////////////////////////////////////////////////////////
					// 求当前i,j到拼接缝3的距离
					dTemp = abs( (m_K[2]*j - i + m_B[2] )/sqrt(m_K[2]*m_K[2] + 1) );
					m_p2dTable[iTemp].img1Weight = dTemp/fDistance[2];              // 归一化
					m_p2dTable[iTemp].img1Weight = m_p2dTable[iTemp].img1Weight + 0.5;
					m_p2dTable[iTemp].img2Weight = 1 - m_p2dTable[iTemp].img1Weight;
				}

				GetRTvalue(TRT[0] , i , m_height_area , m_pCalibrate[LEFT].OnGetFishEyeInf() , m_RT_DvalueAverage[LEFT] );
				mappingFromFisheye( worldLocationT , TRT , m_p2dTable[iTemp] , m_iSourceWide/*sourceMat[LEFT].cols*/ , m_iSourceHeight/*sourceMat[LEFT].rows*/ );

#endif
#if 0
				m_p2dTable[iTemp].img1Idx = LEFT;
				m_p2dTable[iTemp].img1Weight = 1;
				m_p2dTable[iTemp].img2Idx = -1;
				m_p2dTable[iTemp].img2Weight = 0;
			//	GetRTvalue(TRT[0] , i , height_area , m_fishEyeInf[LEFT] , RT_DvalueAverage[LEFT] );
				GetRTvalue(TRT[0] , i , m_height_area , m_pCalibrate[LEFT].OnGetFishEyeInf() , m_RT_DvalueAverage[LEFT] );

//				TRT[1] = m_fishEyeInf[BOTTOM].RT[0];
				mappingFromFisheye( worldLocationT , TRT , m_p2dTable[iTemp], m_iSourceWide , m_iSourceHeight);
#endif
			}

			//////////////////////////////////////////////////////////////////////////
			if ( (i>=y_top_right)&&(i<y_bottom_right)&&(j>(m_wide_area[0]+m_wide_area[1])) )     // 右区域
			{
#if 1
				m_p2dTable[iTemp].img1Idx = RIGHT;
				if ( i<yBottom_topRight )                       // 说明上区域在左上角的重合区域
				{
					m_p2dTable[iTemp].img2Idx = TOP;
					TRT[1] = m_pCalibrate[TOP].OnGetRTParam( 1 );

					//////////////////////////////////////////////////////////////////////////
					// 求当前i,j点到拼接缝2的距离
					dTemp = abs( (m_K[1]*j - i + m_B[1] )/sqrt(m_K[1]*m_K[1] + 1) );
					m_p2dTable[iTemp].img1Weight = dTemp/fDistance[1];              // 归一化
					m_p2dTable[iTemp].img1Weight = m_p2dTable[iTemp].img1Weight + 0.5;
					m_p2dTable[iTemp].img2Weight = 1 - m_p2dTable[iTemp].img1Weight;
				}
				if ( (i>=yBottom_topRight )&&( i<=yTop_bottomright ) )   // 没有在重合区域
				{
					m_p2dTable[iTemp].img1Weight = 1;          // 上边的图像为权重为1
					m_p2dTable[iTemp].img2Weight = 0;
				}
				if ( i>yTop_bottomright )                      // 说明上区域在右上角的重合区域
				{
					m_p2dTable[iTemp].img2Idx = BOTTOM;
					TRT[1] = m_pCalibrate[BOTTOM].OnGetRTParam( 1 );

					//////////////////////////////////////////////////////////////////////////
					// 求当前i,j到拼接缝4的距离
					dTemp = abs( (m_K[3]*j - i + m_B[3] )/sqrt(m_K[3]*m_K[3] + 1) );
					m_p2dTable[iTemp].img1Weight = dTemp/fDistance[3];              // 归一化
					m_p2dTable[iTemp].img1Weight = m_p2dTable[iTemp].img1Weight + 0.5;
					m_p2dTable[iTemp].img2Weight = 1 - m_p2dTable[iTemp].img1Weight;
				}

				GetRTvalue(TRT[0] , i , m_height_area , m_pCalibrate[RIGHT].OnGetFishEyeInf() , m_RT_DvalueAverage[RIGHT] );
				mappingFromFisheye( worldLocationT , TRT , m_p2dTable[iTemp] , m_iSourceWide/*sourceMat[RIGHT].cols*/ , m_iSourceHeight/*sourceMat[RIGHT].rows*/ );

#endif

#if 0
				m_p2dTable[iTemp].img1Idx = RIGHT;
				m_p2dTable[iTemp].img1Weight = 1;
				m_p2dTable[iTemp].img2Idx = -1;
				m_p2dTable[iTemp].img2Weight = 0;
//				GetRTvalue(TRT[0] , i , height_area , m_fishEyeInf[RIGHT] , RT_DvalueAverage[RIGHT] );
				GetRTvalue(TRT[0] , i , m_height_area , m_pCalibrate[RIGHT].OnGetFishEyeInf() , m_RT_DvalueAverage[RIGHT] );

//				TRT[1] = m_fishEyeInf[BOTTOM].RT[0];
				mappingFromFisheye( worldLocationT , TRT , m_p2dTable[iTemp] , m_iSourceWide , m_iSourceHeight);
#endif
			}

			//////////////////////////////////////////////////////////////////////////
			if ( (i>=y_bottom_left)&&(i>=y_bottom_right)&&(i>(m_height_area[0]+m_height_area[1])) )   // 下区域
			{
#if 1
				m_p2dTable[iTemp].img1Idx = BOTTOM;
				if ( i<yBottom_bottomLeft )                       // 说明上区域在左上角的重合区域
				{
					m_p2dTable[iTemp].img2Idx = LEFT;
					TRT[1] = m_pCalibrate[LEFT].OnGetRTParam( 1 );//m_fishEyeInf[LEFT].RT[1];

					//////////////////////////////////////////////////////////////////////////
					// 求当前i,j点到拼接缝2的距离
					dTemp = abs( (m_K[2]*j - i + m_B[2] )/sqrt(m_K[2]*m_K[2] + 1) );
					m_p2dTable[iTemp].img1Weight = dTemp/fDistance[2];              // 归一化
					m_p2dTable[iTemp].img1Weight = m_p2dTable[iTemp].img1Weight + 0.5;
					m_p2dTable[iTemp].img2Weight = 1 - m_p2dTable[iTemp].img1Weight;

					//m_p2dTable[iTemp].img1Weight = 0;
					//m_p2dTable[iTemp].img2Weight = 1;
				}
				if ( (i>=yBottom_bottomLeft )&&( i>=yBottom_bottomright ) )   // 没有在重合区域
				{
					m_p2dTable[iTemp].img1Weight = 1;          // 上边的图像为权重为1
					m_p2dTable[iTemp].img2Weight = 0;
				}
				if ( i<yBottom_bottomright )                      // 说明上区域在右上角的重合区域
				{
					m_p2dTable[iTemp].img2Idx = RIGHT;
					TRT[1] = m_pCalibrate[RIGHT].OnGetRTParam( 1 );//m_fishEyeInf[RIGHT].RT[1];

					//////////////////////////////////////////////////////////////////////////
					// 求当前i,j到拼接缝4的距离
					dTemp = abs( (m_K[3]*j - i + m_B[3] )/sqrt(m_K[3]*m_K[3] + 1) );
					m_p2dTable[iTemp].img1Weight = dTemp/fDistance[3];              // 归一化
					m_p2dTable[iTemp].img1Weight = m_p2dTable[iTemp].img1Weight + 0.5;
					m_p2dTable[iTemp].img2Weight = 1 - m_p2dTable[iTemp].img1Weight;

					//m_p2dTable[iTemp].img1Weight = 0;
					//m_p2dTable[iTemp].img2Weight = 1;
				}

				GetRTvalue(TRT[0] , j , m_wide_area , m_pCalibrate[BOTTOM].OnGetFishEyeInf()/*m_fishEyeInf[BOTTOM]*/ , m_RT_DvalueAverage[BOTTOM] );
				mappingFromFisheye( worldLocationT , TRT , m_p2dTable[iTemp] , m_iSourceWide/*sourceMat[BOTTOM].cols*/ , m_iSourceHeight/*sourceMat[BOTTOM].rows*/ );

#endif

#if 0
				m_p2dTable[iTemp].img1Idx = BOTTOM;
				m_p2dTable[iTemp].img1Weight = 1;
				m_p2dTable[iTemp].img2Idx = -1;
				m_p2dTable[iTemp].img2Weight = 0;
//				GetRTvalue(TRT[0] , j , wide_area , m_fishEyeInf[BOTTOM] , RT_DvalueAverage[BOTTOM] );
				GetRTvalue(TRT[0] , j , m_wide_area , m_pCalibrate[BOTTOM].OnGetFishEyeInf() , m_RT_DvalueAverage[BOTTOM] );

//				TRT[1] = m_fishEyeInf[BOTTOM].RT[0];
				mappingFromFisheye( worldLocationT , TRT , m_p2dTable[iTemp] , m_iSourceWide , m_iSourceHeight);
#endif
			}


		}
	}
    cout<<__LINE__<<"---------------BEFORE writing old-format table-----------\n";

#if 0

	//Mat dstMat;
	//dstMat.create( DstImageHeight , DstImageWide , CV_8UC3 );//, Scalar(0,0,0) );
	//dstMat.setTo( Scalar(0,0,0) );
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
	int    iImgWide = sourceMat[0].cols;
	int    iImgHeight = sourceMat[0].rows;

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
			if ( m_p2dTable[indexx].img1Idx != -1 )
			{
				// 双线性插值
				// 找到它的四个点

				//////////////////////////////////////////////////////////////////////////
				// 第一幅图像
				x1 = (int)m_p2dTable[indexx].img1Location.x;
				y1 = (int)m_p2dTable[indexx].img1Location.y;
				minus_x = m_p2dTable[indexx].img1Location.x - x1;
				minus_y = m_p2dTable[indexx].img1Location.y - y1;

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
				if ( m_p2dTable[indexx].img2Idx != -1 )
				{
					xx1 = (int)m_p2dTable[indexx].img2Location.x;
					yy1 = (int)m_p2dTable[indexx].img2Location.y;
					minus_x1 = m_p2dTable[indexx].img2Location.x - xx1;
					minus_y1 = m_p2dTable[indexx].img2Location.y - yy1;

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
					dTempx1 = sourceMat[m_p2dTable[indexx].img1Idx].at<Vec3b>(y1 , x1 )[z];
					dTempx1 = dTempx1*(1-minus_x);
					dTempx1 = dTempx1*(1-minus_y);

					dTempx2 = sourceMat[m_p2dTable[indexx].img1Idx].at<Vec3b>(y1 , x2 )[z];
					dTempx2 = dTempx2*(1-minus_x);
					dTempx2 = dTempx2*minus_y;

					dTempx3 = sourceMat[m_p2dTable[indexx].img1Idx].at<Vec3b>(y2 , x1 )[z];
					dTempx3 = dTempx3*minus_x;
					dTempx3 = dTempx3*(1-minus_y);

					dTempx4 = sourceMat[m_p2dTable[indexx].img1Idx].at<Vec3b>(y2 , x2 )[z];
					dTempx4 = dTempx4*minus_x;
					dTempx4 = dTempx4*minus_y;

					dTempx1 = dTempx1 + dTempx2;
					dTempx1 = dTempx1 + dTempx3;
					dTempx1 = dTempx1 + dTempx4;
					dTempx1 = dTempx1*m_p2dTable[indexx].img1Weight;

					dTempx11 = 0;

					// 如果第二幅图像存在
					if ( m_p2dTable[indexx].img2Idx != -1 )
					{
						dTempx11 = sourceMat[m_p2dTable[indexx].img2Idx].at<Vec3b>(yy1 , xx1 )[z];
						dTempx11 = dTempx11*(1-minus_x1);
						dTempx11 = dTempx11*(1-minus_y1);

						dTempx22 = sourceMat[m_p2dTable[indexx].img2Idx].at<Vec3b>(yy1 , xx2 )[z];
						dTempx22 = dTempx22*(1-minus_x1);
						dTempx22 = dTempx22*minus_y1;

						dTempx33 = sourceMat[m_p2dTable[indexx].img2Idx].at<Vec3b>(yy2 , xx1 )[z];
						dTempx33 = dTempx33*minus_x1;
						dTempx33 = dTempx33*(1-minus_y1);

						dTempx44 = sourceMat[m_p2dTable[indexx].img2Idx].at<Vec3b>(yy2 , xx2 )[z];
						dTempx44 = dTempx44*minus_x1;
						dTempx44 = dTempx44*minus_y1;

						dTempx11 = dTempx11 + dTempx22;
						dTempx11 = dTempx11 + dTempx33;
						dTempx11 = dTempx11 + dTempx44;

						dTempx11 = dTempx11*m_p2dTable[indexx].img2Weight;
					}

					cTemp = (unsigned char)(dTempx1+dTempx11);

					dst.at<Vec3b>(i,j)[z] = cTemp;
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

//	imshow( "data" , dst );
//	imwrite( "dstIma.bmp" , dst );
//	waitKey( 0 );
#endif

	FILE*file = NULL;

	if ( file = fopen( "tempimg//Table.bin" , "wb" ) )
	{
		fwrite( m_p2dTable , sizeof(stu2dTable) , iWidePixel*iHeightPixel , file );
		fclose( file );
 	}
    cout<<__LINE__<<"---------------AFTER writing old-format table-----------\n";

	//delete[]m_p2dTable;
	//m_p2dTable = NULL;

	return bReturn;
}

void CGenerateTable::mappingFromFisheye(cv::Mat&WorldLocation, cv::Matx44d*RT, stu2dTable& tableInf, int isrcWide, int isrcHeight)
{
	cv::Mat camera_c;         // 摄像机坐标系
	//Mat RTt(RT[0]);          // 图像1的外参
	cv::Mat cameraLocation;      // 摄像机坐标系的矩阵
	//Mat A1(m_fishEyeInf[tableInf.img1Idx].innerParam);
	cv::Mat A1(m_pCalibrate[tableInf.img1Idx].OnGetFishEyeInf().innerParam);


	int iTemp = 0;
	if ( tableInf.img2Idx == -1 )
	{
		iTemp = tableInf.img1Idx;
	}
	else
	{
		iTemp = tableInf.img2Idx;
	}

	cv::Mat A2(m_pCalibrate[iTemp].OnGetFishEyeInf().innerParam);

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

	//	int i;         // 高
	//	int j;         // 宽

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
#if 1
			pDtemp[0] = pDtemp[0]/pDtemp[2];
			pDtemp[1] = pDtemp[1]/pDtemp[2];
#endif
			//double m1 = sqrt(pDtemp[0] * pDtemp[0] + pDtemp[1]*pDtemp[1]+pDtemp[2]*pDtemp[2]);

			//pDtemp[0] = pDtemp[0] / m1;
			//pDtemp[1] = pDtemp[1] / m1;
			//pDtemp[2] = pDtemp[2] / m1;

			//pDtemp[0] = pDtemp[0] / pDtemp[2];
			//pDtemp[1] = pDtemp[1] / pDtemp[2];

			dTemp = pDtemp[0]*pDtemp[0];
			dTemp = dTemp + pDtemp[1]*pDtemp[1];
			r = sqrt( dTemp );
			thrta = atan( r );

			thrta2 = thrta*thrta;
			thrta4 = thrta2*thrta2;
			thrta6 = thrta4*thrta2;
			thrta8 = thrta4*thrta4;

			//dTemp = 0;

			dTemp = thrta2*m_pCalibrate[iTemp].OnGetFishEyeInf().distortionParam.val[0];
			dTemp = dTemp + 1;

			dTemp1 = thrta4*m_pCalibrate[iTemp].OnGetFishEyeInf().distortionParam.val[1];
			dTemp = dTemp + dTemp1;

			dTemp1 = thrta6*m_pCalibrate[iTemp].OnGetFishEyeInf().distortionParam.val[2];
			dTemp = dTemp + dTemp1;

			dTemp1 = thrta8*m_pCalibrate[iTemp].OnGetFishEyeInf().distortionParam.val[3];
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

void CGenerateTable::GetRTvalue(cv::Matx44d&ReturnRT, int NowIndex, int*pIArrea, const fishEyeInf&fishEye, cv::Matx44d&DValueRT)
{
	int i;
	int j;
	int iTemp;
	int iTemp2;
	iTemp2 = pIArrea[0] + pIArrea[1];
	if ( NowIndex < pIArrea[0] )
	{
		ReturnRT = fishEye.RT[0];
	}
	if ( (NowIndex>=pIArrea[0])&&(NowIndex<iTemp2) )
	{
		for ( i = 0 ; i<DValueRT.rows ; i++ )
		{
			for ( j = 0 ; j<DValueRT.cols ; j++ )
			{
				iTemp = NowIndex - pIArrea[0];
				ReturnRT(i,j) = iTemp*DValueRT(i,j);
				ReturnRT(i,j) = ReturnRT(i,j) + fishEye.RT[0](i,j);
			}
		}
	}
	if ( NowIndex>= iTemp2 )
	{
		ReturnRT = fishEye.RT[1];
	}
}


void CGenerateTable::OnSetSpliceJoint( CvSize2D32f worldShowSize , float fBeginOriginX , float fBeginOriginY , CvSize2D32f worldCarSize , float fWorldCarBeginX , float fWorldCarBeginY , double PixelPerWide , double PixelPerHeight )
{
	//////////////////////////////////////////////////////////////////////////
	// 设计拼接缝
	// 左上角拼接缝的三条拼接线在世界坐标上的值
	float fTemp1;
	float fTemp2;

	int iTempxx = -2000;
	//int iChonghe = 800;

	int bRetrun = 0;
	const char*file = "config.ini";          // 配置文件名和路径

	char* sect;
	char* key;
	bRetrun = iniFileLoad(file);

	if (!bRetrun)
	{
		cout << "config.ini load error!" << endl;
		return;
	}

	sect = "fuse";
	key = "Fusion_Area_Size";
	int iChonghe = iniGetInt(sect, key, 20);

	iniFileFree();

	// 左上角
	CvSize fBeginPoint_topLeft;
	fBeginPoint_topLeft.width  = fWorldCarBeginX;          // x
	fBeginPoint_topLeft.height = fWorldCarBeginY;        // y

	CvSize fEndPoint_topLeft;
	fEndPoint_topLeft.width    = fBeginOriginX + iTempxx;
	fEndPoint_topLeft.height   = fBeginOriginY;

	fTemp1 = (float)(fEndPoint_topLeft.height - fBeginPoint_topLeft.height)/PixelPerHeight;
	fTemp2 = (float)(fEndPoint_topLeft.width - fBeginPoint_topLeft.width )/PixelPerWide;
	m_K[0] = fTemp1/fTemp2;
	m_B[0] = fEndPoint_topLeft.height/PixelPerHeight - m_K[0]*fEndPoint_topLeft.width/PixelPerWide;

#if 1
	// 左上角融合区域
	// 融合区域融合线计算
	CvSize fPointBottom_topLeft;            // 在拼接线的下方
	fPointBottom_topLeft.width = fEndPoint_topLeft.width - iChonghe;//(m_isquareToGap-1)*CHECK_BOARD_PATTERN_LEN + m_ixOffset;
	fPointBottom_topLeft.height = fEndPoint_topLeft.height;
	CvSize fPointTop_topLeft;               // 在拼接线的上方
	fPointTop_topLeft.width = fEndPoint_topLeft.width + iChonghe;
	fPointTop_topLeft.height = fEndPoint_topLeft.height;

	m_BMix[0] = fPointTop_topLeft.height/PixelPerHeight - m_K[0]*fPointTop_topLeft.width/PixelPerWide;
	m_BMix[1] = fPointBottom_topLeft.height/PixelPerHeight - m_K[0]*fPointBottom_topLeft.width/PixelPerWide;
#endif

	//////////////////////////////////////////////////////////////////////////
	// 右上角拼接缝在世界坐标系上值
	CvSize fBeginPoint_topRight;
	fBeginPoint_topRight.width = fWorldCarBeginX + worldCarSize.width;         // x
	fBeginPoint_topRight.height = fWorldCarBeginY;                             // y
	CvSize fEndPoint_topRight;
	fEndPoint_topRight.width = fBeginOriginX+worldShowSize.width - iTempxx;              // x
	fEndPoint_topRight.height = fBeginOriginY;                                 // y

	fTemp1 = (float)(fEndPoint_topRight.height - fBeginPoint_topRight.height)/PixelPerHeight;
	fTemp2 = (float)(fEndPoint_topRight.width - fBeginPoint_topRight.width )/PixelPerWide;
	m_K[1] = fTemp1/fTemp2;
	m_B[1] = fEndPoint_topRight.height/PixelPerHeight - m_K[1]*fEndPoint_topRight.width/PixelPerWide;

#if 1
	// 融合区域融合线
	CvSize fPointBottom_topRight;        // 拼接线的下方
	fPointBottom_topRight.width = fBeginOriginX + worldShowSize.width - iTempxx + iChonghe;
	fPointBottom_topRight.height = fEndPoint_topRight.height;
	CvSize fPointTop_topRight;           // 拼接线的上方
	fPointTop_topRight.width = fBeginOriginX + worldShowSize.width - iTempxx - iChonghe;
	fPointTop_topRight.height = fEndPoint_topRight.height;

	m_BMix[2] = fPointTop_topRight.height/PixelPerHeight - m_K[1]*fPointTop_topRight.width/PixelPerWide;
	m_BMix[3] = fPointBottom_topRight.height/PixelPerHeight - m_K[1]*fPointBottom_topRight.width/PixelPerWide;
#endif
	//////////////////////////////////////////////////////////////////////////

	// 左下角拼接缝在世界坐标系上的值
	CvSize fBeginPoint_bottomLeft;
	fBeginPoint_bottomLeft.width = fBeginPoint_topLeft.width;
	fBeginPoint_bottomLeft.height = fWorldCarBeginY + worldCarSize.height;

	CvSize fEndPoint_BottomLeft;
	fEndPoint_BottomLeft.width = fEndPoint_topLeft.width + iTempxx;
	fEndPoint_BottomLeft.height = fBeginOriginY + worldShowSize.height;

	fTemp1 = (float)(fEndPoint_BottomLeft.height - fBeginPoint_bottomLeft.height)/PixelPerHeight;
	fTemp2 = (float)(fEndPoint_BottomLeft.width - fBeginPoint_bottomLeft.width )/PixelPerWide;
	m_K[2] = fTemp1/fTemp2;
	m_B[2] = fEndPoint_BottomLeft.height/PixelPerHeight - m_K[2]*fEndPoint_BottomLeft.width/PixelPerWide;

#if 1
	// 融合区域拼接线
	CvSize fPointBottom_bottomLeft;           // 拼接线下方
	fPointBottom_bottomLeft.width = fPointTop_topLeft.width + iTempxx;
	fPointBottom_bottomLeft.height = fEndPoint_BottomLeft.height;
	CvSize fPointTop_bottomLeft;              // 拼接线的上方
	fPointTop_bottomLeft.width = fPointBottom_topLeft.width + iTempxx;
	fPointTop_bottomLeft.height = fPointBottom_bottomLeft.height;

	m_BMix[4] = fPointTop_bottomLeft.height/PixelPerHeight - m_K[2]*fPointTop_bottomLeft.width/PixelPerWide;
	m_BMix[5] = fPointBottom_bottomLeft.height/PixelPerHeight - m_K[2]*fPointBottom_bottomLeft.width/PixelPerWide;
#endif


	// 右下角拼接缝在世界坐标系上的值
	CvSize fBeginPoint_bottomRight;
	fBeginPoint_bottomRight.width = fBeginPoint_topRight.width;
	fBeginPoint_bottomRight.height = fBeginPoint_bottomLeft.height;

	CvSize fEndPoint_bottomRight;
	fEndPoint_bottomRight.width = fEndPoint_topRight.width - iTempxx;
	fEndPoint_bottomRight.height = fEndPoint_BottomLeft.height;

	fTemp1 = (float)(fEndPoint_bottomRight.height - fBeginPoint_bottomRight.height)/PixelPerHeight;
	fTemp2 = (float)(fEndPoint_bottomRight.width - fBeginPoint_bottomRight.width )/PixelPerWide;
	m_K[3] = fTemp1/fTemp2;
	m_B[3] = fEndPoint_bottomRight.height/PixelPerHeight - m_K[3]*fEndPoint_bottomRight.width/PixelPerWide;

#if 1
	// 融合区域拼接线
	CvSize fPointBottom_bottomRight;        // 拼接线下方
	fPointBottom_bottomRight.width = fPointTop_topRight.width - iTempxx;
	fPointBottom_bottomRight.height = fEndPoint_bottomRight.height;

	CvSize fPointTop_bottomRight;           // 拼接线上方
	fPointTop_bottomRight.width = fPointBottom_topRight.width - iTempxx;
	fPointTop_bottomRight.height = fEndPoint_bottomRight.height;

	m_BMix[6] = fPointTop_bottomRight.height/PixelPerHeight - m_K[3]*fPointTop_bottomRight.width/PixelPerWide;
	m_BMix[7] = fPointBottom_bottomRight.height/PixelPerHeight - m_K[3]*fPointBottom_bottomRight.width/PixelPerWide;
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
	iTemp1 = fWorldCarBeginX ;// - fBeginOriginX;

	m_wide_area[0] = (int)(iTemp1/PixelPerWide);          // 第一个区域
//	iTemp1 = worldCarSize.width;
	m_wide_area[1] = (int)(worldCarSize.width/PixelPerWide);

	m_wide_area[2] = worldShowSize.width - fWorldCarBeginX;// + fBeginOriginX ;
	m_wide_area[2] = (m_wide_area[2] - worldCarSize.width)/PixelPerWide;

	// y方向上分区
	iTemp1 = fWorldCarBeginY ; // - fBeginOriginY;
	m_height_area[0] = (int)(iTemp1/PixelPerHeight);

	//iTemp1 = worldCarSize.height ;
	m_height_area[1] = (int)(worldCarSize.height/PixelPerHeight);

	m_height_area[2] = worldShowSize.height- fWorldCarBeginY ; // + fBeginOriginY ;
	m_height_area[2] = (m_height_area[2] - worldCarSize.height)/PixelPerHeight;
//	heightLen[2] = heightLen[2] - heightLen[1];

	cv::Matx44d p1;
	cv::Matx44d p2;
	int iCount = 0;


	for ( iTemp1 = 0 ; iTemp1<m_iCalibrateNum ; iTemp1++ )
	{
		if ( iTemp1 <=1 )            // 使用wideLen[1]
		{
			iTemp2 = m_wide_area[1];
		}
		else                         // 使用heightLen[1]
		{
			iTemp2 = m_height_area[1];
		}

		for ( i = 0 ; i<m_RT_DvalueAverage[iTemp1].rows ; i++ )
		{
			for ( j = 0 ; j<m_RT_DvalueAverage[iTemp1].cols ; j++ )
			{
				p1 = m_pCalibrate[iTemp1].OnGetRTParam( 0 );
				p2 = m_pCalibrate[iTemp1].OnGetRTParam( 1 );
				m_RT_DvalueAverage[iTemp1](i,j) = p2(i,j) - p1(i,j);
				m_RT_DvalueAverage[iTemp1](i,j) = m_RT_DvalueAverage[iTemp1](i,j)/(double)iTemp2;
				//m_pCalibrate[iTemp1](i,j) = 

			//	Dvalue_average[iTemp1]( i , j ) = m_fishEyeInf[iTemp1].RT[1](i,j) - m_fishEyeInf[iTemp1].RT[0](i,j);
			//	Dvalue_average[iTemp1]( i , j) = Dvalue_average[iTemp1](i,j)/(double)iTemp2;
			}
		}
	}

}

CGenerateTable::~CGenerateTable()
{

}