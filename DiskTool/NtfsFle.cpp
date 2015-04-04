/***********************************************************************
 * FileName:	NtfsFle.cpp
 * Author:		����
 * Created:		2012��4��20�� ������
 * Purpose:		ʵ��NtfsFile����ķ���
 * Comment:		��������Ҫ��;�ǳ���һ��Ntfs���ϵ��ļ����ṩ�˸��ַ���
 *
 * Modify:		����һ��BUG��������һ��run������������أ���Ŀ¼��Ҫ��ȡ
 *				�ڶ���IndexBlockʱ(IndexBlockһ����4K�����ҵĵ��Ըպ�һ��)
 *				����߼��غ�ʵ���ǵ�һ������غŵ��߼��غţ���������Ҫ����
 *				ʵ����Ҫ����ֺ��뵱ǰrun��ʼ����ֺ�ֻ��,�������������
 *
 *				��ȡ�ļ�ʱ��Ϊ�˲����򵥣�ÿ�ζ����ļ���дָ���ƶ����صı�
 *				��,Ȼ���ȡһ�ص����ݣ��ڼ�����һ�������ж�����Ч���ݣ�Ҳ��
 *				�ǵ�ǰ�����е���Ч����ƫ�ƣ��������һ�ζ�ȡ����������ϵ�
 *				������ô�������Ķ�ȡ��û�в���Ҫ���㻺�������Ч����ƫ���ˣ�
 *				�Ǹ����ɴ���ƫ������һ���������ڣ����Ե�һ�ζ�ȡ֮��ͱ���
 *				��Ϊ0,�����������ȡ����ʧ�ܡ�2012-07-04 08:42
 *
 ***********************************************************************/

#include "disktool.h"
#include "stut_connf.h"

#pragma warning(disable:4996)


DNtfsFile::DNtfsFile()
	: mAttrCnt(0)
	, mFS(0)
	, mFDTLen(0)
{
	this->mFilePointer.QuadPart = 0;
	this->mMftIdx.QuadPart = -1;
	mLIStartFDT.QuadPart = 0;
}

DRES DNtfsFile::InitRecode(DNtfs* fs ,LONG_INT mftIndex)
{
	DRES res = DR_OK;
	BYTE mRecodeBuf[MFT_RECODE_SIZE];//��¼���� 1K
	PFILE_RECODE_HEAD pfrh;
	
	if (this->mFS)//�ļ��Ѿ���ʼ����
		return DR_ALREADY_OPENDED;

	//��ȡָ����mft��¼
	res = fs->ReadMFT(mRecodeBuf , &mftIndex);
	
	if (res != DR_OK){//��ȡ����ʧ��
		this->mMftIdx.QuadPart = -1;
		return res;
	}
	
	this->mFS = fs;

	//��ʼ���ļ���¼ͷ��
	pfrh = PFILE_RECODE_HEAD(mRecodeBuf);
	this->mMftHeadBuf.resize(pfrh->FR_1stAttrOff, 0);
	memcpy(this->mMftHeadBuf.data(), mRecodeBuf, pfrh->FR_1stAttrOff );

	//��ȡ���ݳɹ�
	this->mMftIdx = mftIndex;	
	//this->head = PFILE_RECODE_HEAD(this->mRecodeBuf);
	
	//��ԭÿ������ĩβ��������USA/USN
	WORD usa = GetUSAItem(0);
	SetWORD(mRecodeBuf + 510 , usa);
	usa = GetUSAItem(1);
	SetWORD(mRecodeBuf + 1022 , usa);

// 	if (this->mAttrArr)
// 	{//�ƺ������ڴ�û���ͷ�  
// 		free(this->mAttrArr);
// 		this->mAttrArr = NULL;
// 		this->mAttrCnt = 0;
// 	}

	//��ʼ�������б�
	return InitAttrList((BYTE*)mRecodeBuf);
}
WORD DNtfsFile::GetUSAItem(int index)
{
	//�������кŵ�ƫ��
	WORD offUSN = PFILE_RECODE_HEAD(mMftHeadBuf.data())->FR_USOff;
	offUSN = GetWORD(mMftHeadBuf.data() + offUSN + 2 + index * 2);
	return offUSN;
}

LONG_INT DNtfsFile::GetMftIndex()
{
	return this->mMftIdx;
}
BOOL DNtfsFile::IsFileValid()
{
	return this->mFS != NULL;
}
LONG_INT DNtfsFile::GetParentMftIndex()
{
	LONG_INT parent;
	DNtfsAttr nameAttr;
	DRES	 res = DR_OK;

	parent.QuadPart = -1;

	//�����ļ�����¼��
	res = this->FindAttribute(AD_FILE_NAME , &nameAttr );

	if (res !=  DR_OK) return parent; //ʧ����
		
	parent.QuadPart = (nameAttr.FNGetParentMftIndx().QuadPart << 16)>>16;
	return parent;
}

LONG_INT DNtfsFile::GetCreateTime()
{
	LONG_INT tim = {0};
	DNtfsAttr attr;
	DRES	 res = DR_OK;
	PSTD_INFO psi = NULL;

	
	//�ļ�û�г�ʼ��
	if (!this->mFS) return tim;

	//��ñ�׼��Ϣ
	res = this->FindAttribute(AD_STANDARD_INFORMATION , &attr);
	if (res != DR_OK)  return tim;  //������

	//��׼����
	psi = PSTD_INFO(attr.R_GetAttrBodyPtr());
	tim.QuadPart = psi->SI_CreatTime.QuadPart / 10000000 - (__int64)NTFS_TIME_OFFSET;
	
	return tim;
}
LONG_INT DNtfsFile::GetAlteredTime()
{
	LONG_INT tim = {0};
	DNtfsAttr attr;
	DRES	 res = DR_OK;
	PSTD_INFO psi = NULL;

	//�ļ�û�г�ʼ��
	if (!this->mFS) return tim;

	//��ñ�׼��Ϣ
	res = this->FindAttribute(AD_STANDARD_INFORMATION , &attr);
	if (res != DR_OK)  return tim;  //������

	psi = PSTD_INFO(attr.R_GetAttrBodyPtr());
	tim.QuadPart = psi->SI_AlterTime.QuadPart / 10000000 - NTFS_TIME_OFFSET;

	return tim;
}
LONG_INT DNtfsFile::GetMFTChgTime()
{
	LONG_INT tim = {0};
	DNtfsAttr attr;
	DRES	 res = DR_OK;
	PSTD_INFO psi = NULL;

	//�ļ�û�г�ʼ��
	if (!this->mFS) return tim;

	//��ñ�׼��Ϣ
	res = this->FindAttribute(AD_STANDARD_INFORMATION , &attr);
	if (res != DR_OK)  return tim;  //������

	psi = PSTD_INFO(attr.R_GetAttrBodyPtr());
	tim.QuadPart = psi->SI_MFTChgTime.QuadPart / 10000000 - NTFS_TIME_OFFSET;

	return tim;
}
LONG_INT DNtfsFile::GetReadTime()
{
	LONG_INT tim = {0};
	DNtfsAttr attr;
	DRES	 res = DR_OK;
	PSTD_INFO psi = NULL;

	//�ļ�û�г�ʼ��
	if (!this->mFS) return tim;

	//��ñ�׼��Ϣ
	res = this->FindAttribute(AD_STANDARD_INFORMATION , &attr);
	if (res != DR_OK)  return tim;  //������

	psi = PSTD_INFO(attr.R_GetAttrBodyPtr());
	tim.QuadPart = psi->SI_ReadTime.QuadPart / 10000000 - NTFS_TIME_OFFSET;

	return tim;
}

BOOL DNtfsFile::IsDir()
{
	if (/*PFILE_RECODE_HEAD(this->mRecodeBuf)->FR_Flags == ATTR_DIRECTORY 
		|| */PFILE_RECODE_HEAD(mMftHeadBuf.data())->FR_Flags & 0x2
		)
		return TRUE;
	else 
		return FALSE;
}
DRES DNtfsFile::GetFileName(WCHAR* nameBuf , DWORD len , BYTE nameSpace)
{
	DNtfsAttr attr;
	DWORD	 atttIdx = 0;
	DWORD	 tAttr   = 0;
	DRES	 res     = DR_OK;
	DWORD	 nameLen = 0;
	
	//����û��ʼ��Ŷ
	if (this->mFS == NULL) 	return DR_NO_OPEN;
	//������
	if (!nameBuf ) return DR_INVALED_PARAM;
	wcscpy(nameBuf , L"");//����ջ���



	if (nameSpace != NS__ALL) {
		while(1){
			res = FindAttribute(AD_FILE_NAME , &attr , &atttIdx);
			//û��ָ���ļ����ռ���ļ�������
			if (res != DR_OK)
				return DR_NO_FILE_NAME;

			if (attr.FNGetFileNameSpase() == nameSpace)
				break;//�ҵ�����Ӧ���ļ���
			++atttIdx;
		}
	}else{	//���еĶ�����
		atttIdx = 0;
		tAttr = 0;

		while(1){
			res = FindAttribute(AD_FILE_NAME , &attr , &atttIdx);
			if (res == DR_NO)
				break;//û��ָ�����ļ�����
			else if (res != DR_OK)
				return DR_NO_FILE_NAME;
			
			//�ȱ���һ���ҵ����ļ�������
			tAttr = atttIdx;

			if (attr.FNGetFileNameSpase() &/*==*/ NS_WIN32)  //��һ���Ǳ�����win32�������ռ� �����ݾͿ���
				break;//�ҵ�����Ӧ���ļ���
			++atttIdx;
		}

		//�·�һ�µĴ���û�б�Ҫ��һ����ļ���֧��WIN32
		//�Һ��뻹ֻ����NS_WIN32��NS_DOS_WIN32�Լ�NS_DOS��
		//����ֻҪ������NS_DOS�ܶ������NS_WIN32����NS_DOS_WIN32��
		//Ҳ����˵NS_DOS���ᵥ�����֣���������Ĵ�����û����һ�����Բ�Ҫ
		if (res == DR_NO && tAttr != 0)
		{//û��win32��,�������
// 			while(1){
// 				res = FindAttribute(AD_FILE_NAME , &attr , NULL);
// 				if (res != DR_OK)//û���ļ�������
// 					return DR_NO_FILE_NAME;
// 				else//�ҵ���һ���ļ�������
// 					break;
// 			}
			res = FindAttribute(AD_FILE_NAME , &attr , &tAttr);
			if (res != DR_OK)//û���ļ�������
				return DR_NO_FILE_NAME;
// 			else//�ҵ���һ���ļ�������
//  					break;
		}   
	}    

	//����ļ����ĳ���
	nameLen = attr.FNGetFileNameLen();

	//������������
	if (len < nameLen*2 +1)	return DR_BUF_OVER;
	attr.FNGetFileName(nameBuf);
	nameBuf[nameLen] = 0;

	return DR_OK;
}
DWORD DNtfsFile::GetDOSAttr()
{
	DNtfsAttr attr;
	if(DR_OK != this->FindAttribute(AD_FILE_NAME , &attr))
		return 0;
	else
		return PFILE_NAME(attr.R_GetAttrBodyPtr())->FN_DOSAttr;
}
DWORD DNtfsFile::GetAttrCount()
{
	return this->mAttrCnt;
}


DNtfsFile::PAttrItem DNtfsFile::GetAttr( DWORD index )
{
	if (NULL == this->mFS)
		return NULL; //�·�����һ����Ч���ļ�
	if ( mAttrCnt <= index)//����Խ��?
		return NULL;

	return PAttrItem(mAttrArr.get()) + index;
}

DNtfsFile::PAttrItem DNtfsFile::FindAttribute( DWORD dwAttrType , const DWORD* startIdx /*= 0*/ )
{
	if (NULL == this->mFS)  //�豸��û�д�
		return NULL;
	PAttrItem attr = mAttrArr.get();
	DWORD i = 0;

	for( i = (NULL != startIdx ? (*startIdx) : 0) ; i < mAttrCnt ; ++i )
	{
		if (attr[i].attrType == dwAttrType)
			return (attr + i);   //�ҵ���ָ��������
	}

	if (i >= mAttrCnt)  //û��ָ��������
		return NULL;

	return NULL;
}


LONG_INT DNtfsFile::GetAllocSize()
{
	DNtfsAttr attr;
	LONG_INT off = {0};

	//��ȡ������������
	if (DR_OK != this->FindNoNameDataAttr(&attr))
		return off;  //ľ��������������
	else
	{
		if(attr.IsNonResident())
		{////�Ƿǳ�פ����
			return PNON_RESID_ATTR_HEAD(attr.GetAttrHeadPtr())->ATTR_AllocSize;
		}else{//��פ����,û�з����СҲ����˵ֻ��MFT�з��������ݣ�������û��
			return off;//����ͷ���0
		}
	}
//���ļ��������л�ȡ�ļ��Ĵ�С��Ϣ
// 	PAttrItem pAttr = this->FindAttribute(AD_FILE_NAME);
// 
// 	if (NULL == pAttr)
// 		return off;  //û���ļ�������
// 	else
// 	{
// 		attr.InitAttr(pAttr->attrDataPtr);
// 		return attr.FNGetAllocateSize();
// 	}
}
DRES DNtfsFile::ReadFile(char* buf, DWORD* dwReaded, DWORD dwToRead)
{
	//////////////////////////////////////////////////////////////////////////
	//�ļ������ݴ洢λ�������֣�С�ļ�һ���洢��MFT��¼�У�����ܼ򵥣�����
	//�Ĵ���һ�۾Ϳ��Կ�����������ݴ洢��������������Ļ������е��鷳��������
	//DATA��������һ��Run�б���¼��ÿһ�������(VCN)����Ӧ���߼��غ�(LCN)��
	//Ҳ����������Ҫ����������Run����,������ݵ�ʵ��λ��,�����Ŀ¼�е����ƣ�
	//ֻ��Ŀ¼�Ļ�,ÿһ��������ĩβ�����ֽڶ��ᱻ�滻�ɸ������к�(USN),�����ﲻ
	//�ᣬ�ҵ��˾�ֱ�Ӷ�ȡ�����ˣ���Ҫ��ʲô�˼ɡ����������Ǿ���Ķ�ȡ���ݵĹ�
	//���ˡ��ļ������ݶ�ȡ���������:��ʹ��һ����Ϊ���ݻ���,ÿ�ζ�ȡ���ݶ�����
	//���豸��дָ����뵽�صı߽�,Ȼ����ȥ��һ����(A)�Ĵ�С,��֮���㵱ǰ�ļ�(F)
	//��A�еĶ�дָ��(FP)����A�е�����FP��ǰ������ʱ��Ч�ģ�FP�Ժ����������Ч
	//�ĵ�ȻҲ��Ҫ���㱾�ζ�ȡ�������һ��(B)�е���Чλ��(FEP),ͬ���Ƕ�ȡһ���أ�
	//FEP֮ǰ��������Ч��FEP֮���������Ч
	//////////////////////////////////////////////////////////////////////////
	DRES	 res	  = DR_OK;
	LONG_INT realSize = {0};	//�ļ���ʵ�ʴ�С
	DNtfsAttr dataAttr;			//�ļ�����������
	LONG_INT curPtr   = {0};	//�ļ��ڵĵ�ǰ��дָ��
	LONG_INT vcn	  = {0};	//��ǰ��д�ļ���cvn 
	LONG_INT lcn	  = {0};	//��ǰ��д�ļ���lcn
	LONG_INT clustCnt = {0};	//����
	LONG_INT secOff   = {0};
	BYTE	 SecPerClust = 0;
	DWORD	 dwClustSize = 0;	//ÿ���ֽ���
	std::vector<BYTE> dataBuf;
	DWORD	 dBufoff  = 0;		//dataBuf��д���ݵ�ƫ��
	DWORD	 clustOff = 0;		//��ȡ���Ĵ��е���Ч����ƫ��(FP)
	DWORD	 toReadThisTime = 0;//���ζ�������Ч����
	DWORD	 clustLeave = 0;

	if(!buf || ! dwReaded )	return DR_INVALED_PARAM;		//��������
	*dwReaded = 0;						//������һ��

	if (!this->mFS) return DR_NO_OPEN;//�ļ�ʵ����ʧ��
	if(this->IsDir())	return DR_IS_DIR;	//�����ļ�

	//����ļ�������������
	res = this->FindNoNameDataAttr(&dataAttr);
	if (res != DR_OK)	return DR_OK;	//ϵͳ�����ļ�

	//�ļ���ʵ�ʴ�С
	realSize = this->GetRealSize();
	if (realSize.QuadPart == 0) return DR_OK;//ľ�����ݿ��Զ�ȡ
	if (realSize.QuadPart == this->mFilePointer.QuadPart) return DR_OK;

	//���Ҫ��ȡ����������ʵ��ʣ�µ����ݻ����׶�Ҫ��ȡ����
	if(dwToRead > realSize.QuadPart - mFilePointer.QuadPart)//Ҫ��ȡ�����ݳ����Ƿ����ʣ�µ�����
		dwToRead = (DWORD)(realSize.QuadPart - mFilePointer.QuadPart);//�ض�Ҫ��ȡ������

	//�ж��ļ��Ƿ�Ϊ��פ
	if (!dataAttr.IsNonResident())
	{//פ������ �������
		BYTE* dataPtr = dataAttr.R_GetAttrBodyPtr();
		dataPtr += DWORD(this->mFilePointer.QuadPart);
		memcpy(buf , dataPtr , dwToRead);
		this->mFilePointer.QuadPart += dwToRead;
		*dwReaded = dwToRead;
		return DR_OK;
	}

	//��פ�������д������

	curPtr		= this->mFilePointer;			//�ļ��ڵĵ�ǰ��дָ��
	SecPerClust = this->mFS->GetSecPerClust();	//ÿ��������
	dwClustSize = SecPerClust  * SECTOR_SIZE;	//ÿ���ֽ���
	dataBuf.resize(dwClustSize, 0);				//һ����Ϊ���ɿռ�
	dBufoff		= 0;							//dataBuf��д���ݵ�ƫ��
	clustOff	= DWORD(curPtr.QuadPart % dwClustSize);//��ȡ���Ĵ��е���Ч����ƫ��(FP)
	toReadThisTime = 0;							//���ζ�������Ч����

	//���һ����Ч����ָ��(FEP)
	clustLeave = DWORD(dwClustSize - (curPtr.QuadPart + dwToRead)%dwClustSize);
	if (clustLeave == dwClustSize)  //�պ���һ���غ���û��ʲô����
		clustLeave = 0;

	//��ǰ��д������غ�
	vcn.QuadPart =  curPtr.QuadPart / dwClustSize;

	while(dwToRead > 0)
	{
		//�������غŶ�Ӧ���߼��غ�
		lcn = this->GetLCNByVCN(vcn , &clustCnt);
		if (lcn.QuadPart == -1) {//��Ȼ������
			return DR_INIT_ERR;
		}

		for (; clustCnt.QuadPart && dwToRead > 0;
			--clustCnt.QuadPart , ++vcn.QuadPart )
		{
			if (lcn.QuadPart == -2)
			{//ϡ���ļ�
				memset(dataBuf.data(), 0, dwClustSize);
			}else{
				secOff.QuadPart = lcn.QuadPart * SecPerClust;
				res = this->mFS->ReadData(dataBuf.data(), &secOff , dwClustSize);
				if (res != DR_OK)
				{//�����鷳����
					return DR_INIT_ERR;
				}
				++lcn.QuadPart;
			}
			//���ƶ�ȡ������Ч����
			if (dwToRead >= dwClustSize)//���ζ�ȡ��������һ��
				toReadThisTime = dwClustSize - clustOff;
			else//���ζ�ȡ�����ݲ���һ��
				toReadThisTime = dwClustSize - clustOff - clustLeave;
			
			//���Ƶ���Ч������
			memcpy(buf + dBufoff , dataBuf.data() + clustOff , toReadThisTime );
			dwToRead -=toReadThisTime;
			dBufoff +=toReadThisTime;
			*dwReaded += toReadThisTime;
			curPtr.QuadPart += toReadThisTime;
			clustOff = 0;	//���ǻ������ƫ�ƣ���Ϊ���Դ�Ϊһ����ȡ��Ԫ��
							//����ֻ�Ե�һ�ζ�ȡ��Ч�������һ�κ�����ټ�
							//����ȡ�Ļ��ļ�ָ��ʽ�Ѿ����뵽�ر߽��ˣ�����
							//���ֶξͱ�������Ϊ0�������ȡ����ʱ�ͻ������
							//�⡣2012-07-04 08:42
		}
	}
	//�ļ��ĵ�ǰ��дָ�� 
	this->mFilePointer = curPtr;
	return DR_OK;
}
BOOL DNtfsFile::IsEOF()
{
	return (this->mFilePointer.QuadPart >= this->GetRealSize().QuadPart);
}
DRES DNtfsFile::SetFilePointer(LONG_INT off , BYTE dwMoveMethod)
{  
	LONG_INT realSize = this->GetRealSize();
	LONG_INT temp ;

	//�豸������û�д�
	if (NULL == this->mFS) return DR_NO_OPEN;

	if (dwMoveMethod == FILE_POS_END)
	{//���ļ�Ϊ��ʼ
		temp.QuadPart = realSize.QuadPart - off.QuadPart;
		if (temp.QuadPart < 0 )
		{//��Ч���ƶ�
			return DR_INVALED_PARAM;
		}else{
			this->mFilePointer = temp;
			return DR_OK;
		}
	}else if(dwMoveMethod == FILE_POS_BEGIN){
		
		if (off.QuadPart < 0 || off.QuadPart >= realSize.QuadPart)
		{//��Ч���ƶ�
			return DR_INVALED_PARAM;
		}else{
			this->mFilePointer = off;
			return DR_OK;
		}
	}else if(dwMoveMethod == FILE_POS_CURRENT){
		temp.QuadPart = this->mFilePointer.QuadPart + off.QuadPart;
		if (temp.QuadPart < 0 || temp.QuadPart > realSize.QuadPart)
		{//��Ч���ƶ�
			return DR_INVALED_PARAM;
		}else{
			this->mFilePointer = temp;
			return DR_OK;
		}
	}else
		return DR_INVALED_PARAM;
}
LONG_INT DNtfsFile::GetRealSize()
{
	DNtfsAttr attr;
	LONG_INT off;
	off.QuadPart = 0;

	//��ȡ������������
	if (DR_OK != this->FindNoNameDataAttr(&attr))
		return off;  //ľ��������������
	else
	{
		if(attr.IsNonResident())
		{////�Ƿǳ�פ����
			return PNON_RESID_ATTR_HEAD(attr.GetAttrHeadPtr())->ATTR_ValidSize;
		}else{//��פ����,
			off.QuadPart = PRESID_ATTR_HEAD(attr.GetAttrHeadPtr())->ATTR_DatSz;
			return off;
		}
	}


//���ļ����л�ȡ�ļ��Ĵ�С��Ϣ
// 	PAttrItem pAttr = this->FindAttribute(AD_FILE_NAME);
// 	if (NULL == pAttr)
// 		return off;  //û���ļ�������
// 	else
// 	{
// 		attr.InitAttr(pAttr->attrDataPtr);
// 		return attr.FNGetRealSize();
// 	}

}
DRES DNtfsFile::FindAttribute(DWORD dwAttrType , VOID* att , DWORD* startIdx)
{
	//��һ���������ļ���DRES NtfsFile::InitAttrList( BYTE* attrBuf )
	//AD(this->mRecodeBuf)->FR_1stAttrOff;
	//DWORD dwLen = MFT_RECODE_SIZE - dwOff;//���Լ��ϵ�������
	DRES  res   = DR_NO;
	DWORD i = 0;
	PAttrItem pai = mAttrArr.get();
	DNtfsAttr* attr = (DNtfsAttr*)att;

	if (startIdx)
	{
		if (*startIdx >= this->mAttrCnt)
			return DR_INVALED_PARAM;
		else
			i = *startIdx;
	}

	//�������Ա�
	for (; i < this->mAttrCnt /*&& pai[i].off*/ ; ++i)
	{
		if (pai[i].attrType == dwAttrType)
		{
			//attr->InitAttr(this->mRecodeBuf + pai[i].off/* , &dwLen*/);
			attr->InitAttr(pai[i].attrDataBuf.data());
			//���ص�ǰ������
			
			if (startIdx) *startIdx = i;
			return DR_OK;
		}
	}
	return res;
}
DRES DNtfsFile::FindNoNameDataAttr( DNtfsAttr* attr )
{
	DRES		res = DR_NO;
	DWORD		i	= 0;
	PAttrItem	pai = mAttrArr.get();

	if (NULL == this->mFS) return DR_NO_OPEN;

	if (NULL == attr) return DR_INVALED_PARAM;

	//�������Ա�
	for (; i < this->mAttrCnt /*&& pai[i].off*/ ; ++i)
	{
		if (pai[i].attrType == AD_DATA )
		{
			//attr->InitAttr(this->mRecodeBuf + pai[i].off/* , &dwLen*/);	
			attr->InitAttr(pai[i].attrDataBuf.data());
			if (attr->GetNameLen()) 
				continue; //�Բ�����Ҫ����������������
			//���ص�ǰ������
			return DR_OK;
		}
	}
	return res;
}
DRES DNtfsFile::InitAttrList(BYTE* attrBuf)
{
	//��һ���������ļ���¼�е�ƫ��
	PFILE_RECODE_HEAD	prh	   = PFILE_RECODE_HEAD(attrBuf);
	DWORD				dwOff  = prh->FR_1stAttrOff;
	int					i      = 0;
	int					attrLen= 0;
	PAttrItem			pai    = NULL;
	DNtfsAttr			attr;

	//�ȷ��任�ӿռ�
	this->mAttrCnt = prh->FR_NxtAttrId;				//��������
	mAttrArr.reset(new AttrItem[prh->FR_NxtAttrId]);
	pai = mAttrArr.get();

	//����ÿһ������
	while(GetDWORD(attrBuf + dwOff) != 0xFFFFFFFF){
		attr.InitAttr(attrBuf + dwOff/* , &dwLen*/);
		attrLen = attr.GetAllLen();			//��ǰ���Եĳ���
		
		//��ǰ������MFT�е�ƫ��
		pai[i].mftIndex = this->mMftIdx;		//��Ҫ�ǵõ��� ���ҵ�ATTRBUTE_LIST����Ҫ��������ĸ�ֵ���Ա�֤������ȷ
		pai[i].off		= (WORD)dwOff;			//��ǰ�����ڵ�ǰMFT�е�ƫ��
		pai[i].attrType = attr.GetAttrType();	//��������
		pai[i].id		= attr.GetAttrID();		//����id
		pai[i].attrDataBuf.resize(attrLen, 0);	//�������ݻ�����
		memcpy(pai[i].attrDataBuf.data(), attrBuf + dwOff , attrLen);
		//pai[i++].off = (WORD)dwOff;				//�����ڻ����е�ƫ��
		dwOff += attrLen;				//��һ�����Ե�ƫ��
		++i;

#ifdef _DEBUG  
		//��Ϊ�һ�û���ҵĵ������ҵ�ATTRIBUTE_LIST�������޷��о�������Եľ������
		//�����������̸һ�´�
		if (pai[i].attrType == AD_ATTRIBUTE_LIST)
		{
			MessageBoxA(NULL ,"�ҵ���ATTRIBUTE_LIST����" , "�ҵ���ATTR_LIST" , MB_OK);
		}
#endif
	}
	//���Ե�����
	this->mAttrCnt = i;

	return DR_OK;
}

// DRES DNtfsFile::InitRunList(VOID* att)
// {
// 	NtfsAttr*	attr	= (NtfsAttr*)att;
// 	LONG_INT	start	= {0};
// 	DWORD		runOff	= 0;		//�����е�����ƫ��
// 	DWORD		temp	= 0;
// 	PRunHead	runHead = NULL;
// 	LONG_INT	lcn		= {0};
// 	BYTE*		run		= NULL;
// 	PRunList	runListPtr = NULL;
// 	LONG_INT	dataBuf = {0};
// 	int			i		= 0;
// 
// 	//����
// 	if (!attr) return DR_INVALED_PARAM;
// 	//�Ѿ���ʼ������
// 	if (this->mRunList)		return DR_OK;
// 	
// 	//ֻ�зǳ�פ���Բ��������б�
// 	if (!attr->IsNonResident()) return DR_INIT_ERR;
// 
// 	//runlist�Ľڵ�����
// 	start = attr->NR_GetStartVCN();
// 	this->mRunCnt = (DWORD)(attr->NR_GetEndVCN().QuadPart - start.QuadPart + 1);
// 	this->mRunList = new RunList[this->mRunCnt];
// 
// 	run	   = attr->NR_GetDataPtr();//���е���ʵ���ݵ�ַ
// 
// /*	LONG_INT allCnt = start;*/
// 	runListPtr = PRunList(this->mRunList);
// 
// 	//�Ҳ�֪��Run�ĵڶ����͵������ֶεĵ����������ռ���ٸ��ֽ�
// 	//����ȥ8���ֽڣ���Ӧ��û�����, 
// 	for (i = 0 ;; ++i)
// 	{	
// 		runHead = PRunHead(run + runOff++);  //��ȡrun��ͷ��
// 		if (runHead->all == 0)
// 			break;			     //�������˵�ǰ���Ե���������
// 
// 		if (i == 0)//��ǰVCN
// 			runListPtr[i].vcn = start;
// 		else//��ǰvcn��ǰһ��run�ڵ��vcn+����
// 			runListPtr[i].vcn.QuadPart = 
// 			runListPtr[i-1].vcn.QuadPart + runListPtr[i-1].clustCnt.QuadPart;
// 		
// 		//����
// 		runListPtr[i].clustCnt.QuadPart = 0;//������һ��
// 		//��ȡ���ݴ���  length
// 		memcpy(&(runListPtr[i].clustCnt) , run + runOff , runHead->length);
// 		runOff += runHead->length;
// 
// 		if (runHead->offset == 0)   
// 		{//ϡ���ļ���Run  ����ʾ��ǰvcnû������
// 			runListPtr[i].lcn.QuadPart = 0;
// 			continue;;
// 		}
// 
// 		//���һ���ַ��Ļ����±�
// 		temp = runOff + runHead->offset -1;
// 
// 		//���lcn�����������һ������ �����������з�������ȡ��ȡ����
// 		dataBuf.QuadPart = (char)run[temp--];
// 		//һ�ζ�ȡʣ�µ��ֽ�
// 		for (; temp >= runOff; --temp)
// 			dataBuf.QuadPart = (dataBuf.QuadPart << 8) + GetBYTE(run + temp);
// 
// 		//ʵ�ʵ�lcn
// 		lcn.QuadPart += dataBuf.QuadPart;
// 	
// 		runListPtr[i].lcn = lcn;
// 
// 		//��һ��run��λ��
// 		runOff += runHead->offset;
// 	}
// 
// 	return DR_OK;
// }
// 

LONG_INT DNtfsFile::GetLCNByVCN(LONG_INT vcn , PLONG_INT clustCnt)
{
	DRES		res = DR_OK;
	LONG_INT	lcn = {0};
	DWORD		i	= 0;
	DNtfsAttr	attr;

	lcn.QuadPart = -1;	
	
	if (!m_upRun)
	{
		//��û�г�ʼ��Runlist
		m_upRun.reset(new DRun());

		if (this->IsDir())
		{//��Ŀ¼�Ļ�
			res = this->FindAttribute(AD_INDEX_ALLOCATION , &attr);
			if (res != DR_OK) return lcn;//û��ָ��������
			res = m_upRun->InitRunList(&attr);

			if (res != DR_OK) return lcn;//û��ָ��������
		}else{
			res = this->FindNoNameDataAttr(&attr);
			if (res != DR_OK) return lcn;//û��ָ��������
			if (!attr.IsNonResident()) return lcn;//���Ƿǳ�פ����  û��Runlist

			res = m_upRun->InitRunList(&attr);
			if (res != DR_OK) return lcn;//û��ָ��������
		}
	}

	//��������б�
	lcn = this->m_upRun->GetLCNByVCN(vcn, clustCnt);

	return lcn;
}

void DNtfsFile::Close()
{
	mAttrArr.reset();
	m_upRun.reset();
	mMftHeadBuf.clear();
	mFS = NULL;
	mMftIdx.QuadPart = 0;
}

LONG_INT DNtfsFile::GetMftStartSec()
{
	LONG_INT liSec = {-1};

	//��ǰ�ļ���û�д�
	if (this->mFS == NULL) return liSec;


	return this->mFS->GetSectorOfMFTRecode(this->mMftIdx);

	//�ȼ���һ��ƫ�ư�
// 	liSec = this->mFS->mCluForMFT;
// 	liSec.QuadPart *= mFS->mSecPerClu;	//MFT��ʵ������
// 	liSec.QuadPart += this->mMftIdx.QuadPart * RECODE_PER_SEC;
// 
// 	return  liSec;
}


LONG_INT DNtfsFile::GetFDTOffset()
{
	return mLIStartFDT;
}

WORD DNtfsFile::GetFDTLen()
{
	return mFDTLen;
}

