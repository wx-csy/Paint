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
- [x] drawEllipse
  - [x] Mid-point
- [ ] drawCurve
  - [ ] Bezier
  - [ ] B-spline
- [x] translate
  - [x] Line
  - [x] Polygon
  - [x] Ellipse
  - [ ] Curve
- [x] rotate
  - [x] Line
  - [x] Polygon
  - [ ] Ellipse
  - [ ] Curve
- [ ] scale
  - [ ] Line
  - [ ] Polygon
  - [ ] Ellipse
  - [ ] Curve
- [ ] clip (line)
  - [ ] Cohen-Sutherland
  - [ ] Liang-Barsky

## Build

Softwares used to compile and run: 

- OS: Ubuntu 18.04.2 LTS
- C++ compiler: g++ 7.3.0
- make: GNU Make 4.1
- typesetting engine: XeTeX 3.14159265-2.6-0.99998
- reference management software: BibTeX 0.99d

To build binary executable, type

```bash
make
```

The target file is `build/painter`.

To generate document (report), type

```bash
make doc
```

##  License

This software is licensed under GNU GPL v3.
