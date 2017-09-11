/************************************************************************/
/* 2017-03-19 SandyYann.
	ͨ�������ڴ洫�ݽ������ݣ�ע���������£�
	1.�����ڴ�Ĵ�С�̶���������Ƶ���������Ҫ����ͼ���ٴ��ݣ�	*/
/************************************************************************/

#ifndef _DEC_SERVER_SHARE_MEM_H_
#define _DEC_SERVER_SHARE_MEM_H_

#include <Windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define TIP_ERR_DSSM() printf("ERR:%s, %d\n", __FUNCTION__, __LINE__);

// �̶���С�����ܸı�
#define SHARE_MEM_SIZE (1920*2560*3/2)  

typedef enum emShareMemState
{
	emSMS_NULL = 0,		// initialize null state
	emSMS_PUTED = 1,	// had putted, waited for be getted;
	emSMS_GETED = 2,	// had got,
}EMShareMemState;

typedef enum emDecResState
{
	emDRS_Fail = 0,		// fail, need to exit.
	emDMS_Success = 1,	// Decode finish, and then exit.
	emDMS_Decing = 2,	// Decoding.
}EMDecResState;

typedef struct tagShareMemHeader
{
	EMShareMemState emState;	// �����ڴ�״̬
	int width;
	int height;
	int imgSize;		// ͼ��size
	__int64 pts;		// ʱ���
	__int64 frameId;	// ֡ID
	EMDecResState emDRState;	// �Ƿ�������
}TShareMemHeader;

class CShareMemDecServer
{
public:
	// ��ȡ
	CShareMemDecServer(const char* pShareMemName)
	{
		memcpy(m_aShareMemName, pShareMemName, strlen(pShareMemName));

		createShareMem(SHARE_MEM_SIZE, m_aShareMemName);

		// ������
		m_stream1Mutex = CreateMutex(NULL, false, (LPCWSTR)"streamMutex");

		m_pBufCacheImg = new char[SHARE_MEM_SIZE];
		if (NULL == m_pBufCacheImg)
		{
			TIP_ERR_DSSM();
			return;
		}
	}

	~CShareMemDecServer()
	{
		releaseShareMem();

		if (m_pBufCacheImg) delete[] m_pBufCacheImg;
	}

	// ����һ֡ͼ��,�޷�����ʱ����������
	bool putOneFrame(char* pYuv, int w, int h, __int64 frameId, __int64 pts)
	{
		m_getCount = 0;

		// ���ͼ�����ͼ����Ҫ���ţ�
		checkShareMemSize(w, h);

	AGAIN:
		WaitForSingleObject(m_stream1Mutex, INFINITE);
		// ��ȡ�����ڴ��ͷ��Ϣ
		TShareMemHeader tSMH = getSMH(m_pShareMem);

		if (tSMH.emState == emSMS_GETED || tSMH.emState == emSMS_NULL)
		{
			// could put image data
			tSMH.emState = emSMS_PUTED;
			tSMH.height = h;
			tSMH.width = w;
			tSMH.imgSize = min(w * h * 3 / 2, SHARE_MEM_SIZE);
			tSMH.frameId = frameId;
			tSMH.pts = pts;
			tSMH.emDRState = emDMS_Decing;

			memcpy(m_pShareMem + getShareMemHeaderSize(), pYuv, tSMH.imgSize);

			// set state to got, and then DecServer could put new image data.
			setSMH(m_pShareMem, tSMH);
			ReleaseMutex(m_stream1Mutex);
			return true;
		}
		else
		{
			ReleaseMutex(m_stream1Mutex);

			// If we continue to fail to put it, it will block.
			goto AGAIN;
		}

		return true;
	}

	bool putLastResult(EMDecResState emState)
	{

	AGAIN:
		WaitForSingleObject(m_stream1Mutex, INFINITE);
		// ��ȡ�����ڴ��ͷ��Ϣ
		TShareMemHeader tSMH = getSMH(m_pShareMem);

		if (tSMH.emState == emSMS_GETED || tSMH.emState == emSMS_NULL)
		{
			tSMH.emState = emSMS_PUTED;
			tSMH.emDRState = emState;

			setSMH(m_pShareMem, tSMH);
			ReleaseMutex(m_stream1Mutex);
			return true;
		}
		else
		{
			ReleaseMutex(m_stream1Mutex);

			// If we continue to fail to put it, it will block.
			goto AGAIN;
		}

		return true;
	}

	// ��ȡһ֡ͼ��, ���ؽ���״̬����ʱ���ȡ������������Ҳ�᷵��ʧ��
	EMDecResState getOneFrame(char** ppYuv, int& w, int& h, __int64& frameId, __int64& pts)
	{
		m_getCount = 0;

	AGAIN:
		WaitForSingleObject(m_stream1Mutex, INFINITE);
		// ��ȡ�����ڴ��ͷ��Ϣ
		TShareMemHeader tSMH = getSMH(m_pShareMem);

		if (tSMH.emState == emSMS_PUTED)
		{
			// could get image data
			memcpy(m_pBufCacheImg, m_pShareMem + getShareMemHeaderSize(), tSMH.width * tSMH.height * 3 / 2);
			*ppYuv = m_pBufCacheImg;
			w = tSMH.width;
			h = tSMH.height;
			frameId = tSMH.frameId;
			pts = tSMH.pts;

			// set state to got, and then DecServer could put new image data.
			tSMH.emState = emSMS_GETED;
			setSMH(m_pShareMem, tSMH);
			ReleaseMutex(m_stream1Mutex);

			return tSMH.emDRState;
		}
		else
		{
			ReleaseMutex(m_stream1Mutex);
			m_getCount++;
			// If we continue to fail to get it, it returns false;
			if (m_getCount > 999999999)
			{
				// ��ʱ�䲻�ܻ�ȡ���������򷵻�ʧ�ܣ��˳�
				return emDRS_Fail;
			}
			goto AGAIN;
		}
	}
private:
	inline void createShareMem(int shareMemSize, const char* pShareMemName)
	{
		// ���������ļ����  
		m_hMapFile = CreateFileMappingA(
			INVALID_HANDLE_VALUE,    // �����ļ����  
			NULL,                    // Ĭ�ϰ�ȫ����  
			PAGE_READWRITE,          // �ɶ���д  
			0,                       // ��λ�ļ���С  
			SHARE_MEM_SIZE + sizeof(TShareMemHeader),          // ��λ�ļ���С  
			pShareMemName                   // �����ڴ�����  
			);

		m_pShareMem = (char *)MapViewOfFile(
			m_hMapFile,            // �����ڴ�ľ��  
			FILE_MAP_ALL_ACCESS, // �ɶ�д���  
			0,
			0,
			SHARE_MEM_SIZE + sizeof(TShareMemHeader)
			);
	}
	inline void releaseShareMem()
	{
		if (m_pShareMem)
		{
			UnmapViewOfFile(m_pShareMem);
			m_pShareMem = NULL;
		}
		if (m_hMapFile)
		{
			CloseHandle(m_hMapFile);
			m_hMapFile = NULL;
		}
		if (m_stream1Mutex)
		{
			CloseHandle(m_stream1Mutex);
			m_stream1Mutex = NULL;
		}
	}

	inline int getShareMemHeaderSize()
	{
		return sizeof(TShareMemHeader);
	}

	inline TShareMemHeader getSMH(char* pbuf)
	{
		TShareMemHeader tSMH;
		memcpy(&tSMH, pbuf, sizeof(TShareMemHeader));
		return tSMH;
	}
	inline void setSMH(char* pbuf, TShareMemHeader tSMH)
	{
		memcpy(pbuf, &tSMH, sizeof(TShareMemHeader));
	}

	// putOneFrame�����ڴ�ˣ���Ҫ�������������
	// getOneFrameʱ������Ҫ�����������
	bool checkShareMemSize(int imgWidth, int imgHeight)
	{
		if (imgWidth * imgHeight * 3 / 2 > SHARE_MEM_SIZE)
		{
			// ��Ҫ����ͼ��...

		}
		return TRUE;
	}
private:
	char m_aShareMemName[256];
	HANDLE m_hMapFile;
	char *m_pShareMem;
	HANDLE m_stream1Mutex;

	int m_getCount;

	char* m_pBufCacheImg;
};


#endif