# Paint

## Development Status

- [x] saveCanvas
- [x] setColor
- [x] drawLine
  - [x] DDA
  - [x] Bresenham
- [x] drawPolygon
  - [x] DDA
  - [x] Bresenham
- [ ] drawEllipse
  - [ ] Mid-point
- [ ] drawCurve
  - [ ] Bezier
  - [ ] B-spline
- [x] translate
  - [x] Line
  - [x] Polygon
  - [ ] Ellipse
  - [ ] Curve
- [ ] rotate
  - [ ] Line
  - [ ] Polygon
  - [ ] Ellipse
  - [ ] Curve
- [ ] scale
  - [ ] Line
  - [ ] Polygon
  - [ ] Ellipse
  - [ ] Curve
- [ ] clip (line)


## Environment

This software is developed, compiled and tested on the following platform:

- OS: Ubuntu 18.04.2 LTS
- C++ Compiler: g++ 7.3.0
- make: GNU Make 4.1

## Build

To build the software, simply type

```bash
make
```

The target executable is  `build/paint`.

##  License

This software is licensed under GNU GPL v3.
