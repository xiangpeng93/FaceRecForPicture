#include <Windows.h>
#include <iostream>
#include <Shlobj.h>
#include <vector>

#include <opencv2\highgui\highgui.hpp>
#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#include <cctype>
#include <iostream>
#include <iterator>
#include <stdio.h>
#include <core\types_c.h>
#include<direct.h>

using namespace cv;
using namespace std;
vector<char *> g_picArr;

#define CRTDBG_MAP_ALLOC    
#include <stdlib.h>    
#include <crtdbg.h>   

static void help()
{
	cout << "\nThis program demonstrates the cascade recognizer. Now you can use Haar or LBP features.\n"
		"This classifier can recognize many kinds of rigid objects, once the appropriate classifier is trained.\n"
		"It's most known use is for faces.\n"
		"Usage:\n"
		"./facedetect [--cascade=<cascade_path> this is the primary trained classifier such as frontal face]\n"
		"   [--nested-cascade[=nested_cascade_path this an optional secondary classifier such as eyes]]\n"
		"   [--scale=<image scale greater or equal to 1, try 1.3 for example>]\n"
		"   [--try-flip]\n"
		"   [filename|camera_index]\n\n"
		"see facedetect.cmd for one call:\n"
		"./facedetect --cascade=\"../../data/haarcascades/haarcascade_frontalface_alt.xml\" --nested-cascade=\"../../data/haarcascades/haarcascade_eye.xml\" --scale=1.3\n\n"
		"During execution:\n\tHit any key to quit.\n"
		"\tUsing OpenCV version " << "\n" << endl;
}

int detectAndDraw(Mat& img, Mat &outImg, CascadeClassifier& cascade,
	double scale, bool tryflip , bool result);

int __stdcall OpenPicture(char *fileName)
{
	OPENFILENAME opfn;
	
	char strFilename[MAX_PATH];//����ļ���  
	//��ʼ��  
	ZeroMemory(&opfn, sizeof(OPENFILENAME));
	opfn.lStructSize = sizeof(OPENFILENAME);//�ṹ���С  
	//���ù���  
	opfn.lpstrFilter = "�����ļ�\0*.*";
	//Ĭ�Ϲ�����������Ϊ1  
	opfn.nFilterIndex = 1;
	//�ļ������ֶα����Ȱѵ�һ���ַ���Ϊ \0  
	opfn.lpstrFile = strFilename;
	opfn.lpstrFile[0] = '\0';
	opfn.nMaxFile = sizeof(strFilename);
	//���ñ�־λ�����Ŀ¼���ļ��Ƿ����  
	opfn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;
	// ��ʾ�Ի������û�ѡ���ļ�  
	if(GetOpenFileName(&opfn))
		memcpy(fileName , strFilename,MAX_PATH);
	else return -1;
	return 0;
}

int __stdcall OpenDir(char *dirPath)
{
	TCHAR szPathName[MAX_PATH];
	BROWSEINFO bInfo = { 0 };
	bInfo.hwndOwner = GetForegroundWindow();				//������  
	bInfo.lpszTitle = TEXT("����ļ���```ѡ���ļ���");
	bInfo.ulFlags = BIF_RETURNONLYFSDIRS | BIF_USENEWUI		/*����һ���༭�� �û������ֶ���д·�� �Ի�����Ե�����С֮���..*/ 
		| BIF_UAHINT										/*��TIPS��ʾ*/ 
		| BIF_NONEWFOLDERBUTTON								/*�����½��ļ��а�ť*/;
	LPITEMIDLIST lpDlist;
	lpDlist = SHBrowseForFolder(&bInfo);
	if (lpDlist != NULL)//������ȷ����ť  
	{
		SHGetPathFromIDList(lpDlist, szPathName);
		strcpy(dirPath, szPathName);
	}
	else return -1;
	return 0;
}

int __stdcall GetGrayPicture(char *srcPicPath, char *desPicPath)
{
	if (strlen(srcPicPath)>0)
	{
		cv::Mat image;
		image = imread(srcPicPath, 1);
		cv::Mat image_gray;
		cvtColor(image, image_gray, CV_BGR2GRAY);
		string destPath = srcPicPath;
		destPath.erase(destPath.find_last_of('\\'), destPath.size());

		strcpy(desPicPath, destPath.c_str());
		strcat(desPicPath, "\\Gray\\");

		_mkdir(desPicPath);

		destPath = srcPicPath;
		destPath.erase(0, destPath.find_last_of('\\'));
		strcat(desPicPath, destPath.c_str());
		cv::imwrite(desPicPath, image_gray);
	}
	else return -1;
	return 0;
}

int __stdcall GetThresholdPicture(char *srcPicPath, char *desPicPath)
{
	if (strlen(srcPicPath)>0)
	{
		cv::Mat image;
		image = imread(srcPicPath, 1);
		cv::Mat image_gray;
		cvtColor(image, image_gray, CV_BGR2GRAY);
		//�ֲ���ֵ��		
		int blockSize = 25;
		int constValue = 10;
		cv::Mat image_threshold;
		cv::adaptiveThreshold(image_gray, image_threshold, 255, CV_ADAPTIVE_THRESH_MEAN_C, CV_THRESH_BINARY_INV, blockSize, constValue);

		string destPath = srcPicPath;
		destPath.erase(destPath.find_last_of('\\'), destPath.size());

		strcpy(desPicPath, destPath.c_str());
		strcat(desPicPath, "\\Threshold\\");
		_mkdir(desPicPath);

		destPath = srcPicPath;
		destPath.erase(0, destPath.find_last_of('\\'));
		strcat(desPicPath, destPath.c_str());
		cv::imwrite(desPicPath, image_threshold);
	}
	else return -1;
	return 0;
}

int __stdcall GetHistImg(char *srcPicPath, char *desPicPath)
{
	if (strlen(srcPicPath) > 0)
	{
		cv::Mat image;
		image = imread(srcPicPath, 1);
		cv::Mat image_gray;
		cvtColor(image, image_gray, CV_BGR2GRAY);
		
		const int channels[1] = { 0 };
		const int histSize[1] = { 256 };
		float hranges[2] = { 0, 255 };
		const float* ranges[1] = { hranges };
		MatND hist;
		calcHist(&image_gray, 1, channels, Mat(), hist, 1, histSize, ranges);

		double maxVal = 0;
		double minVal = 0;

		//�ҵ�ֱ��ͼ�е����ֵ����Сֵ
		minMaxLoc(hist, &minVal, &maxVal, 0, 0);
		int histSize1 = hist.rows;
		Mat histImg(histSize1, histSize1, CV_8U, Scalar(255));

		// ��������ֵΪͼ��߶ȵ�90%
		int hpt = static_cast<int>(0.9*histSize1);

		for (int h = 0; h < histSize1; h++)
		{
			float binVal = hist.at<float>(h);
			int intensity = static_cast<int>(binVal*hpt / maxVal);
			line(histImg, Point(h, histSize1), Point(h, histSize1 - intensity), Scalar::all(0));
		}

		string destPath = srcPicPath;
		destPath.erase(destPath.find_last_of('\\'), destPath.size());

		strcpy(desPicPath, destPath.c_str());
		strcat(desPicPath, "\\Hist\\");
		_mkdir(desPicPath);
		destPath = srcPicPath;
		destPath.erase(0, destPath.find_last_of('\\'));
		strcat(desPicPath, destPath.c_str());
		cv::imwrite(desPicPath, histImg);
	}
	else return -1;
	return 0;
}

int __stdcall GetFacePicture(char *srcPicPath, char *desPicPath)
{
	char moudlePath[MAX_PATH] = { 0 };
	GetModuleFileName(NULL, moudlePath, MAX_PATH);
	string cascadeName = moudlePath;
	cascadeName.erase(cascadeName.find_last_of('\\'), cascadeName.size());
	cascadeName += "\\haarcascade_frontalface_alt.xml";
	OutputDebugString(cascadeName.c_str());
	CascadeClassifier cascade;
	if (!cascade.load(cascadeName))
	{
		OutputDebugString("ERROR: Could not load classifier cascade");
		help();
		return -1;
	}
	cv::Mat image;
	cv::Mat faceImg;
	image = imread(srcPicPath, 1);
	OutputDebugString(srcPicPath);
	double scale = 1;
	bool tryflip = false;
	if (!image.empty())
	{
		OutputDebugString("\r\n image is not empty\n");
		int nFace = detectAndDraw(image, faceImg, cascade, scale, tryflip,false);
		if (nFace> 0)
		{
			OutputDebugString("FaceRec success\n");

			string destPath = srcPicPath;
			destPath.erase(destPath.find_last_of('\\'), destPath.size());

			strcpy(desPicPath, destPath.c_str());
			strcat(desPicPath, "\\FaceRec\\");
			_mkdir(desPicPath);

			destPath = srcPicPath;
			destPath.erase(0, destPath.find_last_of('\\'));
			strcat(desPicPath, destPath.c_str());
			cv::imwrite(desPicPath, faceImg);
			return nFace;
		}
	}
	return -2;
}

int __stdcall GetResultPic(char *srcPicPath, char *desPicPath)
{
	char moudlePath[MAX_PATH] = { 0 };
	GetModuleFileName(NULL, moudlePath, MAX_PATH);
	string cascadeName = moudlePath;
	cascadeName.erase(cascadeName.find_last_of('\\'), cascadeName.size());
	cascadeName += "\\haarcascade_frontalface_alt.xml";
	OutputDebugString(cascadeName.c_str());
	CascadeClassifier cascade;
	if (!cascade.load(cascadeName))
	{
		OutputDebugString("ERROR: Could not load classifier cascade");
		help();
		return -1;
	}
	cv::Mat image;
	cv::Mat faceImg;
	image = imread(srcPicPath, 1);
	OutputDebugString(srcPicPath);
	double scale = 1;
	bool tryflip = false;
	if (!image.empty())
	{
		OutputDebugString("\r\n image is not empty\n");
		int nFace = detectAndDraw(image, faceImg, cascade, scale, tryflip, true);
		if (nFace> 0)
		{
			OutputDebugString("FaceRec success\n");

			string destPath = srcPicPath;
			destPath.erase(destPath.find_last_of('\\'), destPath.size());

			strcpy(desPicPath, destPath.c_str());
			strcat(desPicPath, "\\Result\\");
			_mkdir(desPicPath);

			destPath = srcPicPath;
			destPath.erase(0, destPath.find_last_of('\\'));
			strcat(desPicPath, destPath.c_str());
			cv::imwrite(desPicPath, faceImg);
			return nFace;
		}
	}
	return -2;
}
////ֻ�ܴ���Ŀ¼:lpPathֻ����·��  
//void find(wchar_t *lpPath)
//{
//	wchar_t szFind[MAX_PATH];
//	wchar_t szFile[MAX_PATH];
//
//	WIN32_FIND_DATA FindFileData;
//
//	wcscpy(szFind, lpPath);
//	wcscat(szFind, L"//*.*");
//
//	HANDLE hFind = ::FindFirstFile(szFind, &FindFileData);
//	if (INVALID_HANDLE_VALUE == hFind)    return;
//	while (TRUE)
//	{
//		if (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
//		{
//			if (FindFileData.cFileName[0] != '.')
//			{
//				wcscpy(szFile, lpPath);
//				wcscat(szFile, L"//");
//				wcscat(szFile, FindFileData.cFileName);
//				find(szFile);
//			}
//		}
//		else
//		{
//			wstring temp_fileName = lpPath;
//			temp_fileName += L"\\";
//			temp_fileName += FindFileData.cFileName;
//			wchar_t *temp = new wchar_t[MAX_PATH];
//			wcscpy(temp, temp_fileName.c_str());
//			g_picArr.push_back(temp);
//		}
//		if (!FindNextFile(hFind, &FindFileData))
//			break;
//	}
//	FindClose(hFind);
//}
//
////��ͬʱ����Ŀ¼���ļ�:path������·����Ҳ�������ļ����������ļ�ͨ���  
//void _find(wstring path){
//	//ȡ·�������һ��"//"֮ǰ�Ĳ���,����"//"  
//	wstring prefix = path.substr(0, path.find_last_of('//') + 1);
//
//	WIN32_FIND_DATA FindFileData;
//	HANDLE hFind = ::FindFirstFile(path.c_str(), &FindFileData);
//	if (INVALID_HANDLE_VALUE == hFind)
//	{
//		cout << "�ļ�ͨ�������" << endl;
//		return;
//	}
//	while (TRUE)
//	{
//		//Ŀ¼  
//		if (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
//		{
//			//���ǵ�ǰĿ¼��Ҳ���Ǹ�Ŀ¼  
//			if (FindFileData.cFileName[0] != '.')
//			{
//				//������һ��Ŀ¼  
//				_find(prefix + FindFileData.cFileName + L"//" + L"*.*");
//			}
//		}
//		//�ļ�  
//		else
//		{
//			cout << FindFileData.cFileName << endl;
//		}
//		if (!FindNextFile(hFind, &FindFileData))
//			break;
//	}
//	FindClose(hFind);
//}

inline void EnableMemLeakCheck()
{
#ifdef CRTDBG_MAP_ALLOC
	_CrtDumpMemoryLeaks();
	_CrtSetDbgFlag(_CrtSetDbgFlag(_CRTDBG_REPORT_FLAG) | _CRTDBG_LEAK_CHECK_DF);
#endif
}
//
//int main()
//{
//	char fileName[MAX_PATH];
//	//_openPicture(fileName);
//	_openDir(fileName);
//	find(fileName);
//	for (int i = 0; i < g_picArr.size(); i++)
//	{
//		cout << g_picArr[i] << endl;
//		/*delete[] g_picArr[i];
//		g_picArr[i] = NULL;*/
//	}
//
//	//Mat srcImage = imread(fileName);
//	//imshow("����è��", srcImage);
//	EnableMemLeakCheck();
//	return 0;
//}

//int main(int argc, const char** argv)
//{
//	CvCapture* capture = 0;
//	Mat frame, frameCopy, image;
//	const string scaleOpt = "--scale=";
//	size_t scaleOptLen = scaleOpt.length();
//	const string cascadeOpt = "--cascade=";
//	size_t cascadeOptLen = cascadeOpt.length();
//	const string nestedCascadeOpt = "--nested-cascade";
//	size_t nestedCascadeOptLen = nestedCascadeOpt.length();
//	const string tryFlipOpt = "--try-flip";
//	size_t tryFlipOptLen = tryFlipOpt.length();
//	string inputName;
//	bool tryflip = false;
//
//	help();
//
//	CascadeClassifier cascade, nestedCascade;
//	double scale = 1;
//	int  i = 0;
//
//	if (!cascade.load(cascadeName))
//	{
//		cerr << "ERROR: Could not load classifier cascade" << endl;
//		help();
//		return -1;
//	}
//
//	image = imread("lena.jpg", 1);
//	cv::Mat image_gray;
//	cvtColor(image, image_gray, CV_BGR2GRAY);
//	cv::imshow("32", image_gray);
//
//	// �ֲ���ֵ��
//
//	int blockSize = 25;
//	int constValue = 10;
//	cv::Mat local;
//	cv::adaptiveThreshold(image_gray, local, 255, CV_ADAPTIVE_THRESH_MEAN_C, CV_THRESH_BINARY_INV, blockSize, constValue);
//	cv::imshow("322", local);
//
//	if (image.empty()) cout << "Couldn't read lena.jpg" << endl;
//	cvNamedWindow("result", 1);
//
//
//
//	////////////
//	const int channels[1] = { 0 };
//	const int histSize[1] = { 256 };
//	float hranges[2] = { 0, 255 };
//	const float* ranges[1] = { hranges };
//	MatND hist;
//	calcHist(&image_gray, 1, channels, Mat(), hist, 1, histSize, ranges);
//
//	
//	double maxVal = 0;
//	double minVal = 0;
//
//
//	//�ҵ�ֱ��ͼ�е����ֵ����Сֵ
//	minMaxLoc(hist, &minVal, &maxVal, 0, 0);
//	int histSize1 = hist.rows;
//	Mat histImg(histSize1, histSize1, CV_8U, Scalar(255));
//
//	// ��������ֵΪͼ��߶ȵ�90%
//	int hpt = static_cast<int>(0.9*histSize1);
//
//	for (int h = 0; h<histSize1; h++)
//	{
//		float binVal = hist.at<float>(h);
//		int intensity = static_cast<int>(binVal*hpt / maxVal);
//		line(histImg, Point(h, histSize1), Point(h, histSize1 - intensity), Scalar::all(0));
//	}
//	cv::imshow("histImg", histImg);
//
//	cout << "In image read" << endl;
//	if (!image.empty())
//	{
//		detectAndDraw(image, cascade, scale, tryflip);
//		waitKey(0);
//	}
//		
//
//	cvDestroyWindow("result");
//
//	return 0;
//}

int detectAndDraw(Mat& img, Mat &outImg, CascadeClassifier& cascade,
	double scale, bool tryflip, bool result)
{
	int nRet = 0;
	int i = 0;
	double t = 0;
	vector<Rect> faces, faces2;
	const static Scalar colors[] = { CV_RGB(0, 0, 255),
		CV_RGB(0, 128, 255),
		CV_RGB(0, 255, 255),
		CV_RGB(0, 255, 0),
		CV_RGB(255, 128, 0),
		CV_RGB(255, 255, 0),
		CV_RGB(255, 0, 0),
		CV_RGB(255, 0, 255) };
	Mat gray, smallImg(cvRound(img.rows / scale), cvRound(img.cols / scale), CV_8UC1);

	cvtColor(img, gray, CV_BGR2GRAY);
	resize(gray, smallImg, smallImg.size(), 0, 0, INTER_LINEAR);
	equalizeHist(smallImg, smallImg);

	t = (double)cvGetTickCount();
	cascade.detectMultiScale(smallImg, faces,
		1.1, 2, 0
		//|CV_HAAR_FIND_BIGGEST_OBJECT
		//|CV_HAAR_DO_ROUGH_SEARCH
		| CV_HAAR_SCALE_IMAGE
		,
		Size(30, 30));
	if (tryflip)
	{
		flip(smallImg, smallImg, 1);
		cascade.detectMultiScale(smallImg, faces2,
			1.1, 2, 0
			//|CV_HAAR_FIND_BIGGEST_OBJECT
			//|CV_HAAR_DO_ROUGH_SEARCH
			| CV_HAAR_SCALE_IMAGE
			,
			Size(30, 30));
		for (vector<Rect>::const_iterator r = faces2.begin(); r != faces2.end(); r++)
		{
			faces.push_back(Rect(smallImg.cols - r->x - r->width, r->y, r->width, r->height));
		}
	}
	t = (double)cvGetTickCount() - t;
	printf("detection time = %g ms\n", t / ((double)cvGetTickFrequency()*1000.));
	for (vector<Rect>::const_iterator r = faces.begin(); r != faces.end(); r++, i++)
	{
		nRet = nRet++;
		Mat smallImgROI;
		vector<Rect> nestedObjects;
		Point center;
		Scalar color = colors[i % 8];
		double aspect_ratio = (double)r->width / r->height;
		//memcpy(&outImg, &imageROI, sizeof(imageROI));
		if (!result)
		{
			cv::Mat imageROI = img(cv::Rect(r->x - (r->width) / 10, r->y - (r->height) / 10, r->width + (r->width) / 5, r->height + (r->height) / 5));
			outImg = imageROI;
		}
		else
		{
			CvSize czSize;							//Ŀ��ͼ��ߴ� 358 * 441
			int nSize = 1;
			czSize.height = img.size().height / nSize;
			czSize.width = img.size().width / nSize;
			if (img.size().height < 441 * 2 && img.size().width < 358 * 2)
			{
				/*if ((r->height / 441) * 2  > 1 && (r->width / 358) * 2 > 1)
				{*/
					/*nSize += (r->height / 441) * 2   > (r->width / 358) * 2 ? (czSize.width / 358) * 2 : (r->height / 441) * 2;
					if (img.size().height / nSize > 441 && img.size().width > 358)
					{
						czSize.height = img.size().height / nSize;
						czSize.width = img.size().width / nSize;
					}*/
				/*}*/
				czSize.height = img.size().height * 2;
				czSize.width = img.size().width *2;
				nSize = 2;
			}
			
			
			cv::Mat temp;
			cv::resize(img, temp, czSize, 0, 0, 3);

			cv::Mat imageROI;
			if (r->height < 441 /2 || r->width < 358 /2)
				imageROI = temp(cv::Rect((r->x + (r->width) / 2 )* nSize - 179, (r->y + (r->height) / 2- 100)* nSize, 358, 441));
			else
				imageROI = temp(cv::Rect((r->x - (r->width) / 10)*nSize, (r->y - (r->height) / 10)*nSize, (r->width + (r->width) / 5)*nSize, (r->height + (r->height) / 5)*nSize));

			//cv::Mat imageROI = img(cv::Rect(r->x - (r->width) / 2, r->y - (r->height) / 2, r->width + (r->width) , r->height + (r->height)));
			outImg = imageROI;

			/*czSize.height = 441;img.size().height;
			czSize.width = 358; img.size().width;
			cv::Mat temp;
			cv::resize(img, temp, czSize, 0, 0, 3);
			outImg = temp;*/
		}
		/*rectangle(img, cvPoint(cvRound(r->x*scale), cvRound(r->y*scale)),
		cvPoint(cvRound((r->x + r->width - 1)*scale), cvRound((r->y + r->height - 1)*scale)),
		color, 3, 8, 0);	*/			
	}
	return nRet;
	//cv::imshow("result", img);
}

BOOL APIENTRY DllMain(HMODULE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved
	)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		break;
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
		break;
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}
