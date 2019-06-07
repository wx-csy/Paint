#include <QImage>
#include <QPixmap>
#include <paint/paint.h>
#include <paint/device.h>

#ifndef QIMAGEDEVICE_H
#define QIMAGEDEVICE_H

static inline Paint::RGBColor qrgb_to_paint_color(QRgb qrgb) {
    return Paint::RGBColor(qRed(qrgb), qGreen(qrgb), qBlue(qrgb));
}

static inline QRgb paint_color_to_qrgb(Paint::RGBColor color) {
    return qRgb(color.red, color.green, color.blue);
}

class QImageDevice : public Paint::ImageDevice
{
public:
    QImage image;

    explicit QImageDevice(size_t width = 800, size_t height = 600) :
        ImageDevice(width, height), image((int)width, (int)height, QImage::Format_RGB32)
    {
        reset(width, height);
    }

    Paint::RGBColor getPixel(ssize_t x, ssize_t y) const override {
        if (x < 0 || y < 0 || size_t(x) >= width || size_t(y) >= height)
            throw std::range_error("pixel out of canvas");
        return qrgb_to_paint_color(image.pixel(x, y));
    }

    void setPixel(ssize_t x, ssize_t y, Paint::RGBColor color) override {
        if (x < 0 || y < 0 || x >= ssize_t(width) || y >= ssize_t(height))
            return;
        image.setPixel(x, y, paint_color_to_qrgb(color));
    }

    void reset(size_t width, size_t height) override {
        Paint::ImageDevice::reset(width, height);
        image = QImage(width, height, QImage::Format_RGB32);
        image.fill(paint_color_to_qrgb(Paint::Colors::white));
    }

    QPixmap getPixmap() {
        QPixmap pixmap;
        pixmap.convertFromImage(image);
        return pixmap;
    }

    ~QImageDevice() override = default;
};

#endif // QIMAGEDEVICE_H
