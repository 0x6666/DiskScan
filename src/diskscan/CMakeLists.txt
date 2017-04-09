
x_package_begin(diskscan EXECUTABLE)
	x_add_definitions(
		_UNICODE
		UNICODE
	)

	x_enable_mfc(1)
	x_uac_exe_level(requireAdministrator)

	x_package_sources( PCH (StdAfx.cpp StdAfx.h)
		ChildFrm.cpp
		ChildFrm.h
		ColumnSplitter.cpp
		ColumnSplitter.h
		CopyProcessDlg.cpp
		CopyProcessDlg.h
		CtrlBar.cpp
		CtrlBar.h
		DataDoc.cpp
		DataDoc.h
		DataWnd.cpp
		DataWnd.h
		DevVolume.cpp
		DevVolume.h
		DiskInfoView.cpp
		DiskInfoView.h
		DiskScan.cpp
		DiskScan.h
		DiskScan.rc
		#DiskScan.reg
		DiskScanDoc.cpp
		DiskScanDoc.h
		DiskScanView.cpp
		DiskScanView.h
		Fat32Doc.cpp
		Fat32Doc.h
		Fat32FileAttr.cpp
		Fat32FileAttr.h
		Fat32InfoView.cpp
		Fat32InfoView.h
		InfoView.cpp
		InfoView.h
		MainFrm.cpp
		MainFrm.h
		NtfsDoc.cpp
		NtfsDoc.h
		NtfsFileDlg.cpp
		NtfsFileDlg.h
		NtfsInfoView.cpp
		NtfsInfoView.h
		SectorList.cpp
		SectorList.h
		publicFun.cpp
		publicFun.h
		resource.h
	)

	x_link_packages(
		disktool
		strtool
	)

	x_ignorr_warn (
		4996
		4251
		#4018
	)

x_package_end()
