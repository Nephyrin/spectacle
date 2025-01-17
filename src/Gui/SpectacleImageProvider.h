/* SPDX-FileCopyrightText: 2022 Noah Davis <noahadvs@gmail.com>
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#pragma once

#include <QQuickImageProvider>

class SpectacleImageProvider : public QQuickImageProvider
{
public:
    SpectacleImageProvider(ImageType type, Flags flags = Flags());
    ~SpectacleImageProvider() override = default;

    QPixmap requestPixmap(const QString &id, QSize *size, const QSize &requestedSize) override;
};
