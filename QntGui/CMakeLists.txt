
MESSAGE(STATUS "CMAKE_CURRENT_SOURCE_DIR(QNT) : ${CMAKE_CURRENT_SOURCE_DIR}")
MESSAGE(STATUS "CMAKE_CURRENT_BINARY_DIR(QNT) : ${CMAKE_CURRENT_BINARY_DIR}")



SET(SC_SRC_PATH "C:/Supercollider/Supercollider_src" CACHE PATH "Supercollider source path")
SET(SC_BUILD_PATH "C:/Supercollider/Supercollider_build" CACHE PATH "Supercollider build path")



INCLUDE_DIRECTORIES(
	${CMAKE_SOURCE_DIR}/ScBridge
	
	#${SC_SRC_PATH}
	${SC_SRC_PATH}/editors/sc-ide/core
	${SC_SRC_PATH}/editors/sc-ide/primitives
	#${SC_SRC_PATH}/editors/sc-ide/widgets	
	
	${SC_SRC_PATH}/external_libraries/boost
	${SC_SRC_PATH}/external_libraries/yaml-cpp-0.3.0/include
	${SC_SRC_PATH}/external_libraries/oscpack_1_1_0
	
	${SC_SRC_PATH}/include/lang
	${SC_SRC_PATH}/include/lang/langSource
	${SC_SRC_PATH}/include/common
	
)

LINK_DIRECTORIES( 
	${SC_BUILD_PATH}/lang/Release/libsclang.lib 
	#${SC_SRC_PATH}/lang/Release/sclang.lib 
)  

SET(Qnt_hdr
	QntGui.h
	
	${SC_SRC_PATH}/editors/sc-ide/core/sc_process.hpp
	${SC_SRC_PATH}/editors/sc-ide/primitives/sc_ipc_channel.hpp
	#${SC_SRC_PATH}/editors/sc-ide/core/sc_introspection.hpp
	
	#SC_LanguageClient.h
	#${SC_SRC_PATH}/include/lang/SC_LanguageClient.h
	#${SC_SRC_PATH}/include/common/SC_Export.h
	
	#${SC_SRC_PATH}/lang/LangSource/SC_TerminalClient.h
)
SET(Qnt_src
	QntGui.cpp
)


# Add executable called "helloDemo" that is built from the source files
# "demo.cxx" and "demo_b.cxx". The extensions are automatically found.
ADD_EXECUTABLE (QntGui WIN32 ${Qnt_hdr} ${Qnt_src})


# Link the executable to the Hello library. Since the Hello library has
# public include directories we will use those link directories when building
# helloDemo
TARGET_LINK_LIBRARIES (QntGui LINK_PUBLIC Jui)
#TARGET_LINK_LIBRARIES (QntGui LINK_PUBLIC ScBridge)


TARGET_LINK_LIBRARIES(QntGui LINK_PUBLIC ${SC_BUILD_PATH}/lang/Release/libsclang.lib )




if(WIN32)	
	SET(QT_DLL_DIR ${Qt5Core_DIR}/../../../bin)
	SET(QT_LIBRARIES_DEBUG
		${QT_DLL_DIR}/Qt5Cored.dll
		${QT_DLL_DIR}/Qt5Guid.dll
		${QT_DLL_DIR}/Qt5Widgetsd.dll
		${QT_DLL_DIR}/icuin53.dll
		${QT_DLL_DIR}/icuuc53.dll
		${QT_DLL_DIR}/icudt53.dll
	)
	SET(QT_LIBRARIES_RELEASE
		${QT_DLL_DIR}/Qt5Core.dll
		${QT_DLL_DIR}/Qt5Gui.dll
		${QT_DLL_DIR}/Qt5Widgets.dll
		${QT_DLL_DIR}/icuin53.dll
		${QT_DLL_DIR}/icuuc53.dll
		${QT_DLL_DIR}/icudt53.dll
	)

	IF(CMAKE_C_FLAGS_DEBUG)
		FILE(COPY ${QT_LIBRARIES_DEBUG} DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/Debug)
	ENDIF()

	IF(CMAKE_C_FLAGS_RELEASE)
		FILE(COPY ${QT_LIBRARIES_RELEASE} DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/Release)
	ENDIF()
ENDIF()