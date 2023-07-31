// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

/****************************************************************************
**
** Copyright (c) 2007-2008, Apple, Inc.
**
** All rights reserved.
**
** Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are met:
**
**   * Redistributions of source code must retain the above copyright notice,
**     this list of conditions and the following disclaimer.
**
**   * Redistributions in binary form must reproduce the above copyright notice,
**     this list of conditions and the following disclaimer in the documentation
**     and/or other materials provided with the distribution.
**
**   * Neither the name of Apple, Inc. nor the names of its contributors
**     may be used to endorse or promote products derived from this software
**     without specific prior written permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
** CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
** EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
** PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
** PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
** LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
** NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
** SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
**
****************************************************************************/

#include "qcocoaintrospection.h"

QT_BEGIN_NAMESPACE

void qt_cocoa_change_implementation(Class baseClass, SEL originalSel, Class proxyClass, SEL replacementSel, SEL backupSel)
{
    // The following code replaces the _implementation_ for the selector we want to hack
    // (originalSel) with the implementation found in proxyClass. Then it creates
    // a new 'backup' method inside baseClass containing the old, original,
    // implementation (fakeSel). You can let the proxy implementation of originalSel
    // call fakeSel if needed (similar approach to calling a super class implementation).
    // fakeSel must also be implemented in proxyClass, as the signature is used
    // as template for the method one we add into baseClass.
    // NB: You will typically never create any instances of proxyClass; we use it
    // only for stealing its contents and put it into baseClass.
    if (!replacementSel)
        replacementSel = originalSel;

    Method originalMethod = class_getInstanceMethod(baseClass, originalSel);
    Method replacementMethod = class_getInstanceMethod(proxyClass, replacementSel);
    IMP originalImp = method_setImplementation(originalMethod, method_getImplementation(replacementMethod));

    if (backupSel) {
        Method backupMethod = class_getInstanceMethod(proxyClass, backupSel);
        class_addMethod(baseClass, backupSel, originalImp, method_getTypeEncoding(backupMethod));
    }
}

void qt_cocoa_change_back_implementation(Class baseClass, SEL originalSel, SEL backupSel)
{
    Method originalMethod = class_getInstanceMethod(baseClass, originalSel);
    Method backupMethodInBaseClass = class_getInstanceMethod(baseClass, backupSel);
    method_setImplementation(originalMethod, method_getImplementation(backupMethodInBaseClass));
}

QT_END_NAMESPACE