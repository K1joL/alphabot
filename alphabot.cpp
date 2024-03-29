#include "RequestProcessing.h"
#include "ImageProcessing.h"


using namespace std;
using namespace cv;

void SetColor(Mat &frame, VideoCapture cap)
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
        cap >> frame;
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

        char ch = waitKey(500);
        if (ch == 27)
            break;
    }
    destroyAllWindows();
}

int main()
{

    Controller controller;
    cv::VideoCapture Cap("/dev/video0");

    char c;
    cout << "Enter \'s\' to set color of bot : " << endl;
    cin >> c;
    if(c == 's')
    {
        Mat frame;
        SetColor(frame, Cap);
    }else
         controller.FiniteAutomate(Cap);

    return 0;
}
