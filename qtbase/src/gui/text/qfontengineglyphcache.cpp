// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include <private/qfontengineglyphcache_p.h>

QT_BEGIN_NAMESPACE

// out-of-line to avoid vtable duplication, breaking e.g. RTTI
QFontEngineGlyphCache::~QFontEngineGlyphCache()
{
}

QT_END_NAMESPACE