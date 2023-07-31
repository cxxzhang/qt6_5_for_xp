# Copyright (C) 2022 The Qt Company Ltd.
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.16)
project(hierarchyax LANGUAGES CXX)

set(CMAKE_AUTOMOC ON)

if(NOT DEFINED INSTALL_EXAMPLESDIR)
    set(INSTALL_EXAMPLESDIR "examples")
endif()

set(INSTALL_EXAMPLEDIR "${INSTALL_EXAMPLESDIR}/activeqt/hierarchy")

find_package(Qt6 REQUIRED COMPONENTS AxServer Core Gui Widgets)

qt6_add_axserver_library(hierarchyax
    main.cpp
    objects.cpp objects.h
    hierarchy.def
    hierarchy.rc
)

target_link_libraries(hierarchyax PUBLIC
    Qt::Core
    Qt::Gui
    Qt::Widgets
)

install(TARGETS hierarchyax
    RUNTIME DESTINATION "${INSTALL_EXAMPLEDIR}"
    BUNDLE DESTINATION "${INSTALL_EXAMPLEDIR}"
    LIBRARY DESTINATION "${INSTALL_EXAMPLEDIR}"
)
