
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

target_include_directories(ScBridge 
	PRIVATE 
		${CMAKE_CURRENT_LIST_DIR}/include
	PUBLIC
		${_qtIncludeDirs}
)
target_link_libraries(ScBridge 
	PUBLIC 
		Qt5::Core
		Qt5::Gui
		Qt5::Network
		Qt5::Widgets
)

add_custom_command(TARGET ScBridge
	PRE_BUILD
		COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_SOURCE_DIR}/build/include
		COMMAND ${CMAKE_COMMAND} -E copy_if_different ${ScBridge_hdr} ${CMAKE_SOURCE_DIR}/build/include
)
# ////////////////////////////

add_executable(ScTester 
	${ScTester_src}
)

target_include_directories(ScTester 
	PRIVATE 
		${CMAKE_CURRENT_LIST_DIR}/include
	PUBLIC
		${_qtIncludeDirs}
)
target_link_libraries(ScTester 
	PUBLIC 
		ScBridge
		Qt5::Core
		Qt5::Widgets
)

string(TOUPPER ${CMAKE_CONFIGURATION_TYPES} configType)
set_target_properties( ScBridge ScTester
    PROPERTIES
		ARCHIVE_OUTPUT_DIRECTORY_${configType} ${CMAKE_SOURCE_DIR}/build/lib
		LIBRARY_OUTPUT_DIRECTORY_${configType} ${CMAKE_SOURCE_DIR}/build/lib
		RUNTIME_OUTPUT_DIRECTORY_${configType} ${CMAKE_SOURCE_DIR}/build/bin
		DEBUG_POSTFIX d
	#	WIN32_EXECUTABLE TRUE
)