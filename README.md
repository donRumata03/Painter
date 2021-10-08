# Painter
C++ program which deducts the best combination of strokes required to draw an image by a robot painter.


## Brief tecnical desciption (feature list)

The tool is built around optimization algorithms such as [GA](https://en.wikipedia.org/wiki/Genetic_algorithm), [Simulated Annealing](https://en.wikipedia.org/wiki/Simulated_annealing)

_(almost up-to-date)_ Full technical description is currently avialable in russian here: https://github.com/donRumata03/ITMO.STARS_texts/blob/master/FinalResults/Painter.pdf 

## Installation && building

As you could have noticed, build system is CMake and the language is C++.

The third-party libraries are:
- OpenCV (actually, only primitive features for manipulating with images are utilized, ML and NNs are not used)
- Google's C++ testing library
- [sammycage/lunasvg](https://github.com/sammycage/lunasvg) — for both working with SVG as text and rendering it
- argparse — for parsing command line arguments (marvelous!)


Apart from the vendored libs, there are also my (@donRumata03) projects which are ubiquitously used in `Painter` and are sometimes developed within the framework of `Painter`:
  1) https://github.com/donRumata03/pythonic — Common primitives of C++
  2) https://github.com/donRumata03/PowerfulGA — optimization framework (GA with author's modifications, simulated annealing, gradient descent with n_ary search in logariphmic space, Newton-Raphson method)

The «links» to most of the dependencies are situated in the `dependencies` directory.
So, if you aren't going to commit into other libraries and just want to make the prject work, clone it with `--recusive` option 
  `git clone --recusive https://github.com/donRumata03/Painter`

After executing this command, the dependencies will appear in corresponding subfolders of the `./dependencies` directory.

If you want to provide custom paths to dependencies (for example, if you additionally want to contribure into pythonic or PowerfulGA or if you already have some of the third-party libraries), than you can specify the paths to dependencies in *CMake/LibraryConfig.cmake* file (of cource, firstly — create it):
```
set(CURRENT_OPENCV_DIR <...>)
set(CURRENT_GA_DIR <...>)
set(CURRENT_PYTHONIC_DIR <...>)
set(CURRENT_GTEST_DIR <...>)
```

If you have some questions, you're wellcum: t.me/donRumata03
