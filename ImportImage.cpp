#include "ImportImage.h"

ImportImage::~ImportImage() {}

ImportImage::ImportImage(const std::string& Bg_image_filename, const std::string& In_image_filename) {
	/*
	Allows the user to input two BMP files by name that are located in the same directory as Source.cpp.
	The two BMP images (assumed to be Greyscaled (R=G=B)) are then passed into two 2D vectors.
	One for the Background (BG) image and one for the Inserted (IN) image.

	Then the method carries on with a pixel-to-pixel substraction (In_image (2nd par) - Bg_image (1st par))
	*/

	_Bg_image.ReadFromFile(Bg_image_filename.c_str());
	_In_image.ReadFromFile(In_image_filename.c_str()); // conversion from string to char for filename

	if ((_Bg_image.TellHeight() != _In_image.TellHeight()) 
		|| (_Bg_image.TellWidth() != _In_image.TellWidth())) {
		std::cout << "Images are Background and Foreground images are different sizes." << std::endl;
	}
	else {
		width = _Bg_image.TellWidth();
		height = _Bg_image.TellHeight();
	}

	_Bg_pixels.resize(width);	// Resizing the vectors to images dimensions
	_In_pixels.resize(width);

	for (size_t j = 0; j < width; j++) {
		_Bg_pixels[j].resize(height);
		_In_pixels[j].resize(height);
	}

	int temp;
	for (size_t i = 0; i < width; i++) {
		for (size_t j = 0; j < height; j++) {
			/*Only need one value from RGB since all of them are the same*/
			/*Prone to errors if the 2 images do not have the same dimensios*/
			_Bg_pixels[i][j] = _Bg_image(i, j)->Red; // populating background vector
			temp = (_In_image(i, j)->Red - _Bg_image(i, j)->Red);
			if (temp < 0)    // assign 0 to negative values
			{
				_In_pixels[i][j] = 0;
			}
			else {
				_In_pixels[i][j] = temp; // Importing and cleaning Image vector
			}
		}
	}
}

//void ImportImage::InputImage(const std::string &Bg_image_filename, const std::string &In_image_filename)
////*
//	Allows the user to input two BMP files by name that are located in the same directory as Source.cpp.
//	The two BMP images (assumed to be Greyscaled (R=G=B)) are then passed into two 2D vectors.
//	One for the Background (BG) image and one for the Inserted (IN) image.
//
//	Then the method carries on with a pixel-to-pixel substraction (In_image (2nd par) - Bg_image (1st par))
//
//*/
//{
//	_Bg_image.ReadFromFile(Bg_image_filename.c_str());
//	_In_image.ReadFromFile(In_image_filename.c_str()); // conversion from string to char for filename
//
//	// Resizing the vectors to images dimensions
//	_Bg_pixels.resize(_Bg_image.TellWidth());
//	_In_pixels.resize(_Bg_image.TellWidth());
//
//	for (size_t j = 0; j < _Bg_image.TellWidth(); j++)
//	{
//		_Bg_pixels[j].resize(_Bg_image.TellHeight());
//		_In_pixels[j].resize(_Bg_image.TellHeight());
//	}
//
//	//test_file.open("test.txt", std::ios::out | std::ios::trunc); // -------------->
//	int temp;
//	for (size_t i = 0; i < _Bg_image.TellWidth(); i++)
//	{
//		for (size_t j = 0; j < _Bg_image.TellHeight(); j++)
//		{
//			/*Only need one value from RGB since all of them are the same*/
//			/*Prone to errors if the 2 images do not have the same dimensios*/
//			_Bg_pixels[i][j] = _Bg_image(i, j)->Red; // populating background vector
//			temp = (_In_image(i, j)->Red - _Bg_image(i, j)->Red);
//
//			if (temp < 0)  // assign 0 to negative values
//			{
//				_In_pixels[i][j] = 0;
//				//test.push_back(0); // -------------->
//			}
//			else
//			{
//				_In_pixels[i][j] = temp; // Importing and cleaning Image vector
//				//test.push_back(temp); // -------------->
//			}
//		}
//	}
//	/*for (size_t i = 0; i < test.size(); i++)
//	{
//	test_file << test[i] << std::endl;
//	}
//	test_file.close();*/
//}

void ImportImage::ExportImage(const std::string &Export_filename) {
	size_t MAX_RGB = 255;
	for (size_t i = 0; i < _In_image.TellWidth(); i++) {
		for (size_t j = 0; j < _In_image.TellHeight(); j++) {
			/*Converts the Image to White BG and Black Trajectories*/
			_In_image(i, j)->Red = MAX_RGB - _In_pixels[i][j];
			_In_image(i, j)->Blue = MAX_RGB - _In_pixels[i][j];
			_In_image(i, j)->Green = MAX_RGB - _In_pixels[i][j];
		}
	}
	_In_image.WriteToFile(Export_filename.c_str());
}

std::vector <std::vector<int>> ImportImage::PassBgPixelsVector() {
	return _Bg_pixels; // Background vector
}

std::vector <std::vector<int>> ImportImage::PassInPixelsVector() {
	return _In_pixels;
}