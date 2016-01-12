#include <Windows.h>
#include <iostream>
#include <Shlobj.h>
#include <vector>

#include <opencv2\highgui\highgui.hpp>
using namespace cv;
using namespace std;
vector<char *> g_picArr;


#define CRTDBG_MAP_ALLOC    
#include <stdlib.h>    
#include <crtdbg.h>   

int __stdcall _openPicture(char *fileName)
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
	if (GetOpenFileName(&opfn));
	strcpy(fileName , strFilename);
	return 0;
}

int __stdcall _openDir(char *dirPath)
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
	return 0;
}

//ֻ�ܴ���Ŀ¼:lpPathֻ����·��  
void find(char *lpPath)
{
	char szFind[MAX_PATH];
	char szFile[MAX_PATH];

	WIN32_FIND_DATA FindFileData;

	strcpy(szFind, lpPath);
	strcat(szFind, "//*.*");

	HANDLE hFind = ::FindFirstFile(szFind, &FindFileData);
	if (INVALID_HANDLE_VALUE == hFind)    return;
	while (TRUE)
	{
		if (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			if (FindFileData.cFileName[0] != '.')
			{
				strcpy(szFile, lpPath);
				strcat(szFile, "//");
				strcat(szFile, FindFileData.cFileName);
				find(szFile);
			}
		}
		else
		{
			string temp_fileName = lpPath;
			temp_fileName += "\\";
			temp_fileName += FindFileData.cFileName;
			char *temp = new char[MAX_PATH];
			strcpy(temp, temp_fileName.c_str());
			g_picArr.push_back(temp);
		}
		if (!FindNextFile(hFind, &FindFileData))
			break;
	}
	FindClose(hFind);
}

//��ͬʱ����Ŀ¼���ļ�:path������·����Ҳ�������ļ����������ļ�ͨ���  
void _find(string path){
	//ȡ·�������һ��"//"֮ǰ�Ĳ���,����"//"  
	string prefix = path.substr(0, path.find_last_of('//') + 1);

	WIN32_FIND_DATA FindFileData;
	HANDLE hFind = ::FindFirstFile(path.c_str(), &FindFileData);
	if (INVALID_HANDLE_VALUE == hFind)
	{
		cout << "�ļ�ͨ�������" << endl;
		return;
	}
	while (TRUE)
	{
		//Ŀ¼  
		if (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			//���ǵ�ǰĿ¼��Ҳ���Ǹ�Ŀ¼  
			if (FindFileData.cFileName[0] != '.')
			{
				//������һ��Ŀ¼  
				_find(prefix + FindFileData.cFileName + "//" + "*.*");
			}
		}
		//�ļ�  
		else
		{
			cout << FindFileData.cFileName << endl;
		}
		if (!FindNextFile(hFind, &FindFileData))
			break;
	}
	FindClose(hFind);
}

inline void EnableMemLeakCheck()
{
#ifdef CRTDBG_MAP_ALLOC
	_CrtDumpMemoryLeaks();
	_CrtSetDbgFlag(_CrtSetDbgFlag(_CRTDBG_REPORT_FLAG) | _CRTDBG_LEAK_CHECK_DF);
#endif
}

int main()
{
	
	char fileName[MAX_PATH];
	//_openPicture(fileName);
	_openDir(fileName);
	find(fileName);
	for (int i = 0; i < g_picArr.size(); i++)
	{
		cout << g_picArr[i] << endl;
		/*delete[] g_picArr[i];
		g_picArr[i] = NULL;*/
	}

	//Mat srcImage = imread(fileName);
	//imshow("����è��", srcImage);
	EnableMemLeakCheck();
	return 0;
}