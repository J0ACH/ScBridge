
MESSAGE(STATUS "CMAKE_CURRENT_SOURCE_DIR(QNT) : ${CMAKE_CURRENT_SOURCE_DIR}")
MESSAGE(STATUS "CMAKE_CURRENT_BINARY_DIR(QNT) : ${CMAKE_CURRENT_BINARY_DIR}")

OPTION(JUI_SYSTEMFRAME "QntGui with system frame" OFF)

SET(Qnt_hdr
	QntGui.h
	#QntGui_version.h.in
	#${CMAKE_CURRENT_SOURCE_DIR}/../QntGui_version.h.in
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
TARGET_LINK_LIBRARIES (QntGui LINK_PUBLIC ScBridge)




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