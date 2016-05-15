#build file for CMake, see http://www.cmake.org/

CMAKE_MINIMUM_REQUIRED(VERSION 2.6.2)

PROJECT(wxquincy)

SET(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake_modules/")

# for wxWidgets 2.8 remove "propgrid" in the list below and for Windows add "scintilla"
FIND_PACKAGE(wxWidgets COMPONENTS base core adv aui html stc propgrid QUIET)
IF(NOT wxWidgets_FOUND)
    FIND_PACKAGE(wxWidgets COMPONENTS mono REQUIRED)
ENDIF(NOT wxWidgets_FOUND)
INCLUDE("${wxWidgets_USE_FILE}")

# definitions for wxWidgets
INCLUDE_DIRECTORIES(${wxWidgets_INCLUDE_DIRS} ../amx ../linux)
ADD_DEFINITIONS("-D${wxWidgets_DEFINITIONS}")
ADD_DEFINITIONS(-DINI_ANSIONLY)

SET(QUINCY_SRCS wxQuincy.cpp QuincyFrame.cpp QuincySettingsDlg.cpp
    QuincySearchDlg.cpp QuincyReplaceDlg.cpp QuincyReplacePrompt.cpp
    QuincyDialogs.cpp KbdShortcuts.cpp HelpIndex.cpp SymbolBrowser.cpp
    tinyxml/tinyxml2.cpp portscan.cpp minIni.c)
IF(WIN32)
  SET(QUINCY_SRCS ${QUINCY_SRCS} wxquincy.rc)
ELSE(WIN32)
#uncomment this section for wxWidgets 2.8
#  INCLUDE_DIRECTORIES(./propgrid/include)
#  SET(QUINCY_SRCS ${QUINCY_SRCS} ./propgrid/src/propgrid.cpp ./propgrid/src/props.cpp
#      ./propgrid/src/editors.cpp ./propgrid/src/advprops.cpp)
ENDIF(WIN32)
ADD_EXECUTABLE(wxquincy ${QUINCY_SRCS})
TARGET_LINK_LIBRARIES(wxquincy ${wxWidgets_LIBRARIES})
#uncomment this section for wxWidgets 2.8
#IF(WIN32)
#  TARGET_LINK_LIBRARIES(wxquincy ${TARGET_LINK_LIBRARIES} wxcode_msw28u_propgrid)
#ELSE(WIN32)
#  TARGET_LINK_LIBRARIES(wxquincy ${TARGET_LINK_LIBRARIES} wxcode_gtk2u_propgrid-2.8)
#ENDIF(WIN32)

##### Installation instructions

INSTALL(TARGETS wxquincy
    DESTINATION ${CMAKE_INSTALL_PREFIX}/bin
    COMPONENT application)
INSTALL(FILES ${CMAKE_BINARY_DIR}/../doc/Quincy.pdf
              ${CMAKE_BINARY_DIR}/../doc/LICENSE
              ${CMAKE_BINARY_DIR}/../doc/NOTICE
        DESTINATION ${CMAKE_INSTALL_PREFIX}/doc
        COMPONENT documentation)

SET(DEB_DESKTOP_DIR     "/usr/share/applications")
SET(DEB_MIME_DIR        "/usr/share/mime/packages")
SET(DEB_PIXMAPS_DIR     "/usr/share/pixmaps")

INSTALL(FILES ${CMAKE_BINARY_DIR}/wxquincy.desktop DESTINATION ${DEB_DESKTOP_DIR})
INSTALL(FILES ${CMAKE_BINARY_DIR}/wxquincy.xml DESTINATION ${DEB_MIME_DIR})
INSTALL(FILES ${CMAKE_BINARY_DIR}/pawn32.png DESTINATION ${DEB_PIXMAPS_DIR})

##### Packaging instructions

SET(CPACK_GENERATOR DEB)
SET(CPACK_DEBIAN_PACKAGE_ARCHITECTURE i386)
SET(CPACK_DEBIAN_PACKAGE_DEPENDS "libwxgtk2.8-0 (>= 2.8.10-1), libc6 (>= 2.11)")
SET(CPACK_PACKAGE_NAME "PawnIDE")
SET(CPACK_PACKAGE_VENDOR "CompuPhase")
SET(CPACK_DEBIAN_PACKAGE_LICENSE "Apache v2")
SET(CPACK_PACKAGE_CONTACT "Thiadmer Riemersma <thiadmer@compuphase.com>")
SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Integrated Development Environment for the Pawn scripting language.")
SET(CPACK_PACKAGE_DESCRIPTION "This application integrates a code editor, compiler, debugger and project manager.")
SET(CPACK_PACKAGE_VERSION_MAJOR "4")
SET(CPACK_PACKAGE_VERSION_MINOR "0")
SET(CPACK_PACKAGE_VERSION_PATCH "5102")
SET(CPACK_PACKAGE_VERSION "${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.${CPACK_PACKAGE_VERSION_PATCH}")
SET(CPACK_PACKAGE_INSTALL_DIRECTORY "pawn")
SET(CPACK_COMPONENTS_ALL application documentation)
SET(CPACK_PACKAGE_EXECUTABLES "wxquincy")

SET(CPACK_INSTALL_PREFIX "/opt/pawn")
SET(CPACK_SET_DESTDIR TRUE)

INCLUDE(CPack)
