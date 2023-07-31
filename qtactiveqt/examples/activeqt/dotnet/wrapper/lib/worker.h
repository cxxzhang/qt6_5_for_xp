// Copyright (C) 2015 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

#ifndef WORKER_H
#define WORKER_H

#include <QObject>

// native Qt/C++ class
//! [0]
class Worker : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString statusString READ statusString WRITE setStatusString)
public:
    Worker();

    QString statusString() const;

public slots:
    void setStatusString(const QString &string);

signals:
    void statusStringChanged(const QString &string);

private:
    QString status;
};
//! [0]

#endif // WORKER_H
