// Copyright (C) 2015 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

//! [0]
#include "networker.h"
#include "worker.h"
#include "tools.h"

netWorker::netWorker()
{
    workerObject = new Worker();
}
//! [0] //! [1]

netWorker::~netWorker()
{
    delete workerObject;
}
//! [1] //! [2]

String *netWorker::get_StatusString()
{
    return QStringToString(workerObject->statusString());
}
//! [2] //! [3]

void netWorker::set_StatusString(String *string)
{
    workerObject->setStatusString(StringToQString(string));
    __raise statusStringChanged(string);
}
//! [3]
