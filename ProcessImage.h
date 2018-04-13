#pragma once
#include "ImportImage.h"
#include <random>
#include <queue>
#include <utility>

class ProcessImage/*:public ImportImage*/
{
protected:
	std::vector<std::vector <int> > _IN, _BG;  // _BG: background vector, _IN: input vector
	int max, maxX, maxY;
	float _cut_off;
	size_t _width, _height;

	struct Statistics {
		float _mean, _sdiv; // Full statistical analysis of BG image
		float _sMean, _sSdiv; // Sample of population
	}; Statistics stat;
	struct Track_data {
		// import data from _data
		double _track_intensity;
		double _track_length;
		double _track_intensity_per_length;
	}; Track_data track;
	struct node {
		node(int VALUE, int X, int Y) {
			intensity = VALUE;
			xcord = X;
			ycord = Y;
		}

		int intensity;
		int xcord;
		int ycord;
	};
	struct LSF // Least Square fitting
	{
		double _a;
		double _b;
		double _r_sqrd;
		double SEa;
		double SEb;
	};
	struct Color {
		Color(size_t RED, size_t GREEN, size_t BLUE) {
			R = RED;
			G = GREEN;
			B = BLUE;
		}
		size_t R;
		size_t G;
		size_t B;
	};
	typedef std::vector<std::vector<int>> vec2d;

	std::vector<node> _data; // used to store particle's data
	std::vector<std::vector<node>> _run; // vector of _data
	LSF fit_parameters;		// Least Square Fit parameters
	BMP _FinalImage;
	size_t x; size_t y;
	size_t count;

	std::vector<Color> palette;
	LSF LinearLeastSquareFit(std::vector<node> &data);

public:
	ProcessImage(ImportImage& Input);
	~ProcessImage();
	ProcessImage();

	//_____________________________ STATISTICAL ANALYSIS _____________________________//
	void SampleStatistics(vec2d &v);
	void FullStatistics(vec2d &v, const char *par);

	//_____________________________ IMAGE PROCESSING _________________________________//
	void FloodFill(int X, int Y, float cut_off, int invalid, vec2d &v, std::vector<node> &data);  // 4-way non-recursive flood fill
	void FloodFillRecursive(size_t X, size_t Y, float cut_off, vec2d &v);						  // 4-way recursive flood fill
	void EightWay(size_t X, size_t Y, float cut_off, vec2d &v);									  // 8-way recursive flood fill
	void UndoFloodFill(vec2d& v, std::vector<node>& data, int invalid_replacement);

	//_____________________________ DATA ANALYSIS ____________________________________//
	void BinaryFlush(vec2d &v, float cut_off);
	void ConvertToBinary(vec2d &v, float cut_off);
	//void MergePixels();  // to be included
	double TotalBrightness(std::vector<node> &data);
	double TrackLength(std::vector<node> &data);
	double BrightnessPerLength(std::vector<node> &data);
	void AlphaOrBeta(std::vector<node> &data);

	//_____________________________ RUN DATA _________________________________________//
	void SimpleProcessRun();
	void DetectAll(size_t track_length = 100, size_t area_sensitivity = 20);
	void CompleteAnalysis(size_t track_length = 100, size_t area_sensitivity = 20);

	//_____________________________ EXPORT DATA ______________________________________//
	void ExportImageWhiteBG(const std::string &Export_filename);
	void ExportImageBlackBG(const std::string &Export_filename);
	void SimpleImageExport(const std::string &Export_filename);

	//_____________________________ OPERATORS ________________________________________//
	static bool less_than_xcord(const node& lhs, const node& rhs);
	static bool less_than_ycord(const node& lhs, const node& rhs);

	//_____________________________ RETURN VALUES ____________________________________//
	Statistics getBGStats(const char* choose);
	float getSampleMean();
	float getSampleSdiv();
	float getMean();
	float getSdiv();
	int getMax();  // gives both the value and the coordinates of that point
	size_t getWidth();
	size_t getHeight();
	void getFitParameters();

	//_____________________________ OTHER METHODS ____________________________________//
	void ColorPalette();
};