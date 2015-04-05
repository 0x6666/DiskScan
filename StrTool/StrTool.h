/*/////////////////////////////////////////////////////////////////////////
	�������Ҫ�������ַ��Ĵ���,���ַ������ת����Ŀǰֻ����GBK<=>Unicode
/////////////////////////////////////////////////////////////////////////*/
/*#ifdef STRTOOL_EXPORTS
#define STRTOOL_API __declspec(dllexport)
#else
#define STRTOOL_API __declspec(dllimport)
#endif
*/
#define STRTOOL_API

#ifndef	__STR_TOOL__
#define __STR_TOOL__

//���õ�����
#undef  DWORD
typedef unsigned long    DWORD;
#undef  PDWORD
typedef DWORD*		PDWORD;
#undef	USHORT
typedef	unsigned short USHORT;
#undef	UINT
typedef	unsigned int UINT;
#undef	WCHAR
typedef wchar_t WCHAR;
#undef	PWCHAR
typedef wchar_t* PWCHAR;
#undef	CHAR
typedef	char	CHAR;
#undef	UCHAR
typedef	unsigned char	UCHAR;
#undef	BOOL
typedef int		BOOL;
#undef	TRUE
#define TRUE	1
#undef	FALSE
#define FALSE	0

typedef union __tag_W_CHAR_{
	struct{
		CHAR	char2;
		CHAR	char1;
	};
	WCHAR		charw;
}W_CHAR ,* PW_CHAR;


/*/���õļ򵥺꺯��///////////////////////////////////////////////////////////////////////*/
#define  IsMultyByteLead(x)     ((x) <= 0xFE && (x) >= 0x81)
#define  IsSingleByteChar(x)     ((x) < 0x80)

//GBK��ͷ�ַ�
#define  IsGBKHead(x)		IsMultyByteLead(x)
//GBK��β�ַ�
#define  IsGBKTail(x)		((0x40<=(x) && (x)<=0x7E)|| ((x)>=0x80 &&(x)<=0xFE))

//////////////////////////////////////////////////////////////////////////
//��Сд��ASCII�ַ�ת���ɴ�дд��ʽ�������Сд�Ļ��Ͳ�ת��
//////////////////////////////////////////////////////////////////////////
#define ChrToUpper(x)	(((x) >= 0x61 && (x) <= 0x7A ) ? ((x) - 0x20) : (x))

//////////////////////////////////////////////////////////////////////////
//����д��ASCII�ַ�ת����Сдд��ʽ���������д�Ļ��Ͳ�ת��
//////////////////////////////////////////////////////////////////////////
#define ChrToLower(x)	(((x) >= 0x41 && (x) <= 0x5A ) ? ((x) + 0x20) : (x))


/*/Unicode�ַ������CodePage////////////////////////////////////////////////////////////////*/
#define CP_GBK		936      //GBK����ҳ  ����һ����Ĭ�ϴ���ʽ

extern "C"{
/*******************************************************************************************
�ַ���ת��
param
	src		��ת�����ַ�
	toUnic	TRUE ת����Unicode ��FALSEת����codepageָ��������ַ�
	codepage	ָ���ı���ҳ
return  
	res ת������ַ�,
		��toUnic=FALSEʱ,res.char1��ʾ˫�ֽ��ַ��ĵ�һ���ַ�res.char2��Ϊ�ض����ַ�
		���res.char1 == 0��Ϊ���ֽ��ַ�
		��toUnic=TRUEʱres.charw��������unicode�ַ�
********************************************************************************************/
STRTOOL_API W_CHAR WINAPI ChrConvert(W_CHAR src , BOOL toUnic , UINT codepage = CP_GBK);


/*******************************************************************************************
Unicode�ַ���ת���ɶ��ֽ��ַ�
param
	src		��ת����Unicode�ַ���  ��NUL��β
	buf		ת����Ķ��ֽ��ַ���
	bufLen	buf�Ĵ�С ������NUL��,�������Ϊ0��ֻ�Ǽ���ת��������Ҫ�Ļ���ռ�
	codepage ��ǰ�ı����
return 
	ת���˵Ķ��ֽ��ַ�����������Ҫ�Ļ���ռ��С(������NUL)
********************************************************************************************/
STRTOOL_API DWORD WINAPI UnicToMultyByte(const WCHAR* src , CHAR* buf , DWORD bufLen , UINT codepage = CP_GBK);

/*******************************************************************************************
���ֽ��ַ�ת����Unicode
param
	src		��ת���Ķ��ֽ��ַ���  ��NUL��β
	buf		ת�����Unicode�ַ���
	bufLen	buf�Ĵ�С���ַ�������NUL��,�������Ϊ0��ֻ�Ǽ���ת��������Ҫ�Ļ���ռ�
	codepage ��ǰ�ı����
return 
	ת���˵Ķ��ֽ��ַ�����������Ҫ�Ļ���ռ��С(������NUL)
********************************************************************************************/
STRTOOL_API DWORD WINAPI MultyByteToUnic(const CHAR* src , WCHAR* buf ,size_t bufLen , UINT codepage = CP_GBK);

/*******************************************************************************************
��Unicode�ַ��е�Ӣ���ַ�ת���ɴ�д��ʽ
param
	src		��ת�����ַ�
return 
	ת������ַ�  �������Ӣ���ַ� �򷵻�ԭ�ַ�
********************************************************************************************/
STRTOOL_API WCHAR WINAPI WchrToUpper(const WCHAR src);

// 
// //////////////////////////////////////////////////////////////////////////
// //��ASCII�ַ��е�Ӣ���ַ�ת����Сдд��ʽ
// //param
// //	src		��ת�����ַ�
// //return 
// //	ת������ַ�  �������Ӣ���ַ� �򷵻�ԭ�ַ�
// //////////////////////////////////////////////////////////////////////////
// STRTOOL_API CHAR WINAPI ChrToLower(const CHAR src);
// 
// /*******************************************************************************************
// ��ASCII�ַ��е�Ӣ���ַ�ת���ɴ�д��ʽ
// param
// 	src		��ת�����ַ�
// return 
// 	ת������ַ�  ������������ַ� �򷵻�Ԫ�ַ�
// ********************************************************************************************/
// STRTOOL_API CHAR WINAPI ChrToUpper(const CHAR src);

//////////////////////////////////////////////////////////////////////////
//�ж�һ���ַ��Ƿ�Ϊ��Ч�ַ�,���ж�ascllֻ�ж��ڴ���ҳ�еģ���ǰҲֻ֧��GBK
//param
//			src		��Ҫ�жϵ��ַ�
//			codepage �ַ�����ҳ
//return	���ָ�����ַ�����ҳ���ڵĵ�ǰ�ַ��Ļ��ͷ���TRUE������FALSE
//////////////////////////////////////////////////////////////////////////
STRTOOL_API BOOL WINAPI IsValidCode(const W_CHAR src , UINT codepage = CP_GBK);

}
#endif