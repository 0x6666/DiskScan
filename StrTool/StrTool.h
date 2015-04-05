/*/////////////////////////////////////////////////////////////////////////
	这个库主要是用于字符的处理,如字符编码的转换，目前只能是GBK<=>Unicode
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

//常用的类型
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


/*/常用的简单宏函数///////////////////////////////////////////////////////////////////////*/
#define  IsMultyByteLead(x)     ((x) <= 0xFE && (x) >= 0x81)
#define  IsSingleByteChar(x)     ((x) < 0x80)

//GBK的头字符
#define  IsGBKHead(x)		IsMultyByteLead(x)
//GBK的尾字符
#define  IsGBKTail(x)		((0x40<=(x) && (x)<=0x7E)|| ((x)>=0x80 &&(x)<=0xFE))

//////////////////////////////////////////////////////////////////////////
//将小写的ASCII字符转换成大写写形式，如果不小写的话就不转换
//////////////////////////////////////////////////////////////////////////
#define ChrToUpper(x)	(((x) >= 0x61 && (x) <= 0x7A ) ? ((x) - 0x20) : (x))

//////////////////////////////////////////////////////////////////////////
//将大写的ASCII字符转换成小写写形式，如果不大写的话就不转换
//////////////////////////////////////////////////////////////////////////
#define ChrToLower(x)	(((x) >= 0x41 && (x) <= 0x5A ) ? ((x) + 0x20) : (x))


/*/Unicode字符编码的CodePage////////////////////////////////////////////////////////////////*/
#define CP_GBK		936      //GBK代码页  现在一般是默认处理方式

extern "C"{
/*******************************************************************************************
字符的转换
param
	src		待转换的字符
	toUnic	TRUE 转换到Unicode ，FALSE转换到codepage指定编码的字符
	codepage	指定的编码页
return  
	res 转换后的字符,
		当toUnic=FALSE时,res.char1表示双字节字符的第一个字符res.char2则为地二个字符
		如果res.char1 == 0则为单字节字符
		当toUnic=TRUE时res.charw就是整个unicode字符
********************************************************************************************/
STRTOOL_API W_CHAR WINAPI ChrConvert(W_CHAR src , BOOL toUnic , UINT codepage = CP_GBK);


/*******************************************************************************************
Unicode字符串转换成多字节字符
param
	src		待转换的Unicode字符串  以NUL结尾
	buf		转换后的多字节字符串
	bufLen	buf的大小 （包括NUL）,如果此域为0则只是计算转换所以需要的缓存空间
	codepage 当前的编码表
return 
	转换了的多字节字符数，或者需要的缓存空间大小(不包括NUL)
********************************************************************************************/
STRTOOL_API DWORD WINAPI UnicToMultyByte(const WCHAR* src , CHAR* buf , DWORD bufLen , UINT codepage = CP_GBK);

/*******************************************************************************************
多字节字符转换成Unicode
param
	src		待转换的多字节字符串  以NUL结尾
	buf		转换后的Unicode字符串
	bufLen	buf的大小（字符数包括NUL）,如果此域为0则只是计算转换所以需要的缓存空间
	codepage 当前的编码表
return 
	转换了的多字节字符数，或者需要的缓存空间大小(不包括NUL)
********************************************************************************************/
STRTOOL_API DWORD WINAPI MultyByteToUnic(const CHAR* src , WCHAR* buf ,size_t bufLen , UINT codepage = CP_GBK);

/*******************************************************************************************
将Unicode字符中的英文字符转换成大写形式
param
	src		待转换的字符
return 
	转换后的字符  如果不是英文字符 则返回原字符
********************************************************************************************/
STRTOOL_API WCHAR WINAPI WchrToUpper(const WCHAR src);

// 
// //////////////////////////////////////////////////////////////////////////
// //将ASCII字符中的英文字符转换成小写写形式
// //param
// //	src		待转换的字符
// //return 
// //	转换后的字符  如果不是英文字符 则返回原字符
// //////////////////////////////////////////////////////////////////////////
// STRTOOL_API CHAR WINAPI ChrToLower(const CHAR src);
// 
// /*******************************************************************************************
// 将ASCII字符中的英文字符转换成大写形式
// param
// 	src		待转换的字符
// return 
// 	转换后的字符  如果不是引文字符 则返回元字符
// ********************************************************************************************/
// STRTOOL_API CHAR WINAPI ChrToUpper(const CHAR src);

//////////////////////////////////////////////////////////////////////////
//判断一个字符是否为有效字符,不判断ascll只判断在代码页中的，当前也只支持GBK
//param
//			src		将要判断的字符
//			codepage 字符编码页
//return	如果指定的字符编码页存在的当前字符的话就返回TRUE，否则FALSE
//////////////////////////////////////////////////////////////////////////
STRTOOL_API BOOL WINAPI IsValidCode(const W_CHAR src , UINT codepage = CP_GBK);

}
#endif