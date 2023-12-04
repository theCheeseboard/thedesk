#include "waylandportalscreencopyframe.h"

#include <QTemporaryFile>
#include <QUuid>
#include <fcntl.h>
#include <sys/mman.h>
#include <tlogger.h>

struct WaylandPortalScreencopyFramePrivate {
        wl_shm* shm;
        wl_display* display;
        bool ok = false;

        QImage::Format qimageFormat;
        void* data;
        wl_buffer* buf;
        quint32 width;
        quint32 height;
        quint32 size;
        std::function<void(quint32*, quint32)> transform;

        QImage image;
};

WaylandPortalScreencopyFrame::WaylandPortalScreencopyFrame(struct ::zwlr_screencopy_frame_v1* object, wl_shm* shm, wl_display* display) :
    QObject{nullptr}, QtWayland::zwlr_screencopy_frame_v1{object} {
    d = new WaylandPortalScreencopyFramePrivate();
    d->shm = shm;
    d->display = display;
    tDebug("WaylandPortalScreencopyFrame") << "Created screencopy frame";
}

WaylandPortalScreencopyFrame::~WaylandPortalScreencopyFrame() {
    tDebug("WaylandPortalScreencopyFrame") << "Destroyed screencopy frame";
    this->destroy();
    delete d;
}

bool WaylandPortalScreencopyFrame::ok() {
    return d->ok;
}

QPixmap WaylandPortalScreencopyFrame::pixmap() {
    return QPixmap::fromImage(d->image);
}

QImage WaylandPortalScreencopyFrame::image() {
    return d->image;
}

void WaylandPortalScreencopyFrame::zwlr_screencopy_frame_v1_buffer(uint32_t format, uint32_t width, uint32_t height, uint32_t stride) {
    tDebug("WaylandPortalScreencopyFrame") << "Buffer f: " << format << " w: " << width << " h: " << height << " s: " << stride;

    wl_shm_format fmt = static_cast<wl_shm_format>(format);
    d->qimageFormat = QImage::Format_Invalid;
    d->transform = [](quint32*, quint32) {
    }; // noop
    switch (fmt) {
        case WL_SHM_FORMAT_ARGB8888:
            d->qimageFormat = QImage::Format_ARGB32;
            break;
        case WL_SHM_FORMAT_XRGB8888:
            d->qimageFormat = QImage::Format_ARGB32;
            break;
        case WL_SHM_FORMAT_XBGR8888:
            d->qimageFormat = QImage::Format_ARGB32;
            d->transform = [](quint32* data, quint32 size) {
                for (auto i = 0; i < size; i++) {
                    auto r = (data[i] >> 0) & 0xff;
                    auto g = (data[i] >> 8) & 0xff;
                    auto b = (data[i] >> 16) & 0xff;
                    auto a = 0xff;

                    data[i] = (a << 24) | (r << 16) | (g << 8) | b;
                }
            };
            break;
        case WL_SHM_FORMAT_C8:
        case WL_SHM_FORMAT_RGB332:
        case WL_SHM_FORMAT_BGR233:
        case WL_SHM_FORMAT_XRGB4444:
        case WL_SHM_FORMAT_XBGR4444:
        case WL_SHM_FORMAT_RGBX4444:
        case WL_SHM_FORMAT_BGRX4444:
        case WL_SHM_FORMAT_ARGB4444:
        case WL_SHM_FORMAT_ABGR4444:
        case WL_SHM_FORMAT_RGBA4444:
        case WL_SHM_FORMAT_BGRA4444:
        case WL_SHM_FORMAT_XRGB1555:
        case WL_SHM_FORMAT_XBGR1555:
        case WL_SHM_FORMAT_RGBX5551:
        case WL_SHM_FORMAT_BGRX5551:
        case WL_SHM_FORMAT_ARGB1555:
        case WL_SHM_FORMAT_ABGR1555:
        case WL_SHM_FORMAT_RGBA5551:
        case WL_SHM_FORMAT_BGRA5551:
        case WL_SHM_FORMAT_RGB565:
        case WL_SHM_FORMAT_BGR565:
        case WL_SHM_FORMAT_RGB888:
        case WL_SHM_FORMAT_BGR888:
        case WL_SHM_FORMAT_RGBX8888:
        case WL_SHM_FORMAT_BGRX8888:
        case WL_SHM_FORMAT_ABGR8888:
        case WL_SHM_FORMAT_RGBA8888:
        case WL_SHM_FORMAT_BGRA8888:
        case WL_SHM_FORMAT_XRGB2101010:
        case WL_SHM_FORMAT_XBGR2101010:
        case WL_SHM_FORMAT_RGBX1010102:
        case WL_SHM_FORMAT_BGRX1010102:
        case WL_SHM_FORMAT_ARGB2101010:
        case WL_SHM_FORMAT_ABGR2101010:
        case WL_SHM_FORMAT_RGBA1010102:
        case WL_SHM_FORMAT_BGRA1010102:
        case WL_SHM_FORMAT_YUYV:
        case WL_SHM_FORMAT_YVYU:
        case WL_SHM_FORMAT_UYVY:
        case WL_SHM_FORMAT_VYUY:
        case WL_SHM_FORMAT_AYUV:
        case WL_SHM_FORMAT_NV12:
        case WL_SHM_FORMAT_NV21:
        case WL_SHM_FORMAT_NV16:
        case WL_SHM_FORMAT_NV61:
        case WL_SHM_FORMAT_YUV410:
        case WL_SHM_FORMAT_YVU410:
        case WL_SHM_FORMAT_YUV411:
        case WL_SHM_FORMAT_YVU411:
        case WL_SHM_FORMAT_YUV420:
        case WL_SHM_FORMAT_YVU420:
        case WL_SHM_FORMAT_YUV422:
        case WL_SHM_FORMAT_YVU422:
        case WL_SHM_FORMAT_YUV444:
        case WL_SHM_FORMAT_YVU444:
        case WL_SHM_FORMAT_R8:
        case WL_SHM_FORMAT_R16:
        case WL_SHM_FORMAT_RG88:
        case WL_SHM_FORMAT_GR88:
        case WL_SHM_FORMAT_RG1616:
        case WL_SHM_FORMAT_GR1616:
        case WL_SHM_FORMAT_XRGB16161616F:
        case WL_SHM_FORMAT_XBGR16161616F:
        case WL_SHM_FORMAT_ARGB16161616F:
        case WL_SHM_FORMAT_ABGR16161616F:
        case WL_SHM_FORMAT_XYUV8888:
        case WL_SHM_FORMAT_VUY888:
        case WL_SHM_FORMAT_VUY101010:
        case WL_SHM_FORMAT_Y210:
        case WL_SHM_FORMAT_Y212:
        case WL_SHM_FORMAT_Y216:
        case WL_SHM_FORMAT_Y410:
        case WL_SHM_FORMAT_Y412:
        case WL_SHM_FORMAT_Y416:
        case WL_SHM_FORMAT_XVYU2101010:
        case WL_SHM_FORMAT_XVYU12_16161616:
        case WL_SHM_FORMAT_XVYU16161616:
        case WL_SHM_FORMAT_Y0L0:
        case WL_SHM_FORMAT_X0L0:
        case WL_SHM_FORMAT_Y0L2:
        case WL_SHM_FORMAT_X0L2:
        case WL_SHM_FORMAT_YUV420_8BIT:
        case WL_SHM_FORMAT_YUV420_10BIT:
        case WL_SHM_FORMAT_XRGB8888_A8:
        case WL_SHM_FORMAT_XBGR8888_A8:
        case WL_SHM_FORMAT_RGBX8888_A8:
        case WL_SHM_FORMAT_BGRX8888_A8:
        case WL_SHM_FORMAT_RGB888_A8:
        case WL_SHM_FORMAT_BGR888_A8:
        case WL_SHM_FORMAT_RGB565_A8:
        case WL_SHM_FORMAT_BGR565_A8:
        case WL_SHM_FORMAT_NV24:
        case WL_SHM_FORMAT_NV42:
        case WL_SHM_FORMAT_P210:
        case WL_SHM_FORMAT_P010:
        case WL_SHM_FORMAT_P012:
        case WL_SHM_FORMAT_P016:
        case WL_SHM_FORMAT_AXBXGXRX106106106106:
        case WL_SHM_FORMAT_NV15:
        case WL_SHM_FORMAT_Q410:
        case WL_SHM_FORMAT_Q401:
        case WL_SHM_FORMAT_XRGB16161616:
        case WL_SHM_FORMAT_XBGR16161616:
        case WL_SHM_FORMAT_ARGB16161616:
        case WL_SHM_FORMAT_ABGR16161616:
            tDebug("WaylandPortalScreencopyFrame") << "Unsupported SHM format: " << fmt;
            break;
    }

    if (d->qimageFormat == QImage::Format_Invalid) {
        emit done();
        return;
    }

    d->width = width;
    d->height = height;
    d->size = stride * height;

    QString filename = QStringLiteral("/tdportal-%1").arg(QUuid::createUuid().toString(QUuid::Id128));

    auto fd = shm_open(filename.toUtf8().data(), O_RDWR | O_CREAT | O_EXCL, 0600);
    shm_unlink(filename.toUtf8().data());
    ftruncate(fd, d->size);

    d->data = mmap(nullptr, d->size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

    auto pool = wl_shm_create_pool(d->shm, fd, d->size);
    d->buf = wl_shm_pool_create_buffer(pool, 0, width, height, stride, format);
    wl_shm_pool_destroy(pool);
    close(fd);

    copy(d->buf);

    wl_display_roundtrip(d->display);
}

void WaylandPortalScreencopyFrame::zwlr_screencopy_frame_v1_ready(uint32_t tv_sec_hi, uint32_t tv_sec_lo, uint32_t tv_nsec) {
    tDebug("WaylandPortalScreencopyFrame") << "Ready h: " << tv_sec_hi << " l: " << tv_sec_lo << " n: " << tv_nsec;

    QImage image(reinterpret_cast<uchar*>(d->data), d->width, d->height, d->qimageFormat);
    d->image = image.copy();
    for (auto i = 0; i < image.height(); i++) {
        d->transform(reinterpret_cast<quint32*>(d->image.scanLine(i)), d->image.bytesPerLine() / 4);
    }
    d->ok = true;

    munmap(d->data, d->size);
    wl_buffer_destroy(d->buf);

    emit done();
}

void WaylandPortalScreencopyFrame::zwlr_screencopy_frame_v1_failed() {
    tDebug("WaylandPortalScreencopyFrame") << "Screencopy failed";
    munmap(d->data, d->size);
    wl_buffer_destroy(d->buf);

    emit done();
}
