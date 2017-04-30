
x_target_begin(strtool SHARED)
	x_add_definitions(
		_UNICODE
	)
	x_target_sources( PCH(StdAfx.cpp StdAfx.h)
		CodePage.cpp
		CodePage.h
		resource.h
		StrTool.cpp
		strtool.h
		strtool.rc
	)
	x_export_header(
		strtool.h
	)
x_target_end()
