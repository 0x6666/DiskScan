/***********************************************************************
 * FileName:	stut_connf.h
 * Author:		杨松
 * Created:		2012年3月20日 星期二
 * Purpose:		不需要导出了类和数据结构等
 * Comment:		此文件主要是定义一些需要使用的预定义宏以及一些磁盘操作相
 *				关的非导出数据结构、类等
 ***********************************************************************/

#ifndef DISK_INFO_STRUCTS
#define DISK_INFO_STRUCTS

//这个文件设计打算是不需要导出的，但是还是对齐一下比较好
#pragma pack(push , 8)

//需要打开windows的磁盘所以需要使用一些windows下的api

#ifdef _MSC_VER  //是vc编译器

#if _MSC_VER <= 1200	//vc6.0一下一般没有定义一下的一些数据 

#ifndef IOCTL_DISK_GET_LENGTH_INFO  //用于查找设备大小的相关定义
#define IOCTL_DISK_GET_LENGTH_INFO 0x7405C
#endif

#ifndef IOCTL_VOLUME_GET_VOLUME_DISK_EXTENTS     //获得一个逻辑驱动在物理设备上的位置信息
#define IOCTL_VOLUME_GET_VOLUME_DISK_EXTENTS 0x560000
#endif // IOCTL_VOLUME_GET_VOLUME_DISK_EXTENTS

#ifndef GET_LENGTH_INFORMATION		//查找设备的实际大小
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

#ifndef _VOLUME_DISK_EXTENTS  //查询一个卷在物理磁盘上的位置
typedef struct _VOLUME_DISK_EXTENTS {
    DWORD       NumberOfDiskExtents;
    DISK_EXTENT Extents[1];
} VOLUME_DISK_EXTENTS, *PVOLUME_DISK_EXTENTS;
#endif

#endif
#endif

//保存当前对其状态 然后设置一下对其为 1字节
#pragma pack(push , 1)

/*****************************************************************************************
	磁盘分区表结构 DPT
	00H--00H   0x1字节  引导标志 
						80H——活动分区
						00H——示非活动分区
	01H--01H   0x1字节  本分区的起始磁头号
	02H--03H   0x2字节  低6位 本分区起始扇区号  高8位本分区起始柱面号
	04H--04H   0x1字节  分区类型符。
						00H——表示该分区未用（即没有指定）；
						01H——DFAT32
						04H——FAT16
						06H——FAT16基本分区；
						0BH——DFAT32基本分区；
						0CH——DFAT32
						05H——扩展分区；
						07H——NTFS分区  或者HPFS；
						0FH——LBA模式 扩展分区
						63H——Unix   分区  
						83H——Linux  Native
						82H——Linux  交换区
	05H--05H   0x1字节  本分区的结束磁头号
	06H--07H   0x2字节  低6位 本分区结束扇区号  高8位本分区结束柱面号
	08H--0AH   0x4字节  分区相对扇区数，指分区相对于记录该分区的分区表的扇区位置之差
	0BH--0EH   0x4字节  分区总扇区数
	总0x10(16)字节
******************************************************************************************/
typedef struct _tagDPT{
	UCHAR  mGuidFlag;          //引导标记
	UCHAR  mStartHead;         //本分区起始磁头
	USHORT mStartSector:6;     //本分区起始扇区号
	USHORT mStartCylinder:8;   //本分区起始柱面
	UCHAR  mPartType;		   //分区类型
	UCHAR  mEndHead;           //本分区结束磁头
	USHORT mEndSector:6;       //本分区结束扇区号
	USHORT mEndCylinder:8;     //本分区结束柱面
	DWORD  mRelativeSectors;   //本分区的相对其实扇区号
	DWORD  mSectorCount;       //本分区扇区数
}DPT , *PDPT;


/*****************************************************************************************
	磁盘的主引导记录
	0000H--0088H  0x88字节  主引导程序 负责从活动分区中装载，并运行系统引导程序。
	0089H--00E1H  0x58字节  出错信息数据区
	00E2H--01BDH  0xF9字节  保留字段  一般全为零
	
	以上 0x1EB字节暂时不分析

	001BE--001CD   分区表 1
	001CE--001DD   分区表 2
	001DE--001ED   分区表 3
	001EE--001FD   分区表 4
	001FE--001FF   结束标记  55 AA
		
	总 0x20 (512)字节
******************************************************************************************/
typedef struct _tagMBR{
	UCHAR   mGuideCode[0x1BE];         //磁盘引导代码
	////四个分区域记录
	DPT     mDPT[4];
	USHORT  mEndSig;                  //结束标记   55 AA
}MBR , *PMBR;

//逻辑分区前的磁盘分区表
typedef MBR   EBR;
typedef PMBR  PEBR;

//////////////////////////////////////////////////////////////////////////
//扩展分区表
//////////////////////////////////////////////////////////////////////////
typedef struct _tagExtDPT{
	char	partName[8];	//分区名字
	BYTE	noUse1;			//没有使用
	BYTE	partNum;		//分区编号
	BYTE	ctrlNum;		//管理逻辑结点号
	BYTE	noUse2;			//没有使用
	DWORD	startSector;	//其实物理扇区号
	DWORD	sectorCount;	//分区总数
	DWORD	emptySecCnt;	//空闲扇区数
	WORD	noUse3;			//没有使用
	BYTE	partFormate;	//分区类型
	BYTE	noUse4[5];		//没有使用的
}ExtDPT , *PExtDPT;
//判断扩展分表表是否为空
#define IsExtPDPTEmpty(x) ((((PLONG_INT)(x))[0].QuadPart == 0) &&	\
					       (((PLONG_INT)(x))[1].QuadPart == 0) &&	\
						   (((PLONG_INT)(x))[2].QuadPart == 0) &&	\
						   (((PLONG_INT)(x))[3].QuadPart == 0))

//入口的状态
#define ST_LAST		0x1		//00000001	已经匹配了最后一个段了
#define ST_LOWER	0x2		//00000010	有小写
#define ST_UPPER	0x4		//00000100	有大写
#define ST_LFN		0x8		//00001000	一定要长文件名		

//文件状态
#define  F_ST_EOF	0x1		//00000001	已经是文件的结尾



//此结构是DFAT32的DBR
typedef struct  _tagDFAT32_DBR
{
	UCHAR       bsJump[3];          //跳转指令  跳转到dbr的引导程序
	UCHAR       bsOemName[8];       //OEM名称
	USHORT      bsBytesPerSec;      //扇区的字节数
	UCHAR       bsSecPerClus;       //每簇的的扇区数
	USHORT      bsResSectors;       //保留的扇区数(第一个fat之前的扇区数)
	UCHAR       bsFATs;             //FAT数
	USHORT      bsRootDirEnts;      //跟入口点有几个
	USHORT      bsSectors;          //扇区数  大于65535是 使用下面的bsHugeSectors
	UCHAR       bsMedia;            //介质类型YSDISK_MEDIA_TYPE
	USHORT      bsFATsecs;          //每个FAT占的扇区数
	USHORT      bsSecPerTrack;      //每个磁道的扇区数 32
	USHORT      bsHeads;            //有多少个磁头 也就是每个柱面的磁道数
	DWORD       bsHiddenSecs;       //隐藏扇区数
	DWORD       bsHugeSectors;      //一个卷超过65535个扇区是  是用此字段书名扇区数	
	DWORD       bsFATsecs32;        //FAT32 fat表所占扇区数  bsFATsecs必须为0
	USHORT      bsFlags32;          //FAT32 标记
	USHORT      bsVersion32;        //FAT32 高位为主版本号，低位为次版本号
	DWORD       bsFirstDirEntry32;  //FAT32 根目录第一簇(一般为2)
	USHORT      bsFsInfo32;         //FAT32 保留区中 DFAT32 卷 FSINFO 结构所在的扇区号（保留扇区），通常为1
	USHORT      bsBakDbr32;         //FAT32 如果不为 0，表示在保留区中引导记录的备数据所在的扇区号，通常为 6。同时不建议使用 6 以外的其他数值
	UCHAR       bsRreserved[12];    //FAT32 保留 用于以后 FAT 扩展使用，对 DFAT32。此域用 0 填充
	UCHAR       bsDriveNumber;      //驱动器编号
	UCHAR       bsReserved1;        //保留字段
	UCHAR       bsBootSignature;    //磁盘扩展引导区标签 0x29
	DWORD       bsVolumeID;         //磁盘卷ID  在这里设为0x12345678
	UCHAR       bsLabel[11];        //磁盘卷标YSDisk
	UCHAR       bsFSType[8];		//文件系统类型 - FAT12 or FAT16
	UCHAR       bsReserved2[420];   //也是一个保留字段
	USHORT      bsEndSig;			//结束标签 - 0xAA55
}FAT32_DBR , *PFAT32_DBR;

/**********************************************************************************************/
//短文件名的入口结构
typedef struct  _tagSHORT_DIR_ENTRY
{
	UCHAR       mName[8];          //文件名
	UCHAR       mExt[3];	       //文件扩展
	UCHAR       mAttr;		       //文件属性
	UCHAR       mNameCase;         //文件名、扩展名的大小写定义
	UCHAR		mCrtTimeTenth;	   //创建时间的十分之一秒数
	USHORT		mCrtTime;		   //创建时间
	USHORT		mCrtDate;		   //创建日期
	USHORT		mLstAccDate;	   //最后一次访问时间
	USHORT		mFstClusHI;		   //第一簇号的高两字节
	USHORT		mWrtTime;		   //修改时间
	USHORT		mWrtDate;		   //修改日期
	USHORT		mFstClusLO;		   //第一簇号的第两字节
	DWORD       mFileSize;         //文件长度
}SDE, *PSDE;

/**********************************************************************************************
	长文件名的入口结构
***********************************************************************************************/
typedef struct  _tagLONG_DIR_ENTRY
{
	UCHAR       mOrder;			//索引 
	UCHAR       mName1[10];		//名字的第一部分
	UCHAR       mAttr;			//文件属性
	UCHAR       mNameCase;		//文件名、扩展名的大小写定义
	UCHAR		mChksum;		//文件名的校验和
	UCHAR		mName2[12];     //文件名的第二部分
	USHORT      mFstClusLO;     //  0
	UCHAR       mName3[4];		//文件长度
}LDE, *PLDE;



// 16位的FAT时间戳
// 0-4位：有效值是1-31，表示日
// 5-8位：有效值是1-12，表示月
// 9-15位：有效值0-127，相对于1980的年号
typedef union _tagDataStamp {
	struct{
		USHORT day : 5;
		USHORT month : 4;
		USHORT year : 7;
	};
	USHORT date;
}DataStamp ,*PDataStamp;

// 16位的FAT日期戳
// 0-4位：其有效值是0-29，表示两秒数量，表示实际的秒为0-58
// 5-10位：有效值是0-59，表示分钟
// 11-15位：有效值是0-23，表示小时
typedef union _tagTimeStamp {
	struct{
		USHORT sec : 5;
		USHORT min : 6;
		USHORT hour : 5;
	};
	USHORT time;
}TimeStamp , *PTimeStamp;





/************************************************************************/
/* 一下是NTFS转悠数据结构                                               */
/************************************************************************/
//一下定义了NTFS的DBR的数据结构
typedef struct _tagNTFS_DBR{
	BYTE dbrJump[3];		//三个字节的跳转指令
	BYTE dbrOemName[8];		//OEM名称
	
	//接下来是25个字节的BPB
	WORD  bpbBytePerSec;	//Bytes Per Sector
	BYTE  bpbSecPerClu;		//Sectors Per Cluster
	WORD  bpbResSec;		//保留山区数
	BYTE  bpbNULL[3];		//always 0
	WORD  bpbNoUse;			//not used by NTFS
	BYTE  bpbMediaType;		//0xF8 Media Descriptor
	WORD  bpbNULL1;			//always 0
	WORD  bpbSecPerTrack;	//SectorPer Track
	WORD  bpbHeads;			//Number of heads
	DWORD bpbHidSec;		//Hidden Sector
	DWORD bpbNoUse2;		//not used by NTFS
	
	//接下来是48个字节的扩展BPB
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

	//接下来是426个字节的引导代码
	BYTE  dbrCode[426];		//引导代码
	WORD  dbrEnd;			//结束标志

}NTFS_DBR , *PNTFS_DBR;


/************************************************************************/
/* 索引记录头，每个IndexRoot和每个IndexBlockHead都会包含一个这样的结构体*/
/* 主要用记录当前索引区的信息											*/
/************************************************************************/
typedef struct _tagINDEX_HEAD{
	DWORD IH_EntryOff;	//第一个索引项（目录项）的偏移（相对索引块头）
	DWORD IH_TalSzOfEntries;//所有索引项的总尺寸
	DWORD IH_AllocSize;	//所有索引项的总分配尺寸
	BYTE  IH_Flags;		//标志位
						//  0x00小目录(目录项存放IR中)
						//  0x01大目录(需要外部索引块和位图)
	BYTE IH_Resvd[3];	//保留  用于对齐到四字节位置
}INDEX_HEAD , *PINDEX_HEAD;
 
/************************************************************************/
/* 索引根记录                                                           */
/************************************************************************/
typedef struct _tagINDEX_ROOT{
	DWORD IR_AttrType;	//属性的类型
	DWORD IR_ColRule;	//整理规则
	DWORD IR_EntrySz;	//每个索引块 尺寸（4k）
	BYTE  IR_ClusPerRec;//每个索引块 占用的簇数
	BYTE  IR_Resvd[3];	//保留  用于对齐到4字节

	INDEX_HEAD IR_IdxHead;	//索引头部
}INDEX_ROOT , *PINDEX_ROOT;

/************************************************************************/
/* 标准索引头，这也是一个不完全数据结构                                 */
/* 在indexblock中如果文件名超出了8.3规则的话，就会出现两个知indexEntry，*/
/* 都指向同一个MFT记录,一个用于兼容DOS，还有一个具体不知*/
/************************************************************************/
typedef struct _tagINDEX_BLOCK_HEAD{
	BYTE	 IB_Sign[4];	//记录签名：值为：‘INDX’
	WORD	 IB_USOff;		//更新序列号的偏移(M) 恒=0x28
	WORD	 IB_USNSz;		//更新序列号个数+1(N) 恒=0x09(4K/512+1)
	LONG_INT IB_LSN;		//日志文件序列号，该值在记录每次被修改时都会被改动
	LONG_INT IB_VCN;		//当前 索引块 在目录文件 中的虚拟簇号

	INDEX_HEAD IB_indexHead;//索引的头部信息
	//WORD		us;			//Update sequence    //为了保持队形 这里不定以此值
	//0x2A 2S-2 Update sequence array   
}INDEX_BLOCK_HEAD , *PINDEX_BLOCK_HEAD;



/************************************************************************/
/*文件记录头 0x30个字节                                                 */
/************************************************************************/
typedef struct _tagFILE_RECODE_HEAD{
	BYTE  FR_Sign[4];	//记录签名：值为：‘ELIF’
	WORD  FR_USOff;		//更新序列号的偏移(M) (相对文件记录头)
	WORD  FR_USNSz;		//更新序列号个数+1(N)
	BYTE  FR_LSN[8];	//日志文件序列号，该值在记录每次被修改时都会被改动
	WORD  FR_SN;		//重复使用 更新序列号（删除一次加1）
	WORD  FR_LnkCnt;	//目录中记录本文件的引用计数，该值只用于基本文件记录
	WORD  FR_1stAttrOff;//第一个属性数据的偏移
	WORD  FR_Flags;		//标志，该成员可以是以下各值中之一
						//0x0001	记录被使用
						//0x0002	目录文件
	DWORD FR_Size;		//当前记录的尺寸
	DWORD FR_AllocSz;	//当前记录分配的空间的尺寸
	BYTE  FR_BaseFR[8];	//当前文件记录的基本文件记录的索引，如果当前文件记录是基本文
						//件记录则该值为0，否则指向基本文件记录的记录索引。注意：该值
						//的低6字节是MFT记录号，高2字节是该MFT记录的序列号
	WORD  FR_NxtAttrId;	//下一个属性的ID。但不是属性的总数  , 应为每一次添加属性都会增加此值，此值很定不会小于属性总数
						//下一次将会被添加到文件记录的属性的ID，每次往文件记录中添加属性时该值都会增加，每次文件记录被重新使用时该值都会被清零，第一个值肯定是0
	WORD  FR_Resvd;		//保留(XP新增,3.1+)
	DWORD FR_NumOfFR;	//该MFT 记录号 (XP新增,3.1+)
}FILE_RECODE_HEAD , *PFILE_RECODE_HEAD;


/************************************************************************/
/* 这是一个不完全的结构体，因为里面的存空间长度，有另一个字段决定       */
/************************************************************************/
typedef struct _tagFILE_NAME{
	LONG_INT FN_ParentFR;	//父目录的MFT记录的记录索引。
							//注意：该值的低6字节是MFT记录号，高2字节是该MFT记录的序列号
	LONG_INT FN_CreatTime;	//文件创建的时间
	LONG_INT FN_AlterTime;	//文件最后一次被修改的时间
	LONG_INT FN_MFTChg;		//文件的MFT记录被修改的时间
	LONG_INT FN_ReadTime;	//最后一次访问文件的时间
	LONG_INT FN_AllocSize;	//文件数据占用的空间尺寸，该值按簇尺寸对齐
	LONG_INT FN_ValidSize;	//文件的真实尺寸，索引项（目录项）中显示的文件尺寸就是该值。如果文件的数据属性中的开始簇号不为0则本项的值为空，此时需要查询文件尺寸的话必须访问SI属性
	DWORD	 FN_DOSAttr;	//DOS文件属性，
	DWORD	 FN_EA_Reparse;	//扩展属性与链接
	BYTE	 FN_NameSize;	//文件名的字符数(L)
	BYTE 	 FN_NamSpace;	//命名空间，该值可为以下值中的任意一个
							//0：POSIX　可以使用除NULL和分隔符“/”之外的所有UNICODE字符，最大可以使用255个字符。注意：“：”是合法字符，但Windows不允许使用。
							//1：Win32　Win32是POSIX的一个子集，不区分大小写，可以使用除““”、“＊”、“?”、“：”、“/”、“<”、“>”、“\”、“|”之外的任意UNICODE字符，但名字不能以“.”或空格结尾。
							//2：DOS　DOS命名空间是Win32的子集，只支持ASCII码大于空格的8BIT大写字符并且不支持以下字符““”、“＊”、“?”、“：”、“/”、“<”、“>”、“\”、“|”、“+”、“,”、“;”、“=”；同时名字必须按以下格式命名：1~8个字符，然后是“.”，然后再是1~3个字符。
							//3：Win32&DOS　这个命名空间意味着Win32和DOS文件名都存放在同一个文件名属性中。
	WCHAR	FN_FileName[1];	//2*L	不需要以0作为结束字符
// 	//name的产度具体由nameLen决定，所以这是一个不完全结构体
}FILE_NAME ,*PFILE_NAME;

/************************************************************************/
/* 在老的NTFS版本中，STARDAND_INFORMATION属性只用于存放文件的时间和DOS文*/
/* 件属性信息。在Windows 2000中新引进了4个域来描述配额、安全、文件尺寸和*/
/* 日志信息。															*/
/************************************************************************/
typedef struct _tagSTD_INFO{
//-	-	标准属性头(24 字节)
	LONG_INT   SI_CreatTime;	//文件创建时间
	LONG_INT   SI_AlterTime;	//文件最后一次修改时间
	LONG_INT   SI_MFTChgTime;	//文件的MFT记录修改的时间
	LONG_INT   SI_ReadTime;		//最后一次访问的时间
	DWORD	   SI_DOSAttr;		//DOS文件属性，可以是以ATTR_  开头的属于ntfs的属性值组合
	DWORD	   SI_MaxVer;		//文件可用的最大版本号，此值为0表示版本功能被禁止(目前未使用？)
	DWORD	   SI_Ver;			//文件版本号，如果最大版本号为0则值也必须为0
	DWORD	   SI_ClassId;		//不明(目前未使用？)
	DWORD      SI_OwnerId;		//文件拥有者的ID，本ID是$Quota文件中$O或$Q记录中的键，如果该值为0则表示配额设置被禁止
	DWORD      SI_SecurityId;	//安全ID，本值是$Securce文件中$SII索引及$SDS数据流的键
	DWORD	   SI_QuotaCharged;	//该文件最大可使用的空间配额。如果该值为0则表示无配额限制
	DWORD	   SI_USN;			//文件最后一次更新的记录号，该值是$UsnJrnl文件的直接索引，如果该值为0则表示更新日志被禁止
}STD_INFO , *PSTD_INFO;

/************************************************************************/
/* 索引项,此为不完整结构体，也就是说因为此结构的某些属性石变动的，无法确*/
/* 定其大小,所以一般只是用其指针功能                                    */
/************************************************************************/
#define  INDEX_ENTRY_NODE	0x01	//
#define  INDEX_ENTRY_END	0x02	//索引表结束

typedef struct _tagINDEX_RNTRY{
	//下面的域只有在最后的登录标记没有被设置的情况下才是有效的
	//也就是说flag没有标记INDEX_ENTRY_END
	LONG_INT IE_FR;		//文件介绍 
	WORD IE_Size;		//当前索引项的长度 
	WORD IE_DataSize;	//流的长度 
	WORD IE_Flags;		//标记 
	WORD notUse;		//用于对齐
	//下面的域只有在没有设置子节点标记时才存在 
	//也就是说flag没有设置INDEX_ENTRY_NODE
	BYTE IE_Stream[1];	//流，变动的数据段,这是表
						//示一个文件或者目录的话这里存放的FILE_NAME属性结构
	
	//当flag设置了INDEX_ENTRY_NODE时此结构的最后各个字节表示一个vcn
	//最后的8字节 索引分配属性中的子节点的VCN 
}INDEX_ENTRY , *PINDEX_ENTRY;


//属性头
typedef struct _tagATTR_HEAD{
	DWORD ATTR_Type;	//0x00	属性类型。该值必须是$AttrDef文件中定义的属性列表中的一项
	DWORD ATTR_Size;	//0x04	属性尺寸(包括属性头)
	BYTE  ATTR_NonResFlag;//0x08 属性数据非驻留标志，值为1
	BYTE  ATTR_NamSz;	//0x09	属性名尺寸，该值为0时表示当前属性没有名字
	WORD  ATTR_NamOff;	//0x0a	属性名相对于属性头的偏移，该值总是固定为0x18
	WORD  ATTR_Flags;	//0x0c	属性标志，在当前的版本中只支持以下3个标志：
						// 0x0001属性被压缩
						// 0x4000属性被加密
						// 0x8000稀疏属性
						// 注意：只有非驻留的数据(Data)属性才能被压缩
	WORD  ATTR_Id;		//属性ID，在MFT记录中的每个属性都有一个唯一的ID。属性的索引无关
	//BYTE  remain[1];	//用于表示接下来的数据
}ATTR_HEAD , *PATTR_HEAD;


/************************************************************************/
/* 常驻属性头                                                           */
/************************************************************************/
typedef struct _tagRESID_ATTR_HEAD{//常驻属性
	ATTR_HEAD stdHead;		//标注属性头
	DWORD     ATTR_DatSz;	//属性数据尺寸
	WORD	  ATTR_DatOff;	//属性数据相对于属性头的偏移
	BYTE	  ATTR_Indx;	//属性索引标志(对FILE_NAME有效)
	BYTE	  ATTR_Resvd;   //保留  用于对齐到8字节
	//ATTR_AttrNam	2*N	属性名字Unicode字符串，这个字符串不用在结尾处加0
	//				这个属性的有无需要根据ATTR_NamSz决定,所以这里不给出
	//ATTR_AttrDat	0x18+2*N		属性数据，所以的数据都应该按双字对齐
}RESID_ATTR_HEAD , *PRESID_ATTR_HEAD;

/************************************************************************/
/* 非常驻属性头                                                         */
/************************************************************************/
typedef struct _tagNON_RESID_ATTR_HEAD{//非常驻属性
	ATTR_HEAD stdHead;
	LONG_INT ATTR_StartVCN;	//本属性中数据流开始的簇号
	LONG_INT ATTR_EndVCN;	//本属性中数据流最后一簇的簇号
	WORD     ATTR_DataOff;	//数据流描述相对于属性头的偏移，数据应该按双字对齐
	WORD     ATTR_CmpSize;	//压缩单元的尺寸。压缩单元的尺寸必须是2的整数次幂，为0表示未压缩
	DWORD    ATTR_Resvd;	//保留 ,用于对齐到8字节
	LONG_INT ATTR_AllocSize;//属性记录数据块分配的空间的尺寸，该尺寸按簇尺寸对齐
	LONG_INT ATTR_ValidSize;//属性记录数据块的实际尺寸
	LONG_INT ATTR_InitedSize;//属性记录数据块已经初始化数据的尺寸，到目前为止该值都与属性记录数据块分配的尺寸相同
	//ATTR_AttrNam	0x40	2*N	属性名的UNICODE字符串，该字符串不以0结尾
	//ATTR_DataRuns	0x40+2*N		属性数据流描述(为各非常驻属性逻辑簇区域指向,具体实体在指向区域)
}NON_RESID_ATTR_HEAD , *PNON_RESID_ATTR_HEAD;


//恢复对其状态
#pragma pack(pop)

//运行的头部  NTFS
typedef union _tagRunHead{
	struct{
		BYTE length:4;  //长度
		BYTE offset:4;  //偏移
	};
	BYTE all;			//总体描述
}RunHead , *PRunHead;



//此文件系统的文件或目录入口结构
typedef struct _tagDIR_ENTRY
{
	WORD	mIndex;		//当前读写的目录入口号 
	DWORD	mStartClust;//起始簇号
	DWORD	mCurClust;	//当前簇号
	DWORD	mCurSect;	//当前扇区号
	BYTE*	mDir;		//指向FAT32::mView的短文件名入口
	BYTE	mStatus;	//状态
	BYTE	mSFN[11];	//短文件名
	BOOL	mIsDelFile;	//是否是已经删除来的文件
	WCHAR	mLFN[MAX_LFN];//长文件名工作空间 255个字节的大小

	_tagDIR_ENTRY()
		: mIndex(0)
		, mStartClust(0)
		, mCurClust(0)
		, mCurSect(0)
		, mDir(0)
		, mStatus(0)
		, mIsDelFile(0)
	{
		memset(mSFN, 0, 11);
		memset(mLFN, 0, MAX_LFN);
	}

}DirEntry , *PDirEntry;


/****************************************************************************
						这是一个双端链表
	用于列举一个磁盘的的所有分区，每一个分区的节点中需要包含此分区的 起始扇区
	号(绝对的物理扇区号),以及指向此分区的DPT(文件分区表(项))……
*****************************************************************************/
//////////////////////////////////////////////////////////////////////////
//链表节点的比较方法
//param
//		n1,n2	待比较的两个节点地址
//return
//		如果 n1 < n2 返回TRUE
//		否则FALSE
//////////////////////////////////////////////////////////////////////////
typedef BOOL (*NODE_COMPARE)(PVOID  n1 , PVOID n2 );

class DList
{
public:
	//单向链表的节点
	typedef struct __tagList_Node {
		__tagList_Node* mNext;
		PVOID			mData;

		__tagList_Node() :mNext(0), mData(0){}
	}Node,*PNode;

private:
	int        mCount;           //磁盘的分区数
	Node       mHead;           //分区链表头结点
	PNode      mPTail;           //分区链表尾节点

	/*****************************************************************
	屏蔽拷贝构造函数
	应为里面有指针
	******************************************************************/	
	DList(DList& l );
	
	/*****************************************************************
	对链表进行排序 直接插入排序
	param
		head            待排序的链表
		cmpFun			节点比较方式
	return
		NULL            排序失败
	******************************************************************/
	Node* InsertSort(Node * head , NODE_COMPARE cmpFun);
public:
	DList(void);
	~DList(void);
	/*****************************************************************
	添加一个分区到本列表
	param 
		data			要添加的数据
	return 
		TRUE           操作成功
		FALSE          操作失败 （如pn=NULL）
	******************************************************************/
	BOOL AddPart(PVOID data);

	/*****************************************************************
	获得链表中的一个分区节点
	param
		index          节点的索引   0 1 2 …
	return
		NULL           index越界 或者其他错误
	******************************************************************/
	VOID* GetPart(int index);
	
	/*****************************************************************
	删除指定的分区
		param
			index          节点的索引   0 1 2 …
		return
			NULL            没有指定的节点
	******************************************************************/
	PVOID DeletePart(int index);
	void Clear();

	/*****************************************************************
	获得链表的节点数量
	******************************************************************/
	int GetCount(void);

	/*****************************************************************
	对链表进行排序 直接插入排序
	param
		cmpFun          用于比较节点的函数
	return
		NULL            排序失败
	******************************************************************/
	BOOL SortList(NODE_COMPARE cmpFun);
	//////////////////////////////////////////////////////////////////////////
	//在当前链表中插入一个节点
	//param
	//		index		链表中指定的位置
	//		data		带插入的数据其实位置
	//return
	//		操作成功  TRUE  否则失败
	//////////////////////////////////////////////////////////////////////////
	BOOL InsertNode(int index, VOID* data);
};
typedef DList* PDList;

//////////////////////////////////////////////////////////////////////////
//在这里定义了一个磁盘区域的表示方式，可以表示一个MBR。EBR。文件系统分区.
//未分区的区域，或者无法分区的区域
//////////////////////////////////////////////////////////////////////////
typedef struct _tagDPART{
	LONG_INT	mOffset;        //本分区的物理偏移量,也就是本分前使用了的总扇区数
	DPT			mDPT;           //指向本分区的DPT项
	USHORT		mType;          //分区类型
	LONG_INT	mSecCount;		//此区域的扇区数量
	BOOL		mIsMainPart;	//是否为主分区,知否在当前节点值得是卷的时候有效
}DPart , *PDPart;


//////////////////////////////////////////////////////////////////////////
//一个用于FAT32文件查找的数据结构体
//////////////////////////////////////////////////////////////////////////
typedef struct _tagFat32FileFinder{
	BOOL		isFindDel;		//是否是查找已经删除了的文件
	BOOL		isEnd;			//是否已经查找结束了
	DirEntry	entry;			//一个查找的入口
	WCHAR		path[MAX_PATH + 1];//被查找的目录路径

	_tagFat32FileFinder()
		: isFindDel(false)
		, isEnd(false)
	{
		memset(path, 0, MAX_PATH + 1);
	}

}Fat32FileFinder , *PFat32FileFinder;


//////////////////////////////////////////////////////////////////////////
//用于NTFS文件查找的操作句柄
//////////////////////////////////////////////////////////////////////////
typedef struct _tagFileFindHander{
	LONG_INT	vcn;		//虚拟簇号  -1 表示indexRoot中的
	int			index;		//在制定vcn入口索引
	DNtfsFile	dir;		//被查找的目录

	_tagFileFindHander() : index(0){}
}FIND_FILE_HANDER , *PFIND_FILE_HANDER;

//////////////////////////////////////////////////////////////////////////
//逻辑驱动器对应在此设备上的位置
//////////////////////////////////////////////////////////////////////////
typedef struct _tagLOGCDRI{
	LONG_INT	byteOffset;	//卷在当前磁盘的字节偏移
	DWORD		letter;		//卷对饮的逻辑驱动器字符
}LOGCDRI , *PLOGCDRI;



//////////////////////////////////////////////////////////////////////////
//FAT32的时间处理,填写一tm结构
//这个函数只填写了 tm的年，月，日，时，分，秒
//////////////////////////////////////////////////////////////////////////
inline struct tm  mktime(USHORT date , USHORT tim )
{
	TimeStamp ts;
	DataStamp ds;
	struct tm  cTm = {0};

	ts.time = tim;
	ds.date = date;

	//时间部分
	cTm.tm_sec  =  ts.sec * 2;  //秒
	cTm.tm_min  =  ts.min;
	cTm.tm_hour =  ts.hour;

	//日期部分
	cTm.tm_mday = ds.day ;
	cTm.tm_mon  = ds.month - 1;
	cTm.tm_year = ds.year + 80;


// 	//时间部分
// 	cTm.tm_sec =  USHORT(USHORT(tim << 11) >>11) * 2;  //秒
// 	cTm.tm_min = USHORT(USHORT(tim << 5) >> 10);
// 	cTm.tm_hour = USHORT(tim >> 11);
	
// 	//日期部分
// 	cTm.tm_mday = USHORT(USHORT((date << 11)) >>11) ;
// 	cTm.tm_mon = USHORT(USHORT(date << 7) >>12) - 1;
// 	cTm.tm_year = USHORT(date >> 9) + 80;

	return cTm;
}



//64位数据的结构的操作操作簇重载
//在这里就不导出了  这玩意总感觉导出了不是很好
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
					DPT中的第0x6字节的对应的分区类型如下
	00	空  Microsoft 不可以使用		01	FAT32
	02	XENIX root						03	XENIX usr
	04	FAT16 <32M						05	Externded  扩展分区
	06	FAT16							07	HPFS/NTFS
	08	AIX								09	AIX bootable
	0A	OS/2 Boot Manage				0B	Win95 FAT32
	0C	Win95 FAT32						0E	Win95 FAT16
	0F	Win95 externded（>8GB）			10	OPUS
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

	//一下是我自己定义的，类型在我的程序中会使用到。放心，不会冲突的，标准时一个字节
	//我的都超出了一个字节的趋势范围
	#define  PART_MBR		0x100			//MBR
	#define  PART_EBR		0x101			//EBR
	#define  PART_UN_PART	0x102			//没分区的空闲区域
	#define  PART_UNPARTBLE 0x103			//不能分区的区域
/////////////////////////////////////////////////////////////////////////*/

//还原内存对齐方式
#pragma pack(pop)

#endif