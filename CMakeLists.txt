#  :copyright: (c) 2015-2016 The Regents of the University of California.
#  :license: BSD, see LICENSE.md for more details.

cmake_minimum_required(VERSION 2.8)
project(AutoCOM CXX)

# FLAGS
# -----

set(CMAKE_CXX_STANDARD 14)

# OPTIONS
# -------

if(NOT WIN32)
    message(FATAL_ERROR "COM interface only works on Windows")
endif()

option(BUILD_EXAMPLES "Build example files" ON)
option(BUILD_EXECUTABLE "Build AutoCOM executable" ON)
option(BUILD_STATIC "Build static library" ON)
option(BUILD_TESTS "Build unittests (requires GTest)" OFF)
option(HAVE_THERMO "Have Thermo MSFileReader for examples" OFF)
option(HAVE_SCRIPTCONTROL "Have MSScriptControl for examples" OFF)

if(BUILD_STATIC)
    if (MINGW)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static")
    elseif(MSVC)
        set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_RELEASE} /MTd /O0")
        set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT /Ox")
    endif()
endif()

# LIBRARY
# -------

set(AUTOCOM_SOURCES
    src/encoding/converters.cpp
    src/encoding/unicode.cpp
    src/util/alias.cpp
    src/util/exception.cpp
    src/util/type.cpp
    src/bstr.cpp
    src/com.cpp
    src/dispparams.cpp
    src/dispatch.cpp
    src/enum.cpp
    src/iterator.cpp
    src/guid.cpp
    src/safearray.cpp
    src/typeinfo.cpp
    src/variant.cpp
)

set(AUTOCOM_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include")
set(AUTOCOM_INCLUDE_DIRS ${AUTOCOM_INCLUDE_DIR})
set(ITL_INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/itl/include")
include_directories(${AUTOCOM_INCLUDE_DIRS} ${ITL_INCLUDE_DIRS})

if(BUILD_STATIC)
    add_library(AutoCOM STATIC ${AUTOCOM_SOURCES})
else()
    add_library(AutoCOM SHARED ${AUTOCOM_SOURCES})
endif()

set_target_properties(AutoCOM PROPERTIES OUTPUT_NAME autocom)
set(AUTOCOM_LIBRARIES AutoCOM)
if(MSVC)
    list(APPEND AUTOCOM_LIBRARIES ole32.lib oleaut32.lib uuid.lib)
elseif(MINGW)
    list(APPEND AUTOCOM_LIBRARIES -lole32 -loleaut32 -luuid)
endif()

# BIN
# ----

set(AUTOCOM_EXECUTABLE_SOURCES
    bin/autocom.cpp
    bin/options.cpp
    bin/parse.cpp
    bin/write.cpp
)

if(BUILD_EXECUTABLE)
    if(NOT TARGET gflags)
        add_subdirectory(gflags)
    endif()
    include_directories("${CMAKE_CURRENT_BINARY_DIR}/gflags/include")
    add_executable(AutoCOMConfigure ${AUTOCOM_EXECUTABLE_SOURCES})
    set_target_properties(AutoCOMConfigure PROPERTIES OUTPUT_NAME autocom)
    target_link_libraries(AutoCOMConfigure ${AUTOCOM_LIBRARIES} gflags)
endif()

# EXAMPLES
# --------

set(AUTOCOM_LATEBINDING_EXAMPLE_SOURCES
    example/late/regexp.cpp
    example/late/thermo.cpp
    example/late/wsh.cpp
)

# EARLY
if (BUILD_EXAMPLES)
    set(OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}")
    if(MSVC)
        if(CMAKE_BUILD_TYPE MATCHES Release)
            set(OUTPUT_DIRECTORY "${OUTPUT_DIRECTORY}/Release")
        else()
            set(OUTPUT_DIRECTORY "${OUTPUT_DIRECTORY}/Debug")
        endif()
    endif()

    file(COPY example/Thermo.raw
        DESTINATION ${OUTPUT_DIRECTORY}
    )

    foreach(source ${AUTOCOM_LATEBINDING_EXAMPLE_SOURCES})
        get_filename_component(example ${source} NAME_WE)
        set(target "AutoCOMLate_${example}")
        add_executable(${target} "${source}")
        set_target_properties(${target} PROPERTIES OUTPUT_NAME ${example}_late)
        target_link_libraries(${target} ${AUTOCOM_LIBRARIES})
    endforeach(source)
endif()

# LATE
if (BUILD_EXAMPLES AND BUILD_EXECUTABLE)
    include_directories("${CMAKE_CURRENT_BINARY_DIR}")

    # WScript
    add_custom_target(WScript
        COMMAND $<TARGET_FILE:AutoCOMConfigure> -progid="WScript.Shell.1"
        DEPENDS AutoCOMConfigure)
    add_executable(wsh_early example/early/wsh.cpp)
    target_link_libraries(wsh_early ${AUTOCOM_LIBRARIES})
    add_dependencies(wsh_early "WScript")

    # MSScriptControl
    if(HAVE_SCRIPTCONTROL)
        add_custom_target(MSScriptControl
            COMMAND $<TARGET_FILE:AutoCOMConfigure> -progid="MSScriptControl.ScriptControl"
            DEPENDS AutoCOMConfigure)
        add_executable(script_early example/early/script.cpp)
        target_link_libraries(script_early ${AUTOCOM_LIBRARIES})
        add_dependencies(script_early "MSScriptControl")
    endif()

    # Thermo
    if (HAVE_THERMO)
        add_custom_target(MSFileReader
            COMMAND $<TARGET_FILE:AutoCOMConfigure> -progid="{1D23188D-53FE-4C25-B032-DC70ACDBDC02}"
            DEPENDS AutoCOMConfigure)
        add_executable(thermo_early example/early/thermo.cpp)
        target_link_libraries(thermo_early ${AUTOCOM_LIBRARIES})
        add_dependencies(thermo_early "MSFileReader")
    endif()
endif()

# TESTS
# -----

set(AUTOCOM_TEST_SOURCES
    test/bin/parse.cpp
    test/src/encoding/converters.cpp
    test/src/encoding/unicode.cpp
    test/src/util/alias.cpp
    test/src/util/type.cpp
    test/src/bstr.cpp
    test/src/dispparams.cpp
    test/src/guid.cpp
    test/src/safearray.cpp
    test/src/variant.cpp
    test/src/main.cpp

    # GENERATOR
    bin/parse.cpp
    bin/write.cpp
)

if (BUILD_TESTS)
    if(NOT TARGET gtest)
        add_subdirectory(googletest)
    endif()
     include_directories("${CMAKE_CURRENT_SOURCE_DIR}/bin")
     add_executable(AutoCOMTests ${AUTOCOM_TEST_SOURCES})
     target_link_libraries(AutoCOMTests
        gtest
        gtest_main
        ${AUTOCOM_LIBRARIES}
    )

     add_custom_target(check_autocom
        COMMAND $<TARGET_FILE:AutoCOMTests>
        DEPENDS AutoCOMTests
    )

endif()
