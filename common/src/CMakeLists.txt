cmake_minimum_required(VERSION 2.8)

set(COMMON_SOURCES
	constants.cpp
	formatting.cpp
	image_info.cpp
	memory.cpp
	serialization.cpp
	string.cpp
	symbol_resolver.cpp
	time.cpp
)

if (WIN32)
	set(COMMON_SOURCES ${COMMON_SOURCES}
		image_info_win32.cpp
		memory_win32.cpp
		module_win32.cpp
		time_win32.cpp
	)	
elseif (UNIX)
	set(COMMON_SOURCES ${COMMON_SOURCES}
		elf/filemapping_unix.cpp
		elf/sym-elf.cpp
		image_info_unix.cpp
		memory_unix.cpp
		module_linux.cpp
		time_generic.cpp
	)		
endif()

add_library(common STATIC ${COMMON_SOURCES})

if (WIN32)
	target_link_libraries(common dbghelp)
elseif (UNIX)
	target_link_libraries(common dl)
endif()
