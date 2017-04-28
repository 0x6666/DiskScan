
#属性名规范
#XT_xxxx: 设置到TARGET上的属性
#XG_xxxx: 全局属性

#cmake代码目录
set(XG_CMAKE_DIR ${CMAKE_CURRENT_LIST_DIR})

# 导入参数解释函数
include(CMakeParseArguments)

include("${XG_CMAKE_DIR}/internal.cmake")

# x_package_begin(<name> [STATIC|SHARED|EXECUTABLE|CONSOLE])
# 开始定义一个工程
macro(x_package_begin pkg_name)

	_def_package(${pkg_name})

	set(pkg_types "STATIC|SHARED|EXECUTABLE|CONSOLE")

	cmake_parse_arguments(_ARG "" "" ""  ${ARGN})
	set(_argn ${_ARG_UNPARSED_ARGUMENTS})

	_find_radio_option(${_argn} ${pkg_types} "EXECUTABLE" pkg_type)

	if("${pkg_type}" STREQUAL "")
		message(FATAL_ERROR "could not find the package type")
	endif()

	set(X_PACKAGE_NAME "${pkg_name}")
	set(X_PACKAGE_TYPE "${pkg_type}")
	set(X_HAS_EXPORT_HEADER)
	set(X_PACKAGE_SOURCE_FILES)
	set(X_COMPILE_FLAGS)
	set(X_CUR_LINK_PACKAGES) #当前工程需要链接的库
	set(X_EXECUTABLE_ENTRYPOINT "main")
	set(X_PCH_HEADER)

	# c++11
	if(${OS_LINUX})
		add_compile_options(-std=c++11)
	endif()

	#include_directories(${CMAKE_CURRENT_BINARY_DIR})
	include_directories(${CMAKE_CURRENT_SOURCE_DIR})
	include_directories("${CMAKE_BINARY_DIR}/export_header")

endmacro(x_package_begin)

# 结束一个工程的定义
macro(x_package_end)

	if (DEFINED CMAKE_MFC_FLAG)
		unset(X_EXECUTABLE_ENTRYPOINT)
	endif()

	set(_src_to_compile ${X_PACKAGE_SOURCE_FILES})
	_group_src_files_by_dir(${_src_to_compile})
	_set_pch_ref(${_src_to_compile})

	if("${X_PACKAGE_TYPE}" STREQUAL "STATIC")
		add_library(${X_PACKAGE_NAME} STATIC ${X_PACKAGE_SOURCE_FILES})
	elseif("${X_PACKAGE_TYPE}" STREQUAL "SHARED")
		add_library(${X_PACKAGE_NAME} SHARED ${X_PACKAGE_SOURCE_FILES})
	elseif("${X_PACKAGE_TYPE}" STREQUAL "EXECUTABLE")
		if(OS_WIN)
			add_executable(${X_PACKAGE_NAME} WIN32 ${X_PACKAGE_SOURCE_FILES})
			if(X_EXECUTABLE_ENTRYPOINT)
				_append_target_property(${X_PACKAGE_NAME} LINK_FLAGS " /ENTRY:\"${X_EXECUTABLE_ENTRYPOINT}CRTStartup\" ")
				_append_target_property(${X_PACKAGE_NAME} LINK_FLAGS " /OPT:NOREF ")
			endif()
		else()
			add_executable(${X_PACKAGE_NAME} ${X_PACKAGE_SOURCE_FILES})
		endif()
	elseif("${X_PACKAGE_TYPE}" STREQUAL "CONSOLE")
		add_executable(${X_PACKAGE_NAME} ${X_PACKAGE_SOURCE_FILES})
	else()
		message(FATAL_ERROR "invalid package type")
	endif()

	if(OS_WIN AND DEFINED X_UAC_EXE_LEVEL)
		if(("${X_PACKAGE_TYPE}" STREQUAL "EXECUTABLE") OR ("${X_PACKAGE_TYPE}" STREQUAL "CONSOLE"))
			_append_target_property(${X_PACKAGE_NAME} LINK_FLAGS " /level='${X_UAC_EXE_LEVEL}' ")
		else()
			message(WARNING "UAC level should set to executable target")
		endif()
	endif()

	_global_ignore_cmp_warn()
	_append_target_property(${X_PACKAGE_NAME} COMPILE_FLAGS ${X_COMPILE_FLAGS})

	if(X_CUR_LINK_PACKAGES)
		set_property(TARGET ${X_PACKAGE_NAME} PROPERTY XT_LINK_PACKAGES ${X_CUR_LINK_PACKAGES})
	endif()
	set_property(TARGET ${X_PACKAGE_NAME} PROPERTY XT_PACKAGE_TYPE ${X_PACKAGE_TYPE})
endmacro(x_package_end)

# 定义工程源代码文件
# x_package_sources([PCH (header.h [source<.cpp|.cc|.cxx>])]
#					 a.h b.c c.cpp ...
#)
macro(x_package_sources)

	set(_argn ${ARGN})
	while(NOT "${_argn}" STREQUAL "")
		list(GET _argn 0 _src)
		list(REMOVE_AT _argn 0)
		if("${_src}" MATCHES "(PCH)")
			_get_pch_header(_pch_header _pch_src _argn ${_argn})
			list(APPEND X_PCH_HEADER ${_pch_header} ${_pch_src})
			list(APPEND X_PACKAGE_SOURCE_FILES ${_pch_header} ${_pch_src})
		else()
			string(TOLOWER "${_src}" _src)
			if(NOT ${_src} MATCHES "(\\.c|\\.cpp|\\.h|\\.rc|\\.def)")
				message(FATAL_ERROR "Unrecognized source file ${_f}")
			endif()
			list(APPEND X_PACKAGE_SOURCE_FILES ${_src})
		endif()
	endwhile()
endmacro(x_package_sources)

# ignore warnings
macro(x_ignorr_warn)
	_add_ignorr_warn(${ARGN})
endmacro(x_ignorr_warn)

macro(x_export_header)
	foreach(_f ${ARGN})
		_create_export_header_ref(${_f} ${X_PACKAGE_NAME})
	endforeach()
endmacro(x_export_header)

# x_extren_package(_pkg_name _pkg_type
#			<LOCATION _location>
# )
macro(x_extren_package _pkg_name _pkg_type)
	_def_package(${_pkg_name})

	set(_args ${ARGN})
	cmake_parse_arguments(
		X_EXT_PKG
		""
		"LOCATION;IMPT_LOCATION"
		"EXPORT_HEADER;DEPENDS;BINARY_NAMES;BINARY_NAMES_DEBUG;BINARY_NAMES_RELEASE"
		${_args}
	)

	# output dir
	set(_outputdir "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")

	# import location
	if(NOT X_EXT_PKG_IMPT_LOCATION)
		set(X_EXT_PKG_IMPT_LOCATION ${X_EXT_PKG_LOCATION})
	endif()

	# file names
	if(X_EXT_PKG_BINARY_NAMES)
		set(_names ${X_EXT_PKG_BINARY_NAMES})
	else()
		set(_names ${X_EXT_PKG_BINARY_NAMES_${X_BUILD_CFG}})
	endif()

	set(_output_files)
	foreach(_name ${_names})
		if(OS_WIN)
			set(_model_path "${X_EXT_PKG_LOCATION}/${_name}.dll")
			set(_lib "${X_EXT_PKG_IMPT_LOCATION}/${_name}.lib")
		elseif(OS_LINUX)
			set(_so_file_name "lib${_name}.so")
			set(_model_path "${X_EXT_PKG_LOCATION}/${_so_file_name}")
			set(_lib ${_model_path})
		else()
			message(FATAL_ERROR "invalid os")
		endif()

		if("${_pkg_type}!" STREQUAL "SHARED!")
			if(NOT EXISTS ${_outputdir})
				file(MAKE_DIRECTORY "${_outputdir}")
			endif()

			get_filename_component(_filename "${_model_path}" NAME)

			if(OS_WIN)
				set(_cp_command COMMAND ${CMAKE_COMMAND} -E copy ${_model_path} ${_outputdir}/${_filename})
			else()
				set(_cp_command COMMAND cp -R ${_model_path}* ${_outputdir})
			endif()

			add_custom_command(
				OUTPUT ${_outputdir}/${_filename}
				${_cp_command}
				WORKING_DIRECTORY ${_package_destdir}
				MAIN_DEPENDENCY "${_model_path}"
			)

			list(APPEND _output_files ${_outputdir}/${_filename})
		endif()
	endforeach()

	# package type
	if("${_pkg_type}!" STREQUAL "STATIC!")
		add_custom_target(${_pkg_name})
	elseif("${_pkg_type}!" STREQUAL "SHARED!")
		add_custom_target(${_pkg_name} ALL DEPENDS ${_output_files})
	else()
		message(FATAL_ERROR "${_pkg_type} is not a valid ext pkg type")
	endif()

	# dependent
	if(X_EXT_PKG_DEPENDS)
		add_dependencies(${_pkg_name} ${X_EXT_PKG_DEPENDS})
	endif()

	# export header
	if(X_EXT_PKG_EXPORT_HEADER)
		set_property(TARGET ${_pkg_name} PROPERTY XT_HEADER_DIR "${X_EXT_PKG_EXPORT_HEADER}")
	endif()
	set_property(TARGET ${_pkg_name} PROPERTY XT_IMPT_LOCATION "${_model_path}")
	set_property(TARGET ${_pkg_name} PROPERTY XT_IMPT_IMPLIB "${_lib}")
	set_property(TARGET ${_pkg_name} PROPERTY XT_PACKAGE_TYPE "EXTERN")

endmacro(x_extren_package)

macro(x_finish_project)
	_deal_package_link()
	_generate_make_bat()
endmacro(x_finish_project)

# add sub directory
macro(x_sub_dir _name)
	add_subdirectory(${_name})
endmacro(x_sub_dir _name)

# add definitions
macro(x_add_definitions)
	foreach(_d ${ARGN})
		add_definitions(-D${_d})
	endforeach()
endmacro(x_add_definitions)

# 链接一个或多个库
# x_link_packages(<lib1> [| <lib2> [| ... ]])
macro(x_link_packages)
	set(X_CUR_LINK_PACKAGES ${ARGN})
endmacro(x_link_packages)

# 启用MFG
# x_enable_mfc(<0|1|2>)
# 0: standard windows libraries
# 1: static MFC library
# 2: shared MFC library
macro(x_enable_mfc _lib_type)
	if(OS_WIN)
		set(CMAKE_MFC_FLAG ${_lib_type})
		if("${CMAKE_MFC_FLAG}" STREQUAL "1")
			_msvc_link_to_static_crt()
		elseif("${CMAKE_MFC_FLAG}" STREQUAL "2")
			add_definitions(-D_AFXDLL)
		else()
			# what to do ？
		endif()
	endif()
endmacro(x_enable_mfc)

#设置uac level
#x_uac_exe_level(<asInvoker|highestAvailable|requireAdministrator>)
macro(x_uac_exe_level _level)
	set(X_UAC_EXE_LEVEL ${_level})
endmacro(x_uac_exe_level)

