/*
    Paint, a simple rasterization tool
    Copyright (C) 2019 Chen Shaoyuan

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef __DEVICE_H__
#define __DEVICE_H__

namespace Paint {

    class ImageDevice {
    protected:
        size_t width, height;
        ImageDevice(size_t width, size_t height) :
            width(width), height(height) { }

    public:
        virtual RGBColor getPixel(ssize_t x, ssize_t y) const = 0;
        RGBColor getPixel(PointI pt) const {
            return getPixel(pt.x, pt.y);
        }
        virtual void setPixel(ssize_t x, ssize_t y, RGBColor color) = 0;
        void setPixel(PointI pt, RGBColor color) {
            setPixel(pt.x, pt.y, color);
        }
        virtual void reset(size_t width, size_t height) = 0;
        virtual void clear(RGBColor color) {
            for (size_t x = 0; x < width; x++)
                for (size_t y = 0; y < height; y++)
                    setPixel(x, y, color);
        }
        virtual ~ImageDevice() = default;
    };

    class MemoryImageDevice : public ImageDevice {
    private:
        std::vector<RGBColor> data;

    public:
        explicit MemoryImageDevice(size_t width = 800, size_t height = 600) :
            ImageDevice(width, height), data(width * height)
        { }

        MemoryImageDevice(const MemoryImageDevice& other) = delete;
        MemoryImageDevice(MemoryImageDevice&& other) = delete;
        MemoryImageDevice& operator = (const MemoryImageDevice& other) = delete;
        MemoryImageDevice& operator = (MemoryImageDevice&& other) = delete;

        RGBColor getPixel(ssize_t x, ssize_t y) const override {
            if (x < 0 || y < 0 || size_t(x) >= width || size_t(y) >= height)
                throw std::range_error("pixel out of canvas");
            return data[width * y + x];
        }

        void setPixel(ssize_t x, ssize_t y, RGBColor color) override {
            if (x < 0 || y < 0 || size_t(x) >= width || size_t(y) >= height)
                return;
            data[width * y + x] = color;
        }

        void clear(RGBColor color) override {
            std::fill(data.begin(), data.end(), color);
        }

        void reset(size_t width, size_t height) override {
            this->width = width;
            this->height = height;
            data.assign(width * height, RGBColor());
        }
    };
}

#endif
