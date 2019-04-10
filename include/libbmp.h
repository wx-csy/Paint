#ifndef __LIBBMP_H__
#define __LIBBMP_H__

#include <vector>
#include <string>
#include <cstdint>
#include <stdexcept>
#include "paint.h"

namespace LibBmp {

    //
    // BmpError
    //

    enum class BmpError : int
    {
        BMP_FILE_NOT_OPENED = -4,
        BMP_HEADER_NOT_INITIALIZED,
        BMP_INVALID_FILE,
        BMP_ERROR,
        BMP_OK = 0
    };

    //
    // BmpPixbuf
    //

    class BmpPixbuf
    {
        public:
            BmpPixbuf (void);
            BmpPixbuf (const int width,
                       const int height);
            ~BmpPixbuf ();
            
            void init (const int width,
                       const int height);
            
            void set_pixel (const int x,
                            const int y,
                            const unsigned char r,
                            const unsigned char g,
                            const unsigned char b);
            
            unsigned char red_at (const int x,
                                  const int y) const;
            unsigned char green_at (const int x,
                                    const int y) const;
            unsigned char blue_at (const int x,
                                   const int y) const;
            
            
            void write (const int row,
                        std::ofstream& f);
            
            void read (const int row,
                       std::ifstream& f);
        private:
            size_t len_row;
            size_t len_pixel = 3;
            
            std::vector<unsigned char> data;
    };

    //
    // BmpImg
    //

    class BmpImg : public BmpPixbuf
    {
        public:
            BmpImg (void);
            BmpImg (const int width,
                    const int height);
            ~BmpImg ();
            
            enum BmpError write (const std::string& filename);
            enum BmpError read (const std::string& filename);
            
            int get_width (void) const;
            int get_height (void) const;
        private:
            // Use a struct to read this in one call
            struct
            {
            unsigned int bfSize = 0;
            unsigned int bfReserved = 0;
            unsigned int bfOffBits = 54;
            unsigned int biSize = 40;
            int biWidth = 0;
            int biHeight = 0;
            unsigned short biPlanes = 1;
            unsigned short biBitCount = 24;
            unsigned int biCompression = 0;
            unsigned int biSizeImage = 0;
            int biXPelsPerMeter = 0;
            int biYPelsPerMeter = 0;
            unsigned int biClrUsed = 0;
            unsigned int biClrImportant = 0;
            } header;
    };
    
    class BmpCanvas : public Paint::Canvas {
    private: 
        BmpImg bmpimg;
    
    public:
        BmpCanvas(std::size_t width = 800, std::size_t height = 600) :
            Canvas(width, height), bmpimg(width, height) { }
        
        Paint::RGBColor getPixel(int x, int y) const override {
            if (x < 0 || y < 0 || 
                std::size_t(x) >= width || std::size_t(y) >= height)
                throw std::range_error("pixel out of range");
            uint8_t r = bmpimg.red_at(x, y),
                    g = bmpimg.green_at(x, y),
                    b = bmpimg.blue_at(x, y);   
            return Paint::RGBColor(r, g, b);
        }
        
        void setPixel(int x, int y, Paint::RGBColor color) override {
            if (x < 0 || y < 0 || 
                std::size_t(x) >= width || std::size_t(y) >= height)
                return;
            bmpimg.set_pixel(x, y, color.red, color.green, color.blue); 
        }
        
        void reset(std::size_t width, std::size_t height) {
            this->width = width;
            this->height = height;
            bmpimg = BmpImg(width, height); 
        }

        void save(std::string filename) {
            if (static_cast<int>(bmpimg.write(filename)) < 0)
                throw std::runtime_error("cannot save to '" + filename + "'");
        }
    };

}

#endif /* __LIBBMP_H__ */
