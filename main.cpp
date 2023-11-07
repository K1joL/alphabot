#include <iostream>
#include "detector.h"
#include "controller.h"

using namespace cv;
using namespace std;

int main()
{
    Controller ctrlr("localhost");
    VideoCapture capture("/dev/video0");
    
    ctrlr.ProcessEvents(capture);

    return 0;
}