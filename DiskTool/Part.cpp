/***********************************************************************
 * FileName:	Part.cpp
 * Author:		����
 * Created:		2012��3��20�� ���ڶ�
 * Purpose:		ʵ�ִ�������Ľڵ��������
 * Comment:		�������Ҫ�ǳ���һ������������MBR��EBR��FAT32��NTFS���
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
