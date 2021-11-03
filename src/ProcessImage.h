#pragma once
#include <queue>
#include <random>
#include <utility>

#include "ImportImage.h"

struct Statistics {
  double mean, sdiv;    // Full statistical analysis of BG image
};

struct Track_data {
  // import data from _data
  double _track_intensity;
  double _track_length;
  double _track_intensity_per_length;
};

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

struct LSF  // Least Square fitting
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

class ProcessImage {
 protected:
  std::vector<std::vector<int>> _IN;  // Input vector
  std::vector<std::vector<int>> _BG;  // Background vector
  int max, maxX, maxY;
  double _cut_off;             // cut-off brightness
  size_t _WIDTH;               // image width
  size_t _HEIGHT;              // image height
  const size_t MAX_RGB = 255;  // Max RGB value

  std::string _in_fname;       // Input filename
  std::string _bg_fname;       // Background filename
  std::string _in_fname_base;  // Input basename
  std::string _bg_fname_base;  // Background basename

  std::vector<node> _data;              // used to store particle's data
  std::vector<std::vector<node>> _run;  // vector of _data
  LSF fit_parameters;                   // Least Square Fit parameters
  BMP _FinalImage;
  size_t x;
  size_t y;
  size_t count;

  Statistics stat;
  Track_data track;
  std::vector<Color> palette;
  LSF LinearLeastSquareFit(std::vector<node> &data);

 public:
  /**
   * @brief Construct a new Process Image object
   *
   * @param Input Image object
   */
  ProcessImage(ImportImage &Input);
  ~ProcessImage();
  ProcessImage();

  //_____________________________ STATISTICAL ANALYSIS _______________________//

  /**
   * @brief Calculates the statistical quantities of MEAN and STD DEVIATION of
   * a 2D vector<int> and stores them to a struct Statistics. The method is
   * tuned to only operate for POSITIVE INTs. Modify if (*column >=0) to account
   * for any/ no threshold.
   *
   * @param v Vector derived from an Image
   * @param remove_zero Excludes pixels the 0 values from the statistical
   *                    calculation. Usefull, for analysis after the vector has
   *                    been "Flushed".
   */
  void CalculateStats(vec2d &v, const bool remove_zero = false);

  //_____________________________ IMAGE PROCESSING ___________________________//

  /**
   * @brief A 4-way non- recursive algorithm that uses an existing struct NODE
   * and a 2D VECTOR<int> containing Greyscale values	of an image. The
   * algorithm uses a QUEUE<NODE> to Temporarily store the X, Y coordinates of
   * the points in the 2D VECTOR. And the DATA<NODE> VECTOR to store that data
   * permanently.
   *
   * The valid values of the 2D VECTOR are substituted with an invalid value
   * (-1) and their coordinates along with their intensity values are stored in
   * a VECTOR<NODE>
   *
   * @param X x-coordinate of the input node
   * @param Y y-coordinate of the input node
   * @param cut_off A threshold value for intensity that determines for which
   *                pixels the algorithms will run
   * @param invalid A value that substitutes the intensity of the pixels the
   *                algorithms operates on. Pixels are "marked" invalid in order
   *                to be identifiable later on by their value
   * @param v vector that will be operated on
   * @param data Holds (Intensity, X, Y) for each node the algorithm is true
   */
  void FloodFill(int X, int Y, double cut_off, int invalid, vec2d &v,
                 std::vector<node> &data);

  /**
   * @brief A 4-way recursive flood fill algorithm, meant to be run as part of
   * the class: ProcessImage. It utilises a 2D VECTOR<int>, the struct
   * STATISTICS and methods from the STATISTICAL ANALYSIS section
   *
   * @warning In some cases it can cause a stack overflow (especially when
   * run in Debug mode)
   *
   * @param X x-coordinate of the input node
   * @param Y y-coordinate of the input node
   * @param cut_off A threshold value for intensity that determines for which
   *                pixels the algorithms will run
   * @param v vector that will be operated on
   */
  void FloodFillRecursive(size_t X, size_t Y, double cut_off, vec2d &v);

  /**
   * @brief 8-way recursive flood fill Recursive Flood Fill Algorithm that in
   * addition to the 4-way method, it also operates on the diagonal pixels of
   * the input point.
   *
   * @warning In some cases it can cause a stack overflow (especially when
   * run in Debug mode)
   *
   * [SEE FloodFillRecursive() DOCUMENTATION FOR MORE INFO]
   *
   * @param X x-coordinate of the input node
   * @param Y y-coordinate of the input node
   * @param cut_off A threshold value for intensity that determines for which
   *                pixels the algorithms will run
   * @param v The vector that will be operated on
   */
  void EightWay(size_t X, size_t Y, double cut_off, vec2d &v);

  /**
   * @brief A method meant to replace the INVALID value in 2D vector v
   * used in the Flood Fill with the INVALID_REPLACEMENT value.
   * It requires the input of a VECTOR<NODE> that stores the values of the Flood
   * Filled region.
   *
   * @param v vector equivelent of the image, contains the INVALID values
   * @param data Vector containing the data (INTENSITY,X,Y) of the points where
   *              the FloodFill should be undone
   * @param invalid_replacement A value to replace the original invalid. Marks
   *                            these nodes as points of interest.
   *                            Default value is -2
   */
  void UndoFloodFill(vec2d &v, std::vector<node> &data,
                     int invalid_replacement);

  //_____________________________ DATA ANALYSIS ______________________________//

  /**
   * @brief "Flushes" the vector in a Binary like way.
   * The method will replace all the points in VECTOR V that are smaller
   * than the CUT_OFF value with 0. [Use to reduce background noise]
   *
   * @param v vector that will be operated on
   * @param cut_off A threshold value for intensity that determines for which
   *                pixels the algorithms will run
   */
  void Threshold(vec2d &v, const double cut_off);
  /**
   * @brief This method converts the input vector into a binary RGB value
   * vector. The values of the vector that are smaller than the cut_off get
   * substituted by 0, while the values larger than the cut_off get substituted
   * with MAX_RGB = 255
   *
   * @param v vector that will be operated on
   * @param cut_off A threshold value for intensity that determines for which
   *                pixels the algorithms will run
   */
  void ConvertBlackWhite(vec2d &v, const double cut_off);

  /**
   * @brief Averages the pixel values of vector v across a square with
   * area patch_sz x patch_sz
   * 
   * @param v Input pixel vector
   * @param patch_sz Cells across which to average
   */
  void SmoothImage(vec2d &v, const int patch_sz = 1);

  /**
   * @brief Calculates the tracks intensity. Uses an instance of STRUCT
   * Track_data to store the track intensity.
   *
   * @param data Image data
   * @return double
   */
  double TotalBrightness(std::vector<node> &data);

  /**
   * @brief Calculates the length of the best fit line by projecting the
   * points in the trajectory onto the best fit ( X, Y -> best fit points),
   * sorting them and using the Pythagorean Theorem to calculate the hypotenuse
   *
   * @param data Image data
   * @return double
   */
  double TrackLength(std::vector<node> &data);

  /**
   * @brief Intensity per unit length
   *
   * @param data Image data
   * @return double
   */
  double BrightnessPerLength(std::vector<node> &data);
  /**
   * @brief Uses the function y = bx + a, to map a cluster of scatter points
   * onto that line. The method calculates the gradient of the line b, the
   * y-intercept a, the errors of these two parameters SEb and SEa, accordingly
   * and the correlation coef r_sqrd. Returns, an instance of the struct LSF
   * called fit_parameters.
   *
   * @param data Image data
   */
  void AlphaOrBeta(std::vector<node> &data);

  //_____________________________ RUN DATA ___________________________________//

  void SimpleProcessRun();
  void DetectAll(size_t track_length = 100, size_t area_sensitivity = 20);
  void CompleteAnalysis(size_t track_length = 100,
                        size_t area_sensitivity = 20);

  //_____________________________ EXPORT DATA ________________________________//

  void ExportImageWhiteBG(const std::string &Export_filename);
  void ExportImageBlackBG(const std::string &Export_filename);
  void SimpleImageExport(const std::string &Export_filename);

  //_____________________________ OPERATORS __________________________________//

  static bool less_than_xcord(const node &lhs, const node &rhs);
  static bool less_than_ycord(const node &lhs, const node &rhs);

  //_____________________________ RETURN VALUES ______________________________//


  double getMean(vec2d& v, const bool remove_zero = false);
  double getSdiv(vec2d& v, const bool remove_zero = false);
  int getMax();  // gives both the value and the coordinates of that point
  size_t getWidth();
  size_t getHeight();
  void getFitParameters();

  //_____________________________ OTHER METHODS ______________________________//

  void ColorPalette();
};