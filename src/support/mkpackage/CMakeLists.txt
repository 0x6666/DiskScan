
x_target_begin(mkpackage CONSOLE)
	x_add_definitions(
		_UNICODE
	)
	x_target_sources( PCH(stdafx.cpp stdafx.h)
		main.cpp
		command_line.h
		command_line.cpp
	)
	
	x_link_targets(
		Format7zR
	)
	
x_target_end()
