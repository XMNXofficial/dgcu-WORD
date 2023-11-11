cmake_minimum_required (VERSION 3.12)
set(NFD_BUILD_TESTS 1)

# 如果支持，请为 MSVC 编译器启用热重载。
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#解决奇数个数中文在msvc上无法编译utf-8不带bom的代码文件
add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")

project ("dgcu_WORD")

#导入第三方库
	#导入hello_imgui
		add_subdirectory(externalLib/hello_imgui)
		list(APPEND CMAKE_MODULE_PATH ${HELLOIMGUI_CMAKE_PATH})
		include(hello_imgui_add_app)
	#导入nfd
		add_subdirectory(externalLib/nfd)

#导入模块
	add_subdirectory(modules)



#将源代码添加到此项目的可执行文件。
	hello_imgui_add_app (${PROJECT_NAME} "main.cpp" "main.hpp" "res.h" "res.rc")




#链接第三方库
	#native-file-dialog-extended
		target_include_directories(${PROJECT_NAME} PUBLIC externalLib/nfd)
		target_link_libraries(${PROJECT_NAME} PUBLIC nfd)

#链接模块
	#core
		target_include_directories(${PROJECT_NAME} PUBLIC modules/core)
		target_link_libraries(${PROJECT_NAME} PUBLIC core)
	#extern
		target_include_directories(${PROJECT_NAME} PUBLIC modules/extern)
		target_link_libraries(${PROJECT_NAME} PUBLIC extern)

