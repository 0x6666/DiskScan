
x_target_begin(installer EXECUTABLE)
	x_add_definitions(
		_UNICODE
	)
	x_target_sources( PCH(stdafx.cpp stdafx.h)
		main.cpp
	)

	x_link_targets(
		7zdec
	)

x_target_end()
