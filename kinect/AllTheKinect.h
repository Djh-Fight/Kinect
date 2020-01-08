#pragma once
#include "AllFile.h"
class AllTheKinect
{
public:
	AllTheKinect();
	bool GetAndShowDepthData();
	bool GetAndShowColorData();
	bool GetAndShowBoyIndexData();
	bool GetAndShowBoyData();
	bool GetAndDealHandImg();
	~AllTheKinect();
private:
	IKinectSensor*  m_pKinectSensor;  // ��ȡKinect�豸
	IDepthFrameSource*  pDepthFrameSource;//��������Ϣ������
	IDepthFrameReader*  m_pDepthFrameReader;//�������Ϣ֡��ȡ��
	IFrameDescription*  depthFrameDescription;
	int nDepthWidth;
	int nDepthHeight;

	IColorFrameSource* pColorFrameSource;//��ò�ɫ��Ϣ������
	IColorFrameReader* m_pColorFrameReader;//�򿪲�ɫ��Ϣ֡��ȡ��  	
	IFrameDescription* colorFrameDescription;
	int nColorWidth;
	int nColorHeight;

	IBodyIndexFrameSource* pBodyIndexFrameSource;//���������Ϣ������
	IBodyIndexFrameReader* pBodyIndexFrameReader;//��������Ϣ֡��ȡ��

	IBodyFrameSource* pBodyFrameSource;//��ù�����Ϣ������
	IBodyFrameReader* pBodyFrameReader;//�򿪹�����Ϣ֡��ȡ��

	ICoordinateMapper* coordinateMapper;//��������ת��ָ��

	HRESULT hr;
	Joint aJoints[JointType_Count];

	Mat MatDepth8;
	Mat MatDepth16;
	Mat MatRGB;      //ע�⣺�������Ϊ4ͨ����ͼ��Kinect������ֻ����Bgra��ʽ����
	Mat MatDepthToColor1;
};

extern Point ppp;
extern bool flag;

