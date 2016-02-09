
MESSAGE(STATUS "CMAKE_CURRENT_SOURCE_DIR(JUI) : ${CMAKE_CURRENT_SOURCE_DIR}")

CONFIGURE_FILE (
  "${CMAKE_CURRENT_SOURCE_DIR}/JuiConfig.h.in"
  "${CMAKE_CURRENT_BINARY_DIR}/JuiConfig.h"
)
# include for config file
INCLUDE_DIRECTORIES( ${CMAKE_CURRENT_BINARY_DIR} )

FIND_PACKAGE(Qt5Core)
FIND_PACKAGE(Qt5Widgets)
FIND_PACKAGE(Qt5Gui)

SET(QT_JUI_LIBRARIES 
	Qt5::Core
	Qt5::Gui 
	Qt5::Widgets
)

SET(Jui_hdr
	Canvan.h
	Button.h
	Console.h
)

SET(Jui_src
	Canvan.cpp
	Button.cpp
	Console.cpp
)

# Create a library called "Hello" which includes the source file "hello.cxx".
# The extension is already found. Any number of sources could be listed here.
ADD_LIBRARY (Jui ${Jui_hdr} ${Jui_src})

# Make sure the compiler can find include files for our Hello library
# when other libraries or executables link to Hello
TARGET_INCLUDE_DIRECTORIES (Jui PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

TARGET_LINK_LIBRARIES(Jui ${QT_JUI_LIBRARIES})
