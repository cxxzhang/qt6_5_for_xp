// Copyright (C) 2015 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

// lib.h

#pragma once

#using <mscorlib.dll>
using namespace System;

//! [0]
class Worker;

// .NET class
public __gc class netWorker
{
public:
    netWorker();
    ~netWorker();

    __property String *get_StatusString();
    __property void set_StatusString(String *string);

    __event void statusStringChanged(String *args);

private:
    Worker *workerObject;
};
//! [0]
