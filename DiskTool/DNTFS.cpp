/***********************************************************************
 * FileName:	DNtfs.cpp
 * Author:		����
 * Created:		2012��4��20�� ������
 * Purpose:		����DNtfs�ľ���ʵ��
 * Comment:		������Ƕ�һ��Ntfs��ĳ����ṩ��Ntfs�����ĸ��ַ���
 *
 * Modify:		�ڳ���������ڼ䷢����ЩĿ¼��IndexBlock�е��ļ���������
 *				��DOS����Ϊ��,Ҳ����˵�ڲ��ҵĹ������������Ҫ���ҵ��ļ�
 *				���ԵĻ������ܻ������Ҳ����˵�Ҳ���������ҽ����еĴ�
 *				���Բ�����������Ϊ�˲������Եĺ�������������ļ����ڶ���
 *				���������.2012-05-21 16:34
 *
 *				��֮ǰrun�Ĳ�����ֱ��д��NTFS�ļ����У������Ļ�ʹ������
 *				������,ֻ�ڷ��ʵ�ʱ��ͨ���ж��Ƿ����ļ���������ʼ������
 *				DATA�����е�RUN����INDEX_ALLOCATION�е�RUN��Ҳ����˵֮��
 *				�������������RUN����ʵ�ʵ�RUN�ǿ��Ա��ܶ�����ʹ�õģ���
 *				�����ڽ�RUN���������ˣ����䶨��ΪDRun��2012-07-01 11:21��
 *
 *				�ڻ�ȡMFT��¼�ĵ�һ������ʱ����ǰ��ֱ��ͨ����¼�Ų���DBR
 *				�е�ָ����MFT�����Ǿ�˵��Щ��������MFT������һ��������
 *				�����������޸��˲���MFT���㷨����Ϊ�ļ��������ʵҲ��һ��
 *				�ļ�,���Բ��ҵĻ�����ͨ����������DATA���Ե���RUN���ʵ��
 *				MFT��¼���ڵľ���λ��,����Ҳ�ͱ�����MFTһ��Ϊ�������.��
 *				Ȼ�����������û�м��������������Ҳ��˵�ļ������ʾһ��
 *				�����Ŀռ�,��Ԥ���ġ�2012-07-01 13:54��
 *
 *				�Ż�MFT�����㷨����֮ǰÿһ��λMFT��¼�Ƕ�Ҫ��ȡ����$MFT
 *				�ļ�������DATA���ԣ������Ļ�Ч�ʻ����һ�㣬�������ڴ�
 *				�豸�Ǿͽ��Ǹ�Runת����MFT_BLOCK������ȥ�ˣ������Ժ�ÿһ
 *				�β���ʱ���㣬�����Ļ�ֻ���ȡһ��$MFT��������Run�ˣ��Ҽ�
 *				����������һЩ��2012-07-18 1:07.
 *
 ***********************************************************************/


#include "disktool.h"
#include "stut_connf.h"
#include "windows.h"

//ȡ����˼��windows���ڱ�ע��ľ��濪��
#pragma warning(disable:4996)

//////////////////////////////////////////////////////////////////////
//���ڲ����ļ����

//////////////////////////////////////////////////////////////////////
// typedef struct _FileFindHander{
// 	LONG_INT vcn;		//����غ�  -1 ��ʾindexRoot�е�
// 	int index;			//���ƶ�vcn�������
// }FIND_FILE_HANDER , *PFIND_FILE_HANDER;


//��������  1  B+������   2  Index_Allocation ����
#define  SEARCH_TYPE  2 


DNtfs::DNtfs()
	: mPMftBlock(NULL)
	, mDev(INVALID_HANDLE_VALUE)
	, mMftBlockCnt(0)
{
	mFSOff.QuadPart			= 0;
	mCluForMFTMirr.QuadPart	= 0;
	mCluForMFT.QuadPart		= 0;
}

DNtfs::~DNtfs()
{
}

DRES DNtfs::OpenDev(const WCHAR* devName, const PLONG_INT off)
{
	if (!devName || !off)
		return DR_INVALED_PARAM;

	if (!mDevName.empty())
		return DR_ALREADY_OPENDED;

	DRES		res = DR_OK;
	size_t		len = 0;
	char		buf[SECTOR_SIZE] = {0};
	PNTFS_DBR	pDBR = (PNTFS_DBR)buf;
	LONG_INT	temp = {0};

	mDevName = devName;
	//�豸��ƫ��
	mFSOff = *off;

	//���豸
	mDev = ::CreateFile(mDevName.c_str(),
						GENERIC_READ | GENERIC_WRITE,
						FILE_SHARE_READ | FILE_SHARE_WRITE,
						NULL,
						OPEN_EXISTING,
						0,
						NULL);
	if (mDev == INVALID_HANDLE_VALUE)
	{//���豸ʧ��
		mDevName.clear();
		mFSOff.QuadPart = 0;
		return DR_OPEN_DEV_ERR;
	}

	//��ȡdbr������
	temp.QuadPart = 0;
	mAllSec.QuadPart = 1;
	res = ReadData(buf , &temp , SECTOR_SIZE );	//��ȡ�����ĵ�һ������������
	mAllSec.QuadPart = 0;
	if(DR_OK != res)	return res;				//��ȡ����ʧ��?
	if(pDBR->dbrEnd != MBR_END)//������Ч
		return DR_INIT_ERR;

	//��ȡMFT����ʼ�߼��غ�
	mSecPerClu		= pDBR->bpbSecPerClu;	//ÿ�ص�ɽ����
//	mResSec			= pDBR->bpbResSec;		//����������
	mAllSec			= pDBR->bpbAllSec;		//�ܵ�������
	mCluForMFT		= pDBR->bpbCluForMFT;	//MFT
	mCluForMFTMirr	= pDBR->bpbCluForMFTMirr;//MFTMirr
	
	//��ʼ��MFT��ṹ
	res = InitMFTBlock();

	if (res != DR_OK)
	{//����ʧ��
		mDevName.clear();
		mFSOff.QuadPart = 0;
		CloseHandle(mDev);
		mDev = INVALID_HANDLE_VALUE;
		if (NULL != mPMftBlock)
		{
			free(mPMftBlock);
			mPMftBlock = NULL;
		}
	}
	return res;
}

DRES DNtfs::InitMFTBlock()
{
	DNtfsFile	mftFile;//��ǰ���MFT�ļ�
	DRES		res = DR_OK;
	DNtfsFile::PAttrItem pAttrItem = NULL;
	DNtfsAttr	ntfsAttr;
	DRun		run;
	DWORD		i;
	LONG_INT	liStartMft;	//��ʼMFT��¼��

	//��$MFT�ļ�
	res = OpenFileW(&mftFile , SYS_FILE_MFT);
	if (res != DR_OK) return res;  //�ļ���ʧ��

	//��������������� 
	if(DR_OK != mftFile.FindNoNameDataAttr(&ntfsAttr))
	{//��ȡ������������ʧ��
		return DR_INNER_ERR;
	}

	//�����������
	if(DR_OK != run.InitRunList(&ntfsAttr))
		return DR_INNER_ERR;

	mMftBlockCnt = DWORD(run.mRunList.size());  //������
	mPMftBlock = (PMFT_BLOCK)malloc(sizeof(MFT_BLOCK) * mMftBlockCnt);  //�ռ����
	memset(mPMftBlock , 0 , sizeof(MFT_BLOCK) * mMftBlockCnt);

	for( i = 0 , liStartMft.QuadPart = 0 ;
		i < mMftBlockCnt ; ++i )
	{
		//��ʼ��¼��
		mPMftBlock[i].liStartMft = liStartMft;

		//��ʼ������
		mPMftBlock[i].liStartSector.QuadPart 
			= run.mRunList[i].lcn.QuadPart * mSecPerClu;
		
		//��¼��
		mPMftBlock[i].liMftCnt.QuadPart
			= (run.mRunList[i].clustCnt.QuadPart * mSecPerClu) / SECTOR_PER_RECODE;

		//дһ��MFT��¼�����ʼ��¼��
		liStartMft.QuadPart += mPMftBlock[i].liMftCnt.QuadPart;
	}

	return DR_OK;
}

BOOL DNtfs::IsDevOpened()
{
	return !mDevName.empty();
}

void DNtfs::CloseDev()
{
	//�ͷ����ֿռ�
	if (!mDevName.empty())
	{
		mDevName.clear();
		mFSOff.QuadPart = 0;
		CloseHandle(mDev);
		mDev = INVALID_HANDLE_VALUE;
	}

	if (NULL != mPMftBlock)
	{
		free(mPMftBlock);
		mMftBlockCnt = 0;
		mPMftBlock = NULL;
	}
}

DRES DNtfs::ReadData(void* buf , PLONG_INT off , DWORD dwReadCnt , BOOL isOffSec)
{
	DRES	 res = DR_OK;
	DWORD	 dwReaded = 0;
	LONG_INT offset;

	//�豸������û�д�
	if (mDevName.empty())
		return DR_NO_OPEN;

	if (isOffSec) //����ƫ��
	{
		if (off->QuadPart >= mAllSec.QuadPart)
			return DR_DEV_CTRL_ERR; //Խ����

		offset.QuadPart = mFSOff.QuadPart + off->QuadPart;	//��ȡ���ݵ�ʵ��ƫ��
		offset.QuadPart *= SECTOR_SIZE;		//�ֽ�ƫ��
	}
	else //�ֽ�ƫ��
	{
		if (off->QuadPart / SECTOR_SIZE >= mAllSec.QuadPart)
			return DR_DEV_CTRL_ERR; //Խ����

		offset.QuadPart = mFSOff.QuadPart * SECTOR_SIZE;
		offset.QuadPart += off->QuadPart;
	}

	//�����ļ�ָ��
	offset.LowPart = SetFilePointer(mDev , offset.LowPart , PLONG(&(offset.HighPart)) ,FILE_BEGIN );
	if (offset.LowPart == -1 && GetLastError() != NO_ERROR )
		res = DR_DEV_CTRL_ERR;

	//��ȡ����
	if(!res && !::ReadFile(mDev , buf , dwReadCnt ,&dwReaded ,NULL) && dwReaded != dwReadCnt)	
		res =  DR_DEV_IO_ERR;

/*	CloseHandle(hDev);								//�ر��Ѿ��򿪵��豸*/
	return res;
}

DRES DNtfs::ReadMFT(void* buf , PLONG_INT index)
{
	//�ȼ���һ��ƫ�ư�
 	LONG_INT off;

	if (index->QuadPart != 0)
	{//ֻ�е�һ���ļ���Ҫ���ң�������Ҫ����
		off =GetSectorOfMFTRecode(*index);
	}else
	{
		off = mCluForMFT;
		off.QuadPart *= mSecPerClu;	//MFT��ʵ������
/*		off.QuadPart += index->QuadPart * RECODE_PER_SEC;*/
	}
	
	//��ȡָ��������
	return ReadData(buf , &off , MFT_RECODE_SIZE , TRUE);
}

int DNtfs::FileNameCmp(const WCHAR * src1,const WCHAR * src2 , int len , BOOL caseSensitiv)
{
	int i = 0;

	if (caseSensitiv)
	{
		for (i = 0 ;  i < len ; ++i)
			if (src1[i] == src2[i]) continue;  //��ǰ�ַ����
			else return  src1[i] - src2[i];       //��ǰ�ַ������
	}else{
		WCHAR w1 ,w2;

		for (i = 0 ;  i < len ; ++i){
			w1 = WchrToUpper(src1[i]);
			w2 = WchrToUpper(src2[i]);
			if (w1 == w2) continue;  //��ǰ�ַ����
			else return  w1 - w2;       //��ǰ�ַ������
		}
	}

	return 0;
}

DRES DNtfs::OpenFileW(DNtfsFile *file , LONG_INT idx)
{
	if (!file)
		return DR_INVALED_PARAM;

	//�豸������û�д�
	if (mDevName.empty())
		return DR_NO_OPEN;

	return file->InitRecode(this , idx);
}

DRES DNtfs::OpenFileW(DNtfsFile *file , int idx)
{
	LONG_INT i;
	i.QuadPart = idx;
	return OpenFileW( file , i);
}
/*

DRES DNtfs::OpenFileA(const char* path , DNtfsFile *file/ * , DWORD attr* / / *= ATTR_NTFS_MASK* /)
{
	WCHAR wPath[MAX_PATH + 1] = {0};
	if (path == NULL)return DR_INVALED_PARAM;

	//����ת��
	MultyByteToUnic(path , wPath , MAX_PATH + 1);

	//����Unicode�ӿ�
	return OpenFileW(wPath , file / *, attr* /);
}
*/

DRES DNtfs::OpenFileW(const WCHAR* path , DNtfsFile *file /*,DWORD attr*/)
{
	size_t		nameLen		= 0;	//�ļ������ܳ���
	DWORD		nameSegLen	= 0;	//�ļ�����һ���ֵĳ���
	const WCHAR*  name		= path; //��ǰ�ļ���
	DWORD		i			= 0;
/*	DWORD		attrT		= attr;	//��ʱ������ֵ*/
	LONG_INT	mftIdx		= {0};
	DRES		res			= DR_OK;//�����������

	LONG_INT	liFDTOff	= {0};
	WORD		fdtLen		= 0;

	//�ļ�ϵͳ�Ƿ��Ѿ�����?
	if (mDevName.empty())
		return DR_NO_OPEN;

	//����
	if (!path || ! file /*|| !attr*/)
		return DR_INVALED_PARAM;
	
	nameLen = wcslen(path);//�ļ������ܳ���
	name    = path; //��ǰ�ļ���
/*	attrT   = attr;			//��ʱ������ֵ*/

// 	//��Ŀ¼�Ļ�
// 	if (attr & ATTR_DIRECTORY)
// 		attrT |= ATTR_DIRECTORY_INDEX;
	
	//��Ŀ¼
	mftIdx.QuadPart = SYS_FILE_ROOT;

	while(name[0]){
		//�����ļ�·����ÿһ��
		for(i = 0 ; name[0] ;++i ){
			if(IsPathSeparator(name[i]))  ++name;//ȥ���ļ���ǰ��ķָ�����
			else  break;		
		}
		//�������
		if (!name[0] )  break;

		//���㵱ǰ·���εĳ���
		for (nameSegLen = 0 ; 
			!IsPathSeparator(name[nameSegLen]) && name[nameSegLen] ;
			++nameSegLen);

/*		if (name[nameSegLen]) attrT = attr | ATTR_DIRECTORY |ATTR_DIRECTORY_INDEX;//���ʼ���*/

		//��Ŀ¼�����ļ�
		//�������е�vcn��Ӧ��block
		//res = FindItemByName(mftIdx , name ,nameSegLen , &mftIdx ,  attrT);
		//����b+��
		res = FindItemByName2(mftIdx , name ,nameSegLen , &mftIdx  , &liFDTOff , &fdtLen);
		//������
		if (res != DR_OK) return res;
		name += nameSegLen;
	}
	res = OpenFileW(file ,mftIdx);
	if (DR_OK == res)
	{
		file->mLIStartFDT = liFDTOff;
		file->mFDTLen = fdtLen;
	}
	
	return res ;
}

DRES DNtfs::FindItemByName(LONG_INT dir , const  WCHAR* name, int len , PLONG_INT mftIdx , DWORD attr)
{
 	DRES			res			= DR_OK;
	LONG_INT		parentMft	= {0};	//�����ҵ�Ŀ¼��mft��¼��
 	DWORD			blockSize	= 0;
 	BYTE*			blockEnd	= 0;
	INDEX_ENTRY*	indexEntry	= NULL;
	LONG_INT		vcn			= {0};	//����غ�
	LONG_INT		lcn			= {0};	//�߼��غ�
	std::vector<BYTE> indexBlockBuf;//��������������ʵ��ַ
 	PFILE_NAME		fn			= NULL; //�ļ�����ָ��
	DNtfsFile		root;
 	DNtfsAttr		attrRoot;
 	DNtfsAttr		attrAllocation;
	DNtfsAttr		attrBitMp;		//Ϊ������


	//����
	if (!name || len <= 0 || len > 255 || !mftIdx) return DR_INVALED_PARAM; 

	if (dir.QuadPart == -1)  //��ָ����Ŀ¼�в���
		parentMft.QuadPart = SYS_FILE_ROOT;
	else
		parentMft = dir;


	//���ָ���ĸ��ļ���¼
	res = OpenFileW(&root , parentMft);
	
	//��ø�����
	res = root.FindAttribute(AD_INDEX_ROOT , &attrRoot);
	//������
	if (res != DR_OK)	return DR_INIT_ERR;
	//������Ĵ�С
	blockSize = attrRoot.IRGetIndexBlockSize();
	//������ĵ�һ����ڵĵ�ַ
	indexEntry = PINDEX_ENTRY(attrRoot.IRGetFistEntry());
	//������Ľ���λ��
	blockEnd = (BYTE*)indexEntry + attrRoot.IRGetAlloIndexEntriesSize();

	//��������������
	for (;(BYTE*)indexEntry != blockEnd 
		;indexEntry = PINDEX_ENTRY((BYTE*)indexEntry + indexEntry->IE_Size))
	{
		if ((BYTE*)indexEntry + 0x10 > blockEnd)//������ 0x10��INDEX_RNTRYͷ���Ĵ�С
		{
			root.Close();
			return  DR_INIT_ERR;
		}
		//����������ڵ�Entry���Ậ������.Ҳ�����к��ӽڵ��ָ��
		if (indexEntry->IE_Flags & INDEX_ENTRY_END)  //���˽�β��
			break;  //������ʼ���vcn

		fn = PFILE_NAME(indexEntry->IE_Stream);  //��ȡ�ļ�������
		if(fn->FN_NameSize != len || !(fn->FN_DOSAttr & attr)) 		continue;			//�ļ����ĳ��Ȳ�ƥ��
		if (!FileNameCmp(fn->FN_FileName ,  name , len , FALSE ))
		{//�ҵ���
			mftIdx->QuadPart = (indexEntry->IE_FR.QuadPart << 16)>>16; //ȥ���������ֽڵ�mft���к�
			root.Close();
			return DR_OK;   //�ҵ��˾�ֱ�ӷ���
		}else
			continue;
	}

	//�����Ӧ������
	res = root.FindAttribute(AD_BITMAP , &attrBitMp);
	if (res == DR_NO) return DR_NO_FILE;  //û��λͼ����   	
	res = root.FindAttribute(AD_INDEX_ALLOCATION , &attrAllocation);
	if (res == DR_NO) return DR_NO_FILE;  //û��vcn->lcnӳ�������
	
	root.Close();		//����Ҫ����


	//������һ�������������Ļ���
	indexBlockBuf.resize(blockSize, 0);

	int bitCnt = attrBitMp.R_GetAttrLen() * 8;  //bitmap�е�bit��
	vcn.QuadPart = 0;//��ʵvcn

	//��������B+��
	while (1)
	{
		//�����һ��vcn
		for( ; vcn.QuadPart < bitCnt ; ++vcn.QuadPart) {
			if (attrBitMp.BMIsBitSet(vcn , this))
				break;
		}
		if (vcn.QuadPart == bitCnt){
			res = DR_NO_FILE;//û���ҵ��ļ�		
			break;
		}

		//����߼��غ�
		lcn = root.GetLCNByVCN(vcn , NULL);
		//lcn = attrAllocation.IAGetLCNByVCN(&vcn , NULL);
		lcn.QuadPart *= mSecPerClu; 
		ReadData(indexBlockBuf.data(), &lcn, blockSize, TRUE);//��ȡָ��������

		//�߻ָ�usa
		PINDEX_BLOCK_HEAD ibh = PINDEX_BLOCK_HEAD(indexBlockBuf.data());

		//���usa������  
		int		usaCnt = ibh->IB_USNSz ;
		WORD	us	   = 0;
		DWORD	offUsa = ibh->IB_USOff;
		int		i	   = 0;
		DWORD	usn = GetWORD(indexBlockBuf.data() + offUsa);

		for (i = 1 ; i < usaCnt ; ++i){
			us = GetWORD(indexBlockBuf.data() + offUsa + i * 2);
			SetWORD(indexBlockBuf.data() + 512 * i - 2, us);
		}

		//��Ӧ����غ�
		if (vcn.QuadPart != ibh->IB_VCN.QuadPart)
		{//����غŲ�ƥ��
			res = DR_INIT_ERR;
			break;
		}

		//������ĵ�һ����ڵĵ�ַ
		indexEntry = PINDEX_ENTRY((BYTE*)&ibh->IB_indexHead + ibh->IB_indexHead.IH_EntryOff);
		//������Ľ���λ��
		blockEnd = (BYTE*)&ibh->IB_indexHead + ibh->IB_indexHead.IH_AllocSize;

		//��������������
		for (;(BYTE*)indexEntry != blockEnd ; 
			indexEntry = PINDEX_ENTRY((BYTE*)indexEntry + indexEntry->IE_Size))
		{
			if ((BYTE*)indexEntry + 0x10 > blockEnd)
			{//������ 0x10��INDEX_RNTRYͷ���Ĵ�С
				res =  DR_INIT_ERR;
				break;
			}

			//����������ڵ�Entry���Ậ������.Ҳ�����к��ӽڵ��ָ��
			if (indexEntry->IE_Flags & INDEX_ENTRY_END)  //���˽�β��
				break;  //������ʼ���vcn

			fn = PFILE_NAME(indexEntry->IE_Stream);
			//if(fn->FN_NameSize != len || !(fn->FN_DOSAttr & attr)) 		continue;	
			if (!FileNameCmp(fn->FN_FileName ,  name , len , FALSE ))
			{//�ҵ���
				if(!(fn->FN_DOSAttr & attr)) 		continue;	//���Բ���
				mftIdx->QuadPart = (indexEntry->IE_FR.QuadPart << 16)>>16; //ȥ���������ֽڵ�mft���к�
				return DR_OK;   //�ҵ��˾�ֱ�ӷ���
			}else
				continue;
		}

		//��һ���غ�
		++vcn.QuadPart;
	}

	return res;
}

BYTE DNtfs::GetSecPerClust()
{
	return mSecPerClu;
}

LONG_INT DNtfs::GetSecCount()
{
	return mAllSec;
}

DRES DNtfs::GetVolumeName(WCHAR * nameBuf , DWORD bufLen)
{
	//����
	if (!nameBuf)
		return DR_INVALED_PARAM;

	//�ļ�ϵͳ�Ƿ��Ѿ�����?
	if (mDevName.empty())
		return DR_NO_OPEN;

	DNtfsFile file;
	//�򿪾��ļ�
	DRES res = OpenFileW(&file, SYS_FILE_VOLUME);
	if (res != DR_OK)
		return res;

	DNtfsAttr attr;
	//�����ļ�������
	res = file.FindAttribute(AD_VOLUME_NAME , &attr);
	if (res != DR_OK)
	{
		file.Close();
		return DR_NO_FILE_NAME;
	}

	//������ֵ��ַ���
	DWORD len = attr.R_GetAttrLen();
	if (bufLen <= len) //���治��
	{
		file.Close();
		return DR_BUF_OVER;
	}

	//�����ļ���
	memcpy(nameBuf , attr.R_GetAttrBodyPtr() , len);
	nameBuf[len/2] = 0;

	//��β��ʱ��ǵ�Ҫ�ر��Ѿ��򿪵��ļ�
	file.Close();
	return res;
}

DRES DNtfs::FindItemByName2(LONG_INT dir , const  WCHAR* name, int len , PLONG_INT mftIdx , PLONG_INT pLIStartFDT , WORD* fdtLen/* , DWORD attr*/)
{
	//����
	if (!name || len <= 0 || len > 255 || !mftIdx)
		return DR_INVALED_PARAM;

	DRES		res = DR_OK;
	LONG_INT	parentMft;	//�����ҵ�Ŀ¼��mft��¼��
	DNtfsFile	root;
	DNtfsAttr	attrRoot;
	LONG_INT	vcn;
	WCHAR		nameBuf[255] = {0};
	BYTE*		blockEnd = 0;
	INDEX_ENTRY* indexEntry;
	PFILE_NAME	fn = NULL;			//�ļ�����ָ��
	DNtfsFile::PAttrItem pAttrItem;	//�ļ�����

	if (dir.QuadPart == -1)  //��ָ����Ŀ¼�в���
		parentMft.QuadPart = SYS_FILE_ROOT;
	else
		parentMft = dir;

	//��ñ����ҵ�Ŀ¼
	res = OpenFileW(&root , parentMft);
	if (res != DR_OK) return res;  //�����ļ�ʧ����
	if (!root.IsDir()){//�ⲻ��һ��Ŀ¼
		root.Close();
		return DR_IS_FILE;
	}
	//��ø�����
	pAttrItem = root.FindAttribute(AD_INDEX_ROOT);
	//res = root.FindAttribute(AD_INDEX_ROOT , &attrRoot);
	if (NULL == pAttrItem){//��������ʧ��
		root.Close();
		return DR_INIT_ERR;
	}
	attrRoot.InitAttr(pAttrItem->attrDataBuf.data());
	

	//������ĵ�һ����ڵĵ�ַ
	indexEntry = PINDEX_ENTRY(attrRoot.IRGetFistEntry());
	//������Ľ���λ��
	blockEnd = (BYTE*)indexEntry + attrRoot.IRGetAlloIndexEntriesSize();

	//��������������
	for (;(BYTE*)indexEntry < blockEnd 
		;indexEntry = PINDEX_ENTRY((BYTE*)indexEntry + indexEntry->IE_Size) )
	{
		if (indexEntry->IE_Flags & INDEX_ENTRY_NODE) {//��һ���ӽڵ�
			
			if (!(indexEntry->IE_Flags & INDEX_ENTRY_END))
			{//��ǰ������û�н���  ��Ҫ�͵�ǰ�Ľڵ�Ƚ�
				fn = PFILE_NAME(indexEntry->IE_Stream);  //��ȡ�ļ�������
				memcpy(nameBuf , fn->FN_FileName , fn->FN_NameSize*2);
				nameBuf[fn->FN_NameSize] = 0;

				//���ļ������бȽ�
				//if(fn->FN_NameSize != len || !(fn->FN_DOSAttr & attr)) 		continue;	
				int flag = FileNameCmp(nameBuf , name , len  , FALSE);
				if (flag == 0)
				{ //���ǵ�ǰ�Ľڵ�
/*					if(!(fn->FN_DOSAttr & attr)) 		continue;	//���Բ���*/
					mftIdx->QuadPart = (indexEntry->IE_FR.QuadPart << 16 )>> 16;
					
					if (NULL != pLIStartFDT)
					{//����FDT����ʼλ��

						//���㵱ǰMFT��ƫ��
						pLIStartFDT->QuadPart = GetSectorOfMFTRecode(parentMft).QuadPart * SECTOR_SIZE;
						//��ǰMFT��INDEX_ROOT����
						pLIStartFDT->QuadPart += pAttrItem->off;
						//INDEX_ROOT�еĵ�ǰ��ڵ�ƫ��
						pLIStartFDT->QuadPart += (((BYTE*)indexEntry) - pAttrItem->attrDataBuf.data());
						
						//��ǰFDT�ĳ���
						if (NULL != fdtLen) *fdtLen = indexEntry->IE_Size;
					}

					root.Close();
					return DR_OK;
				}else if (flag > 0)
				{//���������vcn
					//����������еĿձ������趨������غ�
					vcn = *PLONG_INT((BYTE*)indexEntry + indexEntry->IE_Size - 8);
					res = WalkNode(&root  , vcn , name , len , mftIdx , pLIStartFDT , fdtLen);
					if (res == DR_OK){ //���ҳɹ� 
						root.Close();					
						return DR_OK;   
					}    
					else if (res == DR_NO) //��ǰblockû���ҵ�
						continue;
					else					//������
						break;

				}else  //flag < 0 
				{//���ﲻ�ɽ���  ��һ���ɽ���
					continue;
				}
			}else
			{//�������һ��indexEntry ��������vcn
				//����������еĿձ������趨������غ�
				vcn = *PLONG_INT((BYTE*)indexEntry + indexEntry->IE_Size - 8);
				res = WalkNode(&root  , vcn , name , len , mftIdx , pLIStartFDT , fdtLen);
				root.Close();
				return res;//���һ�������ǰ����ֱ������
			}
		}

		if (indexEntry->IE_Flags & INDEX_ENTRY_END) { //���˽�β��	
			res = DR_NO;
			break;  //������ʼ���vcn
		}
		
		//���ִ�е������˵Ļ��ͱ�ʾ������һ����ͨ�Ľڵ�
		fn = PFILE_NAME(indexEntry->IE_Stream);  //��ȡ�ļ�������
		memcpy(nameBuf , fn->FN_FileName , fn->FN_NameSize*2);
		nameBuf[fn->FN_NameSize] = 0;
		//if(fn->FN_NameSize != len || !(fn->FN_DOSAttr & attr)) 		continue;	
		if (!FileNameCmp(nameBuf , name  , len , FALSE))
		{//�ҵ���
/*			if(!(fn->FN_DOSAttr & attr)) 		continue;	//���Բ���*/
			mftIdx->QuadPart = (indexEntry->IE_FR.QuadPart << 16 )>> 16;

			if (NULL != pLIStartFDT)
			{//����FDT����ʼλ��

				//���㵱ǰMFT��ƫ��
				pLIStartFDT->QuadPart = GetSectorOfMFTRecode(parentMft).QuadPart * SECTOR_SIZE;
				//��ǰMFT��INDEX_ROOT����
				pLIStartFDT->QuadPart += pAttrItem->off;
				//INDEX_ROOT�еĵ�ǰ��ڵ�ƫ��
				pLIStartFDT->QuadPart += (((BYTE*)indexEntry) - pAttrItem->attrDataBuf.data());

				//��ǰFDT�ĳ���
				if (NULL != fdtLen) *fdtLen = indexEntry->IE_Size;
			}

			root.Close();
			return DR_OK;
		}
	}
	root.Close();
	return res;
}

DRES DNtfs::WalkNode(DNtfsFile* root , LONG_INT vcn , const  WCHAR* name, int len , PLONG_INT mftIdx  , PLONG_INT pLIStartFDT , WORD* fdtLen/* , DWORD attr*/)
{
	LONG_INT	lcn			= {0};//�߼��غ�
	DRES		res			= DR_NO;
	DNtfsAttr	attrAllocation;
	DNtfsAttr	attrRoot;
	INDEX_ENTRY* indexEntry	= NULL;
	BYTE*		blockEnd	= 0;
	PFILE_NAME	fn			= NULL;        //�ļ�����ָ��
	WCHAR		nameBuf[255] = {0};

	//��ø�����
	res = root->FindAttribute(AD_INDEX_ROOT , &attrRoot);
	//������
	if (res != DR_OK)	return DR_INIT_ERR;

	//���INDEX_ALLOCATION����
	res = root->FindAttribute( AD_INDEX_ALLOCATION, &attrAllocation);
	//��������
	if (res != DR_OK)	return DR_INIT_ERR;  //�ڲ�����

	//������Ĵ�С
	DWORD blockSize = attrRoot.IRGetIndexBlockSize();

	//������һ�������������Ļ���
	std::vector<BYTE> indexBlockBuf(blockSize, 0);

	//����߼��غ�
	lcn = root->GetLCNByVCN(vcn , NULL);
	//lcn = attrAllocation.IAGetLCNByVCN(&vcn , NULL);
	lcn.QuadPart *= mSecPerClu; 
	ReadData(indexBlockBuf.data(), &lcn, blockSize, TRUE);
	
	//�ȱ���һ��index_block������λ��
	if (NULL != pLIStartFDT)
		pLIStartFDT->QuadPart = lcn.QuadPart * SECTOR_SIZE;

	//�߻ָ�usa
	PINDEX_BLOCK_HEAD ibh = PINDEX_BLOCK_HEAD(indexBlockBuf.data());

	//���usa������  
	int		usaCnt	= ibh->IB_USNSz ;
	WORD	us		= 0;
	DWORD	offUsa	=  ibh->IB_USOff;
	int		i		= 0;
	DWORD	tusn, usn = GetWORD(indexBlockBuf.data() + offUsa);
	for (i = 1 ; i < usaCnt ; ++i)
	{
		us = GetWORD(indexBlockBuf.data() + offUsa + i * 2);
		tusn = GetWORD(indexBlockBuf.data() + 512 * i - 2);
		SetWORD(indexBlockBuf.data() + 512 * i - 2, us);
	}

	if (vcn.QuadPart != ibh->IB_VCN.QuadPart)
	{
		//����غŲ�ƥ��
		return DR_INIT_ERR;
	}

	//������ĵ�һ����ڵĵ�ַ
	indexEntry = PINDEX_ENTRY((BYTE*)&ibh->IB_indexHead + ibh->IB_indexHead.IH_EntryOff);
	//������Ľ���λ��
	blockEnd = (BYTE*)&ibh->IB_indexHead + ibh->IB_indexHead.IH_AllocSize;

	//��������������
	for (;(BYTE*)indexEntry < blockEnd ; 
		indexEntry = PINDEX_ENTRY((BYTE*)indexEntry + indexEntry->IE_Size) )
	{
		//����������ڵ�Entry���Ậ������.Ҳ�����к��ӽڵ��ָ��
		if (indexEntry->IE_Flags & INDEX_ENTRY_NODE) {//��һ���ӽڵ�

			if (!(indexEntry->IE_Flags & INDEX_ENTRY_END))
			{//��ǰ������û�н���  ��Ҫ�͵�ǰ�Ľڵ�Ƚ�
				fn = PFILE_NAME(indexEntry->IE_Stream);  //��ȡ�ļ�������
				memcpy(nameBuf , fn->FN_FileName , fn->FN_NameSize*2);
				nameBuf[fn->FN_NameSize] = 0;

				//���ļ������бȽ�
				//if(fn->FN_NameSize != len || !(fn->FN_DOSAttr & attr)) 		continue;	
				int flag = FileNameCmp(nameBuf , name  ,len , FALSE);
				if (flag == 0)
				{ //���ǵ�ǰ�Ľڵ�
/*					if(!(fn->FN_DOSAttr & attr)) 		continue;	//���Բ���*/
					mftIdx->QuadPart = (indexEntry->IE_FR.QuadPart << 16 )>> 16;

					if (NULL != pLIStartFDT)
					{//����FDT����ʼλ��

						//INDEX_ROOT�еĵ�ǰ��ڵ�ƫ��
						pLIStartFDT->QuadPart += (((BYTE*)indexEntry) - indexBlockBuf.data());

						//��ǰFDT�ĳ���
						if (NULL != fdtLen) *fdtLen = indexEntry->IE_Size;
					}

					res = DR_OK;
					break;
				}else if (flag > 0)
				{//���������vcn
					//����������еĿձ������趨������غ�
					vcn = *PLONG_INT((BYTE*)indexEntry + indexEntry->IE_Size - 8);
					res = WalkNode(root  , vcn , name , len , mftIdx , pLIStartFDT , fdtLen);
					if (res /*!=*/== DR_NO)
						continue;
					else //�ҵ����߳�����
						break;
				}else  //flag < 0 
				{//���ﲻ�ɽ���  ��Ҫ����һ��
					continue;
				}
			}else//����һ���ڵ�  ��Ҫ����ȥ
			{
				vcn = *PLONG_INT((BYTE*)indexEntry + indexEntry->IE_Size - 8);
				res = WalkNode(root  , vcn , name , len , mftIdx , pLIStartFDT , fdtLen);
				break;
			}
		}

		if (indexEntry->IE_Flags & INDEX_ENTRY_END) { //���˽�β��	
			res = DR_NO;
			break;  //������ʼ���vcn
		}

		//���ִ�е������˵Ļ��ͱ�ʾ������һ����ͨ�Ľڵ�
		fn = PFILE_NAME(indexEntry->IE_Stream);  //��ȡ�ļ�������
		memcpy(nameBuf , fn->FN_FileName , fn->FN_NameSize*2);
		nameBuf[fn->FN_NameSize] = 0;
		if(fn->FN_NameSize != len /*|| !(fn->FN_DOSAttr & attr)*/) 		continue;	
		if (!FileNameCmp(nameBuf , name , len  , FALSE))
		{//�ҵ���
			mftIdx->QuadPart = (indexEntry->IE_FR.QuadPart << 16 )>> 16;

			if (NULL != pLIStartFDT)
			{//����FDT����ʼλ��

				//INDEX_ROOT�еĵ�ǰ��ڵ�ƫ��
				pLIStartFDT->QuadPart += (((BYTE*)indexEntry) - indexBlockBuf.data());

				//��ǰFDT�ĳ���
				if (NULL != fdtLen) *fdtLen = indexEntry->IE_Size;
			}

			res = DR_OK;
			break;
		}
	}

	return res;
}

DRES DNtfs::FindFile(DNtfsFile* root, FINDER* hFin)
{
	*hFin = NULL;

	//�豸������û�д�
	if (mDevName.empty())
		return DR_NO_OPEN;

	//����
	if (root == NULL || hFin == NULL)
		return DR_INVALED_PARAM;
	if (!root->IsDir())
		return DR_IS_FILE;	//��Ҫ����Ŀ¼

	//�ڴ����
	std::unique_ptr<FIND_FILE_HANDER> hFind(new FIND_FILE_HANDER());
	hFind->vcn.QuadPart = -1;
	hFind->index = 0;

	//��ָ�����ļ�Ŀ¼
	DRES res = OpenFileW(&hFind->dir, root->GetMftIndex());
	//��Ч��·��
	if (res == DR_NO)
		return DR_INVALID_NAME;
	if (res != DR_OK)
		return DR_INIT_ERR;		//�����ĳ�ʼ����������ڲ����

	//Ҫ���صľ��
	*hFin = FINDER(hFind.release());
	
	return DR_OK;
}

DRES DNtfs::FindFile(const char* root, FINDER* hFind)
{
	WCHAR wPath[MAX_PATH + 1] = {0};
	//����
	if (root == NULL || hFind == NULL) return DR_INVALED_PARAM;

	MultyByteToUnic(root, wPath, MAX_PATH + 1);

	return FindFile(wPath, hFind);
}

DRES DNtfs::FindFile(const WCHAR* path , FINDER* hFin /*,PLONG_INT mftIndx*/)
{
	*hFin = NULL;

	//����
	if (!path || !hFin /*|| !mftIndx */)
		return DR_INVALED_PARAM;

	if (mDevName.empty())
		return DR_NO_OPEN;		//ϵͳ��üͷ��ʼ��

	//������Ҿ��
	std::unique_ptr<FIND_FILE_HANDER> hFind(new FIND_FILE_HANDER());
	hFind->vcn.QuadPart = -1;
	hFind->index = 0;

	//��ָ�����ļ�Ŀ¼
	DRES res = OpenFileW(path, &hFind->dir /*, ATTR_DIRECTORY|ATTR_DIRECTORY_INDEX*/);
	//��Ч��·��
	if (res == DR_NO)
		return DR_INVALID_NAME;
	if (res != DR_OK)
		return DR_INIT_ERR;		//�����ĳ�ʼ����������ڲ����

	//Ҫ���صľ��
	*hFin = FINDER(hFind.release());

	return res;
}

DRES DNtfs::FindNext(/*PFIND_FILE_HANDER*/FINDER hFin ,PLONG_INT mftIndx)
{
	//һ����ڵ�����ʼλ��
	PINDEX_ENTRY	indexEntry;
	BYTE*			blockEnd;
	PFILE_NAME      fn;
	DNtfsAttr		attrRoot;
	DNtfsAttr		attrBitMp;
	//NtfsAttr		attrAllocation;
	DRES			res;
	int				index;
	int				bitCnt; 
	std::vector<BYTE> indexBlockBuf;
	DWORD			blockSize;
	LONG_INT		lcn;
	PFIND_FILE_HANDER hFind = (PFIND_FILE_HANDER)hFin;

	//����Ƿ��Ѿ���ʼ�����ļ�ϴϵͳ
	if (mDevName.empty())
		return DR_NO_OPEN;
	if (!hFind || !mftIndx)
		return DR_INVALED_PARAM;
	if (!hFind->dir.IsFileValid())
		return DR_INVALID_HANDLE;//��Ч�Ĳ��Ҿ��

	if (!hFind || !mftIndx)
		return DR_INVALED_PARAM;

	if (!hFind->dir.IsFileValid())
		return DR_INVALID_HANDLE;//��Ч�Ĳ��Ҿ��

	//��ø�����
	res = hFind->dir.FindAttribute(AD_INDEX_ROOT , &attrRoot);
	//������
	if (res != DR_OK)
		return DR_INIT_ERR;

	//������ĵ�һ����ڵĵ�ַ
	indexEntry = PINDEX_ENTRY(attrRoot.IRGetFistEntry());
	//������Ľ���λ��
	blockEnd = (BYTE*)indexEntry + attrRoot.IRGetAlloIndexEntriesSize();

	//��������������
	if (hFind->vcn.QuadPart == -1)//mft��¼�в���
	{
		for (index = 0;(BYTE*)indexEntry < blockEnd
			;indexEntry = PINDEX_ENTRY((BYTE*)indexEntry + indexEntry->IE_Size) 
			, ++index )
		{
			if ((BYTE*)indexEntry + 0x10 > blockEnd || 
				(BYTE*)indexEntry + indexEntry->IE_DataSize > blockEnd)
			{//�����˻�����
				return DR_INIT_ERR;
			}
			//����������ڵ�Entry���Ậ������.Ҳ�����к��ӽڵ��ָ��
			if (indexEntry->IE_Flags & INDEX_ENTRY_END)  //���˽�β��
				break;  //������ʼ���vcn

			//��û��ָ����λ��
			if (index < hFind->index) continue;

			fn = PFILE_NAME(indexEntry->IE_Stream);  //��ȡ�ļ�������
			if (fn->FN_NamSpace == NS_DOS)//��ҪDOS������
			{//ָ����λ�õ����ʱһ��dos���
				++hFind->index;
				continue;
			}
			//�ҵ���
			mftIndx->QuadPart = indexEntry->IE_FR.QuadPart << 16 >> 16;
			++hFind->index;//��һ�����ҵ�Ŀ¼
			return DR_OK;
		}
		
		//mft��¼��û������
		res = hFind->dir.FindAttribute(AD_BITMAP , &attrBitMp);
		if (res == DR_NO)
		{//û��Bitmap���� ,Ҳ���ǲ�������
			return DR_FAT_EOF;
		}

		//��õ�һ��vcn
		bitCnt = attrBitMp.R_GetAttrLen() * 8;  //bitmap�е�bit��
		hFind->vcn.QuadPart = 0;
		for(; hFind->vcn.QuadPart < bitCnt ; ++hFind->vcn.QuadPart)
		{
			if (attrBitMp.BMIsBitSet(hFind->vcn , this))
				break;
		}
		if (hFind->vcn.QuadPart == bitCnt)
			return DR_FAT_EOF;//û���ҵ���Ч��vcn
		else
			hFind->index = 0;//�ҵ��˵�һ��vcn
	}
	
	//rootIndex���Ѿ��ҹ���,��������indexBlock����

	//��û������Ĵ�С
	blockSize = attrRoot.IRGetIndexBlockSize();
	//������һ�������������Ļ���
	indexBlockBuf.resize(blockSize, 0);

	//���λͼ����
	res = hFind->dir.FindAttribute(AD_BITMAP , &attrBitMp);
	if (res == DR_NO) return DR_INIT_ERR;  //û��λͼ����   
	//���vcn=��lvn ���ձ�����
// 	res = hFind->dir.FindAttribute(AD_INDEX_ALLOCATION , &attrAllocation);
// 	if (res == DR_NO) return DR_INIT_ERR;  //û��vcn->lcnӳ�������


	//��������B+��
	while (1)
	{
		//����߼��غ�
		lcn = hFind->dir.GetLCNByVCN(hFind->vcn , NULL);
		//lcn = attrAllocation.IAGetLCNByVCN(&hFind->vcn , NULL);
		lcn.QuadPart *= mSecPerClu; 
		ReadData(indexBlockBuf.data(), &lcn, blockSize, TRUE);//��ȡָ��������

		//�߻ָ�usa
		PINDEX_BLOCK_HEAD ibh = PINDEX_BLOCK_HEAD(indexBlockBuf.data());

		//���usa������  
		int		usaCnt = ibh->IB_USNSz ;
		WORD	us = 0;
		DWORD	offUsa =  ibh->IB_USOff;
		int		i = 0;
		DWORD	usn = GetWORD(indexBlockBuf.data() + offUsa);
		for (i = 1 ; i < usaCnt ; ++i){
			us = GetWORD(indexBlockBuf.data() + offUsa + i * 2);
			SetWORD(indexBlockBuf.data() + 512 * i - 2, us);
		}

		//��Ӧ����غ�
		if (hFind->vcn.QuadPart != ibh->IB_VCN.QuadPart)
		{//����غŲ�ƥ��
			res = DR_INIT_ERR;
			break;
		}

		//������ĵ�һ����ڵĵ�ַ
		indexEntry = PINDEX_ENTRY((BYTE*)&ibh->IB_indexHead + ibh->IB_indexHead.IH_EntryOff);
		//������Ľ���λ��
		blockEnd = (BYTE*)&ibh->IB_indexHead + ibh->IB_indexHead.IH_AllocSize;

		index = 0;///�������е�����
		//��������������
		for (;(BYTE*)indexEntry != blockEnd ; 
			indexEntry = PINDEX_ENTRY((BYTE*)indexEntry + indexEntry->IE_Size)
			,++index)
		{
			if ((BYTE*)indexEntry + 0x10 > blockEnd || 
				(BYTE*)indexEntry + indexEntry->IE_DataSize > blockEnd)
			{//������ 0x10��INDEX_RNTRYͷ���Ĵ�С
				res =  DR_INIT_ERR;
				break;
			}

			//����������ڵ�Entry���Ậ������.Ҳ�����к��ӽڵ��ָ��
			if (indexEntry->IE_Flags & INDEX_ENTRY_END)  //���˽�β��,��Ҫ�����һ��vcn
				break;  //������ʼ���vcn

			//��û��ָ����λ��
			if (index < hFind->index) continue;

			fn = PFILE_NAME(indexEntry->IE_Stream);  //��ȡ�ļ�������
			if (fn->FN_NamSpace == NS_DOS)//��Ҫ��DOS������
			{//ָ����λ�õ����ʱһ��dos���
				++hFind->index;
				continue;
			}
			//�ҵ���
			mftIndx->QuadPart = indexEntry->IE_FR.QuadPart << 16 >> 16;
			++hFind->index;//��һ�����ҵ�Ŀ¼
			res = DR_OK;
			break;
		}

		if(indexEntry->IE_Flags & INDEX_ENTRY_END)//��ӦΪ������indexEntry�Ľ�������������
		{
			//��һ���غ�
			++hFind->vcn.QuadPart;
			hFind->index = index =0;

			bitCnt = attrBitMp.R_GetAttrLen() * 8;  //bitmap�е�bit��
			for(; hFind->vcn.QuadPart < bitCnt ; ++hFind->vcn.QuadPart) {
				if (attrBitMp.BMIsBitSet(hFind->vcn , this))
					break;
			}
			if (hFind->vcn.QuadPart == bitCnt)
			{//û������Ч��vcn
				res = DR_FAT_EOF;
				break;
			}
		}else
			break;
	}

	return res;
}

void DNtfs::CloseFind(/*PFIND_FILE_HANDER*/FINDER hFin)
{
	PFIND_FILE_HANDER  hFind =  (PFIND_FILE_HANDER)hFin;
	if (hFind && hFind->dir.IsFileValid())
	{
		hFind->dir.Close();
		delete hFind;
	}
}

DRES DNtfs::IsContainNTFSFlag(const WCHAR* cDevName, LONG_INT offset)
{
	//��������
	if (cDevName == NULL)
		return DR_INVALED_PARAM;

	//���豸
	HANDLE hDev = ::CreateFile(cDevName,
								GENERIC_READ | GENERIC_WRITE,
								FILE_SHARE_READ | FILE_SHARE_WRITE,
								NULL,
								OPEN_EXISTING,
								0,
								NULL);
	if (hDev == INVALID_HANDLE_VALUE) //���豸ʧ��
		return DR_OPEN_DEV_ERR;

	DRES		res = DR_OK;
	if (offset.QuadPart > 0)
	{//ƫ��
		//�����ļ�ָ��
		offset.LowPart = ::SetFilePointer(hDev , offset.LowPart , PLONG(&(offset.HighPart)) ,FILE_BEGIN );
		if (offset.LowPart == -1 && ::GetLastError() != NO_ERROR )
			res = DR_DEV_CTRL_ERR;
	}

	//��ȡ����
	NTFS_DBR	nDbr = { 0 };
	DWORD		dwReaded = 0;
	if(DR_OK == res && !::ReadFile(hDev , &nDbr , sizeof(NTFS_DBR) 
		,&dwReaded ,NULL) && dwReaded != sizeof(NTFS_DBR))	
		res =  DR_DEV_IO_ERR;
	
	//����Ҫ��
	CloseHandle(hDev);
	//�豸��ز���ʧ��
	if (res != DR_OK) return res;

	//��NTFS�ļ�����û�й����ģ����Ժܶ�Ķ��������Ǵ�ţ�ǲ�+���
	if ( ( nDbr.dbrEnd != MBR_END)				//DBR�Ľ������
		||(nDbr.bpbBytePerSec != SECTOR_SIZE))	//������С��֮����512��
		return DR_NO;

	//NTFS���
	//4E 54 46 53 20  20 20 20
	if ( ( nDbr.dbrOemName[0] != 0x4E)	//N
		||(nDbr.dbrOemName[1] != 0x54)	//T
		||(nDbr.dbrOemName[2] != 0x46)	//F
		||(nDbr.dbrOemName[3] != 0x53)	//S
		||(nDbr.dbrOemName[4] != 0x20)	//�ո�
		||(nDbr.dbrOemName[5] != 0x20)	//�ո�
		||(nDbr.dbrOemName[6] != 0x20)	//�ո�
		||(nDbr.dbrOemName[7] != 0x20))	//�ո�
		return DR_NO;

	//�����ܵ�����Ļ�Ӧ����û�����
	return DR_OK;
}

const WCHAR* DNtfs::GetDevName()
{
	return mDevName.c_str();
}

LONG_INT DNtfs::GetClustForMFT()
{
	LONG_INT clust = {0};
	if (mDevName.empty())
		return clust;

	return mCluForMFT;
}

LONG_INT DNtfs::GetClustForMFTMirr()
{
	LONG_INT clust = {0};
	if (mDevName.empty())
		return clust;

	return mCluForMFTMirr;
}


LONG_INT DNtfs::GetSectorOfMFTRecode( LONG_INT mft )
{
#define _USE_MFT_BLOCK_ 1 

#if _USE_MFT_BLOCK_ == 0 //��ʹ��MFT_BLOCK ������MFT��¼
	LONG_INT	liSector = {-1};
	DNtfsFile	mftFile;//��ǰ���MFT�ļ�
	DRES		res = DR_OK;
	DNtfsFile::PAttrItem pAttrItem = NULL;
	DNtfsAttr	ntfsAttr;
	LONG_INT	vcn = {-1};
	DRun		run;

	//�豸��û�д򿪵ģ�
	if (NULL == mDevName) return liSector;
	res = OpenFileW(&mftFile , SYS_FILE_MFT);
	if (res != DR_OK) liSector;  //�ļ���ʧ��

	//����λͼ����
	pAttrItem = mftFile.FindAttribute(AD_BITMAP);
	if (NULL == pAttrItem) return liSector;//��������ǲ������

	//��ʼ������
	ntfsAttr.InitAttr(pAttrItem->attrDataPtr);
	//�ж�ָ����MFT�Ƿ���ʹ��
	if(FALSE == ntfsAttr.BMIsBitSet(mft , this ))
		return liSector;//ָ�����ļ���¼������
	

	//��������������� 
	if(DR_OK != mftFile.FindNoNameDataAttr(&ntfsAttr))
	{//��ȡ����ʧ��
		return liSector;
	}

	//�����������
	if(DR_OK != run.InitRunList(&ntfsAttr))
		return liSector;

	//�����ļ�����vcn
	vcn.QuadPart = (mft.QuadPart * SECTOR_PER_RECODE) / GetSecPerClust();

	//��ѯLCN
	vcn = run.GetLCNByVCN(vcn , NULL);
	run.Close();
	if (-1 == vcn.QuadPart)
		return liSector;

	//�����������ƫ��
	liSector.QuadPart = (vcn.QuadPart * mSecPerClu) + ((mft.QuadPart * SECTOR_PER_RECODE) % mSecPerClu);

	//��Դ������Ҫ�ͷŵ�
	mftFile.Close();
	return liSector;
#elif _USE_MFT_BLOCK_ == 1   //ʹ��MFT_BLOCK ������MFT��¼  2012-07-18 1:07
	LONG_INT	liSector = {-1};
	DRES		res = DR_OK;
	DNtfsAttr	ntfsAttr;
	DWORD		i;
	LONG_INT	curMft;	//�ڵ�ǰ������MFT��¼��

	//�豸��û�д򿪵ģ�
	if (mDevName.empty())
		return liSector;
// 	res = OpenFileW(&mftFile , SYS_FILE_MFT);
// 	if (res != DR_OK) liSector;  //�ļ���ʧ��
//
// 	//����λͼ����
// 	pAttrItem = mftFile.FindAttribute(AD_BITMAP);
// 	if (NULL == pAttrItem) return liSector;//��������ǲ������
// 
// 	//��ʼ������
// 	ntfsAttr.InitAttr(pAttrItem->attrDataPtr);
// 	//�ж�ָ����MFT�Ƿ���ʹ��
// 	if(FALSE == ntfsAttr.BMIsBitSet(mft , this ))
// 		return liSector;//ָ�����ļ���¼������


	//����ÿһ��MFT_BLOCK
	for(i = 0 ; i < mMftBlockCnt; ++i)
	{
		if (( mPMftBlock[i].liStartMft.QuadPart <= mft.QuadPart ) &&
			( mPMftBlock[i].liStartMft.QuadPart + 
			  mPMftBlock[i].liMftCnt.QuadPart > mft.QuadPart ) )
		{//Ҫ���ҵ�MFT������һ����
			curMft.QuadPart = mft.QuadPart - mPMftBlock[i].liStartMft.QuadPart;
			liSector.QuadPart = mPMftBlock[i].liStartSector.QuadPart 
				+ curMft.QuadPart * SECTOR_PER_RECODE;
			return liSector;
		}
	}

	return liSector;
#endif
}

