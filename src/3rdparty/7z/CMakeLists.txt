
#主要构建7z-sdk/CPP/7zip/Bundles/Format7zR
#支持压缩解压7z/LZMA/BCJ/BCJ2
x_target_begin(Format7zR STATIC)
	x_add_definitions(
		_UNICODE
		UNICODE
		_NO_CRYPTO
	)
	x_target_sources( #PCH(StdAfx.cpp StdAfx.h)

		7z.h
		stdafx.h
		stdafx.cpp
		
		pkg/c7z.h
		pkg/c7z.cpp

		#CPP/7zip/Bundles/Format7zR/StdAfx.cpp
		#CPP/7zip/Bundles/Format7zR/StdAfx.h

		CPP/7zip/Archive/Archive2.def

		CPP/7zip/ICoder.h
		CPP/7zip/IDecl.h
		CPP/7zip/IPassword.h
		CPP/7zip/IProgress.h
		CPP/7zip/IStream.h
		CPP/7zip/PropID.h

		CPP/7zip/Common/CreateCoder.cpp
		CPP/7zip/Common/CreateCoder.h
		CPP/7zip/Common/CWrappers.cpp
		CPP/7zip/Common/CWrappers.h
		CPP/7zip/Common/InBuffer.cpp
		CPP/7zip/Common/InBuffer.h
		CPP/7zip/Common/InOutTempBuffer.cpp
		CPP/7zip/Common/InOutTempBuffer.h
		CPP/7zip/Common/FilterCoder.cpp
		CPP/7zip/Common/FilterCoder.h
		CPP/7zip/Common/FileStreams.h
		CPP/7zip/Common/FileStreams.cpp
		CPP/7zip/Common/LimitedStreams.cpp
		CPP/7zip/Common/LimitedStreams.h
		CPP/7zip/Common/MethodId.cpp
		CPP/7zip/Common/MethodId.h
		CPP/7zip/Common/MethodProps.cpp
		CPP/7zip/Common/MethodProps.h
		CPP/7zip/Common/OutBuffer.cpp
		CPP/7zip/Common/OutBuffer.h
		CPP/7zip/Common/ProgressUtils.cpp
		CPP/7zip/Common/ProgressUtils.h
		CPP/7zip/Common/PropId.cpp
		CPP/7zip/Common/StreamBinder.cpp
		CPP/7zip/Common/StreamBinder.h
		CPP/7zip/Common/StreamObjects.cpp
		CPP/7zip/Common/StreamObjects.h
		CPP/7zip/Common/StreamUtils.cpp
		CPP/7zip/Common/StreamUtils.h
		CPP/7zip/Common/UniqBlocks.cpp
		CPP/7zip/Common/UniqBlocks.h
		CPP/7zip/Common/VirtThread.cpp
		CPP/7zip/Common/VirtThread.h

		CPP/7zip/Archive/IArchive.h
		CPP/7zip/Archive/XzHandler.h

		CPP/7zip/Archive/ArchiveExports.cpp
		CPP/7zip/Archive/DllExports2.cpp

		CPP/7zip/Archive/Common/CoderMixer2.cpp
		CPP/7zip/Archive/Common/CoderMixer2.h
		CPP/7zip/Archive/Common/HandlerOut.cpp
		CPP/7zip/Archive/Common/HandlerOut.h
		CPP/7zip/Archive/Common/InStreamWithCRC.cpp
		CPP/7zip/Archive/Common/InStreamWithCRC.h
		CPP/7zip/Archive/Common/ItemNameUtils.cpp
		CPP/7zip/Archive/Common/ItemNameUtils.h
		CPP/7zip/Archive/Common/OutStreamWithCRC.cpp
		CPP/7zip/Archive/Common/OutStreamWithCRC.h
		CPP/7zip/Archive/Common/ParseProperties.cpp
		CPP/7zip/Archive/Common/ParseProperties.h

		CPP/7zip/Archive/7z/7zCompressionMode.cpp
		CPP/7zip/Archive/7z/7zCompressionMode.h
		CPP/7zip/Archive/7z/7zDecode.cpp
		CPP/7zip/Archive/7z/7zDecode.h
		CPP/7zip/Archive/7z/7zEncode.cpp
		CPP/7zip/Archive/7z/7zEncode.h
		CPP/7zip/Archive/7z/7zExtract.cpp
		CPP/7zip/Archive/7z/7zFolderInStream.cpp
		CPP/7zip/Archive/7z/7zFolderInStream.h
		CPP/7zip/Archive/7z/7zHandler.cpp
		CPP/7zip/Archive/7z/7zHandler.h
		CPP/7zip/Archive/7z/7zHandlerOut.cpp
		CPP/7zip/Archive/7z/7zHeader.cpp
		CPP/7zip/Archive/7z/7zHeader.h
		CPP/7zip/Archive/7z/7zIn.cpp
		CPP/7zip/Archive/7z/7zIn.h
		CPP/7zip/Archive/7z/7zOut.cpp
		CPP/7zip/Archive/7z/7zOut.h
		CPP/7zip/Archive/7z/7zProperties.cpp
		CPP/7zip/Archive/7z/7zProperties.h
		CPP/7zip/Archive/7z/7zSpecStream.cpp
		CPP/7zip/Archive/7z/7zSpecStream.h
		CPP/7zip/Archive/7z/7zUpdate.cpp
		CPP/7zip/Archive/7z/7zUpdate.h
		CPP/7zip/Archive/7z/7zRegister.cpp

		CPP/7zip/Compress/CodecExports.cpp
		CPP/7zip/Compress/Bcj2Coder.cpp
		CPP/7zip/Compress/Bcj2Coder.h
		CPP/7zip/Compress/Bcj2Register.cpp
		CPP/7zip/Compress/BcjCoder.cpp
		CPP/7zip/Compress/BcjCoder.h
		CPP/7zip/Compress/BcjRegister.cpp
		CPP/7zip/Compress/BranchMisc.cpp
		CPP/7zip/Compress/BranchMisc.h
		CPP/7zip/Compress/BranchRegister.cpp
		CPP/7zip/Compress/ByteSwap.cpp
		CPP/7zip/Compress/CopyCoder.cpp
		CPP/7zip/Compress/CopyCoder.h
		CPP/7zip/Compress/CopyRegister.cpp
		CPP/7zip/Compress/DeltaFilter.cpp
		CPP/7zip/Compress/Lzma2Decoder.cpp
		CPP/7zip/Compress/Lzma2Decoder.h
		CPP/7zip/Compress/Lzma2Encoder.cpp
		CPP/7zip/Compress/Lzma2Encoder.h
		CPP/7zip/Compress/Lzma2Register.cpp
		CPP/7zip/Compress/LzmaDecoder.cpp
		CPP/7zip/Compress/LzmaDecoder.h
		CPP/7zip/Compress/LzmaEncoder.cpp
		CPP/7zip/Compress/LzmaEncoder.h
		CPP/7zip/Compress/LzmaRegister.cpp

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
		CPP/Windows/FileDir.h
		CPP/Windows/FileFind.cpp
		CPP/Windows/FileFind.h
		CPP/Windows/FileIO.cpp
		CPP/Windows/FileIO.h
		CPP/Windows/FileName.cpp
		CPP/Windows/FileName.h
		CPP/Windows/PropVariant.cpp
		CPP/Windows/PropVariant.h
		CPP/Windows/Synchronization.cpp
		CPP/Windows/Synchronization.h
		CPP/Windows/System.cpp
		CPP/Windows/System.h

		C/Alloc.c
		C/Alloc.h
		C/Bcj2.c
		C/Bcj2.h
		C/Bcj2Enc.c
		C/Bra.c
		C/Bra.h
		C/Bra86.c
		C/BraIA64.c
		C/CpuArch.c
		C/CpuArch.h
		C/Delta.c
		C/Delta.h
		C/LzFind.c
		C/LzFind.h
		C/LzFindMt.c
		C/LzFindMt.h
		C/Lzma2Dec.c
		C/Lzma2Dec.h
		C/Lzma2Enc.c
		C/Lzma2Enc.h
		C/LzmaDec.c
		C/LzmaDec.h
		C/LzmaEnc.c
		C/LzmaEnc.h
		C/MtCoder.c
		C/MtCoder.h
		C/Threads.c
		C/Threads.h

		C/7zCrc.c
		C/7zCrcOpt.c
	)

	x_gen_symbal_file()

	x_export_header(
		7z.h
	)
x_target_end()
