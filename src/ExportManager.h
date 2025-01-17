/* This file is part of Spectacle, the KDE screenshot utility
 * SPDX-FileCopyrightText: 2015 Boudhayan Gupta <bgupta@kde.org>
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#pragma once

#include "settings.h"
#include <KLocalizedString>
#include <QDateTime>
class QIODevice;
#include <QMap>
#include <QObject>
#include <QPixmap>
class QPrinter;
#include <QUrl>

class QTemporaryDir;

class ExportManager : public QObject
{
    Q_OBJECT

    // singleton-ize the class

public:
    static ExportManager *instance();

private:
    explicit ExportManager(QObject *parent = nullptr);
    ~ExportManager() override;

    ExportManager(ExportManager const &) = delete;
    void operator=(ExportManager const &) = delete;

    // now the usual stuff

public:
    QString defaultSaveLocation() const;
    QString defaultVideoSaveLocation() const;
    bool isFileExists(const QUrl &url) const;
    bool isImageSavedNotInTemp() const;
    void setPixmap(const QPixmap &pixmap);
    QPixmap pixmap() const;
    void updatePixmapTimestamp();
    void setTimestamp(const QDateTime &timestamp);

    /**
     * The title used to fill the window title template in formatted file names.
     */
    QString windowTitle() const;

    /**
     * Returns a formatted filename using a template string.
     */
    QString formattedFilename(const QString &nameTemplate = Settings::saveFilenameFormat()) const;

    QString suggestedVideoFilename(const QString &extension) const;

    static const QMap<QString, KLocalizedString> filenamePlaceholders;

Q_SIGNALS:
    void pixmapChanged();

    void errorMessage(const QString &str);
    void imageSaved(const QUrl &savedAt);
    void imageCopied();
    void imageLocationCopied(const QUrl &savedAt);
    void imageSavedAndCopied(const QUrl &savedAt);
    void forceNotify(const QUrl &savedAt);

public Q_SLOTS:

    QUrl getAutosaveFilename() const;
    QUrl tempSave();

    void setWindowTitle(const QString &windowTitle);
    void doSave(const QUrl &url = QUrl(), bool notify = false);
    bool doSaveAs(bool notify = false);
    void doSaveAndCopy(const QUrl &url = QUrl());
    void doCopyToClipboard(bool notify = false);
    void doCopyLocationToClipboard(bool notify = false);
    void doPrint(QPrinter *printer);

private:
    QString truncatedFilename(const QString &filename) const;
    using FileNameAlreadyUsedCheck = bool (ExportManager::*)(const QUrl &) const;
    QString autoIncrementFilename(const QString &baseName, const QString &extension, FileNameAlreadyUsedCheck isFileNameUsed) const;
    QString imageFileSuffix(const QUrl &url) const;
    bool writeImage(QIODevice *device, const QByteArray &suffix);
    bool save(const QUrl &url);
    bool localSave(const QUrl &url, const QString &suffix);
    bool remoteSave(const QUrl &url, const QString &suffix);
    bool isTempFileAlreadyUsed(const QUrl &url) const;

    bool m_imageSavedNotInTemp;
    QPixmap m_savePixmap;
    QDateTime m_pixmapTimestamp;
    QUrl m_tempFile;
    QTemporaryDir *m_tempDir = nullptr;
    QList<QUrl> m_usedTempFileNames;
    QString m_windowTitle;
};
