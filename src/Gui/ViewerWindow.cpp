/* This file is part of Spectacle, the KDE screenshot utility
 * SPDX-FileCopyrightText: 2015 Boudhayan Gupta <bgupta@kde.org>
 * SPDX-FileCopyrightText: 2019 David Redondo <kde@david-redondo.de>
 * SPDX-FileCopyrightText: 2022 Noah Davis <noahadvs@gmail.com>
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "ViewerWindow.h"

#include "SpectacleCore.h"
#include "spectacle_gui_debug.h"

#include <QApplication>
#include <QClipboard>

ViewerWindow *ViewerWindow::s_viewerWindowInstance = nullptr;

ViewerWindow::ViewerWindow(Mode mode, QQmlEngine *engine, QWindow *parent)
    : SpectacleWindow(engine, parent)
    , m_mode(mode)
{
    s_viewerWindowInstance = this;
    s_isAnnotating = false;
// QGuiApplication::paletteChanged() is deprecated in Qt 6.
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    connect(qGuiApp, &QGuiApplication::paletteChanged, this, &ViewerWindow::updateColor);
#endif

    connect(SpectacleCore::instance(), &SpectacleCore::screenCaptureUrlChanged, this, &ViewerWindow::imageSizeChanged);
    connect(SpectacleCore::instance(), &SpectacleCore::screenCaptureUrlChanged, this, &ViewerWindow::imageDprChanged);

    connect(m_exportMenu.get(), &ExportMenu::imageShared, this, &ViewerWindow::showImageSharedMessage);

    // set up QML
    setResizeMode(QQuickView::SizeRootObjectToView);
    setMode(mode); // sets source and other stuff based on mode.
    m_oldWindowStates = windowStates();
}

ViewerWindow::~ViewerWindow()
{
    s_viewerWindowInstance = nullptr;
}

ViewerWindow::UniquePointer ViewerWindow::makeUnique(Mode mode, QQmlEngine *engine, QWindow *parent)
{
    return UniquePointer(new ViewerWindow(mode, engine, parent), &SpectacleWindow::deleter);
}

ViewerWindow *ViewerWindow::instance()
{
    return s_viewerWindowInstance;
}

QSize ViewerWindow::imageSize() const
{
    return ExportManager::instance()->pixmap().size();
}

qreal ViewerWindow::imageDpr() const
{
    return ExportManager::instance()->pixmap().devicePixelRatio();
}

void ViewerWindow::setMode(ViewerWindow::Mode mode)
{
    if (mode == Dialog) {
        setBackgroundColorRole(QPalette::Window);
        QVariantMap initialProperties = {
            // Set the parent in initialProperties to avoid having
            // the parent and window be null in Component.onCompleted
            {QStringLiteral("parent"), QVariant::fromValue(contentItem())}
        };
        setSource(QUrl(QStringLiteral("qrc:/src/Gui/DialogPage.qml")), initialProperties);
        auto rootItem = rootObject();
        if (!rootItem) {
            return;
        }
        const QSize implicitSize = {qRound(rootItem->implicitWidth()),
                                    qRound(rootItem->implicitHeight())};
        setMinimumSize(implicitSize);
        setMaximumSize(implicitSize);
        connect(rootItem, &QQuickItem::implicitWidthChanged, this, [this](){
            int implicitWidth = qRound(rootObject()->implicitWidth());
            setMinimumWidth(implicitWidth);
            setMaximumWidth(implicitWidth);
        });
        connect(rootItem, &QQuickItem::implicitHeightChanged, this, [this](){
            int implicitHeight = qRound(rootObject()->implicitHeight());
            setMinimumHeight(implicitHeight);
            setMaximumHeight(implicitHeight);
        });
    } else if (mode == Image) {
        setBackgroundColorRole(QPalette::Base);
        QVariantMap initialProperties = {
            // Set the parent in initialProperties to avoid having
            // the parent and window be null in Component.onCompleted
            {QStringLiteral("parent"), QVariant::fromValue(contentItem())}
        };
        setSource(QUrl(QStringLiteral("qrc:/src/Gui/ImageView.qml")), initialProperties);
        auto rootItem = rootObject();
        if (!rootItem) {
            return;
        }
        updateMinimumSize();
        connect(rootItem, SIGNAL(minimumWidthChanged()), this, SLOT(updateMinimumSize()));
        connect(rootItem, SIGNAL(minimumHeightChanged()), this, SLOT(updateMinimumSize()));
    } else if (mode == Video) {
        
    }
}

void ViewerWindow::updateColor()
{
    setColor(qGuiApp->palette().color(m_backgroundColorRole));
}

void ViewerWindow::setBackgroundColorRole(QPalette::ColorRole role)
{
    m_backgroundColorRole = role;
    updateColor();
}

void ViewerWindow::updateMinimumSize()
{
    if (auto rootItem = rootObject()) {
        const QSize size = {qRound(rootItem->property("minimumWidth").toReal()),
                            qRound(rootItem->property("minimumHeight").toReal())};
        // Resizing should be automatic, but sometimes a manual resizing is needed
        // to fix window sizing/graphical glitches after a minimum size change.
        if (size.width() > width()) {
            setWidth(size.width());
        }
        if (size.height() > height()) {
            setHeight(size.height());
        }
        setMinimumSize(size);
    }
}

void ViewerWindow::showInlineMessage(const QString &qmlFile, const QVariantMap &properties)
{
    rootObject()->setProperty("inlineMessageSource", qmlFile);
    rootObject()->setProperty("inlineMessageData", properties);
}

void ViewerWindow::showSavedScreenshotMessage(const QUrl &messageArgument)
{
    showInlineMessage(QStringLiteral("qrc:/src/Gui/SavedMessage.qml"),
                      {
                          {QLatin1String("messageArgument"), messageArgument},
                          {QLatin1String("video"), false},
                      });
}

void ViewerWindow::showSavedVideoMessage(const QUrl &messageArgument)
{
    showInlineMessage(QStringLiteral("qrc:/src/Gui/SavedMessage.qml"),
                      {
                          {QLatin1String("messageArgument"), messageArgument},
                          {QLatin1String("video"), true},
                      });
}

void ViewerWindow::showSavedAndCopiedMessage(const QUrl &messageArgument)
{
    showInlineMessage(QStringLiteral("qrc:/src/Gui/SavedAndCopiedMessage.qml"), {{QLatin1String("messageArgument"), messageArgument}});
}

void ViewerWindow::showSavedAndLocationCopiedMessage(const QUrl &messageArgument)
{
    showInlineMessage(QStringLiteral("qrc:/src/Gui/SavedAndLocationCopied.qml"), {{QLatin1String("messageArgument"), messageArgument}});
}

void ViewerWindow::showCopiedMessage()
{
    showInlineMessage(QStringLiteral("qrc:/src/Gui/CopiedMessage.qml"), {});
}

void ViewerWindow::showScreenshotFailedMessage()
{
    showInlineMessage(QStringLiteral("qrc:/src/Gui/ScreenshotFailedMessage.qml"), {});
}

void ViewerWindow::showImageSharedMessage(int errorCode, const QString &messageArgument)
{
    if (errorCode == 1 || status() != QQuickView::Ready) {
        // errorCode == 1 means the user cancelled the sharing
        return;
    }

    if (errorCode) {
        showInlineMessage(QStringLiteral("qrc:/src/Gui/ShareErrorMessage.qml"), {{QLatin1String("messageArgument"), messageArgument}});
    } else {
        showInlineMessage(QStringLiteral("qrc:/src/Gui/SharedMessage.qml"), {{QLatin1String("messageArgument"), messageArgument}});
        if (!messageArgument.isEmpty()) {
            QApplication::clipboard()->setText(messageArgument);
        }
    }
}

bool ViewerWindow::event(QEvent *event)
{
// This should work in Qt 5, but doesn't.
// The event type simply never happens in response to color scheme changes.
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    if (event->type() == QEvent::ApplicationPaletteChange) {
        updateColor();
    }
#endif
    return SpectacleWindow::event(event);
}

void ViewerWindow::resizeEvent(QResizeEvent *event)
{
    SpectacleWindow::resizeEvent(event);
    if (auto rootItem = rootObject()) {
        // sometimes rootObject size doesn't keep up with the window size
        rootItem->setSize(this->size());
    }
}

void ViewerWindow::keyReleaseEvent(QKeyEvent *event)
{
    SpectacleWindow::keyReleaseEvent(event);
    if (event->isAccepted() || m_mode == Dialog) {
        return;
    }
    if (event->matches(QKeySequence::Save)) {
        event->accept();
        save();
    } else if (event->matches(QKeySequence::SaveAs)) {
        event->accept();
        saveAs();
    } else if (event->matches(QKeySequence::Copy)) {
        event->accept();
        copyImage();
    } else if (event->matches(QKeySequence::Print)) {
        event->accept();
        showPrintDialog();
    }
    auto document = SpectacleCore::instance()->annotationDocument();
    if (!event->isAccepted() && document) {
        if (document->undoStackDepth() > 0 && event->matches(QKeySequence::Undo)) {
            event->accept();
            document->undo();
        } else if (document->redoStackDepth() > 0 && event->matches(QKeySequence::Redo)) {
            event->accept();
            document->redo();
        }
    }
}
