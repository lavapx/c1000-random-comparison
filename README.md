# c1000-random-comparison
A small benchmark command line tool for comparing std and boost random engines. Motivation and process can be found at the bottom of [this article](http://www.lavaxp.net/cpp-standard-library-in-game-dev-2-2-std-random-2/).

![c1000](https://user-images.githubusercontent.com/27367475/69744801-bcdf3c80-10f5-11ea-8996-f969bf9e0ed2.png)


## Installation

If you're using Visual Studio 2015, the needed boost source and libs are included, if you want to compile your own libs:
1. Download Boost
2. run Bootstrap.bat or .sh
3. run this script and change the [toolset](http://www.boost.org/build/doc/html/bbv2/reference/tools.html#bbv2.reference.tools.compilers) option to what you're using:

```
b2 --stagedir=./stage/x64/debug address-model=64 --build-type=complete --toolset=msvc-14.0 --threading=multi --runtime-link=static --variant=debug
b2 --stagedir=./stage/x64/release address-model=64 --build-type=complete --toolset=msvc-14.0 --threading=multi --runtime-link=static --variant=release

b2 --stagedir=./stage/win32/debug --build-type=complete --toolset=msvc-14.0 --threading=multi --runtime-link=static --variant=debug
b2 --stagedir=./stage/win32/release --build-type=complete --toolset=msvc-14.0 --threading=multi --runtime-link=static --variant=release
```

## Usage

`c1000 -h`

## Built With

[Visual C++ Standard Library](https://msdn.microsoft.com/en-us/library/ct1as7hw.aspx)

[Boost](http://www.boost.org/)

## License

[Boost License](http://www.boost.org/LICENSE_1_0.txt)

All original code is licensed under the MIT License.
