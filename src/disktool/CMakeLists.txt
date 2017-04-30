
x_target_begin(disktool SHARED)
	x_add_definitions(
		_UNICODE
	)
	x_target_sources( PCH (stdafx.cpp stdafx.h)
		fat32/dfat32.cpp
		fat32/dfat32file.cpp
		disk/disk.cpp
		disk/DList.cpp
		ntfs/dntfs.cpp
		ntfs/ntfsattr.cpp
		ntfs/ntfsfile.cpp
		ntfs/run.cpp
		disktool.h
		stut_connf.h
		DiskTool.rc
		#Part.cpp
		#resource.h
		#DataType.cpp
	)
	x_link_targets(
		strtool
	)
	x_export_header(
		disktool.h
	)
	x_ignorr_warn (
		4996
		4251
		#4018
	)
x_target_end()
