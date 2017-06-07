# c1000-random-comparison
A small benchmark command line tool for comparing std and boost random engines. Motivation and process can be found at the bottom of [this article](http://www.lavaxp.net/cpp-standard-library-in-game-dev-2-2-std-random-2/).

## Installation

If you're using Visual Studio 2015, the needed boost source and libs are included, if you want to compile your own libs:
1. Download Boost
2. run Bootstrap.bat or .sh
3. run this script:

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
