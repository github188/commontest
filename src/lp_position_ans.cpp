/************************************************************************
* LP answer read and write, the calibrate format as follow:
* First line: rootpath
* Other line: file full name|%d,%d,%d,%d|x,y,w,h
* Sandy Yann 2016-1-4 : 
* Iniatlly, this tool was writen for KEDACOM testing. 
* Old calibration tool was palced at:
* E:\svnroot\ai_cbb\trunk\ai_cbb_dev\A0611_fvextract\algorithm\�����궨����\tool_label_vehicle\Release
* Sandy Yann September 17 2017: 
* And Later, it can be used as a public tool, public data format.
***********************************************************************/
#include <opencv2/opencv.hpp>
#include "typedef.h"
#include "common_test.h"

std::vector<TPosAns> readPosAns(std::string strAnsFn)
{
	std::vector<TPosAns> vecTPosAns;
	FILE *pOut = fopen(strAnsFn.c_str(), "rb");
	if(pOut == NULL)
	{
		return vecTPosAns;
	}

#define MaxLength 1024*50

	char atext[MaxLength] = {0};
	char atmp[MaxLength] = {0};

	// ��һ�У���·��
	fgets(atext, 1024, pOut);
	while(atext[strlen(atext)-1] == 10 || atext[strlen(atext)-1] == 13)
	{
		atext[strlen(atext)-1] = 0;
	}

	// �ڶ��п�ʼ���ļ�
	while(!feof(pOut))
	{
		memset(atext, 0, MaxLength);
		if(fgets(atext, MaxLength, pOut))
		{
			if(strlen(atext))
			{
				while(atext[strlen(atext)-1] == 10 || atext[strlen(atext)-1] == 13)
				{
					atext[strlen(atext)-1] = 0;
				}
			}

			TPosAns curAns;
			char atemp[MaxLength] = {0};
			char *pCurPos = strstr(atext, "|");
			if(!pCurPos)
			{
				curAns.fileName = std::string(atext);
				continue;
			}

			// �����ļ���
			memcpy(atemp, atext, pCurPos - atext);
			curAns.fileName = std::string(atemp);

			pCurPos = strstr(pCurPos, "|");
			while(pCurPos)
			{
				// ��������λ��
				cv::Rect rt;
				sscanf(pCurPos + 1, "%d,%d,%d,%d", &rt.x, &rt.y, &rt.width, &rt.height);

				curAns.vecRtPos.push_back(rt);

				pCurPos = strstr(pCurPos + 1, "|");
			}

			// һ��ͼ��Ĵ�
			vecTPosAns.push_back(curAns);
		}
	}

	fclose(pOut);

	return vecTPosAns;
}

void writePosAns(std::string strAnsFn, std::vector<TPosAns> tPosAns)
{
	FILE *pOut = fopen(strAnsFn.c_str(), "wb");

	char atext[1024] = {0};

	// ��һ�У���·��
	fprintf(pOut, "rootpath\n");

	std::vector<TPosAns>::iterator iter;
	for (iter = tPosAns.begin(); iter != tPosAns.end() ; iter++)
	{
		// ������ļ���
		fprintf(pOut, "%s", iter->fileName.c_str());

		// �����λ������
		std::vector<cv::Rect> curRt = iter->vecRtPos;
		for(int i = 0; i < curRt.size(); i++)
		{
			fprintf(pOut, "|%d,%d,%d,%d", 
				curRt[i].x,curRt[i].y,
				curRt[i].width,curRt[i].height);
		}
		fprintf(pOut, "\n");
	}

	fclose(pOut);
}