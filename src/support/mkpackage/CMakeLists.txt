
x_package_begin(mkpackage CONSOLE)
	x_add_definitions(
		_UNICODE
	)
	x_package_sources( PCH(stdafx.cpp stdafx.h)
		main.cpp
		command_line.h
		command_line.cpp
	)
	
	x_link_packages(
		Format7zR
	)
	
x_package_end()
