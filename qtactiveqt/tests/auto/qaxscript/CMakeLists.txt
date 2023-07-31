# Copyright (C) 2022 The Qt Company Ltd.
# SPDX-License-Identifier: BSD-3-Clause

#####################################################################
## qaxscript Test:
#####################################################################

qt_internal_add_test(qaxscript
    SOURCES
        tst_qaxscript.cpp
    LIBRARIES
        Qt::AxContainer
)
