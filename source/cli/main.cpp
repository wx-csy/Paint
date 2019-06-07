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

#include <cstdio>
#include <cstdlib>
#include <cstring>

bool mathcoord = false;
void batch();

[[noreturn]] void usage(const char *prog) {
    std::fprintf(stderr,
        "Usage: %s [ -i ] [ input ]\n"
        "\n"
        "-i\tUse mathematical coordinate system.\n"
        "input\tThe input file. If omitted, read from stdin.\n",
        prog);
    exit(EXIT_FAILURE);
}

void parsearg(int argc, char *argv[]) {
    for (int i = 1; i < argc; i++) {
        if (argv[i][0] == '-') {
            if (std::strcmp(argv[i], "-i") == 0) {
                mathcoord = true;
            } else {
                usage(argv[0]);
            }
        } else {
            if (std::freopen(argv[i], "r", stdin) == nullptr) {
                fprintf(stderr, "failed to open file '%s'\n", argv[1]);
                exit(EXIT_FAILURE);
            }
        }
    }
}

int main(int argc, char *argv[]) {
    parsearg(argc, argv);
    batch();
    return 0;    
}
