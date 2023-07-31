# Copyright (C) 2022 The Qt Company Ltd.
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.16)
project(openglax LANGUAGES CXX)

set(CMAKE_AUTOMOC ON)

if(NOT DEFINED INSTALL_EXAMPLESDIR)
    set(INSTALL_EXAMPLESDIR "examples")
endif()

set(INSTALL_EXAMPLEDIR "${INSTALL_EXAMPLESDIR}/activeqt/opengl")

find_package(Qt6 REQUIRED COMPONENTS AxServer Core Gui OpenGL OpenGLWidgets Widgets)

qt6_add_axserver_executable(openglax
    glbox.cpp glbox.h
    globjwin.cpp globjwin.h
    main.cpp
    opengl.def
    opengl.rc
)

target_link_libraries(openglax PUBLIC
    Qt::Core
    Qt::Gui
    Qt::OpenGL
    Qt::OpenGLWidgets
    Qt::Widgets
)

install(TARGETS openglax
    RUNTIME DESTINATION "${INSTALL_EXAMPLEDIR}"
    BUNDLE DESTINATION "${INSTALL_EXAMPLEDIR}"
    LIBRARY DESTINATION "${INSTALL_EXAMPLEDIR}"
)
