/* This file is part of Spectacle, the KDE screenshot utility
 * SPDX-FileCopyrightText: 2015 Boudhayan Gupta <bgupta@kde.org>
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#pragma once

class QGraphicsDropShadowEffect;
#include <QLabel>
class QMouseEvent;
#include <QPixmap>
#include <QPoint>

namespace SpectacleImage
{
static const int SHADOW_RADIUS = 5;
}

class KSImageWidget : public QLabel
{
    Q_OBJECT

public:
    explicit KSImageWidget(QWidget *parent = nullptr);
    void setScreenshot(const QPixmap &pixmap);
    bool isPixmapSet() const;

Q_SIGNALS:

    void dragInitiated();

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private:
    void setScaledPixmap();

    QGraphicsDropShadowEffect *const mDSEffect;
    QPixmap mPixmap;
    QPoint mDragStartPosition;
};
