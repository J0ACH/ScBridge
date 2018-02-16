
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
	${ScTester_hdr}
	${ScTester_src}
)

target_include_directories(ScTester 
	PRIVATE 
		${CMAKE_CURRENT_LIST_DIR}/test
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
		WIN32_EXECUTABLE TRUE
)

# PACKAGE AND INSTALL SETUP ###################################################

#[[
#]]
include(CMakePackageConfigHelpers)
configure_package_config_file (
	${CMAKE_CURRENT_SOURCE_DIR}/ScBridgeConfig.cmake.in
	${CMAKE_BINARY_DIR}/ScBridgeConfig.cmake
	INSTALL_DESTINATION lib/cmake
)
write_basic_package_version_file(
	${CMAKE_BINARY_DIR}/ScBridgeConfigVersion.cmake
	VERSION ${PROJECT_VERSION}
	COMPATIBILITY AnyNewerVersion 
)
cmake_policy(SET CMP0022 NEW)
export ( 
	TARGETS ScBridge
	FILE ${CMAKE_BINARY_DIR}/lib/${CMAKE_CONFIGURATION_TYPES}/ScBridgeTargets.cmake 
	#FILE ${CMAKE_BINARY_DIR}/${CMAKE_PROJECT_NAME}Targets.cmake 
	EXPORT_LINK_INTERFACE_LIBRARIES
)

install(
	TARGETS ScBridge
	EXPORT ScBridgeTargets
	ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
	RUNTIME DESTINATION bin
	CONFIGURATIONS Release
)
install(
	FILES ${ScBridge_hdr}
	DESTINATION include
)
install(
	EXPORT ScBridgeTargets
	DESTINATION lib/cmake/ScBridge-${PROJECT_VERSION}
	FILE ScBridgeTargets.cmake
	#CONFIGURATIONS Release
)
install(
	FILES 
		${CMAKE_BINARY_DIR}/ScBridgeConfig.cmake
		${CMAKE_BINARY_DIR}/ScBridgeConfigVersion.cmake
	DESTINATION	lib/cmake/ScBridge-${PROJECT_VERSION}
)
#[[
#]]