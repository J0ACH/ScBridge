project (ScBridge)

MESSAGE(STATUS "SCBRIDGE")
MESSAGE(STATUS "SCBRIDGE SOURCE DIR" ${CMAKE_CURRENT_SOURCE_DIR})
MESSAGE(STATUS "SCBRIDGE BINARY DIR" ${CMAKE_CURRENT_BINARY_DIR})

SET(SC_SRC_PATH "C:/Supercollider/Supercollider_src" CACHE PATH "Supercollider source path")
SET(SC_BUILD_PATH "C:/Supercollider/Supercollider_build" CACHE PATH "Supercollider build path")
	
#SET(SC_LANG_PATH "${SC_BUILD_PATH}/Lang/Release/libsclang.lib")

FIND_PACKAGE(Qt5Core)
FIND_PACKAGE(Qt5Widgets)
FIND_PACKAGE(Qt5LinguistTools)
FIND_PACKAGE(Qt5WebKitWidgets)
FIND_PACKAGE(Qt5OpenGL)
FIND_PACKAGE(Qt5PrintSupport)
FIND_PACKAGE(Qt5Sensors)
FIND_PACKAGE(Qt5Quick)
FIND_PACKAGE(Qt5Qml)
FIND_PACKAGE(Qt5Sql)
FIND_PACKAGE(Qt5Positioning)

SET(QT_ScBridge_LIBRARIES 
	Qt5::Core
	Qt5::Gui 
	Qt5::Widgets
	Qt5::Network
	Qt5::WebKit
	Qt5::WebKitWidgets
	Qt5::OpenGL 
	Qt5::PrintSupport 
	Qt5::Sensors 
	Qt5::Quick 
	Qt5::Qml 
	Qt5::Sql 
	Qt5::Positioning
)

#for header location
INCLUDE_DIRECTORIES(
	${CMAKE_SOURCE_DIR}
	
	${SC_SRC_PATH}
	${SC_SRC_PATH}/editors/sc-ide/core
	${SC_SRC_PATH}/editors/sc-ide/core/settings
	${SC_SRC_PATH}/editors/sc-ide/core/util
	${SC_SRC_PATH}/editors/sc-ide/primitives
	${SC_SRC_PATH}/editors/sc-ide/widgets	
	
	${SC_SRC_PATH}/external_libraries/boost
	${SC_SRC_PATH}/external_libraries/yaml-cpp-0.3.0/include
	${SC_SRC_PATH}/external_libraries/oscpack_1_1_0
	
	${SC_SRC_PATH}/include/common
	${SC_SRC_PATH}/include/lang
	${SC_SRC_PATH}/include/server
	${SC_SRC_PATH}/include/plugin_interface
	
	${SC_SRC_PATH}/include/plugin_interface
)

#for library location
LINK_DIRECTORIES( 
	${SC_BUILD_PATH}/Lang/Release
	${SC_BUILD_PATH}/server/scsynth/Release
	${SC_BUILD_PATH}/server/plugins/Release
	${SC_BUILD_PATH}/external_libraries/Release
)  


FILE(GLOB_RECURSE SC_INCLUDE_HEADERS ${SC_SRC_PATH}/include/*.h* )
FILE(GLOB_RECURSE SC_EXTLIBRARIES_HEADERS ${SC_SRC_PATH}/external_libraries/*.h* )
FILE(GLOB_RECURSE SC_SCIDE_HEADERS ${SC_SRC_PATH}/editors/sc-ide/*.h* )
FILE(GLOB_RECURSE SC_SCDOC_HEADERS ${SC_SRC_PATH}/SCDoc/*.h* )

FILE(GLOB boost_system_sources ${SC_SRC_PATH}/external_libraries/boost/libs/system/src/*.h*)
FILE(GLOB boost_fs_sources ${SC_SRC_PATH}/external_libraries/boost/libs/filesystem/src/*.h*)
FILE(GLOB boost_regex_sources ${SC_SRC_PATH}/external_libraries/boost/libs/regex/src/*.h*)
FILE(GLOB boost_sync_sources ${SC_SRC_PATH}/external_libraries/boost/libs/regex/src/*.h*)

SET(ScBridge_hdr
	ScBridge.h
	
	${SC_INCLUDE_HEADERS}
	#${SC_EXTLIBRARIES_HEADERS}
	${SC_SCIDE_HEADERS}
	${SC_SCDOC_HEADERS}
)
list(APPEND ScBridge_hdr ${boost_fs_sources} ${boost_system_sources} ${boost_regex_sources})

#list(APPEND hello_sources ${CMAKE_SOURCE_DIR}/common/SC_Win32Utils.cpp)
	#include_directories (../platform/windows/compat_stuff)

#list(APPEND ScBridge_hdr ${CMAKE_SOURCE_DIR}/common/SC_Win32Utils.cpp)



SET(ScBridge_src
	ScBridge.cpp	
)

#ADD_LIBRARY (ScBridge ${ScBridge_hdr} ${ScBridge_src})
ADD_EXECUTABLE (ScBridge ${ScBridge_hdr} ${ScBridge_src})

#TARGET_INCLUDE_DIRECTORIES (ScBridge PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

TARGET_LINK_LIBRARIES(ScBridge 
	${QT_ScBridge_LIBRARIES}
	
	#${SC_BUILD_PATH}/Lang/Release/libsclang.lib
	libsclang
		
	#${SC_BUILD_PATH}/server/scsynth/Release/libscsynth.lib
	libscsynth
	
	#${SC_BUILD_PATH}/external_libraries/Release
	yaml
	oscpack
	boost_system
)

INSTALL(TARGETS ScBridge DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)

