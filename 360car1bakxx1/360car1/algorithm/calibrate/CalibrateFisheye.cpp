#include "CalibrateFisheye.h"

IntrinsicParams::IntrinsicParams() :f(cv::Vec2d::all(0)), c(cv::Vec2d::all(0)), k(cv::Vec4d::all(0)), alpha(0), isEstimate(9, 0)
{
}

IntrinsicParams::IntrinsicParams(cv::Vec2d _f, cv::Vec2d _c, cv::Vec4d _k, double _alpha) :
f(_f), c(_c), k(_k), alpha(_alpha), isEstimate(9,0)
{
}

IntrinsicParams IntrinsicParams::operator+(const cv::Mat& a)
{
	CV_Assert(a.type() == CV_64FC1);
	IntrinsicParams tmp;
	const double* ptr = a.ptr<double>();

	int j = 0;
	tmp.f[0]    = this->f[0]    + (isEstimate[0] ? ptr[j++] : 0);
	tmp.f[1]    = this->f[1]    + (isEstimate[1] ? ptr[j++] : 0);
	tmp.c[0]    = this->c[0]    + (isEstimate[2] ? ptr[j++] : 0);
	tmp.alpha   = this->alpha   + (isEstimate[4] ? ptr[j++] : 0);
	tmp.c[1]    = this->c[1]    + (isEstimate[3] ? ptr[j++] : 0);
	tmp.k[0]    = this->k[0]    + (isEstimate[5] ? ptr[j++] : 0);
	tmp.k[1]    = this->k[1]    + (isEstimate[6] ? ptr[j++] : 0);
	tmp.k[2]    = this->k[2]    + (isEstimate[7] ? ptr[j++] : 0);
	tmp.k[3]    = this->k[3]    + (isEstimate[8] ? ptr[j++] : 0);

	tmp.isEstimate = isEstimate;
	return tmp;
}

IntrinsicParams& IntrinsicParams::operator =(const cv::Mat& a)
{
	CV_Assert(a.type() == CV_64FC1);
	const double* ptr = a.ptr<double>();

	int j = 0;

	this->f[0]  = isEstimate[0] ? ptr[j++] : 0;
	this->f[1]  = isEstimate[1] ? ptr[j++] : 0;
	this->c[0]  = isEstimate[2] ? ptr[j++] : 0;
	this->c[1]  = isEstimate[3] ? ptr[j++] : 0;
	this->alpha = isEstimate[4] ? ptr[j++] : 0;
	this->k[0]  = isEstimate[5] ? ptr[j++] : 0;
	this->k[1]  = isEstimate[6] ? ptr[j++] : 0;
	this->k[2]  = isEstimate[7] ? ptr[j++] : 0;
	this->k[3]  = isEstimate[8] ? ptr[j++] : 0;

	return *this;
}

void IntrinsicParams::Init(const cv::Vec2d& _f, const cv::Vec2d& _c, const cv::Vec4d& _k, const double& _alpha)
{
	this->c = _c;
	this->f = _f;
	this->k = _k;
	this->alpha = _alpha;
}

CCalibrateFisheye::CCalibrateFisheye()
{

}

bool CCalibrateFisheye::getChessboardCorners(cv::Mat mat, int imageIndex, cv::Size m_board_size, vector<cv::Point2f>&cornersPoint)
{
	//Mat imageGray;
	bool patternfound;

	vector<cv::Point2f> m_corners;      // 角点的位置

	// 将mat转换为灰度图
	//cvtColor( mat , imageGray , CV_RGB2GRAY );

	m_corners.clear();                                     // 将上一个corner的数据清理干净

	// 调用opencv函数找角点
	patternfound = cv::findChessboardCorners(mat, m_board_size, m_corners);// , CALIB_CB_ADAPTIVE_THRESH | CALIB_CB_NORMALIZE_IMAGE | CALIB_CB_FAST_CHECK );

	if( !patternfound )              // 角点提取失败
	{
		cout<<"img"<<imageIndex<<"提取角点失败，请删除该图像，重新排序后再次运行该程序!"<<endl;

		return false;
	}
	else                             // 角点提取成功
	{
		// 亚像素精确化
		// 最大迭代30次，或者角点位置变化小于0.1时，停止迭代过程
		// m_corners的值是角点的位置
		cornerSubPix(mat, m_corners, cv::Size(11, 11), cv::Size(-1, -1), cv::TermCriteria(CV_TERMCRIT_EPS + CV_TERMCRIT_ITER, 30, 0.1));

		// 将提取出的角点画出来后，新保存图像
		int j = 0;
		cv::Mat imageTemp = mat.clone();

		for ( j = 0 ; j<m_corners.size() ; j++ )
		{
			circle(imageTemp, m_corners[j], 10, cv::Scalar(0, 0, 255), 2, 8, 0);//OpenCV中三彩色通道的顺序是BGR而不是RGB。
		}

		std::stringstream StrStm;
		string imageFileName;
		StrStm.clear();
		StrStm<<imageIndex<<"_corner.jpg";
		StrStm>>imageFileName;
		imwrite(imageFileName,imageTemp);
		cout<<"img"<<imageIndex<<" 角点提取完毕"<<endl;

		/* 检测到的角点保存到txt文件 */
		string txtFileName;
		StrStm.clear();
		StrStm<<imageIndex<<"_corner.txt";
		StrStm>>txtFileName;
		ofstream fm(txtFileName);
		for (j=0; j<m_corners.size(); j++)
		{
			fm<<m_corners[j].x<<"    "<<m_corners[j].y<<endl;
		}
		fm.close(); 

		cornersPoint = m_corners;

#if 0
		//////////////////////////////////////////////////////////////////////////
		// 将m_corners的值重新排列后输入到cornersPoint返回
		int i=0;
		Point2f dataTemp;
		int iTemppp = 0;
		int iTemppp1 = 0;
		if ( imageIndex == 1 )               // 前图片
		{
			//for ( i = 0 ; i<m_corners.size() ; i++ )
			//{
			//	dataTemp.x = m_corners.at(i).x;
			//	dataTemp.y = m_corners.at(i).y;
			//	cornersPoint.push_back( dataTemp );
			//}
			for ( i = m_corners.size() - 1 ; i>=0 ; i-- )
			{
				dataTemp.x = m_corners.at(i).x;
				dataTemp.y = m_corners.at(i).y;
				cornersPoint.push_back( dataTemp );
			}
		}
		else if ( imageIndex == 2 )          // 后图片
		{
			//for ( i = m_corners.size() - 1 ; i>=0 ; i-- )
			//{
			//	dataTemp.x = m_corners.at(i).x;
			//	dataTemp.y = m_corners.at(i).y;
			//	cornersPoint.push_back( dataTemp );
			//}

			for ( i = 0 ; i<m_corners.size() ; i++ )
			{
				dataTemp.x = m_corners.at(i).x;
				dataTemp.y = m_corners.at(i).y;
				cornersPoint.push_back( dataTemp );
			}
		}
		else if ( imageIndex == 3 )          // 左图片
		{
			//iTemppp1 = m_board_size.width;
			//for ( i = m_board_size.width-1 ; i>=0 ; i-- )
			//{
			//	//iTemppp1 = iTemppp1 - i;
			//	for ( j = 0 ; j<m_board_size.height ; j++ )
			//	{
			//		if ( j == 0 )
			//		{
			//			iTemppp = j*i + i ;
			//		}
			//		else
			//		{
			//			iTemppp = iTemppp + m_board_size.width ;
			//		}

			//		dataTemp.x = m_corners.at( iTemppp ).x;
			//		dataTemp.y = m_corners.at( iTemppp ).y;
			//		cornersPoint.push_back( dataTemp );
			//		
			//	}
			//}

			iTemppp1 = m_board_size.width;
			for ( i = 0 ; i<m_board_size.width ; i++ )
			{
				iTemppp1 = iTemppp1 - i;
				for ( j = m_board_size.height - 1 ; j>=0 ; j-- )
				{
					//if ( j == 0 )
					//{
					//	iTemppp = j*i + i ;
					//}
					//else
					//{
					//	iTemppp = iTemppp + m_board_size.width ;
					//}

					iTemppp = j*m_board_size.width + i;

					dataTemp.x = m_corners.at( iTemppp ).x;
					dataTemp.y = m_corners.at( iTemppp ).y;
					cornersPoint.push_back( dataTemp );

				}
			}
		}
		else if (imageIndex == 4 )           // 右图片
		{
			//iTemppp1 = m_board_size.width;
			//for ( i = m_board_size.width-1 ; i>=0 ; i-- )
			//{
			//	//iTemppp1 = iTemppp1 - i;
			//	for ( j = 0 ; j<m_board_size.height ; j++ )
			//	{
			//		if ( j == 0 )
			//		{
			//			iTemppp = j*i + i ;
			//		}
			//		else
			//		{
			//			iTemppp = iTemppp + m_board_size.width ;
			//		}

			//		dataTemp.x = m_corners.at( iTemppp ).x;
			//		dataTemp.y = m_corners.at( iTemppp ).y;
			//		cornersPoint.push_back( dataTemp );
			//	}
			//}

			iTemppp1 = m_board_size.width;
			for ( i = 0 ; i<m_board_size.width ; i++ )
			{
				iTemppp1 = iTemppp1 - i;
				for ( j = m_board_size.height - 1 ; j>=0 ; j-- )
				{
					//if ( j == 0 )
					//{
					//	iTemppp = j*i + i ;
					//}
					//else
					//{
					//	iTemppp = iTemppp + m_board_size.width ;
					//}

					iTemppp = j*m_board_size.width + i;

					dataTemp.x = m_corners.at( iTemppp ).x;
					dataTemp.y = m_corners.at( iTemppp ).y;
					cornersPoint.push_back( dataTemp );

				}
			}

		}
		//successImageNum = successImageNum + 1;
		//corners_Seq.push_back(m_corners);
#endif
	}


	// image_Seq.push_back( mat );

	return true;
}

cv::Mat CCalibrateFisheye::ComputeHomography(cv::Mat m, cv::Mat M)
{
	int Np = m.cols;

	if (m.rows < 3)
	{
		vconcat(m, cv::Mat::ones(1, Np, CV_64FC1), m);
	}
	if (M.rows < 3)
	{
		vconcat(M, cv::Mat::ones(1, Np, CV_64FC1), M);
	}

	divide(m, cv::Mat::ones(3, 1, CV_64FC1) * m.row(2), m);
	divide(M, cv::Mat::ones(3, 1, CV_64FC1) * M.row(2), M);

	cv::Mat ax = m.row(0).clone();
	cv::Mat ay = m.row(1).clone();

	double mxx = mean(ax)[0];
	double myy = mean(ay)[0];

	ax = ax - mxx;
	ay = ay - myy;

	double scxx = mean(abs(ax))[0];
	double scyy = mean(abs(ay))[0];

	cv::Mat Hnorm(cv::Matx33d(1 / scxx, 0.0, -mxx / scxx,
		0.0,     1/scyy,     -myy/scyy,
		0.0,        0.0,           1.0 ));

	cv::Mat inv_Hnorm(cv::Matx33d(scxx, 0, mxx,
		0,  scyy,   myy,
		0,     0,     1 ));
	cv::Mat mn = Hnorm * m;

	cv::Mat L = cv::Mat::zeros(2 * Np, 9, CV_64FC1);

	for (int i = 0; i < Np; ++i)
	{
		for (int j = 0; j < 3; j++)
		{
			L.at<double>(2 * i, j) = M.at<double>(j, i);
			L.at<double>(2 * i + 1, j + 3) = M.at<double>(j, i);
			L.at<double>(2 * i, j + 6) = -mn.at<double>(0,i) * M.at<double>(j, i);
			L.at<double>(2 * i + 1, j + 6) = -mn.at<double>(1,i) * M.at<double>(j, i);
		}
	}

	if (Np > 4) L = L.t() * L;
	cv::SVD svd(L);
	cv::Mat hh = svd.vt.row(8) / svd.vt.row(8).at<double>(8);
	cv::Mat Hrem = hh.reshape(1, 3);
	cv::Mat H = inv_Hnorm * Hrem;

	if (Np > 4)
	{
		cv::Mat hhv = H.reshape(1, 9)(cv::Rect(0, 0, 1, 8)).clone();
		for (int iter = 0; iter < 10; iter++)
		{
			cv::Mat mrep = H * M;
			cv::Mat J = cv::Mat::zeros(2 * Np, 8, CV_64FC1);
			cv::Mat MMM;
			divide(M, cv::Mat::ones(3, 1, CV_64FC1) * mrep(cv::Rect(0, 2, mrep.cols, 1)), MMM);
			divide(mrep, cv::Mat::ones(3, 1, CV_64FC1) * mrep(cv::Rect(0, 2, mrep.cols, 1)), mrep);
			cv::Mat m_err = m(cv::Rect(0, 0, m.cols, 2)) - mrep(cv::Rect(0, 0, mrep.cols, 2));
			m_err = cv::Mat(m_err.t()).reshape(1, m_err.cols * m_err.rows);
			cv::Mat MMM2, MMM3;
			multiply(cv::Mat::ones(3, 1, CV_64FC1) * mrep(cv::Rect(0, 0, mrep.cols, 1)), MMM, MMM2);
			multiply(cv::Mat::ones(3, 1, CV_64FC1) * mrep(cv::Rect(0, 1, mrep.cols, 1)), MMM, MMM3);

			for (int i = 0; i < Np; ++i)
			{
				for (int j = 0; j < 3; ++j)
				{
					J.at<double>(2 * i, j)         = -MMM.at<double>(j, i);
					J.at<double>(2 * i + 1, j + 3) = -MMM.at<double>(j, i);
				}

				for (int j = 0; j < 2; ++j)
				{
					J.at<double>(2 * i, j + 6)     = MMM2.at<double>(j, i);
					J.at<double>(2 * i + 1, j + 6) = MMM3.at<double>(j, i);
				}
			}
			divide(M, cv::Mat::ones(3, 1, CV_64FC1) * mrep(cv::Rect(0, 2, mrep.cols, 1)), MMM);
			cv::Mat hh_innov = (J.t() * J).inv() * (J.t()) * m_err;
			cv::Mat hhv_up = hhv - hh_innov;
			cv::Mat tmp;
			vconcat(hhv_up, cv::Mat::ones(1, 1, CV_64FC1), tmp);
			cv::Mat H_up = tmp.reshape(1, 3);
			hhv = hhv_up;
			H = H_up;
		}
	}
	return H;
}

void CCalibrateFisheye::undistortPoints(cv::InputArray distorted, cv::OutputArray undistorted, cv::InputArray K, cv::InputArray D, cv::InputArray R, cv::InputArray P)
{
	// will support only 2-channel data now for points
	CV_Assert(distorted.type() == CV_32FC2 || distorted.type() == CV_64FC2);
	undistorted.create(distorted.size(), distorted.type());

	CV_Assert(P.empty() || P.size() == cv::Size(3, 3) || P.size() == cv::Size(4, 3));
	CV_Assert(R.empty() || R.size() == cv::Size(3, 3) || R.total() * R.channels() == 3);
	CV_Assert(D.total() == 4 && K.size() == cv::Size(3, 3) && (K.depth() == CV_32F || K.depth() == CV_64F));

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

	cv::Vec4d k = D.depth() == CV_32F ? (cv::Vec4d)*D.getMat().ptr<cv::Vec4f>() : *D.getMat().ptr<cv::Vec4d>();

	cv::Matx33d RR = cv::Matx33d::eye();
	if (!R.empty() && R.total() * R.channels() == 3)
	{
		cv::Vec3d rvec;
		R.getMat().convertTo(rvec, CV_64F);
		RR = cv::Affine3d(rvec).rotation();
	}
	else if (!R.empty() && R.size() == cv::Size(3, 3))
		R.getMat().convertTo(RR, CV_64F);

	if(!P.empty())
	{
		cv::Matx33d PP;
		P.getMat().colRange(0, 3).convertTo(PP, CV_64F);
		RR = PP * RR;
	}

	// start undistorting
	const cv::Vec2f* srcf = distorted.getMat().ptr<cv::Vec2f>();
	const cv::Vec2d* srcd = distorted.getMat().ptr<cv::Vec2d>();
	cv::Vec2f* dstf = undistorted.getMat().ptr<cv::Vec2f>();
	cv::Vec2d* dstd = undistorted.getMat().ptr<cv::Vec2d>();

	size_t n = distorted.total();
	int sdepth = distorted.depth();

	for(size_t i = 0; i < n; i++ )
	{
		cv::Vec2d pi = sdepth == CV_32F ? (cv::Vec2d)srcf[i] : srcd[i];  // image point
		cv::Vec2d pw((pi[0] - c[0]) / f[0], (pi[1] - c[1]) / f[1]);      // world point

		double scale = 1.0;

		double theta_d = sqrt(pw[0]*pw[0] + pw[1]*pw[1]);
		if (theta_d > 1e-8)
		{
			// compensate distortion iteratively
			double theta = theta_d;
			for(int j = 0; j < 10; j++ )
			{
				double theta2 = theta*theta, theta4 = theta2*theta2, theta6 = theta4*theta2, theta8 = theta6*theta2;
				theta = theta_d / (1 + k[0] * theta2 + k[1] * theta4 + k[2] * theta6 + k[3] * theta8);
			}

			scale = std::tan(theta) / theta_d;
		}

		cv::Vec2d pu = pw * scale; //undistorted point

		// reproject
		cv::Vec3d pr = RR * cv::Vec3d(pu[0], pu[1], 1.0); // rotated point optionally multiplied by new camera matrix
		cv::Vec2d fi(pr[0] / pr[2], pr[1] / pr[2]);       // final

		if( sdepth == CV_32F )
			dstf[i] = fi;
		else
			dstd[i] = fi;
	}
}

cv::Mat CCalibrateFisheye::NormalizePixels(const cv::Mat& imagePoints, const IntrinsicParams& param)
{
	CV_Assert(!imagePoints.empty() && imagePoints.type() == CV_64FC2);

	cv::Mat distorted((int)imagePoints.total(), 1, CV_64FC2), undistorted;
	const cv::Vec2d* ptr = imagePoints.ptr<cv::Vec2d>(0);
	cv::Vec2d* ptr_d = distorted.ptr<cv::Vec2d>(0);
	for (size_t i = 0; i < imagePoints.total(); ++i)
	{
		ptr_d[i] = (ptr[i] - param.c).mul(cv::Vec2d(1.0 / param.f[0], 1.0 / param.f[1]));
		ptr_d[i][0] = ptr_d[i][0] - param.alpha * ptr_d[i][1];
	}
	undistortPoints(distorted, undistorted, cv::Matx33d::eye(), param.k);

	return undistorted;
}

void CCalibrateFisheye::InitExtrinsics(const cv::Mat& _imagePoints, const cv::Mat& _objectPoints, const IntrinsicParams& param, cv::Mat& omckk, cv::Mat& Tckk)
{
	CV_Assert(!_objectPoints.empty() && _objectPoints.type() == CV_64FC3);
	CV_Assert(!_imagePoints.empty() && _imagePoints.type() == CV_64FC2);

	cv::Mat imagePointsNormalized = NormalizePixels(_imagePoints.t(), param).reshape(1).t();
	cv::Mat objectPoints = cv::Mat(_objectPoints.t()).reshape(1).t();
	cv::Mat objectPointsMean, covObjectPoints;
	cv::Mat Rckk;
	int Np = imagePointsNormalized.cols;
	calcCovarMatrix(objectPoints, covObjectPoints, objectPointsMean, CV_COVAR_NORMAL | CV_COVAR_COLS);
	cv::SVD svd(covObjectPoints);
	cv::Mat R(svd.vt);
	if (norm(R(cv::Rect(2, 0, 1, 2))) < 1e-6)
		R = cv::Mat::eye(3, 3, CV_64FC1);
	if (determinant(R) < 0)
		R = -R;
	cv::Mat T = -R * objectPointsMean;
	cv::Mat X_new = R * objectPoints + T * cv::Mat::ones(1, Np, CV_64FC1);
	cv::Mat H = ComputeHomography(imagePointsNormalized, X_new(cv::Rect(0, 0, X_new.cols, 2)));
	double sc = .5 * (norm(H.col(0)) + norm(H.col(1)));
	H = H / sc;
	cv::Mat u1 = H.col(0).clone();
	u1  = u1 / norm(u1);
	cv::Mat u2 = H.col(1).clone() - u1.dot(H.col(1).clone()) * u1;
	u2 = u2 / norm(u2);
	cv::Mat u3 = u1.cross(u2);
	cv::Mat RRR;
	hconcat(u1, u2, RRR);
	hconcat(RRR, u3, RRR);
	Rodrigues(RRR, omckk);
	Rodrigues(omckk, Rckk);
	Tckk = H.col(2).clone();
	Tckk = Tckk + Rckk * T;
	Rckk = Rckk * R;
	Rodrigues(Rckk, omckk);
}

void CCalibrateFisheye::projectPoints_fishEye(cv::InputArray objectPoints, cv::OutputArray imagePoints, cv::InputArray _rvec, cv::InputArray _tvec, cv::InputArray _K, cv::InputArray _D, double alpha, cv::OutputArray jacobian)
{
	// will support only 3-channel data now for points
	CV_Assert(objectPoints.type() == CV_32FC3 || objectPoints.type() == CV_64FC3);
	imagePoints.create(objectPoints.size(), CV_MAKETYPE(objectPoints.depth(), 2));
	size_t n = objectPoints.total();

	CV_Assert(_rvec.total() * _rvec.channels() == 3 && (_rvec.depth() == CV_32F || _rvec.depth() == CV_64F));
	CV_Assert(_tvec.total() * _tvec.channels() == 3 && (_tvec.depth() == CV_32F || _tvec.depth() == CV_64F));
	CV_Assert(_tvec.getMat().isContinuous() && _rvec.getMat().isContinuous());

	cv::Vec3d om = _rvec.depth() == CV_32F ? (cv::Vec3d)*_rvec.getMat().ptr<cv::Vec3f>() : *_rvec.getMat().ptr<cv::Vec3d>();
	cv::Vec3d T = _tvec.depth() == CV_32F ? (cv::Vec3d)*_tvec.getMat().ptr<cv::Vec3f>() : *_tvec.getMat().ptr<cv::Vec3d>();

	CV_Assert(_K.size() == cv::Size(3, 3) && (_K.type() == CV_32F || _K.type() == CV_64F) && _D.type() == _K.type() && _D.total() == 4);

	cv::Vec2d f, c;
	if (_K.depth() == CV_32F)
	{

		cv::Matx33f K = _K.getMat();
		f = cv::Vec2f(K(0, 0), K(1, 1));
		c = cv::Vec2f(K(0, 2), K(1, 2));
	}
	else
	{
		cv::Matx33d K = _K.getMat();
		f = cv::Vec2d(K(0, 0), K(1, 1));
		c = cv::Vec2d(K(0, 2), K(1, 2));
	}

	cv::Vec4d k = _D.depth() == CV_32F ? (cv::Vec4d)*_D.getMat().ptr<cv::Vec4f>() : *_D.getMat().ptr<cv::Vec4d>();

	JacobianRow *Jn = 0;
	if (jacobian.needed())
	{
		int nvars = 2 + 2 + 1 + 4 + 3 + 3; // f, c, alpha, k, om, T,
		jacobian.create(2*(int)n, nvars, CV_64F);
		Jn = jacobian.getMat().ptr<JacobianRow>(0);
	}

	cv::Matx33d R;
	cv::Matx<double, 3, 9> dRdom;
	Rodrigues(om, R, dRdom);
	cv::Affine3d aff(om, T);

	const cv::Vec3f* Xf = objectPoints.getMat().ptr<cv::Vec3f>();
	const cv::Vec3d* Xd = objectPoints.getMat().ptr<cv::Vec3d>();
	cv::Vec2f *xpf = imagePoints.getMat().ptr<cv::Vec2f>();
	cv::Vec2d *xpd = imagePoints.getMat().ptr<cv::Vec2d>();

	for(size_t i = 0; i < n; ++i)
	{
		cv::Vec3d Xi = objectPoints.depth() == CV_32F ? (cv::Vec3d)Xf[i] : Xd[i];
		cv::Vec3d Y = aff*Xi;

		cv::Vec2d x(Y[0] / Y[2], Y[1] / Y[2]);

		double r2 = x.dot(x);
		double r = std::sqrt(r2);

		// Angle of the incoming ray:
		double theta = atan(r);

		double theta2 = theta*theta, theta3 = theta2*theta, theta4 = theta2*theta2, theta5 = theta4*theta,
			theta6 = theta3*theta3, theta7 = theta6*theta, theta8 = theta4*theta4, theta9 = theta8*theta;

		double theta_d = theta + k[0]*theta3 + k[1]*theta5 + k[2]*theta7 + k[3]*theta9;

		double inv_r = r > 1e-8 ? 1.0/r : 1;
		double cdist = r > 1e-8 ? theta_d * inv_r : 1;

		cv::Vec2d xd1 = x * cdist;
		cv::Vec2d xd3(xd1[0] + alpha*xd1[1], xd1[1]);
		cv::Vec2d final_point(xd3[0] * f[0] + c[0], xd3[1] * f[1] + c[1]);

		if (objectPoints.depth() == CV_32F)
			xpf[i] = final_point;
		else
			xpd[i] = final_point;

		if (jacobian.needed())
		{
			//Vec3d Xi = pdepth == CV_32F ? (Vec3d)Xf[i] : Xd[i];
			//Vec3d Y = aff*Xi;
			double dYdR[] = { Xi[0], Xi[1], Xi[2], 0, 0, 0, 0, 0, 0,
				0, 0, 0, Xi[0], Xi[1], Xi[2], 0, 0, 0,
				0, 0, 0, 0, 0, 0, Xi[0], Xi[1], Xi[2] };

			cv::Matx33d dYdom_data = cv::Matx<double, 3, 9>(dYdR) * dRdom.t();
			const cv::Vec3d *dYdom = (cv::Vec3d*)dYdom_data.val;

			cv::Matx33d dYdT_data = cv::Matx33d::eye();
			const cv::Vec3d *dYdT = (cv::Vec3d*)dYdT_data.val;

			//Vec2d x(Y[0]/Y[2], Y[1]/Y[2]);
			cv::Vec3d dxdom[2];
			dxdom[0] = (1.0/Y[2]) * dYdom[0] - x[0]/Y[2] * dYdom[2];
			dxdom[1] = (1.0/Y[2]) * dYdom[1] - x[1]/Y[2] * dYdom[2];

			cv::Vec3d dxdT[2];
			dxdT[0]  = (1.0/Y[2]) * dYdT[0] - x[0]/Y[2] * dYdT[2];
			dxdT[1]  = (1.0/Y[2]) * dYdT[1] - x[1]/Y[2] * dYdT[2];

			//double r2 = x.dot(x);
			cv::Vec3d dr2dom = 2 * x[0] * dxdom[0] + 2 * x[1] * dxdom[1];
			cv::Vec3d dr2dT = 2 * x[0] * dxdT[0] + 2 * x[1] * dxdT[1];

			//double r = std::sqrt(r2);
			double drdr2 = r > 1e-8 ? 1.0/(2*r) : 1;
			cv::Vec3d drdom = drdr2 * dr2dom;
			cv::Vec3d drdT = drdr2 * dr2dT;

			// Angle of the incoming ray:
			//double theta = atan(r);
			double dthetadr = 1.0/(1+r2);
			cv::Vec3d dthetadom = dthetadr * drdom;
			cv::Vec3d dthetadT = dthetadr *  drdT;

			//double theta_d = theta + k[0]*theta3 + k[1]*theta5 + k[2]*theta7 + k[3]*theta9;
			double dtheta_ddtheta = 1 + 3*k[0]*theta2 + 5*k[1]*theta4 + 7*k[2]*theta6 + 9*k[3]*theta8;
			cv::Vec3d dtheta_ddom = dtheta_ddtheta * dthetadom;
			cv::Vec3d dtheta_ddT = dtheta_ddtheta * dthetadT;
			cv::Vec4d dtheta_ddk = cv::Vec4d(theta3, theta5, theta7, theta9);

			//double inv_r = r > 1e-8 ? 1.0/r : 1;
			//double cdist = r > 1e-8 ? theta_d / r : 1;
			cv::Vec3d dcdistdom = inv_r * (dtheta_ddom - cdist*drdom);
			cv::Vec3d dcdistdT = inv_r * (dtheta_ddT - cdist*drdT);
			cv::Vec4d dcdistdk = inv_r *  dtheta_ddk;

			//Vec2d xd1 = x * cdist;
			cv::Vec4d dxd1dk[2];
			cv::Vec3d dxd1dom[2], dxd1dT[2];
			dxd1dom[0] = x[0] * dcdistdom + cdist * dxdom[0];
			dxd1dom[1] = x[1] * dcdistdom + cdist * dxdom[1];
			dxd1dT[0]  = x[0] * dcdistdT  + cdist * dxdT[0];
			dxd1dT[1]  = x[1] * dcdistdT  + cdist * dxdT[1];
			dxd1dk[0]  = x[0] * dcdistdk;
			dxd1dk[1]  = x[1] * dcdistdk;

			//Vec2d xd3(xd1[0] + alpha*xd1[1], xd1[1]);
			cv::Vec4d dxd3dk[2];
			cv::Vec3d dxd3dom[2], dxd3dT[2];
			dxd3dom[0] = dxd1dom[0] + alpha * dxd1dom[1];
			dxd3dom[1] = dxd1dom[1];
			dxd3dT[0]  = dxd1dT[0]  + alpha * dxd1dT[1];
			dxd3dT[1]  = dxd1dT[1];
			dxd3dk[0]  = dxd1dk[0]  + alpha * dxd1dk[1];
			dxd3dk[1]  = dxd1dk[1];

			cv::Vec2d dxd3dalpha(xd1[1], 0);

			//final jacobian
			Jn[0].dom = f[0] * dxd3dom[0];
			Jn[1].dom = f[1] * dxd3dom[1];

			Jn[0].dT = f[0] * dxd3dT[0];
			Jn[1].dT = f[1] * dxd3dT[1];

			Jn[0].dk = f[0] * dxd3dk[0];
			Jn[1].dk = f[1] * dxd3dk[1];

			Jn[0].dalpha = f[0] * dxd3dalpha[0];
			Jn[1].dalpha = 0; //f[1] * dxd3dalpha[1];

			Jn[0].df = cv::Vec2d(xd3[0], 0);
			Jn[1].df = cv::Vec2d(0, xd3[1]);

			Jn[0].dc = cv::Vec2d(1, 0);
			Jn[1].dc = cv::Vec2d(0, 1);

			//step to jacobian rows for next point
			Jn += 2;
		}
	}
}

void CCalibrateFisheye::projectPoints(cv::InputArray objectPoints, cv::OutputArray imagePoints,cv::InputArray _rvec,cv::InputArray _tvec,const IntrinsicParams& param, cv::OutputArray jacobian)
{
	CV_Assert(!objectPoints.empty() && objectPoints.type() == CV_64FC3);
	cv::Matx33d K(param.f[0], param.f[0] * param.alpha, param.c[0],
		0,               param.f[1], param.c[1],
		0,                        0,         1);
	// fisheye::projectPoints(objectPoints, imagePoints, _rvec, _tvec, K, param.k, param.alpha, jacobian);
	projectPoints_fishEye( objectPoints, imagePoints, _rvec, _tvec, K, param.k, param.alpha, jacobian );
}

void CCalibrateFisheye::ComputeExtrinsicRefine(const cv::Mat& imagePoints, const cv::Mat& objectPoints, cv::Mat& rvec, cv::Mat&  tvec, cv::Mat& J, const int MaxIter, const IntrinsicParams& param, const double thresh_cond)
{
	CV_Assert(!objectPoints.empty() && objectPoints.type() == CV_64FC3);
	CV_Assert(!imagePoints.empty() && imagePoints.type() == CV_64FC2);
	cv::Vec6d extrinsics(rvec.at<double>(0), rvec.at<double>(1), rvec.at<double>(2),
		tvec.at<double>(0), tvec.at<double>(1), tvec.at<double>(2));
	double change = 1;
	int iter = 0;

	while (change > 1e-10 && iter < MaxIter)
	{
		std::vector<cv::Point2d> x;
		cv::Mat jacobians;
		projectPoints(objectPoints, x, rvec, tvec, param, jacobians);

		cv::Mat ex = imagePoints - cv::Mat(x).t();
		ex = ex.reshape(1, 2);

		J = jacobians.colRange(8, 14).clone();

		cv::SVD svd(J, cv::SVD::NO_UV);
		double condJJ = svd.w.at<double>(0)/svd.w.at<double>(5);

		if (condJJ > thresh_cond)
			change = 0;
		else
		{
			cv::Vec6d param_innov;
			solve(J, ex.reshape(1, (int)ex.total()), param_innov, cv::DECOMP_SVD + cv::DECOMP_NORMAL);

			cv::Vec6d param_up = extrinsics + param_innov;
			change = norm(param_innov)/norm(param_up);
			extrinsics = param_up;
			iter = iter + 1;

			rvec = cv::Mat(cv::Vec3d(extrinsics.val));
			tvec = cv::Mat(cv::Vec3d(extrinsics.val + 3));
		}
	}
}

bool CCalibrateFisheye::OnCalibrateExtrinsics(cv::InputArrayOfArrays objectPoints, cv::InputArrayOfArrays imagePoints, const IntrinsicParams& param, const int check_cond, const double thresh_cond, cv::InputOutputArray omc, cv::InputOutputArray Tc)
{

	CV_Assert(!objectPoints.empty() && (objectPoints.type() == CV_32FC3 || objectPoints.type() == CV_64FC3));
	CV_Assert(!imagePoints.empty() && (imagePoints.type() == CV_32FC2 || imagePoints.type() == CV_64FC2));
	CV_Assert(omc.type() == CV_64FC3 || Tc.type() == CV_64FC3);

	if (omc.empty()) 
	{
		omc.create(1, (int)objectPoints.total(), CV_64FC3);
	}
	if (Tc.empty()) 
	{
		Tc.create(1, (int)objectPoints.total(), CV_64FC3);
	}

	const int maxIter = 20;

	for(int image_idx = 0; image_idx < (int)imagePoints.total(); ++image_idx)
	{
		cv::Mat omckk, Tckk, JJ_kk;
		cv::Mat image, object;

		//////////////////////////////////////////////////////////////////////////
		// 将objectPoints中的数据转换到object，就是对应的世界坐标系中的点
		objectPoints.getMat(image_idx).convertTo(object,  CV_64FC3);

	//	//float* p = (float*)object.data;
	//	//////////////////////////////////////////////////////////////////////////
	//	//////////////////////////////////////////////////////////////////////////
	//	// 将imagePoints中的数据转换到image的mat数据类型中，就是对应的图像坐标系中的点
		imagePoints.getMat (image_idx).convertTo(image, CV_64FC2);
	//	//////////////////////////////////////////////////////////////////////////


		InitExtrinsics(image, object, param, omckk, Tckk);

		ComputeExtrinsicRefine(image, object, omckk, Tckk, JJ_kk, maxIter, param, thresh_cond);
		if (check_cond)
		{
			cv::SVD svd(JJ_kk, cv::SVD::NO_UV);
			CV_Assert(svd.w.at<double>(0) / svd.w.at<double>((int)svd.w.total() - 1) < thresh_cond);
		}
		omckk.reshape(3,1).copyTo(omc.getMat().col(image_idx));
		Tckk.reshape(3,1).copyTo(Tc.getMat().col(image_idx));
	}

	//if (rvecs.needed()) cv::Mat(omc).convertTo(rvecs, rvecs.empty() ? CV_64FC3 : rvecs.type());
	//if (tvecs.needed()) cv::Mat(Tc).convertTo(tvecs, tvecs.empty() ? CV_64FC3 : tvecs.type());


	return true;
}

bool CCalibrateFisheye::OnOptimizeExtrinsics(cv::InputArrayOfArrays objectPoints, cv::InputArrayOfArrays imagePoints, const IntrinsicParams& param, const int check_cond, const double thresh_cond, cv::InputOutputArray omc, cv::InputOutputArray Tc)
{
	CV_Assert(!objectPoints.empty() && (objectPoints.type() == CV_32FC3 || objectPoints.type() == CV_64FC3));
	CV_Assert(!imagePoints.empty() && (imagePoints.type() == CV_32FC2 || imagePoints.type() == CV_64FC2));
	CV_Assert(omc.type() == CV_64FC3 || Tc.type() == CV_64FC3);

	if (omc.empty()) 
	{
		omc.create(1, (int)objectPoints.total(), CV_64FC3);
	}
	if (Tc.empty()) 
	{
		Tc.create(1, (int)objectPoints.total(), CV_64FC3);
	}

	const int maxIter = 20;

	for(int image_idx = 0; image_idx < (int)imagePoints.total(); ++image_idx)
	{
		cv::Mat omckk, Tckk, JJ_kk;
		cv::Mat image, object;

		//////////////////////////////////////////////////////////////////////////
		// 将objectPoints中的数据转换到object，就是对应的世界坐标系中的点
		objectPoints.getMat(image_idx).convertTo(object,  CV_64FC3);

		//	//float* p = (float*)object.data;
		//	//////////////////////////////////////////////////////////////////////////
		//	//////////////////////////////////////////////////////////////////////////
		//	// 将imagePoints中的数据转换到image的mat数据类型中，就是对应的图像坐标系中的点
		imagePoints.getMat (image_idx).convertTo(image, CV_64FC2);
		//	//////////////////////////////////////////////////////////////////////////

	//	omc.getMat( image_idx ).convertTo( omckk , CV_32SC2 );
		//Tc.getMat( image_idx ).convertTo( Tckk , CV_32SC2 );
	//	omc.getMat().convertTo( omckk ,CV_64FC3 );
	//	Tc.getMat().convertTo( Tckk , CV_64FC3 );

		omc.getMat().reshape( 1 , 3).col( 0 ).convertTo( omckk , CV_64FC3 );
		Tc.getMat().reshape( 1, 3 ).col(0).convertTo( Tckk , CV_64FC3 );
		//Tckk = Tc.getMat();

	//	InitExtrinsics(image, object, param, omckk, Tckk);

		ComputeExtrinsicRefine(image, object, omckk , Tckk , JJ_kk, maxIter, param, thresh_cond);
		if (check_cond)
		{
			cv::SVD svd(JJ_kk, cv::SVD::NO_UV);
			CV_Assert(svd.w.at<double>(0) / svd.w.at<double>((int)svd.w.total() - 1) < thresh_cond);
		}
		omckk.reshape(3,1).copyTo(omc.getMat().col(image_idx));
		Tckk.reshape(3,1).copyTo(Tc.getMat().col(image_idx));
	}
	return true;
}

CCalibrateFisheye::~CCalibrateFisheye()
{

}