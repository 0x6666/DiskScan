//#include "stdafx.h"
#include "disktool.h"
#include "stut_connf.h"
// 
// LONG_INT   operator-(LONG_INT&l1 ,LONG_INT& l2)
// {
// 	LONG_INT res;
// 	res.QuadPart= l1.QuadPart - l2.QuadPart;
// 	return res;
// }
// LONG_INT   operator+(LONG_INT&l1 ,LONG_INT& l2)
// {
// 	LONG_INT res;
// 	res.QuadPart= l1.QuadPart + l2.QuadPart;
// 	return res;
// }
// 
// BOOL  operator>=(LONG_INT&l1 ,LONG_INT& l2)
// {
// 	return (l1.QuadPart >= l2.QuadPart);
// }
// LONG_INT  operator/(LONG_INT&l1 ,DWORD& d )
// {
// 	LONG_INT res ;
// 	res.QuadPart = l1.QuadPart / d;
// 	return res;
// }
// LONG_INT  operator+(LONG_INT&l1 ,DWORD& d)
// {
// 	LONG_INT res;
// 	res.QuadPart= l1.QuadPart + d;
// 	return res;
// }
// BOOL  operator==(LONG_INT&l1 ,LONG_INT& l2)
// {
// 	return l1.QuadPart == l2.QuadPart;
// }
// LONG_INT  operator/(LONG_INT&l1 ,int& d )
// {
// 	LONG_INT res  = l1;
// 	res.QuadPart /=d;
// 	return res;
// }
// LONG_INT operator*(LONG_INT&l1 ,DWORD& d)
// {
// 	LONG_INT res  = l1;
// 	res.QuadPart *=d;
// 	return res;
// }
// LONG_INT operator*(LONG_INT&l1 ,int d)
// {
// 	LONG_INT res  = l1;
// 	res.QuadPart *=d;
// 	return res;
// }