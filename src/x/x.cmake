
#属性名规范
#XT_xxxx: 设置到TARGET上的属性
#XG_xxxx: 全局属性

#cmake代码目录
set(XG_CMAKE_DIR ${CMAKE_CURRENT_LIST_DIR})

# 导入参数解释函数
include(CMakeParseArguments)

include("${XG_CMAKE_DIR}/internal.cmake")

# x_target_begin(<name> [STATIC|SHARED|EXECUTABLE|CONSOLE])
# 开始定义一个工程
macro(x_target_begin tgt_name)

	_def_target(${tgt_name})

	set(tgt_types "STATIC|SHARED|EXECUTABLE|CONSOLE")

	cmake_parse_arguments(_ARG "" "" ""  ${ARGN})
	set(_argn ${_ARG_UNPARSED_ARGUMENTS})

	_find_radio_option(${_argn} ${tgt_types} "EXECUTABLE" tgt_type)

	if("${tgt_type}" STREQUAL "")
		message(FATAL_ERROR "could not find the target type")
	endif()

	set(X_TARGET_NAME "${tgt_name}")
	set(X_TARGET_TYPE "${tgt_type}")
	set(X_HAS_EXPORT_HEADER)
	set(X_TARGET_SOURCES)
	set(X_COMPILE_FLAGS)
	set(X_CUR_LINK_TARGETS) #当前工程需要链接的库
	set(X_EXECUTABLE_ENTRYPOINT "main")
	set(X_PCH_HEADER)

	# c++11
	if(${OS_LINUX})
		add_compile_options(-std=c++11)
	endif()

	#include_directories(${CMAKE_CURRENT_BINARY_DIR})
	include_directories(${CMAKE_CURRENT_SOURCE_DIR})
	include_directories("${CMAKE_BINARY_DIR}/export_header")

endmacro(x_target_begin)

# 结束一个工程的定义
macro(x_target_end)

	if (DEFINED CMAKE_MFC_FLAG)
		unset(X_EXECUTABLE_ENTRYPOINT)
	endif()

	set(_src_to_compile ${X_TARGET_SOURCES})
	_group_src_files_by_dir(${_src_to_compile})
	_set_pch_ref(${_src_to_compile})

	if("${X_TARGET_TYPE}" STREQUAL "STATIC")
		add_library(${X_TARGET_NAME} STATIC ${X_TARGET_SOURCES})
	elseif("${X_TARGET_TYPE}" STREQUAL "SHARED")
		add_library(${X_TARGET_NAME} SHARED ${X_TARGET_SOURCES})
	elseif("${X_TARGET_TYPE}" STREQUAL "EXECUTABLE")
		if(OS_WIN)
			add_executable(${X_TARGET_NAME} WIN32 ${X_TARGET_SOURCES})
			if(X_EXECUTABLE_ENTRYPOINT)
				_append_target_property(${X_TARGET_NAME} LINK_FLAGS " /ENTRY:\"${X_EXECUTABLE_ENTRYPOINT}CRTStartup\" ")
			endif()
		else()
			add_executable(${X_TARGET_NAME} ${X_TARGET_SOURCES})
		endif()
	elseif("${X_TARGET_TYPE}" STREQUAL "CONSOLE")
		add_executable(${X_TARGET_NAME} ${X_TARGET_SOURCES})
	else()
		message(FATAL_ERROR "invalid target type")
	endif()

	if(OS_WIN AND DEFINED X_UAC_EXE_LEVEL)
		if(("${X_TARGET_TYPE}" STREQUAL "EXECUTABLE") OR ("${X_TARGET_TYPE}" STREQUAL "CONSOLE"))
			_append_target_property(${X_TARGET_NAME} LINK_FLAGS " /level='${X_UAC_EXE_LEVEL}' ")
		else()
			message(WARNING "UAC level should set to executable target")
		endif()
	endif()

	if(X_GEN_SYMBAL_FILE AND ("${X_TARGET_TYPE}" STREQUAL "STATIC"))
		_x_find_symbal_tool()
		_x_find_dumpbin()
		set(_symb_path ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}/${X_TARGET_NAME}.c)
		add_custom_command(TARGET ${X_TARGET_NAME} POST_BUILD
			COMMAND "${XG_SYMBAL_TOOL}"
			"-lib"
			${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}/${X_TARGET_NAME}.lib
			"-out"
			${_symb_path}
			"-dumpbin"
			${XG_DUMPBIN_PATH}
			COMMENT "genarate symbal file"
		)
		set_property(TARGET ${X_TARGET_NAME} PROPERTY XT_SYMPBAL_FILE "${_symb_path}")
		if(NOT EXISTS "${_symb_path}")
			set(_cont "// This file is created automatically\n")
			file(WRITE "${_symb_path}" "${_cont}")
		endif()
	endif()

	_global_ignore_cmp_warn()
	_append_target_property(${X_TARGET_NAME} COMPILE_FLAGS ${X_COMPILE_FLAGS})

	if(X_CUR_LINK_TARGETS)
		set_property(TARGET ${X_TARGET_NAME} PROPERTY XT_LINK_TARGETS ${X_CUR_LINK_TARGETS})
	endif()
	set_property(TARGET ${X_TARGET_NAME} PROPERTY XT_TARGET_TYPE ${X_TARGET_TYPE})
endmacro(x_target_end)

# 定义工程源代码文件
# x_target_sources([PCH (header.h [source<.cpp|.cc|.cxx>])]
#					 a.h b.c c.cpp ...
#)
macro(x_target_sources)

	set(_argn ${ARGN})
	while(NOT "${_argn}" STREQUAL "")
		list(GET _argn 0 _src)
		list(REMOVE_AT _argn 0)
		if("${_src}" MATCHES "(PCH)")
			_get_pch_header(_pch_header _pch_src _argn ${_argn})
			list(APPEND X_PCH_HEADER ${_pch_header} ${_pch_src})
			list(APPEND X_TARGET_SOURCES ${_pch_header} ${_pch_src})
		else()
			string(TOLOWER "${_src}" _src)
			if(NOT ${_src} MATCHES "(\\.c|\\.cpp|\\.h|\\.rc|\\.def)")
				message(FATAL_ERROR "Unrecognized source file ${_f}")
			endif()
			list(APPEND X_TARGET_SOURCES ${_src})
		endif()
	endwhile()
endmacro(x_target_sources)

# ignore warnings
macro(x_ignorr_warn)
	_add_ignorr_warn(${ARGN})
endmacro(x_ignorr_warn)

macro(x_export_header)
	foreach(_f ${ARGN})
		_create_export_header_ref(${_f} ${X_TARGET_NAME})
	endforeach()
endmacro(x_export_header)

# x_extren_target(_tgt_name _tgt_type
#			<LOCATION _location>
# )
macro(x_extren_target _tgt_name _tgt_type)
	_def_target(${_tgt_name})

	set(_args ${ARGN})
	cmake_parse_arguments(
		X_EXT_TGT
		""
		"LOCATION;IMPT_LOCATION"
		"EXPORT_HEADER;DEPENDS;BINARY_NAMES;BINARY_NAMES_DEBUG;BINARY_NAMES_RELEASE"
		${_args}
	)

	# output dir
	set(_outputdir "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")

	# import location
	if(NOT X_EXT_TGT_IMPT_LOCATION)
		set(X_EXT_TGT_IMPT_LOCATION ${X_EXT_TGT_LOCATION})
	endif()

	# file names
	if(X_EXT_TGT_BINARY_NAMES)
		set(_names ${X_EXT_TGT_BINARY_NAMES})
	else()
		set(_names ${X_EXT_TGT_BINARY_NAMES_${X_BUILD_CFG}})
	endif()

	set(_output_files)
	foreach(_name ${_names})
		if(OS_WIN)
			set(_model_path "${X_EXT_TGT_LOCATION}/${_name}.dll")
			set(_lib "${X_EXT_TGT_IMPT_LOCATION}/${_name}.lib")
		elseif(OS_LINUX)
			set(_so_file_name "lib${_name}.so")
			set(_model_path "${X_EXT_TGT_LOCATION}/${_so_file_name}")
			set(_lib ${_model_path})
		else()
			message(FATAL_ERROR "invalid os")
		endif()

		if("${_tgt_type}!" STREQUAL "SHARED!")
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
				WORKING_DIRECTORY ${_outputdir}
				MAIN_DEPENDENCY "${_model_path}"
			)

			list(APPEND _output_files ${_outputdir}/${_filename})
		endif()
	endforeach()

	# target type
	if("${_tgt_type}!" STREQUAL "STATIC!")
		add_custom_target(${_tgt_name})
	elseif("${_tgt_type}!" STREQUAL "SHARED!")
		add_custom_target(${_tgt_name} ALL DEPENDS ${_output_files})
	else()
		message(FATAL_ERROR "${_tgt_type} is not a valid ext target type")
	endif()

	# dependent
	if(X_EXT_TGT_DEPENDS)
		add_dependencies(${_tgt_name} ${X_EXT_TGT_DEPENDS})
	endif()

	# export header
	if(X_EXT_TGT_EXPORT_HEADER)
		set_property(TARGET ${_tgt_name} PROPERTY XT_HEADER_DIR "${X_EXT_TGT_EXPORT_HEADER}")
	endif()
	set_property(TARGET ${_tgt_name} PROPERTY XT_IMPT_LOCATION "${_model_path}")
	set_property(TARGET ${_tgt_name} PROPERTY XT_IMPT_IMPLIB "${_lib}")
	set_property(TARGET ${_tgt_name} PROPERTY XT_TARGET_TYPE "EXTERN")

endmacro(x_extren_target)

macro(x_finish_project)
	_deal_target_link()
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
# x_link_targets(<lib1> [| <lib2> [| ... ]])
macro(x_link_targets)
	set(X_CUR_LINK_TARGETS ${ARGN})
endmacro(x_link_targets)

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
	if(OS_WIN)
		set(X_UAC_EXE_LEVEL ${_level})
	endif()
endmacro(x_uac_exe_level)

#启用 include符号
macro(x_gen_symbal_file)
	if(OS_WIN)
		set(X_GEN_SYMBAL_FILE true)
	endif()
endmacro(x_gen_symbal_file)


