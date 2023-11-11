cmake_minimum_required (VERSION 3.12)

project(core)

set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS TRUE)#开启dll符号导出
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})#设置dll输出目录

add_library(core SHARED core.cpp core.hpp)

#vcpkg里的第三方包
	
	set(VCPKG_ROOT D:/Dev_Libs/vcpkg )
	set(VCPKG_BIN ${VCPKG_ROOT}/installed/x64-windows/bin)
	include(${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake)

	find_package(unofficial-minizip CONFIG REQUIRED)
	target_link_libraries(core PUBLIC unofficial::minizip::minizip)

	set(DLL_PATH_zlib ${VCPKG_BIN}/zlib1.dll)
	set(DLL_PATH_minizip ${VCPKG_BIN}/minizip.dll)

	install(FILES ${DLL_PATH_zlib} DESTINATION ${CMAKE_BINARY_DIR})
	install(FILES ${DLL_PATH_minizip} DESTINATION ${CMAKE_BINARY_DIR})


	include(FindIconv) # Attempted without this line also
	find_package(Iconv)
	message("找到了吗:"${Iconv_FOUND})
	target_include_directories(${PROJECT_NAME} PUBLIC ${Iconv_INCLUDE_DIRS})
	target_link_libraries(${PROJECT_NAME} PUBLIC ${Iconv_LIBRARIES})#D:/Dev_Libs/vcpkg/installed/x64-windows/debug/lib/iconv.lib
	string(REPLACE "debug/lib/iconv.lib" "debug/bin/charset-1.dll" DLL_PATH_charset ${Iconv_LIBRARY})#变量:DLL_PATH_charset
	string(REPLACE "debug/lib/iconv.lib" "debug/bin/iconv-2.dll" DLL _PATH_iconv ${Iconv_LIBRARY})#变量:DLL_PATH_iconv
	install(FILES ${DLL_PATH_charset} DESTINATION ${CMAKE_BINARY_DIR})
	install(FILES ${DLL_PATH_iconv} DESTINATION ${CMAKE_BINARY_DIR})