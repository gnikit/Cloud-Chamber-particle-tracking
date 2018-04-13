#include "ProcessImage.h"

ProcessImage::ProcessImage() {}
ProcessImage::ProcessImage(ImportImage& Input) {
  _BG = Input.PassBgPixelsVector();
  _IN = Input.PassInPixelsVector();

  // Initialise the Image
  _FinalImage.SetSize(1920, 1080);
  _FinalImage.SetBitDepth(32);

  count = 0;
}

ProcessImage::~ProcessImage() {}

// STATISTICAL ANALYSIS
void ProcessImage::SampleStatistics(vec2d& v) {
  /*
  * Calculates the sampled statistical quantities (mean, standard deviation) of
  * a 2D vector of ints and stores them into a struct type Statistics
  */
  using namespace std;
  vector<vector<int> >::iterator row;
  stat._sMean = 0;

  size_t i = 0;
  size_t shift, inc;

  for (row = _BG.begin(); row != _BG.end(); ++row) {
    shift = rand() % 9; // up to 9
    inc = rand() % 30 + 1; // 1 to 30

    for (size_t column = shift; column < row->size(); column += inc) // row wise downshift of the image
    {
      if ((*row).at(column) >= 0) {
        stat._sMean += (*row).at(column);
        ++i;
      }
    }
  }
  stat._sMean /= i;

  stat._sSdiv = 0;
  float temp;
  for (row = _BG.begin(); row != _BG.end(); ++row) {
    shift = rand() % 9;  // up to 9
    inc = rand() % 30 + 1;  // 1 to 30

    for (size_t column = shift; column < row->size(); column += inc) // row wise downshift of the image
    {
      if ((*row).at(column) >= 0) {
        temp = pow(((*row).at(column) - stat._sMean), 2);
        stat._sSdiv += temp;
      }
    }
  }
  stat._sSdiv /= i;
  stat._sSdiv = sqrt(stat._sSdiv);
}

void ProcessImage::FullStatistics(vec2d &v, const char* par) {
  /*
  * Calculates the statistical quantities of MEAN and STD DIVIATION of a 2D vector<int> and stores them
  * to a struct Statistics. The method is tuned to only operate for POSITIVE INTs.
  * Modify if (*column >=0) to account for any/ no threshold.
  * 
  * 2D vector<int> &v: The vector that will be operated on. The vector is passed by reference.
  * const char *par: parameter that if == "exclude0" will not account for the 0 elements in the array
  * _____________________________________________________________________________________________________
  * 
  * 2D vector<int> v: Vector derived from an Image
  * const char *par: Parameter that when "exclude0" it ignores the 0 values of the vector.
  * Usefull, for analysis after the vector has been "Flushed".
  */
  using namespace std;
  vector<vector<int> >::iterator row;
  vector<int>::iterator column;
  stat._mean = 0;

  size_t i = 0;

  for (row = v.begin(); row != v.end(); ++row) // full mean
  {
    for (column = row->begin(); column != row->end(); ++column) {
      if (par == "exclude0") {
        if (*column > 0) {
          stat._mean += *column;
          ++i;
        }
      }
      else {
        if (*column >= 0) {
          stat._mean += *column;
          ++i;
        }
      }
    }
  }
  stat._mean /= i;
  stat._sdiv = 0;
  float temp;
  for (row = v.begin(); row != v.end(); ++row) {
    for (column = row->begin(); column != row->end(); ++column) {
      if (par == "exclude0") {
        if (*column > 0) {
          temp = pow((*column - stat._mean), 2);
          stat._sdiv += temp;
        }
      }
      else {
        if (*column >= 0) {
          temp = pow((*column - stat._mean), 2);
          stat._sdiv += temp;
        }
      }
    }
  }
  stat._sdiv /= i;
  stat._sdiv = sqrt(stat._sdiv);
}

//_____________________________ IMAGE PROCESSING _________________________________//
void ProcessImage::FloodFill(int X, int Y, float cut_off, int invalid, vec2d &v, std::vector<node> &data) {
  /*
  * A 4-way non- recursive algorithm that uses an existing struct NODE and a 2D VECTOR<int>
  * containing Greyscale values	of an image. The algorithm uses a QUEUE<NODE> to Temporarily store
  * the X, Y coordinates of the points in the 2D VECTOR. And the DATA<NODE> VECTOR to store that
  * data permanently.
  * 
  * The valid values of the 2D VECTOR are substituted with an invalid value (-1)
  * and their coordinates along with their intensity values are stored in a VECTOR<NODE>
  * 
  * __________________________________________________________________________________________________
  * int X: x-coordinate of the input node
  * int Y: y-coordinate of the input node
  * float cut_off: A threshold value for intensity that determines for which pixels the algorithms
  * will run
  * int invalid: A value that substitutes the intensity of the pixels the algorithms operates on.
  * Pixels are "marked" invalid in order to be identifiable later on by their value.
  * 2D vector<int> &v: The vector that will be operated on. The vector is passed by reference.
  * 
  * vector<node> &data: Holds (Intensity, X, Y) for each node the algorithm is true.
  */
  using namespace std;
  getWidth();
  getHeight();
  size_t  count = 0;
  if (cut_off == invalid) { return; }
  if (v[X][Y] < cut_off) { return; }

  queue<node> Q;
  Q.push({ v[X][Y], X,Y });
  data.push_back({ v[X][Y], X,Y });
  node n(0, 0, 0);

  while (!Q.empty()) {
    n = Q.front();
    Q.pop();
    if ((n.xcord < _width - 1) && (v[n.xcord + 1][n.ycord] > cut_off)) {
      Q.push({ v[n.xcord + 1][n.ycord], n.xcord + 1, n.ycord });
      data.push_back({ v[n.xcord + 1][n.ycord], n.xcord + 1, n.ycord });
      v[n.xcord + 1][n.ycord] = invalid;
    }
    if ((n.xcord > 0) && (v[n.xcord - 1][n.ycord] > cut_off)) {
      Q.push({ v[n.xcord - 1][n.ycord], n.xcord - 1, n.ycord });
      data.push_back({ v[n.xcord - 1][n.ycord], n.xcord - 1, n.ycord });
      v[n.xcord - 1][n.ycord] = invalid;
    }
    if ((n.ycord < _height - 1) && (v[n.xcord][n.ycord + 1] > cut_off)) {
      Q.push({ v[n.xcord][n.ycord + 1], n.xcord, n.ycord + 1 });
      data.push_back({ v[n.xcord][n.ycord + 1], n.xcord, n.ycord + 1 });
      v[n.xcord][n.ycord + 1] = invalid;
    }
    if ((n.ycord > 0) && (v[n.xcord][n.ycord - 1] > cut_off)) {
      Q.push({ v[n.xcord][n.ycord - 1], n.xcord, n.ycord - 1 });
      data.push_back({ v[n.xcord][n.ycord - 1], n.xcord, n.ycord - 1 });
      v[n.xcord][n.ycord - 1] = invalid;
    }
    v[n.xcord][n.ycord] = invalid;
  }
}

void ProcessImage::FloodFillRecursive(size_t X, size_t Y, float cut_off, vec2d &v) {
  /*
  * A 4-way recursive flood fill algorithm, meant to be run as part of the class: ProcessImage.
  * It utilises a 2D VECTOR<int>, the struct STATISTICS and methods from the STATISTICAL ANALYSIS section
  * 
  * **********************************************************************************************
  * ** CAUTION: In some cases it can cause a stack overflow (especially when run in Debug mode) **
  * **********************************************************************************************
  * 
  * __________________________________________________________________________________________________
  * size_t X: x-coordinate of the input node
  * size_t Y: y-coordinate of the input node
  * float cut_off: A threshold value for intensity that determines for which pixels the algorithms
  * will run
  * 2D vector<int> &v: The vector that will be operated on. The vector is passed by reference.
  */
  int invalid = -1;
  getWidth();
  getHeight();
  if (v[X][Y] > (cut_off)) {
    if (X > 0) {
      v[X][Y] = invalid;
      FloodFillRecursive(X - 1, Y, cut_off, v);
    }
    if (X < _width - 1) {
      v[X][Y] = invalid;
      FloodFillRecursive(X + 1, Y, cut_off, v);
    }
    if (Y > 0) {
      v[X][Y] = invalid;
      FloodFillRecursive(X, Y - 1, cut_off, v);
    }
    if (Y < _height - 1, cut_off) {
      v[X][Y] = invalid;
      FloodFillRecursive(X, Y + 1, cut_off, v);
    }
    v[X][Y] = invalid;
  }
}

void ProcessImage::EightWay(size_t X, size_t Y, float cut_off, vec2d &v) {
  /*
  * Recursive Flood Fill Algorithm that in addition to the 4-way method, it also
  * operates on the diagonal pixels of the input point.
  * 
  * **********************************************************************************************
  * ** CAUTION: In some cases it can cause a stack overflow (especially when run in Debug mode) **
  * **********************************************************************************************
  * 
  * [SEE FloodFillRecursive() DOCUMENTATION FOR MORE INFO]
  */
  const int invalid = -1;
  _cut_off = stat._sMean + 1 * stat._sSdiv;

  getWidth();
  getHeight();
  if (v[X][Y] > (cut_off)) {
    if (X > 0) {
      v[X][Y] = invalid;
      EightWay(X - 1, Y, cut_off, v);
    }
    if (X < _width - 1) {
      v[X][Y] = invalid;
      EightWay(X + 1, Y, cut_off, v);
    }
    if (Y > 0) {
      v[X][Y] = invalid;
      EightWay(X, Y - 1, cut_off, v);
    }
    if (Y < _height - 1) {
      v[X][Y] = invalid;
      EightWay(X, Y + 1, cut_off, v);
    }
    if ((X > 0) && (Y < _height - 1)) {
      v[X][Y] = invalid;
      EightWay(X - 1, Y + 1, cut_off, v);
    }
    if ((X < _width - 1) && (Y > 0)) {
      v[X][Y] = invalid;
      EightWay(X + 1, Y - 1, cut_off, v);
    }
    if ((X < _width - 1) && (Y < _height - 1)) {
      v[X][Y] = invalid;
      EightWay(X + 1, Y + 1, cut_off, v);
    }
    if ((X > 0) && (Y > 0)) {
      v[X][Y] = invalid;
      EightWay(X - 1, Y - 1, cut_off, v);
    }
    v[X][Y] = invalid;
  }
}

void ProcessImage::UndoFloodFill(vec2d& v, std::vector<node>& data, int invalid_replacement) {
  /*
  * A method meant to replace the INVALID value in 2D vector v
  * used in the Flood Fill with the INVALID_REPLACEMENT value.
  * It requires the input of a VECTOR<NODE> that stores the values of the Flood Filled region.
  * 
  * _________________________________________________________________________________________________
  * 2D vector<int> &v: The vector equivelant of the image analysed, contains the INVALID values
  * vector<node> &data: Vector containing the data (INTENSITY,X,Y) of the points where the
  * FloodFill should be undone.
  * int invalid_replacement: A value to replace the original invalid. Marks these nodes as
  * points of interest. [-2 is the default]
  */
  for (size_t i = 0; i < data.size(); i++) {
    v[data[i].xcord][data[i].ycord] = invalid_replacement;
  }
}

//_____________________________ DATA ANALYSIS ____________________________________//
void ProcessImage::BinaryFlush(vec2d& v, float cut_off) {
  /*
  * "Flushes" the vector in a Binary like way.
  * The method will replace all the points in VECTOR V that are smaller
  * than the CUT_OFF value with 0. [Use to reduce background noise]
  */
  using namespace std;
  for (size_t i = 0; i < v.size(); i++) {
    for (size_t j = 0; j < v[0].size(); j++) {
      if (v[i][j] < cut_off) {
        v[i][j] = 0;
      }
    }
  }
}

void ProcessImage::ConvertToBinary(vec2d& v, float cut_off) {
  /*
  * This method converts the input vector into a binary RGB value vector.
  * The values of the vector that are smaller than the cut_off get substituted by 0,
  * while the values larger than the cut_off get substitued with MAX_RGB = 255
  */
  using namespace std;
  int MAX_RGB = 255;
  for (size_t i = 0; i < v.size(); i++) {
    for (size_t j = 0; j < v[0].size(); j++) {
      if (v[i][j] < cut_off) {
        v[i][j] = 0;
      }
      else {
        v[i][j] = MAX_RGB;
      }
    }
  }
}

double ProcessImage::TotalBrightness(std::vector<node>& data) {
  /*
  * Calculates the tracks intensity. Uses an instance of STRUCT Track_data to
  * store the track intensity.
  * _____________________________________________________________________________________
  * vector<node> &data: Data of the track
  */
  track._track_intensity = 0;
  for (size_t i = 0; i < data.size(); i++) {
    track._track_intensity += data[i].intensity;
  }
  return track._track_intensity;
}

double ProcessImage::TrackLength(std::vector<node>& data) {
  /*
  * Calculates the length of the best fit line by projecting the
  * points in the trajectory onto the best fit ( X, Y -> best fit points),
  * sorting them and using the Pythagorean Theorem to calculate the hypotenuse
  */
  using namespace std;
  long double b = fit_parameters._b;
  long double a = fit_parameters._a;

  vector<node> projection(data);
  int x_proj; int y_proj;
  for (int i = 0; i < data.size(); i++) {
    y_proj = (b*data[i].xcord + b * b*data[i].ycord + a) / (b*b + 1);
    projection[i].ycord = y_proj;

    x_proj = (data[i].xcord + b * data[i].ycord - b * a) / (b*b + 1);
    projection[i].xcord = x_proj;
    projection[i].intensity = 0;
  }
  sort(projection.begin(), projection.end(), ProcessImage::less_than_xcord);

  int N = projection.size() - 1;
  long double Dx = projection[0].xcord - projection[N].xcord;
  long double Dy = projection[0].ycord - projection[N].ycord;
  track._track_length = sqrt(Dx*Dx + Dy * Dy);

  return track._track_length;
}

double ProcessImage::BrightnessPerLength(std::vector<node>& data) {
  track._track_intensity_per_length = track._track_intensity / track._track_length;
  return track._track_intensity_per_length;
}

void ProcessImage::AlphaOrBeta(std::vector<node>& data) {
  using namespace std;

  double threshold = 750;  //determined by examining individual images of alphas
  if (track._track_intensity_per_length >= threshold) {
    cout << "The trajectory analysed is from an Alpha particle." << endl;
  }
  //else if (400 < track._track_intensity_per_length && threshold > track._track_intensity_per_length)
  //{
  //	cout << "The particle cannot be determined. Do futher fitting analysis to establish the kind" << endl;
  //}
  else {
    cout << "The trajectory analysed is from an Beta particle." << endl;
  }
}

ProcessImage::LSF ProcessImage::LinearLeastSquareFit(std::vector<node> &data) {
  /*
  * Uses the function y = bx + a, to map a cluster of scatter points onto
  * that line. The method calculates the gradient of the line b, the y-intercept a,
  * the errors of these two parameters SEb and SEa, accordingly and the correlation coef
  * r_sqrd.
  * Returns, an instance
  * of the struct LSF called fit_parameters.
  * ______________________________________________________________________
  * vector<node> data: Contains the data of the track that will be fitted
  */
  long double SSxx = 0, SSyy = 0, SSxy = 0;
  long double x_mean = 0, y_mean = 0;
  size_t n = 0;
  for (size_t i = 0; i < data.size(); i++) {
    SSxx += (data[i].xcord*data[i].xcord);
    SSyy += (data[i].ycord*data[i].ycord);
    SSxy += (data[i].xcord*data[i].ycord);

    x_mean += data[i].xcord;
    y_mean += data[i].ycord;
    n++;
  }
  x_mean /= n;
  y_mean /= n;

  SSxx = SSxx - n * x_mean* x_mean;
  SSyy = SSyy - n * y_mean* y_mean;
  SSxy = SSxy - n * x_mean* y_mean;

  fit_parameters._b = SSxy / SSxx;
  fit_parameters._a = y_mean - fit_parameters._b* x_mean;
  fit_parameters._r_sqrd = (SSxy*SSxy) / (SSxx*SSyy);

  long double s = sqrt((SSyy - fit_parameters._b*SSxy) / (n - 2));

  fit_parameters.SEa = s * sqrt(1 / n + (x_mean*x_mean) / (SSxx));
  fit_parameters.SEb = s * sqrt(1 / SSxx);

  return fit_parameters;
}

//_____________________________ RUN DATA _________________________________________//

void ProcessImage::SimpleProcessRun() {
  using namespace std;
  getMax();
  SampleStatistics(_BG); // used to determine cut_off
  _cut_off = stat._sMean + 1 * stat._sSdiv;

  BinaryFlush(_IN, _cut_off); // optional
  FullStatistics(_IN, "exclude0");			// optional
  //_cut_off = stat._mean + 1 * stat._sdiv;
  int _invalid = -1;

  FloodFill(maxX, maxY, _cut_off, _invalid, _IN, _data);
  LinearLeastSquareFit(_data);  // Runs a LLST fit for the particle
  getFitParameters();  // Prints fit parameters for the tracks
  getFitParameters();
  cout << endl;
  cout << "Total Brightness of the track: " << TotalBrightness(_data) << endl;;
  cout << "Length of the track: " << TrackLength(_data) << endl;
  cout << "Brightness per unit Lenght: " << BrightnessPerLength(_data) << endl;
  AlphaOrBeta(_data);
  cout << "______________________________________________________" << endl;
}

void ProcessImage::DetectAll(size_t track_length, size_t area_sensitivity) {
  using namespace std;
  //SampleStatistics(_IN);
  _cut_off = (stat._sMean + 1 * stat._sSdiv) / 2;  // usually is effective as a cut_off

  FullStatistics(_IN, "include0");  // OR use this
  _cut_off = (stat._mean + 1 * stat._sdiv);

  cout << "Using a cut off of: " << _cut_off
    << " for the Binary flush process" << endl;

  BinaryFlush(_IN, _cut_off); // <- small cut_off yields better results
  getMax();

  _cut_off *= 2;
  count = 0;
  int _invalid = -1;
  while (max > _cut_off)  // max brightness sensitivity
  {
    vector<node> temp;
    FloodFill(maxX, maxY, _cut_off / 2, _invalid, _IN, temp);  // using original cut_off
    if (temp.size() < track_length) // acceptable track_length
    {
      UndoFloodFill(_IN, temp, -2);
      count++;
    }
    else {
      sort(temp.begin(), temp.end(), ProcessImage::less_than_xcord); // sorting in x
      _run.push_back(temp);
      ExportImageBlackBG("./output/ImageBeforeAppending.bmp");  // TODO: Make modular
    }
    getMax();
  }

  size_t i = 0;
  size_t j = 0;
  size_t k = 0;
  size_t u = 0;

  cout << "No. of trajectories before search: " << _run.size() << std::endl;
  clog << "No. of Trajectories discarded due to small No. of pixels: " << count << endl;

  /* The following process will (attempt to) append trajectories that are nearby */
  bool isFound;
  for (i = 0; i < _run.size() - 1; i++) {
    isFound = false;
    for (j = i + 1; j < _run.size(); j++) // i should ALWAYS !=j // when it reaches the end and end = true j does not reset but increments
    {
      isFound = false;
      for (k = 0; k < _run[i].size(); k++)  // index of temp1
      {
        for (u = 0; u < _run[j].size(); u++)  // index of _data
        {
          if ((abs(_run[i][k].xcord - _run[j][u].xcord) < area_sensitivity) && (abs(_run[i][k].ycord - _run[j][u].ycord) < area_sensitivity)) {
            _run[i].insert(_run[i].end(), _run[j].begin(), _run[j].end()); // appends temp to _data
            _run.erase(_run.begin() + (j)); // deletes the trajectory copied
            isFound = true;
          }
          if (isFound) { break; }
        }
        if (isFound) { break; }
      }
    }
  }
  cout << "Trajectories Detected: " << _run.size() << std::endl;
}

void ProcessImage::CompleteAnalysis(size_t track_length, size_t area_sensitivity) {
  DetectAll(track_length, area_sensitivity);
  std::cout << std::endl;
  for (int track = 0; track < _run.size(); track++) {
    std::cout << "Track: " << track + 1 << std::endl;
    LinearLeastSquareFit(_run[track]);
    getFitParameters();
    std::cout << std::endl;
    std::cout << "Total Brightness of the track: " << TotalBrightness(_run[track]) << std::endl;;
    std::cout << "Length of the track: " << TrackLength(_run[track]) << std::endl;
    std::cout << "Brightness per unit Lenght: " << BrightnessPerLength(_run[track]) << std::endl;
    AlphaOrBeta(_run[track]);
    std::cout << "______________________________________________________" << std::endl;
  }
}

// EXPORT DATA
void ProcessImage::ExportImageWhiteBG(const std::string & Export_filename) {
  const size_t MAX_RGB = 255;
  ColorPalette();  // contains the colors used in the image

  count = 0;
  getWidth();
  getHeight();

  size_t k = 0;
  while (k < _run.size()) {
    for (k = 0; k < _run.size(); k++) {
      for (size_t z = 0; z < _run[k].size(); z++) {
        _FinalImage(_run[k][z].xcord, _run[k][z].ycord)->Red = palette[count].R;
        _FinalImage(_run[k][z].xcord, _run[k][z].ycord)->Green = palette[count].G;
        _FinalImage(_run[k][z].xcord, _run[k][z].ycord)->Blue = palette[count].B;
      }
      count++;

      if (count == palette.size() - 1) { count = 0; }
    }
  }

  for (size_t i = 0; i < _width; i++) {
    for (size_t j = 0; j < _height; j++) {
      if (_IN[i][j] == -1) { /*DO NOTHING*/
      }
      else if (_IN[i][j] == -2)  // replacement_invalid
      {
        _FinalImage(i, j)->Red = 0; // makes it black
        _FinalImage(i, j)->Green = 0;
        _FinalImage(i, j)->Blue = 0;
      }
      else {
        _FinalImage(i, j)->Red = MAX_RGB - _IN[i][j];
        _FinalImage(i, j)->Green = MAX_RGB - _IN[i][j];
        _FinalImage(i, j)->Blue = MAX_RGB - _IN[i][j];
      }
    }
  }
  _FinalImage.WriteToFile(Export_filename.c_str());
}

void ProcessImage::ExportImageBlackBG(const std::string & Export_filename) {
  ColorPalette();

  count = 0;
  getWidth();
  getHeight();

  // Choosing a different color for different trajectories
  size_t k = 0;
  while (k < _run.size()) {
    for (k = 0; k < _run.size(); k++) {
      for (size_t z = 0; z < _run[k].size(); z++) {
        _FinalImage(_run[k][z].xcord, _run[k][z].ycord)->Red = palette[count].R;
        _FinalImage(_run[k][z].xcord, _run[k][z].ycord)->Green = palette[count].G;
        _FinalImage(_run[k][z].xcord, _run[k][z].ycord)->Blue = palette[count].B;
      }
      count++;
      if (count == palette.size() - 1) // Looping through the palette
      {
        count = 0;
      }
    }
  }
  for (size_t i = 0; i < _width; i++) {
    for (size_t j = 0; j < _height; j++) {
      if (_IN[i][j] == -1) { /*DO NOTHING*/ }

      else if (_IN[i][j] == -2)  // replacement_invalid
      {
        _FinalImage(i, j)->Red = 0;
        _FinalImage(i, j)->Blue = 0;
        _FinalImage(i, j)->Green = 0;
      }
      else {
        _FinalImage(i, j)->Red = _IN[i][j];
        _FinalImage(i, j)->Blue = _IN[i][j];
        _FinalImage(i, j)->Green = _IN[i][j];
      }
    }
  }
  _FinalImage.WriteToFile(Export_filename.c_str());
}

void ProcessImage::SimpleImageExport(const std::string & Export_filename) {
  for (size_t i = 0; i < 1920; i++) {
    for (size_t j = 0; j < 1080; j++) {
      if (_IN[i][j] == -1) {
        _FinalImage(i, j)->Red = 0;
        _FinalImage(i, j)->Blue = 255;
        _FinalImage(i, j)->Green = 255;
      }
      else if (_IN[i][j] == -2)  // replacement_invalid
      {
        _FinalImage(i, j)->Red = 0;
        _FinalImage(i, j)->Blue = 0;
        _FinalImage(i, j)->Green = 255;
      }
      else {
        _FinalImage(i, j)->Red = _IN[i][j];
        _FinalImage(i, j)->Blue = _IN[i][j];
        _FinalImage(i, j)->Green = _IN[i][j];
      }
    }
  }
  _FinalImage.WriteToFile(Export_filename.c_str());
}

// OPERATORS
bool ProcessImage::less_than_xcord(const node & lhs, const node & rhs) {
  return (lhs.xcord < rhs.xcord);
}
bool ProcessImage::less_than_ycord(const node & lhs, const node & rhs) {
  return (lhs.ycord < rhs.ycord);
}

// RETURN VALUES
ProcessImage::Statistics ProcessImage::getBGStats(const char* name) {
  /*
  * A method stricly used to be called by the user to Display the statistical data
  * of the Background Image.
  * Combine with std::cout << getMean(), getSdiv(), getSampleMean() and getSampleSdiv()
  * to display the stat data of the BG.
  */
  if (name == "full") {
    FullStatistics(_BG, "include0");
  }
  else if (name == "sample") {
    SampleStatistics(_BG);
  }
  else if (name == "both") {
    FullStatistics(_BG, "include0");
    SampleStatistics(_BG);
  }
  else {
    //TODO: throw exception
  }
  return stat;
}
float ProcessImage::getSampleMean() {
  return stat._sMean;
}
float ProcessImage::getSampleSdiv() {
  return stat._sSdiv;
}
float ProcessImage::getMean() {
  return stat._mean;
}
float ProcessImage::getSdiv() {
  return stat._sdiv;
}
int ProcessImage::getMax() {

  max = 0;
  for (size_t i = 0; i < _IN.size(); i++) {
    for (size_t j = 0; j < _IN[0].size(); j++) {
      if (max < _IN[i][j]) // O(n^2) complexity, Improvement necessary
      {    // 2D vector spliting:emptyV.push_back(v[i][j]->max->find(max) could be more efficient
        max = _IN[i][j];
        x = i;
        y = j;
      }
    }
  }

  maxX = x;
  maxY = y;
  return max;
}
size_t ProcessImage::getWidth() {
  return _width = _BG.size();
}
size_t ProcessImage::getHeight() {
  return _height = _BG[0].size();
}
void ProcessImage::getFitParameters() {
  using namespace std;
  cout << "Linear Least Square Fit of the form: y = b*x + a" << endl;
  cout << "Fit parameter b: " << fit_parameters._b << " with Error SE(b): " << fit_parameters.SEb << endl;
  cout << "Fit parameter a: " << fit_parameters._a << " with Error SE(a): " << fit_parameters.SEa << endl;
  cout << "Correlation coefficient r^2: " << fit_parameters._r_sqrd << endl;
}

// OTHER METHODS 
void ProcessImage::ColorPalette() {
  Color RED(255, 0, 0), GREEN(0, 255, 0), BLUE(0, 0, 255), BLACK(0, 0, 0),
    WHITE(255, 255, 255), CYAN(0, 255, 255),
    YELLOW(255, 255, 0), MAGENTA(255, 0, 255),
    PURPLE(153, 51, 255), ORANGE(255, 128, 0), DARK_GREEN(0, 100, 0),
    DEEP_PURPLE(76, 0, 153), PINK_LIGHT(255, 153, 255),
    BROWN(102, 51, 0), BLUE_LIGHT(0, 128, 255), DARK_VIOLET(148, 0, 211),
    AZURE(131, 139, 139), TURQUOISE(0, 134, 139),
    SALMON(255, 140, 105), DARK_GREY(169, 169, 169),
    SEPIA(94, 38, 18), BISQUE(238, 213, 183);

  palette.push_back(RED); palette.push_back(GREEN);
  palette.push_back(BLUE); palette.push_back(CYAN);
  palette.push_back(YELLOW); palette.push_back(MAGENTA);
  palette.push_back(PURPLE); palette.push_back(ORANGE);
  palette.push_back(DARK_GREEN); palette.push_back(DEEP_PURPLE);
  palette.push_back(PINK_LIGHT); palette.push_back(BROWN);
  palette.push_back(BLUE_LIGHT); palette.push_back(DARK_VIOLET);
  palette.push_back(AZURE); palette.push_back(TURQUOISE);
  palette.push_back(SALMON); palette.push_back(DARK_GREY);
  palette.push_back(SEPIA); palette.push_back(BISQUE);
}