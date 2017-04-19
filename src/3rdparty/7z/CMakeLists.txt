#主要构建7z-sdk/CPP/7zip/Bundles/Format7zR
#支持压缩解压7z/LZMA/BCJ/BCJ2
x_package_begin(7z STATIC)
	x_add_definitions(
		_UNICODE
		UNICODE
		_NO_CRYPTO
	)
	x_package_sources( #PCH(StdAfx.cpp StdAfx.h)

		CPP/7zip/Bundles/Format7zR/StdAfx.cpp
		CPP/7zip/Bundles/Format7zR/StdAfx.h

		CPP/7zip/Archive/Archive2.def

		CPP/Common/CrcReg.cpp
		CPP/Common/CRC.cpp
		CPP/Common/IntToString.cpp
		CPP/Common/IntToString.h
		CPP/Common/NewHandler.cpp
		CPP/Common/NewHandler.h
		CPP/Common/MyString.cpp
		CPP/Common/MyString.h
		CPP/Common/StringConvert.cpp
		CPP/Common/StringConvert.h
		CPP/Common/StringToInt.cpp
		CPP/Common/StringToInt.h
		CPP/Common/MyVector.cpp
		CPP/Common/MyVector.h
		CPP/Common/Wildcard.cpp
		CPP/Common/Wildcard.h

		CPP/Windows/FileDir.cpp
		CPP/Windows/FileFind.cpp
		CPP/Windows/FileIO.cpp
		CPP/Windows/FileName.cpp
		CPP/Windows/PropVariant.cpp
		CPP/Windows/Synchronization.cpp
		CPP/Windows/System.cpp

		CPP/7zip/Common/CreateCoder.cpp
		CPP/7zip/Common/CWrappers.cpp
		CPP/7zip/Common/InBuffer.cpp
		CPP/7zip/Common/InOutTempBuffer.cpp
		CPP/7zip/Common/FilterCoder.cpp
		CPP/7zip/Common/LimitedStreams.cpp
		CPP/7zip/Common/MethodId.cpp
		CPP/7zip/Common/MethodProps.cpp
		CPP/7zip/Common/OutBuffer.cpp
		CPP/7zip/Common/ProgressUtils.cpp
		CPP/7zip/Common/PropId.cpp
		CPP/7zip/Common/StreamBinder.cpp
		CPP/7zip/Common/StreamObjects.cpp
		CPP/7zip/Common/StreamUtils.cpp
		CPP/7zip/Common/UniqBlocks.cpp
		CPP/7zip/Common/VirtThread.cpp

		CPP/7zip/Archive/ArchiveExports.cpp
		CPP/7zip/Archive/DllExports2.cpp

		CPP/7zip/Archive/Common/CoderMixer2.cpp
		CPP/7zip/Archive/Common/HandlerOut.cpp
		CPP/7zip/Archive/Common/InStreamWithCRC.cpp
		CPP/7zip/Archive/Common/ItemNameUtils.cpp
		CPP/7zip/Archive/Common/OutStreamWithCRC.cpp
		CPP/7zip/Archive/Common/ParseProperties.cpp

		CPP/7zip/Archive/7z/7zCompressionMode.cpp
		CPP/7zip/Archive/7z/7zDecode.cpp
		CPP/7zip/Archive/7z/7zEncode.cpp
		CPP/7zip/Archive/7z/7zExtract.cpp
		CPP/7zip/Archive/7z/7zFolderInStream.cpp
		CPP/7zip/Archive/7z/7zHandler.cpp
		CPP/7zip/Archive/7z/7zHandlerOut.cpp
		CPP/7zip/Archive/7z/7zHeader.cpp
		CPP/7zip/Archive/7z/7zIn.cpp
		CPP/7zip/Archive/7z/7zOut.cpp
		CPP/7zip/Archive/7z/7zProperties.cpp
		CPP/7zip/Archive/7z/7zSpecStream.cpp
		CPP/7zip/Archive/7z/7zUpdate.cpp
		CPP/7zip/Archive/7z/7zRegister.cpp

		CPP/7zip/Compress/CodecExports.cpp
		CPP/7zip/Compress/Bcj2Coder.cpp
		CPP/7zip/Compress/Bcj2Register.cpp
		CPP/7zip/Compress/BcjCoder.cpp
		CPP/7zip/Compress/BcjRegister.cpp
		CPP/7zip/Compress/BranchMisc.cpp
		CPP/7zip/Compress/BranchRegister.cpp
		CPP/7zip/Compress/ByteSwap.cpp
		CPP/7zip/Compress/CopyCoder.cpp
		CPP/7zip/Compress/CopyRegister.cpp
		CPP/7zip/Compress/DeltaFilter.cpp
		CPP/7zip/Compress/Lzma2Decoder.cpp
		CPP/7zip/Compress/Lzma2Encoder.cpp
		CPP/7zip/Compress/Lzma2Register.cpp
		CPP/7zip/Compress/LzmaDecoder.cpp
		CPP/7zip/Compress/LzmaEncoder.cpp
		CPP/7zip/Compress/LzmaRegister.cpp

		C/Alloc.c
		C/Bcj2.c
		C/Bcj2Enc.c
		C/Bra.c
		C/Bra86.c
		C/BraIA64.c
		C/CpuArch.c
		C/Delta.c
		C/LzFind.c
		C/LzFindMt.c
		C/Lzma2Dec.c
		C/Lzma2Enc.c
		C/LzmaDec.c
		C/LzmaEnc.c
		C/MtCoder.c
		C/Threads.c
		C/7zCrcOpt.c
	)
x_package_end()
