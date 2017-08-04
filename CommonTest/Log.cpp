/**/

/**************************************************************
	����־��صĴ���
	Sandy Yann 
	2016-04-02
**************************************************************/

/************************************************************************
/* ��־����Ϊ��ÿ����־�������ǣ����������м���ʱ�䣬�����ͱ���ɾ��һ��ʱ��
	֮ǰ����־�ļ� 	

	2017-2-14 �޸�Ϊ������������ʼ��һ����־�����֣�û����־����ʱ��û����־��
				����������־�ļ����ֿ��Գ�ʼ����û�г�ʼ��������Ĭ�����֣���
				��������Ȼ��ʱ����Ϣ*/
/************************************************************************/
#include "CommonTest.h"
#include "opencv2/opencv.hpp"

#include <Windows.h>
#include <time.h>
#include <string>
#include <stdio.h>
#include <stdlib.h>

#include<io.h>

#define SHOW_LOG_CONSOLE 1	// �Ƿ���ʾ��־��������
#define LOG_NAME	"alg"	// ��ʼ����־�����֣����û�е���InitialLog��	

CTAPI std::string getExePath()
{
	char szFilePath[MAX_PATH + 1] = { 0 };
	GetModuleFileNameA(NULL, szFilePath, MAX_PATH);
	(strrchr(szFilePath, '\\'))[0] = 0; // ɾ���ļ�����ֻ���·���ִ�  
	std::string path = szFilePath;
	return path;
}

static void judgeTimeAndDel(std::string strFullFn)
{
	std::string fn = getFileNameFromFullName(strFullFn);
	int curY = 0;
	int curM = 0;
	int curD = 0;
	int oldY = 0;
	int oldM = 0;
	int oldD = 0;
	std::string strTemp = fn.substr(fn.find("_") + 1, fn.length() - fn.find("_") - 1);
	sscanf(strTemp.c_str(), "%d-%d-%d", &oldY, &oldM, &oldD);

	time_t curTm = time(0);
	char tmp[64] = { 0 };
	strftime(tmp, sizeof(tmp), "%Y-%m-%d-%H-%M-%S", localtime(&curTm));
	sscanf(tmp, "%d-%d-%d", &curY, &curM, &curD);

	std::cout << strFullFn << std::endl;

	// ���log��ʽ����
	if (oldY == 0 || oldM == 0 || oldD == 0)
	{
		return;
	}

	if (curY * 12 * 30 + curM * 30 + curD - oldY * 12 * 30 - oldM * 30 - oldD >= 28)
	{
		DelFile(strFullFn.c_str());
	}
}

// ɾ��һ������ǰ����־�ļ�,����̫��
static void delOneMonthAgoLog()
{
	std::string strRootPath = getExePath();
	std::string strLogFullName = strRootPath + "\\" + "*.log";

	intptr_t Handle;
	struct _finddata_t FileInfo;
	if ((Handle = _findfirst(strLogFullName.c_str(), &FileInfo)) == -1L)
	{
		//printf("û���ҵ�ƥ�����Ŀ\n");
		return;
	}
	else
	{
		std::string curFullFn = strRootPath + "\\" + FileInfo.name;
		judgeTimeAndDel(curFullFn);
		while (_findnext(Handle, &FileInfo) == 0)
		{
			curFullFn = strRootPath + "\\" + FileInfo.name;
			judgeTimeAndDel(curFullFn);
			//printf("%s\n", FileInfo.name);
		}

		_findclose(Handle);
	}
}

static std::string getLogFileName(std::string strLogFn)
{
	// ɾ��һ������ǰ����־�ļ�
	delOneMonthAgoLog();

	time_t t = time(0);
	char tmp[64];
	strftime(tmp, sizeof(tmp), "%Y-%m-%d-%H-%M-%S", localtime(&t));

	strLogFn = strLogFn.empty() ? "alg" : strLogFn;
	return strLogFn + "_" + std::string(tmp) + ".log";
}
static std::string g_LogFileName = getLogFileName(std::string());

// ��õ�ǰʱ�䣬��ȷ����; ��ʽΪ��year-month-day h:m:s
CTAPI std::string getCurStrTime()
{
	time_t t = time(0);
	char tmp[64];
	strftime(tmp, sizeof(tmp), "%Y-%m-%d %H:%M:%S", localtime(&t));

	return std::string(tmp);
}

// ���е���־��Ϣ���������log.txt
CTAPI void printfLog(const char* pLogText)
{
	std::string strLogFullName = getExePath() + "\\" + g_LogFileName;

	static FILE* g_pFOut = NULL;
	if (g_pFOut == NULL)
	{
		g_pFOut = fopen(strLogFullName.c_str(), "a+");	// ����ļ������ڣ��򴴽�
		if (NULL == g_pFOut)
		{
			printf("can't write log\n");
			return;
		}
	}

	time_t t = time(0);
	char tmp[64];
	strftime(tmp, sizeof(tmp), "%Y-%m-%d %H:%M:%S", localtime(&t));

	fprintf(g_pFOut, "%s : %s", tmp,  pLogText);
	fflush(g_pFOut);
	
	printf("%s : %s\n", tmp, pLogText);
}

/* ��ʼ����־�����֣��������ʼ����ʹ��Ĭ������log_ʱ��.txt,
���һ������ǰ����־�ļ�ɾ��	*/
CTAPI void InitialLog(const char* pLogFn)
{
	// ��ʼ����־����,Ĭ��Ϊ"alg_time.log"
	g_LogFileName = getLogFileName(pLogFn);

}