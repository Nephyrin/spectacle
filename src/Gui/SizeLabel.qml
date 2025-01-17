/* SPDX-FileCopyrightText: 2022 Noah Davis <noahadvs@gmail.com>
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

import QtQuick 2.15
import QtQml 2.15
import QtQuick.Templates 2.15 as T
import org.kde.kirigami 2.19 as Kirigami
import org.kde.spectacle.private 1.0

T.Label {
    id: root
    property size size: Qt.size(0, 0)
    Binding on text {
        value: `${size.width}×${size.height}`
        when: root.size.width > 0 && root.size.height > 0
        restoreMode: Binding.RestoreNone
    }
    textFormat: Text.PlainText
    horizontalAlignment: Text.AlignHCenter
    verticalAlignment: Text.AlignVCenter
    elide: Text.ElideNone
    wrapMode: Text.NoWrap
    color: palette.windowText
    background: Item { // Label implicit size is readonly, but you can still influence it via the background
        implicitWidth: contextWindow.dprRound(root.contentWidth + root.leftPadding + root.rightPadding)
        implicitHeight: contextWindow.dprRound(root.contentHeight + root.topPadding + root.bottomPadding)
    }
}
