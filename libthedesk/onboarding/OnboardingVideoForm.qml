import QtQuick 2.0
import QtMultimedia 6.4

Item {
    VideoOutput {
        id: video
        x: 0
        y: 0
        width: parent.width
        height: parent.height
//        flushMode: VideoOutput.LastFrame
        fillMode: VideoOutput.PreserveAspectCrop
//        source: sourceVideo
    }
}
