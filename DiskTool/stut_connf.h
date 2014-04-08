/***********************************************************************
 * FileName:	stut_connf.h
 * Author:		����
 * Created:		2012��3��20�� ���ڶ�
 * Purpose:		����Ҫ������������ݽṹ��
 * Comment:		���ļ���Ҫ�Ƕ���һЩ��Ҫʹ�õ�Ԥ������Լ�һЩ���̲�����
 *				�صķǵ������ݽṹ�����
 ***********************************************************************/

#ifndef DISK_INFO_STRUCTS
#define DISK_INFO_STRUCTS

//����ļ���ƴ����ǲ���Ҫ�����ģ����ǻ��Ƕ���һ�±ȽϺ�
#pragma pack(push , 8)



#include "string.h"
#include "stdio.h"
#include "time.h"


//��Ҫ��windows�Ĵ���������Ҫʹ��һЩwindows�µ�api

#ifdef _MSC_VER  //��vc������

#include "windows.h"
#include "Winioctl.h"

#if _MSC_VER <= 1200	//vc6.0һ��һ��û�ж���һ�µ�һЩ���� 

#ifndef IOCTL_DISK_GET_LENGTH_INFO  //���ڲ����豸��С����ض���
#define IOCTL_DISK_GET_LENGTH_INFO 0x7405C
#endif

#ifndef IOCTL_VOLUME_GET_VOLUME_DISK_EXTENTS     //���һ���߼������������豸�ϵ�λ����Ϣ
#define IOCTL_VOLUME_GET_VOLUME_DISK_EXTENTS 0x560000
#endif // IOCTL_VOLUME_GET_VOLUME_DISK_EXTENTS

#ifndef GET_LENGTH_INFORMATION		//�����豸��ʵ�ʴ�С
typedef struct _GET_LENGTH_INFORMATION {
	LARGE_INTEGER   Length;
} GET_LENGTH_INFORMATION, *PGET_LENGTH_INFORMATION;
#endif

#ifndef _DISK_EXTENT
typedef struct _DISK_EXTENT {
    DWORD           DiskNumber;
    LARGE_INTEGER   StartingOffset;
    LARGE_INTEGER   ExtentLength;
} DISK_EXTENT, *PDISK_EXTENT;
#endif

#ifndef _VOLUME_DISK_EXTENTS  //��ѯһ��������������ϵ�λ��
typedef struct _VOLUME_DISK_EXTENTS {
    DWORD       NumberOfDiskExtents;
    DISK_EXTENT Extents[1];
} VOLUME_DISK_EXTENTS, *PVOLUME_DISK_EXTENTS;
#endif


#endif
#endif

//�ַ��������ؿ�
#include "../StrTool/StrTool.h"
#ifdef _DEBUG
#pragma comment(lib,"../StrTool/Debug/StrTool.lib")
#else
#pragma comment(lib,"../StrTool/Release/StrTool.lib")
#endif


/*/////////////////////////////////////////////////////////////////////////
	�ر�һЩwindows���濪��
/////////////////////////////////////////////////////////////////////////*/
#pragma warning(disable:4996)        //sprintf


//���浱ǰ����״̬ Ȼ������һ�¶���Ϊ 1�ֽ�
#pragma pack(push , 1)

/*****************************************************************************************
	���̷�����ṹ DPT
	00H--00H   0x1�ֽ�  ������־ 
						80H���������
						00H����ʾ�ǻ����
	01H--01H   0x1�ֽ�  ����������ʼ��ͷ��
	02H--03H   0x2�ֽ�  ��6λ ��������ʼ������  ��8λ��������ʼ�����
	04H--04H   0x1�ֽ�  �������ͷ���
						00H������ʾ�÷���δ�ã���û��ָ������
						01H����DFAT32
						04H����FAT16
						06H����FAT16����������
						0BH����DFAT32����������
						0CH����DFAT32
						05H������չ������
						07H����NTFS����  ����HPFS��
						0FH����LBAģʽ ��չ����
						63H����Unix   ����  
						83H����Linux  Native
						82H����Linux  ������
	05H--05H   0x1�ֽ�  �������Ľ�����ͷ��
	06H--07H   0x2�ֽ�  ��6λ ����������������  ��8λ���������������
	08H--0AH   0x4�ֽ�  ���������������ָ��������ڼ�¼�÷����ķ����������λ��֮��
	0BH--0EH   0x4�ֽ�  ������������
	��0x10(16)�ֽ�
******************************************************************************************/
typedef struct _tagDPT{
	UCHAR  mGuidFlag;          //�������
	UCHAR  mStartHead;         //��������ʼ��ͷ
	USHORT mStartSector:6;     //��������ʼ������
	USHORT mStartCylinder:8;   //��������ʼ����
	UCHAR  mPartType;		   //��������
	UCHAR  mEndHead;           //������������ͷ
	USHORT mEndSector:6;       //����������������
	USHORT mEndCylinder:8;     //��������������
	DWORD  mRelativeSectors;   //�������������ʵ������
	DWORD  mSectorCount;       //������������
}DPT , *PDPT;


/*****************************************************************************************
	���̵���������¼
	0000H--0088H  0x88�ֽ�  ���������� ����ӻ������װ�أ�������ϵͳ��������
	0089H--00E1H  0x58�ֽ�  ������Ϣ������
	00E2H--01BDH  0xF9�ֽ�  �����ֶ�  һ��ȫΪ��
	
	���� 0x1EB�ֽ���ʱ������

	001BE--001CD   ������ 1
	001CE--001DD   ������ 2
	001DE--001ED   ������ 3
	001EE--001FD   ������ 4
	001FE--001FF   �������  55 AA
		
	�� 0x20 (512)�ֽ�
******************************************************************************************/
typedef struct _tagMBR{
	UCHAR   mGuideCode[0x1BE];         //������������
	////�ĸ��������¼
	DPT     mDPT[4];
	USHORT  mEndSig;                  //�������   55 AA
}MBR , *PMBR;

//�߼�����ǰ�Ĵ��̷�����
typedef MBR   EBR;
typedef PMBR  PEBR;

//////////////////////////////////////////////////////////////////////////
//��չ������
//////////////////////////////////////////////////////////////////////////
typedef struct _tagExtDPT{
	char	partName[8];	//��������
	BYTE	noUse1;			//û��ʹ��
	BYTE	partNum;		//�������
	BYTE	ctrlNum;		//�����߼�����
	BYTE	noUse2;			//û��ʹ��
	DWORD	startSector;	//��ʵ����������
	DWORD	sectorCount;	//��������
	DWORD	emptySecCnt;	//����������
	WORD	noUse3;			//û��ʹ��
	BYTE	partFormate;	//��������
	BYTE	noUse4[5];		//û��ʹ�õ�
}ExtDPT , *PExtDPT;
//�ж���չ�ֱ���Ƿ�Ϊ��
#define IsExtPDPTEmpty(x) ((((PLONG_INT)(x))[0].QuadPart == 0) &&	\
					       (((PLONG_INT)(x))[1].QuadPart == 0) &&	\
						   (((PLONG_INT)(x))[2].QuadPart == 0) &&	\
						   (((PLONG_INT)(x))[3].QuadPart == 0))

//��ڵ�״̬
#define ST_LAST		0x1		//00000001	�Ѿ�ƥ�������һ������
#define ST_LOWER	0x2		//00000010	��Сд
#define ST_UPPER	0x4		//00000100	�д�д
#define ST_LFN		0x8		//00001000	һ��Ҫ���ļ���		

//�ļ�״̬
#define  F_ST_EOF	0x1		//00000001	�Ѿ����ļ��Ľ�β



//�˽ṹ��DFAT32��DBR
typedef struct  _tagDFAT32_DBR
{
	UCHAR       bsJump[3];          //��תָ��  ��ת��dbr����������
	UCHAR       bsOemName[8];       //OEM����
	USHORT      bsBytesPerSec;      //�������ֽ���
	UCHAR       bsSecPerClus;       //ÿ�صĵ�������
	USHORT      bsResSectors;       //������������(��һ��fat֮ǰ��������)
	UCHAR       bsFATs;             //FAT��
	USHORT      bsRootDirEnts;      //����ڵ��м���
	USHORT      bsSectors;          //������  ����65535�� ʹ�������bsHugeSectors
	UCHAR       bsMedia;            //��������YSDISK_MEDIA_TYPE
	USHORT      bsFATsecs;          //ÿ��FATռ��������
	USHORT      bsSecPerTrack;      //ÿ���ŵ��������� 32
	USHORT      bsHeads;            //�ж��ٸ���ͷ Ҳ����ÿ������Ĵŵ���
	DWORD       bsHiddenSecs;       //����������
	DWORD       bsHugeSectors;      //һ������65535��������  ���ô��ֶ�����������	
	DWORD       bsFATsecs32;        //FAT32 fat����ռ������  bsFATsecs����Ϊ0
	USHORT      bsFlags32;          //FAT32 ���
	USHORT      bsVersion32;        //FAT32 ��λΪ���汾�ţ���λΪ�ΰ汾��
	DWORD       bsFirstDirEntry32;  //FAT32 ��Ŀ¼��һ��(һ��Ϊ2)
	USHORT      bsFsInfo32;         //FAT32 �������� DFAT32 �� FSINFO �ṹ���ڵ������ţ�������������ͨ��Ϊ1
	USHORT      bsBakDbr32;         //FAT32 �����Ϊ 0����ʾ�ڱ�������������¼�ı��������ڵ������ţ�ͨ��Ϊ 6��ͬʱ������ʹ�� 6 �����������ֵ
	UCHAR       bsRreserved[12];    //FAT32 ���� �����Ժ� FAT ��չʹ�ã��� DFAT32�������� 0 ���
	UCHAR       bsDriveNumber;      //���������
	UCHAR       bsReserved1;        //�����ֶ�
	UCHAR       bsBootSignature;    //������չ��������ǩ 0x29
	DWORD       bsVolumeID;         //���̾�ID  ��������Ϊ0x12345678
	UCHAR       bsLabel[11];        //���̾��YSDisk
	UCHAR       bsFSType[8];		//�ļ�ϵͳ���� - FAT12 or FAT16
	UCHAR       bsReserved2[420];   //Ҳ��һ�������ֶ�
	USHORT      bsEndSig;			//������ǩ - 0xAA55
}FAT32_DBR , *PFAT32_DBR;

/**********************************************************************************************/
//���ļ�������ڽṹ
typedef struct  _tagSHORT_DIR_ENTRY
{
	UCHAR       mName[8];          //�ļ���
	UCHAR       mExt[3];	       //�ļ���չ
	UCHAR       mAttr;		       //�ļ�����
	UCHAR       mNameCase;         //�ļ�������չ���Ĵ�Сд����
	UCHAR		mCrtTimeTenth;	   //����ʱ���ʮ��֮һ����
	USHORT		mCrtTime;		   //����ʱ��
	USHORT		mCrtDate;		   //��������
	USHORT		mLstAccDate;	   //���һ�η���ʱ��
	USHORT		mFstClusHI;		   //��һ�غŵĸ����ֽ�
	USHORT		mWrtTime;		   //�޸�ʱ��
	USHORT		mWrtDate;		   //�޸�����
	USHORT		mFstClusLO;		   //��һ�غŵĵ����ֽ�
	DWORD       mFileSize;         //�ļ�����
}SDE, *PSDE;

/**********************************************************************************************
	���ļ�������ڽṹ
***********************************************************************************************/
typedef struct  _tagLONG_DIR_ENTRY
{
	UCHAR       mOrder;			//���� 
	UCHAR       mName1[10];		//���ֵĵ�һ����
	UCHAR       mAttr;			//�ļ�����
	UCHAR       mNameCase;		//�ļ�������չ���Ĵ�Сд����
	UCHAR		mChksum;		//�ļ�����У���
	UCHAR		mName2[12];     //�ļ����ĵڶ�����
	USHORT      mFstClusLO;     //  0
	UCHAR       mName3[4];		//�ļ�����
}LDE, *PLDE;



// 16λ��FATʱ���
// 0-4λ����Чֵ��1-31����ʾ��
// 5-8λ����Чֵ��1-12����ʾ��
// 9-15λ����Чֵ0-127�������1980�����
typedef union _tagDataStamp {
	struct{
		USHORT day : 5;
		USHORT month : 4;
		USHORT year : 7;
	};
	USHORT date;
}DataStamp ,*PDataStamp;

// 16λ��FAT���ڴ�
// 0-4λ������Чֵ��0-29����ʾ������������ʾʵ�ʵ���Ϊ0-58
// 5-10λ����Чֵ��0-59����ʾ����
// 11-15λ����Чֵ��0-23����ʾСʱ
typedef union _tagTimeStamp {
	struct{
		USHORT sec : 5;
		USHORT min : 6;
		USHORT hour : 5;
	};
	USHORT time;
}TimeStamp , *PTimeStamp;





/************************************************************************/
/* һ����NTFSת�����ݽṹ                                               */
/************************************************************************/
//һ�¶�����NTFS��DBR�����ݽṹ
typedef struct _tagNTFS_DBR{
	BYTE dbrJump[3];		//�����ֽڵ���תָ��
	BYTE dbrOemName[8];		//OEM����
	
	//��������25���ֽڵ�BPB
	WORD  bpbBytePerSec;	//Bytes Per Sector
	BYTE  bpbSecPerClu;		//Sectors Per Cluster
	WORD  bpbResSec;		//����ɽ����
	BYTE  bpbNULL[3];		//always 0
	WORD  bpbNoUse;			//not used by NTFS
	BYTE  bpbMediaType;		//0xF8 Media Descriptor
	WORD  bpbNULL1;			//always 0
	WORD  bpbSecPerTrack;	//SectorPer Track
	WORD  bpbHeads;			//Number of heads
	DWORD bpbHidSec;		//Hidden Sector
	DWORD bpbNoUse2;		//not used by NTFS
	
	//��������48���ֽڵ���չBPB
	DWORD bpbNoUse3;		//not used by NTFS
	LONG_INT  bpbAllSec;	//Total Sectors
	LONG_INT  bpbCluForMFT;	//Logical Cluster Number for the file $MFT
	LONG_INT  bpbCluForMFTMirr;//Logical Cluster Number for the file $MFTMirr

	INT bpbCluPreRecod;		//Clusters Per File Record Segment
	INT bpbCluPerIndex;		//Clusters Per Index Block
	/************************************************************************/
	/* This can be negative, which means that the size of the MFT/Index record
	is smaller than a cluster. In this case the size of the MFT/Index record in
	bytes is equal to 2^(-1 * Clusters per MFT/Index record). So for example if
	Clusters per MFT Record is 0xF6 (-10 in decimal), the MFT record size is 
	2^(-1 * -10) = 2^10 = 1024 bytes.                                       */
	/************************************************************************/

	LONG_INT  bpbVolNum;	//Volume Serial Number
	DWORD bpbChecksum;		//Checksum	

	//��������426���ֽڵ���������
	BYTE  dbrCode[426];		//��������
	WORD  dbrEnd;			//������־

}NTFS_DBR , *PNTFS_DBR;


/************************************************************************/
/* ������¼ͷ��ÿ��IndexRoot��ÿ��IndexBlockHead�������һ�������Ľṹ��*/
/* ��Ҫ�ü�¼��ǰ����������Ϣ											*/
/************************************************************************/
typedef struct _tagINDEX_HEAD{
	DWORD IH_EntryOff;	//��һ�������Ŀ¼���ƫ�ƣ����������ͷ��
	DWORD IH_TalSzOfEntries;//������������ܳߴ�
	DWORD IH_AllocSize;	//������������ܷ���ߴ�
	BYTE  IH_Flags;		//��־λ
						//  0x00СĿ¼(Ŀ¼����IR��)
						//  0x01��Ŀ¼(��Ҫ�ⲿ�������λͼ)
	BYTE IH_Resvd[3];	//����  ���ڶ��뵽���ֽ�λ��
}INDEX_HEAD , *PINDEX_HEAD;
 
/************************************************************************/
/* ��������¼                                                           */
/************************************************************************/
typedef struct _tagINDEX_ROOT{
	DWORD IR_AttrType;	//���Ե�����
	DWORD IR_ColRule;	//�������
	DWORD IR_EntrySz;	//ÿ�������� �ߴ磨4k��
	BYTE  IR_ClusPerRec;//ÿ�������� ռ�õĴ���
	BYTE  IR_Resvd[3];	//����  ���ڶ��뵽4�ֽ�

	INDEX_HEAD IR_IdxHead;	//����ͷ��
}INDEX_ROOT , *PINDEX_ROOT;

/************************************************************************/
/* ��׼����ͷ����Ҳ��һ������ȫ���ݽṹ                                 */
/* ��indexblock������ļ���������8.3����Ļ����ͻ��������֪indexEntry��*/
/* ��ָ��ͬһ��MFT��¼,һ�����ڼ���DOS������һ�����岻֪*/
/************************************************************************/
typedef struct _tagINDEX_BLOCK_HEAD{
	BYTE	 IB_Sign[4];	//��¼ǩ����ֵΪ����INDX��
	WORD	 IB_USOff;		//�������кŵ�ƫ��(M) ��=0x28
	WORD	 IB_USNSz;		//�������кŸ���+1(N) ��=0x09(4K/512+1)
	LONG_INT IB_LSN;		//��־�ļ����кţ���ֵ�ڼ�¼ÿ�α��޸�ʱ���ᱻ�Ķ�
	LONG_INT IB_VCN;		//��ǰ ������ ��Ŀ¼�ļ� �е�����غ�

	INDEX_HEAD IB_indexHead;//������ͷ����Ϣ
	//WORD		us;			//Update sequence    //Ϊ�˱��ֶ��� ���ﲻ���Դ�ֵ
	//0x2A 2S-2 Update sequence array   
}INDEX_BLOCK_HEAD , *PINDEX_BLOCK_HEAD;



/************************************************************************/
/*�ļ���¼ͷ 0x30���ֽ�                                                 */
/************************************************************************/
typedef struct _tagFILE_RECODE_HEAD{
	BYTE  FR_Sign[4];	//��¼ǩ����ֵΪ����ELIF��
	WORD  FR_USOff;		//�������кŵ�ƫ��(M) (����ļ���¼ͷ)
	WORD  FR_USNSz;		//�������кŸ���+1(N)
	BYTE  FR_LSN[8];	//��־�ļ����кţ���ֵ�ڼ�¼ÿ�α��޸�ʱ���ᱻ�Ķ�
	WORD  FR_SN;		//�ظ�ʹ�� �������кţ�ɾ��һ�μ�1��
	WORD  FR_LnkCnt;	//Ŀ¼�м�¼���ļ������ü�������ֵֻ���ڻ����ļ���¼
	WORD  FR_1stAttrOff;//��һ���������ݵ�ƫ��
	WORD  FR_Flags;		//��־���ó�Ա���������¸�ֵ��֮һ
						//0x0001	��¼��ʹ��
						//0x0002	Ŀ¼�ļ�
	DWORD FR_Size;		//��ǰ��¼�ĳߴ�
	DWORD FR_AllocSz;	//��ǰ��¼����Ŀռ�ĳߴ�
	BYTE  FR_BaseFR[8];	//��ǰ�ļ���¼�Ļ����ļ���¼�������������ǰ�ļ���¼�ǻ�����
						//����¼���ֵΪ0������ָ������ļ���¼�ļ�¼������ע�⣺��ֵ
						//�ĵ�6�ֽ���MFT��¼�ţ���2�ֽ��Ǹ�MFT��¼�����к�
	WORD  FR_NxtAttrId;	//��һ�����Ե�ID�����������Ե�����  , ӦΪÿһ��������Զ������Ӵ�ֵ����ֵ�ܶ�����С����������
						//��һ�ν��ᱻ��ӵ��ļ���¼�����Ե�ID��ÿ�����ļ���¼���������ʱ��ֵ�������ӣ�ÿ���ļ���¼������ʹ��ʱ��ֵ���ᱻ���㣬��һ��ֵ�϶���0
	WORD  FR_Resvd;		//����(XP����,3.1+)
	DWORD FR_NumOfFR;	//��MFT ��¼�� (XP����,3.1+)
}FILE_RECODE_HEAD , *PFILE_RECODE_HEAD;


/************************************************************************/
/* ����һ������ȫ�Ľṹ�壬��Ϊ����Ĵ�ռ䳤�ȣ�����һ���ֶξ���       */
/************************************************************************/
typedef struct _tagFILE_NAME{
	LONG_INT FN_ParentFR;	//��Ŀ¼��MFT��¼�ļ�¼������
							//ע�⣺��ֵ�ĵ�6�ֽ���MFT��¼�ţ���2�ֽ��Ǹ�MFT��¼�����к�
	LONG_INT FN_CreatTime;	//�ļ�������ʱ��
	LONG_INT FN_AlterTime;	//�ļ����һ�α��޸ĵ�ʱ��
	LONG_INT FN_MFTChg;		//�ļ���MFT��¼���޸ĵ�ʱ��
	LONG_INT FN_ReadTime;	//���һ�η����ļ���ʱ��
	LONG_INT FN_AllocSize;	//�ļ�����ռ�õĿռ�ߴ磬��ֵ���سߴ����
	LONG_INT FN_ValidSize;	//�ļ�����ʵ�ߴ磬�����Ŀ¼�����ʾ���ļ��ߴ���Ǹ�ֵ������ļ������������еĿ�ʼ�غŲ�Ϊ0�����ֵΪ�գ���ʱ��Ҫ��ѯ�ļ��ߴ�Ļ��������SI����
	DWORD	 FN_DOSAttr;	//DOS�ļ����ԣ�
	DWORD	 FN_EA_Reparse;	//��չ����������
	BYTE	 FN_NameSize;	//�ļ������ַ���(L)
	BYTE 	 FN_NamSpace;	//�����ռ䣬��ֵ��Ϊ����ֵ�е�����һ��
							//0��POSIX������ʹ�ó�NULL�ͷָ�����/��֮�������UNICODE�ַ���������ʹ��255���ַ���ע�⣺�������ǺϷ��ַ�����Windows������ʹ�á�
							//1��Win32��Win32��POSIX��һ���Ӽ��������ִ�Сд������ʹ�ó�������������������?��������������/������<������>������\������|��֮�������UNICODE�ַ��������ֲ����ԡ�.����ո��β��
							//2��DOS��DOS�����ռ���Win32���Ӽ���ֻ֧��ASCII����ڿո��8BIT��д�ַ����Ҳ�֧�������ַ�������������������?��������������/������<������>������\������|������+������,������;������=����ͬʱ���ֱ��밴���¸�ʽ������1~8���ַ���Ȼ���ǡ�.����Ȼ������1~3���ַ���
							//3��Win32&DOS����������ռ���ζ��Win32��DOS�ļ����������ͬһ���ļ��������С�
	WCHAR	FN_FileName[1];	//2*L	����Ҫ��0��Ϊ�����ַ�
// 	//name�Ĳ��Ⱦ�����nameLen��������������һ������ȫ�ṹ��
}FILE_NAME ,*PFILE_NAME;

/************************************************************************/
/* ���ϵ�NTFS�汾�У�STARDAND_INFORMATION����ֻ���ڴ���ļ���ʱ���DOS��*/
/* ��������Ϣ����Windows 2000����������4��������������ȫ���ļ��ߴ��*/
/* ��־��Ϣ��															*/
/************************************************************************/
typedef struct _tagSTD_INFO{
//-	-	��׼����ͷ(24 �ֽ�)
	LONG_INT   SI_CreatTime;	//�ļ�����ʱ��
	LONG_INT   SI_AlterTime;	//�ļ����һ���޸�ʱ��
	LONG_INT   SI_MFTChgTime;	//�ļ���MFT��¼�޸ĵ�ʱ��
	LONG_INT   SI_ReadTime;		//���һ�η��ʵ�ʱ��
	DWORD	   SI_DOSAttr;		//DOS�ļ����ԣ���������ATTR_  ��ͷ������ntfs������ֵ���
	DWORD	   SI_MaxVer;		//�ļ����õ����汾�ţ���ֵΪ0��ʾ�汾���ܱ���ֹ(Ŀǰδʹ�ã�)
	DWORD	   SI_Ver;			//�ļ��汾�ţ�������汾��Ϊ0��ֵҲ����Ϊ0
	DWORD	   SI_ClassId;		//����(Ŀǰδʹ�ã�)
	DWORD      SI_OwnerId;		//�ļ�ӵ���ߵ�ID����ID��$Quota�ļ���$O��$Q��¼�еļ��������ֵΪ0���ʾ������ñ���ֹ
	DWORD      SI_SecurityId;	//��ȫID����ֵ��$Securce�ļ���$SII������$SDS�������ļ�
	DWORD	   SI_QuotaCharged;	//���ļ�����ʹ�õĿռ��������ֵΪ0���ʾ���������
	DWORD	   SI_USN;			//�ļ����һ�θ��µļ�¼�ţ���ֵ��$UsnJrnl�ļ���ֱ�������������ֵΪ0���ʾ������־����ֹ
}STD_INFO , *PSTD_INFO;

/************************************************************************/
/* ������,��Ϊ�������ṹ�壬Ҳ����˵��Ϊ�˽ṹ��ĳЩ����ʯ�䶯�ģ��޷�ȷ*/
/* �����С,����һ��ֻ������ָ�빦��                                    */
/************************************************************************/
#define  INDEX_ENTRY_NODE	0x01	//
#define  INDEX_ENTRY_END	0x02	//���������

typedef struct _tagINDEX_RNTRY{
	//�������ֻ�������ĵ�¼���û�б����õ�����²�����Ч��
	//Ҳ����˵flagû�б��INDEX_ENTRY_END
	LONG_INT IE_FR;		//�ļ����� 
	WORD IE_Size;		//��ǰ������ĳ��� 
	WORD IE_DataSize;	//���ĳ��� 
	WORD IE_Flags;		//��� 
	WORD notUse;		//���ڶ���
	//�������ֻ����û�������ӽڵ���ʱ�Ŵ��� 
	//Ҳ����˵flagû������INDEX_ENTRY_NODE
	BYTE IE_Stream[1];	//�����䶯�����ݶ�,���Ǳ�
						//ʾһ���ļ�����Ŀ¼�Ļ������ŵ�FILE_NAME���Խṹ
	
	//��flag������INDEX_ENTRY_NODEʱ�˽ṹ���������ֽڱ�ʾһ��vcn
	//����8�ֽ� �������������е��ӽڵ��VCN 
}INDEX_ENTRY , *PINDEX_ENTRY;


//����ͷ
typedef struct _tagATTR_HEAD{
	DWORD ATTR_Type;	//0x00	�������͡���ֵ������$AttrDef�ļ��ж���������б��е�һ��
	DWORD ATTR_Size;	//0x04	���Գߴ�(��������ͷ)
	BYTE  ATTR_NonResFlag;//0x08 �������ݷ�פ����־��ֵΪ1
	BYTE  ATTR_NamSz;	//0x09	�������ߴ磬��ֵΪ0ʱ��ʾ��ǰ����û������
	WORD  ATTR_NamOff;	//0x0a	���������������ͷ��ƫ�ƣ���ֵ���ǹ̶�Ϊ0x18
	WORD  ATTR_Flags;	//0x0c	���Ա�־���ڵ�ǰ�İ汾��ֻ֧������3����־��
						// 0x0001���Ա�ѹ��
						// 0x4000���Ա�����
						// 0x8000ϡ������
						// ע�⣺ֻ�з�פ��������(Data)���Բ��ܱ�ѹ��
	WORD  ATTR_Id;		//����ID����MFT��¼�е�ÿ�����Զ���һ��Ψһ��ID�����Ե������޹�
	//BYTE  remain[1];	//���ڱ�ʾ������������
}ATTR_HEAD , *PATTR_HEAD;


/************************************************************************/
/* ��פ����ͷ                                                           */
/************************************************************************/
typedef struct _tagRESID_ATTR_HEAD{//��פ����
	ATTR_HEAD stdHead;		//��ע����ͷ
	DWORD     ATTR_DatSz;	//�������ݳߴ�
	WORD	  ATTR_DatOff;	//�����������������ͷ��ƫ��
	BYTE	  ATTR_Indx;	//����������־(��FILE_NAME��Ч)
	BYTE	  ATTR_Resvd;   //����  ���ڶ��뵽8�ֽ�
	//ATTR_AttrNam	2*N	��������Unicode�ַ���������ַ��������ڽ�β����0
	//				������Ե�������Ҫ����ATTR_NamSz����,�������ﲻ����
	//ATTR_AttrDat	0x18+2*N		�������ݣ����Ե����ݶ�Ӧ�ð�˫�ֶ���
}RESID_ATTR_HEAD , *PRESID_ATTR_HEAD;

/************************************************************************/
/* �ǳ�פ����ͷ                                                         */
/************************************************************************/
typedef struct _tagNON_RESID_ATTR_HEAD{//�ǳ�פ����
	ATTR_HEAD stdHead;
	LONG_INT ATTR_StartVCN;	//����������������ʼ�Ĵغ�
	LONG_INT ATTR_EndVCN;	//�����������������һ�صĴغ�
	WORD     ATTR_DataOff;	//�������������������ͷ��ƫ�ƣ�����Ӧ�ð�˫�ֶ���
	WORD     ATTR_CmpSize;	//ѹ����Ԫ�ĳߴ硣ѹ����Ԫ�ĳߴ������2���������ݣ�Ϊ0��ʾδѹ��
	DWORD    ATTR_Resvd;	//���� ,���ڶ��뵽8�ֽ�
	LONG_INT ATTR_AllocSize;//���Լ�¼���ݿ����Ŀռ�ĳߴ磬�óߴ簴�سߴ����
	LONG_INT ATTR_ValidSize;//���Լ�¼���ݿ��ʵ�ʳߴ�
	LONG_INT ATTR_InitedSize;//���Լ�¼���ݿ��Ѿ���ʼ�����ݵĳߴ磬��ĿǰΪֹ��ֵ�������Լ�¼���ݿ����ĳߴ���ͬ
	//ATTR_AttrNam	0x40	2*N	��������UNICODE�ַ��������ַ�������0��β
	//ATTR_DataRuns	0x40+2*N		��������������(Ϊ���ǳ�פ�����߼�������ָ��,����ʵ����ָ������)
}NON_RESID_ATTR_HEAD , *PNON_RESID_ATTR_HEAD;


//�ָ�����״̬
#pragma pack(pop)

//���е�ͷ��  NTFS
typedef union _tagRunHead{
	struct{
		BYTE length:4;  //����
		BYTE offset:4;  //ƫ��
	};
	BYTE all;			//��������
}RunHead , *PRunHead;



//���ļ�ϵͳ���ļ���Ŀ¼��ڽṹ
typedef struct _tagDIR_ENTRY
{
	WORD	mIndex;		//��ǰ��д��Ŀ¼��ں� 
	DWORD	mStartClust;//��ʼ�غ�
	DWORD	mCurClust;	//��ǰ�غ�
	DWORD	mCurSect;	//��ǰ������
	BYTE*	mDir;		//ָ��FAT32::mView�Ķ��ļ������
	BYTE	mStatus;	//״̬
	BYTE	mSFN[11];	//���ļ���
	BOOL	mIsDelFile;	//�Ƿ����Ѿ�ɾ�������ļ�
	WCHAR	mLFN[MAX_LFN];//���ļ��������ռ� 255���ֽڵĴ�С
}DirEntry , *PDirEntry;


/****************************************************************************
						����һ��˫������
	�����о�һ�����̵ĵ����з�����ÿһ�������Ľڵ�����Ҫ�����˷����� ��ʼ����
	��(���Ե�����������),�Լ�ָ��˷�����DPT(�ļ�������(��))����
*****************************************************************************/
//////////////////////////////////////////////////////////////////////////
//����ڵ�ıȽϷ���
//param
//		n1,n2	���Ƚϵ������ڵ��ַ
//return
//		��� n1 < n2 ����TRUE
//		����FALSE
//////////////////////////////////////////////////////////////////////////
typedef BOOL (*NODE_COMPARE)(PVOID  n1 , PVOID n2 );

class DList
{
public:
	//��������Ľڵ�
	typedef struct __tagList_Node{
	__tagList_Node*	mNext;
	PVOID			mData;
	}Node,*PNode;

private:
	int        mCount;           //���̵ķ�����
	Node       mHead;           //��������ͷ���
	PNode      mPTail;           //��������β�ڵ�

	/*****************************************************************
	���ο������캯��
	ӦΪ������ָ��
	******************************************************************/	
	DList(DList& l );
	
	/*****************************************************************
	������������� ֱ�Ӳ�������
	param
		head            �����������
		cmpFun			�ڵ�ȽϷ�ʽ
	return
		NULL            ����ʧ��
	******************************************************************/
	Node* InsertSort(Node * head , NODE_COMPARE cmpFun);
public:
	DList(void);
	~DList(void);
	/*****************************************************************
	���һ�����������б�
	param 
		data			Ҫ��ӵ�����
	return 
		TRUE           �����ɹ�
		FALSE          ����ʧ�� ����pn=NULL��
	******************************************************************/
	BOOL AddPart(PVOID data);

	/*****************************************************************
	��������е�һ�������ڵ�
	param
		index          �ڵ������   0 1 2 ��
	return
		NULL           indexԽ�� ������������
	******************************************************************/
	VOID* GetPart(int index);
	
	/*****************************************************************
	ɾ��ָ���ķ���
		param
			index          �ڵ������   0 1 2 ��
		return
			NULL            û��ָ���Ľڵ�
	******************************************************************/
	PVOID DeletePart(int index);
	
	/*****************************************************************
	�������Ľڵ�����
	******************************************************************/
	int GetCount(void);

	/*****************************************************************
	������������� ֱ�Ӳ�������
	param
		cmpFun          ���ڱȽϽڵ�ĺ���
	return
		NULL            ����ʧ��
	******************************************************************/
	BOOL SortList(NODE_COMPARE cmpFun);
	//////////////////////////////////////////////////////////////////////////
	//�ڵ�ǰ�����в���һ���ڵ�
	//param
	//		index		������ָ����λ��
	//		data		�������������ʵλ��
	//return
	//		�����ɹ�  TRUE  ����ʧ��
	//////////////////////////////////////////////////////////////////////////
	BOOL InsertNode(int index, VOID* data);
};
typedef DList* PDList;

//////////////////////////////////////////////////////////////////////////
//�����ﶨ����һ����������ı�ʾ��ʽ�����Ա�ʾһ��MBR��EBR���ļ�ϵͳ����.
//δ���������򣬻����޷�����������
//////////////////////////////////////////////////////////////////////////
typedef struct _tagDPART{
	LONG_INT	mOffset;        //������������ƫ����,Ҳ���Ǳ���ǰʹ���˵���������
	DPT			mDPT;           //ָ�򱾷�����DPT��
	USHORT		mType;          //��������
	LONG_INT	mSecCount;		//���������������
	BOOL		mIsMainPart;	//�Ƿ�Ϊ������,֪���ڵ�ǰ�ڵ�ֵ���Ǿ��ʱ����Ч
}DPart , *PDPart;


//////////////////////////////////////////////////////////////////////////
//һ������FAT32�ļ����ҵ����ݽṹ��
//////////////////////////////////////////////////////////////////////////
typedef struct _tagFat32FileFinder{
	BOOL		isFindDel;		//�Ƿ��ǲ����Ѿ�ɾ���˵��ļ�
	BOOL		isEnd;			//�Ƿ��Ѿ����ҽ�����
	DirEntry	entry;			//һ�����ҵ����
	WCHAR		path[MAX_PATH + 1];//�����ҵ�Ŀ¼·��

}Fat32FileFinder , *PFat32FileFinder;


//////////////////////////////////////////////////////////////////////////
//����NTFS�ļ����ҵĲ������
//////////////////////////////////////////////////////////////////////////
typedef struct _tagFileFindHander{
	LONG_INT	vcn;		//����غ�  -1 ��ʾindexRoot�е�
	int			index;		//���ƶ�vcn�������
	DNtfsFile	dir;		//�����ҵ�Ŀ¼
}FIND_FILE_HANDER , *PFIND_FILE_HANDER;

//////////////////////////////////////////////////////////////////////////
//�߼���������Ӧ�ڴ��豸�ϵ�λ��
//////////////////////////////////////////////////////////////////////////
typedef struct _tagLOGCDRI{
	LONG_INT	byteOffset;	//���ڵ�ǰ���̵��ֽ�ƫ��
	DWORD		letter;		//��������߼��������ַ�
}LOGCDRI , *PLOGCDRI;



//////////////////////////////////////////////////////////////////////////
//FAT32��ʱ�䴦��,��дһtm�ṹ
//�������ֻ��д�� tm���꣬�£��գ�ʱ���֣���
//////////////////////////////////////////////////////////////////////////
inline struct tm  mktime(USHORT date , USHORT tim )
{
	TimeStamp ts;
	DataStamp ds;
	struct tm  cTm = {0};

	ts.time = tim;
	ds.date = date;

	//ʱ�䲿��
	cTm.tm_sec  =  ts.sec * 2;  //��
	cTm.tm_min  =  ts.min;
	cTm.tm_hour =  ts.hour;

	//���ڲ���
	cTm.tm_mday = ds.day ;
	cTm.tm_mon  = ds.month - 1;
	cTm.tm_year = ds.year + 80;


// 	//ʱ�䲿��
// 	cTm.tm_sec =  USHORT(USHORT(tim << 11) >>11) * 2;  //��
// 	cTm.tm_min = USHORT(USHORT(tim << 5) >> 10);
// 	cTm.tm_hour = USHORT(tim >> 11);
	
// 	//���ڲ���
// 	cTm.tm_mday = USHORT(USHORT((date << 11)) >>11) ;
// 	cTm.tm_mon = USHORT(USHORT(date << 7) >>12) - 1;
// 	cTm.tm_year = USHORT(date >> 9) + 80;

	return cTm;
}



//64λ���ݵĽṹ�Ĳ�������������
//������Ͳ�������  �������ܸо������˲��Ǻܺ�
// LONG_INT operator-(LONG_INT&l1 ,LONG_INT& l2); 
// LONG_INT operator+(LONG_INT&l1 ,LONG_INT& l2); 
// BOOL operator>=(LONG_INT&l1 ,LONG_INT& l2); 
// LONG_INT operator/(LONG_INT&l1 ,DWORD& d ); 
// LONG_INT operator/(LONG_INT&l1 ,int& d ); 
// LONG_INT operator+(LONG_INT&l1 ,DWORD& d); 
// LONG_INT operator*(LONG_INT&l1 ,DWORD& d); 
// LONG_INT operator*(LONG_INT&l1 ,int d); 
// BOOL operator==(LONG_INT&l1 ,LONG_INT& l2); 



/*/////////////////////////////////////////////////////////////////////////
					DPT�еĵ�0x6�ֽڵĶ�Ӧ�ķ�����������
	00	��  Microsoft ������ʹ��		01	FAT32
	02	XENIX root						03	XENIX usr
	04	FAT16 <32M						05	Externded  ��չ����
	06	FAT16							07	HPFS/NTFS
	08	AIX								09	AIX bootable
	0A	OS/2 Boot Manage				0B	Win95 FAT32
	0C	Win95 FAT32						0E	Win95 FAT16
	0F	Win95 externded��>8GB��			10	OPUS
	11	Hidden FAT12					12	Compaq diagnost
	14	Hidden FAT16 <32G				16	Hidden FAT16
	17	Hidden HPFS/HTFS				18	AST Windows swap
	1B	Hidden FAT32					1C	Hidden FAT32 partition (using LAB-mode INT 13 extension)
	1E	Hidden LAB VFAT partition		24	NEC DOS
	3C	Partition Magic					40	Venix 80286
	41	PPC PreP Boot					42	SFS
	4D	QNX4.x							4E	QNX4.x 2nd part
	4F	QNX4.x 3rd part					50	Ontrack DM
	51	Ontrack DM6 Aux					52	CP/M
	53	Ontrack DM6 Aux					54	Ontrack DM6
	55	EZ-Drive						56	Golden Bow
	5C	Priam Edick						61	Speed Stor
	63	GNU HURD or Sys					64	Novell Notware
	65	Novell Notware					70	Disk Secure Mult
	75	PC/IX							80	Old Minix
	81	Minix/Old Linux					82	Linux Swap
	83	Linux							84	OS/2 hidden C:
	85	Linux externded					86	NTFS volume set
	87	NTFS volume set					8E  Linux LVM
	93	Amoeba
	94	Amoeba BBT						A0	IBM thinkpad hidden partition
	A5	BSD/386							A6	Open BSD
	A7	NextSTEP						B7	BSDI fs
	B8	BSDI swap						BE	Solaris Boot
	C0	DR-DOS/Novell DOS secured 		C1	DR-DOS/Novell DOS secured 
	C4	DR-DOS/Novell DOS secured 		C6	DR-DOS/Novell DOS secured 
	C7	Syrinx							DB	CP/M/CTOS
	E1	DOS access						E3	DOS R/O
	E4	SpeedStor						EB	BeOS fs
	F1	SpeedStor						F2	DOS 3.3+  secondary
	F4	SpeedStor						FE	LAN step
	FF	BBT

	//һ�������Լ�����ģ��������ҵĳ����л�ʹ�õ������ģ������ͻ�ģ���׼ʱһ���ֽ�
	//�ҵĶ�������һ���ֽڵ����Ʒ�Χ
	#define  PART_MBR		0x100			//MBR
	#define  PART_EBR		0x101			//EBR
	#define  PART_UN_PART	0x102			//û�����Ŀ�������
	#define  PART_UNPARTBLE 0x103			//���ܷ���������
/////////////////////////////////////////////////////////////////////////*/

//��ԭ�ڴ���뷽ʽ
#pragma pack(pop)

#endif