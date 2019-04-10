#include <vector>
#include <string>
#include <utility>
#include <iostream>
#include <algorithm>
#include <unordered_map>
#include <memory>
#include "paint.h"
#include "libbmp.h"
#include "util.h"
#include "common.h"

using util::from_string;
using util::limit_range;

static LibBmp::BmpCanvas canvas;
static Paint::RGBColor forecolor;
static std::unordered_map<int, std::unique_ptr<Paint::Element>> elems;
static const std::unordered_map<std::string, 
        Paint::LineDrawingAlgorithm> ldalg {
    { "DDA",            Paint::LineDrawingAlgorithm::DDA            },
    { "Bresenham",      Paint::LineDrawingAlgorithm::Bresenham      },
};
static int line = 0;

using CommandHandler = void (*)(std::vector<std::string>& args);

static void resetCanvas(std::vector<std::string>& args) {
    if (args.size() != 3) 
        throw std::invalid_argument("invalid argument number");
    std::size_t width = limit_range<std::size_t>(from_string(args[1]), 
                    0, Paint::MAX_COORDINATE),
                height = limit_range<std::size_t>(from_string(args[2]),
                    0, Paint::MAX_COORDINATE);
    canvas.reset(width, height);
}

static void saveCanvas(std::vector<std::string>& args) {
    if (args.size() != 2)
        throw std::invalid_argument("invalid argument number");
    for (auto& e : elems)
        e.second->paint(canvas);
    canvas.save(args[1]);
}

static void setColor(std::vector<std::string>& args) {
    if (args.size() != 4)
        throw std::invalid_argument("invalid argument number");
    std::uint8_t red = limit_range<std::uint8_t>(from_string(args[1])),
                 green = limit_range<std::uint8_t>(from_string(args[2])),
                 blue = limit_range<std::uint8_t>(from_string(args[3]));
    forecolor = Paint::RGBColor(red, green, blue);
}

static void drawLine(std::vector<std::string>& args) {
    if (args.size() != 7) 
        throw std::invalid_argument("invalid argument number");
    int id = from_string(args[1]);
    float x1 = from_string<float>(args[2]), y1 = from_string<float>(args[3]),
          x2 = from_string<float>(args[4]), y2 = from_string<float>(args[5]);
    if (!elems.emplace(id, 
            new Paint::LineElement(x1, y1, x2, y2, forecolor, 
                ldalg.at(args[6]))).second) 
        throw std::invalid_argument(
            "id " + std::to_string(id) + " already exists");
}

static void drawPolygon(std::vector<std::string>& args) {
    if (args.size() != 4) 
        throw std::invalid_argument("invalid argument number");
    int id = from_string(args[1]);
    std::size_t nr_point = 
        limit_range<std::size_t>(from_string(args[2]), 2, 1000000);
    std::string str;
    Paint::LineDrawingAlgorithm algo = ldalg.at(args[3]);
    if (std::getline(std::cin, str).fail())
        throw std::invalid_argument("points of polygon expected");
    line++;
    std::vector<std::string> points_str = util::split(str);
    if (points_str.size() != nr_point * 2)
        throw std::invalid_argument("invalid number of coordinates");
    std::vector<std::pair<float, float>> points;
    for (size_t i = 0; i < nr_point; i++) 
        points.emplace_back(from_string<float>(points_str[i*2]),
                            from_string<float>(points_str[i*2+1]));
    if (!elems.emplace(id,
                new Paint::PolygonElement(points, forecolor, algo)).second)
        throw std::invalid_argument(
            "id " + std::to_string(id) + " already exists");
}

static const std::unordered_map<std::string, CommandHandler> handler {
    { "resetCanvas",    resetCanvas     },
    { "saveCanvas",     saveCanvas      },
    { "setColor",       setColor        },
    { "drawLine",       drawLine        },
    { "drawPolygon",    drawPolygon     },
};

void batch() {
    std::string command;
    while (std::getline(std::cin, command)) {
        line++;
        std::vector<std::string> tokens = util::split(command);
        if (tokens.empty()) continue;
        try {
            handler.at(tokens.front())(tokens); 
        } catch (const std::exception& ex) {
            std::cerr << "line " << line << ": " << ex.what() << std::endl;
        }
    }
}
