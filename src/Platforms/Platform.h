/* This file is part of Spectacle, the KDE screenshot utility
 * SPDX-FileCopyrightText: 2019 Boudhayan Gupta <bgupta@kde.org>

 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#pragma once

#include "ScreenImage.h"

#include <QFlags>
#include <QObject>

class Platform : public QObject
{
    Q_OBJECT
    Q_PROPERTY(GrabModes supportedGrabModes READ supportedGrabModes NOTIFY supportedGrabModesChanged)
    // Currently, supportedShutterModes never changes.
    // Be sure to add a changed signal if it is ever able to change.
    Q_PROPERTY(ShutterModes supportedShutterModes READ supportedShutterModes CONSTANT)

public:
    enum GrabMode {
        InvalidChoice = 0x00,
        AllScreens = 0x01,
        CurrentScreen = 0x02,
        ActiveWindow = 0x04,
        WindowUnderCursor = 0x08,
        TransientWithParent = 0x10,
        AllScreensScaled = 0x20,
        PerScreenImageNative = 0x40,
    };
    Q_DECLARE_FLAGS(GrabModes, GrabMode)
    Q_FLAG(GrabModes)

    enum ShutterMode { Immediate = 0x01, OnClick = 0x02 };
    Q_DECLARE_FLAGS(ShutterModes, ShutterMode)
    Q_FLAG(ShutterModes)

    explicit Platform(QObject *parent = nullptr);
    ~Platform() override = default;

    virtual GrabModes supportedGrabModes() const = 0;
    virtual ShutterModes supportedShutterModes() const = 0;

public Q_SLOTS:
    virtual void doGrab(Platform::ShutterMode shutterMode, Platform::GrabMode grabMode, bool includePointer, bool includeDecorations) = 0;

Q_SIGNALS:
    void supportedGrabModesChanged();

    void newScreenshotTaken(const QPixmap &thePixmap);
    void newScreensScreenshotTaken(const QVector<ScreenImage> &screenImages);

    void newScreenshotFailed();
    void windowTitleChanged(const QString &theWindowTitle);
};

Q_DECLARE_OPERATORS_FOR_FLAGS(Platform::GrabModes)
Q_DECLARE_OPERATORS_FOR_FLAGS(Platform::ShutterModes)
