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

#include <paint/paint.h>
#include <paint/canvas.h>
#include <paint/primitive.h>
#include <paint/util.h>
#include <paint/common.h>

#include <libbmp.h>

using util::from_string;
using util::limit_range;

extern bool mathcoord;

static Paint::Canvas<LibBmp::BmpDevice> canvas;
static Paint::RGBColor forecolor;

static inline float read_x(std::string str) { return from_string<float>(str); }
static inline float read_y(std::string str) {
    float val = from_string<float>(str);
    if (mathcoord) val = canvas.getHeight() - val;
    return val;
}


static const std::unordered_map<std::string, 
        Paint::Line::Algorithm> ldalg {
    { "DDA",            Paint::Line::Algorithm::DDA            },
    { "Bresenham",      Paint::Line::Algorithm::Bresenham      },
};
static const std::unordered_map<std::string,
    Paint::LineClippingAlgorithm> clipalg {
    { "Cohen-Sutherland",   Paint::LineClippingAlgorithm::CohenSutherland   },
    { "Liang-Barsky",       Paint::LineClippingAlgorithm::LiangBarsky       },
};


static int line = 0;

static bool batch_readline(std::string& str) {
    if (std::getline(std::cin, str).fail()) return false;
    size_t pos = str.find('#');
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
    canvas.primitives.clear();
}

static void resize(std::vector<std::string>& args) {
    if (args.size() != 3)
        throw std::invalid_argument("invalid argument number");
    size_t width = limit_range<size_t>(from_string(args[1]),
                                       0, Paint::MAX_COORDINATE),
        height = limit_range<size_t>(from_string(args[2]),
                                     0, Paint::MAX_COORDINATE);
    canvas.reset(width, height);
}

static void saveCanvas(std::vector<std::string>& args) {
    if (args.size() != 2)
        throw std::invalid_argument("invalid argument number");
    canvas.clear(Paint::Colors::white);
    canvas.paint();
    canvas.save(args[1]);
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
    float x1 = read_x(args[2]), y1 = read_y(args[3]),
          x2 = read_x(args[4]), y2 = read_y(args[5]);
    if (!canvas.primitives.emplace(id,
            new Paint::Line(Paint::PointF(x1, y1), Paint::PointF(x2, y2),
                forecolor, ldalg.at(args[6]))).second)
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
    Paint::Line::Algorithm algo = ldalg.at(args[3]);
    if (!batch_readline(str))
        throw std::invalid_argument("points of polygon expected");
    std::vector<std::string> points_str = util::split(str);
    if (points_str.size() != nr_point * 2)
        throw std::invalid_argument("invalid number of coordinates");
    std::vector<std::pair<float, float>> points;
    for (size_t i = 0; i < nr_point; i++) 
        points.emplace_back(read_x(points_str[i*2]),
                            read_y(points_str[i*2+1]));
    if (canvas.add_primitive(new Paint::Polygon(points, forecolor, algo), id) < 0)
        throw std::invalid_argument("id " + std::to_string(id) + " already exists");
}

static void drawEllipse(std::vector<std::string>& args) {
    if (args.size() != 6) 
        throw std::invalid_argument("invalid argument number");
    int id = from_string(args[1]);
    float x = read_x(args[2]), y = read_y(args[3]),
          rx = from_string<float>(args[4]), ry = from_string<float>(args[5]);
    if (canvas.add_primitive(new Paint::Ellipse(x, y, rx, ry, forecolor), id) < 0)
        throw std::invalid_argument("id " + std::to_string(id) + " already exists");
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
        throw std::invalid_argument("points of curve expected");
    std::vector<std::string> points_str = util::split(str);
    if (points_str.size() != nr_point * 2)
        throw std::invalid_argument("invalid number of coordinates");
    std::vector<Paint::PointF> points;
    for (size_t i = 0; i < nr_point; i++)
        points.emplace_back(read_x(points_str[i*2]),
                            read_y(points_str[i*2+1]));
    if (args[3] == "BSpline") {
        if (canvas.add_primitive(new Paint::BSpline(points, forecolor), id) < 0)
            throw std::invalid_argument("id " + std::to_string(id) + " already exists");
    } else if (args[3] == "Bezier") {
        if (canvas.add_primitive(new Paint::BSpline(points, forecolor), id) < 0)
            throw std::invalid_argument("id " + std::to_string(id) + " already exists");
    } else throw std::invalid_argument("unrecognized curve type");
}

static void translate(std::vector<std::string>& args) {
    if (args.size() != 4) 
        throw std::invalid_argument("invalid argument number");
    int id = from_string(args[1]);
    float dx = from_string<float>(args[2]), dy = -from_string<float>(args[3]);
    canvas[id].translate(dx, dy);
}

static void rotate(std::vector<std::string>& args) {
    if (args.size() != 5) 
        throw std::invalid_argument("invalid argument number");
    int id = from_string(args[1]);
    float cx = read_x(args[2]), cy = read_y(args[3]);
    float rdeg = from_string<float>(args[4]);
    canvas[id].rotate(cx, cy, rdeg);
}

static void scale(std::vector<std::string>& args) {
    if (args.size() != 5)
        throw std::invalid_argument("invalid argument number");
    int id = from_string(args[1]);
    float cx = read_x(args[2]), cy = read_y(args[3]);
    float s = from_string<float>(args[4]);
    canvas[id].scale(cx, cy, s);
}

static void clip(std::vector<std::string>& args) {
    if (args.size() != 7)
        throw std::invalid_argument("invalid argument number");
    int id = from_string(args[1]);
    float x1 = read_x(args[2]), y1 = read_y(args[3]);
    float x2 = read_x(args[4]), y2 = read_y(args[5]);
    Paint::LineClippingAlgorithm algo = clipalg.at(args[6]);
    dynamic_cast<Paint::Line&>(canvas[id]).clip(x1, y1, x2, y2, algo);
}

static const std::unordered_map<std::string, CommandHandler> handler {
    { "resetCanvas",    resetCanvas     },
    { "resize",         resize          },
    { "saveCanvas",     saveCanvas      },
    { "setColor",       setColor        },
    { "drawLine",       drawLine        },
    { "drawPolygon",    drawPolygon     },
    { "drawEllipse",    drawEllipse     },
    { "drawCurve",      drawCurve       },
    { "translate",      translate       },
    { "rotate",         rotate          },
    { "scale",          scale           },
    { "clip",           clip            },
};

void batch() {
    std::string command;
    while (batch_readline(command)) {
        std::vector<std::string> tokens = util::split(command);
        if (tokens.empty()) continue;
        try {
            handler.at(tokens.front())(tokens); 
        } catch (const std::exception& ex) {
            std::cerr << "line " << line << ": " << ex.what() << std::endl;
        }
    }
}
