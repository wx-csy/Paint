# Paint

## Build

The prebuilt binary file is in `binary` directory. However, recompilation on your machine is strongly recommended to avoid potential incompatibility.

Softwares used to compile and run: 

- OS: Ubuntu 18.04.2 LTS
- C++ compiler: g++ 7.3.0
- make: GNU Make 4.1
- qt: QMake version 3.1, using Qt version 5.9.5 
- typesetting engine: XeTeX 3.14159265-2.6-0.99998
- reference management software: BibTeX 0.99d

To build binary executable, type

```bash
make targets
```

The target file is `build/painter` (CLI) and `build/painter-gui` (GUI).

To generate document (report), type

```bash
make doc
```

##  License

This software is licensed under GNU GPL v3.

​	

