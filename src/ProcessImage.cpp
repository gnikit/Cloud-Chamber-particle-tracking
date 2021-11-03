#include "ProcessImage.h"

ProcessImage::ProcessImage() {}
ProcessImage::ProcessImage(ImportImage& Input) {
  _BG = Input.PassBgPixelsVector();
  _IN = Input.PassInPixelsVector();

  _WIDTH = _IN.size();
  _HEIGHT = _IN[0].size();

  // Initialise the Image
  _FinalImage.SetSize(_WIDTH, _HEIGHT);
  _FinalImage.SetBitDepth(32);

  _in_fname = Input._in_fname;
  _bg_fname = Input._bg_fname;
  _in_fname_base = Input._in_fname_base;
  _bg_fname_base = Input._bg_fname_base;

  count = 0;
}

ProcessImage::~ProcessImage() {}

// STATISTICAL ANALYSIS

void ProcessImage::CalculateStats(vec2d& v, const bool remove_zero) {
  getMean(v, remove_zero);
  getSdiv(v, remove_zero);
}

void ProcessImage::FloodFill(int X, int Y, double cut_off, int invalid,
                             vec2d& v, std::vector<node>& data) {
  using namespace std;
  getWidth();
  getHeight();
  if (cut_off == invalid) {
    return;
  }
  if (v[X][Y] < cut_off) {
    return;
  }

  queue<node> Q;
  Q.push({v[X][Y], X, Y});
  data.push_back({v[X][Y], X, Y});
  node n(0, 0, 0);

  while (!Q.empty()) {
    n = Q.front();
    Q.pop();
    if ((n.xcord < (int)_WIDTH - 1) && (v[n.xcord + 1][n.ycord] > cut_off)) {
      Q.push({v[n.xcord + 1][n.ycord], n.xcord + 1, n.ycord});
      data.push_back({v[n.xcord + 1][n.ycord], n.xcord + 1, n.ycord});
      v[n.xcord + 1][n.ycord] = invalid;
    }
    if ((n.xcord > 0) && (v[n.xcord - 1][n.ycord] > cut_off)) {
      Q.push({v[n.xcord - 1][n.ycord], n.xcord - 1, n.ycord});
      data.push_back({v[n.xcord - 1][n.ycord], n.xcord - 1, n.ycord});
      v[n.xcord - 1][n.ycord] = invalid;
    }
    if ((n.ycord < (int)_HEIGHT - 1) && (v[n.xcord][n.ycord + 1] > cut_off)) {
      Q.push({v[n.xcord][n.ycord + 1], n.xcord, n.ycord + 1});
      data.push_back({v[n.xcord][n.ycord + 1], n.xcord, n.ycord + 1});
      v[n.xcord][n.ycord + 1] = invalid;
    }
    if ((n.ycord > 0) && (v[n.xcord][n.ycord - 1] > cut_off)) {
      Q.push({v[n.xcord][n.ycord - 1], n.xcord, n.ycord - 1});
      data.push_back({v[n.xcord][n.ycord - 1], n.xcord, n.ycord - 1});
      v[n.xcord][n.ycord - 1] = invalid;
    }
    v[n.xcord][n.ycord] = invalid;
  }
}

void ProcessImage::FloodFillRecursive(size_t X, size_t Y, double cut_off,
                                      vec2d& v) {
  int invalid = -1;
  getWidth();
  getHeight();
  if (v[X][Y] > (cut_off)) {
    if (X > 0) {
      v[X][Y] = invalid;
      FloodFillRecursive(X - 1, Y, cut_off, v);
    }
    if (X < _WIDTH - 1) {
      v[X][Y] = invalid;
      FloodFillRecursive(X + 1, Y, cut_off, v);
    }
    if (Y > 0) {
      v[X][Y] = invalid;
      FloodFillRecursive(X, Y - 1, cut_off, v);
    }
    if (Y < _HEIGHT - 1) {
      v[X][Y] = invalid;
      FloodFillRecursive(X, Y + 1, cut_off, v);
    }
    v[X][Y] = invalid;
  }
}

void ProcessImage::EightWay(size_t X, size_t Y, double cut_off, vec2d& v) {
  const int invalid = -1;

  getWidth();
  getHeight();
  if (v[X][Y] > (cut_off)) {
    if (X > 0) {
      v[X][Y] = invalid;
      EightWay(X - 1, Y, cut_off, v);
    }
    if (X < _WIDTH - 1) {
      v[X][Y] = invalid;
      EightWay(X + 1, Y, cut_off, v);
    }
    if (Y > 0) {
      v[X][Y] = invalid;
      EightWay(X, Y - 1, cut_off, v);
    }
    if (Y < _HEIGHT - 1) {
      v[X][Y] = invalid;
      EightWay(X, Y + 1, cut_off, v);
    }
    if ((X > 0) && (Y < _HEIGHT - 1)) {
      v[X][Y] = invalid;
      EightWay(X - 1, Y + 1, cut_off, v);
    }
    if ((X < _WIDTH - 1) && (Y > 0)) {
      v[X][Y] = invalid;
      EightWay(X + 1, Y - 1, cut_off, v);
    }
    if ((X < _WIDTH - 1) && (Y < _HEIGHT - 1)) {
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

void ProcessImage::UndoFloodFill(vec2d& v, std::vector<node>& data,
                                 int invalid_replacement) {
  for (size_t i = 0; i < data.size(); i++) {
    v[data[i].xcord][data[i].ycord] = invalid_replacement;
  }
}

//_____________________________ DATA ANALYSIS ________________________________//

void ProcessImage::Threshold(vec2d& v, const double cut_off) {
  for (size_t i = 0; i < v.size(); i++) {
    for (size_t j = 0; j < v[i].size(); j++) {
      if (v[i][j] < cut_off) v[i][j] = 0;
    }
  }
}

void ProcessImage::ConvertBlackWhite(vec2d& v, const double cut_off) {
  for (size_t i = 0; i < v.size(); i++) {
    for (size_t j = 0; j < v[0].size(); j++) {
      if (v[i][j] < cut_off)
        v[i][j] = 0;
      else
        v[i][j] = MAX_RGB;
    }
  }
}

void ProcessImage::SmoothImage(vec2d& v, const int patch_sz) {
  vec2d av(v);
  for (size_t i = patch_sz; i < v.size() - patch_sz; i++) {
    for (size_t j = patch_sz; j < v[i].size() - patch_sz; j++) {
      int tmp = 0;
      for (int dx = -patch_sz; dx < patch_sz + 1; dx++) {
        for (int dy = -patch_sz; dy < patch_sz + 1; dy++) {
          tmp += v[i + dx][j + dy];
        }
      }
      av[i][j] = (int)tmp / pow(patch_sz * 2 + 1, 2);
    }
  }
  v = av;
}

double ProcessImage::TotalBrightness(std::vector<node>& data) {
  track._track_intensity = 0;
  for (size_t i = 0; i < data.size(); i++) {
    track._track_intensity += data[i].intensity;
  }
  return track._track_intensity;
}

double ProcessImage::TrackLength(std::vector<node>& data) {
  using namespace std;
  long double b = fit_parameters._b;
  long double a = fit_parameters._a;

  vector<node> projection(data);
  int x_proj;
  int y_proj;
  for (size_t i = 0; i < data.size(); i++) {
    y_proj = (b * data[i].xcord + b * b * data[i].ycord + a) / (b * b + 1);
    projection[i].ycord = y_proj;

    x_proj = (data[i].xcord + b * data[i].ycord - b * a) / (b * b + 1);
    projection[i].xcord = x_proj;
    projection[i].intensity = 0;
  }
  sort(projection.begin(), projection.end(), ProcessImage::less_than_xcord);

  int N = projection.size() - 1;
  long double Dx = projection[0].xcord - projection[N].xcord;
  long double Dy = projection[0].ycord - projection[N].ycord;
  track._track_length = sqrt(Dx * Dx + Dy * Dy);

  return track._track_length;
}

double ProcessImage::BrightnessPerLength(std::vector<node>& data) {
  track._track_intensity_per_length =
      track._track_intensity / track._track_length;
  return track._track_intensity_per_length;
}

void ProcessImage::AlphaOrBeta(std::vector<node>& data) {
  using namespace std;

  double threshold = 750;  // determined by examining individual images of
                           // alphas
  if (track._track_intensity_per_length >= threshold) {
    cout << "The trajectory analysed is from an Alpha particle." << endl;
  }
  // else if (400 < track._track_intensity_per_length && threshold >
  // track._track_intensity_per_length)
  //{
  //	cout << "The particle cannot be determined. Do futher fitting analysis
  // to establish the kind" << endl;
  // }
  else {
    cout << "The trajectory analysed is from an Beta particle." << endl;
  }
}

LSF ProcessImage::LinearLeastSquareFit(std::vector<node>& data) {
  long double SSxx = 0, SSyy = 0, SSxy = 0;
  long double x_mean = 0, y_mean = 0;
  size_t n = 0;
  for (size_t i = 0; i < data.size(); i++) {
    SSxx += (data[i].xcord * data[i].xcord);
    SSyy += (data[i].ycord * data[i].ycord);
    SSxy += (data[i].xcord * data[i].ycord);

    x_mean += data[i].xcord;
    y_mean += data[i].ycord;
    n++;
  }
  x_mean /= n;
  y_mean /= n;

  SSxx = SSxx - n * x_mean * x_mean;
  SSyy = SSyy - n * y_mean * y_mean;
  SSxy = SSxy - n * x_mean * y_mean;

  fit_parameters._b = SSxy / SSxx;
  fit_parameters._a = y_mean - fit_parameters._b * x_mean;
  fit_parameters._r_sqrd = (SSxy * SSxy) / (SSxx * SSyy);

  long double s = sqrt((SSyy - fit_parameters._b * SSxy) / (n - 2));

  fit_parameters.SEa = s * sqrt(1 / n + (x_mean * x_mean) / (SSxx));
  fit_parameters.SEb = s * sqrt(1 / SSxx);

  return fit_parameters;
}

//_____________________________ RUN DATA _____________________________________//

void ProcessImage::SimpleProcessRun() {
  using namespace std;

  // Average intensities of input and background
  for (size_t l = 0; l < 5; l++) {
    SmoothImage(_IN, 3);
    SmoothImage(_BG, 3);
  }
  // Subtract background from input
  for (size_t i = 0; i < _IN.size(); i++) {
    for (size_t j = 0; j < _IN[i].size(); j++) {
      int val = _IN[i][j] - _BG[i][j];
      if (val < 0) val = 0;
      _IN[i][j] = val;
    }
  }

  getMax();
  CalculateStats(_BG);  // used to determine cut_off
  _cut_off = (stat.mean + 1 * stat.sdiv);

  Threshold(_IN, _cut_off);
  CalculateStats(_IN, true);
  //_cut_off = stat._mean + 1 * stat._sdiv;
  int _invalid = -1;

  FloodFill(maxX, maxY, _cut_off, _invalid, _IN, _data);
  LinearLeastSquareFit(_data);  // Runs a LST fit for the particle
  getFitParameters();           // Prints fit parameters for the tracks
  getFitParameters();
  cout << endl;
  cout << "Total Brightness of the track: " << TotalBrightness(_data) << endl;
  ;
  cout << "Length of the track: " << TrackLength(_data) << endl;
  cout << "Brightness per unit Length: " << BrightnessPerLength(_data) << endl;
  AlphaOrBeta(_data);
  cout << "______________________________________________________" << endl;
}

void ProcessImage::DetectAll(size_t track_length, size_t area_sensitivity) {
  using namespace std;

  // These numbers are hardwired. They are reasonable values
  // but they should really be input parameters e.g.
  // smoothing_cycles and fuzz
  // Average intensities of input and background
  for (size_t l = 0; l < 5; l++) {
    SmoothImage(_IN, 3);
    SmoothImage(_BG, 3);
  }
  // Subtract background from input
  for (size_t i = 0; i < _IN.size(); i++) {
    for (size_t j = 0; j < _IN[i].size(); j++) {
      int val = _IN[i][j] - _BG[i][j];
      if (val < 0) val = 0;
      _IN[i][j] = val;
    }
  }

  CalculateStats(_IN);  // OR use this
  _cut_off = (stat.mean + 1 * stat.sdiv);

  cout << "Using a cut off of: " << _cut_off << " for the Binary flush process"
       << endl;

  Threshold(_IN, _cut_off);  // <- small cut_off yields better results
  getMax();

  _cut_off *= 2;
  count = 0;
  int _invalid = -1;
  while (max > _cut_off)  // max brightness sensitivity
  {
    vector<node> temp;
    // using original cut_off
    FloodFill(maxX, maxY, _cut_off / 2, _invalid, _IN, temp);
    if (temp.size() < track_length)  // acceptable track_length
    {
      UndoFloodFill(_IN, temp, -2);
      count++;
    } else {
      // sorting in x
      sort(temp.begin(), temp.end(), ProcessImage::less_than_xcord);
      _run.push_back(temp);
      ExportImageBlackBG(_in_fname_base + "ImageBeforeTrackAppending.bmp");
    }
    getMax();
  }

  size_t i = 0;
  size_t j = 0;
  size_t k = 0;
  size_t u = 0;

  cout << "No. of trajectories before search: " << _run.size() << std::endl;
  clog << "No. of Trajectories discarded due to small No. of pixels: " << count
       << endl;

  /* The following process will (attempt to) append trajectories that are nearby
   */
  bool isFound;
  for (i = 0; i < _run.size() - 1; i++) {
    isFound = false;
    for (j = i + 1; j < _run.size();
         j++)  // i should ALWAYS !=j // when it reaches the end and end = true
               // j does not reset but increments
    {
      isFound = false;
      for (k = 0; k < _run[i].size(); k++)  // index of temp1
      {
        for (u = 0; u < _run[j].size(); u++)  // index of _data
        {
          if ((abs(_run[i][k].xcord - _run[j][u].xcord) <
               (int)area_sensitivity) &&
              (abs(_run[i][k].ycord - _run[j][u].ycord) <
               (int)area_sensitivity)) {
            _run[i].insert(_run[i].end(), _run[j].begin(),
                           _run[j].end());   // appends temp to _data
            _run.erase(_run.begin() + (j));  // deletes the trajectory copied
            isFound = true;
          }
          if (isFound) {
            break;
          }
        }
        if (isFound) {
          break;
        }
      }
    }
  }
  cout << "Trajectories Detected: " << _run.size() << std::endl;
}

void ProcessImage::CompleteAnalysis(size_t track_length,
                                    size_t area_sensitivity) {
  DetectAll(track_length, area_sensitivity);
  std::cout << std::endl;
  for (size_t track = 0; track < _run.size(); track++) {
    std::cout << "Track: " << track + 1 << std::endl;
    LinearLeastSquareFit(_run[track]);
    getFitParameters();
    std::cout << std::endl;
    std::cout << "Total Brightness of the track: "
              << TotalBrightness(_run[track]) << std::endl;
    ;
    std::cout << "Length of the track: " << TrackLength(_run[track])
              << std::endl;
    std::cout << "Brightness per unit Lenght: "
              << BrightnessPerLength(_run[track]) << std::endl;
    AlphaOrBeta(_run[track]);
    std::cout << "______________________________________________________"
              << std::endl;
  }
}

// EXPORT DATA
void ProcessImage::ExportImageWhiteBG(const std::string& fname) {
  ColorPalette();  // contains the colors used in the image

  count = 0;
  getWidth();
  getHeight();

  size_t k = 0;
  while (k < _run.size()) {
    for (k = 0; k < _run.size(); k++) {
      for (size_t z = 0; z < _run[k].size(); z++) {
        _FinalImage(_run[k][z].xcord, _run[k][z].ycord)->Red = palette[count].R;
        _FinalImage(_run[k][z].xcord, _run[k][z].ycord)->Green =
            palette[count].G;
        _FinalImage(_run[k][z].xcord, _run[k][z].ycord)->Blue =
            palette[count].B;
      }
      count++;

      if (count == palette.size() - 1) {
        count = 0;
      }
    }
  }

  for (size_t i = 0; i < _WIDTH; i++) {
    for (size_t j = 0; j < _HEIGHT; j++) {
      if (_IN[i][j] == -1) {       /*DO NOTHING*/
      } else if (_IN[i][j] == -2)  // replacement_invalid
      {
        _FinalImage(i, j)->Red = 0;  // makes it black
        _FinalImage(i, j)->Green = 0;
        _FinalImage(i, j)->Blue = 0;
      } else {
        _FinalImage(i, j)->Red = MAX_RGB - _IN[i][j];
        _FinalImage(i, j)->Green = MAX_RGB - _IN[i][j];
        _FinalImage(i, j)->Blue = MAX_RGB - _IN[i][j];
      }
    }
  }
  _FinalImage.WriteToFile(fname.c_str());
}

void ProcessImage::ExportImageBlackBG(const std::string& fname) {
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
        _FinalImage(_run[k][z].xcord, _run[k][z].ycord)->Green =
            palette[count].G;
        _FinalImage(_run[k][z].xcord, _run[k][z].ycord)->Blue =
            palette[count].B;
      }
      count++;
      if (count == palette.size() - 1)  // Looping through the palette
      {
        count = 0;
      }
    }
  }
  for (size_t i = 0; i < _WIDTH; i++) {
    for (size_t j = 0; j < _HEIGHT; j++) {
      if (_IN[i][j] == -1) { /*DO NOTHING*/
      }

      else if (_IN[i][j] == -2)  // replacement_invalid
      {
        _FinalImage(i, j)->Red = 0;
        _FinalImage(i, j)->Blue = 0;
        _FinalImage(i, j)->Green = 0;
      } else {
        _FinalImage(i, j)->Red = _IN[i][j];
        _FinalImage(i, j)->Blue = _IN[i][j];
        _FinalImage(i, j)->Green = _IN[i][j];
      }
    }
  }
  _FinalImage.WriteToFile(fname.c_str());
}

void ProcessImage::SimpleImageExport(const std::string& fname) {
  for (size_t i = 0; i < 1920; i++) {
    for (size_t j = 0; j < 1080; j++) {
      if (_IN[i][j] == -1) {
        _FinalImage(i, j)->Red = 0;
        _FinalImage(i, j)->Blue = 255;
        _FinalImage(i, j)->Green = 255;
      } else if (_IN[i][j] == -2)  // replacement_invalid
      {
        _FinalImage(i, j)->Red = 0;
        _FinalImage(i, j)->Blue = 0;
        _FinalImage(i, j)->Green = 255;
      } else {
        _FinalImage(i, j)->Red = _IN[i][j];
        _FinalImage(i, j)->Blue = _IN[i][j];
        _FinalImage(i, j)->Green = _IN[i][j];
      }
    }
  }
  _FinalImage.WriteToFile(fname.c_str());
}

// OPERATORS
bool ProcessImage::less_than_xcord(const node& lhs, const node& rhs) {
  return (lhs.xcord < rhs.xcord);
}
bool ProcessImage::less_than_ycord(const node& lhs, const node& rhs) {
  return (lhs.ycord < rhs.ycord);
}

// RETURN VALUES

double ProcessImage::getMean(vec2d& v, const bool remove_zero) {
  stat.mean = 0;
  size_t c = 0;

  for (size_t i = 0; i < v.size(); ++i) {
    for (size_t j = 0; j < v[i].size(); ++j) {
      if (remove_zero) {
        if (v[i][j] > 0) {
          stat.mean += v[i][j];
          ++c;
        }
      } else {
        if (v[i][j] >= 0) {
          stat.mean += v[i][j];
          ++c;
        }
      }
    }
  }
  stat.mean /= c;

  return stat.mean;
}

double ProcessImage::getSdiv(vec2d& v, const bool remove_zero) {
  stat.sdiv = 0;
  size_t c = 0;

  for (size_t i = 0; i < v.size(); ++i) {
    for (size_t j = 0; j < v[i].size(); ++j) {
      if (remove_zero) {
        if (v[i][j] > 0) {
          stat.sdiv += pow((v[i][j] - stat.mean), 2);
          ++c;
        }
      } else {
        if (v[i][j] >= 0) {
          stat.sdiv += pow((v[i][j] - stat.mean), 2);
          ++c;
        }
      }
    }
  }
  stat.sdiv /= c;
  stat.sdiv = sqrt(stat.sdiv);

  return stat.sdiv;
}

int ProcessImage::getMax() {
  max = 0;
  for (size_t i = 0; i < _IN.size(); i++) {
    for (size_t j = 0; j < _IN[i].size(); j++) {
      // O(n^2) complexity, Improvement necessary
      // 2D vector spliting:emptyV.push_back(v[i][j]->max->find(max) could be
      // more efficient
      if (max < _IN[i][j]) {
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

size_t ProcessImage::getWidth() { return _WIDTH = _BG.size(); }

size_t ProcessImage::getHeight() { return _HEIGHT = _BG[0].size(); }

void ProcessImage::getFitParameters() {
  using namespace std;
  cout << "Linear Least Square Fit of the form: y = b*x + a" << endl;
  cout << "Fit parameter b: " << fit_parameters._b
       << " with Error SE(b): " << fit_parameters.SEb << endl;
  cout << "Fit parameter a: " << fit_parameters._a
       << " with Error SE(a): " << fit_parameters.SEa << endl;
  cout << "Correlation coefficient r^2: " << fit_parameters._r_sqrd << endl;
}

// OTHER METHODS

void ProcessImage::ColorPalette() {
  Color RED(255, 0, 0), GREEN(0, 255, 0), BLUE(0, 0, 255), BLACK(0, 0, 0),
      WHITE(255, 255, 255), CYAN(0, 255, 255), YELLOW(255, 255, 0),
      MAGENTA(255, 0, 255), PURPLE(153, 51, 255), ORANGE(255, 128, 0),
      DARK_GREEN(0, 100, 0), DEEP_PURPLE(76, 0, 153), PINK_LIGHT(255, 153, 255),
      BROWN(102, 51, 0), BLUE_LIGHT(0, 128, 255), DARK_VIOLET(148, 0, 211),
      AZURE(131, 139, 139), TURQUOISE(0, 134, 139), SALMON(255, 140, 105),
      DARK_GREY(169, 169, 169), SEPIA(94, 38, 18), BISQUE(238, 213, 183);

  palette.push_back(RED);
  palette.push_back(GREEN);
  palette.push_back(BLUE);
  palette.push_back(CYAN);
  palette.push_back(YELLOW);
  palette.push_back(MAGENTA);
  palette.push_back(PURPLE);
  palette.push_back(ORANGE);
  palette.push_back(DARK_GREEN);
  palette.push_back(DEEP_PURPLE);
  palette.push_back(PINK_LIGHT);
  palette.push_back(BROWN);
  palette.push_back(BLUE_LIGHT);
  palette.push_back(DARK_VIOLET);
  palette.push_back(AZURE);
  palette.push_back(TURQUOISE);
  palette.push_back(SALMON);
  palette.push_back(DARK_GREY);
  palette.push_back(SEPIA);
  palette.push_back(BISQUE);
}