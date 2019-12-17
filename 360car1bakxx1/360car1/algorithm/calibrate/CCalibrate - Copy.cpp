#include "CCalibrate.h"

CCalibrate::CCalibrate()
{
	//////////////////////////////////////////////////////////////////////////
	// 在这里可以读配置文件里面的参数进行初使化
	m_Points_World.clear();
	//m_Points_World1.clear();
}

// 如果bClear为true的话，那么就说明是第一个棋盘
// 如果为false的话，那么就说明是第二个棋盘
void CCalibrate::OnSetCornerWorldLocation( int iBeginX , int iBeginY , CvSize dimension , int iCheckBoardSize , int iCheckBoardSize_height , int iLocation , bool bClear )
{
	Point3f pLocation;
	vector<Point3f> pTempx;

	m_iLocation = iLocation;        // 存储当前摄像头的位置
	m_cornerDimension = dimension;
	if ( bClear )                    // 第一个棋盘
	{
		m_Points_World.clear();         // 清0

	}
	for ( int i = 0 ; i < dimension.height ; i++ )
	{
		for ( int j = 0 ; j<dimension.width ; j++ )
		{
			pLocation.x = iBeginX + j*iCheckBoardSize;
			pLocation.y = iBeginY + i*iCheckBoardSize_height;
			pLocation.z = 0;

			pTempx.push_back( pLocation );

		}
	}

	m_Points_World.push_back( pTempx );
	


}

// 进行重排
void CCalibrate::OnReSort_Square_Img( vector<vector<Point2f> >&pTemp , int iSindex )
{
	vector<Point2f> temp1 = pTemp.at(0);
	vector<Point2f> temp2 = pTemp.at(1);
	vector<Point2f> temp;
	int i;
	int j;
	Point2f pPoint;
	//////////////////////////////////////////////////////////////////////////
	// 如果iSindex=TOP，就不需要任何操作
	//////////////////////////////////////////////////////////////////////////
	// 如果iSindex=BOTTOM,所有的逆序
	if ( iSindex == BOTTOM )
	{
		pTemp.clear();
		for ( i = temp2.size() - 1; i>=0 ; i-- )
		{
			pPoint = temp2.at(i);

			temp.push_back( pPoint );
		}

		pTemp.push_back( temp );

		temp.clear();
		for ( i = temp1.size()-1 ; i>=0 ; i-- )
		{
			pPoint = temp1.at(i);
			temp.push_back( pPoint );
		}
		pTemp.push_back( temp );
	}

	//////////////////////////////////////////////////////////////////////////
	// 如果iSindex=LEFT
	if ( iSindex == LEFT )
	{
		pTemp.clear();

		pPoint = temp2.at(1);
		temp.push_back( pPoint );

		pPoint = temp2.at(3);
		temp.push_back( pPoint );

		pPoint = temp2.at( 0 );
		temp.push_back( pPoint );

		pPoint = temp2.at( 2 );
		temp.push_back( pPoint );
		pTemp.push_back( temp );

		temp.clear();

		pPoint = temp1.at( 1 );
		temp.push_back( pPoint );
		//pTemp.push_back( temp );

		pPoint = temp1.at( 3 );
		temp.push_back( pPoint );

		pPoint = temp1.at( 0 );
		temp.push_back( pPoint );

		pPoint = temp1.at(2);
		temp.push_back( pPoint );

		pTemp.push_back( temp );
		//pTemp.push_back( temp );
	/*	temp = pTemp;
		pTemp.clear();

		pPoint = temp.at(5);
		pTemp.push_back( pPoint );

		pPoint = temp.at(7);
		pTemp.push_back( pPoint );

		pPoint = temp.at(4);
		pTemp.push_back( pPoint );

		pPoint = temp.at(6);
		pTemp.push_back( pPoint );

		pPoint = temp.at(1);
		pTemp.push_back( pPoint );

		pPoint = temp.at(3);
		pTemp.push_back( pPoint );

		pPoint = temp.at(0);
		pTemp.push_back( pPoint );

		pPoint = temp.at(2);
		pTemp.push_back( pPoint );*/
	}

	//////////////////////////////////////////////////////////////////////////
	// 如果iSindex = RIGHT
	if ( iSindex == RIGHT )
	{
		pTemp.clear();

		pPoint = temp1.at( 2 );
		temp.push_back( pPoint );
		//pTemp.push_back( temp );

		pPoint = temp1.at( 0 );
		temp.push_back( pPoint );

		pPoint = temp1.at( 3 );
		temp.push_back( pPoint );

		pPoint = temp1.at( 1 );
		temp.push_back( pPoint );

		pTemp.push_back( temp );


		temp.clear();

		pPoint = temp2.at(2);
		temp.push_back( pPoint );

		pPoint = temp2.at(0);
		temp.push_back( pPoint );

		pPoint = temp2.at( 3 );
		temp.push_back( pPoint );

		pPoint = temp2.at( 1 );
		temp.push_back( pPoint );
		pTemp.push_back( temp );

	/*	temp = pTemp;
		pTemp.clear();

		pPoint = temp.at( 2 );
		pTemp.push_back( pPoint );

		pPoint = temp.at( 0 );
		pTemp.push_back( pPoint );

		pPoint = temp.at( 3 );
		pTemp.push_back( pPoint );

		pPoint = temp.at( 1 );
		pTemp.push_back( pPoint );

		pPoint = temp.at( 6 );
		pTemp.push_back( pPoint );

		pPoint = temp.at( 4 );
		pTemp.push_back( pPoint );

		pPoint = temp.at( 7 );
		pTemp.push_back( pPoint );

		pPoint = temp.at( 5 );
		pTemp.push_back( pPoint );*/

	}
}
// 标定
bool CCalibrate::OnCalibrateX(Mat pMat , vector<vector<Point3f> > pRect , int&iCalibrateProcess )
{
	//imshow( "zhou" , pMat );
	//waitKey( 0 );
	// 先找pMat中的角点
	vector<Point2f> corners;
	bool bReturn = true;
	//Mat imageGray;
	Mat ThreshMat;
	int i = 0;

	////////////////////////////////////////////////////////////////////////////
	//// 增加对比度
	int srcHeight = pMat.rows;
	int srcWidth  = pMat.cols;
	int srcStep   = pMat.step;
	unsigned char* pImgData= pMat.data;
	float pixsum=0;
	for(int i=0; i<srcHeight; i++)
	{
		unsigned char* pImgdata = pImgData + i*srcStep;
		for(int j=0; j<srcWidth; j++)
		{
			pixsum += pImgdata[j];
			//pixnum += 1;
		}
	}

	float pixnum = srcHeight * srcWidth;
	float yaver = pixsum/pixnum;

	Mat conimage_Mat(pMat.rows,pMat.cols,CV_8U,Scalar(0)); 
	unsigned char* pImgDatac = conimage_Mat.data;
	for(int i=0; i<srcHeight; i++)
	{
		unsigned char* pImgdata = pImgData + i*srcStep;
		unsigned char* pImgdatac= pImgDatac+ i*srcStep;
		for(int j=0; j<srcWidth; j++)
		{
			float conval = 2.35*(pImgdata[j] - yaver) + yaver;
			if (conval>255) conval=255;
			else if (conval<0) conval=0;
			else conval = conval;

			pImgdatac[j] = conval;
		}
	}

	//imshow( "fff" , conimage_Mat );
	//waitKey( 0 );
	// 
	//////////////////////////////////////////////////////////////////////////
	
	
	// 查找角点
	// 角点检测算法可归纳为三类：
	// (1)基于灰度图像的角点检测
	// (2)基于二值图像的角点检测
	// (3)基于轮廓曲线的角点检测

	///////////////////////////////////////////////////////////////////////////
	// 加上局部二值化查找角点
	// 自适应二值化
//	cv::adaptiveThreshold(pMat, ThreshMat, 255, CV_ADAPTIVE_THRESH_GAUSSIAN_C/*CV_ADAPTIVE_THRESH_MEAN_C*/, CV_THRESH_BINARY/*CV_THRESH_BINARY*/, 101, 0);
	//////////////////////////////////////////////////////////////////////////
//	imshow("ff", ThreshMat);
//	waitKey(0);

	m_Points_Img.clear();
	bReturn = findChessboardCorners( /*conimage_Mat*/ pMat/*ThreshMat*/ , m_cornerDimension , corners , CALIB_CB_ADAPTIVE_THRESH|CALIB_CB_NORMALIZE_IMAGE|CALIB_CB_FAST_CHECK );

	iCalibrateProcess++;

	// 图像二值化
	if ( !bReturn )             // 如果角点没有找成功，那么就开始使用其他的方案
	{
		corners.clear();

		for ( i = 130 ; i< 200 ; i= i+10 )
		{
			// 对单通道数组应用固定阈值操作, 对灰度图像进行阈值操作得到二值图像。
			threshold( /*conimage_Mat*/pMat , ThreshMat , i , 255 , THRESH_BINARY );

			//bReturn = findChessboardCorners( ThreshMat , m_cornerDimension , corners , CALIB_CB_ADAPTIVE_THRESH|CALIB_CB_NORMALIZE_IMAGE|CALIB_CB_FAST_CHECK );
			if ( bReturn )
			{
			//	imshow( "zhou" , ThreshMat );
			//	waitKey( 0 );
				break;
			}
		}
		
	}

	iCalibrateProcess++;

	if ( !bReturn )
	{

#if DEBUG_SHOW_IMAGE                // 存储标定图像
		int j = 0;
		Mat imageTemp = pMat.clone();

		for ( j = 0 ; j<corners.size() ; j++ )
		{
			circle( imageTemp, corners[j], 10, Scalar(0,0,255), 2, 8, 0);//OpenCV中三彩色通道的顺序是BGR而不是RGB。
		}

		std::stringstream StrStm;
		string imageFileName;
		StrStm.clear();
		StrStm<<"tempimg//"<<m_iLocation<<"_corner.jpg";
		StrStm>>imageFileName;
		imwrite(imageFileName,imageTemp);

		imshow(imageFileName, imageTemp);
		waitKey(0);
#endif

		return bReturn;
	}

	// 亚像素精细化
	// 亚像素精确化
	// 最大迭代30次，或者角点位置变化小于0.1时，停止迭代过程
	// m_corners的值是角点的位置

	cornerSubPix( /*conimage_Mat*/pMat, corners, Size(11, 11), Size(-1, -1), TermCriteria(CV_TERMCRIT_EPS + CV_TERMCRIT_ITER, 30, 0.1));

	iCalibrateProcess++;

	// 判断第一个角点大约在图像上的哪个位置(左上？右上？左下？右下？)
	// 前判断是以列扫描方式还是行扫描方式
	OriginInImage Location;
	ArrayModeInImage ArrayMode;
	int iCheckLen = ( m_cornerDimension.width>m_cornerDimension.height?m_cornerDimension.height:m_cornerDimension.width );
	OriginLocationAndErgodicMode( corners , iCheckLen , Location , ArrayMode );

	iCalibrateProcess++;

	// 将corners里面的数据进行重排
	switch( m_iLocation )
	{
	case TOP:OnReSort_Top( Location , ArrayMode , corners, m_cornerDimension ,m_Points_Img );break;        // 对前图像的角点进行重新排序
	case BOTTOM:OnResort_Bottom( Location , ArrayMode , corners , m_cornerDimension ,m_Points_Img );break;  // 对后图像的角点进行重新排序
	case LEFT:OnResort_Left( Location , ArrayMode , corners , m_cornerDimension , m_Points_Img );break;      // 对左图像的角点进行重新排序
	case RIGHT:OnResort_Right( Location , ArrayMode , corners , m_cornerDimension ,m_Points_Img );break;
	}

	iCalibrateProcess++;

	// 查找corners中y点的最大值和最小值
	onFindMaxYAndMinY( m_Points_Img );
	iCalibrateProcess++;

	// 获取两个正方形共八个顶点
	m_Rect_Img.clear();
	bReturn = OnGetRectPosition( /*conimage_Mat*/pMat , m_iLocation , m_Rect_Img );
	if ( !bReturn )
	{
		return bReturn;                // 正方形检测有误，返回不继续往下执行
	}
	iCalibrateProcess++;
	// 对m_Rect_Img进行重新排序
	OnReSort_Square_Img( m_Rect_Img , m_iLocation );

	iCalibrateProcess++;

#if 1
	// 对m_Rect_Img进行亚像素操作
	Mat temppppp;
	cv::adaptiveThreshold(conimage_Mat, temppppp, 255, /*CV_ADAPTIVE_THRESH_GAUSSIAN_C*/CV_ADAPTIVE_THRESH_MEAN_C, CV_THRESH_BINARY, 151, 0);
	//imshow("ffff", temppppp);
	//waitKey(0);
	
	RectImgSubPix(temppppp/*conimage_Mat*/ /*pMat*/, m_Rect_Img);
#endif
	//for ( int i = 0 ; i<m_Rect_Img.size() ; i++ )
	//{
	//	cornerSubPix( pMat /*imageGray*/ , m_Rect_Img.at(i) , Size( 7 , 7 ) , Size( -1 , -1 ) , TermCriteria( CV_TERMCRIT_EPS + CV_TERMCRIT_ITER , 30 , 0.1 ) );
	//}
	iCalibrateProcess++;

	// 目前角点也获取了，对应点也获取了，接下来就是标定的过程
	OnCalibrate_private( m_Points_World , pRect , m_Points_Img , m_Rect_Img , pMat.size() , iCalibrateProcess );

#if DEBUG_SHOW_IMAGE
	// 将世界坐标系的点和图像坐标系上的点存储成文件，查看世界坐标系的点与图像坐标系的点是否是对应的
	if ( bReturn )
	{
		string temp;
		std::stringstream StrStm;
		std::stringstream StrStm2;
		string storegePath;
		string storegePath2;
		vector<Point3f> pWorldPoint;

	//	for ( int i = 0 ; i<FISHEYE_COUNT ; i++ )
	//	{
			StrStm.clear();
			StrStm<<"tempimg//"<<m_iLocation<<"_worldx.txt";
			StrStm>>storegePath;
			ofstream foutmm(storegePath);
			StrStm2.clear();
			StrStm2<<"tempimg//"<<m_iLocation<<"_imgx.txt";
			StrStm2>>storegePath2;
			ofstream foutmm1( storegePath2 );

			for ( int j = 0 ; j< m_Points_World.size() ; j++  )
			{
				pWorldPoint = m_Points_World.at(j);
				for ( int xx = 0 ; xx < pWorldPoint.size() ; xx++ )
				{
					Point3f xf = pWorldPoint.at( xx );
					foutmm<<xf.x<<' '<<xf.y<<' '<<endl;
				}
				
				foutmm<<endl;

			}

			for ( int z = 0;z<m_Points_Img.size() ; z++ )
			{
				Point2f gf = m_Points_Img.at(z);
				foutmm1<<gf.x<<' '<<gf.y<<endl;
			}
	//	}
	}

	int j = 0;
	int rrr = 0;
	Mat imageTemp = pMat.clone();
	vector<Point2f> pTempppp;

	for ( j = 0 ; j<corners.size() ; j++ )
	{
		circle( imageTemp, corners[j], 10, Scalar(0,0,255), 2, 8, 0);//OpenCV中三彩色通道的顺序是BGR而不是RGB。
	}

	for ( j = 0 ; j<m_Rect_Img.size() ; j++ )
	{
		pTempppp = m_Rect_Img.at(j);
		for ( rrr = 0 ; rrr<pTempppp.size() ; rrr++ )
		{
			circle( imageTemp , pTempppp.at(rrr), 2 , Scalar(0 , 0 , 255) , 2 );
		}
	}

	std::stringstream StrStm;
	string imageFileName;
	StrStm.clear();
	StrStm<<"tempimg//"<<m_iLocation<<"_corner.jpg";
	StrStm>>imageFileName;
	imwrite(imageFileName,imageTemp);

#endif

	return bReturn;
}

// 亚像素化
void CCalibrate::RectImgSubPix( Mat grayImage , vector<vector<Point2f> >&rect_Point )
{
	//for ( int i = 0 ; i<m_Rect_Img.size() ; i++ )
	//{
	//	cornerSubPix( pMat /*imageGray*/ , m_Rect_Img.at(i) , Size( 7 , 7 ) , Size( -1 , -1 ) , TermCriteria( CV_TERMCRIT_EPS + CV_TERMCRIT_ITER , 30 , 0.1 ) );
	//}

	// 6到12为窗口的大小
	vector<Point2f> tempPoint1[2];
	vector<Point2f> tempPoint2[2];
	vector<vector<Point2f> > tempPoint3 = rect_Point;
	int index[2][4]={0};              // 0表示初使未被填充任何数据。1表示可以填充数据。2表示不能填充数据。全为2退出
	Point2f         Temp[2];
	bool bCheck;
	int iTempCount = 0;

	tempPoint1[0] = rect_Point.at(0);
	tempPoint1[1] = rect_Point.at(1);

	for ( int i = 1 ; i < 12 ; i++ )
	{
		bCheck = true;

		tempPoint2[0] = tempPoint1[0];
		tempPoint2[1] = tempPoint1[1];
		for( int j = 0 ; j<rect_Point.size() ; j++ )
		{
			cornerSubPix( grayImage , tempPoint2[j] , Size(i,i) , Size(-1,-1) , TermCriteria(  CV_TERMCRIT_EPS + CV_TERMCRIT_ITER , 30 , 0.01 ) );

			// 比较tempPoint1和tempPoint2，如果有相同的值，说明亚像素化失败，那么i就加1继续亚像素化
			for ( int z = 0 ; z<tempPoint2[j].size() ; z++ )
			{
				Temp[0] = tempPoint2[j].at(z);
				Temp[1] = tempPoint1[j].at(z);

				// 如果有值相等的，说明亚像素化失败
				if ( (abs(Temp[0].x - Temp[1].x)<10e-6)&&(abs(Temp[0].y - Temp[1].y)<10e-6) )
				//if ((abs(Temp[0].x - Temp[1].x)<0.5) && (abs(Temp[0].y - Temp[1].y)<0.5))
				{
					bCheck = false;
					continue;                       // 跳出当前的for循环
				}

				if ( index[j][z] == 0 )
				{
					index[j][z] = 1;
				}
			}
			
		}

		// 检查index，如果为1的话，那么就从相应位置取出数据填充
		
		for ( int x = 0 ; x<2 ; x++ )
		{
			for ( int y = 0 ; y<4 ; y++ )
			{
				if ( index[x][y] == 1 )
				{
					tempPoint3.at(x).at(y) = tempPoint2[x].at(y);

					index[x][y] = 2;            // 表示已经成功亚像素化

					iTempCount = iTempCount + 1;
				}

				//if ( index[x][y] == 2 )
				//{
				//	iTempCount = iTempCount + 1; 
				//}
				
				
			}
			
		}

		if ( iTempCount == 8 )
		{
			bCheck = true;
			break;
		}
		//else
		//{
		//	bCheck = false;
		//}
		
		

		// 如果值不相等，说明亚像素成功，就不需要再亚像素化了
		//if ( bCheck )
		//{
		//	break;
		//}
		
	}

	//if ( bCheck )
	//{
		rect_Point.clear();
		rect_Point = tempPoint3;

	//	rect_Point.push_back( tempPoint2[0] );
	//	rect_Point.push_back( tempPoint2[1] );
	//}
}

void CCalibrate::onFindMaxYAndMinY( vector<Point2f> corners )
{
	float fTemp;

	yMax = corners.at(0).y;
	yMin = corners.at(0).y;

	for ( int i = 1 ; i< corners.size() ; i++ )
	{
		fTemp = corners.at(i).y;

		if ( yMax < fTemp )
		{
			yMax = fTemp;
		}

		if ( yMin > fTemp )
		{
			yMin = fTemp;
		}
	}

	xMax = corners.at(0).x;
	xMin = corners.at(0).x;

	for ( int i = 1 ; i< corners.size() ; i++ )
	{
		fTemp = corners.at(i).x;

		if ( xMax < fTemp )
		{
			xMax = fTemp;
		}

		if ( xMin > fTemp )
		{
			xMin = fTemp;
		}
	}


}

void CCalibrate::AdaptiveFindThreshold(const Mat src , double *low, double *high, int aperture_size )  
{                                                                                
	//cv::Mat src = cv::cvarrToMat(image);                                     
	const int cn = src.channels();                                           
	cv::Mat dx(src.rows, src.cols, CV_16SC(cn));                             
	cv::Mat dy(src.rows, src.cols, CV_16SC(cn));                             

	cv::Sobel(src, dx, CV_16S, 1, 0 , aperture_size, 1 , cv::BORDER_CONSTANT );  
	cv::Sobel(src, dy, CV_16S, 0, 1 , aperture_size, 1 , cv::BORDER_CONSTANT );  

	CvMat _dx = dx, _dy = dy;                                                
	_AdaptiveFindThreshold(&_dx, &_dy, low, high);                           

} 

// 仿照matlab，自适应求高低两个门限                                              
void CCalibrate::_AdaptiveFindThreshold(CvMat *dx, CvMat *dy, double *low, double *high) 
{
	CvSize size;                                                             
	IplImage *imge=0;                                                        
	int i,j;                                                                 
	CvHistogram *hist;                                                       
	int hist_size = 255;                                                     
	float range_0[]={0,256};                                                 
	float* ranges[] = { range_0 };                                           
	double PercentOfPixelsNotEdges = 0.7;                                    
	size = cvGetSize(dx);                                                    
	imge = cvCreateImage(size, IPL_DEPTH_32F, 1);                            
	// 计算边缘的强度, 并存于图像中                                          
	float maxv = 0;                                                          
	for(i = 0; i < size.height; i++ )                                        
	{                                                                        
		const short* _dx = (short*)(dx->data.ptr + dx->step*i);          
		const short* _dy = (short*)(dy->data.ptr + dy->step*i);          
		float* _image = (float *)(imge->imageData + imge->widthStep*i);  
		for(j = 0; j < size.width; j++)                                  
		{                                                                
			_image[j] = (float)(abs(_dx[j]) + abs(_dy[j]));          
			maxv = maxv < _image[j] ? _image[j]: maxv;               

		}                                                                
	}                                                                        
	if(maxv == 0){                                                           
		*high = 0;                                                       
		*low = 0;                                                        
		cvReleaseImage( &imge );                                         
		return;                                                          
	}                                                                        

	// 计算直方图                                                            
	range_0[1] = maxv;                                                       
	hist_size = (int)(hist_size > maxv ? maxv:hist_size);                    
	hist = cvCreateHist(1, &hist_size, CV_HIST_ARRAY, ranges, 1);            
	cvCalcHist( &imge, hist, 0, NULL );                                      
	int total = (int)(size.height * size.width * PercentOfPixelsNotEdges);   
	float sum=0;                                                             
	int icount = hist->mat.dim[0].size;                                      

	float *h = (float*)cvPtr1D( hist->bins, 0 );                             
	for(i = 0; i < icount; i++)                                              
	{                                                                        
		sum += h[i];                                                     
		if( sum > total )                                                
			break;                                                   
	}                                                                        
	// 计算高低门限                                                          
	*high = (i+1) * maxv / hist_size ;                                       
	*low = *high * 0.4;                                                      
	cvReleaseImage( &imge );                                                 
	cvReleaseHist(&hist);
}

bool CCalibrate::CheckRect( vector<Point>&RectPoint , int imageWide , vector<vector<Point> >&leftCorner , vector<vector<Point> >&rightCorner )
{
	bool bReturn = true;

	int           iTemp = imageWide/2;                     // 中间的分界线
	int           iBe = 0/*150*/;                               // 从中间分界线向两边移动300个像素，经验值

	Point         sTemp;
	Point         sTemp1;
	Point         sTempMax;
	int           iTempMaxIndex;
	Point         sTempMin;
	int           iTempMinIndex;
	vector<Point> RectPoint_bak = RectPoint;
	vector<int>   vIndex;

	// 第一步，以图像正中间的那条竖线为准，提取出左边的正方形和右边的正方形
	int i = 0;

	bool bLeft = false;
	bool bRight = false;
	bool bLeftCheck = false;
	bool bRightCheck = false;

	// 检查左边
	for ( i = 0 ; i<RectPoint.size() ; i++ )
	{
		sTemp = RectPoint.at( i );

		//////////////////////////////////////////////////////////////////////////
		// 走到这一步区分左边和右边的点
		bLeft = false;


		if ( sTemp.x <(iTemp-iBe) )           // 所有的点在左边
		{
			bLeft = true;
		}
		else
		{
			bLeft = false;
			break;
		}
	}

	//////////////////////////////////////////////////////////////////////////
	// 
	sTempMax = RectPoint.at(0);
	sTempMin = RectPoint.at(0);
	iTempMaxIndex = 0;
	iTempMinIndex = 0;
	for ( i = 1 ; i<RectPoint.size() ; i++ )
	{
		sTemp = RectPoint.at(i);
		// 检查x的最大值和最小值
		if ( sTempMax.x < sTemp.x )
		{
			sTempMax = sTemp;
			iTempMaxIndex = i;
		}
		if ( sTempMin.x > sTemp.x )
		{
			sTempMin = sTemp;
			iTempMinIndex = i;
		}
	}


	//////////////////////////////////////////////////////////////////////////
	// bLeft和bRight中只有一个为true，另一个为FALSE,如果都为true的话，那就说明此点跨越了中间
	if ( bLeft&&(i == RectPoint.size() ) )            // 左边的点
	{
		int iTempLeft_bottom = 0;
		int iTempRight_Top = 0;
		// 左边，小于sTempMin的Y值个数不小于两个
		for ( i = 0 ; i<RectPoint.size() ; i++ )
		{
			sTemp = RectPoint.at(i);
			if ( sTempMin.y > sTemp.y )
			{
				iTempLeft_bottom = iTempLeft_bottom + 1;
			}
			if ( sTempMax.y < sTemp.y )
			{
				iTempRight_Top = iTempRight_Top + 1;
			}
		}

		if ( (iTempLeft_bottom >=2)&&(iTempRight_Top>=2 ) )
		{
			bLeftCheck = true;
		}
		else
		{
			bLeftCheck = false;
		}
	}
	else
	{
		bLeftCheck = false;
	}

	// 检查右边
	for ( i = 0 ; i<RectPoint.size() ; i++ )
	{
		sTemp = RectPoint.at( i );

		//////////////////////////////////////////////////////////////////////////
		// 走到这一步区分左边和右边的点
		bRight = false;


		if ( sTemp.x >(iTemp+iBe) )           // 所有的点在左边
		{
			bRight = true;
		}
		else
		{
			bRight = false;
			break;
		}
	}

	if ( bRight&&(i == RectPoint.size() ) )            // 右边的点
	{
		int iTempLeft_top = 0;
		int iTempRight_bottom = 0;
		// 左边，小于sTempMin的Y值个数不小于两个
		for ( i = 0 ; i<RectPoint.size() ; i++ )
		{
			sTemp = RectPoint.at(i);
			if ( sTempMin.y < sTemp.y )
			{
				iTempLeft_top = iTempLeft_top + 1;
			}
			if ( sTempMax.y > sTemp.y )
			{
				iTempRight_bottom = iTempRight_bottom + 1;
			}
		}

		if ( (iTempLeft_top >=2)&&(iTempRight_bottom>=2 ) )
		{
			bRightCheck = true;
		}
		else
		{
			bRightCheck = false;
		}
		
	}
	else
	{
		bRightCheck = false;
	}

	//////////////////////////////////////////////////////////////////////////
	if ( bLeftCheck || bRightCheck )
	{
		//////////////////////////////////////////////////////////////////////////
		// 如果满足要求，就将其重新排序后输出

		RectPoint.clear();
		for ( i = 0 ; i<RectPoint_bak.size() ; i++ )
		{
			if ( (i != iTempMinIndex)&&(i!= iTempMaxIndex) )
			{
				//sTemp = RectPoint_bak.at( i );
				vIndex.push_back( i );
			}
		}

		sTemp = RectPoint_bak.at( vIndex.at(0) );
		sTemp1 = RectPoint_bak.at(vIndex.at(1) );

		if ( bLeftCheck )             // 左边的点
		{
			// sTempMax的x值最大时，为正方形右上角
			// sTempMin，为正方形的左下角
			if ( sTemp.y < sTemp1.y )
			{
				RectPoint.push_back( sTemp );
			}
			else
			{
				RectPoint.push_back( sTemp1 );
			}

			RectPoint.push_back( sTempMax );

			RectPoint.push_back( sTempMin );

			if ( sTemp.y > sTemp1.y )
			{
				RectPoint.push_back( sTemp );
			}
			else
			{
				RectPoint.push_back( sTemp1 );
			}

			leftCorner.push_back( RectPoint );

		}
		if ( bRightCheck )            // 右边的点
		{
			// sTempMax为最大时，为正方形右下角
			// sTempMin，为正方形的左上角
			RectPoint.push_back( sTempMin );
			
			if ( sTemp.y < sTemp1.y )
			{
				RectPoint.push_back( sTemp );
				RectPoint.push_back( sTemp1 );
			}
			else
			{
				RectPoint.push_back( sTemp1 );
				RectPoint.push_back( sTemp );
			}

			RectPoint.push_back( sTempMax );

			rightCorner.push_back( RectPoint );
		}
		//////////////////////////////////////////////////////////////////////////
		bReturn = true;
	}
	else
	{
		bReturn = false;
	}

	return bReturn;
}

// 检查当前点是否在棋盘的角点处，如果在棋盘的角点处，那么就不是我们想要的点
bool CCalibrate::bCheckRectOnCorner( vector<Point2f>  Points_Img , vector<Point> PointLocation )
{
	int i = 0 ;
	int j = 0 ;
	bool bFinded = false;
	float fDistence = 10;

	Point2f point1;
	Point   point2;

	for ( i = 0 ; i<PointLocation.size() ; i++ )
	{
		point2 = PointLocation.at(i);
		for ( j = 0 ; j<Points_Img.size() ; j++ )
		{
			point1 = Points_Img.at(j);

			//////////////////////////////////////////////////////////////////////////
			// 求point1到point2的距离
			fDistence = sqrt( (point2.x - point1.x)*(point2.x - point1.x) + (point2.y - point1.y)*(point2.y - point1.y) );

			if ( fDistence <10 )
			{
				bFinded = true;
				break;
			}
		}

		if ( bFinded )
		{
			break;
		}
	}
	return !bFinded;
}

// 求一定距离范围内的均值
void CCalibrate::iGetAverage( vector<vector<Point2f> >&pPoint , int iMinIndex , vector<Point2f>&average )
{
	vector<Point2f> pMinPoint;
	vector<Point2f> pCurrentPoint;      // 当前点
	int iLen = 20;                      // 相对应位置不能大于20个像素

	float p1;
	float p2;
	float p3;
	float p4;
	float p1_y;
	float p2_y;
	float p3_y;
	float p4_y;

	unsigned int location1 = 0;
	unsigned int location2 = 0;
	unsigned int location3 = 0;
	unsigned int location4 = 0;
	unsigned int location1_y = 0;
	unsigned int location2_y = 0;
	unsigned int location3_y = 0;
	unsigned int location4_y = 0;
	
	int iCount = 0;

	//while( true )
	for ( int iEnd = 0 ; iEnd < pPoint.size(); iEnd++ )
	{
		pMinPoint = pPoint.at( iMinIndex );

		for ( int i = 0 ; i<pPoint.size() ; i++ )
		{
			if ( i == iMinIndex )
			{
				continue;
			}

			pCurrentPoint = pPoint.at( i );

			p1 = abs(pCurrentPoint.at(0).x - pMinPoint.at(0).x);
			p2 = abs(pCurrentPoint.at(1).x - pMinPoint.at(1).x);
			p3 = abs(pCurrentPoint.at(2).x - pMinPoint.at(2).x);
			p4 = abs(pCurrentPoint.at(3).x - pMinPoint.at(3).x);

			p1_y = abs(pCurrentPoint.at(0).y - pMinPoint.at(0).y);
			p2_y = abs(pCurrentPoint.at(1).y - pMinPoint.at(1).y);
			p3_y = abs(pCurrentPoint.at(2).y - pMinPoint.at(2).y);
			p4_y = abs(pCurrentPoint.at(3).y - pMinPoint.at(3).y);

			if ( (p1<iLen)&&(p1_y<iLen)&&(p2<iLen)&&(p2_y<iLen)&&(p3<iLen)&&(p3_y<iLen)&&(p4<iLen)&&(p4_y<iLen) )
			{
				location1 = location1 + pCurrentPoint.at(0).x;
				location2 = location2 + pCurrentPoint.at(1).x;
				location3 = location3 + pCurrentPoint.at(2).x;
				location4 = location4 + pCurrentPoint.at(3).x;

				location1_y = location1_y + pCurrentPoint.at(0).y;
				location2_y = location2_y + pCurrentPoint.at(1).y;
				location3_y = location3_y + pCurrentPoint.at(2).y;
				location4_y = location4_y + pCurrentPoint.at(3).y;

				iCount = iCount + 1;
			}


		}
		if ( iCount == 0 )
		{
			iMinIndex = rand()%pPoint.size();
		}
		else
		{
			break;
		}
	}
	

	location1 = location1 + pMinPoint.at(0).x;
	location2 = location2 + pMinPoint.at(1).x;
	location3 = location3 + pMinPoint.at(2).x;
	location4 = location4 + pMinPoint.at(3).x;

	location1_y = location1_y + pMinPoint.at(0).y;
	location2_y = location2_y + pMinPoint.at(1).y;
	location3_y = location3_y + pMinPoint.at(2).y;
	location4_y = location4_y + pMinPoint.at(3).y;

	Point2f pPoint2f;

	pPoint2f.x = location1/(iCount+1);
	pPoint2f.y = location1_y/(iCount+1);
	average.push_back( pPoint2f );

	pPoint2f.x = location2/(iCount+1);
	pPoint2f.y = location2_y/(iCount+1);
	average.push_back( pPoint2f );

	pPoint2f.x = location3/(iCount+1);
	pPoint2f.y = location3_y/(iCount+1);
	average.push_back( pPoint2f );

	pPoint2f.x = location4/(iCount+1);
	pPoint2f.y = location4_y/(iCount+1);
	average.push_back( pPoint2f );
	//average.at(0)
	
}

// 计算点到直线的距离
static double PointToSegDist(double x, double y, double x1, double y1, double x2, double y2)
{
	double cross = (x2 - x1) * (x - x1) + (y2 - y1) * (y - y1);
	if (cross <= 0) return sqrt((x - x1) * (x - x1) + (y - y1) * (y - y1));

	double d2 = (x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1);
	if (cross >= d2) return sqrt((x - x2) * (x - x2) + (y - y2) * (y - y2));

	double r = cross / d2;
	double px = x1 + (x2 - x1) * r;
	double py = y1 + (y2 - y1) * r;
	return sqrt((x - px) * (x - px) + (py - y) * (py - y));
}	

void CCalibrate::editPointRange(Point&location, int rows, int cols)
{
	if (location.x < 0)
	{
		location.x = 0;
	}

	if (location.y < 0)
	{
		location.y = 0;
	}

	if (location.x >= cols)
	{
		location.x = cols - 1;
	}

	if (location.y >= rows)
	{
		location.y = rows - 1;
	}
}

// 在二值化中查找边界点
bool CCalibrate::CheckBinaryPoint(Point PointLocation, Mat BinaryMat )
{
	// 在四邻域内查找
	bool bReturn = false;
	Point PTemp;
	int data[4] = {0};
	int iAdd = 0;
	editPointRange(PointLocation, BinaryMat.rows, BinaryMat.cols);
	iAdd = BinaryMat.data[PointLocation.y*BinaryMat.cols + PointLocation.x] / 255;

	if (iAdd == 0)             // 检查当前点是否在边界上
	{
		return bReturn;
	}

	iAdd = 0;

	// 上
	PTemp.x = PointLocation.x;
	PTemp.y = PointLocation.y - 1;
	editPointRange(PTemp, BinaryMat.rows, BinaryMat.cols );
	data[0] = (int)BinaryMat.data[ PTemp.y*BinaryMat.cols + PTemp.x ] / 255;

	// 下
	PTemp.y = PointLocation.y + 1;
	editPointRange(PTemp, BinaryMat.rows, BinaryMat.cols);
	data[1] = (int)BinaryMat.data[PTemp.y*BinaryMat.cols + PTemp.x] / 255;

	// 左
	PTemp.x = PointLocation.x - 1;
	PTemp.y = PointLocation.y;
	editPointRange(PTemp, BinaryMat.rows, BinaryMat.cols);
	data[2] = (int)BinaryMat.data[PTemp.y*BinaryMat.cols + PTemp.x] / 255;

	// 右
	PTemp.x = PointLocation.x + 1;
	editPointRange(PTemp, BinaryMat.rows, BinaryMat.cols);
	data[3] = (int)BinaryMat.data[PTemp.y*BinaryMat.cols + PTemp.x] / 255;

	for (int i = 0; i < 4; i++)
	{
		iAdd = iAdd + data[i];
	}

	if ((iAdd > 0) && (iAdd < 4))
	{
		bReturn = true;
	}
	return bReturn;
}

bool CCalibrate::ergodicBinaryMat(Point BeginPoint, Mat BinaryMat, vector<Point>&edgaPoint)
{
	bool bReturn = false;

	//////////////////////////////////////////////////////////////
	// 在二值化中找起点
	int iRange = 2;        // 查找领域内是否有边缘点
	Point pPointBegin;
	Point pPoint1,pPoint2;          // 遍历的点
	Mat identifiIndex(BinaryMat.rows, BinaryMat.cols, CV_8UC1, Scalar(0));           // 访问标识位，未被访问为0，以被访问为1

	for (int i = (-1)*iRange; i < iRange+1; i++)
	{
		for (int j = (-1)*iRange; j < iRange+1; j++)
		{
			pPointBegin.y = BeginPoint.y + i;
			pPointBegin.x = BeginPoint.x + j;

			bReturn = CheckBinaryPoint(pPointBegin, BinaryMat);

			if (bReturn)
			{
				i = iRange;
				break;
			}
		}
	}

	if (!bReturn)       // 没找到
	{
		return bReturn;
	}

	////////////////////////////////////////////////////////////////
	// 以pPoint为起点遍历二值化边缘点，8邻域内
	edgaPoint.push_back(pPointBegin);
	identifiIndex.data[pPointBegin.y*BinaryMat.cols + pPointBegin.x] = 1;              // 设置访问位为1

	pPoint1 = pPointBegin;

	while (true)
	{
		// 8领域检查
		for (int i = -1; i < 2; i++)
		{
			for (int j = -1; j < 2; j++)
			{
				if ((i == 0) && (j == 0))
				{
					continue;
				}

				pPoint2.x = pPoint1.x + j;
				pPoint2.y = pPoint1.y + i;
				// 检查点
				editPointRange(pPoint2, BinaryMat.rows, BinaryMat.cols );

				// 判断pPoint2点是否为边缘点
			}
		}
	}
	////////////////////////////////////////////////////////////////

	return bReturn;
}

// 在轮廓图图像上查找4个顶点
void CCalibrate::GetRectPosFromContourPic(Mat *erodeMat, vector<Point> *Contour, bool bRight, vector<Point> *roi_point_approx)
{
	bool bReturn = false;

	Point minX;
	Point maxX;
	Point minY;
	Point maxY;

	vector<Point> vpMinX;
	vector<Point> vpMinY;
	vector<Point> vpMaxX;
	vector<Point> vpMaxY;

	int index[4] = { 0 };	// minX, maxX, minY, maxY

	vector<Point> near;

	double distance = 0.0;
	double tmpDistance = 0.0;

	Point pVertex;

	minX = maxX = minY = maxY = Contour->at(0);

	for (int j = 1; j < Contour->size(); j++)
	{
		if (bRight)
		{
			if (Contour->at(j).x <= minX.x)
			{
				minX = Contour->at(j);
				index[0] = j;
			}
			else if (Contour->at(j).x >= maxX.x)
			{
				maxX = Contour->at(j);
				index[1] = j;
			}

			if (Contour->at(j).y <= minY.y)
			{
				minY = Contour->at(j);
				index[2] = j;
			}
			else if (Contour->at(j).y >= maxY.y)
			{
				maxY = Contour->at(j);
				index[3] = j;
			}
		}
		else
		{
			if (Contour->at(j).x <= minX.x)
			{
				minX = Contour->at(j);
				index[0] = j;
			}
			else if (Contour->at(j).x >= maxX.x)
			{
				maxX = Contour->at(j);
				index[1] = j;
			}

			if (Contour->at(j).y <= minY.y)
			{
				minY = Contour->at(j);
				index[2] = j;
			}
			else if (Contour->at(j).y >= maxY.y)
			{
				maxY = Contour->at(j);
				index[3] = j;
			}
		}
	}

	// 顶点精细化
	vpMaxX.clear();
	vpMaxY.clear();
	vpMinX.clear();
	vpMinY.clear();
	for (int j = 0; j < Contour->size(); j++)
	{
		if (Contour->at(j).x == minX.x) vpMinX.push_back(Contour->at(j));
		else if (Contour->at(j).x == maxX.x) vpMaxX.push_back(Contour->at(j));

		if (Contour->at(j).y == minY.y) vpMinY.push_back(Contour->at(j));
		else if (Contour->at(j).y == maxY.y) vpMaxY.push_back(Contour->at(j));
	}

	Point pMinX;
	Point pMaxX;
	Point pMinY;
	Point pMaxY;

	if (bRight)
	{
		pMaxY = vpMinX.front();
		for (int j = 1; j < vpMinX.size(); j++)
		{
			if (vpMinX.at(j).y < pMaxY.y) pMaxY = vpMinX.at(j);
		}

		pMinY = vpMaxX.front();
		for (int j = 1; j < vpMaxX.size(); j++)
		{
			if (vpMaxX.at(j).y > pMinY.y) pMinY = vpMaxX.at(j);
		}

		pMaxX = vpMinY.front();
		for (int j = 1; j < vpMinY.size(); j++)
		{
			if (vpMinY.at(j).x > pMaxX.x) pMaxX = vpMinY.at(j);
		}

		pMinX = vpMaxY.front();
		for (int j = 1; j < vpMaxY.size(); j++)
		{
			if (vpMaxY.at(j).x < pMinX.x) pMinX = vpMaxY.at(j);
		}
	}
	else
	{
		pMaxY = vpMinX.front();
		for (int j = 1; j < vpMinX.size(); j++)
		{
			if (vpMinX.at(j).y > pMaxY.y) pMaxY = vpMinX.at(j);
		}

		pMinY = vpMaxX.front();
		for (int j = 1; j < vpMaxX.size(); j++)
		{
			if (vpMaxX.at(j).y < pMinY.y) pMinY = vpMaxX.at(j);
		}

		pMinX = vpMinY.front();
		for (int j = 1; j < vpMinY.size(); j++)
		{
			if (vpMinY.at(j).x < pMinX.x) pMinX = vpMinY.at(j);
		}

		pMaxX = vpMaxY.front();
		for (int j = 1; j < vpMaxY.size(); j++)
		{
			if (vpMaxY.at(j).x > pMaxX.x) pMaxX = vpMaxY.at(j);
		}
	}

	int maxIndex = index[0] > index[1] ? index[0] : index[1];
	int minIndex = index[0] > index[1] ? index[1] : index[0];
	if (index[3] > minIndex && index[3] < maxIndex)
	{
		for (int j = 0; j < minIndex; j++)
		{
			tmpDistance = PointToSegDist(Contour->at(j).x, Contour->at(j).y, pMaxY.x, pMaxY.y, pMinY.x, pMinY.y);
			if (tmpDistance - distance > 1e-6)
			{
				distance = tmpDistance;
				pVertex = Contour->at(j);
			}
		}

		for (int j = maxIndex; j < Contour->size(); j++)
		{
			tmpDistance = PointToSegDist(Contour->at(j).x, Contour->at(j).y, pMaxY.x, pMaxY.y, pMinY.x, pMinY.y);
			if (tmpDistance - distance > 1e-6)
			{
				distance = tmpDistance;
				pVertex = Contour->at(j);
			}
		}
	}
	else
	{
		for (int j = minIndex; j < maxIndex; j++)
		{
			tmpDistance = PointToSegDist(Contour->at(j).x, Contour->at(j).y, pMaxY.x, pMaxY.y, pMinY.x, pMinY.y);
			if (tmpDistance - distance > 1e-6)
			{
				distance = tmpDistance;
				pVertex = Contour->at(j);
			}
		}
	}

	roi_point_approx->clear();
	if (bRight)
	{
		roi_point_approx->push_back(pMaxY);
		roi_point_approx->push_back(pMinY);
		roi_point_approx->push_back(pVertex);
		roi_point_approx->push_back(pMinX);
	}
	else
	{
		roi_point_approx->push_back(pMaxY);
		roi_point_approx->push_back(pMinY);
		roi_point_approx->push_back(pVertex);
		roi_point_approx->push_back(pMaxX);
	}
}

// 在二值化图像上获取四边形在图像坐标系上的位置
void CCalibrate::GetRectPosFromBinPic(Mat *ThreshMat, Mat *binMatBGR, Mat *erodeMat, vector<Point> *Contour, vector<Point> *roi_point_approx)
{
	bool bReturn = false;

	Point minX;
	Point maxX;
	Point minY;
	Point maxY;

	vector<Point> vpMinX;
	vector<Point> vpMinY;
	vector<Point> vpMaxX;
	vector<Point> vpMaxY;

	// 第一步，以图像正中间的那条竖线为准，提取出左边的正方形和右边的正方形
	int           iTemp = binMatBGR->cols / 2;                     // 中间的分界线
	int           iBe = 0/*150*/;                               // 从中间分界线向两边移动300个像素，经验值
	Point         sTemp;

	bool bLeft = false;
	bool bRight = false;

	vector<Point> near;

	double distance = 0.0;
	double tmpDistance = 0.0;


	//////////////////////////////////////////////////////////////
	// 在二值化中找边缘点
	int iRange = 5;        // 查找领域内是否有边缘点
	Point pPointBegin;
	Point BeginPoint;
	vector<Point> edge;
	int count = Contour->size();

#if 0
	edge.clear();
	for (int k = 0; k < Contour.size(); k++)
	{
		BeginPoint = Contour.at(k);

		for (int i = (-1)*iRange; i < iRange + 1; i++)
		{
			for (int j = (-1)*iRange; j < iRange + 1; j++)
			{
				pPointBegin.y = BeginPoint.y + i;
				pPointBegin.x = BeginPoint.x + j;

				bReturn = CheckBinaryPoint(pPointBegin, ThreshMat);

				if (bReturn)
				{
					edge.push_back(pPointBegin);
				}
			}
		}
	}

	// 过滤相同的点
	bool needPush = false;
	vector<Point> uniqueEdge;
	uniqueEdge.clear();
	uniqueEdge.push_back(edge.front());

	for (vector<Point>::iterator it = edge.begin() + 1; it != edge.end();)
	{
		needPush = true;
		for (vector<Point>::iterator it1 = uniqueEdge.begin(); it1 != uniqueEdge.end();)
		{
			if ((*it).x == (*it1).x && (*it).y == (*it1).y)
			{
				needPush = false;
				break;
			}
			it1++;
		}

		if (needPush) uniqueEdge.push_back(*it);
		it++;
	}


	// 画出边缘点
	int px, py;
	for (int j = 0; j < uniqueEdge.size(); j++)
	{
		px = uniqueEdge.at(j).x;
		py = uniqueEdge.at(j).y;

		binMatBGR.data[py * binMatBGR.cols * 3 + px * 3] = 0;
		binMatBGR.data[py * binMatBGR.cols * 3 + px * 3 + 1] = 0;
		binMatBGR.data[py * binMatBGR.cols * 3 + px * 3 + 2] = 255;
	}

#endif

#if 1
	// 检查左边
	for (int j = 0; j < roi_point_approx->size(); j++)
	{
		sTemp = roi_point_approx->at(j);

		//////////////////////////////////////////////////////////////////////////
		// 走到这一步区分左边和右边的点
		bLeft = false;


		if (sTemp.x <(iTemp - iBe))           // 所有的点在左边
		{
			bLeft = true;
		}
		else
		{
			bLeft = false;
			break;
		}
	}

	// 检查右边
	for (int j = 0; j < roi_point_approx->size(); j++)
	{
		sTemp = roi_point_approx->at(j);

		//////////////////////////////////////////////////////////////////////////
		// 走到这一步区分左边和右边的点
		bRight = false;


		if (sTemp.x >(iTemp + iBe))           // 所有的点在左边
		{
			bRight = true;
		}
		else
		{
			bRight = false;
			break;
		}
	}

	if (!bRight && !bLeft)        // 查找的四个点不满足要求
	{
		return;
	}

#if 0
	// 寻找4个顶点
	int index[4] = {0};
	minX = maxX = minY = maxY = uniqueEdge.at(0);

	for (int j = 1; j < uniqueEdge.size(); j++)
	{
		//if (bRight)
		{
			if (uniqueEdge.at(j).x <= minX.x)
			{
				minX = uniqueEdge.at(j);
				index[0] = j;
			}
			else if (uniqueEdge.at(j).x >= maxX.x)
			{
				maxX = uniqueEdge.at(j);
				index[1] = j;
			}

			if (uniqueEdge.at(j).y <= minY.y)
			{
				minY = uniqueEdge.at(j);
				index[2] = j;
			}
			else if (uniqueEdge.at(j).y >= maxY.y)
			{
				maxY = uniqueEdge.at(j);
				index[3] = j;
			}
		}
		//else
		//{
		//	if (uniqueEdge.at(j).x <= minX.x)
		//	{
		//		minX = uniqueEdge.at(j);
		//		index[0] = j;
		//	}
		//	else if (uniqueEdge.at(j).x >= maxX.x)
		//	{
		//		maxX = uniqueEdge.at(j);
		//		index[1] = j;
		//	}

		//	if (uniqueEdge.at(j).y <= minY.y)
		//	{
		//		minY = uniqueEdge.at(j);
		//	}
		//	else if (uniqueEdge.at(j).y >= maxY.y)
		//	{
		//		maxY = uniqueEdge.at(j);
		//	}
		//}
	}

	// 顶点精细化
	vpMaxX.clear();
	vpMaxY.clear();
	vpMinX.clear();
	vpMinY.clear();
	for (int j = 0; j < uniqueEdge.size(); j++)
	{
		if (uniqueEdge.at(j).x == minX.x) vpMinX.push_back(uniqueEdge.at(j));
		else if (uniqueEdge.at(j).x == maxX.x) vpMaxX.push_back(uniqueEdge.at(j));

		if (uniqueEdge.at(j).y == minY.y) vpMinY.push_back(uniqueEdge.at(j));
		else if (uniqueEdge.at(j).y == maxY.y) vpMaxY.push_back(uniqueEdge.at(j));
	}
#else

	vector<Point> roi_point_contour;
	roi_point_contour.clear();
	GetRectPosFromContourPic(erodeMat, Contour, bRight, &roi_point_contour);

	//// 画出边缘点
	//int px, py;
	//Mat erodeTest = imread("tempimg/erodeTest.jpg");;
	////cvtColor(erodeMat, erodeTest, CV_GRAY2BGR);//转化边缘检测后的图为灰度图  

	//for (int j = 0; j < roi_point_contour.size(); j++)
	//{
	//	px = roi_point_contour.at(j).x;
	//	py = roi_point_contour.at(j).y;

	//	erodeTest.data[py * erodeTest.cols * 3 + px * 3] = 0;
	//	erodeTest.data[py * erodeTest.cols * 3 + px * 3 + 1] = 0;
	//	erodeTest.data[py * erodeTest.cols * 3 + px * 3 + 2] = 255;
	//}

	//imwrite("tempimg/erodeTest.jpg", erodeTest);

	vector<vector<Point>> vpVertexs;
	vector<Point> vpTemp;

	vpVertexs.clear();
	iRange = 6;
	for (int k = 0; k < roi_point_contour.size(); k++)
	{
		vpTemp.clear();
		BeginPoint = roi_point_contour.at(k);
		for (int i = (-1)*iRange; i < iRange + 1; i++)
		{
			for (int j = (-1)*iRange; j < iRange + 1; j++)
			{
				pPointBegin.y = BeginPoint.y + i;
				pPointBegin.x = BeginPoint.x + j;

				bReturn = CheckBinaryPoint(pPointBegin, *ThreshMat);

				if (bReturn)
				{
					vpTemp.push_back(pPointBegin);
				}
			}
		}
		vpVertexs.push_back(vpTemp);
	}

	vpMinX.clear();
	vpMaxX.clear();
	vpMinY.clear();
	vpMaxY.clear();


	minX = vpVertexs.at(0).at(0);
	maxX = vpVertexs.at(1).at(0);
	minY = vpVertexs.at(2).at(0);
	maxY = vpVertexs.at(3).at(0);

	for (int j = 0; j < vpVertexs.at(0).size(); j++)
	{
		if (vpVertexs.at(0).at(j).x <= minX.x)
		{
			minX = vpVertexs.at(0).at(j);
		}
	}

	for (int j = 0; j < vpVertexs.at(1).size(); j++)
	{
		if (vpVertexs.at(1).at(j).x >= maxX.x)
		{
			maxX = vpVertexs.at(1).at(j);
		}
	}


	for (int j = 0; j < vpVertexs.at(2).size(); j++)
	{
		if (vpVertexs.at(2).at(j).y <= minY.y)
		{
			minY = vpVertexs.at(2).at(j);
		}
	}


	for (int j = 0; j < vpVertexs.at(3).size(); j++)
	{
		if (vpVertexs.at(3).at(j).y >= maxY.y)
		{
			maxY = vpVertexs.at(3).at(j);
		}
	}

	// 顶点精细化
	vpMaxX.clear();
	vpMaxY.clear();
	vpMinX.clear();
	vpMinY.clear();

	for (int j = 0; j < vpVertexs.at(0).size(); j++)
	{
		if (vpVertexs.at(0).at(j).x == minX.x) vpMinX.push_back(vpVertexs.at(0).at(j));
	}

	for (int j = 0; j < vpVertexs.at(1).size(); j++)
	{
		if (vpVertexs.at(1).at(j).x == maxX.x) vpMaxX.push_back(vpVertexs.at(1).at(j));
	}

	for (int j = 0; j < vpVertexs.at(2).size(); j++)
	{
		if (vpVertexs.at(2).at(j).y == minY.y) vpMinY.push_back(vpVertexs.at(2).at(j));
	}

	for (int j = 0; j < vpVertexs.at(3).size(); j++)
	{
		if (vpVertexs.at(3).at(j).y == maxY.y) vpMaxY.push_back(vpVertexs.at(3).at(j));
	}
#endif
	Point pMinX;
	Point pMaxX;
	Point pMinY;
	Point pMaxY;

	//Point pVertex;
	//int maxIndex = index[0] > index[1] ? index[0] : index[1];
	//int minIndex = index[0] > index[1] ? index[1] : index[0];

	if (bRight)
	{
		pMaxY = vpMinX.front();
		for (int j = 1; j < vpMinX.size(); j++)
		{
			if (vpMinX.at(j).y < pMaxY.y) pMaxY = vpMinX.at(j);
		}

		pMinY = vpMaxX.front();
		for (int j = 1; j < vpMaxX.size(); j++)
		{
			if (vpMaxX.at(j).y > pMinY.y) pMinY = vpMaxX.at(j);
		}

		pMaxX = vpMinY.front();
		for (int j = 1; j < vpMinY.size(); j++)
		{
			if (vpMinY.at(j).x > pMaxX.x) pMaxX = vpMinY.at(j);
		}

		pMinX = vpMaxY.front();
		for (int j = 1; j < vpMaxY.size(); j++)
		{
			if (vpMaxY.at(j).x < pMinX.x) pMinX = vpMaxY.at(j);
		}

		/*if (index[3] > minIndex && index[3] < maxIndex)
		{
			for (int j = 0; j < minIndex; j++)
			{
				tmpDistance = PointToSegDist(uniqueEdge.at(j).x, uniqueEdge.at(j).y, pMaxY.x, pMaxY.y, pMinY.x, pMinY.y);
				if (tmpDistance - distance > 1e-6)
				{
					distance = tmpDistance;
					pVertex = uniqueEdge.at(j);
				}
			}

			for (int j = maxIndex; j < uniqueEdge.size(); j++)
			{
				tmpDistance = PointToSegDist(uniqueEdge.at(j).x, uniqueEdge.at(j).y, pMaxY.x, pMaxY.y, pMinY.x, pMinY.y);
				if (tmpDistance - distance > 1e-6)
				{
					distance = tmpDistance;
					pVertex = uniqueEdge.at(j);
				}
			}
		}
		else
		{
			for (int j = minIndex; j < maxIndex; j++)
			{
				tmpDistance = PointToSegDist(uniqueEdge.at(j).x, uniqueEdge.at(j).y, pMaxY.x, pMaxY.y, pMinY.x, pMinY.y);
				if (tmpDistance - distance > 1e-6)
				{
					distance = tmpDistance;
					pVertex = uniqueEdge.at(j);
				}
			}
		}

		roi_point_approx.clear();
		roi_point_approx.push_back(pMinX);
		roi_point_approx.push_back(pVertex);
		roi_point_approx.push_back(pMinY);
		roi_point_approx.push_back(pMaxY);*/
	}
	else
	{
		pMaxY = vpMinX.front();
		for (int j = 1; j < vpMinX.size(); j++)
		{
			if (vpMinX.at(j).y > pMaxY.y) pMaxY = vpMinX.at(j);
		}

		pMinY = vpMaxX.front();
		for (int j = 1; j < vpMaxX.size(); j++)
		{
			if (vpMaxX.at(j).y < pMinY.y) pMinY = vpMaxX.at(j);
		}

		pMinX = vpMinY.front();
		for (int j = 1; j < vpMinY.size(); j++)
		{
			if (vpMinY.at(j).x < pMinX.x) pMinX = vpMinY.at(j);
		}

		pMaxX = vpMaxY.front();
		for (int j = 1; j < vpMaxY.size(); j++)
		{
			if (vpMaxY.at(j).x > pMaxX.x) pMaxX = vpMaxY.at(j);
		}
	}

	//if (index[3] > minIndex && index[3] < maxIndex)
	//{
	//	for (int j = 0; j < minIndex; j++)
	//	{
	//		tmpDistance = PointToSegDist(uniqueEdge.at(j).x, uniqueEdge.at(j).y, pMaxY.x, pMaxY.y, pMinY.x, pMinY.y);
	//		if (tmpDistance - distance > 1e-6)
	//		{
	//			distance = tmpDistance;
	//			pVertex = uniqueEdge.at(j);
	//		}
	//	}

	//	for (int j = maxIndex; j < uniqueEdge.size(); j++)
	//	{
	//		tmpDistance = PointToSegDist(uniqueEdge.at(j).x, uniqueEdge.at(j).y, pMaxY.x, pMaxY.y, pMinY.x, pMinY.y);
	//		if (tmpDistance - distance > 1e-6)
	//		{
	//			distance = tmpDistance;
	//			pVertex = uniqueEdge.at(j);
	//		}
	//	}
	//}
	//else
	//{
	//	for (int j = minIndex; j < maxIndex; j++)
	//	{
	//		tmpDistance = PointToSegDist(uniqueEdge.at(j).x, uniqueEdge.at(j).y, pMaxY.x, pMaxY.y, pMinY.x, pMinY.y);
	//		if (tmpDistance - distance > 1e-6)
	//		{
	//			distance = tmpDistance;
	//			pVertex = uniqueEdge.at(j);
	//		}
	//	}
	//}


	roi_point_approx->clear();
	roi_point_approx->push_back(pMinX);
	roi_point_approx->push_back(pMaxX);
	roi_point_approx->push_back(pMinY);
	roi_point_approx->push_back(pMaxY);

#endif
}

// 获取正方形在图像坐标系上的位置
bool CCalibrate::OnGetRectPosition( const Mat pMat , const int iLocation , vector<vector<Point2f>> &pRect )
{
	bool bReturn = false;
#if 1
	//Mat  grayMat;
	Mat  dilate_Mat;                           // 存储膨胀后的图
	Mat  erode_Mat;                            // 存储腐蚀后的图

	vector<vector<Point> > leftCorner;          // 检测的所有的满足要求的正方形的点
	vector<vector<Point> > rightCorner;         // 检测的所有的满足要求的正方形的点

	//cvtColor( pMat , grayMat , CV_RGB2GRAY );
	//vector<vector<vector<Point2f > > > pTempRect;
	vector<vector<Point2f> > pRectLeft;
	vector<vector<Point2f> > pRectRight;

	Mat canny_Mat = pMat.clone();

	// 创建结构元素
	Mat element(3,3,CV_8U , Scalar(0) );       // 

	for(int i=0;i<3;i++)
	{
		for ( int j = 0 ; j<3 ; j++ )
		{
			element.at<uchar>(i,j)=1;              // 非十字形的
		}
		//element.at<uchar>(1,i)=1;
		//element.at<uchar>(i,1)=1;          // 十字形的

	}

	// 形态学变换（腐蚀）
	//erode(pMat, erode_Mat, element, Point(-1, -1), 1);
	

	//////////////////////////////////////////////////////////////////////////
	// 二值化
	Mat ThreshMat;
	Mat ThreshMedian;
	int iThresh = 100;
//	if ( (m_iLocation == 2)||(m_iLocation == 3 ) )
//	{
	//	for ( int i = 100 ; i< 255 ; i++ )
	//	{
	// 160是经验值，在程序检测过程中，若不能检测出正方形，那么就不断的调整门限值
	// 每个门限使用两种方案进行检测，一种是只用中值滤波后的canny；另一种是若canny检测不出来，那么就分别膨胀和腐蚀后再检测正方形
	// 若还是检测不出来，那么就改变门限值
	//		threshold( pMat/*grayMat*/ , ThreshMat , 0/*162*//*150*/ , 255 , THRESH_BINARY|THRESH_OTSU );

		//	imshow( "thresh" , ThreshMat );
		//	waitKey( 0 );
	//	}
//	}
//	else
//	{
//		threshold( grayMat , ThreshMat , 150 , 255 , THRESH_BINARY );
//	}
			//while ( true )
//			for ( iThresh = 100 ; iThresh < 180 ; iThresh = iThresh + 30 )
//			{
	//			if ( iThresh == 100 )
	//			{
				//	threshold( pMat/*grayMat*/ , ThreshMat , 0/*162*//*150*/ , 255 , THRESH_BINARY|THRESH_OTSU );
	//				threshold( pMat/*grayMat*/ , ThreshMat , 0/*162*//*150*/ , 255 , THRESH_BINARY|THRESH_OTSU );
	//			}
	//			else
		//		{
				//	threshold( pMat , ThreshMat , iThresh , 255 , THRESH_BINARY );
		//			threshold( pMat , ThreshMat , iThresh , 255 , THRESH_BINARY );
		//		}
	//////////////////////////////////////////////////////////////////////////
	// 自适应二值化
	cv::adaptiveThreshold(pMat, ThreshMat, 255, /*CV_ADAPTIVE_THRESH_GAUSSIAN_C*/CV_ADAPTIVE_THRESH_MEAN_C, CV_THRESH_BINARY_INV/*CV_THRESH_BINARY*/, 151, 0);

//	cv::adaptiveThreshold(pMat, ThreshMat, 255, CV_ADAPTIVE_THRESH_GAUSSIAN_C/*CV_ADAPTIVE_THRESH_MEAN_C*/, CV_THRESH_BINARY, 101, 0);
	//////////////////////////////////////////////////////////////////////////
	//	imshow( "binaryImage" , ThreshMat );
	//waitKey( 0 );
	std::stringstream StrStm;
	string imageFileName;
	StrStm.clear();
	StrStm << "tempimg//" << iLocation << "_binaryImage.jpg";
	StrStm >> imageFileName;
	imwrite(imageFileName, ThreshMat);

	Mat binMat = ThreshMat.clone();
	Mat binMatBGR;
	cvtColor(binMat, binMatBGR, CV_GRAY2BGR);//转化边缘检测后的图为灰度图  

	//////////////////////////////////////////////////////////////////////////
	// 中值滤波
	medianBlur( ThreshMat , ThreshMedian , 5 );
	//////////////////////////////////////////////////////////////////////////

	//imshow("ThreshMedian", ThreshMedian);
	//waitKey(0);
	//}

	//////////////////////////////////////////////////////////////////////////
#if 1
	// canny查找边缘
	double low = 0.0;
	double high = 0.0;
	//adaptivefind
	AdaptiveFindThreshold( ThreshMedian , &low , &high );         // 自动门限

	Canny( ThreshMedian , canny_Mat , low , high );

	//////////////////////////////////////////////////////////////////////////
	// 形态学变换（膨胀）
	dilate( canny_Mat , dilate_Mat , element , Point(-1 , -1) , 1 );
	//////////////////////////////////////////////////////////////////////////
	// 形态学变换（腐蚀）
	erode( dilate_Mat , erode_Mat , element , Point(-1,-1) , 1 );
	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	//imshow("canny_Mat", canny_Mat);
	//waitKey( 0 );
	//imshow("dilate_Mat", dilate_Mat);
	//waitKey( 0 );
	//imshow("erode_Mat", erode_Mat);
	//waitKey( 0 );

	StrStm.clear();
	StrStm << "tempimg//" << iLocation << "_erode.jpg";
	StrStm >> imageFileName;
	imwrite(imageFileName, erode_Mat);

	Mat erodeMat = erode_Mat.clone();

#if 0
	Mat hough_Mat = pMat.clone();
	Mat dstImage;
	cvtColor(hough_Mat, dstImage, CV_GRAY2BGR);//转化边缘检测后的图为灰度图  

	//【3】进行霍夫线变换  
	vector<Vec4i> lines;//定义一个矢量结构lines用于存放得到的线段矢量集合  
	HoughLinesP(canny_Mat, lines, 1, CV_PI / 180, 20, 10, 5);

	//【4】依次在图中绘制出每条线段  
	for (size_t i = 0; i < lines.size(); i++)
	{
		Vec4i l = lines[i];
		line(dstImage, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(0, 0, 255), 1, CV_AA);
	}

	imshow("【边缘检测后的图】", dstImage);
	waitKey(0);
#endif
#if 0/*DEBUG_SHOW_IMAGE*/
	std::stringstream StrStm;
	string imageFileName;
	StrStm.clear();
	StrStm<<"tempimg//"<<m_iLocation<<"_binary.jpg";
	StrStm>>imageFileName;
	imwrite(imageFileName,erode_Mat);
#endif

	//vector<Vec4i> lines;
	//HoughLinesP( erode_Mat , lines , 1 , CV_PI/180 , 80 , 50 , 10 );
	//for( size_t i = 0 ; i<lines.size() ; i++ )
	//{
	//	Vec4i I = lines[i];

	//	line(erode_Mat , Point(I[0] , I[1]) , Point(I[2] , I[3]) , Scalar( 0 , 0 , 255 ) , CV_AA );
	//}

//	imshow( "kkk" , erode_Mat );
//	waitKey( 0 );
#endif

#if 1
	// 使用查找面积法查找轮廓
	CvMemStorage *storage = cvCreateMemStorage(0);
	CvSeq *first_contour = NULL;
	vector<vector<Point> > contourPoint;
	vector<Vec4i> hierarchy;
	vector<vector<Point> > contourPointD;              // 存储大于5000面积的点
	double area, maxArea = 1000;      //设面积最大值大于5000Pixel 
	//double maxArea1 = 12000;
	Mat contoursImage = pMat.clone();

	//cvFindContours(&IplImage(mat)/*dsw*/, storage, &first_contour, sizeof(CvContour), /*CV_RETR_LIST*/ CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);    
	findContours( erode_Mat , contourPoint , hierarchy , CV_RETR_LIST/*CV_RETR_EXTERNAL*/ , CV_CHAIN_APPROX_NONE  );

	if( !contourPoint.empty() && !hierarchy.empty() )
	{

		for(int i=0;i<contourPoint.size();i++)
		{  
			area = fabs( contourArea( contourPoint.at(i) ) );
			if ( area > maxArea )
			{
				drawContours(contoursImage,contourPoint,i,Scalar(0),1);  

				//cv::imshow("roi_approx", contoursImage);
				//waitKey(0);

				contourPointD.push_back( contourPoint.at(i) );
			}


		}  

		//////////////////////////////////////////////////////////////////////////
		// 从Point_Connect中提取出正方形的顶点，并将其存入到cornerPoint点中
		//	bReturn = OnFindRectPointInImage( contourPointD , pRect , pMat.cols , pMat.rows , m_iLocation );

	}
	//cv::imshow("roi_approx",contoursImage );
	//waitKey( 0 );


	// 多边形拟合
#if 1
	vector<Point> roi_point_approx;
	Point xxx1;
	cv::Mat roi_approx(pMat.size(),CV_8UC3,cv::Scalar(0,0,0));

	Mat hough_Mat = pMat.clone();
	Mat dstImage;
	cvtColor(hough_Mat, dstImage, CV_GRAY2BGR);//转化边缘检测后的图为灰度图  

	auto i = contourPointD.begin();

	for ( int yyy = 0 ; yyy<contourPointD.size() ; yyy++  )
	{
		// 对图像轮廓点进行多边形拟合
		approxPolyDP( *i, roi_point_approx, 11, 1 );
		if ( roi_point_approx.size() != 4 )
		{
			i = i + 1;
			continue;
		}
		//////////////////////////////////////////////////////////////////////////
		// 检测当前的点是否是棋盘的角点，若是棋盘的角点，那么就返回为false，不继续往下执行
		bReturn = bCheckRectOnCorner( m_Points_Img , roi_point_approx );
		if ( !bReturn )
		{
			i = i + 1;
			continue;
		}
		//////////////////////////////////////////////////////////////////////////

		vector<Point> contour = *i;

		GetRectPosFromBinPic(&ThreshMat, &binMatBGR, &erodeMat, &contour, &roi_point_approx);

#if 0
		// 检查左边
		for (int j = 0; j < roi_point_approx.size(); j++)
		{
			sTemp = roi_point_approx.at(j);

			//////////////////////////////////////////////////////////////////////////
			// 走到这一步区分左边和右边的点
			bLeft = false;


			if (sTemp.x <(iTemp - iBe))           // 所有的点在左边
			{
				bLeft = true;
			}
			else
			{
				bLeft = false;
				break;
			}
		}

		// 检查右边
		for (int j = 0; j < roi_point_approx.size(); j++)
		{
			sTemp = roi_point_approx.at(j);

			//////////////////////////////////////////////////////////////////////////
			// 走到这一步区分左边和右边的点
			bRight = false;


			if (sTemp.x >(iTemp + iBe))           // 所有的点在左边
			{
				bRight = true;
			}
			else
			{
				bRight = false;
				break;
			}
		}

		if (!bRight && !bLeft)        // 查找的四个点不满足要求
		{
			i = i + 1;
			continue;
		}

		minX = maxX = minY = maxY = (*i).at(0);

		for (int j = 1; j < (*i).size(); j++)
		{
			if (bRight)
			{
				if ((*i).at(j).x <= minX.x)
				{
					minX = (*i).at(j);
				}
				else if ((*i).at(j).x >= maxX.x)
				{
					maxX = (*i).at(j);
				}

				if ((*i).at(j).y <= minY.y)
				{
					if ((*i).at(j).x >= minY.x) minY = (*i).at(j);
				}
				else if ((*i).at(j).y >= maxY.y)
				{
					maxY = (*i).at(j);
				}
			}
			else
			{
				if ((*i).at(j).x <= minX.x)
				{
					minX = (*i).at(j);
				}
				else if ((*i).at(j).x >= maxX.x)
				{
					maxX = (*i).at(j);
				}

				if ((*i).at(j).y <= minY.y)
				{
					if ((*i).at(j).x <= minY.x) minY = (*i).at(j);
				}
				else if ((*i).at(j).y >= maxY.y)
				{
					maxY = (*i).at(j);
				}
			}
		}

		for (int j = 0; j < (*i).size(); j++)
		{
			tmpDistance = PointToSegDist((*i).at(j).x, (*i).at(j).y, minX.x, minX.y, maxX.x, maxX.y);
			if (tmpDistance - distance > 1e-6)
			{
				distance = tmpDistance;
				pVertex = (*i).at(j);
			}
		}

		near.clear();
		pTemp.x = minX.x - 1;
		pTemp.y = minX.y - 1;
		near.push_back(pTemp);

		pTemp.x = minX.x;
		pTemp.y = minX.y - 1;
		near.push_back(pTemp);

		pTemp.x = minX.x + 1;
		pTemp.y = minX.y - 1;
		near.push_back(pTemp);


		pTemp.x = minX.x - 1;
		pTemp.y = minX.y;
		near.push_back(pTemp);

		pTemp.x = minX.x;
		pTemp.y = minX.y;
		near.push_back(pTemp);

		pTemp.x = minX.x + 1;
		pTemp.y = minX.y;
		near.push_back(pTemp);


		pTemp.x = minX.x - 1;
		pTemp.y = minX.y + 1;
		near.push_back(pTemp);

		pTemp.x = minX.x;
		pTemp.y = minX.y + 1;
		near.push_back(pTemp);

		pTemp.x = minX.x + 1;
		pTemp.y = minX.y + 1;
		near.push_back(pTemp);
		
		roi_point_approx.clear();
		roi_point_approx.push_back(minX);
		roi_point_approx.push_back(maxX);
		roi_point_approx.push_back(minY);
		roi_point_approx.push_back(maxY);

#endif
#if 0
		int iRandom = 0;
		vector<Point> edgePoint;       // 二值化的边缘
		bool bCheck;
//		int itemp = (*i).size();
		int iEnd = 0;
		while ( true )
		{
			iRandom = rand() % (*i).size();

			bCheck = ergodicBinaryMat((*i).at(iRandom), ThreshMat, edgePoint);

			if (bCheck||(iEnd==40))               // 遍历成功,edgePoint存放了二值化的边缘点
			{
				break;
			}

			iEnd = iEnd + 1;
		}

		if (!bCheck)
		{
			i = i + 1;
			continue;
		}
#endif
		//////////////////////////////////////////////////////////////////////////
		// 将当前四个点传进去，看看是否为满足要求的当前四边形的形状
		bReturn = CheckRect( roi_point_approx , pMat.cols , leftCorner , rightCorner );
		if ( !bReturn )        // 查找的四个点不满足要求
		{
			i = i + 1;
			continue;
		}

		//////////////////////////////////////////////////////////////////////////
		// 进一步检查leftCorner和rightCorner里面的点，提取出正方形的四个顶点
		//	OnDetermineRectInImg( leftCorner , rightCorner , dst );

#if 1
		//////////////////////////////////////////////////////////////////////////
		for ( int xxx = 0 ; xxx<roi_point_approx.size() ; xxx++ )
		{
			xxx1 = roi_point_approx.at(xxx);
			cv::circle(dstImage, xxx1, 2, cv::Scalar(0, 0, 255));
		}
#endif

		i = i + 1;
	}

	//imshow("roi_approx", dstImage);
	//waitKey(0);

	//imshow("binTest", binMatBGR);
	//waitKey(0);
	//int value1 = binMatBGR.data[553 * binMatBGR.cols * 3 + 311 * 3];
	//int value2 = binMatBGR.data[553 * binMatBGR.cols * 3 + 311 * 3 + 1];
	//int value3 = binMatBGR.data[553 * binMatBGR.cols * 3 + 311 * 3 + 2];
	//imwrite("binTest.jpg", binMatBGR);
	//imwrite("binOrig.jpg", binMat);

	if ( (leftCorner.size()!=0)&&(rightCorner.size()!=0 ) )
	{
		// 进一步检查leftCorner和rightCorner里面的点，提取出正方形的四个顶点
		pRect.clear();
		OnDetermineRectInImg( leftCorner , rightCorner , pRect );

		if ( pRect.size() == 2 )        // rect的0表示左，1表示右
		{
			bReturn = true;
			// 将pRect入临时表内
			//pTempRect.push_back( pRect );
#if 0
			pRectLeft.push_back( pRect.at(0) );
			pRectRight.push_back( pRect.at(1) );

			pRect.clear();
#endif
		}
		else
		{
			bReturn = false;
		}
		//bReturn = false;
					
	}
	//else
	//{
	//	bReturn = false;
	//}

	//if ( iThresh > 220 )
	//{
	//	break;
	//}
				

	//////////////////////////////////////////////////////////////////////////
	// 手动改变门限值
	//threshold( pMat/*grayMat*/ , ThreshMat , iThresh , 255 , THRESH_BINARY );
	//iThresh = iThresh + 5;             // 每次以5增加

	//cout<<iThresh<<endl;
	//if ( iThresh == 155 )
	//{
	//	int kkk = 30;
	//}
				
	//imshow( "bitine" , ThreshMat );
	//waitKey( 0 );
//			}

	#if 0
	if ( (pRectLeft.size()!= 0)||(pRectRight.size() != 0 )  )
	{
		int iMin;
		vector<Point2f> pTemp;

		iMin = iGetMinDis2f( pRectLeft , xMin );
		pTemp = pRectLeft.at(iMin);                         // 直接取距离的最小值就当做是黑色的正方形
	//				iGetAverage( pRectLeft , iMin , pTemp );            // 如果有多个正方形的话，那么就求这多个正方形的和，下同
		pRect.push_back( pTemp );
		pTemp.clear();
		//pRect.push_back( pRectLeft.at(iMin) );

		iMin = iGetMinDis2f( pRectRight , xMax );
		pTemp.clear();
		pTemp = pRectRight.at(iMin);
	//				iGetAverage( pRectRight , iMin , pTemp );
		pRect.push_back( pTemp );
		//pRect.push_back( pRectRight.at(iMin) );

		bReturn = true;
	}
	else
	{
		bReturn = false;
	}
#endif
			

	
#if 0
	cv::imshow("roi_approx",roi_approx);
		waitKey( 0 );
#endif

	return bReturn;

#endif
#if 0
	Mat contoursImage(erode_Mat.rows,erode_Mat.cols,CV_8U,Scalar(255));  
	if( !contourPoint.empty() && !hierarchy.empty() )
	{
		double area, maxArea = 1500;      //设面积最大值大于5000Pixel  
		for(int i=0;i<contourPoint.size();i++)
		{  
			area = fabs( contourArea( contourPoint.at(i) ) );
			if ( area > maxArea )
			{
				drawContours(contoursImage,contourPoint,i,Scalar(0),1); 


			}


		} 	
		imshow( "zhou" , contoursImage );
		cvWaitKey( 0 ); 
	}
#endif


#endif

#endif

#if 0
	vector<Point2f> vPtemp;
	Point2f         pTemp;

	vPtemp.clear();

	if ( m_iLocation == 0 )
	{
		pTemp.x = 261;
		pTemp.y = 289;
		vPtemp.push_back( pTemp );

		pTemp.x = 368;
		pTemp.y = 289;
		vPtemp.push_back( pTemp );

		pTemp.x = 154;
		pTemp.y = 362;
		vPtemp.push_back( pTemp );

		pTemp.x = 246;
		pTemp.y = 387;
		vPtemp.push_back( pTemp );

		pRect.push_back( vPtemp );

		vPtemp.clear();
		pTemp.x = 888;
		pTemp.y = 320;
		vPtemp.push_back( pTemp );

		pTemp.x = 988;
		pTemp.y = 334;
		vPtemp.push_back( pTemp );

		pTemp.x = 990;
		pTemp.y = 433;
		vPtemp.push_back( pTemp );

		pTemp.x = 1076;
		pTemp.y = 419;
		vPtemp.push_back( pTemp );

		pRect.push_back( vPtemp );
	}

	if ( m_iLocation == 1 )
	{
		pTemp.x = 1121;
		pTemp.y = 439;
		vPtemp.push_back( pTemp );

		pTemp.x = 1032;
		pTemp.y = 458;
		vPtemp.push_back( pTemp );

		pTemp.x = 1030;
		pTemp.y = 336;
		vPtemp.push_back( pTemp );

		pTemp.x = 927;
		pTemp.y = 324;
		vPtemp.push_back( pTemp );

		pRect.push_back( vPtemp );

		vPtemp.clear();
		pTemp.x = 281;
		pTemp.y = 454;
		vPtemp.push_back( pTemp );

		pTemp.x = 192;
		pTemp.y = 433;
		vPtemp.push_back( pTemp );

		pTemp.x = 390;
		pTemp.y = 323;
		vPtemp.push_back( pTemp );

		pTemp.x = 285;
		pTemp.y = 331;
		vPtemp.push_back( pTemp );

		pRect.push_back( vPtemp );
	}

	if ( m_iLocation == 2 )
	{
		pTemp.x = 1023;
		pTemp.y = 327;
		vPtemp.push_back( pTemp );

		pTemp.x = 1089;
		pTemp.y = 410;
		vPtemp.push_back( pTemp );

		pTemp.x = 969;
		pTemp.y = 323;
		vPtemp.push_back( pTemp );

		pTemp.x = 1050;
		pTemp.y = 429;
		vPtemp.push_back( pTemp );

		pRect.push_back( vPtemp );

		vPtemp.clear();
		pTemp.x = 119;
		pTemp.y = 322;
		vPtemp.push_back( pTemp );

		pTemp.x = 60;
		pTemp.y = 412;
		vPtemp.push_back( pTemp );

		pTemp.x = 90;
		pTemp.y = 324;
		vPtemp.push_back( pTemp );

		pTemp.x = 41;
		pTemp.y = 398;
		vPtemp.push_back( pTemp );

		pRect.push_back( vPtemp );
	}
	if ( m_iLocation == 3 )
	{
		pTemp.x = 116;
		pTemp.y = 487;
		vPtemp.push_back( pTemp );

		pTemp.x = 171;
		pTemp.y = 385;
		vPtemp.push_back( pTemp );

		pTemp.x = 153;
		pTemp.y = 497;
		vPtemp.push_back( pTemp );

		pTemp.x = 222;
		pTemp.y = 371;
		vPtemp.push_back( pTemp );

		pRect.push_back( vPtemp );

		vPtemp.clear();
		pTemp.x = 1140;
		pTemp.y = 486;
		vPtemp.push_back( pTemp );

		pTemp.x = 1089;
		pTemp.y = 378;
		vPtemp.push_back( pTemp );

		pTemp.x = 1159;
		pTemp.y = 482;
		vPtemp.push_back( pTemp );

		pTemp.x = 1119;
		pTemp.y = 391;
		vPtemp.push_back( pTemp );

		pRect.push_back( vPtemp );
	}
#endif
}

// 内部有对正方形的检测，查找看看是否找到了正方形的顶点在图像上的坐标值
bool CCalibrate::OnFindRectPointInImage( vector<vector<Point> >&source , vector<vector<Point2f> >&dst , int imageWide , int imageHeight , int iLocation )
{
	// Point的X代表宽，Y代表高
	//////////////////////////////////////////////////////////////////////////
	// 执行到这一步，棋盘区域的正方形肯定已经提取出来了，现在的目的就是怎样才能将棋盘区域的正方形准确的提取出来
	bool bReturn = true;
	vector<vector<Point> > leftSource;          // 左图的一堆点，包含了正方形
	vector<vector<Point> > leftCorner;          // 检测的所有的满足要求的正方形的点
	bool bLeft;

	vector<vector<Point> > rightSource;         // 右图的一堆点，包含了正方形
	vector<vector<Point> > rightCorner;         // 检测的所有的满足要求的正方形的点
	bool bRight;

	vector<Point> sTemp;
	Point         pTemp;
	int           iTemp = imageWide/2;                     // 中间的分界线
	int           iBe = 150;                               // 从中间分界线向两边移动300个像素，经验值

	int iRemove = 1000;                  // 大于1000个点的被剔除
	int iRemovex = 100;                   // 小于300的点剔除
	// 第一步，以图像正中间的那条竖线为准，提取出左边和正方形和右边的正方形
	int i = 0;
	int j = 0;
	for ( i = 0 ; i<source.size() ; i++ )
	{
		sTemp = source.at( i );

		if ( (sTemp.size() > iRemove)||(sTemp.size() < iRemovex ) )
		{
			continue;
		}

		//////////////////////////////////////////////////////////////////////////
		// 走到这一步区分左边和右边的点
		bLeft = false;
		bRight = false;
		for ( j = 0 ;  j<sTemp.size() ; j++ )
		{
			pTemp = sTemp.at( j );


			if ( pTemp.x <(iTemp-iBe) )           // 所有的点在左边
			{
				bLeft = true;
			}
			else
			{
				bLeft = false;
				break;
			}
	//		if ( pTemp.x > (iTemp+iBe) )          // 所有的点在右边
	//		{
	//			bRight = true;
	//		}
		}

		//////////////////////////////////////////////////////////////////////////
		// bLeft和bRight中只有一个为true，另一个为FALSE,如果都为true的话，那就说明此点跨越了中间
		if ( bLeft&&(j == sTemp.size() ) )            // 左边的点
		{
			leftSource.push_back( sTemp );
		}

		for ( j = 0 ; j<sTemp.size() ; j++ )
		{
			pTemp = sTemp.at( j );

			if ( pTemp.x >(iTemp + iBe ))
			{
				bRight = true;
			}
			else
			{
				bRight = false;
				break;
			}
		}

		if ( bRight&&(j == sTemp.size() ) )              // 右边
		{
			rightSource.push_back( sTemp );
		}

	}

	//////////////////////////////////////////////////////////////////////////
	// 将leftSource代入检测函数中，检测其是否为满足要求的鱼眼镜头拍摄的点
	OnCheckRect( leftSource , leftCorner , true );            // true表示左
	OnCheckRect( rightSource , rightCorner , false );         // false表示右

	if ( (leftCorner.size() == 0)||( rightCorner.size() == 0 ) )
	{
		bReturn = false;
		return bReturn;
	}

#if 0
	Mat contoursImage( imageHeight ,imageWide,CV_8U,Scalar(255));  
//	if( !leftSource.empty() )
//	{
		//	double area, maxArea = 2000;      //设面积最大值大于5000Pixel  
		for(int i=0;i<leftSource.size();i++)
		{  
			//	area = fabs( contourArea( contourPoint.at(i) ) );
			//	if ( area > maxArea )
			//	{
			drawContours(contoursImage,leftSource ,i,Scalar(0),1); 


			//	}


		} 	
		imshow( "zhou" , contoursImage );
		cvWaitKey( 0 ); 
//	}
#endif

	//////////////////////////////////////////////////////////////////////////
	// 进一步检查leftCorner和rightCorner里面的点，提取出正方形的四个顶点
	OnDetermineRectInImg( leftCorner , rightCorner , dst );

	int kkk;
	kkk = 300;
	return bReturn;
}

int CCalibrate::iGetMinDis2f( vector<vector<Point2f> >&pPoint , float fThreath )
{
	float fTemp = 0;

	vector<Point2f> PointTemp;

	Point2f pTemp;
	int iLeftIndex = 0;
	float fMin;

	PointTemp = pPoint.at(0);


	for ( int i = 0 ; i<PointTemp.size() ; i++ )
	{
		pTemp = PointTemp.at(i);

		fTemp = fTemp + (pTemp.x - fThreath )*( pTemp.x - fThreath );
	}

	fMin = fTemp;              // 第一个点的值看成是最大值

	for ( int i = 1 ; i<pPoint.size() ; i++ )
	{
		PointTemp = pPoint.at(i);
		fTemp = 0;

		for ( int j = 0 ; j<PointTemp.size() ; j++ )
		{
			pTemp = PointTemp.at(j);

			fTemp = fTemp + (pTemp.x - fThreath )*( pTemp.x - fThreath );
		}

		if ( fTemp < fMin )
		{
			iLeftIndex = i;
			fMin = fTemp;
		}

	}

	return iLeftIndex;
}

int CCalibrate::iGetMinDis( vector<vector<Point> >&pPoint , float fThreath )
{
	float fTemp = 0;

	vector<Point> PointTemp;

	Point pTemp;
	int iLeftIndex = 0;
	float fMin;

	PointTemp = pPoint.at(0);
	

	for ( int i = 0 ; i<PointTemp.size() ; i++ )
	{
		pTemp = PointTemp.at(i);

		fTemp = fTemp + (pTemp.x - fThreath )*( pTemp.x - fThreath );
	}

	fMin = fTemp;              // 第一个点的值看成是最大值

	for ( int i = 1 ; i<pPoint.size() ; i++ )
	{
		PointTemp = pPoint.at(i);
		fTemp = 0;

		for ( int j = 0 ; j<PointTemp.size() ; j++ )
		{
			pTemp = PointTemp.at(j);

			fTemp = fTemp + (pTemp.x - fThreath )*( pTemp.x - fThreath );
		}

		if ( fTemp < fMin )
		{
			iLeftIndex = i;
			fMin = fTemp;
		}

	}

	return iLeftIndex;

}
void CCalibrate::SetCalibrate( calibrateScheme calibrates )
{
	m_CalibrateScheme = calibrates;            // 设置标定方案
}

// 根据找出的四个顶点，进一步确定每幅图像的四个顶点，共8个顶点
void CCalibrate::OnDetermineRectInImg( vector<vector<Point> >&leftPoint , vector<vector<Point> >&rightPoint , vector<vector<Point2f> >&dst )
{
	int iLeftCount = leftPoint.size();
	int iRightCount = rightPoint.size();
	int i;
	int j;
	int z;

	vector<Point> leftPointTemp;
	int iLeftIndex = 0;

	vector<Point> rightPointTemp;
	int iRightIndex = 0;

	float fTemp;
	float fMin;

	int iTemp = 0;

	Point pTempleft;
	Point pTempRight;
	Point2f fpTemp;
	vector<vector<Point> > leftPoint1;
	vector<vector<Point> > rightPoint1;
	if ( (m_iLocation == 0)||(m_iLocation == 1 ) )
	{
		iTemp = m_cornerDimension.height;
	}
	else
	{
		iTemp = m_cornerDimension.width;
	}
	float yDivision ;//= yMin - (yMax - yMin)/iTemp;
	float yDivision1 ;//= yMax + (yMax - yMin)/iTemp;

	if ( (m_iLocation == TOP)||(m_iLocation == BOTTOM) )
	{
		yDivision = yMin + (yMax - yMin)/iTemp;
		
	}
	else
	{
		//yDivision = yMin - 1.5*((yMax - yMin)/iTemp);
		if ( m_CalibrateScheme == BAOWO_BX7 )
		{
			yDivision = yMin + (yMax - yMin)/iTemp;
		}
		else
		{
			yDivision = yMin - 1.5*((yMax - yMin)/iTemp);
		}
		
		
	//	yDivision1 = yMax;// + (yMax - yMin)/iTemp;
	}
yDivision1 = yMax - (yMax - yMin)/iTemp;
	

	iTemp = 0;

	for ( i = 0 ; i<iLeftCount ; i++ )
	{
		leftPointTemp = leftPoint.at(i);
	
		// 第0和第1个点的y肯定小于yMax
		// 第2和第3个点的y肯定大于ymin
		if ( ((leftPointTemp.at(2).y > yDivision )&&(leftPointTemp.at(3).y > yDivision ))&&((leftPointTemp.at(0).y < yDivision1)&&(leftPointTemp.at(1).y <yDivision1)) )
		{
			leftPoint1.push_back( leftPointTemp );
		}
	}

	for ( i = 0 ; i<iRightCount ; i++ )
	{
		rightPointTemp = rightPoint.at(i);

		// 第0和第1个点的y肯定小于yMax
		// 第2和第3个点的y肯定大于ymin
		if ( ((rightPointTemp.at(2).y > yDivision/*yMin*/)&&(rightPointTemp.at(3).y > yDivision ))&&((rightPointTemp.at(0).y < yDivision1)&&(rightPointTemp.at(1).y <yDivision1)) )
		{
			rightPoint1.push_back( rightPointTemp );
		}
	}

	leftPoint.clear();
	rightPoint.clear();

	if ( (leftPoint1.size() == 0)||(rightPoint1.size() == 0 ) )
	{
		return;
	}

	//////////////////////////////////////////////////////////////////////////
	// 左边找离xmin点最近的点
	iLeftIndex = iGetMinDis( leftPoint1 , xMin );
	//fTemp = 0;
	//
	//leftPointTemp = leftPoint1.at(0);
	//iLeftIndex = 0;

	//for ( i = 0 ; i<leftPointTemp.size() ; i++ )
	//{
	//	pTempleft = leftPointTemp.at(i);

	//	fTemp = fTemp + (pTempleft.x - xMin )*( pTempleft.x - xMin );
	//}
	//
	//fMin = fTemp;              // 第一个点的值看成是最大值

	//for ( i = 1 ; i<leftPoint1.size() ; i++ )
	//{
	//	leftPointTemp = leftPoint1.at(i);
	//	fTemp = 0;

	//	for ( j = 0 ; j<leftPointTemp.size() ; j++ )
	//	{
	//		pTempleft = leftPointTemp.at(j);

	//		fTemp = fTemp + (pTempleft.x - xMin )*( pTempleft.x - xMin );
	//	}

	//	if ( fTemp < fMin )
	//	{
	//		iLeftIndex = i;
	//		fMin = fTemp;
	//	}

	//}

	//////////////////////////////////////////////////////////////////////////
	// 右边找离xmax点最近的点
	iRightIndex = iGetMinDis( rightPoint1 , xMax );
	//fTemp = 0;

	//rightPointTemp = rightPoint1.at(0);
	//iRightIndex = 0;

	//for ( i = 0 ; i<rightPointTemp.size() ; i++ )
	//{
	//	pTempRight = rightPointTemp.at(i);

	//	fTemp = fTemp + (pTempRight.x - xMax )*( pTempRight.x - xMax );
	//}

	//fMin = fTemp;              // 第一个点的值看成是最大值

	//for ( i = 1 ; i<rightPoint1.size() ; i++ )
	//{
	//	rightPointTemp = rightPoint1.at(i);
	//	fTemp = 0;

	//	for ( j = 0 ; j<rightPointTemp.size() ; j++ )
	//	{
	//		pTempRight = rightPointTemp.at(j);

	//		fTemp = fTemp + (pTempRight.x - xMax )*( pTempRight.x - xMax );
	//	}

	//	if ( fTemp < fMin )
	//	{
	//		iRightIndex = i;
	//		fMin = fTemp;
	//	}

	//}

	vector<Point2f> vTempp;
	vTempp.clear();
	for ( i = 0 ; i<leftPoint1.at(iLeftIndex).size() ; i++ )
	{
		fpTemp.x = leftPoint1.at(iLeftIndex).at(i).x;
		fpTemp.y = leftPoint1.at(iLeftIndex).at(i).y;
		vTempp.push_back( fpTemp );
	}
	dst.push_back( vTempp );

	vTempp.clear();
	for ( j = 0 ; j<rightPoint1.at(iRightIndex).size() ; j++)
	{
		fpTemp.x = rightPoint1.at(iRightIndex).at(j).x;
		fpTemp.y = rightPoint1.at(iRightIndex).at(j).y;
		vTempp.push_back( fpTemp );
	}
	dst.push_back( vTempp );

//	dst.push_back( leftPoint1.at(iLeftIndex) );
//	dst.push_back( rightPoint1.at(iRightIndex) );
	//leftPointTemp.clear();
	//rightPointTemp.clear();

//	for ( i = 0 ; i<leftPoint1.size() ; i++ )
//	{
		
//	}
	//////////////////////////////////////////////////////////////////////////
	// 右边找离xmax点最近的点



#if 0
	for ( i = 0 ; i<leftPoint1.size() ; i++ )
	{
		leftPointTemp = leftPoint1.at(i);

		for ( j = 0 ; j<rightPoint1.size() ; j++ )
		{
			rightPointTemp = rightPoint1.at(j);

			fTemp = 0;

			if ( (i == 0)&&(j == 0) )
			{
				// 左右只有四个点
				for ( z = 0 ; z<rightPointTemp.size() ; z++ )
				{
					if ( z == 0 )
					{
						iTemp = 1;
					}
					if ( z == 1 )
					{
						iTemp = 0;
					}
					if ( z == 2 )
					{
						iTemp = 3;
					}
					if ( z == 3 )
					{
						iTemp = 2;
					}
					pTempleft = leftPointTemp.at( z );
					pTempRight = rightPointTemp.at( iTemp );

					// 左右两个正方形差不多在同一个y上
					fTemp = fTemp + abs(pTempRight.y - pTempleft.y);
				}

				iLeftIndex = i;
				iRightIndex = j;
				fMax = fTemp;
			}

			else
			{
				// 左右只有四个点
				for ( z = 0 ; z<rightPointTemp.size() ; z++ )
				{
					if ( z == 0 )
					{
						iTemp = 1;
					}
					if ( z == 1 )
					{
						iTemp = 0;
					}
					if ( z == 2 )
					{
						iTemp = 3;
					}
					if ( z == 3 )
					{
						iTemp = 2;
					}

					pTempleft = leftPointTemp.at( z );
					pTempRight = rightPointTemp.at( iTemp );

					fTemp = fTemp + abs(pTempRight.y - pTempleft.y);
				}

				// 选取左右两点的y值差绝对值最小的
				if ( fMax > fTemp )
				{
					iLeftIndex = i;
					iRightIndex = j;
					fMax = fTemp;
				}

			}
		}
	}

	// 将iLeftIndex和iRightIndex中的值取出来放入到dst中
	// dst中的前4个元素是本幅图像左边的正方形点，存储格式为左上，右上，左下和右下。
	// dst中的后4个元素是本幅图像右边的正方形点，存储格式同上
	vector<Point2f> vTempp;
	vTempp.clear();
	for ( i = 0 ; i<leftPoint1.at(iLeftIndex).size() ; i++ )
	{
		fpTemp.x = leftPoint1.at(iLeftIndex).at(i).x;
		fpTemp.y = leftPoint1.at(iLeftIndex).at(i).y;
		vTempp.push_back( fpTemp );
	}
	dst.push_back( vTempp );

	vTempp.clear();
	for ( j = 0 ; j<rightPoint1.at(iRightIndex).size() ; j++)
	{
		fpTemp.x = rightPoint1.at(iRightIndex).at(j).x;
		fpTemp.y = rightPoint1.at(iRightIndex).at(j).y;
		vTempp.push_back( fpTemp );
	}
	dst.push_back( vTempp );

#endif

}

void CCalibrate::OnCheckRect( vector<vector<Point> >&source , vector<vector<Point> >&cornerLocation , bool bLeft )
{
	int i;
	int j;
	vector<Point> pTemp;

	Point pTopLeft;         // 左上
	int   iTopLeft_index = 0;

	Point pTopRight;        // 右上
	int   iTopRight_index = 0;

	Point pBottomRight;     // 右下
	int   iBottomRight_index = 0;

	Point pBottomLeft;      // 左下
	int   iBottomLeft_index = 0;

	float kParam;           // 直线的斜率
	float bParam;           // 直线的偏移量

	float d1;               // 前一半的最大距离
	int d1_index;           // 前一半的最大距离的下标值
	//
	//float d2;               // 后一半的最大的距离
	//int d2_index;           // 后一半的最大距离的下标值

	float dTemp;
	vector<Point> pLocation;

	int iTemp1;
	int iTemp2;

	for ( i = 0 ; i<source.size() ; i++ )
	{
		pTemp = source.at(i);

		pTopLeft = pTemp.at(0);
		pTopRight = pTemp.at(0);
		pBottomLeft = pTemp.at(0);
		pBottomRight = pTemp.at(0);

		for ( j = 1 ; j<pTemp.size() ; j++ )
		{
#if 0
			if ( (i == 4)&&(j == 393) )
			{
				int kkk;
				kkk = 300;
			}
#endif
			if ( bLeft )                           // 图的左边正方形框
			{
				// x值最小，左下角
				if ( pBottomLeft.x >= pTemp.at(j).x )
				{
					// 如果x值相等的话，那么就选y值最大的
					if ( pBottomLeft.x == pTemp.at(j).x )             // 如果x值相等的话，那么就先y值最大的
					{
						if ( pBottomLeft.y < pTemp.at(j).y )
						{
							pBottomLeft = pTemp.at(j);
							iBottomLeft_index = j;
						}
					}
					else
					{
						pBottomLeft = pTemp.at(j);
						iBottomLeft_index = j;
					}

				}

				// x值最大，右上角
				if ( pTopRight.x <= pTemp.at(j).x )
				{
					// 如果x值相等的话，那么就选y值最小的
					if ( pTopRight.x == pTemp.at(j).x )
					{
						if ( pTopRight.y > pTemp.at(j).y )
						{
							pTopRight = pTemp.at( j );
							iTopRight_index = j;
						}
					}
					else
					{
						pTopRight = pTemp.at(j);
						iTopRight_index = j;
					}
				}

				// y值最大，右下角
				if ( pBottomRight.y <= pTemp.at(j).y )
				{
					// 如果y值相等的话，那么就选x值最大的
					if ( pBottomRight.y == pTemp.at(j).y  )
					{
						if ( pBottomRight.x < pTemp.at(j).x )
						{
							pBottomRight = pTemp.at(j);
							iBottomRight_index = j;
						}
					}
					else
					{
						pBottomRight = pTemp.at( j );
						iBottomRight_index = j;
					}
				}
			}
			else                                   // 图的右边正方形框
			{
				// x最小，左上角
				if ( pTopLeft.x >= pTemp.at(j).x )
				{
					// 如果x值相等的话，那么就取y值最小的那个点
					if ( pTopLeft.x == pTemp.at(j).x )
					{
						if ( pTopLeft.y > pTemp.at(j).y )
						{
							pTopLeft = pTemp.at(j);
							iTopLeft_index = j;
						}
					}
					else
					{
						pTopLeft = pTemp.at( j );
						iTopLeft_index = j;
					}
				}

				// x最大，右下角
				if ( pBottomRight.x <= pTemp.at(j).x )
				{
					// 如果x值相等的话，那么就取y值最大的那个点
					if ( pBottomRight.x == pTemp.at(j).x )
					{
						if ( pBottomRight.y < pBottomRight.y )
						{
							pBottomRight = pTemp.at(j);
							iBottomRight_index = j;
						}
					}
					else
					{
						pBottomRight = pTemp.at( j );
						iBottomRight_index = j;
					}
				}

				// y最大，左下角
				if ( pBottomLeft.y <= pTemp.at(j).y )
				{
					// 如果y值相等的话，那么就取x值最小的那个点
					if ( pBottomLeft.y == pTemp.at(j).y )
					{
						if ( pBottomLeft.x > pTemp.at(j).x )
						{
							pBottomLeft = pTemp.at( j );
							iBottomLeft_index = j;
						}
					}
					else
					{
						pBottomLeft = pTemp.at(j);
						iBottomLeft_index = j;
					}
				}
			}
		}

		//////////////////////////////////////////////////////////////////////////
		// 到这里，已经找到了3个点了，现在找第四个点
		// 方法是：如果是左边的正方形，那么就过左下角到右上角，做一条直线，然后求每个点到这条线的距离
		// 如果是右边的正方形，那么就过左上角到右下角，做一条直线，然后求每个点到这条线的距离
		if ( bLeft )      // 如果是左边的那个正方形，那么就从左下角到右上角
		{
			kParam = (float)(pBottomLeft.y - pTopRight.y)/(float)( pBottomLeft.x - pTopRight.x );
			bParam = pTopRight.y - kParam*pTopRight.x;

			// 左边的点，判断右下点的index是否在左下和右上的点的index之间
			// 顺时针和逆时针都要考虑到
			// ||符号的前面代表是逆时针。符号的后面代表的是顺时针
			if ( ((iBottomRight_index<iTopRight_index)&&( iBottomRight_index > iBottomLeft_index ))||( (iBottomRight_index >iTopRight_index)&&( iBottomRight_index < iBottomLeft_index ) ) )        // 如果在这之间的话，那么就寻找之外的区域
			{
				// 如果是顺时针方向（iTopleft_index<iBottomLeft_index）
				// 此if生成两段
				d1 = abs( (kParam*(pTemp.at(0).x) - pTemp.at(0).y + bParam )/sqrt(kParam*kParam + 1) );            // 都是从0开始

				d1_index = 0;

				if ( iTopRight_index < iBottomLeft_index )
				{
					for ( j = 1 ; j<iTopRight_index ; j++ )
					{
						dTemp = abs( (kParam*(pTemp.at( j ).x) - pTemp.at( j ).y + bParam )/sqrt(kParam*kParam + 1) );
						if ( d1 < dTemp )
						{
							d1 = dTemp;
							d1_index = j;
						}
					}

					for ( j = iBottomLeft_index ; j< pTemp.size() ; j++ )
					{
						dTemp = abs( (kParam*(pTemp.at( j ).x) - pTemp.at( j ).y + bParam )/sqrt(kParam*kParam + 1) );
						if ( d1 < dTemp )
						{
							d1 = dTemp;
							d1_index = j;
						}
					}
				}
				// 如果是逆时针方向（iTopRight_index>iBottomLeft_index）
				else
				{
					for ( j = 1 ; j<iBottomLeft_index ; j++ )
					{
						dTemp = abs( (kParam*(pTemp.at( j ).x) - pTemp.at( j ).y + bParam )/sqrt(kParam*kParam + 1) );
						if ( d1 < dTemp )
						{
							d1 = dTemp;
							d1_index = j;
						}
					}

					for ( j = iTopRight_index ; j< pTemp.size() ; j++ )
					{
						dTemp = abs( (kParam*(pTemp.at( j ).x) - pTemp.at( j ).y + bParam )/sqrt(kParam*kParam + 1) );
						if ( d1 < dTemp )
						{
							d1 = dTemp;
							d1_index = j;
						}
					}
				}
			}
			else
			{
				// 顺时针方向
				if ( iTopRight_index < iBottomLeft_index )
				{
					d1 = abs( (kParam*(pTemp.at(iTopRight_index).x) - pTemp.at( iTopRight_index ).y + bParam )/sqrt(kParam*kParam + 1) );            // 都是从0开始

					d1_index = iTopRight_index;

					for ( j = iTopRight_index + 1 ; j< iBottomLeft_index ; j++ )
					{
						dTemp = abs( (kParam*(pTemp.at( j ).x) - pTemp.at( j ).y + bParam )/sqrt(kParam*kParam + 1) );
						if ( d1 < dTemp )
						{
							d1 = dTemp;
							d1_index = j;
						}
					}

				}
				else                      // 逆
				{
					d1 = abs( (kParam*(pTemp.at(iBottomLeft_index).x) - pTemp.at( iBottomLeft_index ).y + bParam )/sqrt(kParam*kParam + 1) );            // 都是从0开始

					d1_index = iBottomLeft_index;

					for ( j = iBottomLeft_index + 1 ; j< iTopRight_index ; j++ )
					{
						dTemp = abs( (kParam*(pTemp.at( j ).x) - pTemp.at( j ).y + bParam )/sqrt(kParam*kParam + 1) );
						if ( d1 < dTemp )
						{
							d1 = dTemp;
							d1_index = j;
						}
					}

				}
			}

			pTopLeft = pTemp.at( d1_index );
		}
		else              // 如果是右边的那个正方形，那么就从左上角到右下角
		{
			kParam = (float)(pTopLeft.y - pBottomRight.y)/(float)( pTopLeft.x - pBottomRight.x );
			bParam = pBottomRight.y - kParam*pBottomRight.x;

			// 左边的点，判断右下点的index是否在左下和右上的点的index之间
			// 顺时针和逆时针都要考虑到
			// ||符号的前面代表是逆时针。符号的后面代表的是顺时针
			if ( ((iBottomLeft_index < iBottomRight_index)&&( iBottomLeft_index > iTopLeft_index ))||( (iBottomLeft_index >iBottomRight_index )&&( iBottomLeft_index < iTopLeft_index ) ) )        // 如果在这之间的话，那么就寻找之外的区域
			{
				// 如果是顺时针方向（iTopleft_index<iBottomLeft_index）
				// 此if生成两段
				d1 = abs( (kParam*(pTemp.at(0).x) - pTemp.at(0).y + bParam )/sqrt(kParam*kParam + 1) );            // 都是从0开始

				d1_index = 0;

				if ( iBottomRight_index < iTopLeft_index )
				{
					for ( j = 1 ; j<iBottomRight_index ; j++ )
					{
						dTemp = abs( (kParam*(pTemp.at( j ).x) - pTemp.at( j ).y + bParam )/sqrt(kParam*kParam + 1) );
						if ( d1 < dTemp )
						{
							d1 = dTemp;
							d1_index = j;
						}
					}

					for ( j = iTopLeft_index ; j< pTemp.size() ; j++ )
					{
						dTemp = abs( (kParam*(pTemp.at( j ).x) - pTemp.at( j ).y + bParam )/sqrt(kParam*kParam + 1) );
						if ( d1 < dTemp )
						{
							d1 = dTemp;
							d1_index = j;
						}
					}
				}
				// 如果是逆时针方向（iTopRight_index>iBottomLeft_index）
				else
				{
					for ( j = 1 ; j<iTopLeft_index ; j++ )
					{
						dTemp = abs( (kParam*(pTemp.at( j ).x) - pTemp.at( j ).y + bParam )/sqrt(kParam*kParam + 1) );
						if ( d1 < dTemp )
						{
							d1 = dTemp;
							d1_index = j;
						}
					}

					for ( j = iBottomRight_index ; j< pTemp.size() ; j++ )
					{
						dTemp = abs( (kParam*(pTemp.at( j ).x) - pTemp.at( j ).y + bParam )/sqrt(kParam*kParam + 1) );
						if ( d1 < dTemp )
						{
							d1 = dTemp;
							d1_index = j;
						}
					}
				}
			}
			else
			{
				// 顺时针方向
				if ( iBottomRight_index < iTopLeft_index )
				{
					d1 = abs( (kParam*(pTemp.at(iTopRight_index).x) - pTemp.at( iTopRight_index ).y + bParam )/sqrt(kParam*kParam + 1) );            // 都是从0开始

					d1_index = iBottomRight_index;

					for ( j = iBottomRight_index + 1 ; j< iTopLeft_index ; j++ )
					{
						dTemp = abs( (kParam*(pTemp.at( j ).x) - pTemp.at( j ).y + bParam )/sqrt(kParam*kParam + 1) );
						if ( d1 < dTemp )
						{
							d1 = dTemp;
							d1_index = j;
						}
					}

				}
				else                      // 逆
				{
					d1 = abs( (kParam*(pTemp.at(iBottomLeft_index).x) - pTemp.at( iBottomLeft_index ).y + bParam )/sqrt(kParam*kParam + 1) );            // 都是从0开始

					d1_index = iTopLeft_index;

					for ( j = iTopLeft_index + 1 ; j< iBottomRight_index ; j++ )
					{
						dTemp = abs( (kParam*(pTemp.at( j ).x) - pTemp.at( j ).y + bParam )/sqrt(kParam*kParam + 1) );
						if ( d1 < dTemp )
						{
							d1 = dTemp;
							d1_index = j;
						}
					}

				}

			}

			pTopRight = pTemp.at( d1_index );
		}

		pLocation.clear();
		//////////////////////////////////////////////////////////////////////////
		// 在这里检测正方形的左上和右上，左下和右下的差值不能大于20个像素点
		iTemp1 = abs(pTopLeft.y - pTopRight.y);
		iTemp2 = abs(pBottomLeft.y - pBottomRight.y);

	//	if ( (iTemp1<30)&&(iTemp2<30) )                 // 正方形的Y值之差不能大于20个像素点
	//	{
			pLocation.push_back( pTopLeft );
			pLocation.push_back( pTopRight );
			pLocation.push_back( pBottomLeft );
			pLocation.push_back( pBottomRight );

			cornerLocation.push_back( pLocation );
	//	}
		//////////////////////////////////////////////////////////////////////////

	}
}

// 进行标定
void CCalibrate::OnCalibrate_private( const vector<vector<Point3f> >&position_World , const vector<vector<Point3f> >&Rect_World , const vector<Point2f>&position_Img , const vector<vector<Point2f> >&Rect_Img , CvSize imgSize , int&iProcess )
{
	vector<vector<Point3f> > worldPoint;
	vector<vector<Point2f> > imgPoint;

	vector<Point3f> worldPoint_Temp;
	vector<Point2f> imgPoint_Temp;

	Point3f         wTemp;
	Point2f         iTemp;

	int i;
	int j;


	worldPoint.clear();
	imgPoint.clear();
	worldPoint_Temp.clear();
	imgPoint_Temp.clear();
	// 填充世界坐标系
	//worldPoint_Temp = position_World;
	//worldPoint.push_back( position_World );
	worldPoint = position_World;
	for ( i = 0 ; i< Rect_World.size() ; i++ )
	{
		//worldPoint_Temp.clear();
		for ( j = 0 ; j<Rect_World.at(i).size() ; j++ )
		{
			wTemp = Rect_World.at(i).at(j);
			worldPoint_Temp.push_back( wTemp );
		}
		//worldPoint_Temp.push_back( position_World.at(0) );
		//worldPoint.push_back( worldPoint_Temp );
	}

	iProcess++;
	// 填充图像坐标系
	//imgPoint_Temp = position_Img;
	imgPoint.push_back( position_Img );
	for ( i = 0 ; i<Rect_Img.size() ; i++ )
	{
		//imgPoint_Temp.clear();
		for ( j = 0 ; j<Rect_Img.at(i).size(); j++ )
		{
			iTemp = Rect_Img.at(i).at(j);
			imgPoint_Temp.push_back( iTemp );
		}

		//imgPoint_Temp.push_back( position_Img.at(0) );

		//imgPoint.push_back( imgPoint_Temp );
	}

	iProcess++;

	// 标定内参和畸变系数
	worldPoint.push_back( worldPoint_Temp );
	imgPoint.push_back( imgPoint_Temp );
	OnCalibrateInnerParam( worldPoint , imgPoint , m_fishInf.innerParam , m_fishInf.distortionParam ,  imgSize );

	iProcess++;

#if 0
	m_fishInf.distortionParam.val[0] = -0.0274947;
	m_fishInf.distortionParam.val[1] = 0.0145664;
	m_fishInf.distortionParam.val[2] = -0.0139627;
	m_fishInf.distortionParam.val[3] = 0.00337951;
	//m_fishInf.distortionParam.val[0] = -0.0235;
	//m_fishInf.distortionParam.val[1] = -0.0057;
	//m_fishInf.distortionParam.val[2] = 0.0032;
	//m_fishInf.distortionParam.val[3] = -8.2553e-04;
#endif
	//////////////////////////////////////////////////////////////////////////
	// 标定外参
	//m_onBrate.InitExtrinsics()
	Vec4d distortion_coeffs;
	for ( i = 0 ; i<4 ; i++ )          // 固定为4个畸变系数
	{
		distortion_coeffs.val[i] = m_fishInf.distortionParam.val[i];
	}
	Vec2d _c( m_fishInf.innerParam.val[2] , m_fishInf.innerParam.val[6] );		// 表示图像的中心点
	Vec2d _f( m_fishInf.innerParam.val[0] , m_fishInf.innerParam.val[5] );		// 表示图像的fx和fy
	int flags = 0;
	IntrinsicParams IntriParams;
	flags |= cv::fisheye::CALIB_RECOMPUTE_EXTRINSIC;
	flags |= cv::fisheye::CALIB_FIX_SKEW;	// 求解时假设内参中fx=fy

	IntriParams.Init( _f , _c , distortion_coeffs );
	IntriParams.isEstimate[0] = 1;
	IntriParams.isEstimate[1] = 1;
	IntriParams.isEstimate[2] = 1;
	IntriParams.isEstimate[3] = 1;
	IntriParams.isEstimate[4] = flags & cv::fisheye::CALIB_FIX_SKEW ? 0 : 1;
	IntriParams.isEstimate[5] = flags & cv::fisheye::CALIB_FIX_K1 ? 0 : 1;
	IntriParams.isEstimate[6] = flags & cv::fisheye::CALIB_FIX_K2 ? 0 : 1;
	IntriParams.isEstimate[7] = flags & cv::fisheye::CALIB_FIX_K3 ? 0 : 1;
	IntriParams.isEstimate[8] = flags & cv::fisheye::CALIB_FIX_K4 ? 0 : 1;

	const int check_cond = flags & cv::fisheye::CALIB_CHECK_COND ? 1 : 0;
	double thresh_cond = 1e6;

	/*CCalibrateFisheye onBrate;*/
	std::vector<Vec3d> omc, Tc,omc1,Tc1 , TempOmc[2];
	Mat rotation_matrix1 = Mat(3,3,CV_64FC1, Scalar::all(0)); /* 保存每幅图像的旋转矩阵 */  

	iProcess++;

	// 标定外参
	m_onBrate.OnCalibrateExtrinsics( worldPoint , imgPoint , IntriParams , check_cond , thresh_cond , omc1 , Tc1 );
	iProcess++;
	//////////////////////////////////////////////////////////////////////////
	// 进行外参优化
	for ( i = 0 ; i<Rect_World.size() ; i++ )
	{
		worldPoint_Temp.clear();
		imgPoint_Temp.clear();
		worldPoint.clear();
		imgPoint.clear();
	//	omc = omc1;
	//	Tc = Tc1;
		omc.clear();
		Tc.clear();
		omc.push_back( omc1.at(1) );
		Tc.push_back( Tc1.at(1) );

		worldPoint_Temp = Rect_World.at( i );
		imgPoint_Temp = Rect_Img.at( i );

		worldPoint.push_back( worldPoint_Temp );
		imgPoint.push_back( imgPoint_Temp );

		m_onBrate.OnOptimizeExtrinsics( worldPoint , imgPoint , IntriParams , check_cond , thresh_cond , omc , Tc );

		Rodrigues( omc , rotation_matrix1 );
		TempOmc[i] = omc;

		double* dTemp = (double*)rotation_matrix1.data;
		// 构造外参矩阵
		m_fishInf.RT[i].val[0] = dTemp[0];
		m_fishInf.RT[i].val[1] = dTemp[1];
		m_fishInf.RT[i].val[2] = dTemp[2];
		m_fishInf.RT[i].val[3] = Tc.at(0).val[0];

		m_fishInf.RT[i].val[4] = dTemp[3];
		m_fishInf.RT[i].val[5] = dTemp[4];
		m_fishInf.RT[i].val[6] = dTemp[5];
		m_fishInf.RT[i].val[7] = Tc.at(0).val[1];

		m_fishInf.RT[i].val[8] = dTemp[6];
		m_fishInf.RT[i].val[9] = dTemp[7];
		m_fishInf.RT[i].val[10] = dTemp[8];
		m_fishInf.RT[i].val[11] = Tc.at(0).val[2];

		m_fishInf.RT[i].val[12] = 0;
		m_fishInf.RT[i].val[13] = 0;
		m_fishInf.RT[i].val[14] = 0;
		m_fishInf.RT[i].val[15] = 1;

		iProcess++;

	}

#if DEBUG_SHOW_IMAGE
{
	string temp;
	std::stringstream StrStm;
	string storegePath;

	StrStm.clear();
	StrStm << "tempimg//" << m_iLocation << "_Param.txt";
	StrStm >> storegePath;
	ofstream foutmm(storegePath);

	foutmm << "相机内参数矩阵ss：" << endl;
	foutmm << m_fishInf.innerParam << endl;
	foutmm << "畸变系数ss：\n";
	foutmm << m_fishInf.distortionParam/*distortion_coeffs*/ << endl;

	for (j = 0; j < 2; j++)
	{
		foutmm << "第" << j + 1 << "个外参矩阵:" << endl;
		foutmm << m_fishInf.RT[j] << endl;

		foutmm << "旋转矩阵:" << endl;
		for (i = 0; i < TempOmc[j].size(); i++)
		{
			foutmm << TempOmc[j].at(i);
			foutmm << ' ';
		}
		foutmm << endl;
	}
}

#endif
}

// innerParam是计算出来的内部参数
bool CCalibrate::OnCalibrateInnerParam(vector<vector<Point3f> >&worldPoint , vector<vector<Point2f> >&imgPoint , Matx34d&innerParam/*Matx33d&innerParam*/ , Matx14d&distor , Size image_size )
{
	bool bReturn = true;

	cv::Matx33d m_intrinsic_matrix;               // 摄像机内参矩阵
	cv::Vec4d   m_distortion_coeffs;              // 摄像机的4个畸变系数：k1,k2,k3,k4
	std::vector<cv::Vec3d> m_rotation_vectors;    // 每幅图像的旋转向量
	std::vector<cv::Vec3d> m_translation_vectors; // 每幅图像的平衡向量

	double rms = 0;

	int flags = 0;
	flags |= cv::fisheye::CALIB_RECOMPUTE_EXTRINSIC;
	flags |= cv::fisheye::CALIB_FIX_SKEW;
	innerParam.zeros();

	rms = fisheye::calibrate( worldPoint , imgPoint , image_size , m_intrinsic_matrix , m_distortion_coeffs , m_rotation_vectors , m_translation_vectors , flags , cv::TermCriteria(2 , 50 , 1e-6) );

	//	Vec2d _c(m_intrinsic_matrix.val[2] , m_intrinsic_matrix.val[5] );
	//	Vec2d _f(425 , 423);
	//innerParam.val[0] = 425;
	//innerParam.val[2] = m_intrinsic_matrix.val[2];
	//innerParam.val[5] = 423;
	//innerParam.val[6] = m_intrinsic_matrix.val[5];
	//innerParam.val[10] = 1;

	//innerParam = m_intrinsic_matrix;
	innerParam.val[0] = m_intrinsic_matrix.val[0];
	innerParam.val[1] = m_intrinsic_matrix.val[1];
	innerParam.val[2] = m_intrinsic_matrix.val[2];
	innerParam.val[3] = 0;

	innerParam.val[4] = m_intrinsic_matrix.val[3];
	innerParam.val[5] = m_intrinsic_matrix.val[4];
	innerParam.val[6] = m_intrinsic_matrix.val[5];
	innerParam.val[7] = 0;

	innerParam.val[8] = m_intrinsic_matrix.val[6];
	innerParam.val[9] = m_intrinsic_matrix.val[7];
	innerParam.val[10] = m_intrinsic_matrix.val[8];

	distor.val[0] = m_distortion_coeffs.val[0];
	distor.val[1] = m_distortion_coeffs.val[1];
	distor.val[2] = m_distortion_coeffs.val[2];
	distor.val[3] = m_distortion_coeffs.val[3];

	return bReturn;
}

// 判断第一个角点所在的方位，以及角点的存储方式（行/列）
void CCalibrate::OriginLocationAndErgodicMode( vector<Point2f> cornerLocation ,int CheckLen , OriginInImage&Location , ArrayModeInImage&ArrayMode )
{
	Point2f pFist;
	Point2f pLast;
	int iTempx;
	int iTempy;

	pFist = cornerLocation.at( 0 );
	pLast = cornerLocation.at( cornerLocation.size() - 1 );
	//////////////////////////////////////////////////////////////////////////
	// 判断原点的位置
	if ( pFist.x < pLast.x )                // 原点在左边
	{
		if ( pFist.y < pLast.y )            // 左上
		{
			Location = LEFT_TOP;
		}
		else								// 左下
		{
			Location = LEFT_BOTTOM;
		}
	}
	else                                   // 原点在右边
	{
		if ( pFist.y < pLast.y )           // 右上
		{
			Location = RIGHT_TOP;
		}
		else                               // 右下
		{
			Location = RIGHT_BOTTOM;
		}
	}

	//////////////////////////////////////////////////////////////////////////
	// 判断角点是列存储还是行存储的
	pLast = cornerLocation.at( CheckLen - 1 );
	iTempx = abs(pLast.x - pFist.x);
	iTempy = abs( pLast.y - pFist.y );

	ArrayMode = ( iTempx>iTempy?ROW_MODE:COLOMN_MODE );
}

void CCalibrate::OnReSort_Top( OriginInImage originLocation , ArrayModeInImage arrayMode , vector<Point2f>&source , CvSize cornerSize , vector<Point2f>&dst )
{
	int i;
	int j;
	int iTemp;

	// 这里本身图像的原点应该在左上角,以行排列
	//OriginInImage ImageOrigin = LEFT_TOP;
	//ArrayModeInImage ImageMode = ROW_MODE;
	// 每副图像又有8种情况

	Point2f fPoint;
	if ( originLocation == LEFT_TOP )              // 原点在左上角
	{
		iTemp = 0;
		for ( i = 0 ; i<cornerSize.height ; i++ )
		{
			for ( j = 0 ; j<cornerSize.width ; j++ )
			{
				if ( arrayMode == ROW_MODE )
				{
					iTemp = i*cornerSize.width + j;
				}
				else
				{
					iTemp = j*cornerSize.height + i;
				}
				fPoint = source.at( iTemp );
				dst.push_back( fPoint );
			}
		}
	}
	else if ( originLocation == RIGHT_TOP )        // 原点在右上角
	{
		iTemp = 0;
		for ( i = 0 ; i<cornerSize.height ; i++ )
		{
			for ( j = cornerSize.width - 1 ; j>=0 ; j-- )
			{
				if ( arrayMode == ROW_MODE )
				{
					iTemp = i*cornerSize.width + j;
				}
				else
				{
					iTemp = j*cornerSize.height + i;
				}
				fPoint = source.at( iTemp );
				dst.push_back( fPoint );
			}
		}
	}
	else if ( originLocation == LEFT_BOTTOM )      //原点在左下角
	{
		iTemp = 0;
		for ( i = cornerSize.height - 1 ; i>=0 ; i-- )
		{
			for ( j = 0 ; j<cornerSize.width ; j++ )
			{
				if ( arrayMode == ROW_MODE )            // 行排列
				{
					iTemp = i*cornerSize.width + j;
				}
				else                                   // 列排列
				{
					iTemp = j*cornerSize.height + i;
				}

				fPoint = source.at( iTemp );
				dst.push_back( fPoint );
			}
		}
	}
	else if ( originLocation == RIGHT_BOTTOM )     // 原点在右下角
	{
		iTemp = 0;
		for ( i = cornerSize.height - 1 ; i>=0 ; i-- )
		{
			for ( j = cornerSize.width - 1 ; j>=0 ; j-- )
			{
				if ( arrayMode == ROW_MODE )
				{
					iTemp = i*cornerSize.width + j;
				}
				else
				{
					iTemp = j*cornerSize.height + i;
				}
				fPoint = source.at( iTemp );
				dst.push_back( fPoint );
			}
		}
	}
}

void CCalibrate::OnResort_Bottom( OriginInImage originLocation , ArrayModeInImage arrayMode , vector<Point2f>&source   , CvSize cornerSize , vector<Point2f>&dst )
{
	int i;
	int j;
	int iTemp;

	// 这里本身图像的原点应该在右下角,以行排列
	//OriginInImage ImageOrigin = RIGHT_BOTTOM;
	//ArrayModeInImage ImageMode = ROW_MODE;
	// 每副图像又有8种情况

	Point2f fPoint;
	if ( originLocation == LEFT_TOP )              // 原点在左上角
	{
		iTemp = 0;
		for ( i = cornerSize.height - 1 ; i>=0 ; i-- )
		{
			for ( j = cornerSize.width - 1; j>=0 ; j-- )
			{
				if ( arrayMode == ROW_MODE )
				{
					iTemp = i*cornerSize.width + j;
				}
				else
				{
					iTemp = j*cornerSize.height + i;
				}
				fPoint = source.at( iTemp );
				dst.push_back( fPoint );
			}
		}
	}
	else if ( originLocation == RIGHT_TOP )
	{
		iTemp = 0;
		for ( i = cornerSize.height - 1; i>=0 ; i-- )
		{
			for ( j = 0 ; j<cornerSize.width ; j++ )
			{
				if ( arrayMode == ROW_MODE )
				{
					iTemp = i*cornerSize.width + j;
				}
				else
				{
					iTemp = j*cornerSize.height + i;
				}
				fPoint = source.at( iTemp );
				dst.push_back( fPoint );
			}
		}
	}
	else if ( originLocation == LEFT_BOTTOM )
	{
		iTemp = 0;
		for ( i = 0 ; i<cornerSize.height ; i++ )
		{
			for ( j = cornerSize.width - 1 ; j>=0 ; j-- )
			{
				if ( arrayMode == ROW_MODE )
				{
					iTemp = i*cornerSize.width + j;
				}
				else
				{
					iTemp = j*cornerSize.height + i;
				}
				fPoint = source.at( iTemp );
				dst.push_back( fPoint );
			}
		}
	}
	else if ( originLocation == RIGHT_BOTTOM )
	{
		iTemp = 0;
		for ( i = 0 ; i<cornerSize.height ; i++ )
		{
			for ( j = 0 ; j<cornerSize.width ; j++ )
			{
				if ( arrayMode == ROW_MODE )
				{
					iTemp = i*cornerSize.width + j;
				}
				else
				{
					iTemp = j*cornerSize.height + i;
				}
				fPoint = source.at( iTemp );
				dst.push_back( fPoint );
			}
		}
	}
}

void CCalibrate::OnResort_Left( OriginInImage originLocation , ArrayModeInImage arrayMode , vector<Point2f>&source  , CvSize cornerSize , vector<Point2f>&dst )
{
	int i;
	int j;
	int iTemp;

	// 这里本身图像的原点应该在右上角,以行排列
	//OriginInImage ImageOrigin = RIGHT_TOP;
	//ArrayModeInImage ImageMode = COLOMN_MODE;
	// 每副图像又有8种情况

	Point2f fPoint;
	if ( originLocation == LEFT_TOP )              // 原点在左上角
	{
		iTemp = 0;
		for ( i = cornerSize.height - 1 ; i>=0 ; i-- )
		{
			for ( j = 0 ; j<cornerSize.width ; j++ )
			{
				if ( arrayMode == ROW_MODE )
				{
					iTemp = j*cornerSize.height + i;
				}
				else
				{
					iTemp = i*cornerSize.width + j;
				}
				fPoint = source.at( iTemp );
				dst.push_back( fPoint );
			}
		}
	}
	else if ( originLocation == RIGHT_TOP )
	{
		iTemp = 0;
		for ( i = 0;i<cornerSize.height ; i++ )
		{
			for ( j = 0 ; j<cornerSize.width ; j++ )
			{
				if ( arrayMode == ROW_MODE )
				{
					iTemp = j*cornerSize.height + i;
				}
				else
				{
					iTemp = i*cornerSize.width + j;
				}
				fPoint = source.at( iTemp );
				dst.push_back( fPoint );
			}
		}
	}
	else if( originLocation == LEFT_BOTTOM )
	{
		iTemp = 0;
		for ( i = cornerSize.height - 1;i>=0 ; i-- )
		{
			for ( j = cornerSize.width - 1 ; j>=0 ; j-- )
			{
				if ( arrayMode == ROW_MODE )
				{
					iTemp = j*cornerSize.height + i;
				}
				else
				{
					iTemp = i*cornerSize.width + j;
				}
				fPoint = source.at( iTemp );
				dst.push_back( fPoint );
			}
		}
	}
	else if ( originLocation == RIGHT_BOTTOM )
	{
		for ( i = 0 ; i<cornerSize.height ; i++ )
		{
			for ( j = cornerSize.width - 1 ; j>=0;j-- )
			{
				if ( arrayMode == ROW_MODE )
				{
					iTemp = j*cornerSize.height + i;
				}
				else
				{
					iTemp = i*cornerSize.width + j;
				}
				fPoint = source.at( iTemp );
				dst.push_back( fPoint );
			}
		}
	}
}

void CCalibrate::OnResort_Right( OriginInImage originLocation , ArrayModeInImage arrayMode , vector<Point2f>&source  , CvSize cornerSize , vector<Point2f>&dst )
{
	int i;
	int j;
	int iTemp = 0;

	// 这里本身图像的原点应该在左下角,以行排列
	//OriginInImage ImageOrigin = LEFT_BOTTOM;
	//ArrayModeInImage ImageMode = COLOMN_MODE;
	// 每副图像又有8种情况

	Point2f fPoint;
	if ( originLocation == LEFT_TOP )              // 原点在左上角
	{
		for ( i = 0 ; i<cornerSize.height ; i++ )
		{
			for ( j = cornerSize.width - 1 ; j>=0 ; j-- )
			{
				if ( arrayMode == ROW_MODE )
				{
					iTemp = j*cornerSize.height + i;
				}
				else
				{
					iTemp = i*cornerSize.width + j;
				}
				fPoint = source.at( iTemp );
				dst.push_back( fPoint );
			}
		}
	}
	else if ( originLocation == RIGHT_TOP )
	{
		for ( i = cornerSize.height - 1 ; i>=0 ; i-- )
		{
			for ( j = cornerSize.width - 1 ; j>=0 ; j-- )
			{
				if ( arrayMode == ROW_MODE )
				{
					iTemp = j*cornerSize.height + i;
				}
				else
				{
					iTemp = i*cornerSize.width + j;
				}
				fPoint = source.at( iTemp );
				dst.push_back( fPoint );
			}
		}
	}
	else if ( originLocation == LEFT_BOTTOM )
	{
		for ( i = 0 ; i<cornerSize.height ; i++ )
		{
			for ( j = 0 ; j<cornerSize.width ; j++ )
			{
				if ( arrayMode == ROW_MODE )
				{
					iTemp = j*cornerSize.height + i;
				}
				else
				{
					iTemp = i*cornerSize.width + j;
				}
				fPoint = source.at( iTemp );
				dst.push_back( fPoint );
			}
		}
	}

	else if ( originLocation == RIGHT_BOTTOM )
	{
		for ( i = cornerSize.height - 1 ; i>=0 ; i-- )
		{
			for ( j =0 ; j<cornerSize.width ; j++ )
			{
				if ( arrayMode == ROW_MODE )
				{
					iTemp = j*cornerSize.height + i;
				}
				else
				{
					iTemp = i*cornerSize.width + j;
				}
				fPoint = source.at( iTemp );
				dst.push_back( fPoint );
			}
		}
	}

}

void CCalibrate::TopFishEye()
{
	m_fishInf.innerParam.val[0] = 409.8972272139037;
	m_fishInf.innerParam.val[1] = 0;
	m_fishInf.innerParam.val[2] = 589.7017375569486;
	m_fishInf.innerParam.val[3] = 0;

	m_fishInf.innerParam.val[4] = 0;
	m_fishInf.innerParam.val[5] = 386.7165777092637;
	m_fishInf.innerParam.val[6] = 332.1156186989055;
	m_fishInf.innerParam.val[7] = 0;

	m_fishInf.innerParam.val[8] = 0;
	m_fishInf.innerParam.val[9] = 0;
	m_fishInf.innerParam.val[10] = 1;
	m_fishInf.innerParam.val[11] = 0;

	m_fishInf.distortionParam.val[0] = -0.00717087;
	m_fishInf.distortionParam.val[1] = 0.0096037;
	m_fishInf.distortionParam.val[2] = -0.0165054;
	m_fishInf.distortionParam.val[3] = 0.00430257;

	m_fishInf.RT[0].val[0] = 0.9957974858517287;
	m_fishInf.RT[0].val[1] = -0.0893764401170096;
	m_fishInf.RT[0].val[2] = -0.0199804685477444;
	m_fishInf.RT[0].val[3] = -2431.93;

	m_fishInf.RT[0].val[4] = 0.06018322464710833;
	m_fishInf.RT[0].val[5] = 0.4741745064653454;
	m_fishInf.RT[0].val[6] = 0.8783715141609625;
	m_fishInf.RT[0].val[7] = -365.5;

	m_fishInf.RT[0].val[8] = -0.06903149022332125;
	m_fishInf.RT[0].val[9] = -0.8758826344724258;
	m_fishInf.RT[0].val[10] = 0.4775607437668959;
	m_fishInf.RT[0].val[11] = 2235.5;

	m_fishInf.RT[0].val[12] = 0;
	m_fishInf.RT[0].val[13] = 0;
	m_fishInf.RT[0].val[14] = 0;
	m_fishInf.RT[0].val[15] = 1;

	m_fishInf.RT[1].val[0] = 0.9963624512732778;
	m_fishInf.RT[1].val[1] = -0.07952277607960541;
	m_fishInf.RT[1].val[2] = -0.03062668407285043;
	m_fishInf.RT[1].val[3] = -2389.27;

	m_fishInf.RT[1].val[4] = 0.0644914105247522;
	m_fishInf.RT[1].val[5] = 0.4687376910594861;
	m_fishInf.RT[1].val[6] = 0.8809800423101251;
	m_fishInf.RT[1].val[7] = -377.426;

	m_fishInf.RT[1].val[8] = -0.05570209745811313;
	m_fishInf.RT[1].val[9] = -0.8797505925345062;
	m_fishInf.RT[1].val[10] = 0.4721611708664237;
	m_fishInf.RT[1].val[11] = 2189.5;

	m_fishInf.RT[1].val[12] = 0;
	m_fishInf.RT[1].val[13] = 0;
	m_fishInf.RT[1].val[14] = 0;
	m_fishInf.RT[1].val[15] = 1;
}

void CCalibrate::BottomFishEye()
{
	m_fishInf.innerParam.val[0] = 420.9886564091485;
	m_fishInf.innerParam.val[1] = 0;
	m_fishInf.innerParam.val[2] = 646.7567005517021;
	m_fishInf.innerParam.val[3] = 0;

	m_fishInf.innerParam.val[4] = 0;
	m_fishInf.innerParam.val[5] = 424.1572437340512;
	m_fishInf.innerParam.val[6] = 382.5751663738684;
	m_fishInf.innerParam.val[7] = 0;

	m_fishInf.innerParam.val[8] = 0;
	m_fishInf.innerParam.val[9] = 0;
	m_fishInf.innerParam.val[10] = 1;
	m_fishInf.innerParam.val[11] = 0;

	m_fishInf.distortionParam.val[0] = -0.0162402;
	m_fishInf.distortionParam.val[1] = -0.0170192;
	m_fishInf.distortionParam.val[2] = 0.0107508;
	m_fishInf.distortionParam.val[3] = -0.00305117;

	m_fishInf.RT[0].val[0] = -0.9997221733437676;
	m_fishInf.RT[0].val[1] = 0.01977903371212515;
	m_fishInf.RT[0].val[2] = 0.01282052846915778;
	m_fishInf.RT[0].val[3] = 2481.17;

	m_fishInf.RT[0].val[4] = -0.001572732607902484;
	m_fishInf.RT[0].val[5] = -0.5986819506305645;
	m_fishInf.RT[0].val[6] = 0.8009852985550523;
	m_fishInf.RT[0].val[7] = 4697.71;

	m_fishInf.RT[0].val[8] = 0.02351813421506708;
	m_fishInf.RT[0].val[9] = 0.8007426002246894;
	m_fishInf.RT[0].val[10] = 0.5985467279573452;
	m_fishInf.RT[0].val[11] = -4839.65;

	m_fishInf.RT[0].val[12] = 0;
	m_fishInf.RT[0].val[13] = 0;
	m_fishInf.RT[0].val[14] = 0;
	m_fishInf.RT[0].val[15] = 1;

	m_fishInf.RT[1].val[0] = -0.9994407660381962;
	m_fishInf.RT[1].val[1] = 0.03027118978095952;
	m_fishInf.RT[1].val[2] = 0.01420599346151834;
	m_fishInf.RT[1].val[3] = 2361.37;

	m_fishInf.RT[1].val[4] = -0.006641639124729809;
	m_fishInf.RT[1].val[5] = -0.5960758803847431;
	m_fishInf.RT[1].val[6] = 0.8029006373476674;
	m_fishInf.RT[1].val[7] = 4687.67;

	m_fishInf.RT[1].val[8] = 0.03277260762771903;
	m_fishInf.RT[1].val[9] = 0.8023572769613289;
	m_fishInf.RT[1].val[10] = 0.5959435848270213;
	m_fishInf.RT[1].val[11] = -4888.31;

	m_fishInf.RT[1].val[12] = 0;
	m_fishInf.RT[1].val[13] = 0;
	m_fishInf.RT[1].val[14] = 0;
	m_fishInf.RT[1].val[15] = 1;
}

void CCalibrate::LeftFisheye()
{
	m_fishInf.innerParam.val[0] = 414.6588835512559;
	m_fishInf.innerParam.val[1] = 0;
	m_fishInf.innerParam.val[2] = 613.2710138780921;
	m_fishInf.innerParam.val[3] = 0;

	m_fishInf.innerParam.val[4] = 0;
	m_fishInf.innerParam.val[5] = 413.9149751591133;
	m_fishInf.innerParam.val[6] = 337.033488186057;
	m_fishInf.innerParam.val[7] = 0;

	m_fishInf.innerParam.val[8] = 0;
	m_fishInf.innerParam.val[9] = 0;
	m_fishInf.innerParam.val[10] = 1;
	m_fishInf.innerParam.val[11] = 0;

	m_fishInf.distortionParam.val[0] = -0.0140115;
	m_fishInf.distortionParam.val[1] = 0.00303879;
	m_fishInf.distortionParam.val[2] = -0.00724362;
	m_fishInf.distortionParam.val[3] = 0.00176118;

	m_fishInf.RT[0].val[0] = 0.08843446145571196;
	m_fishInf.RT[0].val[1] = -0.9927806859439128;
	m_fishInf.RT[0].val[2] = -0.08102996756615682;
	m_fishInf.RT[0].val[3] = 3573.69;

	m_fishInf.RT[0].val[4] = 0.641031798079625;
	m_fishInf.RT[0].val[5] = -0.005540738498654191;
	m_fishInf.RT[0].val[6] = 0.7674943218471996;
	m_fishInf.RT[0].val[7] = -212.598;

	m_fishInf.RT[0].val[8] = -0.7624025051623593;
	m_fishInf.RT[0].val[9] = -0.119815732830141;
	m_fishInf.RT[0].val[10] = 0.6359139959841544;
	m_fishInf.RT[0].val[11] = 2381.6;

	m_fishInf.RT[0].val[12] = 0;
	m_fishInf.RT[0].val[13] = 0;
	m_fishInf.RT[0].val[14] = 0;
	m_fishInf.RT[0].val[15] = 1;

	m_fishInf.RT[1].val[0] = 0.07435845263048413;
	m_fishInf.RT[1].val[1] = -0.9957012134554774;
	m_fishInf.RT[1].val[2] = -0.05522602688669553;
	m_fishInf.RT[1].val[3] = 3429.41;

	m_fishInf.RT[1].val[4] = 0.6373919722684472;
	m_fishInf.RT[1].val[5] = 0.004863387279004205;
	m_fishInf.RT[1].val[6] = 0.7705243806343272;
	m_fishInf.RT[1].val[7] = -268.038;

	m_fishInf.RT[1].val[8] = -0.766943475237999;
	m_fishInf.RT[1].val[9] = -0.09249562685589186;
	m_fishInf.RT[1].val[10] = 0.6350135941870825;
	m_fishInf.RT[1].val[11] = 2224.83;

	m_fishInf.RT[1].val[12] = 0;
	m_fishInf.RT[1].val[13] = 0;
	m_fishInf.RT[1].val[14] = 0;
	m_fishInf.RT[1].val[15] = 1;
}

void CCalibrate::RightFisheye()
{
	m_fishInf.innerParam.val[0] = 421.8997320735303;
	m_fishInf.innerParam.val[1] = 0;
	m_fishInf.innerParam.val[2] = 591.4066607244006;
	m_fishInf.innerParam.val[3] = 0;

	m_fishInf.innerParam.val[4] = 0;
	m_fishInf.innerParam.val[5] = 424.4088558694213;
	m_fishInf.innerParam.val[6] = 455.9922967514847;
	m_fishInf.innerParam.val[7] = 0;

	m_fishInf.innerParam.val[8] = 0;
	m_fishInf.innerParam.val[9] = 0;
	m_fishInf.innerParam.val[10] = 1;
	m_fishInf.innerParam.val[11] = 0;

	m_fishInf.distortionParam.val[0] = -0.0457764;
	m_fishInf.distortionParam.val[1] = 0.0460327;
	m_fishInf.distortionParam.val[2] = -0.0350901;
	m_fishInf.distortionParam.val[3] = 0.00779205;

	m_fishInf.RT[0].val[0] = 0.04906943037921636;
	m_fishInf.RT[0].val[1] = 0.9952155398426825;
	m_fishInf.RT[0].val[2] = 0.08448798883804207;
	m_fishInf.RT[0].val[3] = -3860.01;

	m_fishInf.RT[0].val[4] = -0.8174400664509474;
	m_fishInf.RT[0].val[5] = -0.008590606176231391;
	m_fishInf.RT[0].val[6] = 0.5759495978349108;
	m_fishInf.RT[0].val[7] = 3527.77;

	m_fishInf.RT[0].val[8] = 0.5739197929701762;
	m_fishInf.RT[0].val[9] = -0.0973253859029739;
	m_fishInf.RT[0].val[10] = 0.813107520870338;
	m_fishInf.RT[0].val[11] = -817.57;

	m_fishInf.RT[0].val[12] = 0;
	m_fishInf.RT[0].val[13] = 0;
	m_fishInf.RT[0].val[14] = 0;
	m_fishInf.RT[0].val[15] = 1;

	m_fishInf.RT[1].val[0] = 0.05073970775137447;
	m_fishInf.RT[1].val[1] = 0.9959717514725247;
	m_fishInf.RT[1].val[2] = 0.07393072653542832;
	m_fishInf.RT[1].val[3] = -3760.76;

	m_fishInf.RT[1].val[4] = -0.8144028221882277;
	m_fishInf.RT[1].val[5] = -0.001585663721717506;
	m_fishInf.RT[1].val[6] = 0.5802977932772202;
	m_fishInf.RT[1].val[7] = 3471.55;

	m_fishInf.RT[1].val[8] = 0.5780774388169416;
	m_fishInf.RT[1].val[9] = -0.08965353277653262;
	m_fishInf.RT[1].val[10] = 0.8110417491051445;
	m_fishInf.RT[1].val[11] = -887.951;

	m_fishInf.RT[1].val[12] = 0;
	m_fishInf.RT[1].val[13] = 0;
	m_fishInf.RT[1].val[14] = 0;
	m_fishInf.RT[1].val[15] = 1;
}

// 获取当前鱼眼镜头内参
Matx34d*CCalibrate::OnGetInnerParam()
{
	return &m_fishInf.innerParam;
}

// 获取当前鱼眼镜头的畸变系数
Matx14d*CCalibrate::OnGetDistortionParam()
{
	return &m_fishInf.distortionParam;
}

// 获取外参,iCount表示获取的外参的个数
Matx44d CCalibrate::OnGetRTParam( int iCount )
{
//	iCount = 2;      // 两个外参

	return m_fishInf.RT[iCount];            // RT的排列方式是先上后下，先左后右
}

fishEyeInf CCalibrate::OnGetFishEyeInf()
{
	return m_fishInf;
}
CCalibrate::~CCalibrate()
{

}