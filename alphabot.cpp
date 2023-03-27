#include "RequestProcessing.h"
#include "ImageProcessing.h"

using namespace std;
using namespace cv;

void SetColor(Mat frame)
{
    namedWindow("Result");
    namedWindow("Settings");
    createTrackbar("h1", "Settings", 0, 255);
    createTrackbar("s1", "Settings", 0, 255);
    createTrackbar("v1", "Settings", 0, 255);
    createTrackbar("h2", "Settings", 0, 255);
    createTrackbar("s2", "Settings", 0, 255);
    createTrackbar("v2", "Settings", 0, 255);
    while(true)
    {
        Mat frameHsv;
        cvtColor(frame, frameHsv, COLOR_BGR2HSV);
        int h1 = getTrackbarPos("h1", "Settings");
        int s1 = getTrackbarPos("s1", "Settings");
        int v1 = getTrackbarPos("v1", "Settings");
        int h2 = getTrackbarPos("h2", "Settings");
        int s2 = getTrackbarPos("s2", "Settings");
        int v2 = getTrackbarPos("v2", "Settings");
        vector <int> HsvMin = {h1, s1, v1};
        vector <int> HsvMax = {h2, s2, v2};
        Mat thresh;
        inRange(frameHsv, HsvMin, HsvMax, thresh);
        imshow("Result", thresh);

        char ch = waitKey(1000);
        if (ch == 27)
            break;
    }
    destroyAllWindows;
}

int main()
{
    Request request;
    Controller controller;
    cv::VideoCapture Cap("/dev/video0");

    controller.FiniteAutomate(request, Cap);

    return 0;
}
