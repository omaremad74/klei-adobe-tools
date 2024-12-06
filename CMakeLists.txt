cmake_minimum_required(VERSION 3.2)

project("KFormatLib")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(PACKAGE_VERSION_MAJOR 0)
set(PACKAGE_VERSION_MINOR 2)
set(PACKAGE_VERSION_PATCH 1)
set(PACKAGE_VERSION "${PACKAGE_VERSION_MAJOR}.${PACKAGE_VERSION_MINOR}.${PACKAGE_VERSION_PATCH}")

ENABLE_LANGUAGE(CXX)

set( INCLUDE_DIRS "" )
set( LIBRARIES "kleiformat" "squish" "zip" ) # "libzip"

set( KADOBE_INCLUDE_DIRS "" )
set( KADOBE_LIBRARIES "" )
set( KANIMVIEWER_INCLUDE_DIRS "" )
set( KANIMVIEWER_LIBRARIES "" )

set(CMAKE_BUILD_TYPE Release)

find_package(ImageMagick COMPONENTS Magick++ MagickWand MagickCore REQUIRED)
#find_package(SDL2 REQUIRED)

set( ZLIB_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/lib/zlib )
set( ZLIB_USE_STATIC_LIBS ON )

list( APPEND INCLUDE_DIRS "${ImageMagick_INCLUDE_DIRS}" )
list( APPEND LIBRARIES "${ImageMagick_LIBRARIES}" )

list( APPEND KADOBE_LIBRARIES "${LIBRARIES}" )
list( APPEND KANIMVIEWER_LIBRARIES "${LIBRARIES}" )

list( APPEND INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/src/kleiformat ${PROJECT_SOURCE_DIR}/lib ${PROJECT_SOURCE_DIR}/lib/libzip )

list( APPEND KADOBE_INCLUDE_DIRS "${INCLUDE_DIRS}" )
list( APPEND KANIMVIEWER_INCLUDE_DIRS "${INCLUDE_DIRS}" ) # "lib/imgui"

add_subdirectory(src)
add_subdirectory(lib)

if(NOT WIN32)
	#add_executable(kanimviewer ${KANIMVIEWER_SOURCES} ${KANIMVIEWER_HEADERS})
else()
	#add_executable(kanimviewer WIN32 ${KANIMVIEWER_SOURCES} ${KANIMVIEWER_HEADERS})
endif()
add_library(kadobe SHARED ${KADOBE_SOURCES} ${KADOBE_HEADERS})
add_library(kleiformat STATIC ${KLEIFORMAT_SOURCES} ${KLEIFORMAT_HEADERS})
#add_library(imgui STATIC
#	"lib/imgui/imgui.cpp"
#	"lib/imgui/imgui_draw.cpp"
#	"lib/imgui/imgui_tables.cpp"
#	"lib/imgui/imgui_widgets.cpp"
#	"lib/imgui/backends/imgui_impl_sdl2.cpp"
#	"lib/imgui/backends/imgui_impl_opengl3.cpp"
#)

#target_include_directories(imgui PUBLIC "lib/imgui")

target_link_libraries( kadobe ${KADOBE_LIBRARIES} )
#target_link_libraries( kanimviewer ${KANIMVIEWER_LIBRARIES} )
#target_link_libraries( imgui ${KANIMVIEWER_LIBRARIES} )

set_property( TARGET kadobe PROPERTY INCLUDE_DIRECTORIES "${KADOBE_INCLUDE_DIRS}" )
#set_property( TARGET kanimviewer PROPERTY INCLUDE_DIRECTORIES "${KANIMVIEWER_INCLUDE_DIRS}" )
set_property( TARGET kleiformat PROPERTY INCLUDE_DIRECTORIES "${INCLUDE_DIRS}" )

if(MSVC)
   #set_target_properties(kanimviewer PROPERTIES LINK_FLAGS_DEBUG "/SUBSYSTEM:CONSOLE")
   #set_target_properties(kanimviewer PROPERTIES COMPILE_DEFINITIONS_DEBUG "_CONSOLE")
   #set_target_properties(kanimviewer PROPERTIES LINK_FLAGS_RELWITHDEBINFO "/SUBSYSTEM:CONSOLE")
   #set_target_properties(kanimviewer PROPERTIES COMPILE_DEFINITIONS_RELWITHDEBINFO "_CONSOLE")
   #set_target_properties(kanimviewer PROPERTIES LINK_FLAGS_RELEASE "/SUBSYSTEM:CONSOLE")
   #set_target_properties(kanimviewer PROPERTIES COMPILE_DEFINITIONS_RELEASE "_CONSOLE")
   #set_target_properties(kanimviewer PROPERTIES LINK_FLAGS_MINSIZEREL "/SUBSYSTEM:CONSOLE")
   #set_target_properties(kanimviewer PROPERTIES COMPILE_DEFINITIONS_MINSIZEREL "_CONSOLE")
endif(MSVC)

#add_subdirectory
#target_include_directories
#target_link_libraries

# Packing Stuff
include(InstallRequiredSystemLibraries)
set(CPACK_PACKAGE_VERSION_MAJOR "${PACKAGE_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${PACKAGE_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${PACKAGE_VERSION_PATCH}")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Klei Format Adobe Tools ")
set(CPACK_PACKAGE_VENDOR "Omar A.")
set(CPACK_GENERATOR "TGZ")
set(CPACK_SOURCE_PACKAGE_FILE_NAME "${CMAKE_PROJECT_NAME}-${PACKAGE_VERSION}")
set(CPACK_SOURCE_GENERATOR "TGZ")
# The following components are regex's to match anywhere (unless anchored)
# in absolute path + filename to find files or directories to be excluded
# from source tarball.
set(CPACK_SOURCE_IGNORE_FILES
"~$"
"/[.].+$"
"^${CMAKE_CURRENT_SOURCE_DIR}/local/"
"^${CMAKE_CURRENT_SOURCE_DIR}/deps/"
"^${CMAKE_CURRENT_SOURCE_DIR}/build/"
"^${CMAKE_CURRENT_SOURCE_DIR}/config.h$"
"/cmake_install.cmake$"
"/CMakeFiles/"
"/Makefile$"
"^${CMAKE_BINARY_DIR}/.*"
)
include(CPack)