#ifndef GENERATE_3D_TABLE_H
#define GENERATE_3D_TABLE_H
///////////////////////////////////////
// ����3D��ı����

#include "../common.h"
#include "../calibrate/CCalibrate.h"
#include "../iniFile/inirw.h"
// ʹ��OPENCL���ɲ��ұ�

#define PI 3.1415926

#define SHOW_3DIMG 0          // �Ƿ���ʾ�ʹ洢3Dͼ��

#define PER_HEIGHT 1.3          // ��ֵ������ʾ�ĸ߶�

struct virtualCameraParam                                        // �����������������
{
	double cameraMaxFieldView;                                             // ���������ӳ���
	float circleRadius;                                                   // ����Բ�뾶���Ժ���Ϊ��λ��
	int longRes;												// ���ȷֱ���
	int latRes;													// γ�ȷֱ���
	//int   cameraHeight;                                          
	//Point3f circleCenterLocation;                                // Բ�ĵ�λ��,��Z���ϣ��Ժ���Ϊ��λ
};

struct G3DtableParam                                             // ���ɴ�3D���һ��Ҫ�Ĳ���
{
	// ����ΪFISHEYE_COUNT
	CCalibrate* CalibrateParam;                                // �����ı궨����һ�����ĸ�

	//////////////////////////////////////////////////////////////////////////
	CvSize2D32f  world_show_size;							// ����������ϵ����ʾ����Ĵ�С���Ժ���Ϊ��λ��
	CvSize2D32f worldcarSize;                                       // ��������������ϵ�ĳߴ�
	float fWorldCarBeginX;
	float fWorldCarBeginY;
	//////////////////////////////////////////////////////////////////////////

#if NEW3D
	//////////////////////////////////////////////////////////////////////////
	// ���غ�������ĸ���������������ϵ�ĵ��ͼ������ϵ�ĵ��¼������
	vector<vector<cv::Point3f> >*pRectW;
	vector<vector<cv::Point2f> >*pRectWImg;
	//////////////////////////////////////////////////////////////////////////
#endif


	int nx;
	int ny;

	//////////////////////////////////////////////////////////////////////////
	//CvSize2D32f worldshowSize;                                 // ����������ϵ����ʾ�ĳߴ�
	//float fBeginOriginX;
	//float fBeginOriginY;
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	// ƴ�ӷ죬����ΪFISHEYE_COUNT
	float*m_K;        // ƴ�ӷ��б�ʣ�0��1��2��3�ֱ��ʾ����ƴ�ӷ죬����ƴ�ӷ죬����ƴ�ӷ죬����ƴ�ӷ�
	float*m_B;        // ƴ�ӷ�bֵ��ͬ��
	// �ں��ߣ�����Ϊ2*FISHEYE_COUNT
	float*m_BMix;   // �ں��ߣ��������ں��ߣ��ֳ����ĸ����֣�ÿ����1�顣����Ϊ�ں�����ƴ���ߵ��·���ż��������0��Ϊ�ں�����ƴ���ߵ��Ϸ�   
	//////////////////////////////////////////////////////////////////////////
};

struct stu3dTable               // ����Ĳ��ұ�
{
	cv::Point2f iLocation;            // ��ǰ����ͼ�������ֵ

	short/*cameraLocation*/ img1Idx;     // ��һ������ͼ���IDֵ�������ֵΪ-1����ô�ʹ�������κ�ͼ����ӳ��ֵ
	short/*cameraLocation*/ img2Idx;     // �ڶ�������ͼ���IDֵ�������ֵΪ-1����ô�ʹ�������κ�ͼ����ӳ��ֵ

	cv::Point2f img1Location;         // iLocation��Ӧ������ͼ���ϵĵ�
	float img1Weight;           // ��һ��ͼ���Ȩ��

	
	cv::Point2f img2Location;         // iLocation��Ӧ����ǰ����ͼ���ϵĵ�
	float img2Weight;           // �ڶ���ͼ���ϵ�Ȩ��
};

// �غ�����д�ɣ�����P_TOP|P_LEFT
enum PointLocation
{ 
	P_NO = 0,
	P_TOP = 1,               // ǰ����
	P_BOTTOM = 2 ,            // ������
	P_LEFT = 4 ,              // ������
	P_RIGHT = 8 ,             // ������
	P_CAR = 16                 // С������
};
//  
class CGenerate3Dtable
{
public:
	CGenerate3Dtable();
	~CGenerate3Dtable();

	void OnGenerate3Dtable( G3DtableParam* p3DtableParam );

	void OnGenerateNew3Dtable( G3DtableParam*p3DtableParam );
private:

	// �������������
	virtualCameraParam m_vCameraParam;

	// ������Բ�ĵĸ߶�
	float m_CircleCenter;

	// ����궨������Ĭ��Ϊ����BX7�ı궨����
	calibrateScheme m_CalibrateScheme;

	// ���̸�����Ĵ�С
	int m_iWide; // ���
	int m_iHeight; // �߶�

	cv::Matx44d m_RT_3DvalueAverage[4];         // ���ֵ��ֻ���м�Ľ������򡣷ֱ��Ӧ���ĸ����̸��м佥�䲿�֡����󽥱䵽�ң����Ͻ��䵽�¡��±��ʾ�ϡ��¡������

	//////////////////////////////////////////////////////////////////////////
	// ��ͼ�����
	int                     m_wide_area[3];        // �±�Ϊ0��1��2�ֱ��ʾΪ������
	int                     m_height_area[3];      // �±�Ϊ0��1��2�ֱ��ʾΪ������
	cv::Matx44d                 m_RT_DvalueAverage[4];         // ���ֵ��ֻ���м�Ľ������򡣷ֱ��Ӧ���ĸ����̸��м佥�䲿�֡����󽥱䵽�ң����Ͻ��䵽�¡��±��ʾ�ϡ��¡������

	int                     m_iSourceWide;          // ԭ����ͼ��Ŀ��
	int                     m_iSourceHeight;        // ԭ����ͼ��ĸ߶�

	//////////////////////////////////////////////////////////////////////////
	// ƴ�ӷ��ֱ��б�ʺ�bֵ
	float                   m_K[FISHEYE_COUNT];        // ƴ�ӷ��б�ʣ�0��1��2��3�ֱ��ʾ����ƴ�ӷ죬����ƴ�ӷ죬����ƴ�ӷ죬����ƴ�ӷ�
	float                   m_B[FISHEYE_COUNT];        // ƴ�ӷ�bֵ��ͬ��

	float                   m_BMix[2 * FISHEYE_COUNT];   // �ں��ߣ��������ں��ߣ��ֳ����ĸ����֣�ÿ����1�顣����Ϊ�ں�����ƴ���ߵ��·���ż��������0��Ϊ�ں�����ƴ���ߵ��Ϸ�     

	// �غ������С
	int m_doubleAreaSize;

private:
	// ��ȡ���̸��С
	bool OnGetCheckBoardSize(struct virtualCameraParam &vCameraParam, int&wide, int&height);

	// ��ȡ����С����λ��
	int OnGetPointLocation(double xLocation , double yLocation ,  G3DtableParam* p3DtableParam );

	// ����ƴ�ӷ���ں�����
	void OnSetSpliceJoint(CvSize2D32f worldShowSize, float fBeginOriginX, float fBeginOriginY, CvSize2D32f worldCarSize, float fWorldCarBeginX, float fWorldCarBeginY, CCalibrate *pCameraParam);

	// ������
	PointLocation OnCheckisInCarRange( double xLocation , double yLocation , CvSize2D32f worldCarSize , float fWorldCarBeginX , float fWorldCarBeginY );

	// ������
	PointLocation OnCheckisInCarTop( double xLocation , double yLocation , CvSize2D32f worldCarSize , float fWorldCarBeginX , float fWorldCarBeginY , float* pK , float* pB );

	// ������
	PointLocation OnCheckisInCarBottom( double xLocation , double yLocation , CvSize2D32f worldCarSize , float fWorldCarBeginX , float fWorldCarBeginY , float* pK , float* pB );

	// ������
	PointLocation OnCheckisInCarLeft( double xLocation , double yLocation , CvSize2D32f worldCarSize , float fWorldCarBeginX , float fWorldCarBeginY , float* pK , float* pB );

	// ������
	PointLocation OnCheckisInCarRight( double xLocation , double yLocation , CvSize2D32f worldCarSize , float fWorldCarBeginX , float fWorldCarBeginY , float* pK , float* pB );

	// ������ͼ����ӳ��
	// ������ͼ��ӳ��
	void mappingFromFisheye_3D(cv::Mat&WorldLocation, cv::Matx44d*RT, stu3dTable& tableInf, int isrcWide, int isrcHeight, CCalibrate *pCameraParam);

	// ����ÿ��RT�ľ�ֵ
	void OnGenerateRTAverage( G3DtableParam* p3DtableParam );

	// ��ȡ3D���ڵ�ֵ
	// xΪ��������ϵ��xֵ
	// yΪ��������ϵ��yֵ
	// pLocationΪ��ǰ����ͷ���ڳ������ĸ��ط�
	// ��ȡ��ǰ��RTֵ
	void OnGet3DRTvalue(int x, int y, PointLocation pLocation, cv::Matx44d&RT, G3DtableParam*p3DtableParam);

	// �����������ϵ������ͼ
	void OnCameraCoordinate(float x1, float y1, float z1, cv::Mat&Img, CCalibrate&pCameraParam, int&iSrcX, int&iSrcY);

	// ��������������ϵ�����������ϵ������
	// ����������ϵ��worldpoint1������ӳ�䵽��һ����cameraPoint1�����������ϵ����ȥ
	void OnWorldmaptoCamera(vector<cv::Point3f> worldpoint1, vector<cv::Point3f>&cameraPoint1, PointLocation pLocation1, G3DtableParam*paramC);
};


#endif