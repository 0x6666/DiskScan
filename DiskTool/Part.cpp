/***********************************************************************
 * FileName:	Part.cpp
 * Author:		杨松
 * Created:		2012年3月20日 星期二
 * Purpose:		实现磁盘区域的节点基本方法
 * Comment:		这个类主要是抽象一个磁盘区域，如MBR，EBR，FAT32卷NTFS卷等
 ***********************************************************************/
// 
// #include "disktool.h"
// #include "stut_connf.h"
// 
// DPart::~DPart(){}
// DPart::DPart()
// : mType(0)
// , mIsMainPart(FALSE)
// {
// //	this->mDeviceName[0] = 0;
// 	this->mOffset.QuadPart = 0;
// 	::memset(&this->mDPT , 0  ,sizeof(this->mDPT));
// 	this->mSecCount.QuadPart = 0;
// }
// 
// bool DPart::IsActivityPart(void)
// {
// 	return (this->mDPT.mGuidFlag == 0x80);
// }
// 
// int DPart::GetStartHead(void)
// {
// 	return this->mDPT.mStartHead;
// }
// 
// int DPart::GetStartSector(void)
// {
// 	return this->mDPT.mStartSector;
// }
// 
// int DPart::GetStartCylinder(void)
// {
// 	return this->mDPT.mStartCylinder;
// }
// 
// USHORT DPart::GetPartFormat(void)
// {
// 	return this->mType;
// }
// 
// DPT DPart::GetDPT(void)
// {
// 	return this->mDPT;
// }
// 
// unsigned long DPart::GetRelativeSectors(void)
// {
// 	return this->mDPT.mRelativeSectors;
// }
// 
// LONG_INT DPart::GetSectorCount(void)
// {
// 	return this->mSecCount;
// }
// LONG_INT DPart::GetPartOffset()
// {
// 	return mOffset;
// }
// // const char* DPart::GetDevName(void)
// // {
// // 	return this->mDeviceName;
// // }
