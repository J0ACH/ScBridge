
MESSAGE(STATUS "CMAKE_CURRENT_SOURCE_DIR(SCBRIDGE) : ${CMAKE_CURRENT_SOURCE_DIR}")

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
	${CMAKE_SOURCE_DIR}/ScBridge
	
	${SC_SRC_PATH}
	${SC_SRC_PATH}/editors/sc-ide/core
	${SC_SRC_PATH}/editors/sc-ide/primitives
	${SC_SRC_PATH}/editors/sc-ide/widgets	
	
	${SC_SRC_PATH}/external_libraries/boost
	${SC_SRC_PATH}/external_libraries/yaml-cpp-0.3.0/include
	${SC_SRC_PATH}/external_libraries/oscpack_1_1_0
	
)

LINK_DIRECTORIES( ${SC_BUILD_PATH}/Lang/Release/libsclang.lib )  


SET(ScBridge_hdr
	ScBridge.h
	
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

SET(ScBridge_src
	ScBridge.cpp
)

ADD_LIBRARY (ScBridge ${ScBridge_hdr} ${ScBridge_src})

TARGET_INCLUDE_DIRECTORIES (ScBridge PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

TARGET_LINK_LIBRARIES(ScBridge ${QT_ScBridge_LIBRARIES})
TARGET_LINK_LIBRARIES(ScBridge sclang)