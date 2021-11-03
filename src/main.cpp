//#include "RunImageAnalysis.h"
#include <ctime>

#include "ProcessImage.h"

int main() {
  srand(time(NULL));
  // image with trajectories
  std::string input_object = "../images/scene00044.bmp";
  // background image
  std::string bg_object = "../images/4bg.bmp";
  ImportImage track(bg_object, input_object);
  ProcessImage process(track);
  process.CompleteAnalysis(100, 100);

  std::string black = "./output/BlackBG.bmp";
  std::string white = "./output/WhiteBG.bmp";

  process.ExportImageBlackBG(black);
  process.ExportImageWhiteBG(white);
}