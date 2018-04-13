//#include "RunImageAnalysis.h"
#include "ProcessImage.h"
#include <ctime>


int main() {
  srand(time(NULL));
  std::string input_object = "test4.bmp";
  std::string bg_object = "bg4.bmp";
  ImportImage track(bg_object, input_object);
  ProcessImage process(track);
  process.CompleteAnalysis(100, 20);


  std::string black = "BlackBG.bmp";
  std::string white = "WhiteBG.bmp";

  /*black = input_object + black;
  white = input_object + white;*/

  process.ExportImageBlackBG(black);
  process.ExportImageWhiteBG(white);

  system("pause");
}