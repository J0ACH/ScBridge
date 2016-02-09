
MESSAGE(STATUS "CMAKE_CURRENT_SOURCE_DIR(SCBRIDGE) : ${CMAKE_CURRENT_SOURCE_DIR}")

SET(SC_SRC_PATH "C:/Supercollider/Supercollider_src" CACHE PATH "Supercollider source path")
#SET(SC_BUILD_PATH "C:/Supercollider/Supercollider_build" CACHE PATH "Supercollider build path")
	
#SET(SC_LANG_PATH "${SC_BUILD_PATH}/Lang/Release/libsclang.lib")

#FIND_PACKAGE(Qt5Core)
#FIND_PACKAGE(Qt5Widgets)
#FIND_PACKAGE(Qt5Gui)
#FIND_PACKAGE(Qt5Network)
#FIND_PACKAGE(Qt5WebKit)
#FIND_PACKAGE(Qt5WebKitWidgets)

SET(QT_ScBridge_LIBRARIES 
	#Qt5::Core
	#Qt5::Gui 
	#Qt5::Widgets
	#Qt5::Network
	#Qt5::WebKit
	#Qt5::WebKitWidgets
	
)

INCLUDE_DIRECTORIES(
	${CMAKE_SOURCE_DIR}/ScBridge
	
	#${SC_SRC_PATH}/editors/sc-ide/core
	#${SC_SRC_PATH}/editors/sc-ide/primitives
	#${SC_SRC_PATH}/editors/sc-ide/widgets	
	
	#${SC_SRC_PATH}/external_libraries/boost
	#${SC_SRC_PATH}/external_libraries/yaml-cpp-0.3.0/include
	#${SC_SRC_PATH}/external_libraries/oscpack_1_1_0
	
)


SET(ScBridge_hdr
	ScBridge.h
)

SET(ScBridge_src
	ScBridge.cpp
)

ADD_LIBRARY (ScBridge ${ScBridge_hdr} ${ScBridge_src})

TARGET_INCLUDE_DIRECTORIES (ScBridge PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

TARGET_LINK_LIBRARIES(ScBridge ${QT_ScBridge_LIBRARIES})