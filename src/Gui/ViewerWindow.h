/* This file is part of Spectacle, the KDE screenshot utility
 * SPDX-FileCopyrightText: 2015 Boudhayan Gupta <bgupta@kde.org>
 * SPDX-FileCopyrightText: 2022 Noah Davis <noahadvs@gmail.com>
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#pragma once

#include "Gui/SpectacleWindow.h"

class ViewerWindowPrivate;

/**
 * The window used for viewing media after it has been accepted or finished recording.
 * This has to be a separate window from the selection/capture window because reusing
 * the same window and changing the flags doesn't work nicely on Wayland. For example,
 * the window uses default window decorations instead of normal decorations.
 */
class ViewerWindow : public SpectacleWindow
{
    Q_OBJECT
    // Only create this window after the image has been set
    Q_PROPERTY(QSize imageSize READ imageSize NOTIFY imageSizeChanged FINAL)
    Q_PROPERTY(qreal imageDpr READ imageDpr NOTIFY imageDprChanged FINAL)

public:
    enum Mode {
        Dialog,
        Image,
        Video,
    };

    using UniquePointer = std::unique_ptr<ViewerWindow, decltype(&SpectacleWindow::deleter)>;

    static UniquePointer makeUnique(Mode mode, QQmlEngine *engine, QWindow *parent = nullptr);

    static ViewerWindow *instance();

    QSize imageSize() const;
    qreal imageDpr() const;

    void showSavedScreenshotMessage(const QUrl &messageArgument);
    void showSavedVideoMessage(const QUrl &messageArgument);
    void showSavedAndCopiedMessage(const QUrl &messageArgument);
    void showSavedAndLocationCopiedMessage(const QUrl &messageArgument);
    void showCopiedMessage();
    void showScreenshotFailedMessage();

Q_SIGNALS:
    void imageSizeChanged();
    void imageDprChanged();

protected:
    bool event(QEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;

private:
    explicit ViewerWindow(Mode mode, QQmlEngine *engine, QWindow *parent = nullptr);
    ~ViewerWindow();

    void setMode(ViewerWindow::Mode mode);
    Q_SLOT void updateColor();
    Q_SLOT void updateMinimumSize();
    Q_SLOT void showInlineMessage(const QString &source, const QVariantMap &properties);
    Q_SLOT void showImageSharedMessage(int errorCode, const QString &messageArgument);

    void setBackgroundColorRole(QPalette::ColorRole role);

    bool m_pixmapExists = false;
    QPalette::ColorRole m_backgroundColorRole;
    Qt::WindowStates m_oldWindowStates;
    const Mode m_mode;
    static ViewerWindow *s_viewerWindowInstance;
};
