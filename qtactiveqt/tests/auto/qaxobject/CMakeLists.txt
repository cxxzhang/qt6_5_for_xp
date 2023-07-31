# Copyright (C) 2022 The Qt Company Ltd.
# SPDX-License-Identifier: BSD-3-Clause

#####################################################################
## qaxobject Test:
#####################################################################

qt_internal_add_test(qaxobject
    SOURCES
        tst_qaxobject.cpp
    LIBRARIES
        Qt::AxContainer
        Qt::Gui
)
