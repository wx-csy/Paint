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

#ifndef __SRC_PRIMITIVE_ALGO_H__
#define __SRC_PRIMITIVE_ALGO_H__

#include <cmath>
#include <utility>

// degree is given clockwise
static inline void init_rotate_matrix(float deg, float mat[2][2]) {
    float rad = std::fmod(deg, 360.0f) / 180.0f * std::acos(-1.0f);
    mat[0][0] = std::cos(rad);  mat[0][1] = -std::sin(rad);
    mat[1][0] = -mat[0][1];     mat[1][1] = mat[0][0];
}

template <size_t N>
static inline void matrix_transform(float mat[N][N], float point[N]) {
    float res[N] = {};
    for (size_t i = 0; i < N; i++)
        for (size_t j = 0; j < N; j++)
            res[i] += mat[i][j] * point[j];
    for (size_t i = 0; i < N; i++)
        point[i] = res[i];
}

static inline std::pair<float, float> rel_mat_apply
        (float cx, float cy, float x, float y, float mat[2][2]) {
    float point[2] = {x - cx, y - cy};
    matrix_transform(mat, point);
    return std::make_pair(point[0] + cx, point[1] + cy);
}

static inline std::pair<float, float> rel_scale
        (float cx, float cy, float x, float y, float s) {
    return std::make_pair((x - cx) * s + cx, (y - cy) * s + cy);
}

#endif