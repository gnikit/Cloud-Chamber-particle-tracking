#pragma once
#if defined(__INTEL_COMPILER)
#include <mathimf.h>  // Intel Math library
#define COMPILER "INTEL"
#elif defined(__GNUC__)
#include <math.h>
#define COMPILER "G++"
#else
#include <math.h>
#define COMPILER "ELSE"
#endif

#include <algorithm>
#include <cassert>
#include <fstream>
#include <iostream>
#include <iterator>
#include <string>
#include <vector>

#include "EasyBMP.h"

class ImportImage {
 protected:
  typedef std::vector<std::vector<int>> vec2d;
  vec2d _Bg_pixels;  // 2D vectors containing
  vec2d _In_pixels;  // R values for every pixel
  BMP _Bg_image;
  BMP _In_image;
  size_t width;
  size_t height;

 public:
  std::string _in_fname;       // Input filename
  std::string _bg_fname;       // Background filename
  std::string _in_fname_base;  // Input basename
  std::string _bg_fname_base;  // Background basename

  /**
   * @brief Construct a new Import Image object.
   * Allows the user to input two BMP files by name that are located in the same
   * directory as *Source.cpp. The two BMP images (assumed to be Greyscaled
   * (R=G=B)) are then passed into two 2D vectors. One for the Background (BG)
   * image and one for the Inserted (IN) image.
   *
   *Then the method carries on with a pixel-to-pixel substraction (In_image (2nd
   *par) - Bg_image (1st par))
   *
   * @param Bg_image_filename background image filename
   * @param In_imagefile_name input image filename
   */
  ImportImage(const std::string& Bg_image_filename,
              const std::string& In_imagefile_name);
  ~ImportImage();

  /**
   * @brief
   *
   * @param Export_filename
   */
  void ExportImage(const std::string& Export_filename);
  std::vector<std::vector<int>> PassBgPixelsVector();
  std::vector<std::vector<int>> PassInPixelsVector();
};