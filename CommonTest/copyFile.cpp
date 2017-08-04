/************************************************************************
/* �ļ���copy��ز���
	Sandy Yann
	2016-1-4															*/
/************************************************************************/
#include "CommonTest.h"


#include <Windows.h>
// �����ļ��������·�������ڣ��򴴽�·��
BOOL createPath(std::string strFn)
{
	std::string strCurPath = strFn.substr(0, strFn.find('\\'));

	WIN32_FIND_DATAA fd;
	HANDLE hFind = ::FindFirstFileA( strCurPath.c_str(), &fd );
	while( hFind != INVALID_HANDLE_VALUE )
	{
		if ( fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )
			return TRUE;
	}

	if ( !::CreateDirectoryA( strCurPath.c_str(), NULL ) )
	{
		return FALSE;
	}

	return TRUE;
}

/*=======================================================================
Function : �ļ�copy��Դ�ļ�����Ŀ���ļ���,ʵ�ⷢ��������дҪ�ȵ���dos����Ҫ��
Param    : 
Return   : void
=========================================================================*/
void copyFile2DstFn(std::string strSrcFn, std::string strDstFn)
{
	FILE* pfSrc = fopen(strSrcFn.c_str(), "rb");

	// ����ļ����Ƿ���ڣ���������ڣ��򴴽�
	createPath(strDstFn);
	FILE* pfDst = fopen(strDstFn.c_str(), "wb");

	if (pfDst && pfSrc)
	{
		char curChr;
		for(;;)
		{
			int r = fread(&curChr, 1,1, pfSrc);
			if (r)
			{
				fwrite(&curChr, 1,1, pfDst);
			}
			else
			{
				break;
			}
		}
	}

	if(pfSrc) fclose(pfSrc);
	if(pfDst) fclose(pfDst);
}