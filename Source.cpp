//#include "RunImageAnalysis.h"
#include "ProcessImage.h"
#include <ctime>


int main() {
  srand(time(NULL));
  std::string input_object = "./images/scene00044.bmp";  // image with trajectories
  std::string bg_object = "./images/4bg.bmp";  // background image
  ImportImage track(bg_object, input_object);
  ProcessImage process(track);
  process.CompleteAnalysis(100, 100);


  std::string black = "./output/BlackBG.bmp";
  std::string white = "./output/WhiteBG.bmp";

  /*black = input_object + black;
  white = input_object + white;*/

  process.ExportImageBlackBG(black);
  process.ExportImageWhiteBG(white);

  system("pause");
}