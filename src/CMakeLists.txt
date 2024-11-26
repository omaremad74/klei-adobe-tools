set( local_KADOBE_SOURCES 
	kadobe/adobe_main.cpp
)

set( local_KADOBE_HEADERS
	kadobe/adobe_main.hpp kadobe/jsapi.hpp
)

#

set( local_KANIMVIEWER_SOURCES
	kanimviewer/kanimviewer.cpp
)

set( local_KANIMVIEWER_HEADERS
	kanimviewer/kanimviewer.hpp
)


set( local_KLEIFORMAT_SOURCES
	kleiformat/anim.cpp
    kleiformat/build.cpp
    kleiformat/ktex.cpp
    kleiformat/xfl.cpp
    kleiformat/tinyxml2.cpp
    kleiformat/logger.cpp
)

set( local_KLEIFORMAT_HEADERS
    kleiformat/anim.hpp
    kleiformat/binary_util.hpp
    kleiformat/build.hpp
    kleiformat/common.hpp
    kleiformat/ktex.hpp
    kleiformat/strings.hpp
    kleiformat/xfl.hpp
    kleiformat/tinyxml2.h
    kleiformat/logger.hpp
)

set( prefixed_KADOBE_SOURCES )
set( prefixed_KADOBE_HEADERS )

foreach(src ${local_KADOBE_SOURCES})
	list(APPEND prefixed_KADOBE_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/${src})
endforeach()
foreach(hdr ${local_KADOBE_HEADERS})
	list(APPEND prefixed_KADOBE_HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/${hdr})
endforeach()


set( prefixed_KANIMVIEWER_SOURCES )
set( prefixed_KANIMVIEWER_HEADERS )

foreach(src ${local_KANIMVIEWER_SOURCES})
	list(APPEND prefixed_KANIMVIEWER_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/${src})
endforeach()
foreach(hdr ${local_KANIMVIEWER_HEADERS})
	list(APPEND prefixed_KANIMVIEWER_HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/${hdr})
endforeach()


set( prefixed_KLEIFORMAT_SOURCES )
set( prefixed_KLEIFORMAT_HEADERS )

foreach(src ${local_KLEIFORMAT_SOURCES})
	list(APPEND prefixed_KLEIFORMAT_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/${src})
endforeach()
foreach(hdr ${local_KLEIFORMAT_HEADERS})
	list(APPEND prefixed_KLEIFORMAT_HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/${hdr})
endforeach()


set( KADOBE_SOURCES "${prefixed_KADOBE_SOURCES}" PARENT_SCOPE )
set( KADOBE_HEADERS "${prefixed_KADOBE_HEADERS}" PARENT_SCOPE )

set( KANIMVIEWER_SOURCES "${prefixed_KANIMVIEWER_SOURCES}" PARENT_SCOPE )
set( KANIMVIEWER_HEADERS "${prefixed_KANIMVIEWER_HEADERS}" PARENT_SCOPE )

set( KLEIFORMAT_SOURCES "${prefixed_KLEIFORMAT_SOURCES}" PARENT_SCOPE )
set( KLEIFORMAT_HEADERS "${prefixed_KLEIFORMAT_HEADERS}" PARENT_SCOPE )