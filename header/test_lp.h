/*********************************************************
* Licence plate detect and recognize related test tool
* Sandy Yann. Sep. 11 2017: this was placed in 'test_lp.h'
* September 17 2017: Chinses of all public part was instead of English. 
*********************************************************/
#include "typedef.h"
#include "color_space_convert.h"

#ifndef _TEST_LP_H_
#define _TEST_LP_H_

#if 1	// ��opencv���
#include <opencv2/opencv.hpp>
// ��ȡMat��roiͼ�񣬳����߽粿�ֲ�ɫΪ110
CTAPI cv::Mat getRoiFromMat1(cv::Mat src, cv::Rect roiRt);
// ���ݳ���λ�õ����ģ��������ܰ���ȱ������ߣ�ͼ���⣬��ֵ110
CTAPI cv::Mat getRoiFromMatByLpPos(cv::Mat src, cv::Rect tRtLp, float f32L, float f32R, float f32T, float f32B, int l32LpEx/*������������*/);

/**
* @brief Get roi image from NV12.
* @param pNV12: src nv12 image.
* @param width: nv12 width
* @param height: nv12 height
* @param rtRoi: ROI
* @param pNV12ROI: Out roi NV12 image.
*/
void getRoiNV12(const uint8_t* pNV12, int width, int height, cv::Rect rtRoi, uint8_t* pNV12ROI);

/**
* @brief Get roi image from NV12 image.
* @param w: NV12 image width
* @param h: NV12 image height
* @param rtRoi: ROI, member must be even numbers.
*/
cv::Mat getRoiFromNV12(const uint8_t* pNV12, int w, int h, cv::Rect rtRoi);

/* LP answer format struct */
typedef struct
{
	std::vector<cv::Rect> vecRtPos;	// All LP rectangles in one image.
	std::string fileName;			// Current image full name.
}TPosAns; 

/**
* @brief Read lp answer from file.
* @return veter of struct of 'TPosAns'
*/
CTAPI std::vector<TPosAns> readPosAns(std::string strAnsFn);

/**
* @brief Save lp answer to file.Format=filename|x,y,w,h|x1,y1,w1,h1...
* @param strAnsFn: the answer file name.
* @param tPosAns: the lp calibration result of one image.
*/
CTAPI void writePosAns(std::string strAnsFn, std::vector<TPosAns> tPosAns);

#endif

// ��TImage��ص��ԣ���ʾ�ȹ���
#if 1
#include "typedef.h"

typedef enum emImgType
{
	GRAY = 0,
	YUV = 1,	// Y,U,V, planer storage.Aslo call I420.
	NV12 = 2,	// Y single planer, uv mix storage.
	BGR24 = 3,
	BGRP = 4,	// Plane, split storage.
}ImgType;

/* Image type, as OpenCV IplImage */
typedef struct tagImage
{
	char* pu8Data;
	int width;
	int height;
	int channel;
	int step;
	uint32_t imgType;
}TImage;

inline TImage* creatTImage(cv::Mat src, ImgType eImgType)
{
	TImage* ptImg = new TImage();
	int nWidth = src.cols;
	int nHeight = src.rows;
	char * pu8YUV420 = new char[src.step * src.rows];

	ptImg->imgType = (uint32_t)eImgType; //NV12; //I420;
	ptImg->width = nWidth;
	ptImg->height = nHeight;

	if (YUV == eImgType || NV12 == eImgType)
	{
		ptImg->step = nWidth;
		ptImg->pu8Data = pu8YUV420;

		if (YUV == eImgType)
		{
			bgr24_to_yuv((uint8_t*)pu8YUV420, (uint8_t*)src.data, nWidth, nHeight, (int)src.step);
		}
		else
		{
			bgr24_to_nv12((uint8_t*)pu8YUV420, (uint8_t*)src.data, nWidth, nHeight, (int)src.step);
		}
	}
	else if (BGR24 == eImgType)
	{
		ptImg->step = (int)src.step;
		ptImg->pu8Data = pu8YUV420;

		memcpy(pu8YUV420, src.data, src.step * src.rows);
	}
	else if (GRAY == eImgType)
	{
		if (src.channels() > 1)
		{
			cv::cvtColor(src, src, 6/*CV_BGR2GRAY*/);
		}
		ptImg->step = (int)src.step;
		ptImg->pu8Data = pu8YUV420;
		memcpy(pu8YUV420, src.data, src.step * src.rows);
	}
	else
	{
		printf("Err: Ŀǰ��֧�ֵ�����");
		delete[] pu8YUV420;
		delete ptImg;
		return NULL;
	}

	//cv::Mat maty = cv::Mat(ptImg->height, ptImg->width, CV_8UC1, ptImg->pu8Data);
	//cv::imshow("y", maty);
	//cv::Mat matu = cv::Mat(ptImg->atPlane[1].height, ptImg->atPlane[1].width, CV_8UC1, ptImg->atPlane[1].pu8Data);
	//cv::imshow("matu", matu);
	//cv::Mat matv = cv::Mat(ptImg->atPlane[2].height, ptImg->atPlane[2].width, CV_8UC1, ptImg->atPlane[2].pu8Data);
	//cv::imshow("matv", matv);
	//cv::waitKey();

	return ptImg;
}

inline void releaesTImage(TImage** pptImg)
{
	TImage* ptImg = (TImage*)*pptImg;
	if (ptImg)
	{
		if (ptImg->pu8Data)
		{
			delete[] ptImg->pu8Data;
		}
		delete ptImg;
		*pptImg = NULL;
	}
}

CTAPI void imshowTImage(const std::string& winname, TImage* ptImg, int flags = 1);
CTAPI void waitKeyTImage(int delay = 0);

// ��ȡTImage��roiͼ�񣬳����߽粿�ֲ�ɫΪ110
CTAPI void getRoiFromTImage1(const TImage *ptSrc, TImage *ptRoi, TRect tRtRoi);

// ���ݳ���λ�ã���ȡ����ͼƬ
CTAPI int GetRoi(const TRect tNumPlate, TImage* tSrc, TImage* tDst, const double d64Hor, const double d64Ver);

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
	float f32L, float f32R, float f32T, float f32B, int l32LpEx);

inline TRect Rect2TRect(cv::Rect rt)
{
	TRect tRt;
	tRt.x = rt.x;
	tRt.y = rt.y;
	tRt.w = rt.width;
	tRt.h = rt.height;
	return tRt;
};

inline cv::Rect TRect2Rect(TRect tRt)
{
	cv::Rect rt;
	rt.x = tRt.x;
	rt.y = tRt.y;
	rt.width = tRt.w;
	rt.height = tRt.h;
	return rt;
};

// ���������ļ���
CTAPI bool parseVehLogoFileName(std::string strfn,
	TRect& tLpPosition,
	char s8Plate[64],
	int& l32LpHorizontalTheta);

/**
* @brief Parse position answer file, is same to 'readPosAns', Answer format��filename|x,y,w,h|x,y,w,h
* @param strSetFn: the answer file name.
* @param vecFn: vector of all LP file full names.
* @param vvPosRt: first vector for all files, second vector for all rectangle of one file.
*/
CTAPI void parsePosAns(std::string strSetFn, std::vector<std::string>& vecFn, std::vector<std::vector<TRect> >& vvPosRt);
CTAPI void parsePosAns(std::string strSetFn, std::vector<std::string>& vecFn, std::vector<std::vector<cv::Rect> >& vvPosRt);

CTAPI TImage* creatTImage(cv::Mat src, ImgType eImgType);
#define Mat2TImage creatTImage
CTAPI void releaesTImage(TImage** pptImg);

#endif


#endif /* _TEST_LP_H_ */