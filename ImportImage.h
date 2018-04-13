/* Includes the files underneath only once per compilation*/
/*
List of compilers that support #pragma once
Compiler	    #pragma once

Clang	         Supported
Comeau C / C++   Supported
C++Builder XE3	 Supported
Digital Mars C++ Supported
GCC	             Supported(since 3.4)
HP C / aC++	     Supported(since at least A.06.12)
IBM XL C / C++	 Supported(since 13.1.1)
Intel C++ Compiler	    Supported
Microsoft Visual C++	Supported(since 4.2)
Pelles C	     Supported
ARM DS - 5	     Supported
IAR C / C++      Supported
Oracle Developer Studio C / C++	Supported(since 12.5)
Portland Group C / C++	        Not supported
*/
#pragma once
#if defined (__INTEL_COMPILER)
#include <mathimf.h>  // Intel Math library
#define COMPILER "INTEL"
#elif defined (__GNUC__)
#include <math.h>
#define COMPILER "G++"
#else
#include <math.h>
#define COMPILER "YOU KNOW NOTHING JOHN SNOW"
#endif

#include <iostream>
#include <cmath>
#include "EasyBMP.h"
#include <vector>
#include <string>
#include <fstream> // file writing
#include <iterator>
#include <cassert>
#include <algorithm>

class ImportImage {
private:
  std::ofstream test_file;
protected:
  typedef std::vector<std::vector<int>> vec2d;
  vec2d _Bg_pixels; // 2D vectors containing
  vec2d _In_pixels; // R values for every pixel
  BMP _Bg_image;
  BMP _In_image;
  size_t width;
  size_t height;
public:
  std::vector<int> test;
  ImportImage(const std::string& Bg_image_filename, const std::string& In_imagefile_name);
  ~ImportImage();


  void ExportImage(const std::string& Export_filename);
  std::vector <std::vector<int>> PassBgPixelsVector();
  std::vector <std::vector<int>> PassInPixelsVector();
};