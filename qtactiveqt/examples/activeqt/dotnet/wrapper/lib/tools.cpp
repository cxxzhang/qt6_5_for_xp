// Copyright (C) 2015 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

//! [0]
#include <QString>

#using <mscorlib.dll>
#include <vcclr.h>

using namespace System;

String *QStringToString(const QString &qstring)
{
    return new String((const wchar_t *)qstring.utf16());
}
//! [0] //! [1]

QString StringToQString(String *string)
{
    const wchar_t __pin *chars = PtrToStringChars(string);
    return QString::fromWCharArray(chars);
}
//! [1]
