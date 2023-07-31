// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QWINDOWSCONTEXT_H
#define QWINDOWSCONTEXT_H

#include "qtwindowsglobal.h"
#include <QtCore/qt_windows.h>

#include <QtCore/qscopedpointer.h>
#include <QtCore/qsharedpointer.h>
#include <QtCore/qloggingcategory.h>
#include <QtCore/qoperatingsystemversion.h>
#include <QtCore/private/qsystemlibrary_p.h>

#define STRICT_TYPED_ITEMIDS
#include <shlobj.h>
#include <shlwapi.h>
#include <shellscalingapi.h>

QT_BEGIN_NAMESPACE

using namespace Qt::StringLiterals;

Q_DECLARE_LOGGING_CATEGORY(lcQpaWindow)
Q_DECLARE_LOGGING_CATEGORY(lcQpaEvents)
Q_DECLARE_LOGGING_CATEGORY(lcQpaGl)
Q_DECLARE_LOGGING_CATEGORY(lcQpaMime)
Q_DECLARE_LOGGING_CATEGORY(lcQpaInputMethods)
Q_DECLARE_LOGGING_CATEGORY(lcQpaDialogs)
Q_DECLARE_LOGGING_CATEGORY(lcQpaMenus)
Q_DECLARE_LOGGING_CATEGORY(lcQpaTablet)
Q_DECLARE_LOGGING_CATEGORY(lcQpaAccessibility)
Q_DECLARE_LOGGING_CATEGORY(lcQpaUiAutomation)
Q_DECLARE_LOGGING_CATEGORY(lcQpaTrayIcon)
Q_DECLARE_LOGGING_CATEGORY(lcQpaScreen)

class QWindow;
class QPlatformScreen;
class QPlatformWindow;
class QWindowsMenuBar;
class QWindowsScreenManager;
class QWindowsTabletSupport;
class QWindowsWindow;
class QWindowsMimeRegistry;
struct QWindowCreationContext;
struct QWindowsContextPrivate;
class QPoint;
class QKeyEvent;
class QPointingDevice;

struct QWindowsApi
{
    decltype(&::GetDpiForWindow) pGetDpiForWindow = nullptr;
    decltype(&::GetDpiForMonitor) pGetDpiForMonitor = nullptr;
    decltype(&::SystemParametersInfoForDpi) pSystemParametersInfoForDpi = nullptr;
    decltype(&::GetSystemMetricsForDpi) pGetSystemMetricsForDpi = nullptr;
    decltype(&::AdjustWindowRectExForDpi) pAdjustWindowRectExForDpi = nullptr;
    decltype(&::GetProcessDpiAwareness) pGetProcessDpiAwareness = nullptr;
    decltype(&::SetProcessDpiAwareness) pSetProcessDpiAwareness = nullptr;
    decltype(&::SetProcessDpiAwarenessContext) pSetProcessDpiAwarenessContext = nullptr;
    decltype(&::EnableNonClientDpiScaling) pEnableNonClientDpiScaling = nullptr;
    decltype(&::GetAwarenessFromDpiAwarenessContext) pGetAwarenessFromDpiAwarenessContext = nullptr;
    decltype(&::GetWindowDpiAwarenessContext) pGetWindowDpiAwarenessContext = nullptr;
    decltype(&::GetThreadDpiAwarenessContext) pGetThreadDpiAwarenessContext = nullptr;
    decltype(&::AreDpiAwarenessContextsEqual) pAreDpiAwarenessContextsEqual = nullptr;
    decltype(&::GetPointerType) pGetPointerType = nullptr;
    decltype(&::GetPointerInfo) pGetPointerInfo = nullptr;
    decltype(&::GetPointerFrameTouchInfo) pGetPointerFrameTouchInfo = nullptr;
    decltype(&::GetPointerFrameTouchInfoHistory) pGetPointerFrameTouchInfoHistory = nullptr;
    decltype(&::GetPointerPenInfo) pGetPointerPenInfo = nullptr;
    decltype(&::GetPointerPenInfoHistory) pGetPointerPenInfoHistory = nullptr;
    decltype(&::SkipPointerFrameMessages) pSkipPointerFrameMessages = nullptr;
    decltype(&::GetPointerDeviceRects) pGetPointerDeviceRects = nullptr;
    decltype(&::SetDisplayAutoRotationPreferences) pSetDisplayAutoRotationPreferences = nullptr;
    decltype(&::GetDisplayAutoRotationPreferences) pGetDisplayAutoRotationPreferences = nullptr;
    decltype(&::RegisterTouchWindow) pRegisterTouchWindow = nullptr;
    decltype(&::UnregisterTouchWindow) pUnregisterTouchWindow = nullptr;
    decltype(&::IsTouchWindow) pIsTouchWindow = nullptr;
    decltype(&::GetDisplayConfigBufferSizes) pGetDisplayConfigBufferSizes = nullptr;
    decltype(&::QueryDisplayConfig) pQueryDisplayConfig = nullptr;
    decltype(&::DisplayConfigGetDeviceInfo) pDisplayConfigGetDeviceInfo = nullptr;
    decltype(&::GetTouchInputInfo) pGetTouchInputInfo = nullptr;
    decltype(&::CloseTouchInputHandle) pCloseTouchInputHandle = nullptr;
    decltype(&::ChangeWindowMessageFilterEx) pChangeWindowMessageFilterEx = nullptr;
    decltype(&::Shell_NotifyIconGetRect) pShell_NotifyIconGetRect = nullptr;
    decltype(&::IsValidDpiAwarenessContext) pIsValidDpiAwarenessContext = nullptr;

    static QWindowsApi *instance()
    {
        static QWindowsApi api;
        return &api;
    }

    bool supportsPointerApi() const
    {
        return pGetPointerType && pGetPointerInfo && pGetPointerFrameTouchInfo
            && pGetPointerFrameTouchInfoHistory && pGetPointerPenInfo
            && pGetPointerPenInfoHistory && pSkipPointerFrameMessages
            && pGetPointerDeviceRects;
    }

private:
    Q_DISABLE_COPY_MOVE(QWindowsApi)

    explicit QWindowsApi()
    {
        if (QOperatingSystemVersion::isWin7OrGreater()) {
            QSystemLibrary shell32(u"shell32"_s);
            pShell_NotifyIconGetRect = reinterpret_cast<decltype(pShell_NotifyIconGetRect)>(shell32.resolve("Shell_NotifyIconGetRect"));

            QSystemLibrary user32(u"user32"_s);
            pRegisterTouchWindow = reinterpret_cast<decltype(pRegisterTouchWindow)>(user32.resolve("RegisterTouchWindow"));
            pUnregisterTouchWindow = reinterpret_cast<decltype(pUnregisterTouchWindow)>(user32.resolve("UnregisterTouchWindow"));
            pIsTouchWindow = reinterpret_cast<decltype(pIsTouchWindow)>(user32.resolve("IsTouchWindow"));
            pGetDisplayConfigBufferSizes = reinterpret_cast<decltype(pGetDisplayConfigBufferSizes)>(user32.resolve("GetDisplayConfigBufferSizes"));
            pQueryDisplayConfig = reinterpret_cast<decltype(pQueryDisplayConfig)>(user32.resolve("QueryDisplayConfig"));
            pDisplayConfigGetDeviceInfo = reinterpret_cast<decltype(pDisplayConfigGetDeviceInfo)>(user32.resolve("DisplayConfigGetDeviceInfo"));
            pGetTouchInputInfo = reinterpret_cast<decltype(pGetTouchInputInfo)>(user32.resolve("GetTouchInputInfo"));
            pCloseTouchInputHandle = reinterpret_cast<decltype(pCloseTouchInputHandle)>(user32.resolve("CloseTouchInputHandle"));
            pChangeWindowMessageFilterEx = reinterpret_cast<decltype(pChangeWindowMessageFilterEx)>(user32.resolve("ChangeWindowMessageFilterEx"));

            if (QOperatingSystemVersion::isWin8OrGreater()) {
                pGetPointerType = reinterpret_cast<decltype(pGetPointerType)>(user32.resolve("GetPointerType"));
                pGetPointerInfo = reinterpret_cast<decltype(pGetPointerInfo)>(user32.resolve("GetPointerInfo"));
                pGetPointerFrameTouchInfo = reinterpret_cast<decltype(pGetPointerFrameTouchInfo)>(user32.resolve("GetPointerFrameTouchInfo"));
                pGetPointerFrameTouchInfoHistory = reinterpret_cast<decltype(pGetPointerFrameTouchInfoHistory)>(user32.resolve("GetPointerFrameTouchInfoHistory"));
                pGetPointerPenInfo = reinterpret_cast<decltype(pGetPointerPenInfo)>(user32.resolve("GetPointerPenInfo"));
                pGetPointerPenInfoHistory = reinterpret_cast<decltype(pGetPointerPenInfoHistory)>(user32.resolve("GetPointerPenInfoHistory"));
                pSkipPointerFrameMessages = reinterpret_cast<decltype(pSkipPointerFrameMessages)>(user32.resolve("SkipPointerFrameMessages"));
                pGetPointerDeviceRects = reinterpret_cast<decltype(pGetPointerDeviceRects)>(user32.resolve("GetPointerDeviceRects"));
                pSetDisplayAutoRotationPreferences = reinterpret_cast<decltype(pSetDisplayAutoRotationPreferences)>(user32.resolve("SetDisplayAutoRotationPreferences"));
                pGetDisplayAutoRotationPreferences = reinterpret_cast<decltype(pGetDisplayAutoRotationPreferences)>(user32.resolve("GetDisplayAutoRotationPreferences"));

                if (QOperatingSystemVersion::isWin8Point1OrGreater()) {
                    QSystemLibrary shcore(u"shcore"_s);
                    pGetDpiForMonitor = reinterpret_cast<decltype(pGetDpiForMonitor)>(shcore.resolve("GetDpiForMonitor"));
                    pGetProcessDpiAwareness = reinterpret_cast<decltype(pGetProcessDpiAwareness)>(shcore.resolve("GetProcessDpiAwareness"));
                    pSetProcessDpiAwareness = reinterpret_cast<decltype(pSetProcessDpiAwareness)>(shcore.resolve("SetProcessDpiAwareness"));

                    if (QOperatingSystemVersion::isWin10RS1OrGreater()) {
                        pGetDpiForWindow = reinterpret_cast<decltype(pGetDpiForWindow)>(user32.resolve("GetDpiForWindow"));
                        pSystemParametersInfoForDpi = reinterpret_cast<decltype(pSystemParametersInfoForDpi)>(user32.resolve("SystemParametersInfoForDpi"));
                        pGetSystemMetricsForDpi = reinterpret_cast<decltype(pGetSystemMetricsForDpi)>(user32.resolve("GetSystemMetricsForDpi"));
                        pAdjustWindowRectExForDpi = reinterpret_cast<decltype(pAdjustWindowRectExForDpi)>(user32.resolve("AdjustWindowRectExForDpi"));
                        pEnableNonClientDpiScaling = reinterpret_cast<decltype(pEnableNonClientDpiScaling)>(user32.resolve("EnableNonClientDpiScaling"));
                        pGetAwarenessFromDpiAwarenessContext = reinterpret_cast<decltype(pGetAwarenessFromDpiAwarenessContext)>(user32.resolve("GetAwarenessFromDpiAwarenessContext"));
                        pGetWindowDpiAwarenessContext = reinterpret_cast<decltype(pGetWindowDpiAwarenessContext)>(user32.resolve("GetWindowDpiAwarenessContext"));
                        pGetThreadDpiAwarenessContext = reinterpret_cast<decltype(pGetThreadDpiAwarenessContext)>(user32.resolve("GetThreadDpiAwarenessContext"));
                        pAreDpiAwarenessContextsEqual = reinterpret_cast<decltype(pAreDpiAwarenessContextsEqual)>(user32.resolve("AreDpiAwarenessContextsEqual"));
                        pIsValidDpiAwarenessContext = reinterpret_cast<decltype(pIsValidDpiAwarenessContext)>(user32.resolve("IsValidDpiAwarenessContext"));

                        if (QOperatingSystemVersion::isWin10RS2OrGreater()) {
                            pSetProcessDpiAwarenessContext = reinterpret_cast<decltype(pSetProcessDpiAwarenessContext)>(user32.resolve("SetProcessDpiAwarenessContext"));
                        }
                    }
                }
            }
        }
    }

    ~QWindowsApi() = default;
};

class QWindowsContext
{
    Q_DISABLE_COPY_MOVE(QWindowsContext)
public:
    using HandleBaseWindowHash = QHash<HWND, QWindowsWindow *>;

    enum SystemInfoFlags
    {
        SI_RTL_Extensions = 0x1,
        SI_SupportsTouch = 0x2,
        SI_SupportsPointer = 0x4,
    };

    // Verbose flag set by environment variable QT_QPA_VERBOSE
    static int verbose;

    explicit QWindowsContext();
    ~QWindowsContext();

    bool initTouch();
    bool initTouch(unsigned integrationOptions); // For calls from QWindowsIntegration::QWindowsIntegration() only.
    void registerTouchWindows();
    bool initTablet();
    bool initPointer(unsigned integrationOptions);
    bool disposeTablet();

    bool initPowerNotificationHandler();

    int defaultDPI() const;

    static QString classNamePrefix();
    QString registerWindowClass(const QWindow *w);
    QString registerWindowClass(QString cname, WNDPROC proc,
                                unsigned style = 0, HBRUSH brush = nullptr,
                                bool icon = false);
    HWND createDummyWindow(const QString &classNameIn,
                           const wchar_t *windowName,
                           WNDPROC wndProc = nullptr, DWORD style = WS_OVERLAPPED);

    HDC displayContext() const;
    int screenDepth() const;

    static QWindowsContext *instance();

    void addWindow(HWND, QWindowsWindow *w);
    void removeWindow(HWND);

    QWindowsWindow *findClosestPlatformWindow(HWND) const;
    QWindowsWindow *findPlatformWindow(HWND) const;
    QWindowsWindow *findPlatformWindow(const QWindowsMenuBar *mb) const;
    QWindow *findWindow(HWND) const;
    QWindowsWindow *findPlatformWindowAt(HWND parent, const QPoint &screenPoint,
                                             unsigned cwex_flags) const;

    static bool shouldHaveNonClientDpiScaling(const QWindow *window);

    QWindow *windowUnderMouse() const;
    void clearWindowUnderMouse();

    inline bool windowsProc(HWND hwnd, UINT message,
                            QtWindows::WindowsEventType et,
                            WPARAM wParam, LPARAM lParam, LRESULT *result,
                            QWindowsWindow **platformWindowPtr);

    QWindow *keyGrabber() const;
    void setKeyGrabber(QWindow *hwnd);

    QSharedPointer<QWindowCreationContext> setWindowCreationContext(const QSharedPointer<QWindowCreationContext> &ctx);
    QSharedPointer<QWindowCreationContext> windowCreationContext() const;

    static void setTabletAbsoluteRange(int a);

    static bool setProcessDpiAwareness(QtWindows::DpiAwareness dpiAwareness);
    static QtWindows::DpiAwareness processDpiAwareness();
    static QtWindows::DpiAwareness windowDpiAwareness(HWND hwnd);

    static bool isDarkMode();

    void setDetectAltGrModifier(bool a);

    // Returns a combination of SystemInfoFlags
    unsigned systemInfo() const;

    bool useRTLExtensions() const;
    QList<int> possibleKeys(const QKeyEvent *e) const;

    HandleBaseWindowHash &windows();

    static bool isSessionLocked();

    QWindowsMimeRegistry &mimeConverter() const;
    QWindowsScreenManager &screenManager();
    QWindowsTabletSupport *tabletSupport() const;

    bool asyncExpose() const;
    void setAsyncExpose(bool value);

    static void forceNcCalcSize(HWND hwnd);

    static bool systemParametersInfo(unsigned action, unsigned param, void *out, unsigned dpi = 0);
    static bool systemParametersInfoForScreen(unsigned action, unsigned param, void *out,
                                              const QPlatformScreen *screen = nullptr);
    static bool systemParametersInfoForWindow(unsigned action, unsigned param, void *out,
                                              const QPlatformWindow *win = nullptr);
    static bool nonClientMetrics(NONCLIENTMETRICS *ncm, unsigned dpi = 0);
    static bool nonClientMetricsForScreen(NONCLIENTMETRICS *ncm,
                                          const QPlatformScreen *screen = nullptr);
    static bool nonClientMetricsForWindow(NONCLIENTMETRICS *ncm,
                                          const QPlatformWindow *win = nullptr);

    static DWORD readAdvancedExplorerSettings(const wchar_t *subKey, DWORD defaultValue);

    static bool filterNativeEvent(MSG *msg, LRESULT *result);
    static bool filterNativeEvent(QWindow *window, MSG *msg, LRESULT *result);

    [[nodiscard]] static UINT getDpiForWindow(const HWND hWnd);
    [[nodiscard]] static UINT getDpiForMonitor(const HMONITOR hMonitor);
    [[nodiscard]] static UINT getDpiForPrimaryMonitor();
    [[nodiscard]] static UINT getMostPossibleDpiForWindow(const HWND hWnd);
    [[nodiscard]] static UINT getMostPossibleDpiForMonitor(const HMONITOR hMonitor);
    [[nodiscard]] static int getResizeBorderThicknessForDpi(const UINT dpi);
    [[nodiscard]] static int getResizeBorderThickness(const HWND hWnd);
    [[nodiscard]] static int getResizeBorderThickness(const HMONITOR hMonitor);
    [[nodiscard]] static int getTitleBarHeightForDpi(const UINT dpi);
    [[nodiscard]] static int getTitleBarHeight(const HWND hWnd);
    [[nodiscard]] static int getTitleBarHeight(const HMONITOR hMonitor);

private:
    void handleFocusEvent(QtWindows::WindowsEventType et, QWindowsWindow *w);
#ifndef QT_NO_CONTEXTMENU
    bool handleContextMenuEvent(QWindow *window, const MSG &msg);
#endif
    void handleExitSizeMove(QWindow *window);
    void unregisterWindowClasses();

    QScopedPointer<QWindowsContextPrivate> d;
    static QWindowsContext *m_instance;
};

extern "C" LRESULT QT_WIN_CALLBACK qWindowsWndProc(HWND, UINT, WPARAM, LPARAM);

QT_END_NAMESPACE

#endif // QWINDOWSCONTEXT_H
