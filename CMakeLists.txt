# if necessary, set CMAKE_PREFIX_PATH to the path where libmikmod
# is installed, which you can do on your cmake command line, like:
# cmake -DCMAKE_PREFIX_PATH=/path/to/libmikmod_dir ....

PROJECT(tplayer)
CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

LIST(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

SET(VERSION "0.0.1")

STRING(REGEX MATCHALL "([0-9]+)" VERSION_DIGITS "${VERSION}")

LIST(GET VERSION_DIGITS 0 CPACK_PACKAGE_VERSION_MAJOR)
LIST(GET VERSION_DIGITS 1 CPACK_PACKAGE_VERSION_MINOR)
LIST(GET VERSION_DIGITS 2 CPACK_PACKAGE_VERSION_PATCH)

SET(CPACK_PACKAGE_NAME "tplayer")
SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY "tPlayer - Tracker Player")
SET(CPACK_PACKAGE_VENDOR "devsdmf")
#SET(CPACK_PACAKGE_DESCRIPTION_FILE "${CMAKE_SOURCE_DIR}/README")
#SET(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/COPYING")

SET(CPACK_PACKAGE_INSTALL_DIRECTORY "${CPACK_PACKAGE_DESCRIPTION_SUMMARY} ${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.${CPACK_PACKAGE_VERSION_PATCH}")
SET(CPACK_SOURCE_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.${CPACK_PACKAGE_VERSION_PATCH}")

SET(base_with_ver "mikmod-[0-9]+\\\\.[0-9]+\\\\.[0-9]+")
SET(CPACK_SOURCE_IGNORE_FILES
    "/_CPack_Packages/"
    "/CMakeFiles/"
    "/.deps/"
    "^${base_with_ver}(-Source|-Linux)?/"
    "${base_with_ver}.tar\\\\.(gz|bz2|Z|lzma|xz)$"
    "\\\\.o$"
    "~$"
    "/\\\\.svn/"
    "/CMakeCache\\\\.txt$"
    "/CTestTestfile\\\\.cmake$"
    "/cmake_install\\\\.cmake$"
    "/CPackConfig\\\\.cmake$"
    "/CPackSourceConfig\\\\.cmake$"
    "/tags$"
    "^config\\\\.h$"
    "/install_manifest\\\\.txt$"
    "/Testing/"
    "ids-whitelist\\\\.txt"
    "/_Inline/"
    "/(B|build|BUILD)/"
    "/autom4te.cache/"
)

INCLUDE(CPack)
INCLUDE(CheckFunctionExists)
INCLUDE(CheckSymbolExists)
INCLUDE(CheckCCompilerFlag)
INCLUDE(CheckCSourceCompiles)
INCLUDE(tplayer_macros)

CHECK_MULTI_INCLUDE_FILES("ncurses.h" "curses.h" "ncurses/curses.h")

SET(EXTRA_LIBS )

FIND_PATH(MIKMOD_INCLUDE_DIR mikmod.h)
FIND_LIBRARY(MIKMOD_LIBRARIES mikmod)
IF (MIKMOD_LIBRARIES STREQUAL "MIKMOD_LIBRARIES-NOTFOUND")
    MESSAGE(FATAL_ERROR "libmikmod not found.")
ELSE()
    MESSAGE(STATUS "Found MikMod: ${MIKMOD_LIBRARIES}")
ENDIF()

IF(UNIX OR APPLE)
    INCLUDE(FindCurses)
    IF(NOT CURSES_FOUND)
        MESSAGE(FATAL_ERROR "Curses library not found.")
    ENDIF()

    IF(HAVE_NCURSES_H)
        SET(CURSES_HDR "ncurses.h")
    ELSEIF(HAVE_CURSES_H)
        SET(CURSES_HDR "curses.h")
    ELSEIF(HAVE_NCURSES_CURSES_H)
        SET(CURSES_HDR "ncurses/curses.h")
    ELSE()
        MESSAGE(FATAL_ERROR "Neither curses.h nor ncurses.h found.")
    ENDIF()

    SET(CMAKE_REQUIRED_LIBRARIES ${CURSES_LIBRARY})
    CHECK_FUNCTION_EXISTS(resizeterm HAVE_NCURSES_RESIZETERM)
    CHECK_C_SOURCE_COMPILES(
        "#include <${CURSES_HDR}>
        int main(void) {
        #ifdef ACS_ULCORNER
        return ACS_ULCORNER;
        #endif
        return 0;
        }"
        CURSES_LINKSOK
    )

    IF(CURSES_LINKSOK)
        LIST(APPEND EXTRA_LIBS ${CURSES_LIBRARY})
    ELSE()
        FIND_LIBRARY(TINFO_LIBRARY tinfo)
        IF(TINFO_LIBRARY STREQUAL "TINFO_LIBRARY-NOTFOUND")
            MESSAGE(FATAL_ERROR "libtinfo needed for ncurses, but not found")
        ELSE()
            MESSAGE(STATUS "Found libtinfo: ${TINFO_LIBRARY}")
            SET(CMAKE_REQUIRED_LIBRARIES ${CURSES_LIBRARY} ${TINFO_LIBRARY})
            CHECK_C_SOURCE_COMPILES(
                "#include <${CURSES_HDR}>
                int main(void) {
                #ifdef ACS_ULCORNER
                return ACS_ULCORNER;
                #endif
                return 0;
                }"
                TINFO_LINKSOK
            )
            
            IF(TINFO_LINKSOK)
                LIST(APPEND EXTRA_LIBS ${CURSES_LIBRARY})
                LIST(APPEND EXTRA_LIBS ${TINFO_LIBRARY})
            ELSE()
                MESSAGE(FATAL_ERROR "failed linking to ncruses library.")
            ENDIF()
        ENDIF()
    ENDIF()
ENDIF()


SET(HAVE_PTHREAD)
IF (NOT WIN32)
 INCLUDE(FindThreads)
 IF (CMAKE_USE_PTHREADS_INIT)
    SET (HAVE_PTHREAD 1)
    IF (CMAKE_THREAD_LIBS_INIT)
        LIST (APPEND EXTRA_LIBS ${CMAKE_THREAD_LIBS_INIT})
    ENDIF()
 ENDIF()
ENDIF()

CHECK_MULTI_FUNCTIONS_EXISTS(
    mkstemp srandom snprintf vsnprintf strerror
    usleep
    srandom
    fnmatch
    memcmp
    vprintf
)

CHECK_C_SOURCE_COMPILES(
 "#include <sys/types.h>
  #include <signal.h>
  int main(void) { return *(signal(0,0))(0) == 1; }"
  RETSIGTYPE_INT
)
if(RETSIGTYPE_INT)
  set(RETSIGTYPE int)
else()
  set(RETSIGTYPE void)
endif()
MESSAGE(STATUS "Return type of signal handlers: ${RETSIGTYPE}")

SET(CMAKE_REQUIRED_INCLUDES ${MIKMOD_INCLUDE_DIR})
SET(CMAKE_REQUIRED_LIBRARIES ${MIKMOD_LIBRARIES})
CHECK_FUNCTION_EXISTS (MikMod_free HAVE_MIKMOD_FREE)

########### compiler flags ##############

SET(COMPILER_FLAGS_TO_CHECK
    "-Wall" "-Werror=implicit-function-declaration"
)

IF (CPU_ARCH)
    LIST(APPEND COMPILER_FLAGS_TO_CHECK "-march=${CPU_ARCH}")
ENDIF(CPU_ARCH)

########### install files ###############

ADD_DEFINITIONS("-DHAVE_CONFIG_H")
SET (PACKAGE_DATA_DIR "${CMAKE_INSTALL_PREFIX}/share/mikmod")

CONFIGURE_FILE(${CMAKE_SOURCE_DIR}/config.h.cmake ${CMAKE_BINARY_DIR}/config.h) # So it can find config.h
INCLUDE_DIRECTORIES(BEFORE ${CMAKE_SOURCE_DIR})
INCLUDE_DIRECTORIES(BEFORE ${CMAKE_BINARY_DIR})
INCLUDE_DIRECTORIES(./include ./src ./libs/nuklear ./libs/nuklear/src)

INSTALL(
    FILES
        tplayerrc
    DESTINATION
        "share/playerrc"
)

#ADD_SUBDIRECTORY(src)

######### targets ###########

SET(tplayer_SRCS
    ./src/main.cpp
    ./src/gui.cpp
)

INCLUDE_DIRECTORIES(${MIKMOD_INCLUDE_DIR})

ADD_EXECUTABLE(tplayer ${tplayer_SRCS})

TARGET_LINK_LIBRARIES(tplayer ${MIKMOD_LIBRARIES} ${EXTRA_LIBS})

INSTALL(TARGETS tplayer DESTINATION bin)



















# flags

# files

# includes
#include_directories( ./include ./src ./libs/libmikmod/include )

# target
#add_executable( tplayer ./src/main.cpp )

# external libs
#add_subdirectory( ./libs/libmikmod )
#target_link_libraries( tplayer libmikmod )

#add_library(libmikmod STATIC IMPORTED)
#set_property(TARGET libmikmod PROPERTY IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/libs/libmikmod/build/libmikmod.a)
#target_link_libraries(tplayer libmikmod)

