#include <vector>
#include <string>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <unordered_map>
#include <iterator>
#include <memory>
#include "paint.h"
#include "libbmp.h"
#include "common.h"

static LibBmp::BmpCanvas canvas;
static Paint::Pen pen(canvas);
static std::unordered_map<int, std::unique_ptr<Paint::Element>> elems;

static std::vector<std::string> split(std::string str) {
    std::istringstream iss(str);
    return std::vector<std::string>(
        std::istream_iterator<std::string>(iss),
        std::istream_iterator<std::string>()
    );
}

using CommandHandler = void (*)(std::vector<std::string>& args);

static void resetCanvas(std::vector<std::string>& args) {
    if (args.size() != 3) 
        throw std::invalid_argument("invalid argument number");
    std::size_t width = str2uint<std::size_t>(args[1]), 
                height = str2uint<std::size_t>(args[2]);
    canvas.reset(width, height);
}

static void saveCanvas(std::vector<std::string>& args) {
    if (args.size() != 2)
        throw std::invalid_argument("invalid argument number");
    canvas.save(args[1]);
}

static void setColor(std::vector<std::string>& args) {
    if (args.size() != 4)
        throw std::invalid_argument("invalid argument number");
    std::uint8_t red = str2uint<std::uint8_t>(args[1]),
                 green = str2uint<std::uint8_t>(args[2]),
                 blue = str2uint<std::uint8_t>(args[3]);
    pen = Paint::Pen(canvas, Paint::RGBColor(red, green, blue));
}

static void drawLine(std::vector<std::string>& args) {
    if (args.size() != 6)  
        throw std::invalid_argument("invalid argument number");
    int id = str2int(args[1]);
    float x1 = str2float(args[2]), y1 = str2float(args[3]),
          x2 = str2float(args[4]), y2 = str2float(args[5]);
    if (!elems.emplace(id, new Paint::LineElement(x1, y1, x2, y2)).second) 
        throw std::invalid_argument(
            "id " + std::to_string(id) + " already exists");
}

static const std::unordered_map<std::string, CommandHandler> handler = 
{
    { "resetCanvas",    resetCanvas     },
    { "saveCanvas",     saveCanvas      },
    { "setColor",       setColor        },
    { "drawLine",       drawLine        },
};

void batch() {
    int line = 0;
    std::string command;
    while (std::getline(std::cin, command)) {
        line++;
        std::vector<std::string> tokens = split(command);
        if (tokens.empty()) continue;
        try {
            handler.at(tokens.front())(tokens); 
        } catch (const std::exception& ex) {
            std::cerr << "line " << line << ": " << ex.what() << std::endl;
        }
    }
}
