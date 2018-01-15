
include(FindSupercollider)
find_package(Qt5 COMPONENTS Core Widgets Gui Network REQUIRED)

add_subdirectory (include)
add_subdirectory (src)
add_subdirectory (test)

# ////////////////////////////

add_library(ScBridge SHARED
	${ScBridge_hdr}
	${ScBridge_src}
)

get_target_property(_qtCoreInclude Qt5::Core INTERFACE_INCLUDE_DIRECTORIES) 
get_target_property(_qtGuiInclude Qt5::Gui INTERFACE_INCLUDE_DIRECTORIES) 
get_target_property(_qtNetworkInclude Qt5::Network INTERFACE_INCLUDE_DIRECTORIES) 
get_target_property(_qtWidgetsInclude Qt5::Widgets INTERFACE_INCLUDE_DIRECTORIES) 
list(APPEND _qtIncludeDirs ${_qtCoreInclude})
list(APPEND _qtIncludeDirs ${_qtGuiInclude})
list(APPEND _qtIncludeDirs ${_qtNetworkInclude})
list(APPEND _qtIncludeDirs ${_qtWidgetsInclude})
list(REMOVE_DUPLICATES _qtIncludeDirs)

add_custom_command(TARGET ScBridge
	PRE_BUILD
		COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_PATH}/include
		COMMAND ${CMAKE_COMMAND} -E copy_if_different ${ScBridge_hdr} ${CMAKE_BINARY_PATH}/include2
)

target_include_directories(ScBridge
	PRIVATE
		${CMAKE_CURRENT_LIST_DIR}/include
	PUBLIC
		${_qtIncludeDirs}
	INTERFACE 
		$<BUILD_INTERFACE:${CMAKE_BINARY_PATH}/include>
		$<INSTALL_INTERFACE:include>
)

string(TOUPPER ${CMAKE_CONFIGURATION_TYPES} configType)
get_target_property(_qtCoreLib Qt5::Core IMPORTED_IMPLIB_${configType}) 
get_target_property(_qtGuiLib Qt5::Gui IMPORTED_IMPLIB_${configType}) 
get_target_property(_qtNetworkLib Qt5::Network IMPORTED_IMPLIB_${configType}) 
get_target_property(_qtWidgetsLib Qt5::Widgets IMPORTED_IMPLIB_${configType}) 
get_target_property(_qtWinMainLib Qt5::WinMain IMPORTED_LOCATION_${configType}) 
list(APPEND _qtLinkLibs ${_qtCoreLib})
list(APPEND _qtLinkLibs ${_qtGuiLib})
list(APPEND _qtLinkLibs ${_qtNetworkLib})
list(APPEND _qtLinkLibs ${_qtWidgetsLib})
list(APPEND _qtLinkLibs ${_qtWinMainLib})

message(STATUS "configType: " ${configType})
message(STATUS "_qtLinkLibs: " ${_qtLinkLibs})

target_link_libraries(ScBridge 
	PUBLIC
		${_qtLinkLibs}
)

# ////////////////////////////

add_executable(ScTester 
	${ScTester_src}
)

target_include_directories(ScTester 
	PRIVATE 
		${CMAKE_CURRENT_LIST_DIR}/include
)
target_link_libraries(ScTester 
	PRIVATE 
		ScBridge
)


set_target_properties( ScBridge ScTester
    PROPERTIES
		ARCHIVE_OUTPUT_DIRECTORY lib
		LIBRARY_OUTPUT_DIRECTORY lib
		RUNTIME_OUTPUT_DIRECTORY bin
		DEBUG_POSTFIX d
)