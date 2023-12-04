#ifndef WAYLANDPORTALSCREENCOPYFRAME_H
#define WAYLANDPORTALSCREENCOPYFRAME_H

#include <QObject>
#include <qwayland-wlr-screencopy-unstable-v1.h>

struct WaylandPortalScreencopyFramePrivate;
class WaylandPortalScreencopyFrame : public QObject,
                                     public QtWayland::zwlr_screencopy_frame_v1 {
        Q_OBJECT
    public:
        explicit WaylandPortalScreencopyFrame(struct ::zwlr_screencopy_frame_v1* object, wl_shm* shm, wl_display* display);
        ~WaylandPortalScreencopyFrame();

        bool ok();
        QPixmap pixmap();
        QImage image();

    signals:
        void done();

    private:
        WaylandPortalScreencopyFramePrivate* d;

        // zwlr_screencopy_frame_v1 interface
    protected:
        void zwlr_screencopy_frame_v1_buffer(uint32_t format, uint32_t width, uint32_t height, uint32_t stride);
        void zwlr_screencopy_frame_v1_ready(uint32_t tv_sec_hi, uint32_t tv_sec_lo, uint32_t tv_nsec);
        void zwlr_screencopy_frame_v1_failed();
};

#endif // WAYLANDPORTALSCREENCOPYFRAME_H
