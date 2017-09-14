/*********************************************************
* Licence plate detect and recognize related test tool
* Sandy Yann. Sep. 11 2017
*********************************************************/

#ifndef _TEST_LP_H_
#define _TEST_LP_H_

#if 1	// ��opencv���
#include <opencv2/opencv.hpp>
// ��ȡMat��roiͼ�񣬳����߽粿�ֲ�ɫΪ110
CTAPI cv::Mat getRoiFromMat1(cv::Mat src, cv::Rect roiRt);
// ���ݳ���λ�õ����ģ��������ܰ���ȱ������ߣ�ͼ���⣬��ֵ110
CTAPI cv::Mat getRoiFromMatByLpPos(cv::Mat src, cv::Rect tRtLp, float f32L, float f32R, float f32T, float f32B, int l32LpEx/*������������*/);

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

CTAPI void imshowTImage(const std::string& winname, TImage* ptImg, int flags = 1);
CTAPI void waitKeyTImage(int delay = 0);

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
	float f32L, float f32R, float f32T, float f32B, l32 l32LpEx);

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
CTAPI void parsePosAns(std::string strSetFn, std::vector<std::string>& vecFn, std::vector<std::vector<TRect> >& vvPosRt);
CTAPI void parsePosAns(std::string strSetFn, std::vector<std::string>& vecFn, std::vector<std::vector<cv::Rect> >& vvPosRt);

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


#endif /* _TEST_LP_H_ */