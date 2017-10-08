/************************************************************************
* Read file name list from file[dir.set], and return vector.
* 'dir.set' format as follow:
* filename1.jpg
* filename2.jpg
* /home/myjpg/filename3.jpg
* ...
* Produce 'dir.set' commond line: 
* Windows:	dir *.jpg /b/s > dir.set
* Linux:	...
* Sandy Yann Nov. 24 2015												
************************************************************************/
#include "common_test.h"

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include <iostream>
using std::string;
using std::vector;

/**
* @brief Window and linux path separator
*/
#ifdef _WIN32
#define PATH_SEPARATOR "\\"
#else
#define PATH_SEPARATOR "/"
#endif


/**
* @brief Get file name from full path name
* @param strFullName: full path name
* @return file name
*/
CTAPI std::string getFileNameFromFullName(std::string strFullName)
{
	int pos = strFullName.rfind(PATH_SEPARATOR);
	std::string fn = strFullName.substr(pos + 1, strFullName.length());
	return fn;
}

// ���ļ�ȫ·�����У���ȡ������n��Ŀ¼�����֣�0=�ļ������Ҳ������ؿ�
CTAPI std::string getDirNameFromFullName(std::string strFullName, int n/*����n������*/)
{
	int pos = strFullName.rfind(PATH_SEPARATOR);
	std::string strLeftDirFn;
	if (n == 0)
	{
		return strFullName.substr(pos + 1, strFullName.length());
	}

	strLeftDirFn = strFullName.substr(0, pos);
	for (int i = 1; i < n; i++)
	{
		pos = strLeftDirFn.rfind(PATH_SEPARATOR);
		if (pos < 0)
		{
			return std::string();
		}
		strLeftDirFn = strLeftDirFn.substr(0, pos);
	}

	// ��󣬷��ص���n��Ŀ¼������
	pos = strLeftDirFn.rfind(PATH_SEPARATOR);
	if (pos < 0)
	{
		return std::string();
	}
	std::string fn = strLeftDirFn.substr(pos + 1, strLeftDirFn.length());
	return fn;
}

std::vector<std::string> readsample(std::string strFN)
{
	std::vector<std::string> vecRes;
	int g_id = 0;
	FILE*pfin  = fopen(strFN.c_str(), "rb");
	if (NULL == pfin)
	{
		std::cout << "can't fopen " << strFN << std::endl;
		return vecRes;
	}

	printf("start readsample\n");
	char as8Buf[2048] = {0};
	while(NULL != fgets(as8Buf, 2048, pfin))
	{
		if(as8Buf[strlen(as8Buf)-1] == '\n')
		{
			as8Buf[strlen(as8Buf)-1] = '\0';
		}
		if(as8Buf[strlen(as8Buf)-1] == '\r')
		{
			as8Buf[strlen(as8Buf)-1] = '\0';
		}

		if (strlen(as8Buf) == 0)
		{
			continue;
		}

		printf("\r%d:",g_id++);

		vecRes.push_back(as8Buf);
	}
	fclose(pfin);

	printf("\nreadsample finish\n");
	return vecRes;
}

// ȥ�������д�mask��ͼ������
std::vector<std::string> readsample_del_mask(std::string strFN)
{
	std::vector<std::string> vecRes;
	std::vector<std::string> vecRes2;

	vecRes = readsample(strFN);
	for (size_t i = 0; i < vecRes.size(); i++)
	{
		if (!strstr((vecRes)[i].c_str(), "mask.jpg"))
		{
			vecRes2.push_back((vecRes)[i]);
		}
	}

	return vecRes2;
}

std::vector<TImgSet> readsample_with_mask(std::string strFN)
{
	std::vector<std::string> vecRes;
	std::vector<TImgSet> vecImgSet;

	vecRes = readsample(strFN);
	size_t total = vecRes.size()/2*2;	// 2�ı���

	// �ж��Ƿ���mask.jpg
	bool bHaveMaskImg = true;
	if(total < 1 || strstr(vecRes[1].c_str(), "mask.jpg") == NULL)
	{
		printf("readsample_with_mask don't have mask file\n");
		bHaveMaskImg = false;
	}

	// ��maskҪ�ɶԶ�ȡ
	if (bHaveMaskImg)
	{
		for (size_t i = 0; i < total; i+=2)
		{
			if (strstr(vecRes[i+1].c_str(), "mask.jpg") != NULL)
			{
				TImgSet tIS;
				tIS.strFn = vecRes[i];
				tIS.strFnMask = vecRes[i+1];
				vecImgSet.push_back(tIS);
			}
			else
			{
				std::cout << "readsample_with_mask have problem!\n";
			}
		}
	}
	else	// û��maskģʽ
	{
		for (size_t i = 0; i < vecRes.size(); i++)
		{
			TImgSet tIS;
			tIS.strFn = vecRes[i];
			vecImgSet.push_back(tIS);
		}
	}

	return vecImgSet;
}

// 1���ļ����£�ѡ��n���ļ�������<=n��ȫѡ)
std::vector<std::string> sampleChoose1DirNFile(std::vector<std::string> vecFn, int n)
{
	std::vector<std::string> vecRes;
	string oldDir;
	int sameNum = 0;
	for (size_t i = 0; i < vecFn.size(); i++)
	{
		string curFnDir = vecFn[i].substr(0, vecFn[i].rfind("\\"));
		if (oldDir == curFnDir)
		{
			sameNum ++;
			if(sameNum <= n)
			{
				vecRes.push_back(vecFn[i]);
			}
		}
		else
		{
			// ��һ�γ���
			oldDir = curFnDir;
			sameNum = 1;
			if(sameNum <= n)
			{
				vecRes.push_back(vecFn[i]);
			}
		}
	}

	return vecRes;
}

// 1���ļ����£��������е��ļ�����vector
std::vector<std::string> sampleChooseOneDir(std::vector<std::string> vecFn, int startId)
{
	std::vector<std::string> vecRes;
	string oldDir;

	for (size_t i = startId; i < vecFn.size(); i++)
	{
		string curFnDir = vecFn[i].substr(0, vecFn[i].rfind("\\"));
		if (oldDir == curFnDir)
		{
			vecRes.push_back(vecFn[i]);
		}
		else
		{
			// ��һ�γ���
			oldDir = curFnDir;
			if (vecRes.size() <= 0)
			{
				vecRes.push_back(vecFn[i]);
				continue;
			}
			
			break;
		}
	}

	return vecRes;
}

/* MKDirTest sample
for(int i = 0;i<2; i++)
{
	char adir[64] = {0};
	sprintf(adir, "xxx/%05d", 20000+i);
	MKDirTest(adir);
}
*/

#ifdef _WIN32
#include <stdio.h>  // for snprintf
#include <string>
#include <vector>
#include <memory>
#include <direct.h>
#include <io.h>
#include <iostream>
#include <fstream>
int MKDirTest(const char *filepath)
{
	char buffer[256];
	char *label;
	strcpy(buffer, filepath);
	string ss;

	label = strtok(buffer, "\\");

	while (label)
	{
		ss += label;
		int xxx = ss.find(".ans");
		int xxx2 = ss.find(".jpg");

		if((int)ss.find(".jpg")>0  || (int)ss.find("JPG")>0 ||
			(int)ss.find(".fea")>0 || 
			(int)ss.find(".ans")>0 || 
			(int)ss.find(".txt")>0 ||
			(int)ss.find(".bmp")>0 || (int)ss.find(".BMP")>0)
		{
			break;
		}

		if (_access(ss.c_str(), 0)<0)
		{
			_mkdir(ss.c_str());
		}

		ss.append("\\");
		label = strtok(NULL, "\\");
	}

	return 0;
}
#else	/* LINUX */
#include <unistd.h>
#include <sys/stat.h>
int MKDirTest(const char *filepath)
{
	char buffer[256];
	char *label;
	strcpy(buffer, filepath);
	string ss;

	label = strtok(buffer, "/");

	while (label)
	{
		ss += label;
		int xxx = ss.find(".ans");
		int xxx2 = ss.find(".jpg");

		if((int)ss.find(".jpg")>0  || (int)ss.find("JPG")>0 ||
			(int)ss.find(".fea")>0 || 
			(int)ss.find(".ans")>0 || 
			(int)ss.find(".txt")>0 ||
			(int)ss.find(".bmp")>0 || (int)ss.find(".BMP")>0)
		{
			break;
		}

		if (access(ss.c_str(), 0)<0) {
			mkdir(ss.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
		}

		ss.append("/");
		label = strtok(NULL, "/");
	}

	return 0;
}
#endif

CTAPI int RMDirTest(const char *filepath)
{
	std::string cmd = std::string("rmdir /s/q ") + filepath;

	return system(cmd.c_str());
}

CTAPI void DelFile(const char* fn)
{
	remove(fn);
}

// ����ͼ����ļ�����ʽ����
bool parseVehLogoFileName(string strfn, 
						  TRect& tLpPosition, 
						  char s8Plate[64], 
						  int& l32LpHorizontalTheta)
{
	//�жϵ�ǰ�Ƿ��г���
	char buffer[128];
	char *pLabel = NULL;
	string s(strfn);
	int nPosss = s.find_last_of("\\");
	s.erase(0, nPosss+1);
	TRect tLocRectRes;
	nPosss = s.find("[");
	string stmp = s.substr(nPosss+1, s.length()-nPosss-6);
	strcpy(buffer, stmp.c_str());
	pLabel = strtok(buffer, ",");
	tLpPosition.x = atoi(pLabel);

	pLabel = strtok(NULL, ",");
	tLpPosition.y = atoi(pLabel);

	pLabel = strtok(NULL, ",");
	tLpPosition.w = atoi(pLabel);

	pLabel = strtok(NULL, ",");
	tLpPosition.h = atoi(pLabel);

	pLabel = strtok(NULL, ",");
	strcpy(s8Plate, pLabel);

	pLabel = strtok(NULL, ",");
	l32LpHorizontalTheta = atoi(pLabel);

	return true;
}

static std::vector<TRect> getAllAnsPos(std::string strTemp)
{
	std::vector<TRect> vecRt;

	for (;;)
	{
		TRect rt;
		sscanf(strTemp.c_str(), "%d,%d,%d,%d", &rt.x, &rt.y, &rt.w, &rt.h);
		size_t p = strTemp.find('|');
		if (p >= 0)
		{
			strTemp = strTemp.substr(p+1, strTemp.length());
			vecRt.push_back(rt);
		}
		else
			break;
	}

	return vecRt;
}

// ����λ�ô��ļ�
// �𰸸�ʽΪ��filename|x,y,w,h|x,y,w,h
CTAPI void parsePosAns(std::string strSetFn, std::vector<std::string>& vecFn, std::vector<std::vector<TRect> >& vvPosRt)
{
	std::vector<std::string> vecRows = readsample(strSetFn);

	vecFn.clear();
	vvPosRt.clear();

	// �������е�λ����Ϣ
	for (size_t i = 0; i < vecRows.size(); i++)
	{
		std::string strCurFn;
		std::string strTemp = vecRows[i];
		std::vector<TRect> vecRt;

		int pos = strTemp.find('|');
		if (pos > 0)	// �ҵ�λ����Ϣ
		{
			strCurFn = strTemp.substr(0, pos);
			vecRt = getAllAnsPos(strTemp.substr(pos + 1, strTemp.length()));
		}
		else	// û���ҵ�λ����Ϣ
		{
			strCurFn = strTemp;
		}

		vecFn.push_back(strCurFn);
		vvPosRt.push_back(vecRt);
	}
}

CTAPI void parsePosAns(std::string strSetFn, std::vector<std::string>& vecFn, std::vector<std::vector<cv::Rect> >& vvPosRt)
{
	std::vector<std::vector<TRect> > vvPosMyRt;
	parsePosAns(strSetFn, vecFn, vvPosMyRt);

	for (size_t i = 0; i < vvPosMyRt.size(); i++)
	{
		std::vector<cv::Rect> vecCvRt;
		for (size_t j = 0; j < vvPosMyRt[i].size(); j++)
		{
			vecCvRt.push_back(cv::Rect(vvPosMyRt[i][j].x, 
				vvPosMyRt[i][j].y, vvPosMyRt[i][j].w, vvPosMyRt[i][j].h));
		}
		vvPosRt.push_back(vecCvRt);
	}
}

CTAPI std::vector<std::string> readsample_from_dir(std::string rootdir)	// ��ȡ�ļ����µ������ļ�
{
	char cmdtxt[256] = {0};
	sprintf(cmdtxt, "dir %s\\*.* /b/s > cur_rfd_.txt", rootdir.c_str());
	system(cmdtxt);

	std::vector<std::string> res = readsample("cur_rfd_.txt");

	// ɾ����ʱ�ļ�
	DelFile("cur_rfd_.txt");
	return res;
}

CTAPI std::vector<std::string> readsample_from_dir_suffix(std::string rootdir, std::string suffix/*.c*/)	// ��ȡ�ļ����µ������ļ�(ĳ�ֺ�׺��
{
	char cmdtxt[256] = {0};
	sprintf(cmdtxt, "dir %s\\*%s /b/s > cur_rfd_.txt", rootdir.c_str(), suffix.c_str());
	system(cmdtxt);

	std::vector<std::string> res = readsample("cur_rfd_.txt");

	// ɾ����ʱ�ļ�
	DelFile("cur_rfd_.txt");
	return res;
}