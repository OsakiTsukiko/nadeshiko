cmake_minimum_required(VERSION 3.20)

# ARGUMENTS
option(PLATFORM "PLATFORM" null)
option(LIBTYPE "LIBTYPE" "shared")
# set(ENV{LD_RUN_PATH} '$ORIGIN/lib')

project(nadeshiko)

if (${PLATFORM} STREQUAL "linux")
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(GTK REQUIRED gtk+-3.0)
    pkg_check_modules(WEBKIT REQUIRED webkit2gtk-4.0)

    set(SOURCES src/nadeshiko.c)

    if (${LIBTYPE} STREQUAL "static")
        add_library(${PROJECT_NAME} STATIC ${SOURCES})
    else()
        add_library(${PROJECT_NAME} SHARED ${SOURCES})
    endif()
    # add_executable(${PROJECT_NAME} ${SOURCES})

    target_include_directories(${PROJECT_NAME} PRIVATE ${GTK_INCLUDE_DIRS} ${WEBKIT_INCLUDE_DIRS} src/thirdparty/include)
    target_link_directories(${PROJECT_NAME} PRIVATE src/thirdparty/lib)
    target_link_libraries(${PROJECT_NAME} PRIVATE ${GTK_LIBRARIES} ${WEBKIT_LIBRARIES} webview microhttpd)

# TODO: WIN AND MACOS TOO
else()
    message(FATAL_ERROR "Specify a platform with -DPLATFORM=linux|win|macos")
endif()
