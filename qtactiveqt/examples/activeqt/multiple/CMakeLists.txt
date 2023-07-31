# Copyright (C) 2022 The Qt Company Ltd.
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.16)
project(multipleax LANGUAGES CXX)

set(CMAKE_AUTOMOC ON)

if(NOT DEFINED INSTALL_EXAMPLESDIR)
    set(INSTALL_EXAMPLESDIR "examples")
endif()

set(INSTALL_EXAMPLEDIR "${INSTALL_EXAMPLESDIR}/activeqt/multiple")

find_package(Qt6 REQUIRED COMPONENTS AxServer Core Gui Widgets)

qt6_add_axserver_library(multipleax
    ax1.h
    ax2.h
    main.cpp
    multipleax.def
    multipleax.rc
)

target_link_libraries(multipleax PUBLIC
    Qt::Core
    Qt::Gui
    Qt::Widgets
)

install(TARGETS multipleax
    RUNTIME DESTINATION "${INSTALL_EXAMPLEDIR}"
    BUNDLE DESTINATION "${INSTALL_EXAMPLEDIR}"
    LIBRARY DESTINATION "${INSTALL_EXAMPLEDIR}"
)
