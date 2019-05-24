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

#ifndef __CANVAS_H__
#define __CANVAS_H__

#include <map>
#include <type_traits>
#include <memory>

#include <paint/paint.h>
#include <paint/primitive.h>

namespace Paint {

    template <typename DeviceT>
    class Canvas : public DeviceT {
        static_assert(std::is_base_of<ImageDevice, DeviceT>::value,
            "DeviceT must be derived from Image::ImageDevice");

    public:
        std::map<int, std::unique_ptr<Primitive>> primitives;

        void paint() {
            for (auto& ps : primitives)
                ps.second.paint(this);
        }

        template <typename T>
        int add_primitive(T* primitive, int id = -1) {
            if (id == -1)
                id = primitives.empty() ? 0 :primitives.rbegin()->first + 1;
            if (primitives.emplace(id, static_cast<Primitive*>(primitive)).second) id = -1;
            return id;
        }

        Primitive& operator[] (int id) {
            return *primitives[id];
        }
    };
}

#endif