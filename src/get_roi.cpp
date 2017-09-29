/******************************************************************************
����һ��ͼ���rect����ȡ��Ӧ��roiͼ��
2016-04-03
Sandy Yann
******************************************************************************/

#include <string.h>
#include "CommonTest.h"
#include <opencv2/opencv.hpp>
#include "typedef.h"


// ���roi yxp�汾
#if 1

// �����߽粿�ֲ�ɫΪ110
cv::Mat getRoiFromMat1(cv::Mat src, cv::Rect roiRt)
{
	if (roiRt.width <=0 || roiRt.height <= 0)
	{
		PrintfLogArg("paramter err\n");
	}

	//cv::rectangle(src, roiRt, cv::Scalar(0,255,0), 2);
	//cv::imshow("src", src);
	//cv::waitKey(0);

	cv::Mat roi = cv::Mat(roiRt.height, roiRt.width, src.type());
	roi.setTo(110);

	// ��ԭʼͼ���е���ʵλ��
	int x1,x2,y1,y2;
	x1 = MAX(0, roiRt.x);
	y1 = MAX(0, roiRt.y);
	x2 = MIN(src.cols - 1, roiRt.width + roiRt.x - 1);
	y2 = MIN(src.rows - 1, roiRt.height + roiRt.y - 1);

	int rw = x2 - x1 + 1;
	int rh = y2 - y1 + 1;

	// ��Ӧ��roiͼ���еĿ�ʼλ��
	int roix1 = MAX(0, -roiRt.x);
	int roiy1 = MAX(0, -roiRt.y);

	uchar* psrcbuf = src.data + y1 * src.step + src.channels()*x1;
	uchar* proibuf = roi.data + roiy1 * roi.step + roi.channels()*roix1;
	for (int h = 0; h < rh; h++)
	{
		memcpy(proibuf, psrcbuf, rw * src.channels());
		psrcbuf += src.step;
		proibuf += roi.step;
	}

	return roi;
}

// ���ݳ���λ�õ����ģ��������ܰ���ȱ������ߣ�ͼ���⣬��ֵ110
cv::Mat getRoiFromMatByLpPos(cv::Mat src, cv::Rect tRtLp, float f32L,float f32R,float f32T,float f32B, int l32LpEx)
{
	// ���Ƶ�����
	int cx = tRtLp.x + tRtLp.width/2;
	int cy = tRtLp.y + tRtLp.height/2;

	// �������ģ���4������
	int x1 = (int)(cx - f32L * tRtLp.width);
	int x2 = (int)(cx + f32R * tRtLp.width);
	int y1 = (int)(cy - f32T * tRtLp.width);
	int y2 = (int)(cy + f32B * tRtLp.width);

	cv::Rect rtRoi;
	rtRoi.x=x1;
	rtRoi.y=y1;
	rtRoi.width = x2 - x1 + 1;
	rtRoi.height = y2 - y1 + 1;

	cv::Mat roiMat = getRoiFromMat1(src, rtRoi);

	// Ĩ��roi�еĳ���
	// ��Ӧ��roiͼ���еĿ�ʼλ��
	// ���ߺ���³���λ�ã�����Ĩ��roi�еĳ���
	cv::Rect rtLp2 = tRtLp;
	rtLp2.x -= l32LpEx;
	rtLp2.y -= l32LpEx;
	rtLp2.width += 2*l32LpEx;
	rtLp2.height += 2*l32LpEx;

	int roiStartPosX = rtLp2.x - x1;
	int roiStartPosY = rtLp2.y - y1;

	// ��鳵��λ�ã���roiͼ�����Ƿ�Խ��
	int roiX1,roiX2,roiY1,roiY2;
	roiX1 = MAX(0, roiStartPosX);
	roiX2 = MIN(roiMat.cols - 1, roiStartPosX + rtLp2.width - 1);
	roiY1 = MAX(0, roiStartPosY);
	roiY2 = MIN(roiMat.rows - 1, roiStartPosY + rtLp2.height - 1);

	cv::Rect rtLpInRoi = cv::Rect(roiX1, roiY1, roiX2 - roiX1 + 1, roiY2 - roiY1 + 1);
	cv::Mat roiLp = roiMat(rtLpInRoi);
	roiLp.setTo(110);

	return roiMat;
}

/**
* brief@ Get roi image from NV12.
* param@ pNV12: src nv12 image.
* param@ width: nv12 width
* param@ height: nv12 height
* param@ rtRoi: ROI
* param@ pNV12ROI: Out roi NV12 image.
*/
void getRoiNV12(const uint8_t* pNV12, int width, int height, cv::Rect rtRoi, uint8_t* pNV12ROI)
{
	int x1, y1, x2, y2;
	int& roiWidth = rtRoi.width;
	int& roiHeight = rtRoi.height;
	x1 = rtRoi.x;
	y1 = rtRoi.y;
	x2 = rtRoi.width + x1;
	y2 = rtRoi.height + y1;

	// Copy Y
	const uint8_t* pSrc = pNV12 + y1 * width + x1;
	uint8_t* pDst = pNV12ROI;
	for (int h = y1; h < y2; h++) {
		memcpy(pDst, pSrc, roiWidth);
		pSrc += width;
		pDst += roiWidth;
	}

	// Copy UV
	pSrc = pNV12 + width * (height + (y1 >> 1)) + x2;
	pDst = pNV12ROI + roiWidth * roiHeight;
	for (int h = 0; h < (roiHeight >> 1); h++) {
		memcpy(pDst, pSrc, roiWidth);
		pSrc += width;
		pDst += roiWidth;
	}
}

/**
* brief@ Get roi image from NV12 image.
* param@ w: NV12 image width
* param@ h: NV12 image height
* param@ rtRoi: ROI, member must be even numbers.
*/
cv::Mat getRoiFromNV12(const uint8_t* pNV12, int width, int height, cv::Rect rtRoi)
{
	uint8_t* pNV12ROI = NULL;

	// Must be even numbers.
	rtRoi.x = rtRoi.x / 2 * 2;
	rtRoi.y = rtRoi.y / 2 * 2;
	rtRoi.width = rtRoi.width / 2 * 2;
	rtRoi.height = rtRoi.height / 2 * 2;
	
	cv::Mat roiMat = cv::Mat(rtRoi.height, rtRoi.width, CV_8UC3);

	pNV12ROI = new uint8_t[rtRoi.height * rtRoi.width * 3 / 2];
	if (NULL == pNV12ROI) {
		std::cout << "Can't new " << __FUNCTION__ << "_" << __LINE__ << std::endl;
		return cv::Mat();
	}

	// Copy roi NV12 to 'pNV12ROI'
	getRoiNV12((uint8_t*)pNV12, width, height, rtRoi, (uint8_t*)pNV12ROI);

	nv12_to_bgr24(pNV12ROI, rtRoi.width, rtRoi.height, roiMat.data);

	if (pNV12ROI) {
		delete[] pNV12ROI;
	}

	return roiMat;
}

/*=======================================================================
Function : ��ȡTImage��roiͼ�񣬳����߽粿�ֲ�ɫΪ110  
Param    : ptSrc[in]	ԭʼͼ��
		   ptRoi[in]	���ص�roiͼ���ڴ����ʵ�ֿ��٣��µĿ�ߣ��ڲ��ḳֵ
		   tRtRoi[in]	roiλ��
Return   : void
=========================================================================*/
void getRoiFromTImage1(const TImage *ptSrc, TImage *ptRoi, TRect tRtRoi)
{
	int l32Channel;
	char *pu8SrcBuf;
	char *pu8DstBuf;
	int l32SrcStride;
	int dstStep;
	int x1,x2,y1,y2;
	
	if(ptRoi == NULL || ptSrc == NULL)
	{
		return ;
	}

	// ���ý��ͼ��width��height���ڴ�����Ѿ�����
	ptRoi->width = tRtRoi.w;
	ptRoi->height = tRtRoi.h;
	l32Channel = ptSrc->step / ptSrc->width;
	ptRoi->step = tRtRoi.w * l32Channel;

	pu8SrcBuf = (char *)ptSrc->pu8Data;
	pu8DstBuf = (char *)ptRoi->pu8Data;
	l32SrcStride = ptSrc->step;
	dstStep = ptRoi->step;

	// ���ñ߽�ֵΪ110,ȫ����ֵ
	memset(pu8DstBuf, 110, ptRoi->step*ptRoi->height);
	memset(pu8DstBuf + ptRoi->step*ptRoi->height, 128, ptRoi->width*ptRoi->height/2);

	// ��ԭʼͼ���е���ʵλ��
	x1 = MAX(0, tRtRoi.x);
	y1 = MAX(0, tRtRoi.y);
	x2 = MIN(ptSrc->width - 1, tRtRoi.w + tRtRoi.x - 1);
	y2 = MIN(ptSrc->height - 1, tRtRoi.h + tRtRoi.y - 1);

	int rw = x2 - x1 + 1;
	int rh = y2 - y1 + 1;

	// ��Ӧ��roiͼ���еĿ�ʼλ��
	int roix1 = MAX(0, -tRtRoi.x);
	int roiy1 = MAX(0, -tRtRoi.y);

	char* psrcbuf = pu8SrcBuf + y1 * l32SrcStride + l32Channel*x1;
	char* proibuf = pu8DstBuf + roiy1 * dstStep + l32Channel*roix1;
	for (int h = 0; h < rh; h++)
	{
		memcpy(proibuf, psrcbuf, rw * l32Channel);
		psrcbuf += l32SrcStride;
		proibuf += dstStep;
	}


	if (ptSrc->imgType == NV12)
	{
		char* pu8UV = (char*)ptRoi->pu8Data + tRtRoi.w * tRtRoi.h;
		psrcbuf = pu8SrcBuf + ptSrc->step * ptSrc->height + y1 / 2 * l32SrcStride + l32Channel*x1;
		proibuf = (char*)pu8UV + roiy1 / 2 * dstStep + l32Channel*roix1;
		for (int h = 0; h < rh/2; h++)
		{
			memcpy(proibuf, psrcbuf, rw * l32Channel);
			psrcbuf += l32SrcStride;
			proibuf += dstStep;
		}
	}
	else if(ptSrc->imgType == YUV)
	{
		// u
		char* pu8U = (char*)ptRoi->pu8Data + tRtRoi.w * tRtRoi.h;
		psrcbuf = pu8SrcBuf + ptSrc->step * ptSrc->height + (y1 * l32SrcStride / 2 + l32Channel*x1)/2;
		proibuf = (char*)pu8U + (roiy1 * dstStep / 2 + l32Channel*roix1) / 2;
		for (int h = 0; h < rh/2; h++)
		{
			memcpy(proibuf, psrcbuf, rw * l32Channel / 2);
			psrcbuf += l32SrcStride/2;
			proibuf += dstStep/2;
		}

		// v
		char* pu8V = (char*)ptRoi->pu8Data + tRtRoi.w * tRtRoi.h * 5 / 4;
		psrcbuf = pu8SrcBuf + ptSrc->step * ptSrc->height * 5 / 4 + (y1 * l32SrcStride / 2 + l32Channel*x1)/2;
		proibuf = (char*)pu8V + (roiy1 * dstStep / 2 + l32Channel*roix1) / 2;
		for (int h = 0; h < rh/2; h++)
		{
			memcpy(proibuf, psrcbuf, rw * l32Channel / 2);
			psrcbuf += l32SrcStride/2;
			proibuf += dstStep/2;
		}
	}
	else
	{
		PrintfLogArg("only support yuv420 or nv12\n");
	}
}

/*=======================================================================
Function : ���ݳ���λ�ã�������ߺ��roi��������䵽ͼ���⣬��ֵ110��
		   ͬʱ�����ı�����lpex���غ󣬳���λ��Ҳ���ó�110   
Param    : ptSrc[in]	ԭʼͼ��
		   ptRoi[in]	���ص�roiͼ���ڴ����ʵ�ֿ��٣��µĿ�ߣ��ڲ��ḳֵ
		   f32L,f32R,f32T,f32B[in] ���ݳ���λ�ã����������¸����䳵�ƿ�ȵı���
		   lpex[in]		������������������
Return   : void
=========================================================================*/
void getRoiTImageByLpPos(const TImage *ptSrc, TImage *ptRoi, const TRect tRtLp, 
						 float f32L,float f32R,float f32T,float f32B, int l32LpEx)
{
	// �³���λ�ã�����Ĩ��roi�еĳ���
	TRect rtLp2 = tRtLp;
	rtLp2.x -= l32LpEx;
	rtLp2.y -= l32LpEx;
	rtLp2.w += 2*l32LpEx;
	rtLp2.h += 2*l32LpEx;

	int cx = tRtLp.x + tRtLp.w/2;
	int cy = tRtLp.y + tRtLp.h/2;

	int x1 = (int)(cx - f32L * tRtLp.w);
	int x2 = (int)(cx + f32R * tRtLp.w);
	int y1 = (int)(cy - f32T * tRtLp.w);
	int y2 = (int)(cy + f32B * tRtLp.w);

	TRect tRtRoi;
	tRtRoi.x=x1;
	tRtRoi.y=y1;
	tRtRoi.w = x2 - x1 + 1;
	tRtRoi.h = y2 - y1 + 1;

	getRoiFromTImage1(ptSrc, ptRoi, tRtRoi);

	// ����������Ϊ110���и����裬������ԭʼͼ��һ������Խ��ġ�
	// ��Ӧ��roiͼ���еĿ�ʼλ��
	int roiStartPosX = rtLp2.x - x1;
	int roiStartPosY = rtLp2.y - y1;

	int l32Channel = ptSrc->step / ptSrc->width;

	// ��鳵��λ�ã���roiͼ�����Ƿ�Խ��
	int roiX1,roiX2,roiY1,roiY2;
	roiX1 = MAX(0, roiStartPosX);
	roiX2 = MIN(ptRoi->width - 1, roiStartPosX + rtLp2.w - 1);
	roiY1 = MAX(0, roiStartPosY);
	roiY2 = MIN(ptRoi->height - 1, roiStartPosY + rtLp2.h - 1);

	char* pu8LpBuf = (char*)ptRoi->pu8Data + roiY1 * ptRoi->step + roiX1 * l32Channel;
	int l32CopySize = MAX(0, (roiX2 - roiX1)*l32Channel);
	for (int h = 0; h < (roiY2 - roiY1); h++)
	{
		memset(pu8LpBuf, 110, l32CopySize);
		pu8LpBuf += ptRoi->step;
	}
}

#endif