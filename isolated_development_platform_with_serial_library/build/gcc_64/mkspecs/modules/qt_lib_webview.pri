QT.webview.VERSION = 5.12.1
QT.webview.name = QtWebView
QT.webview.module = Qt5WebView
QT.webview.libs = $$QT_MODULE_LIB_BASE
QT.webview.includes = $$QT_MODULE_INCLUDE_BASE $$QT_MODULE_INCLUDE_BASE/QtWebView
QT.webview.frameworks =
QT.webview.bins = $$QT_MODULE_BIN_BASE
QT.webview.plugin_types = webview
QT.webview.depends = core gui
QT.webview.run_depends = quick_private core_private gui_private
QT.webview.uses =
QT.webview.module_config = v2
QT.webview.DEFINES = QT_WEBVIEW_LIB
QT.webview.enabled_features =
QT.webview.disabled_features =
QT_CONFIG +=
QT_MODULES += webview
