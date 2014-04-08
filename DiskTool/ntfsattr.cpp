/***********************************************************************
 * FileName:	ntfsattr.cpp
 * Author:		����
 * Created:		2012��4��20�� ������
 * Purpose:		����NtfsAttr�ľ���ʵ��
 * Comment:		������Ƕ�Ntfs�ļ��ĸ������Ե�һ������,�����Ҹо��������
 *				��һЩ��������
 ***********************************************************************/

#include "disktool.h"
#include "stut_connf.h"

#include "assert.h"

#define  STD_ATTR_HEAD_LEN 0x18

DNtfsAttr::DNtfsAttr() :mAttrBuf(NULL){}
DNtfsAttr::~DNtfsAttr(){}

DRES DNtfsAttr::InitAttr(void* src)
{
	int off = 0;  //��һ����src�еĶ�ȡ����
	//���й���
	if (NULL == src)  return DR_INVALED_PARAM;

	//�Ѿ����������б��ĩβ��
	if (GetDWORD(src) == 0xFFFFFFFF)    return DR_FAT_EOF;

	//�ȱ������ݰ�
	this->mAttrBuf = (BYTE*)src;

	return DR_OK;
}

DWORD DNtfsAttr::GetAttrType()
{
	return PATTR_HEAD(this->mAttrBuf)->ATTR_Type;
}
DWORD DNtfsAttr::GetAllLen()
{
	return PATTR_HEAD(this->mAttrBuf)->ATTR_Size;
}
BOOL DNtfsAttr::IsNonResident()
{
	return (PATTR_HEAD(this->mAttrBuf)->ATTR_NonResFlag == 1);
}
WORD DNtfsAttr::GetNameLen()
{
	return PATTR_HEAD(this->mAttrBuf)->ATTR_NamSz;
}

BOOL DNtfsAttr::IsCompressed()
{
	return (PATTR_HEAD(this->mAttrBuf)->ATTR_Flags & 0x0001);
}
BOOL DNtfsAttr::IsEncrypted()
{
	return (PATTR_HEAD(this->mAttrBuf)->ATTR_Flags & 0x4000);
}
BOOL DNtfsAttr::IsSparse()
{
	return (PATTR_HEAD(this->mAttrBuf)->ATTR_Flags & 0x8000);
}
WORD DNtfsAttr::GetAttrID()
{
	return PATTR_HEAD(this->mAttrBuf)->ATTR_Id;
}

WORD DNtfsAttr::R_GetStdHeadLen()
{
	return STD_ATTR_HEAD_LEN + PATTR_HEAD(this->mAttrBuf)->ATTR_NamSz * 2;//�������ߴ�(�ֽ�)
}

BYTE* DNtfsAttr::GetAttrHeadPtr()
{
	return this->mAttrBuf;
}
BYTE* DNtfsAttr::R_GetAttrBodyPtr()
{
	DWORD offset = STD_ATTR_HEAD_LEN + PATTR_HEAD(this->mAttrBuf)->ATTR_NamSz * 2;//�������ߴ�(�ֽ�)
	return (BYTE*)(this->mAttrBuf + offset);
}
DWORD DNtfsAttr::R_GetAttrLen()
{
	return PRESID_ATTR_HEAD(this->mAttrBuf)->ATTR_DatSz;
}
DWORD DNtfsAttr::R_GetAttrOff()
{
	return PRESID_ATTR_HEAD(this->mAttrBuf)->ATTR_DatOff;
}
LONG_INT DNtfsAttr::NR_GetStartVCN()  //��ñ������������Ŀ�ʼ����غ�
{
	return PNON_RESID_ATTR_HEAD(this->mAttrBuf)->ATTR_StartVCN;
}
LONG_INT DNtfsAttr::NR_GetEndVCN()	//��ñ������������Ľ�������غ�
{
	return PNON_RESID_ATTR_HEAD(this->mAttrBuf)->ATTR_EndVCN;
}
WORD	 DNtfsAttr::NR_GetDataOff()	//����������������������ͷ��ƫ�ƣ�����Ӧ�ð�˫�ֶ���
{
	return PNON_RESID_ATTR_HEAD(this->mAttrBuf)->ATTR_DataOff;
}
WORD     DNtfsAttr::NR_GetCmpSize()	//ѹ����Ԫ�ĳߴ硣ѹ����Ԫ�ĳߴ������2���������ݣ�Ϊ0��ʾδѹ��
{
	return PNON_RESID_ATTR_HEAD(this->mAttrBuf)->ATTR_CmpSize;
}
LONG_INT DNtfsAttr::NR_GetAllocSize() //���Լ�¼���ݿ����Ŀռ�ĳߴ磬�óߴ簴�سߴ����
{
	return PNON_RESID_ATTR_HEAD(this->mAttrBuf)->ATTR_AllocSize;
}
LONG_INT DNtfsAttr::NR_GetValidSize() //���Լ�¼���ݿ��ʵ�ʳߴ�
{
	return PNON_RESID_ATTR_HEAD(this->mAttrBuf)->ATTR_ValidSize;
}
LONG_INT DNtfsAttr::NR_GetInitedSize()//���Լ�¼���ݿ��Ѿ���ʼ�����ݵĳߴ磬��ĿǰΪֹ��ֵ�������Լ�¼���ݿ����ĳߴ���ͬ
{
	return PNON_RESID_ATTR_HEAD(this->mAttrBuf)->ATTR_InitedSize;
}
BYTE*	 DNtfsAttr::NR_GetDataPtr()
{
	return mAttrBuf + PNON_RESID_ATTR_HEAD(this->mAttrBuf)->ATTR_DataOff;//���е���ʵ���ݵ�ַ
}

DWORD DNtfsAttr::NR_GetStdHeadLen()
{
	return PNON_RESID_ATTR_HEAD(this->mAttrBuf)->ATTR_DataOff;//���е���ʵ���ݵ�ַ
}

DRES DNtfsAttr::GetAttrName(WCHAR* buf , int len0)
{
	int len = this->GetNameLen();
	if (0 == len){  //û���ļ���
		buf[0] = 0;
		return DR_NO;
	}
	if (len0 < len + 1)
	{//���治��
		buf[0] = 0;
		return DR_BUF_OVER;
	}

	memcpy(buf , (BYTE*)this->mAttrBuf +   PATTR_HEAD(this->mAttrBuf)->ATTR_NamOff, len *2);
	buf[len] = NULL;

	return DR_OK;
}

DWORD DNtfsAttr::FNGetFileNameLen()
{
	DWORD		offset =0x18 + PATTR_HEAD(this->mAttrBuf)->ATTR_NamSz * 2;//�������ߴ�(�ֽ�)
	PFILE_NAME  pfn = PFILE_NAME(this->mAttrBuf + offset);
	
	return pfn->FN_NameSize;
}
DRES  DNtfsAttr::FNGetFileName(WCHAR* buf)
{
	DWORD offset = PATTR_HEAD(this->mAttrBuf)->ATTR_NamSz * 2;//�������ߴ�(�ֽ�)
	//�ļ�����������ƫ��
	offset += ( 0x18 + 0x42); //������������ͷ���ļ�������ƫ��

	//�ļ�������
	DWORD len = FNGetFileNameLen();
	
	//�����ļ�������
	memcpy(buf ,this->mAttrBuf  +  offset , len*2 );

	return DR_OK;
}
BYTE DNtfsAttr::FNGetFileNameSpase()
{
	DWORD offset = PATTR_HEAD(this->mAttrBuf)->ATTR_NamSz * 2;//�������ߴ�(�ֽ�)
	//�ļ�����������ƫ��
	offset += 0x18; //������������ͷ���ļ�������ƫ��

	PFILE_NAME pfn = PFILE_NAME(mAttrBuf + offset);

	return pfn->FN_NamSpace;
}

LONG_INT DNtfsAttr::FNGetParentMftIndx()
{
	DWORD offset = PATTR_HEAD(this->mAttrBuf)->ATTR_NamSz * 2;//�������ߴ�(�ֽ�)
	//�ļ�����������ƫ��
	offset += 0x18; //������������ͷ���ļ�������ƫ��

	PFILE_NAME pfn = PFILE_NAME(mAttrBuf + offset);

	return pfn->FN_ParentFR;
}

DWORD DNtfsAttr::FNGetFlags()
{
	DWORD offset = PATTR_HEAD(this->mAttrBuf)->ATTR_NamSz * 2;//�������ߴ�(�ֽ�)
	//�ļ�����������ƫ��
	offset += 0x18; //������������ͷ���ļ�������ƫ��

	PFILE_NAME pfn = PFILE_NAME(mAttrBuf + offset);

	return pfn->FN_DOSAttr;
}

LONG_INT DNtfsAttr::FNGetRealSize()
{
	DWORD offset = PATTR_HEAD(this->mAttrBuf)->ATTR_NamSz * 2;//�������ߴ�(�ֽ�)
	//�ļ�����������ƫ��
	offset += 0x18; //������������ͷ���ļ�������ƫ��

	PFILE_NAME pfn = PFILE_NAME(mAttrBuf + offset);

	return pfn->FN_ValidSize;
}

LONG_INT DNtfsAttr::FNGetAllocateSize()
{
	DWORD offset = PATTR_HEAD(this->mAttrBuf)->ATTR_NamSz * 2;//�������ߴ�(�ֽ�)
	//�ļ�����������ƫ��
	offset += 0x18; //������������ͷ���ļ�������ƫ��

	PFILE_NAME pfn = PFILE_NAME(mAttrBuf + offset);

	return pfn->FN_AllocSize;
}

DWORD DNtfsAttr::IRGetAttrAttrType()
{
	DWORD offset =0x18 + PATTR_HEAD(this->mAttrBuf)->ATTR_NamSz * 2;//�������ߴ�(�ֽ�)

	PINDEX_ROOT pr = PINDEX_ROOT(mAttrBuf + offset);

	return pr->IR_AttrType;
}
DWORD DNtfsAttr::IRGetIndexBlockSize()
{
	DWORD offset =0x18 + PATTR_HEAD(this->mAttrBuf)->ATTR_NamSz * 2;//�������ߴ�(�ֽ�)

	PINDEX_ROOT pr = PINDEX_ROOT(mAttrBuf + offset);

	return pr->IR_EntrySz;
}
BOOL DNtfsAttr::IRIsLargeIndex()
{
	DWORD offset = PATTR_HEAD(this->mAttrBuf)->ATTR_NamSz * 2;//�������ߴ�(�ֽ�)
	//�ļ�����������ƫ��
	offset += ( 0x18 ); //������������ͷ������������ƫ��

	PINDEX_ROOT pr = PINDEX_ROOT(mAttrBuf + offset);

	return (pr->IR_IdxHead.IH_Flags == 0x01);
}
DWORD DNtfsAttr::IRGetIndexEntriesSize()
{
	DWORD offset = PATTR_HEAD(this->mAttrBuf)->ATTR_NamSz * 2;//�������ߴ�(�ֽ�)
	//�ļ�����������ƫ��
	offset += ( 0x18 ); //������������ͷ������������ƫ��

	PINDEX_ROOT pr = PINDEX_ROOT(mAttrBuf + offset);

	return pr->IR_IdxHead.IH_TalSzOfEntries;
}
DWORD DNtfsAttr::IRGetAlloIndexEntriesSize()
{
	DWORD offset = PATTR_HEAD(this->mAttrBuf)->ATTR_NamSz * 2;//�������ߴ�(�ֽ�)
	//�ļ�����������ƫ��
	offset += ( 0x18 ); //������������ͷ������������ƫ��

	PINDEX_ROOT pr = PINDEX_ROOT(mAttrBuf + offset);

	return pr->IR_IdxHead.IH_AllocSize;
}
BYTE* DNtfsAttr::IRGetFistEntry()
{
	DWORD offset = PATTR_HEAD(this->mAttrBuf)->ATTR_NamSz * 2;//�������ߴ�(�ֽ�)
	//�ļ�����������ƫ��
	offset +=  0x18 ;//��λ�ƶ����ֶε�Ƭλ��(��������Ե���ʵλ��)

	PINDEX_ROOT pr = PINDEX_ROOT(mAttrBuf + offset);

	return (BYTE*)&(pr->IR_IdxHead) + pr->IR_IdxHead.IH_EntryOff;
}

LONG_INT DNtfsAttr::IAGetLCNByVCN(LONG_INT* vcn , PLONG_INT ClustCnt)
{
	//�߼��غ�
	LONG_INT lcn = {0};
	LONG_INT start = this->NR_GetStartVCN();//��ʵ����غ�
	LONG_INT end   = this->NR_GetEndVCN();  //��������غ�
	BYTE*	 run	   = this->mAttrBuf + this->NR_GetDataOff();//���е���ʵ���ݵ�ַ
	DWORD	 runOff   = 0;		//�����е�����ƫ��
	DWORD	 temp = 0;
	PRunHead runHead = NULL;

	//�Ҳ�֪��Run�ĵڶ����͵������ֶεĵ����������ռ���ٸ��ֽ�
	//����ȥ8���ֽڣ���Ӧ��û�����, 
	LONG_INT dataBuf = {0};

	//Խ����  ����0  
	if (vcn->QuadPart > end.QuadPart || vcn->QuadPart < start.QuadPart){
		dataBuf.QuadPart = 0;
		return dataBuf;
	}
	//ѭ������
	int i = (int)(vcn->QuadPart - start.QuadPart) + 1;
	for ( ; i != 0 ; --i )
	{	
		runHead = PRunHead(run + runOff++);  //��ȡrun��ͷ��
		if (runHead->all == 0)
			break;			     //�������˵�ǰ���Ե���������
		
		if (ClustCnt)	{//��Ҫ��ȡ ����
			ClustCnt->QuadPart = 0;//������һ��
			//��ȡ���ݴ���  length
			memcpy(ClustCnt , run + runOff , runHead->length);
		}
		runOff += runHead->length;

		//���һ���ַ��Ļ����±�
		temp = runOff + runHead->offset -1;

		//����lcn���������һ������ �����������з�������ȡ��ȡ����
		dataBuf.QuadPart = (char)run[temp--];
		//һ�ζ�ȡmeiyi8geʣ�µ��ֽ�
		for (; temp >= runOff; --temp)
			dataBuf.QuadPart = (dataBuf.QuadPart << 8) + GetBYTE(run + temp);
	
		//ʵ�ʵ�lcn
		lcn.QuadPart += dataBuf.QuadPart;
		//��һ��run��λ��
		runOff += runHead->offset;
	}

	return lcn;
}

BOOL DNtfsAttr::BMIsBitSet(LONG_INT bit , DNtfs* fs)
{
	BYTE	bt = 0;
	DWORD	byteCnt = 0;

	//Ҫ�жϵ�ǰ�����Ƿ��ǳ�פ��
	if (this->IsNonResident())
	{//�ǳ�פ��
		DRun run;				//��ȡ�����б�
		if(DR_OK != run.InitRunList(this))
			return FALSE;

		LONG_INT	vcn;
		DWORD		secNum = 0;//���е�����ƫ��
		DWORD		byteNum	= 0;//�����е��ֽ�ƫ��
		char		buf[SECTOR_SIZE] = {0};//��������
		DRES		res = DR_OK;
		
		//����ָ����λ���ڵ�vcn
		vcn.QuadPart = ((bit.QuadPart / 8)/SECTOR_SIZE)/fs->GetSecPerClust();

		//��ѯLCV
		vcn = run.GetLCNByVCN(vcn , NULL);
		run.Close();//�ã����ʹ���Ѿ������
		
		if ( -1 == vcn.QuadPart )
			return FALSE;  //��ȡlcnʧ��
		
		//��������LCN�е�������
		secNum = ((bit.QuadPart / 8) / SECTOR_SIZE) % fs->GetSecPerClust();

		//�����������ֽ�ƫ��
		byteNum = (bit.QuadPart / 8) % SECTOR_SIZE;

		//����λƫ��
		byteCnt = bit.QuadPart % 8;

		//��ȡ����
		vcn.QuadPart *= fs->GetSecPerClust();
		vcn.QuadPart += secNum;
		res = fs->ReadData(buf , &vcn , SECTOR_SIZE , TRUE );
		if (DR_OK !=  res) return FALSE;

		//ȡ��ָ�����ֽ�
		bt = GetBYTE(buf + byteNum);

	}else{//��פ��
		DWORD offset = PATTR_HEAD(this->mAttrBuf)->ATTR_NamSz * 2;//�������ߴ�(�ֽ�)
		//�ļ�����������ƫ��
		offset +=  0x18 ;//��λ�ƶ����ֶε�Ƭλ��(��������Ե���ʵλ��)

		//�����ֽ���ƫ��
		byteCnt = DWORD(bit.QuadPart / 8 );

		//���ָ��Ϊ���ڵ��ֽ�
		bt = GetBYTE(mAttrBuf + offset + byteCnt);
		//λƫ��
		byteCnt = bit.QuadPart % 8;
	}

	bt = bt >> byteCnt;
	bt = bt << 7;
	return (bt != 0);
}


DWORD DNtfsAttr::SIGetFlags()
{
	DWORD		offset = 0x18 + PATTR_HEAD(this->mAttrBuf)->ATTR_NamSz * 2;//�������ߴ�(�ֽ�)
	PSTD_INFO  psi = PSTD_INFO(this->mAttrBuf + offset);

	return psi->SI_DOSAttr;
}
