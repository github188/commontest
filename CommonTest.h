/************************************************************************/
/* ���Գ��ú���
	2015-11-24 Sandy Yann												*/
/************************************************************************/

#include <string>
#include <vector>
#include <stdio.h>
#include <stdlib.h>

#ifndef _COMMON_TEST_H_
#define _COMMON_TEST_H_

#ifdef COMMONTEST_EXPORTS
#define CTAPI __declspec(dllexport)
#else
#define CTAPI /*__declspec(dllimport)*/
#endif

#define CC_CHECK(condition) if(!(condition)){printf("FAIL:%s, %d\n", __FUNCTION__, __LINE__);}

#if 1	// ��opencv���
#include <opencv2/opencv.hpp>
// ��ȡMat��roiͼ�񣬳����߽粿�ֲ�ɫΪ110
CTAPI cv::Mat getRoiFromMat1(cv::Mat src, cv::Rect roiRt);
// ���ݳ���λ�õ����ģ��������ܰ���ȱ������ߣ�ͼ���⣬��ֵ110
CTAPI cv::Mat getRoiFromMatByLpPos(cv::Mat src, cv::Rect tRtLp, float f32L,float f32R,float f32T,float f32B, int l32LpEx/*������������*/);

/* ��Գ���λ�ô𰸣�����Ͳ鿴���ߣ���Ӧ�Ĵ𰸶�д����,
	���λ����Ĳ��ԣ���ʵ������������𰸶�д	*/
typedef struct
{
	std::vector<cv::Rect> vecRtPos;  // һ��ͼƬ�еĳ���λ��
	std::string fileName;       // ��ǰͼƬ������
}TPosAns;  // �𰸽ṹ��

CTAPI std::vector<TPosAns> readPosAns(std::string strAnsFn);
CTAPI void writePosAns(std::string strAnsFn, std::vector<TPosAns> tPosAns);

#endif

// ��TImage��ص��ԣ���ʾ�ȹ���
#if 1
#include "typedef.h"

CTAPI void imshowTImage(const std::string& winname, TImage* ptImg, int flags=1);
CTAPI void waitKeyTImage(int delay=0);

// ��ȡTImage��roiͼ�񣬳����߽粿�ֲ�ɫΪ110
CTAPI void getRoiFromTImage1(const TImage *ptSrc, TImage *ptRoi, TRect tRtRoi);

// ���ݳ���λ�ã���ȡ����ͼƬ
CTAPI l32 GetRoi(const TRect tNumPlate, TImage* tSrc, TImage* tDst, const d64 d64Hor, const d64 d64Ver);

/*=======================================================================
Function : ���ݳ���λ�ã�������ߺ��roi��������䵽ͼ���⣬��ֵ110��
		   ͬʱ�����ı�����lpex���غ󣬳���λ��Ҳ���ó�110   
Param    : ptSrc[in]	ԭʼͼ��
		   ptRoi[in]	���ص�roiͼ���ڴ����ʵ�ֿ��٣��µĿ�ߣ��ڲ��ḳֵ
		   f32L,f32R,f32T,f32B[in] ���ݳ���λ�ã����������¸����䳵�ƿ�ȵı���
		   lpex[in]		������������������
Return   : void
=========================================================================*/
CTAPI void getRoiTImageByLpPos(const TImage *ptSrc, TImage *ptRoi, const TRect tRtLp, 
						 float f32L,float f32R,float f32T,float f32B, l32 l32LpEx);

inline TRect Rect2TRect(cv::Rect rt)
{
	TRect tRt;
	tRt.l32Left = rt.x;
	tRt.l32Top = rt.y;
	tRt.l32Width = rt.width;
	tRt.l32Height = rt.height;
	return tRt;
};
inline cv::Rect TRect2Rect(TRect tRt)
{
	cv::Rect rt;
	rt.x = tRt.l32Left;
	rt.y = tRt.l32Top;
	rt.width = tRt.l32Width;
	rt.height = tRt.l32Height;
	return rt;
};

// ���������ļ���
CTAPI bool parseVehLogoFileName(std::string strfn, 
						  TRect& tLpPosition, 
						  char s8Plate[64], 
						  l32& l32LpHorizontalTheta);
// ����λ�ô��ļ�
// �𰸸�ʽΪ��filename|x,y,w,h|x,y,w,h
CTAPI void parsePosAns(std::string strSetFn, std::vector<std::string>& vecFn, std::vector<std::vector<TRect>>& vvPosRt);
CTAPI void parsePosAns(std::string strSetFn, std::vector<std::string>& vecFn, std::vector<std::vector<cv::Rect>>& vvPosRt);

CTAPI void  ConvertRGB2NV12(u8 *puYUVSP, u8 *pu8RGB, l32 l32Width, l32 l32Height, l32 stride);
CTAPI void  ConvertRGB2I420(u8 *puYUV420, u8 *pu8RGB, l32 l32Width, l32 l32Height, l32 l32Stride);

CTAPI void FveCvtNV12BGR24(const u8 *puYUVSP, l32 l32Width, l32 l32Height, u8 *pu8RGB, l32 l32DstStride);
CTAPI void FvevehI420BGR24(TImage *tYuvImg, l32 l32Width, l32 l32Height, u8 *pu8RGB, l32 l32DstStride, BOOL bFlip);

// yuv ת���� mat
CTAPI cv::Mat Yuv2Mat(const char *pYuv420, int width, int height);

CTAPI TImage* creatTImage(cv::Mat src, EImageType eImgType);
#define Mat2TImage creatTImage
CTAPI void releaesTImage(TImage** pptImg);

#endif


// ���ļ�ȫ·�����У���ȡ�ļ�����(\����Ĳ���)
CTAPI std::string getFileNameFromFullName(std::string strFullName);

// ���ļ�ȫ·�����У���ȡ������n��Ŀ¼�����֣�0=�ļ������Ҳ������ؿ�
CTAPI std::string getDirNameFromFullName(std::string strFullName, int n/*����n������*/);

/*=======================================================================
Function : Read file name list  
Param    : strFN[in] dir.set
Return   : vector<string>
=========================================================================*/
CTAPI std::vector<std::string> readsample(std::string strFN);
CTAPI std::vector<std::string> readsample_from_dir(std::string rootdir);	// ��ȡ�ļ����µ������ļ�
CTAPI std::vector<std::string> readsample_from_dir_suffix(std::string rootdir, std::string suffix/*.c*/);	// ��ȡ�ļ����µ������ļ�(ĳ�ֺ�׺��
CTAPI std::vector<std::string> readsample_del_mask(std::string strFN);
// 1���ļ����£�ѡ��n���ļ�������<=n��ȫѡ)
CTAPI std::vector<std::string> sampleChoose1DirNFile(std::vector<std::string> vecFn, int n);
// 1���ļ����£��������е��ļ�����vector
CTAPI std::vector<std::string> sampleChooseOneDir(std::vector<std::string> vecFn, int startId);

// �ļ�copy��Դ�ļ�����Ŀ���ļ���
CTAPI void copyFile2DstFn(std::string strSrcFn, std::string strDstFn);

typedef struct
{
	std::string strFn;		// ԭʼͼ���ļ���
	std::string strFnMask;	// ԭʼͼ���Ӧ��mask�ļ���
}TImgSet;
// ��set����ȡ�ɶԵ��ļ����֣�һ����ԭʼͼ��һ����mask
CTAPI std::vector<TImgSet> readsample_with_mask(std::string strFN);

/*=======================================================================
Function : Create folder by path  
Param    : 
Return   : void
=========================================================================*/
CTAPI int MKDirTest(const char *filepath);
CTAPI int RMDirTest(const char *filepath);	// ����ļ���
#define DeleteDirTest RMDirTest
#define DelDir RMDirTest
// ɾ��ȫ�����ļ�
CTAPI void DelFile(const char* fn);


// ���̲߳���
typedef void* (__stdcall*MultiOpen)(int idx/*�߳����*/);
typedef int (__stdcall*MultiProcess)(void* pvHandle, const char* fn, void *pvUser);
typedef int(__stdcall*MultiClose)(void*pvHandle);
CTAPI void multiThrdTest(std::string strSetFn,	// set�ļ���
						 int thrnum,	// �߳���Ŀ>=1
						 MultiOpen myOpen,
						 MultiProcess myProcess,
						 MultiClose myClose,
						 void *pvUser);
CTAPI void multiThrdTest(std::vector<std::string> vecFn,	// set�ļ���
	int thrnum,	// �߳���Ŀ>=1
	MultiOpen myOpen,
	MultiProcess myProcess,
	MultiClose myClose,
	void *pvUser);

/*=======================================================================
Function : Show retrieval result  
Param    : vecScore[in] The score of each image
		   vecFn[in] Image library.
		   strRefFn[in] Reference image
		   bSave[in] false: show result; true: not show, directly save result
Return   : void
=========================================================================*/
CTAPI void showRetrievalResut(std::vector<float> vecScore, std::vector<std::string> vecFn,
							  std::string strRefFn, bool bSave, std::string svpath);
							  


// ͨ�����������
#if 1
// ���̲߳���ʱ��vector�ֶ�(ƽ��)
CTAPI std::vector<std::string> vecStrSection(int totalSection, int curId/*��0��ʼ*/,
									   std::vector<std::string> vecAll);
CTAPI std::vector<TImgSet> vecStrSection(int totalSection, int curId/*��0��ʼ*/,
										 std::vector<TImgSet> vecAll);

// �ļ�ͷ����¼�ļ����ж��ٸ�������
CTAPI void writeHeader(FILE* pfOut, int num);
CTAPI void writeHeader3P(FILE* pfOut, int num, int dim);

// ���ļ�ͷ,��ı䵱ǰλ�õ��ļ�ͷ
CTAPI void readHeader(FILE* pfOut, int& num);
CTAPI void readHeader3P(FILE* pfOut, int& num, int &dim);

CTAPI void writeText(FILE* pfOut, int val);
CTAPI void writeText(FILE* pfOut, float val);
CTAPI void writeText(FILE* pfOut, char val);
CTAPI void writeVector(FILE* pfOut, std::vector<float> val);

CTAPI void readText(FILE* pfIn, int& val);
CTAPI void readText(FILE* pfIn, float& val);
CTAPI void readText(FILE* pfIn, char& val);
CTAPI void readVector(FILE* pfIn, std::vector<float>& vecVal);
CTAPI void readVector2Buf(FILE* pfIn, float * pArr);

// sting ��д
CTAPI void writeString(FILE* pfOut, std::string str);
inline std::string readString(FILE* pfIn)
{
	std::string str;
	int len = 0;
	readText(pfIn, len);
	if (len < 0)// �ļ���β
	{
		return str;
	}

	char* ptext = new char[len + 1];
	memset(ptext, 0, len + 1);
	fread(ptext, 1, len, pfIn);
	ptext[len] = 0;
	str = std::string(ptext);
	delete[] ptext;

	return str;
}
inline BOOL readString2Buf(FILE* pfIn, char* pBuf, int bufSZ)
{
	std::string str;
	int len = -1;
	readText(pfIn, len);
	if (len > bufSZ || len < 0)// �ļ���β
	{
		printf("Err: fun = ; lineNo = \n", __FUNCTION__, __LINE__);
		return FALSE;
	}

	fread(pBuf, 1, len, pfIn);
	pBuf[len] = 0;
	return TRUE;
}

// �����д
CTAPI void writeArray(FILE* pfOut, char* pf32Arr, int lenlen);
inline char* readArray(FILE* pfIn, int& len){
	readText(pfIn, len);
	//fscanf(pfIn, "%d,", &len);

	char* ptext = new char[len + 1];
	fread(ptext, 1, len, pfIn);
	ptext[len] = 0;

	return ptext;
}

// true���ļ���β��
CTAPI bool fileEof(FILE* pfIn);
#endif

// ���������ļ�����
#if 1
// �������ļ�����ȡ�����ļ����
CTAPI void* readConfig(const char* ps8ConfigFn);
CTAPI void releaseConfig(void**ppvHandle);

// ���������ļ��������ȡĳһ���ֵ
CTAPI bool parseConfig(void* pvHandle, char* ps8Label, int &val);
CTAPI bool parseConfig(void* pvHandle, char* ps8Label, float &val);
CTAPI bool parseConfig(void* pvHandle, char* ps8Label, std::string & strOut);
CTAPI bool parseConfig(void* pvHandle, char* ps8Label, bool &val);
/* ��������
class CConfigParam
{
public:
	CConfigParam(std::string strConfigFn)
	{
		m_pvHandle = readConfig(strConfigFn.c_str());
		if (NULL == m_pvHandle)
		{
			std::cout << "can't open " << strConfigFn << " \n";
			exit(0);
		}

		parseConfig(m_pvHandle, "m_strAlgType",	m_strAlgType);
	}
	~CConfigParam()
	{
		releaseConfig(&m_pvHandle);
	}

	std::string m_strAlgType;
private:
	void *m_pvHandle;
	
};
*/
#endif

CTAPI std::string getExePath();

// ��õ�ǰʱ�䣬��ȷ����; ��ʽΪ��year-month-day h:m:s
CTAPI std::string getCurStrTime();
#if 1	// ��־����
/* ��ʼ����־�����֣��������ʼ����ʹ��Ĭ������log_ʱ��.txt,
	���һ������ǰ����־�ļ�ɾ����
	��������ʱ�����ʼ����־���ļ�����	��	*/
CTAPI void InitialLog(const char* pLogFn);

// ���е���־��Ϣ���������log_time.log��
CTAPI void printfLog(const char* pLogText);
#define PrintfLogArg(_format, ...) {\
	char atext[1024] = { 0 };		\
	sprintf(atext, "%s_%d : ", __FUNCTION__, __LINE__);	\
	sprintf(atext + strlen(atext), _format, ##__VA_ARGS__);	\
	printfLog(atext);			\
}
#endif


#endif
