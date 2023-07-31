// Copyright (C) 2021 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

#ifndef BIRTHDAYPARTY_H
#define BIRTHDAYPARTY_H

#include <QObject>
#include <QQmlListProperty>
#include "person.h"

class BirthdayParty : public QObject
{
    Q_OBJECT
// ![0]
    Q_PROPERTY(Person *host READ host WRITE setHost)
    Q_PROPERTY(QQmlListProperty<Person> guests READ guests)
    QML_ELEMENT
// ![0]
public:
    using QObject::QObject;

    Person *host() const;
    void setHost(Person *);

    QQmlListProperty<Person> guests();
    qsizetype guestCount() const;
    Person *guest(qsizetype) const;

private:
    Person *m_host = nullptr;
    QList<Person *> m_guests;
};

#endif // BIRTHDAYPARTY_H
