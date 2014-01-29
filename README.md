C++ OpenGL Freetype & HarfBuzz text drawing
---------------------------------------

Simple library to draw text in OpenGL

### License

Copyright (c) 2014 Sébastien Rombauts (sebastien.rombauts@gmail.com)

Distributed under the MIT License (MIT) (See accompanying file LICENSE.txt
or copy at http://opensource.org/licenses/MIT)

### Getting and Building the dependencies

1. Get the development libraries under Linux :

```bash
sudo apt-get install libxrandr-dev libxi-dev libgl1-mesa-dev mesa-common-dev
```

### Building the application

On Linux :

```bash
cmake .
cmake --build .     # of simply "make"
```

On Windows :

```bash
cmake . -G "Visual Studio 10"
cmake --build .     # or simply [open and build solution]
```
