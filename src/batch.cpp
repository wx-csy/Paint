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

static bool batch_readline(std::string& str) {
    size_t pos;
    if (std::getline(std::cin, str).fail()) return false;
    pos = str.find('#');
    if (pos != std::string::npos) str.resize(pos);
    line++;
    return true;
}

using CommandHandler = void (*)(std::vector<std::string>& args);

static void resetCanvas(std::vector<std::string>& args) {
    if (args.size() != 3) 
        throw std::invalid_argument("invalid argument number");
    size_t width = limit_range<size_t>(from_string(args[1]), 
                    0, Paint::MAX_COORDINATE),
           height = limit_range<size_t>(from_string(args[2]),
                    0, Paint::MAX_COORDINATE);
    canvas.reset(width, height);
}

static void saveCanvas(std::vector<std::string>& args) {
    canvas.clear(Paint::Colors::white);
    if (args.size() != 2)
        throw std::invalid_argument("invalid argument number");
    for (auto& e : elems)
        e.second->paint(canvas);
    canvas.save(args[1]);
    elems.clear();
}

static void setColor(std::vector<std::string>& args) {
    if (args.size() != 4)
        throw std::invalid_argument("invalid argument number");
    uint8_t red = limit_range<uint8_t>(from_string(args[1])),
            green = limit_range<uint8_t>(from_string(args[2])),
            blue = limit_range<uint8_t>(from_string(args[3]));
    forecolor = Paint::RGBColor(red, green, blue);
}

static void drawLine(std::vector<std::string>& args) {
    if (args.size() != 7) 
        throw std::invalid_argument("invalid argument number");
    int id = from_string(args[1]);
    float x1 = from_string<float>(args[2]), y1 = from_string<float>(args[3]),
          x2 = from_string<float>(args[4]), y2 = from_string<float>(args[5]);
    if (!elems.emplace(id, 
            new Paint::Line(x1, y1, x2, y2, forecolor, 
                ldalg.at(args[6]))).second) 
        throw std::invalid_argument(
            "id " + std::to_string(id) + " already exists");
}

static void drawPolygon(std::vector<std::string>& args) {
    if (args.size() != 4) 
        throw std::invalid_argument("invalid argument number");
    int id = from_string(args[1]);
    size_t nr_point = 
        limit_range<size_t>(from_string(args[2]), 2, 1000000);
    std::string str;
    Paint::LineDrawingAlgorithm algo = ldalg.at(args[3]);
    if (!batch_readline(str))
        throw std::invalid_argument("points of polygon expected");
    line++;
    std::vector<std::string> points_str = util::split(str);
    if (points_str.size() != nr_point * 2)
        throw std::invalid_argument("invalid number of coordinates");
    std::vector<std::pair<float, float>> points;
    for (size_t i = 0; i < nr_point; i++) 
        points.emplace_back(from_string<float>(points_str[i*2]),
                            from_string<float>(points_str[i*2+1]));
    if (!elems.emplace(id, new Paint::Polygon(points, forecolor, algo)).second)
        throw std::invalid_argument(
            "id " + std::to_string(id) + " already exists");
}

static void drawEllipse(std::vector<std::string>& args) {
    if (args.size() != 6) 
        throw std::invalid_argument("invalid argument number");
    int id = from_string(args[1]);
    float x = from_string<float>(args[2]), y = from_string<float>(args[3]),
          rx = from_string<float>(args[4]), ry = from_string<float>(args[5]);
    if (!elems.emplace(id, new Paint::Ellipse(x, y, rx, ry, forecolor)).second)
        throw std::invalid_argument(
            "id " + std::to_string(id) + " already exists");
}

static void drawCurve(std::vector<std::string>& args) {
    if (args.size() != 4)
        throw std::invalid_argument("invalid argument number");
    int id = from_string(args[1]);
    size_t nr_point =
        limit_range<size_t>(from_string(args[2]), 2, 1000000);
    std::string str;
    // Paint::LineDrawingAlgorithm algo = ldalg.at(args[3]);
    if (!batch_readline(str))
        throw std::invalid_argument("points of polygon expected");
    line++;
    std::vector<std::string> points_str = util::split(str);
    if (points_str.size() != nr_point * 2)
        throw std::invalid_argument("invalid number of coordinates");
    std::vector<Paint::PointF> points;
    for (size_t i = 0; i < nr_point; i++)
        points.emplace_back(from_string<float>(points_str[i*2]),
                            from_string<float>(points_str[i*2+1]));
    if (!elems.emplace(id, new Paint::BezierCurve(points, forecolor)).second)
        throw std::invalid_argument("id " + std::to_string(id) + " already exists");
}

static void translate(std::vector<std::string>& args) {
    if (args.size() != 4) 
        throw std::invalid_argument("invalid argument number");
    int id = from_string(args[1]);
    float dx = from_string<float>(args[2]), dy = from_string<float>(args[3]);
    elems.at(id)->translate(dx, dy);
}

static void rotate(std::vector<std::string>& args) {
    if (args.size() != 5) 
        throw std::invalid_argument("invalid argument number");
    int id = from_string(args[1]);
    float cx = from_string<float>(args[2]), cy = from_string<float>(args[3]);
    float rdeg = from_string<float>(args[4]);
    elems.at(id)->rotate(cx, cy, rdeg);
}

static void scale(std::vector<std::string>& args) {
    if (args.size() != 5)
        throw std::invalid_argument("invalid argument number");
    int id = from_string(args[1]);
    float cx = from_string<float>(args[2]), cy = from_string<float>(args[3]);
    float s = from_string<float>(args[4]);
    elems.at(id)->scale(cx, cy, s);
}

static const std::unordered_map<std::string, CommandHandler> handler {
    { "resetCanvas",    resetCanvas     },
    { "saveCanvas",     saveCanvas      },
    { "setColor",       setColor        },
    { "drawLine",       drawLine        },
    { "drawPolygon",    drawPolygon     },
    { "drawEllipse",    drawEllipse     },
    { "drawCurve",      drawCurve       },
    { "translate",      translate       },
    { "rotate",         rotate          },
    { "scale",          scale           },
};

void batch() {
    std::string command;
    while (batch_readline(command)) {
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
