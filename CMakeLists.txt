#-----------------------------------------------------------------------------
#
# Ryzom Core
#  Authors: Nevrax and the Ryzom Core Community
#  Version: 0.12.0

CMAKE_MINIMUM_REQUIRED(VERSION 3.22)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)


project(RyzomCore VERSION 4.1.0.0 LANGUAGES CXX C)

option(WITH_SNOWBALLS_CLIENT ON)
option(WITH_SNOWBALLS_SERVER ON)
option(WITH_TESTING "Build tests" ON)

option(WITH_RYZOM_TOOLS OFF)
option(WITH_RYZOM_SERVER OFF)

option(RYZOM_CLIENT_CREATE_ACCOUNT_URL     "https://core.ryzom.dev/ams/")
option(RYZOM_CLIENT_EDIT_ACCOUNT_URL       "https://core.ryzom.dev/ams/")
option(RYZOM_CLIENT_FORGET_PASSWORD_URL    "https://core.ryzom.dev/ams/")
option(RYZOM_CLIENT_PATCH_URL              "https://cdn.ryzom.dev/core4/patch/")
option(RYZOM_CLIENT_APP_NAME               "core4")
option(RYZOM_CLIENT_RELEASENOTES_URL       "https://core4.ryzom.dev/releasenotes/index.php")
option(RYZOM_CLIENT_RELEASENOTES_RING_URL  "https://core4.ryzom.dev/releasenotes_ring/index.php")
option(RYZOM_CLIENT_UI_FEATURE_FLAG        "ryzomcore")




#-----------------------------------------------------------------------------
# Load some macros.
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/CMakeModules")
#-----------------------------------------------------------------------------
# Set CMake 2.6 Policies.
IF(COMMAND cmake_policy)
  # Works around warnings libraries linked against that don't
  # have absolute paths (e.g. -lpthread)
  cmake_policy(SET CMP0003 NEW)

  # allow to link to qtmain automatically under Windows
  IF(POLICY CMP0020)
    CMAKE_POLICY(SET CMP0020 NEW)
  ENDIF()

  IF (POLICY CMP0048)
    cmake_policy(SET CMP0048 NEW)
  ENDIF ()
ENDIF()


IF(WITH_SNOWBALLS_SERVER)
  ADD_DEFINITIONS(-DWITH_SNOWBALLS_SERVER)
ENDIF()

#-----------------------------------------------------------------------------

INCLUDE(nel)
INCLUDE(ConfigureChecks)
INCLUDE(CheckDepends)

# Force out of source builds.
CHECK_OUT_OF_SOURCE()

# To be able to specify a different deployment target on Mac OS X :
# export MACOSX_DEPLOYMENT_TARGET=10.6

IF(CMAKE_VERSION VERSION_GREATER "2.8.10")
  STRING(TIMESTAMP CURRENT_YEAR "%Y")
ELSE()
  SET(CURRENT_YEAR "2019")
ENDIF()


PROJECT(RyzomCore VERSION 4.1.0.0 LANGUAGES CXX C)
SET(NL_VERSION_MAJOR ${RyzomCore_VERSION_MAJOR})
SET(NL_VERSION_MINOR ${RyzomCore_VERSION_MINOR})
SET(NL_VERSION_PATCH ${RyzomCore_VERSION_PATCH} CACHE STRING "Patch version")
SET(NL_VERSION_BUILD ${RyzomCore_VERSION_TWEAK} CACHE STRING "Build number")
SET(YEAR "2001-${CURRENT_YEAR}")
SET(AUTHOR "Winch Gate and The Ryzom Core Community")


#-----------------------------------------------------------------------------
# Redirect output files
SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
SET(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# DLL should be in the same directory as EXE under Windows
IF(WIN32)
  SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
ELSE()
  SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
ENDIF()

IF(WIN32)
  IF(WITH_MFC)
    FIND_PACKAGE(MFC QUIET)
  ENDIF()
ENDIF()

#-----------------------------------------------------------------------------
# Set default config options
#
NL_SETUP_DEFAULT_OPTIONS()
NL_SETUP_NEL_DEFAULT_OPTIONS()
NL_SETUP_NELNS_DEFAULT_OPTIONS()
NL_SETUP_RYZOM_DEFAULT_OPTIONS()
NL_SETUP_SNOWBALLS_DEFAULT_OPTIONS()

NL_SETUP_BUILD()
NL_SETUP_BUILD_FLAGS()

NL_SETUP_PREFIX_PATHS()
RYZOM_SETUP_PREFIX_PATHS()


#-----------------------------------------------------------------------------
#Platform specifics

SETUP_EXTERNAL()

IF(WIN32)
  SET(WINSOCK2_LIB ws2_32.lib)

  IF(WITH_MFC)
    FIND_PACKAGE(CustomMFC REQUIRED)
  ENDIF()
ENDIF()

IF(HUNTER_ENABLED)
  # This fix is for compiling OpenSSL in Azure Pipeline linux agent where ENV{SYSTEM} == "build"
  IF(DEFINED ENV{SYSTEM} AND UNIX AND NOT APPLE)
    STRING(TOUPPER "$ENV{SYSTEM}" _tmp)
    IF (_tmp STREQUAL "BUILD")
      UNSET(ENV{SYSTEM})
    ENDIF()
    UNSET(_tmp)
  ENDIF()
  ## TODO: hack for freetype hunter package
  SET(ON 1)
  ##
  HUNTER_ADD_PACKAGE(ZLIB)

  FIND_PACKAGE(ZLIB CONFIG REQUIRED)
  SET(ZLIB_LIBRARY ZLIB::zlib)

  HUNTER_ADD_PACKAGE(libxml2)
  FIND_PACKAGE(libxml2 CONFIG REQUIRED)
  SET(LIBXML2_LIBRARIES ${LIBXML2_LIBRARIES} libxml2::libxml2)

  # cross compiling doesn't quite work on the hunter openssl package therefor compile it separately and use it
  IF(NOT CMAKE_CROSSCOMPILING)
    HUNTER_ADD_PACKAGE(OpenSSL)
  ENDIF()
  FIND_PACKAGE(OpenSSL REQUIRED)
  # TODO: is OpenSSL::Crypto only needed for WIN32?
  SET(OPENSSL_LIBRARIES OpenSSL::SSL OpenSSL::Crypto)

  # for precompiled headers, not needed if using TARGET_PRECOMPILE_HEADERS() from cmake 3.16+
  SET(ZLIB_INCLUDE_DIR ZLIB_ROOT/include)
  SET(LIBXML2_INCLUDE_DIR ${LIBXML2_ROOT}/include/libxml2)
ELSE()
FIND_PACKAGE(ZLIB REQUIRED)
FIND_PACKAGE(LibXml2 REQUIRED)
FIND_PACKAGE(PNG REQUIRED)
FIND_PACKAGE(OpenSSL REQUIRED)
FIND_PACKAGE(GIF)
FIND_PACKAGE(Jpeg)

IF(WIN32)
  SET(OPENSSL_LIBRARIES ${OPENSSL_LIBRARIES} Crypt32.lib)
ENDIF()
ENDIF() #hunter

IF(WITH_LIBOVR)
  FIND_PACKAGE(LibOVR)
ENDIF()

IF(WITH_LIBVR)
  FIND_PACKAGE(LibVR)
ENDIF()

NL_CONFIGURE_CHECKS()

IF(WITH_STATIC_LIBXML2)
  ADD_DEFINITIONS(-DLIBXML_STATIC)
ENDIF()

FIND_PACKAGE(Iconv QUIET)
IF(ICONV_FOUND)
  INCLUDE_DIRECTORIES(${ICONV_INCLUDE_DIR})
  SET(LIBXML2_LIBRARIES ${LIBXML2_LIBRARIES} ${ICONV_LIBRARIES})
ENDIF()

IF(WITH_STATIC)
  # libxml2 could need winsock2 library
  IF(WINSOCK2_LIB)
    SET(LIBXML2_LIBRARIES ${LIBXML2_LIBRARIES} ${WINSOCK2_LIB})
  ENDIF()
ENDIF()

IF(FINAL_VERSION)
  ADD_DEFINITIONS(-DFINAL_VERSION=1)
ENDIF()

IF(WITH_SSE2)
  ADD_DEFINITIONS(-DNL_HAS_SSE2)
  IF(WITH_SSE3)
    ADD_DEFINITIONS(-DNL_HAS_SSE3)
  ENDIF()
ENDIF()

IF(APPLE)
  FIND_LIBRARY(CARBON_FRAMEWORK Carbon)
  FIND_LIBRARY(FOUNDATION_FRAMEWORK Foundation)
  FIND_LIBRARY(SECURITY_FRAMEWORK Security)

  IF(APPLE_CERTIFICATE)
    # Find codesign_allocate

    # Xcode 7.0 and later versions
    SET(CODESIGN_ALLOCATE ${OSX_DEVELOPER_ROOT}/Toolchains/XcodeDefault.xctoolchain/usr/bin/codesign_allocate)

    IF(NOT EXISTS "${CODESIGN_ALLOCATE}")
      # Xcode 6.4 and previous versions
      SET(CODESIGN_ALLOCATE ${CMAKE_OSX_SYSROOT}/usr/bin/codesign_allocate)
    ENDIF()

    IF(NOT EXISTS "${CODESIGN_ALLOCATE}")
      # System path
      SET(CODESIGN_ALLOCATE /usr/bin/codesign_allocate)
    ENDIF()

    IF(NOT EXISTS "${CODESIGN_ALLOCATE}")
      MESSAGE(WARNING "Unable to find codesign_allocate in standard directories")
      SET(CODESIGN_ALLOCATE)
    ENDIF()
  ENDIF()
ENDIF()

INCLUDE(FindHelpers)

#TODO: hunter qt package
IF(WITH_QT5)
  FIND_QT5()
ENDIF()

IF(WITH_QT)
  SET(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} ${QTDIR} $ENV{QTDIR})

  # Use Qt 4
  FIND_PACKAGE(Qt4 COMPONENTS QtCore QtGui QtXml QtOpenGL REQUIRED)
ENDIF()

#TODO: hunter assimp package
IF(WITH_ASSIMP)
  FIND_PACKAGE(assimp REQUIRED)
ENDIF()

IF(WITH_MSQUIC)
  FIND_PACKAGE(msquic REQUIRED)
ENDIF()

if(WITH_TESTS)
    add_subdirectory(tests)
endif()



IF(WITH_NEL)
  IF(HUNTER_ENABLED)
    IF(WITH_GUI)
      HUNTER_ADD_PACKAGE(luabind)
      FIND_PACKAGE(Luabind REQUIRED)
    ENDIF()

    HUNTER_ADD_PACKAGE(CURL)
    FIND_PACKAGE(CURL CONFIG REQUIRED)
    # TODO: for nelgui
    SET(CURL_LIBRARIES CURL::libcurl libxml2::libxml2)
  ELSE()
    IF(WITH_GUI)
      FIND_PACKAGE(Luabind REQUIRED)
    ENDIF()

    FIND_PACKAGE(CURL REQUIRED)
    # if cmake finds config-file package, ie FIND_PACKAGE(CURL CONFIG), then CURL_LIBRARIES is not defined
    IF(TARGET CURL::libcurl)
      SET(CURL_LIBRARIES CURL::libcurl)
    ENDIF()

    IF((WIN32 OR CURL_LIBRARIES MATCHES "\\.a") AND WITH_STATIC_CURL)
      SET(CURL_STATIC ON)
    ELSE()
      SET(CURL_STATIC OFF)
    ENDIF()

    IF(CURL_STATIC)
      SET(CURL_DEFINITIONS -DCURL_STATICLIB)

      LIST(APPEND CURL_INCLUDE_DIRS ${OPENSSL_INCLUDE_DIR})
      LIST(APPEND CURL_LIBRARIES ${OPENSSL_LIBRARIES})

      IF(WIN32)
        LIST(APPEND CURL_LIBRARIES Crypt32 Cryptui)
      ELSE()
        # CURL depends on libidn
        FIND_LIBRARY(IDN_LIBRARY idn)
        IF(IDN_LIBRARY)
          LIST(APPEND CURL_LIBRARIES ${IDN_LIBRARY})
        ENDIF()

        # CURL Macports version can depend on libidn, libidn2, libintl, libpsl and libiconv too
        IF(APPLE)
          FIND_LIBRARY(INTL_LIBRARY intl)
          IF(INTL_LIBRARY)
            LIST(APPEND CURL_LIBRARIES ${INTL_LIBRARY})
          ENDIF()

          FIND_LIBRARY(IDN2_LIBRARY idn2)
          IF(IDN2_LIBRARY)
            LIST(APPEND CURL_LIBRARIES ${IDN2_LIBRARY})
          ENDIF()

          FIND_LIBRARY(PSL_LIBRARY psl)
          IF(PSL_LIBRARY)
            LIST(APPEND CURL_LIBRARIES ${PSL_LIBRARY})
          ENDIF()

          FIND_LIBRARY(UNISTRING_LIBRARY unistring)
          IF(UNISTRING_LIBRARY)
            LIST(APPEND CURL_LIBRARIES ${UNISTRING_LIBRARY})
          ENDIF()

          LIST(APPEND CURL_LIBRARIES ${SECURITY_FRAMEWORK})
        ENDIF()
      ENDIF()
    ENDIF()
  ENDIF() #hunter

  INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/shared/nel/include)
  ADD_SUBDIRECTORY(shared/nel)

  # Aliases for targets
  SET(NELMISC_LIBRARIES nelmisc)
  SET(NELNET_LIBRARIES nelnet)
  SET(NELLIGO_LIBRARIES nelligo)
  SET(NELGEORGES_LIBRARIES nelgeorges)
  SET(NEL3D_LIBRARIES nel3d)
  SET(NELGUI_LIBRARIES nelgui)
  SET(NELSOUND_LIBRARIES nelsound)
ELSE()
#  FIND_PACKAGE(NeL COMPONENTS 3d misc pacs sound nimp REQUIRED)
  FIND_PACKAGE(NeL REQUIRED)
  INCLUDE_DIRECTORIES(${NEL_INCLUDE_DIRS})
ENDIF()

FIND_PACKAGE(MySQL)

ADD_SUBDIRECTORY(shared)
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/shared)

IF(WITH_SNOWBALLS_CLIENT)
  ADD_SUBDIRECTORY(client)
ENDIF()

IF(WITH_SNOWBALLS_SERVER)
  ADD_SUBDIRECTORY(server)
ENDIF()


IF(WITH_NEL_TESTS)
  ENABLE_TESTING()
  ADD_TEST(NAME nel_unit_test COMMAND nel_unit_test)
  IF(BUILD_DASHBOARD)
    INCLUDE(Dart)
    SET(SVNCOMMAND svn)
    SET(SVNSOURCEDIR http://dev.ryzom.com/svn/trunk/nel)
    SET(GENERATELOGS svn2cl)
  ENDIF()
ENDIF()

# packaging information
SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Ryzom Core MMORPG Framework")
SET(CPACK_PACKAGE_VENDOR "Ryzom Core")
SET(CPACK_PACKAGE_DESCRIPTION_FILE ${CMAKE_SOURCE_DIR}/README.md)
SET(CPACK_RESOURCE_FILE_LICENSE ${CMAKE_SOURCE_DIR}/COPYING)
SET(CPACK_PACKAGE_VERSION_MAJOR "${NL_VERSION_MAJOR}")
SET(CPACK_PACKAGE_VERSION_MINOR "${NL_VERSION_MINOR}")
SET(CPACK_PACKAGE_VERSION_PATCH "${NL_VERSION_PATCH}")
SET(CPACK_INSTALL_CMAKE_PROJECTS "${CMAKE_BINARY_DIR};NeL;ALL;/")
SET(CPACK_PACKAGE_EXECUTABLES "ryzomcore${NL_VERSION}" "ryzomcore")

# NSIS Specific Packing Setup
SET(CPACK_PACKAGE_INSTALL_REGISTRY_KEY "RyzomCore")
SET(CPACK_NSIS_MODIFY_PATH "ON")
SET(CPACK_NSIS_MUI_ICON ${CMAKE_SOURCE_DIR}/nel/resources/nevraxpill.ico)
SET(CPACK_NSIS_MUI_UNIICON ${CMAKE_SOURCE_DIR}/nel/resources/nevraxpill.ico)
SET(CPACK_PACKAGE_ICON ${CMAKE_SOURCE_DIR}/nel/resources\\\\nel.bmp)
SET(CPACK_NSIS_DISPLAY_NAME "${CPACK_PACKAGE_INSTALL_DIRECTORY} RyzomCore")
SET(CPACK_NSIS_HELP_LINK "http:\\\\\\\\dev.ryzom.com")
SET(CPACK_NSIS_URL_INFO_ABOUT "http:\\\\\\\\dev.ryzom.com\\\\projects\\\\nel\\\\wiki")
SET(CPACK_NSIS_CONTACT "matt.raykowski@gmail.com")

## Source Packages
SET(CPACK_PACKAGE_FILE_NAME "ryzomcore-${NL_VERSION}")
SET(CPACK_SOURCE_PACKAGE_FILE_NAME "ryzomcore-${NL_VERSION}")
IF(WIN32)
  #SET(CPACK_GENERATOR "NSIS")
  SET(CPACK_GENERATOR "NSIS;ZIP")
  SET(CPACK_SOURCE_GENERATOR "ZIP")
ELSE()
  SET(CPACK_SET_DESTDIR TRUE)
  SET(CPACK_GENERATOR "TGZ;STGZ")
  SET(CPACK_SOURCE_GENERATOR "TGZ")
ENDIF()
set(CPACK_SOURCE_IGNORE_FILES
	"~$"
	"\\\\.cvsignore$"
	"^${CMAKE_SOURCE_DIR}.*/CVS/"
	"^${CMAKE_SOURCE_DIR}.*/\\\\.svn/"
	"^${CMAKE_SOURCE_DIR}/debian/"
	"^${CMAKE_SOURCE_DIR}/old/")
IF(WIN32)
  IF(NOT CMAKE_BUILD_TYPE STREQUAL "Release")
    SET(CMAKE_INSTALL_DEBUG_LIBRARIES TRUE)
    IF(WITH_QT)
      INCLUDE(${QT_USE_FILE})
      INSTALL(FILES
        "${QT_LIBRARY_DIR}/../bin/QtGuid4.dll"
        "${QT_LIBRARY_DIR}/../bin/QtXmld4.dll"
        "${QT_LIBRARY_DIR}/../bin/QtCored4.dll"
        DESTINATION ${NL_BIN_PREFIX})
    ENDIF()
  ELSE()
    IF(WITH_QT)
      INCLUDE(${QT_USE_FILE})
      INSTALL(FILES
        "${QT_LIBRARY_DIR}/../bin/QtGui4.dll"
        "${QT_LIBRARY_DIR}/../bin/QtXml4.dll"
        "${QT_LIBRARY_DIR}/../bin/QtCore4.dll"
        DESTINATION ${NL_BIN_PREFIX})
    ENDIF()
  ENDIF()

  # Install CEGUI and its dependencies.
  IF(WITH_NEL_CEGUI)
    INSTALL(FILES "${CEGUI_LIB_DIR}/CEGUIBase.dll" DESTINATION ${NL_BIN_PREFIX})
    INSTALL(FILES "${CEGUI_LIB_DIR}/CEGUIDevilImageCodec.dll" DESTINATION ${NL_BIN_PREFIX})
    INSTALL(FILES "${CEGUI_LIB_DIR}/CEGUIExpatParser.dll" DESTINATION ${NL_BIN_PREFIX})
    INSTALL(FILES "${CEGUI_LIB_DIR}/CEGUIFalagardWRBase.dll" DESTINATION ${NL_BIN_PREFIX})
    INSTALL(FILES "${CEGUI_LIB_DIR}/Devil.dll" DESTINATION ${NL_BIN_PREFIX})
    INSTALL(FILES "${CEGUI_LIB_DIR}/ILU.dll" DESTINATION ${NL_BIN_PREFIX})
  ENDIF()

  # Only the tools require MFC.
  IF(WITH_TOOLS)
    SET(CMAKE_INSTALL_MFC_LIBRARIES TRUE)
  ENDIF()
  #INCLUDE(InstallRequiredSystemLibraries)
ENDIF()

INCLUDE(CPack)

INCLUDE(CMakePackaging.txt)

## Debian Packages
#INCLUDE(UseDebian)
#IF(DEBIAN_FOUND)
#  ADD_DEBIAN_TARGETS(nel)
#ENDIF()