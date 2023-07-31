// Copyright (C) 2019 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QQMLMETAOBJECT_P_H
#define QQMLMETAOBJECT_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include <private/qqmlpropertycache_p.h>

#include <QtQml/qtqmlglobal.h>
#include <QtCore/qvarlengtharray.h>
#include <QtCore/qmetaobject.h>

QT_BEGIN_NAMESPACE

// QQmlMetaObject serves as a wrapper around either QMetaObject or QQmlPropertyCache.
// This is necessary as we delay creation of QMetaObject for synthesized QObjects, but
// we don't want to needlessly generate QQmlPropertyCaches every time we encounter a
// QObject type used in assignment or when we don't have a QQmlEngine etc.
//
// This class does NOT reference the propertycache.
class QQmlEnginePrivate;
class QQmlPropertyData;
class Q_QML_EXPORT QQmlMetaObject
{
public:
    typedef QVarLengthArray<QMetaType, 9> ArgTypeStorage;

    inline QQmlMetaObject() = default;
    inline QQmlMetaObject(const QObject *);
    inline QQmlMetaObject(const QMetaObject *);
    inline QQmlMetaObject(const QQmlPropertyCache::ConstPtr &);
    inline QQmlMetaObject(const QQmlMetaObject &);

    inline QQmlMetaObject &operator=(const QQmlMetaObject &);

    inline bool isNull() const;

    inline const char *className() const;
    inline int propertyCount() const;

    inline const QMetaObject *metaObject() const;

    QMetaType methodReturnType(const QQmlPropertyData &data, QByteArray *unknownTypeError) const;
    /*!
      \internal
      Returns false if one of the types is unknown. Otherwise, fills \a argstorage with the
      metatypes of the function.
    */
    bool methodParameterTypes(int index, ArgTypeStorage *argStorage,
                              QByteArray *unknownTypeError) const;
    /*!
      \internal
      Returns false if one of the types is unknown. Otherwise, fills \a argstorage with the
      metatypes of the function.
    */
    bool constructorParameterTypes(int index, ArgTypeStorage *dummy, QByteArray *unknownTypeError) const;


    static bool canConvert(const QQmlMetaObject &from, const QQmlMetaObject &to);

    // static_metacall (on Gadgets) doesn't call the base implementation and therefore
    // we need a helper to find the correct meta object and property/method index.
    static void resolveGadgetMethodOrPropertyIndex(QMetaObject::Call type, const QMetaObject **metaObject, int *index);

    static bool methodParameterTypes(const QMetaMethod &method, ArgTypeStorage *argStorage,
                              QByteArray *unknownTypeError);
protected:
    const QMetaObject *_m = nullptr;

};

QQmlMetaObject::QQmlMetaObject(const QObject *o)
{
    if (o)
        _m = o->metaObject();
}

QQmlMetaObject::QQmlMetaObject(const QMetaObject *m)
    : _m(m)
{
}

QQmlMetaObject::QQmlMetaObject(const QQmlPropertyCache::ConstPtr &m)
{
    if (m)
        _m = m->createMetaObject();
}

QQmlMetaObject::QQmlMetaObject(const QQmlMetaObject &o)
    : _m(o._m)
{
}

QQmlMetaObject &QQmlMetaObject::operator=(const QQmlMetaObject &o)
{
    _m = o._m;
    return *this;
}

bool QQmlMetaObject::isNull() const
{
    return !_m;
}

const char *QQmlMetaObject::className() const
{
    if (!_m)
        return nullptr;
    return metaObject()->className();
}

int QQmlMetaObject::propertyCount() const
{
    if (!_m)
        return 0;
    return metaObject()->propertyCount();
}

const QMetaObject *QQmlMetaObject::metaObject() const
{
    return _m;
}

QT_END_NAMESPACE

#endif // QQMLMETAOBJECT_P_H
