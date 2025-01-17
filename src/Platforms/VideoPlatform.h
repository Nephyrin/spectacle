/* This file is part of Spectacle, the KDE screenshot utility
 * SPDX-FileCopyrightText: 2022 Aleix Pol Gonzalez <aleixpol@kde.org>

 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#pragma once

#include <QBasicTimer>
#include <QElapsedTimer>
#include <QFlags>
#include <QObject>
#include <QRect>
#include <variant>

class QScreen;

class VideoPlatform : public QObject
{
    Q_OBJECT
    Q_PROPERTY(RecordingModes supportedRecordingModes READ supportedRecordingModes CONSTANT)
    Q_PROPERTY(bool isRecording READ isRecording NOTIFY recordingChanged)
    Q_PROPERTY(qint64 recordedTime READ recordedTime NOTIFY recordedTimeChanged)

public:
    explicit VideoPlatform(QObject *parent = nullptr);
    ~VideoPlatform() override = default;

    enum RecordingMode : char {
        Screen = 0b001, //< records a specific output, provided its QScreen::name()
        Window = 0b010, //< records a specific window, provided its uuid
        Region = 0b100, //< records the provided region rectangle
    };
    Q_ENUM(RecordingMode)
    using RecordingOption = std::variant<QScreen *, QRect, QString>;
    Q_DECLARE_FLAGS(RecordingModes, RecordingMode)

    bool isRecording() const;
    virtual RecordingModes supportedRecordingModes() const = 0;
    virtual QString extension() const = 0;
    qint64 recordedTime() const;

protected:
    void setRecording(bool recording);
    void timerEvent(QTimerEvent *event) override;

public Q_SLOTS:
    virtual void startRecording(const QString &path, VideoPlatform::RecordingMode recordingMode,
                                const VideoPlatform::RecordingOption &option, bool includePointer) = 0;
    virtual void finishRecording() = 0;

Q_SIGNALS:
    void recordingChanged(bool isRecording);
    void recordingSaved(const QString &path);
    void recordedTimeChanged();

private:
    QElapsedTimer m_elapsedTimer;
    QBasicTimer m_basicTimer;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(VideoPlatform::RecordingModes)
