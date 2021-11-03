#include "ImportImage.h"

ImportImage::~ImportImage() {}

ImportImage::ImportImage(const std::string& Bg_image_filename,
                         const std::string& In_image_filename) {
  // Save the input and background filenames
  _in_fname = In_image_filename;
  _bg_fname = Bg_image_filename;
  // Discard the .bmp extension and keep only the basename
  _in_fname_base = _in_fname.substr(0, _in_fname.length() - 4);
  _bg_fname_base = _bg_fname.substr(0, _bg_fname.length() - 4);

  _In_image.ReadFromFile(_in_fname.c_str());
  _Bg_image.ReadFromFile(_bg_fname.c_str());

  if ((_Bg_image.TellHeight() != _In_image.TellHeight()) ||
      (_Bg_image.TellWidth() != _In_image.TellWidth())) {
    std::cout
        << "Images are Background and Foreground images are different sizes."
        << std::endl;
  } else {
    width = _Bg_image.TellWidth();
    height = _Bg_image.TellHeight();
  }

  // Resizing the vectors to images dimensions
  _Bg_pixels.resize(width);
  _In_pixels.resize(width);

  for (size_t j = 0; j < width; j++) {
    _Bg_pixels[j].resize(height);
    _In_pixels[j].resize(height);
  }

  // Populate Input and Background vectors
  for (size_t i = 0; i < width; i++) {
    for (size_t j = 0; j < height; j++) {
      /*Only need one value from RGB since all of them are the same*/
      /*Prone to errors if the 2 images do not have the same dimensios*/
      _Bg_pixels[i][j] = _Bg_image(i, j)->Red;  // populating background vector
      _In_pixels[i][j] = _In_image(i, j)->Red;
    }
  }
}

void ImportImage::ExportImage(const std::string& Export_filename) {
  size_t MAX_RGB = 255;
  for (int i = 0; i < _In_image.TellWidth(); i++) {
    for (int j = 0; j < _In_image.TellHeight(); j++) {
      /*Converts the Image to White BG and Black Trajectories*/
      _In_image(i, j)->Red = MAX_RGB - _In_pixels[i][j];
      _In_image(i, j)->Blue = MAX_RGB - _In_pixels[i][j];
      _In_image(i, j)->Green = MAX_RGB - _In_pixels[i][j];
    }
  }
  _In_image.WriteToFile(Export_filename.c_str());
}

std::vector<std::vector<int>> ImportImage::PassBgPixelsVector() {
  return _Bg_pixels;  // Background vector
}

std::vector<std::vector<int>> ImportImage::PassInPixelsVector() {
  return _In_pixels;
}