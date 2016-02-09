
MESSAGE(STATUS "CMAKE_CURRENT_SOURCE_DIR(QNT) : ${CMAKE_CURRENT_SOURCE_DIR}")
MESSAGE(STATUS "CMAKE_CURRENT_BINARY_DIR(QNT) : ${CMAKE_CURRENT_BINARY_DIR}")

SET(SC_SRC_PATH "C:/Supercollider/Supercollider_src" CACHE PATH "Supercollider source path")
SET(SC_BUILD_PATH "C:/Supercollider/Supercollider_build" CACHE PATH "Supercollider build path")


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

SET(QT_QntGui_LIBRARIES 
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

INCLUDE_DIRECTORIES(
	${CMAKE_SOURCE_DIR}/QntGui
	${CMAKE_CURRENT_BINARY_DIR}
	
	${SC_SRC_PATH}
	${SC_SRC_PATH}/editors/sc-ide/core
	${SC_SRC_PATH}/editors/sc-ide/primitives
	${SC_SRC_PATH}/editors/sc-ide/widgets	
	
	${SC_SRC_PATH}/external_libraries/boost
	${SC_SRC_PATH}/external_libraries/yaml-cpp-0.3.0/include
	${SC_SRC_PATH}/external_libraries/oscpack_1_1_0
)

LINK_DIRECTORIES( 
	
	${SC_BUILD_PATH}/Lang/Release/libsclang.lib 
	
	${SC_BUILD_PATH}/external_libraries/Release/boost_system.lib
	${SC_BUILD_PATH}/external_libraries/Release/oscpack.lib
	${SC_BUILD_PATH}/external_libraries/Release/yaml.lib
	
	)  


SET(Qnt_hdr
	QntGui.h
	
	${SC_SRC_PATH}/editors/sc-ide/core/main.hpp
    ${SC_SRC_PATH}/editors/sc-ide/core/sig_mux.hpp
    ${SC_SRC_PATH}/editors/sc-ide/core/sc_process.hpp
    ${SC_SRC_PATH}/editors/sc-ide/core/sc_server.hpp
    ${SC_SRC_PATH}/editors/sc-ide/core/doc_manager.hpp
    ${SC_SRC_PATH}/editors/sc-ide/core/settings/manager.hpp
    ${SC_SRC_PATH}/editors/sc-ide/core/settings/theme.hpp
    ${SC_SRC_PATH}/editors/sc-ide/core/session_manager.hpp
    ${SC_SRC_PATH}/editors/sc-ide/primitives/sc_ipc_channel.hpp
    ${SC_SRC_PATH}/editors/sc-ide/widgets/cmd_line.hpp
    ${SC_SRC_PATH}/editors/sc-ide/widgets/doc_list.hpp
    ${SC_SRC_PATH}/editors/sc-ide/widgets/documents_dialog.hpp
    ${SC_SRC_PATH}/editors/sc-ide/widgets/editor_box.hpp
    ${SC_SRC_PATH}/editors/sc-ide/widgets/find_replace_tool.hpp
    ${SC_SRC_PATH}/editors/sc-ide/widgets/goto_line_tool.hpp
    ${SC_SRC_PATH}/editors/sc-ide/widgets/lookup_dialog.hpp
    ${SC_SRC_PATH}/editors/sc-ide/widgets/main_window.hpp
    ${SC_SRC_PATH}/editors/sc-ide/widgets/multi_editor.hpp
    ${SC_SRC_PATH}/editors/sc-ide/widgets/popup_text_input.hpp
    ${SC_SRC_PATH}/editors/sc-ide/widgets/post_window.hpp
    ${SC_SRC_PATH}/editors/sc-ide/widgets/session_switch_dialog.hpp
    ${SC_SRC_PATH}/editors/sc-ide/widgets/sessions_dialog.hpp
    ${SC_SRC_PATH}/editors/sc-ide/widgets/tool_box.hpp
    ${SC_SRC_PATH}/editors/sc-ide/widgets/help_browser.hpp
    ${SC_SRC_PATH}/editors/sc-ide/widgets/audio_status_box.hpp
    ${SC_SRC_PATH}/editors/sc-ide/widgets/lang_status_box.hpp
    ${SC_SRC_PATH}/editors/sc-ide/widgets/code_editor/editor.hpp
    ${SC_SRC_PATH}/editors/sc-ide/widgets/code_editor/sc_editor.hpp
    ${SC_SRC_PATH}/editors/sc-ide/widgets/code_editor/highlighter.hpp
    ${SC_SRC_PATH}/editors/sc-ide/widgets/code_editor/line_indicator.hpp
    ${SC_SRC_PATH}/editors/sc-ide/widgets/code_editor/overlay.hpp
    ${SC_SRC_PATH}/editors/sc-ide/widgets/code_editor/autocompleter.hpp
    ${SC_SRC_PATH}/editors/sc-ide/widgets/code_editor/completion_menu.hpp
    ${SC_SRC_PATH}/editors/sc-ide/widgets/settings/dialog.hpp
    ${SC_SRC_PATH}/editors/sc-ide/widgets/settings/general_page.hpp
    ${SC_SRC_PATH}/editors/sc-ide/widgets/settings/sclang_page.hpp
    ${SC_SRC_PATH}/editors/sc-ide/widgets/settings/editor_page.hpp
    ${SC_SRC_PATH}/editors/sc-ide/widgets/settings/shortcuts_page.hpp
    ${SC_SRC_PATH}/editors/sc-ide/widgets/util/path_chooser_widget.hpp
    ${SC_SRC_PATH}/editors/sc-ide/widgets/util/text_format_list_widget.hpp
    ${SC_SRC_PATH}/editors/sc-ide/widgets/util/key_sequence_edit.hpp
    ${SC_SRC_PATH}/editors/sc-ide/widgets/util/popup_widget.hpp
    ${SC_SRC_PATH}/editors/sc-ide/widgets/util/color_widget.hpp
    ${SC_SRC_PATH}/editors/sc-ide/widgets/util/docklet.hpp
    ${SC_SRC_PATH}/editors/sc-ide/widgets/util/volume_widget.hpp
	
)

FILE (GLOB_RECURSE all_hdr *hpp)

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
TARGET_LINK_LIBRARIES (QntGui 
	${SC_BUILD_PATH}/external_libraries/Release/boost_system.lib
	${SC_BUILD_PATH}/external_libraries/Release/oscpack.lib
	${SC_BUILD_PATH}/external_libraries/Release/yaml.lib
)
#TARGET_LINK_LIBRARIES (QntGui LINK_PUBLIC ScBridge)

TARGET_LINK_LIBRARIES(QntGui LINK_PUBLIC ${QT_QntGui_LIBRARIES})
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