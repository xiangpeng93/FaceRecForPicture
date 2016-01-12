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
	
	char strFilename[MAX_PATH];//存放文件名  
	//初始化  
	ZeroMemory(&opfn, sizeof(OPENFILENAME));
	opfn.lStructSize = sizeof(OPENFILENAME);//结构体大小  
	//设置过滤  
	opfn.lpstrFilter = "所有文件\0*.*";
	//默认过滤器索引设为1  
	opfn.nFilterIndex = 1;
	//文件名的字段必须先把第一个字符设为 \0  
	opfn.lpstrFile = strFilename;
	opfn.lpstrFile[0] = '\0';
	opfn.nMaxFile = sizeof(strFilename);
	//设置标志位，检查目录或文件是否存在  
	opfn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;
	// 显示对话框让用户选择文件  
	if (GetOpenFileName(&opfn));
	strcpy(fileName , strFilename);
	return 0;
}

int __stdcall _openDir(char *dirPath)
{
	TCHAR szPathName[MAX_PATH];
	BROWSEINFO bInfo = { 0 };
	bInfo.hwndOwner = GetForegroundWindow();				//父窗口  
	bInfo.lpszTitle = TEXT("浏览文件夹```选择文件夹");
	bInfo.ulFlags = BIF_RETURNONLYFSDIRS | BIF_USENEWUI		/*包含一个编辑框 用户可以手动填写路径 对话框可以调整大小之类的..*/ 
		| BIF_UAHINT										/*带TIPS提示*/ 
		| BIF_NONEWFOLDERBUTTON								/*不带新建文件夹按钮*/;
	LPITEMIDLIST lpDlist;
	lpDlist = SHBrowseForFolder(&bInfo);
	if (lpDlist != NULL)//单击了确定按钮  
	{
		SHGetPathFromIDList(lpDlist, szPathName);
		strcpy(dirPath, szPathName);
	}
	return 0;
}

//只能处理目录:lpPath只能是路径  
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

//可同时处理目录和文件:path可以是路径，也可以是文件名，或者文件通配符  
void _find(string path){
	//取路径名最后一个"//"之前的部分,包括"//"  
	string prefix = path.substr(0, path.find_last_of('//') + 1);

	WIN32_FIND_DATA FindFileData;
	HANDLE hFind = ::FindFirstFile(path.c_str(), &FindFileData);
	if (INVALID_HANDLE_VALUE == hFind)
	{
		cout << "文件通配符错误" << endl;
		return;
	}
	while (TRUE)
	{
		//目录  
		if (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			//不是当前目录，也不是父目录  
			if (FindFileData.cFileName[0] != '.')
			{
				//查找下一级目录  
				_find(prefix + FindFileData.cFileName + "//" + "*.*");
			}
		}
		//文件  
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
	//imshow("祈福的猫咪", srcImage);
	EnableMemLeakCheck();
	return 0;
}