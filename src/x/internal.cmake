
if("${CMAKE_SYSTEM_NAME}" STREQUAL "Windows")
	set(OS_WIN TRUE)
elseif("${CMAKE_SYSTEM_NAME}" STREQUAL "Linux")
	set(OS_LINUX TRUE)
elseif("${CMAKE_SYSTEM_NAME}" STREQUAL "Darwin")
	set(OS_MAC TRUE)
else()
	message(FATAL_ERROR "Unknown platform: ${CMAKE_SYSTEM_NAME}!")
endif()

# out put
set(XG_OUTPUT_ROOT "${CMAKE_BINARY_DIR}/${CMAKE_PROJECT_NAME}")
set(XG_X_TOOLS "${CMAKE_BINARY_DIR}/x_tools")
set(XG_OUTPUT_ROOT_RELEASE "${XG_OUTPUT_ROOT}")
set(XG_OUTPUT_ROOT_DEBUG "${XG_OUTPUT_ROOT}")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${XG_OUTPUT_ROOT}/lib")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE "${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG "${XG_OUTPUT_ROOT_DEBUG}/lib")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${XG_OUTPUT_ROOT}/bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG "${XG_OUTPUT_ROOT_DEBUG}/bin")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${XG_OUTPUT_ROOT}/bin")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG "${XG_OUTPUT_ROOT_DEBUG}/bin")

# 全局数据
set(XG_TARGETS CACHE INTERNAL "all targets" FORCE)

macro(_ensure_tgt_exist _tgt_name)
	if (NOT TARGET ${_tgt_name})
		message(FATAL_ERROR "target \"${_tgt_name}\" not found!")
	endif()
endmacro(_ensure_tgt_exist)

macro(_def_target _tgt_name)
	foreach(_p ${XG_TARGETS})
		if (${_p} STREQUAL ${_tgt_name})
			message(FATAL_ERROR "target \"${_tgt_name}\" already defined!")
		endif()
	endforeach()
	set(XG_TARGETS ${XG_TARGETS} ${_tgt_name} CACHE INTERNAL "all targets" FORCE)
endmacro(_def_target)

# 查找是否含有某个选项
# example: _find_radio_option(values "R1|R2|R3" "R2" _result)
function(_find_radio_option _values _radio_options _default _output)

	set(${_output} "" PARENT_SCOPE)

	string(REGEX MATCHALL "[^|]+" _opt_list "${_radio_options}")

	set(_count 0)
	set(_data)

	foreach(_value ${_values})
		
		list(FIND _opt_list "${_value}" _index)
		if(NOT _index EQUAL -1)
			math(EXPR _count "${_count}+1")
			set(_data "${_value}")
		endif()
	endforeach()

	if(_count EQUAL 0)
		set(${_output} ${_default} PARENT_SCOPE)
	elseif(_count EQUAL 1)
		set(${_output} ${_data} PARENT_SCOPE)
	else()
		message(FATAL_ERROR "find more radio option!!!")
	endif()

endfunction()

macro(_append_target_property _target _property _value)
	get_target_property(_org_val ${_target} ${_property})
	if(_org_val)
		set_target_properties(${_target} PROPERTIES ${_property} "${_org_val} ${_value}")
	else()
		set_target_properties(${_target} PROPERTIES ${_property} "${_value}")
	endif()
endmacro()

macro(_create_export_header_ref _f _tgtn)
	set(_pub_h "${CMAKE_BINARY_DIR}/export_header/${_tgtn}/${_f}")
	_create_include_ref("${_f}" "${_pub_h}")
endmacro()

macro(_create_include_ref _f _pub_h)
	if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${_f}")
		message(FATAL_ERROR "${_f} does not exists!")
	endif()

	if(NOT EXISTS "${_pub_h}")
		get_filename_component(_pub_d "${_pub_h}" PATH)
		file(RELATIVE_PATH _relative_path "${_pub_d}" "${CMAKE_CURRENT_SOURCE_DIR}/${_f}")
		set(_cont "// This file is created automatically, please do not modify it!\n\n#include \"${_relative_path}\"\n\n")
		file(WRITE "${_pub_h}" "${_cont}")
	endif()
endmacro()

macro(_add_compale_flags _flag)
	set(X_COMPILE_FLAGS "${X_COMPILE_FLAGS} ${_flag}")
endmacro(_add_compale_flags)

macro(_add_ignorr_warn)
	set(_warns ${ARGN})
	if(OS_WIN)
		foreach(_w ${_warns})
			_add_compale_flags("/wd${_w}")
		endforeach()
	else()
		message("todo: ignore warning")
	endif()
endmacro(_add_ignorr_warn)

macro(_global_ignore_cmp_warn)
	_add_ignorr_warn(
		4819
	)
endmacro(_global_ignore_cmp_warn)

function(_x_find_env)
	if(NOT XG_ENV_PATH)
		if(OS_WIN)
			get_filename_component(
				XG_ENV_PATH 
				"[HKEY_CURRENT_USER\\Software\\kingsoft\\Office\\wpsenv;qt-kso-integration]" 
				ABSOLUTE 
				)
			if(NOT XG_ENV_PATH)
				message(FATAL_ERROR "Can not found XG_ENV_PATH!")
			endif()
			message("XG_ENV_PATH path: ${XG_ENV_PATH}")
			set(XG_ENV_PATH "${XG_ENV_PATH}" CACHE PATH "env path")
		else()
			message(FATAL_ERROR "todo: XG_ENV_PATH")
		endif()
	endif()
endfunction()

function(_x_find_qt)
	if(NOT XG_ENV_PATH)
		_x_find_env()
		set(_path "${XG_ENV_PATH}/3rdparty/qt")
		if(EXISTS "${_path}")
			set(X_QT_PATH "${_path}" CACHE FILEPATH "qt path")
		else()
			message(FATAL_ERROR "Can not found ${_path}")
		endif()
	endif()
endfunction()

#_x_find_qt()

FUNCTION(_x_find_cl)
	IF(NOT XG_CL_PATH)
		set(XG_CL_PATH "cl.exe" CACHE FILEPATH "cl path")
	#	EXECUTE_PROCESS(COMMAND "where" "cl.exe"
	#		TIMEOUT 10
	#		RESULT_VARIABLE _res_val
	#		OUTPUT_VARIABLE _out_val
	#		ERROR_QUIET
	#		#ERROR_VARIABLE _err_val
	#		)
	#	IF("${_res_val}!" STREQUAL "0!")
	#		set(XG_CL_PATH "${_out_val}" CACHE FILEPATH "cl path")
	#		#MESSAGE("${XG_CL_PATH}")
	#	ELSE()
	#		message(FATAL_ERROR "Can not found cl.exe")
	#	ENDIF()
	ENDIF()
ENDFUNCTION()

function(_x_find_dumpbin)
	IF(NOT XG_DUMPBIN_PATH)
		EXECUTE_PROCESS(COMMAND "where" "dumpbin.exe"
			TIMEOUT 10
			RESULT_VARIABLE _res_val
			OUTPUT_VARIABLE _out_val
			ERROR_QUIET
			#ERROR_VARIABLE _err_val
			)
		IF("${_res_val}!" STREQUAL "0!")
			set(XG_DUMPBIN_PATH
			"dumpbin.exe"#"${_out_val}"
			CACHE FILEPATH "dumpbin path")
			#MESSAGE("${XG_DUMPBIN_PATH}")
		ELSE()
			message(FATAL_ERROR "Can not found dumpbin.exe")
		ENDIF()
	ENDIF()
endfunction(_x_find_dumpbin)


FUNCTION(_x_find_symbal_tool)
	IF(NOT XG_SYMBAL_TOOL OR
		NOT EXISTS "${XG_SYMBAL_TOOL}" OR
		"${XG_CMAKE_DIR}/tools/gem_symbal.cpp" IS_NEWER_THAN "${XG_SYMBAL_TOOL}")
			IF(NOT EXISTS "${XG_X_TOOLS}/st")
				file(MAKE_DIRECTORY "${XG_X_TOOLS}/st")
			ENDIF()
			_x_find_cl()
			message("build st.exe")
			EXECUTE_PROCESS(COMMAND "${XG_CL_PATH}" "/EHsc" "/O2" "/Ot" "/Ox" "${XG_CMAKE_DIR}/tools/gem_symbal.cpp" "/Fe${XG_X_TOOLS}/st.exe" "/Fd${XG_X_TOOLS}/st.pdb"
				TIMEOUT 10
				WORKING_DIRECTORY "${XG_X_TOOLS}/st"
				RESULT_VARIABLE _res_val
				OUTPUT_VARIABLE _out_val
				ERROR_VARIABLE _err_val
				#OUTPUT_QUIET
				#ERROR_QUIET
				)
			#MESSAGE("${_res_val}")
			#MESSAGE("${_out_val}")
			#MESSAGE("${_err_val}")
			if(NOT "${_res_val}!" STREQUAL "0!")
				#message(FATAL_ERROR "${_res_val}")
				message(FATAL_ERROR "${_out_val}")
			endif()
		ENDIF()

		IF(NOT EXISTS "${XG_X_TOOLS}/st.exe")
			message(FATAL_ERROR "Can not build st.exe")
		ELSE()
			set(XG_SYMBAL_TOOL "${XG_X_TOOLS}/st.exe" CACHE FILEPATH "st path")
			#ssMESSAGE("${XG_SYMBAL_TOOL}")
		ENDIF()
ENDFUNCTION(_x_find_symbal_tool)

macro(_generate_make_bat)
	if(CMAKE_GENERATOR MATCHES "^Visual Studio")
		configure_file("${XG_CMAKE_DIR}/template/make.bat.in" "${CMAKE_BINARY_DIR}/make.bat" @ONLY)
	endif()
endmacro()

macro(_get_target_impt_lib _tgt_name _impt_lib)
	get_property(_tgt_type TARGET ${_tgt_name} PROPERTY XT_TARGET_TYPE)
	if (NOT _tgt_type)
		message(FATAL_ERROR "${_tgt_name}'type not found")
	endif()

	if("${_tgt_type}" STREQUAL "EXTERN")
		get_property(${_impt_lib} TARGET ${_tgt_name} PROPERTY XT_IMPT_IMPLIB)
	else()
		if(OS_WIN)
			set(${_impt_lib} "${CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${X_BUILD_CFG}}/${_tgt_name}.lib")
		elseif(OS_LINUX)
			if("${_tgt_type}" STREQUAL "SHARED")
				set(${_impt_lib} "${CMAKE_LIBRARY_OUTPUT_DIRECTORY_${X_BUILD_CFG}}/lib${_tgt_name}.so")
			elseif("${_tgt_type}" STREQUAL "STATIC")
				set(${_impt_lib} "${CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${X_BUILD_CFG}}/$lib{_tgt_name}.a")
			else()
				message(FATAL_ERROR "can't link to executable target ${_tgt_name}")
			endif()
		elseif(OS_MAC)
			if("${_tgt_type}" STREQUAL "SHARED")
				set(${_impt_lib} "${CMAKE_LIBRARY_OUTPUT_DIRECTORY_${X_BUILD_CFG}}/lib${_tgt_name}.dylib")
			elseif("${_tgt_type}" STREQUAL "STATIC")
				set(${_impt_lib} "${CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${X_BUILD_CFG}}/$lib{_tgt_name}.a")
			else()
				message(FATAL_ERROR "can't link to executable target ${_tgt_name}")
			endif()
		endif()
	endif()

endmacro(_get_target_impt_lib)

#处理链接依赖关系
macro(_deal_target_link)
	foreach(_tgt ${XG_TARGETS})
		get_property(_link_tgts TARGET ${_tgt} PROPERTY XT_LINK_TARGETS)
		if (NOT _link_tgts)
			#continue()
		else()
			set(_link_arg)
			set(_dep_arg)
			foreach(_lp ${_link_tgts})
				_ensure_tgt_exist(${_lp})
				_get_target_impt_lib(${_lp} _impt_lib)
				set(_link_arg "${_link_arg} ${_impt_lib} ")
				set(_dep_arg "${_dep_arg}${_impt_lib};")
				get_property(_sym_file TARGET ${_lp} PROPERTY XT_SYMPBAL_FILE)
				if(_sym_file)
					source_group("ImpSymbalFile" FILES ${_sym_file})
					#get_property(_src TARGET ${_tgt} PROPERTY SOURCES)
					#list(APPEND _src ${_sym_file})
					set_property(TARGET ${_tgt} APPEND PROPERTY SOURCES ${_sym_file})
					#target_sources(${_tgt} PRIVATE  ${_sym_file})
				endif()
				#set_property(TARGET ${X_TARGET_NAME} PROPERTY XT_SYMPBAL_FILE "${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}/${X_TARGET_NAME}.c")
			endforeach()
			_append_target_property(${_tgt} LINK_FLAGS ${_link_arg})
			set_property(TARGET ${_tgt} APPEND PROPERTY LINK_DEPENDS ${_dep_arg})
			add_dependencies(${_tgt} ${_link_tgts})
		endif()
	endforeach()
endmacro(_deal_target_link)

# for mfc
macro(replace_flags var these those)
	if("${${var}}" MATCHES "${these}")
		string(REGEX REPLACE "${these}" "${those}" ${var} "${${var}}")
		#message(STATUS "info: ${var} changed to '${${var}}'")
	endif()
	#message(STATUS "info: ${var}='${${var}}'")
endmacro()

macro(_msvc_link_to_static_crt)
	if(MSVC)
		set(has_correct_flag 0)
		foreach(lang C CXX)
			foreach(suffix "" _DEBUG _MINSIZEREL _RELEASE _RELWITHDEBINFO)
				replace_flags("CMAKE_${lang}_FLAGS${suffix}" "/MD" "/MT")
				if(CMAKE_${lang}_FLAGS${suffix} MATCHES "/MT")
					set(has_correct_flag 1)
				endif()
			endforeach()
		endforeach()
		if(NOT has_correct_flag)
			message(FATAL_ERROR "no CMAKE_*_FLAGS var contains /MT")
		endif()
	endif()
endmacro()

macro(_group_src_files_by_dir)
	foreach(_file ${ARGN})
		if(IS_ABSOLUTE ${_file})
			file(RELATIVE_PATH _path ${CMAKE_CURRENT_SOURCE_DIR} ${_file})
		endif()

		get_filename_component(_path ${_file} PATH)
		file(TO_NATIVE_PATH "${_path}" _path)
		source_group("${_path}" FILES ${_file})
	endforeach()
endmacro(_group_src_files_by_dir)

macro(_get_pch_header _pch_hd_out _pch_src_out _argn_out)
	set(_argn ${ARGN})

	list(GET _argn 0 _src)
	if(NOT "${_src}!" STREQUAL "(!")
		message(FATAL_ERROR "invalid pch declare ${_src}")
	endif()
	list(REMOVE_AT _argn 0)

	while(NOT "${_argn}" STREQUAL "")
		list(GET _argn 0 _src)
		list(REMOVE_AT _argn 0)
		if("${_src}" STREQUAL ")")
			break()
		elseif(${_src} MATCHES "(\\.h$)")
			if(${_pch_hd_out})
				message(FATAL_ERROR "too many pch header files")
			endif()
			set(${_pch_hd_out} ${_src})
		elseif(${_src} MATCHES "(\\.cpp$|\\.cc$|\\.cxx$)")
			if(${_pch_hd_out})
				message(FATAL_ERROR "too many pch src files")
			endif()
			set(${_pch_src_out} ${_src})
		else()
			message(FATAL_ERROR "invalid pch file ${_src}")
		endif()
	endwhile()

	if(NOT ${_pch_src_out} OR NOT ${_pch_hd_out})
		message(FATAL_ERROR "pch not found")
	endif()

	set(${_argn_out} ${_argn})
endmacro(_get_pch_header)

macro(_set_pch_ref)
	set(_srcs ${ARGN})
	if(X_PCH_HEADER)
		list(GET X_PCH_HEADER 0 _pch_header)
		list(GET X_PCH_HEADER 1 _pch_src)

		if(MSVC AND CMAKE_GENERATOR MATCHES "^Visual Studio")
			foreach(_file ${_srcs})
				if(_file MATCHES "(\\.cpp$|\\.cc$|\\.cxx$)")
					set_property(SOURCE "${_file}" PROPERTY COMPILE_FLAGS "/Yu\"${_pch_header}\"")
				endif()
			endforeach()
			set_property(SOURCE "${_pch_src}" PROPERTY COMPILE_FLAGS "/Yc\"${_pch_header}\"")
		endif()
	endif()
endmacro(_set_pch_ref)
