
#7z decoder program (decoding 7z files)
x_target_begin(7zdec STATIC)
	x_add_definitions(
		_UNICODE
		UNICODE
		_7ZIP_PPMD_SUPPPORT
	)

	x_target_sources(
		#7zMain.c
		Precomp.c
		Precomp.h

		pkg/stdafx.h
		pkg/stdafx.cpp
		pkg/7zdec.h
		pkg/7zdec.cpp

		../../7zAlloc.c
		../../7zBuf.c
		../../7zCrc.c
		../../7zCrcOpt.c
		../../7zFile.c
		../../7zDec.c
		../../7zArcIn.c
		../../7zStream.c
		../../Bcj2.c
		../../Bra.c
		../../Bra86.c
		../../BraIA64.c
		../../CpuArch.c
		../../Delta.c
		../../Lzma2Dec.c
		../../LzmaDec.c
		../../Ppmd7.c
		../../Ppmd7Dec.c
	)

	x_export_header(
		pkg/7zdec.h
	)

x_target_end()
