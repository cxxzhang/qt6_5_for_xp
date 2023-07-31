// Copyright (C) 2015 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

#include "worker.h"
#include "tools.h"

Worker::Worker()
{
    status = "Idle";
}

void Worker::setStatusString(const QString &string)
{
    status = string;
    emit statusStringChanged(status);
}

QString Worker::statusString() const
{
    return status;
}
