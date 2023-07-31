// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only WITH Qt-GPL-exception-1.0

#include <QtTest/QtTest>
#include "ieframe.h" // generated header
#include <QApplication>

class tst_dumpcpp : public QObject
{
    Q_OBJECT

private slots:
    void toggleAddressBar();
};

// A simple test to verify that an object can be instantiated and interacted with
void tst_dumpcpp::toggleAddressBar()
{
    SHDocVw::WebBrowser* webBrowser = new SHDocVw::WebBrowser;
    QVERIFY(webBrowser);
    bool addressBar = webBrowser->AddressBar();
    addressBar = !addressBar;
    webBrowser->SetAddressBar(addressBar);
    QVERIFY(webBrowser->AddressBar() == addressBar);
    delete webBrowser;
}

QTEST_MAIN(tst_dumpcpp)
#include "tst_dumpcpp.moc"
