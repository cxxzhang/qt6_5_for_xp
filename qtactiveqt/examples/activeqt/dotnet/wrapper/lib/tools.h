// Copyright (C) 2015 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

#ifndef TOOLS_H
#define TOOLS_H

#using <mscorlib.dll>

QT_BEGIN_NAMESPACE
class QString;
QT_END_NAMESPACE

System::String *QStringToString(const QString &qstring);
QString StringToQString(System::String *string);

#endif // TOOLS_H
