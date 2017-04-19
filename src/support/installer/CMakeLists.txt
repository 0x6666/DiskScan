
x_package_begin(installer EXECUTABLE)
	x_add_definitions(
		_UNICODE
	)
	x_package_sources( PCH(stdafx.cpp stdafx.h)
		main.cpp
	)
x_package_end()
