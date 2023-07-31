// Copyright (C) 2022 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "qcocoamessagedialog.h"

#include "qcocoawindow.h"
#include "qcocoahelpers.h"

#include <QtCore/qmetaobject.h>
#include <QtCore/qscopedvaluerollback.h>
#include <QtCore/qtimer.h>

#include <QtGui/qtextdocument.h>
#include <QtGui/private/qguiapplication_p.h>
#include <QtGui/private/qcoregraphics_p.h>
#include <QtGui/qpa/qplatformtheme.h>

#include <AppKit/NSAlert.h>
#include <AppKit/NSButton.h>

QT_USE_NAMESPACE

using namespace Qt::StringLiterals;

QT_BEGIN_NAMESPACE

QCocoaMessageDialog::~QCocoaMessageDialog()
{
    hide();
    [m_alert release];
}

static QString toPlainText(const QString &text)
{
    // FIXME: QMessageDialog supports Qt::TextFormat, which
    // nowadays includes Qt::MarkdownText, but we don't have
    // the machinery to deal with that yet. We should as a
    // start plumb the dialog's text format to the platform
    // via the dialog options.

    if (!Qt::mightBeRichText(text))
        return text;

    QTextDocument textDocument;
    textDocument.setHtml(text);
    return textDocument.toPlainText();
}

static NSControlStateValue controlStateFor(Qt::CheckState state)
{
    switch (state) {
    case Qt::Checked: return NSControlStateValueOn;
    case Qt::Unchecked: return NSControlStateValueOff;
    case Qt::PartiallyChecked: return NSControlStateValueMixed;
    }
    Q_UNREACHABLE();
}

/*
    Called from QDialogPrivate::setNativeDialogVisible() when the message box
    is ready to be shown.

    At this point the options() will reflect the specific dialog shown.

    Returns true if the helper could successfully show the dialog, or
    false if the cross platform fallback dialog should be used instead.
*/
bool QCocoaMessageDialog::show(Qt::WindowFlags windowFlags, Qt::WindowModality windowModality, QWindow *parent)
{
    Q_UNUSED(windowFlags);

    qCDebug(lcQpaDialogs) << "Asked to show" << windowModality << "dialog with parent" << parent;

    if (m_alert.window.visible) {
        qCDebug(lcQpaDialogs) << "Dialog already visible, ignoring request to show";
        return true; // But we don't want to  show the fallback dialog instead
    }

    // We can only do application and window modal dialogs
    if (windowModality == Qt::NonModal)
        return false;

    // And only window modal if we have a parent
    if (windowModality == Qt::WindowModal && (!parent || !parent->handle())) {
        qCWarning(lcQpaDialogs, "Cannot run window modal dialog without parent window");
        return false;
    }

    // And without options we don't know what to show
    if (!options())
        return false;

    if (windowModality == Qt::ApplicationModal && QThread::currentThread()->loopLevel() > 1) {
        qCWarning(lcQpaDialogs, "Cannot use native application modal dialog from nested event loop");
        return false;
    }

    Q_ASSERT(!m_alert);
    m_alert = [NSAlert new];
    m_alert.window.title = options()->windowTitle().toNSString();

    QString text = toPlainText(options()->text());
    QString details = toPlainText(options()->detailedText());
    if (!details.isEmpty())
        text += u"\n\n"_s + details;
    m_alert.messageText = text.toNSString();
    m_alert.informativeText = toPlainText(options()->informativeText()).toNSString();

    switch (auto standardIcon = options()->icon()) {
    case QMessageDialogOptions::NoIcon: {
        // We only reflect the pixmap icon if the standard icon is unset,
        // as setting a standard icon will also set a corresponding pixmap
        // icon, which we don't want since it conflicts with the platform.
        // If the user has set an explicit pixmap icon however, the standard
        // icon will be NoIcon, so we're good.
        QPixmap iconPixmap = options()->iconPixmap();
        if (!iconPixmap.isNull())
            m_alert.icon = [NSImage imageFromQImage:iconPixmap.toImage()];
        break;
    }
    case QMessageDialogOptions::Information:
    case QMessageDialogOptions::Question:
        [m_alert setAlertStyle:NSAlertStyleInformational];
        break;
    case QMessageDialogOptions::Warning:
        [m_alert setAlertStyle:NSAlertStyleWarning];
        break;
    case QMessageDialogOptions::Critical:
        [m_alert setAlertStyle:NSAlertStyleCritical];
        break;
    }

    bool defaultButtonAdded = false;
    bool cancelButtonAdded = false;

    const auto addButton = [&](auto title, auto tag, auto role) {
        title = QPlatformTheme::removeMnemonics(title);
        NSButton *button = [m_alert addButtonWithTitle:title.toNSString()];

        // Calling addButtonWithTitle places buttons starting at the right side/top of the alert
        // and going toward the left/bottom. By default, the first button has a key equivalent of
        // Return, any button with a title of "Cancel" has a key equivalent of Escape, and any button
        // with the title "Don't Save" has a key equivalent of Command-D (but only if it's not the first
        // button). Unfortunately QMessageBox does not currently plumb setDefaultButton/setEscapeButton
        // through the dialog options, so we can't forward this information directly. The closest we
        // can get right now is to use the role to set the button's key equivalent.

        if (role == AcceptRole && !defaultButtonAdded) {
            button.keyEquivalent = @"\r";
            defaultButtonAdded = true;
        } else if (role == RejectRole && !cancelButtonAdded) {
            button.keyEquivalent = @"\e";
            cancelButtonAdded = true;
        }

        if (@available(macOS 11, *))
            button.hasDestructiveAction = role == DestructiveRole;

        // The NSModalResponse of showing an NSAlert normally depends on the order of the
        // button that was clicked, starting from the right with NSAlertFirstButtonReturn (1000),
        // NSAlertSecondButtonReturn (1001), NSAlertThirdButtonReturn (1002), and after that
        // NSAlertThirdButtonReturn + n. The response can also be customized per button via its
        // tag, which, following the above logic, can include any positive value from 1000 and up.
        // In addition the system reserves the values from -1000 and down for its own modal responses,
        // such as NSModalResponseStop, NSModalResponseAbort, and NSModalResponseContinue.
        // Luckily for us, the QPlatformDialogHelper::StandardButton enum values all fall within
        // the positive range, so we can use the standard button value as the tag directly.
        // The same applies to the custom button IDs, as these are generated in sequence after
        // the QPlatformDialogHelper::LastButton.
        Q_ASSERT(tag >= NSAlertFirstButtonReturn);
        button.tag = tag;
    };

    const auto *platformTheme = QGuiApplicationPrivate::platformTheme();
    if (auto standardButtons = options()->standardButtons()) {
        for (int standardButton = FirstButton; standardButton < LastButton; standardButton <<= 1) {
            if (standardButtons & standardButton) {
                auto title = platformTheme->standardButtonText(standardButton);
                addButton(title, standardButton, buttonRole(StandardButton(standardButton)));
            }
        }
    }

    const auto customButtons = options()->customButtons();
    for (auto customButton : customButtons)
        addButton(customButton.label, customButton.id, customButton.role);


    // QMessageDialog's logic for adding a fallback OK button if no other buttons
    // are added depends on QMessageBox::showEvent(), which is too late when
    // native dialogs are in use. To ensure there's always an OK button with a tag
    // we recognize we add it explicitly here as a fallback.
    if (!m_alert.buttons.count) {
        addButton(platformTheme->standardButtonText(StandardButton::Ok),
            StandardButton::Ok, ButtonRole::AcceptRole);
    }

    if (auto checkBoxLabel = options()->checkBoxLabel(); !checkBoxLabel.isNull()) {
        checkBoxLabel = QPlatformTheme::removeMnemonics(checkBoxLabel);
        m_alert.suppressionButton.title = checkBoxLabel.toNSString();
        auto state = options()->checkBoxState();
        m_alert.suppressionButton.allowsMixedState = state == Qt::PartiallyChecked;
        m_alert.suppressionButton.state = controlStateFor(state);
        m_alert.showsSuppressionButton = YES;
    }

    qCDebug(lcQpaDialogs) << "Showing" << m_alert;

    if (windowModality == Qt::WindowModal) {
        auto *cocoaWindow = static_cast<QCocoaWindow*>(parent->handle());
        [m_alert beginSheetModalForWindow:cocoaWindow->nativeWindow()
            completionHandler:^(NSModalResponse response) {
                processResponse(response);
            }
        ];
    } else {
        // The dialog is application modal, so we need to call runModal,
        // but we can't call it here as the nativeDialogInUse state of QDialog
        // depends on the result of show(), and we can't rely on doing it
        // in exec(), as we can't guarantee that the user will call exec()
        // after showing the dialog. As a workaround, we call it from exec(),
        // but also make sure that if the user returns to the main runloop
        // we'll run the modal dialog from there.
        QTimer::singleShot(0, this, [this]{
            if (m_alert && NSApp.modalWindow != m_alert.window) {
                qCDebug(lcQpaDialogs) << "Running deferred modal" << m_alert;
                processResponse([m_alert runModal]);
            }
        });
    }

    return true;
}

void QCocoaMessageDialog::exec()
{
    Q_ASSERT(m_alert);

    if (modality() == Qt::WindowModal) {
        qCDebug(lcQpaDialogs) << "Running local event loop for window modal" << m_alert;
        QEventLoop eventLoop;
        QScopedValueRollback updateGuard(m_eventLoop, &eventLoop);
        m_eventLoop->exec(QEventLoop::DialogExec);
    } else {
        qCDebug(lcQpaDialogs) << "Running modal" << m_alert;
        processResponse([m_alert runModal]);
    }
}

// Custom modal response code to record that the dialog was hidden by us
static const NSInteger kModalResponseDialogHidden = NSAlertThirdButtonReturn + 1;

static Qt::CheckState checkStateFor(NSControlStateValue state)
{
    switch (state) {
    case NSControlStateValueOn: return Qt::Checked;
    case NSControlStateValueOff: return Qt::Unchecked;
    case NSControlStateValueMixed: return Qt::PartiallyChecked;
    }
    Q_UNREACHABLE();
}

void QCocoaMessageDialog::processResponse(NSModalResponse response)
{
    qCDebug(lcQpaDialogs) << "Processing response" << response << "for" << m_alert;

    // We can't re-use the same dialog for the next show() anyways,
    // since the options may have changed, so get rid of it now,
    // before we emit anything that might recurse back to hide/show/etc.
    auto alert = std::exchange(m_alert, nil);
    [alert autorelease];

    if (alert.showsSuppressionButton)
        emit checkBoxStateChanged(checkStateFor(alert.suppressionButton.state));

     if (response >= NSAlertFirstButtonReturn) {
        // Safe range for user-defined modal responses
        if (response == kModalResponseDialogHidden) {
            // Dialog was explicitly hidden by us, so nothing to report
            qCDebug(lcQpaDialogs) << "Dialog was hidden; ignoring response";
        } else {
            // Dialog buttons
            if (response <= StandardButton::LastButton) {
                Q_ASSERT(response >= StandardButton::FirstButton);
                auto standardButton = StandardButton(response);
                emit clicked(standardButton, buttonRole(standardButton));
            } else {
                auto *customButton = options()->customButton(response);
                Q_ASSERT(customButton);
                emit clicked(StandardButton(customButton->id), customButton->role);
            }
        }
    } else {
        // We have to consider NSModalResponses beyond the ones specific to
        // the alert buttons as the alert may be canceled programmatically.

        switch (response) {
        case NSModalResponseContinue:
            // Modal session is continuing (returned by runModalSession: only)
            Q_UNREACHABLE();
        case NSModalResponseOK:
            emit accept();
            break;
        case NSModalResponseCancel:
        case NSModalResponseStop: // Modal session was broken with stopModal
        case NSModalResponseAbort: // Modal session was broken with abortModal
            emit reject();
            break;
        default:
            qCWarning(lcQpaDialogs) << "Unrecognized modal response" << response;
        }
    }

    if (m_eventLoop)
        m_eventLoop->exit(response);
}

void QCocoaMessageDialog::hide()
{
    if (!m_alert)
        return;

    if (m_alert.window.visible) {
        qCDebug(lcQpaDialogs) << "Hiding" << modality() << m_alert;

        // Note: Just hiding or closing the NSAlert's NWindow here is not sufficient,
        // as the dialog is running a modal event loop as well, which we need to end.

        if (modality() == Qt::WindowModal) {
            // Will call processResponse() synchronously
            [m_alert.window.sheetParent endSheet:m_alert.window returnCode:kModalResponseDialogHidden];
        } else {
            if (NSApp.modalWindow == m_alert.window) {
                // Will call processResponse() asynchronously
                [NSApp stopModalWithCode:kModalResponseDialogHidden];
            } else {
                qCWarning(lcQpaDialogs, "Dialog is not top level modal window. Cannot hide.");
            }
        }
    } else {
        qCDebug(lcQpaDialogs) << "No need to hide already hidden" << m_alert;
        auto alert = std::exchange(m_alert, nil);
        [alert autorelease];
    }
}

Qt::WindowModality QCocoaMessageDialog::modality() const
{
    Q_ASSERT(m_alert && m_alert.window);
    return m_alert.window.sheetParent ? Qt::WindowModal : Qt::ApplicationModal;
}

QT_END_NAMESPACE
