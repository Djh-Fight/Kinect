#include <windows.h>
#include "Kinect.h"
#include <ShlObj_core.h>
#include <strsafe.h>
#include <iostream>
#include <opencv2/core/core.hpp>  
#include <opencv2\opencv.hpp>
#include <opencv2/highgui/highgui.hpp>  
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/ml/ml.hpp>
#include "AllTheKinect.h"
using namespace cv;
using namespace std;


void Trian(void) {
	string  classifierSavePath = "E:/Train/ImagePath.txt";



	int totalSampleCount = 20;

	cout << "//////////////////////////////////////////////////////////////////" << endl;
	cout << "totalSampleCount: " << totalSampleCount << endl;

	Mat sampleFeaturesMat(totalSampleCount, 1764, CV_32FC1);
	//64*128��ѵ���������þ�����totalSample*3780,64*64��ѵ���������þ�����totalSample*1764
	Mat sampleLabelMat(totalSampleCount, 1, CV_32SC1);//������ʶ  

	vector<string> imagePath;
	vector<int> imageClass;
	string buffer;
	ifstream trainingData(classifierSavePath);
	int numOfLine = 0;
	while (!trainingData.eof())
	{
		getline(trainingData, buffer);
		//cout << buffer << endl;
		if (!buffer.empty())
		{
			numOfLine++;
			if (numOfLine % 2 != 0)
			{
				//��ȡ�������
				imageClass.push_back(atoi(buffer.c_str()));
			}
			else
			{
				//��ȡͼ��·��
				imagePath.push_back(buffer);
			}
		}
	}
	trainingData.close();

	cout << "************************************************************" << endl;
	cout << "start to training ..." << endl;

	for (int i = 0; i < totalSampleCount; i++)
	{
		cout << imagePath[i] << endl;
		Mat img = imread(imagePath[i]);
		if (img.data == NULL)
		{
			cout << "image sample load error: " << i << " " << imagePath[i] << endl;
			system("pause");
			continue;
		}

		HOGDescriptor hog(cv::Size(64, 64), cv::Size(16, 16), cv::Size(8, 8), cv::Size(8, 8), 9);
		vector<float> featureVec;

		hog.compute(img, featureVec, cv::Size(8, 8));

		for (vector<float>::size_type j = 0; j < featureVec.size(); j++)
		{
			sampleFeaturesMat.at<float>(i, j) = featureVec[j];
		}
		sampleLabelMat.at<int>(i, 0) = imageClass[i];
	}
	cout << "size of featureVectorOfSample: " << sampleFeaturesMat.size() << endl;
	cout << "size of classOfSample: " << sampleLabelMat.size() << endl;
	cout << "end of training for  samples..." << endl;
	cout << "************************************************************" << endl;
	
	cout << "start to train for SVM classifier..." << endl;
	
	Ptr<cv::ml::SVM> svm = cv::ml::SVM::create();//SVM������ params;
	svm->setType(cv::ml::SVM::C_SVC);
	svm->setKernel(cv::ml::SVM::LINEAR);
	svm->setTermCriteria(cv::TermCriteria(TermCriteria::MAX_ITER, 3000, 1e-6));
	svm->setC(0.01);
	
	svm->train(sampleFeaturesMat, cv::ml::ROW_SAMPLE, sampleLabelMat);
	//system("pause");
	svm->save(string("E:/Train/classifier.xml"));

}

void Text() {
	Ptr<cv::ml::SVM> svm = cv::ml::SVM::create();//SVM������
	svm = cv::ml::SVM::load(string("E:/Train/classifier.xml"));
	if (!svm)
	{
		cout << "Load file failed..." << endl;
	}
	Mat test;
	test = imread("E:/Train/ab3a.jpg");
	HOGDescriptor hog(Size(64, 64), Size(16, 16), Size(8, 8), Size(8, 8), 9);
	vector<float> descriptors;//HOG����������
	hog.compute(test, descriptors, Size(8, 8));//����HOG�����ӣ���ⴰ���ƶ�����(8,8)

	int r = svm->predict(descriptors);   //�������н���Ԥ��
	cout << "The number is " << r << endl;
	system("pause");
}

void main03() {


}

int main(void) {
	AllTheKinect Kinect;
	while (1) {
		Kinect.GetAndShowDepthData();
		Kinect.GetAndShowColorData();
		Kinect.GetAndShowBoyIndexData();
		Kinect.GetAndShowBoyData();
		Kinect.GetAndDealHandImg();
	}
}





int main01()
{
	IKinectSensor* m_pKinectSensor;  // ��ȡKinect�豸

	IDepthFrameSource* pDepthFrameSource = NULL;//��������Ϣ������
	IDepthFrameReader* m_pDepthFrameReader = NULL;//�������Ϣ֡��ȡ��
	IFrameDescription* depthFrameDescription = NULL;
	int nDepthWidth = 0;
	int nDepthHeight = 0;

	IColorFrameSource* pColorFrameSource = NULL;//��ò�ɫ��Ϣ������
	IColorFrameReader* m_pColorFrameReader = NULL;//�򿪲�ɫ��Ϣ֡��ȡ��  	
	IFrameDescription* colorFrameDescription = NULL;
	int nColorWidth = 0;
	int nColorHeight = 0;

	IBodyIndexFrameSource* pBodyIndexFrameSource = NULL;//���������Ϣ������
	IBodyIndexFrameReader* pBodyIndexFrameReader = NULL;//��������Ϣ֡��ȡ��

	IBodyFrameSource* pBodyFrameSource = NULL;//��ù�����Ϣ������
	IBodyFrameReader* pBodyFrameReader = NULL;//�򿪹�����Ϣ֡��ȡ��

	ICoordinateMapper* coordinateMapper = NULL;//��������ת��ָ��

	Mat i_rgb(1080, 1920, CV_8UC4);      //ע�⣺�������Ϊ4ͨ����ͼ��Kinect������ֻ����Bgra��ʽ����
	Mat i_depth_8(424, 512, CV_8UC1);    //�������ͼ��8λ
	Mat i_depth_16(424, 512, CV_16UC1);  //�������ͼ��16λ

	/*-------------��������--------------*/
	HRESULT hr;

	hr = GetDefaultKinectSensor(&m_pKinectSensor);
	m_pKinectSensor->get_CoordinateMapper(&coordinateMapper);
	if (m_pKinectSensor) {
		hr = m_pKinectSensor->Open();
	}
	else {
		cout << "��ȡKinect�豸ʧ��" << endl;
		return hr;
	}
	if (SUCCEEDED(hr)) {
		//��������Ϣ������
		hr = m_pKinectSensor->get_DepthFrameSource(&pDepthFrameSource);
		//�������Ϣ֡��ȡ��
		hr = pDepthFrameSource->OpenReader(&m_pDepthFrameReader);
	}
	if (SUCCEEDED(hr)) {
		//��ò�ɫ��Ϣ������  
		hr = m_pKinectSensor->get_ColorFrameSource(&pColorFrameSource);
		//�򿪲�ɫ��Ϣ֡��ȡ��  
		hr = pColorFrameSource->OpenReader(&m_pColorFrameReader);
	}
	if (SUCCEEDED(hr)) {
		//���������Ϣ������
		hr = m_pKinectSensor->get_BodyIndexFrameSource(&pBodyIndexFrameSource);
		//��������Ϣ֡��ȡ��
		hr = pBodyIndexFrameSource->OpenReader(&pBodyIndexFrameReader);
	}
	if (SUCCEEDED(hr)) {
		//��ù�����Ϣ������
		hr = m_pKinectSensor->get_BodyFrameSource(&pBodyFrameSource);
		//�򿪹�����Ϣ֡��ȡ��
		hr = pBodyFrameSource->OpenReader(&pBodyFrameReader);
	}

	UINT32 yuzhi = 0;
	UINT32 yuzhi1 = 0;
	UINT32 nForwardYuzhi = 0;

	while (true) {
		DepthSpacePoint p1 = { 0,0 }, p2 = { 0,0 };
		Mat iDepthToColor(424, 512, CV_8UC4, Scalar::all(0));//�������ӳ�䵽��ɫ

		Joint aJoints[JointType_Count];
		IColorFrame* m_pColorFrame = NULL;//��ɫ��Ϣ����
		IDepthFrame* m_pDepthFrame = NULL;//�����Ϣ����
		IBodyIndexFrame* m_pBodyIndexFrame = NULL;//������Ϣ����
		IBodyFrame* m_pBodyFrame = NULL;
		while (m_pDepthFrame == NULL)
			hr = m_pDepthFrameReader->AcquireLatestFrame(&m_pDepthFrame);//��ȡ�������
		while (m_pColorFrame == NULL)
			hr = m_pColorFrameReader->AcquireLatestFrame(&m_pColorFrame);//��ȡ��ɫ����
		while (m_pBodyIndexFrame == NULL)
			hr = pBodyIndexFrameReader->AcquireLatestFrame(&m_pBodyIndexFrame);//��ȡ��������
		while (m_pBodyFrame == NULL)
			hr = pBodyFrameReader->AcquireLatestFrame(&m_pBodyFrame);//��ȡ��������

		m_pDepthFrame->get_FrameDescription(&depthFrameDescription);
		depthFrameDescription->get_Height(&nDepthHeight);
		depthFrameDescription->get_Width(&nDepthWidth);

		m_pColorFrame->get_FrameDescription(&colorFrameDescription);
		colorFrameDescription->get_Height(&nColorHeight);
		colorFrameDescription->get_Width(&nColorWidth);

		UINT nBodyIndexBufferSize = 0;
		BYTE* pBodyIndexBuffer = NULL;
		m_pBodyIndexFrame->AccessUnderlyingBuffer(&nBodyIndexBufferSize, &pBodyIndexBuffer);

		IBody* ppBodies[BODY_COUNT] = { 0 };
		m_pBodyFrame->GetAndRefreshBodyData(_countof(ppBodies), ppBodies);
		Mat mImg(nColorHeight, nColorWidth, CV_8UC4);
		for (int i = 0; i < BODY_COUNT; ++i) {
			IBody* pBody = ppBodies[i];
			if (pBody)
			{
				BOOLEAN bTracked = false;
				hr = pBody->get_IsTracked(&bTracked);

				//HandState 
				//pBody->get_IsRestricted
				if (SUCCEEDED(hr) && bTracked)
				{

					hr = pBody->GetJoints(_countof(aJoints), aJoints);

					/*DrawLine(mImg, aJoints[JointType_SpineBase], aJoints[JointType_SpineMid], coordinateMapper);
					DrawLine(mImg, aJoints[JointType_SpineMid], aJoints[JointType_SpineShoulder], coordinateMapper);
					DrawLine(mImg, aJoints[JointType_SpineShoulder], aJoints[JointType_Neck], coordinateMapper);
					DrawLine(mImg, aJoints[JointType_Neck], aJoints[JointType_Head], coordinateMapper);

					DrawLine(mImg, aJoints[JointType_SpineShoulder], aJoints[JointType_ShoulderLeft], coordinateMapper);
					DrawLine(mImg, aJoints[JointType_ShoulderLeft], aJoints[JointType_ElbowLeft], coordinateMapper);
					DrawLine(mImg, aJoints[JointType_ElbowLeft], aJoints[JointType_WristLeft], coordinateMapper);
					DrawLine(mImg, aJoints[JointType_WristLeft], aJoints[JointType_HandLeft], coordinateMapper);
					DrawLine(mImg, aJoints[JointType_HandLeft], aJoints[JointType_HandTipLeft], coordinateMapper);
					DrawLine(mImg, aJoints[JointType_HandLeft], aJoints[JointType_ThumbLeft], coordinateMapper);

					DrawLine(mImg, aJoints[JointType_SpineShoulder], aJoints[JointType_ShoulderRight], coordinateMapper);
					DrawLine(mImg, aJoints[JointType_ShoulderRight], aJoints[JointType_ElbowRight], coordinateMapper);
					DrawLine(mImg, aJoints[JointType_ElbowRight], aJoints[JointType_WristRight], coordinateMapper);
					DrawLine(mImg, aJoints[JointType_WristRight], aJoints[JointType_HandRight], coordinateMapper);
					DrawLine(mImg, aJoints[JointType_HandRight], aJoints[JointType_HandTipRight], coordinateMapper);
					DrawLine(mImg, aJoints[JointType_HandRight], aJoints[JointType_ThumbRight], coordinateMapper);

					DrawLine(mImg, aJoints[JointType_SpineBase], aJoints[JointType_HipLeft], coordinateMapper);
					DrawLine(mImg, aJoints[JointType_HipLeft], aJoints[JointType_KneeLeft], coordinateMapper);
					DrawLine(mImg, aJoints[JointType_KneeLeft], aJoints[JointType_AnkleLeft], coordinateMapper);
					DrawLine(mImg, aJoints[JointType_AnkleLeft], aJoints[JointType_FootLeft], coordinateMapper);

					DrawLine(mImg, aJoints[JointType_SpineBase], aJoints[JointType_HipRight], coordinateMapper);
					DrawLine(mImg, aJoints[JointType_HipRight], aJoints[JointType_KneeRight], coordinateMapper);
					DrawLine(mImg, aJoints[JointType_KneeRight], aJoints[JointType_AnkleRight], coordinateMapper);
					DrawLine(mImg, aJoints[JointType_AnkleRight], aJoints[JointType_FootRight], coordinateMapper);*/
					coordinateMapper->MapCameraPointToDepthSpace(aJoints[JointType_ElbowRight].Position, &p1);
					coordinateMapper->MapCameraPointToDepthSpace(aJoints[JointType_SpineShoulder].Position, &p2);
					//cout << aJoints[JointType_HandRight].Position.Z + 20 << endl;
				}
			}
		}

		//depth������ͼƬ��
		if (SUCCEEDED(hr))
			hr = m_pDepthFrame->CopyFrameDataToArray(nDepthHeight * nDepthWidth, reinterpret_cast<UINT16*>(i_depth_16.data));
		//depth���8λ���ݡ���>>��ʾ����
		UINT16* depthData = new UINT16[512 * 424];
		hr = m_pDepthFrame->CopyFrameDataToArray(nDepthHeight * nDepthWidth, depthData);
		for (int i = 0; i < nDepthHeight * nDepthWidth; i++) {
			BYTE intensity = static_cast<BYTE>(depthData[i] % 256);
			reinterpret_cast<BYTE*>(i_depth_8.data)[i] = intensity;
		}
		equalizeHist(i_depth_8, i_depth_8);

		UINT nBufferSize_depth = 0;
		UINT16* pBuffer_depth = NULL;
		//��ȡͼ�����ظ�����ָ��ͼ���ָ��
		m_pDepthFrame->AccessUnderlyingBuffer(&nBufferSize_depth, &pBuffer_depth);
		// color������ͼƬ��
		UINT nColorBufferSize = 1920 * 1080 * 4;
		if (SUCCEEDED(hr))
			hr = m_pColorFrame->CopyConvertedFrameDataToArray(nColorBufferSize, reinterpret_cast<BYTE*>(i_rgb.data), ColorImageFormat::ColorImageFormat_Bgra);
		ColorSpacePoint* colorSpacePoint = new ColorSpacePoint[512 * 424];
		coordinateMapper->MapDepthFrameToColorSpace(nDepthHeight * nDepthWidth, pBuffer_depth, nDepthHeight * nDepthWidth, colorSpacePoint);


		cout << "x:" << p1.X << "  y:" << p1.Y << endl;
		UINT32 weizhi = static_cast<uint>(floor(p1.Y) * nDepthWidth + floor(p1.X));
		UINT32 weizhi1 = static_cast<uint>(floor(p2.Y) * nDepthWidth + floor(p2.X));
		//cout << "x:" << weizhi << "  y:" << weizhi1 << endl;
		if (weizhi >= nDepthWidth * nDepthHeight) continue;
		if (weizhi1 >= nDepthWidth * nDepthHeight) continue;
		if (aJoints[JointType_SpineShoulder].TrackingState == TrackingState_Tracked) {
			yuzhi1 = depthData[weizhi1];
		}
		if (aJoints[JointType_ElbowRight].TrackingState == TrackingState_Tracked) {
			yuzhi = depthData[weizhi];
		}
		if (yuzhi == 0 && yuzhi < yuzhi1) {
			yuzhi = nForwardYuzhi;
		}
		else {
			nForwardYuzhi = yuzhi;
		}
		cout << yuzhi << "  " << yuzhi1 << endl;
		for (int i = 0; i < nDepthHeight; i++)
		{
			for (int j = 0; j < nDepthWidth; j++)
			{
				unsigned int index = i * nDepthWidth + j;
				ColorSpacePoint csp = colorSpacePoint[index];
				int colorX = static_cast<int>(floor(csp.X + 0.5));
				int colorY = static_cast<int>(floor(csp.Y + 0.5));
				//ѡȡ���ڲ�ɫͼ���ϵĵ㲢��ǰ������������ֵ�ָ�,�ٴθ�����ֵ��С
				if (colorX >= 0 && colorX < nColorWidth && colorY >= 0 && colorY < nColorHeight /*&& *depthData < 650*/)
				{
					BYTE player = pBodyIndexBuffer[index];
					if (player != 0xff && *depthData < yuzhi)//*depthData < 650
					//������ɫ��Ϣ
						iDepthToColor.at<cv::Vec4b>(i, j) = i_rgb.at<cv::Vec4b>(colorY, colorX) ;
					//if (*depthData == 0)  iDepthToColor.at<cv::Vec4b>(i, j) = 0;
				}
				depthData++;
			}

		}
		//delete[] depthData;
		circle(iDepthToColor, Point(512, 424), 4, Vec3b(0, 0, 255), 5);

		Rect abv(p1.X, p1.Y, 100, 100);
		rectangle(iDepthToColor, Point(p1.X - 50, p1.Y - 50), Point(p1.X + 50, p1.Y + 50), Scalar(255, 0, 0), 1, 1, 0);
		Mat handData = iDepthToColor(abv);
		Mat i_rgb_resize = i_rgb.clone();       // ��С���㿴
		cv::resize(i_rgb_resize, i_rgb_resize, Size(512, 424));
		// ��ʾ
		imshow("aaa", handData);
		imshow("rgb", i_rgb_resize);
		imshow("i_depth_16", i_depth_16);
		imshow("i_depth_8", i_depth_8);
		imshow("iDepthToColor", iDepthToColor);
		//imshow("aa", mImg);
		if (waitKey(1) == VK_ESCAPE)
			break;
		//setMouseCallback("rgb", onMouse, &i_rgb_resize);
		//imshow("i_src_depth", i_depth);
		//if (waitKey(1) == VK_ESCAPE)
		//	break;

		//if (iii < 50) {
			//GetScreenshotFileName(szScreenshotPath, _countof(szScreenshotPath));
			//HRESULT hr = SaveBitmapToFile(reinterpret_cast<BYTE*>(i_rgb_resize.data),20/* nWidth*/, 20/*nHeight*/, sizeof(RGBQUAD) * 8, szScreenshotPath);

			//Sleep(100);
			//cout << "ok" << hr << endl;
			//iii++;
		//}
		//if(iii<20)
		//iii++;

		// �ͷ���Դ
		m_pDepthFrame->Release();
		m_pColorFrame->Release();
		m_pBodyIndexFrame->Release();
		m_pBodyFrame->Release();
	}
	// �رմ��ڣ��豸
	cv::destroyAllWindows();
	m_pKinectSensor->Close();
	std::system("pause");
	return 0;
	/*UINT16* depthData = new UINT16[424 * 512];
	IMultiSourceFrame* m_pMultiFrame = nullptr;
	int iii = 0;
	WCHAR szScreenshotPath[MAX_PATH];
	int nWidth = 0;
	int nHeight = 0;
	IFrameDescription* pFrameDescription = NULL;
	IFrameDescription* pFrameDescription1 = NULL;
	BYTE iDepthSytle;
	unsigned int a = 0;
	while (true)
	{

		// depth������ͼƬ��
		if (SUCCEEDED(hr))
		{
			hr = m_pDepthFrame->CopyFrameDataToArray(424 * 512, depthData);
			for (int i = 0; i < 512 * 424; i++)
			{
			  // 0-255���ͼ��Ϊ����ʾ���ԣ�ֻȡ������ݵĵ�8λ
			  //BYTE intensity = static_cast<BYTE>(depthData[i] % 256);
				//BYTE intensity = static_cast<BYTE>(depthData[i]);
				UINT16 intensity = static_cast<UINT16>(depthData[i]);
				iDepthSytle = intensity & 0x0007;
				//cout << (int)iDepthSytle << endl;
				//if (iDepthSytle == 0x01)
					reinterpret_cast<UINT16*>(i_depth.data)[i] = (intensity >> 3);
					//cout << (int)(i_depth.data)[i] << endl;
				//else
					//reinterpret_cast<UINT16*>(i_depth.data)[i] = 0;
			}

			// ʵ����16λunsigned int����
			hr = m_pDepthFrame->CopyFrameDataToArray(424 * 512, reinterpret_cast<UINT16*>(i_src_depth.data));
		}

	}*/

}
