cmake_minimum_required(VERSION 3.25)
project(app_ftp)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    set(QT_VERSION 6)
    set(REQUIRED_LIBS Core Sql Widgets Gui Xml Network Core5Compat)
    set(REQUIRED_LIBS_QUALIFIED Qt6::Core Qt6::Sql Qt6::Widgets Qt6::Gui Qt6::Xml Qt6::Network Qt6::Core5Compat)
    set(MACOSX TRUE)

    add_library(app_ftp SHARED
            src/ftp_handler.h
            src/module.cpp
            src/module.h
    )
    target_link_libraries(app_ftp ${REQUIRED_LIBS_QUALIFIED})
    find_package(Boost 1.83.0 REQUIRED)
    find_package(Qt${QT_VERSION} COMPONENTS ${REQUIRED_LIBS} REQUIRED)
    target_link_libraries(${PROJECT_NAME} ${REQUIRED_LIBS_QUALIFIED})
    include_directories(../../include/..)
    include_directories(../../include/tcp/..)
endif ()
if (WIN32)
    set(QT_VERSION 6)
    set(REQUIRED_LIBS Core Sql Widgets Gui Network Xml WinExtras)
    set(REQUIRED_LIBS_QUALIFIED Qt6::Core Qt6::Sql Qt6::Widgets Qt6::Gui Qt6::Network Qt6::Xml Qt6::WinExtras)
    find_package(Boost 1.68.0 REQUIRED)
    INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIR})
    add_library(app_ftp SHARED
            src/ftp_handler.h
            src/module.cpp
            src/module.h
    )
    #    target_link_libraries(app_ftp c:/local/boost_1_68_0/stage/x86/lib/libboost_system-vc141-mt-gd-x32-1_68.lib)
    target_link_libraries(app_ftp ${REQUIRED_LIBS_QUALIFIED})
    find_package(Qt${QT_VERSION} COMPONENTS ${REQUIRED_LIBS} REQUIRED)
    target_link_libraries(${PROJECT_NAME} ${REQUIRED_LIBS_QUALIFIED})
endif ()
