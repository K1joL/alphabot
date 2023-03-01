#include "opencv2/opencv.hpp"
#include <iostream>

using namespace std;
using namespace cv;

struct Color
{
    uint8_t Hue = 0;
    uint8_t Saturation = 0;
    uint8_t Value = 0;
    Color(uint8_t H, uint8_t S, uint8_t V)
    {
        Hue = H;
        Saturation = S;
        Value = V;
    }

}BlueHSV(100,90,70), MagentaHSV(167, 80, 90);

class MassCenter
{
    public:
        int X;
        int Y;
        MassCenter(Rect Rectangle);
        MassCenter(MassCenter Center1, MassCenter Center2);
};

class MovementCalculation
{
    public:
        float AngleInRadian;
        int DistanceInPixel;
        void findAngle(MassCenter blob1, MassCenter blob2, Point2i Destination);
        void findDistanceToDestination(MassCenter AverageCenter, Point2i Destination);

};

MassCenter::MassCenter(Rect Rectangle)
{
    Point2i TopLeftPointRect = Rectangle.tl();
    Point2i BottomRightPointRect = Rectangle.br();
    this->X = (TopLeftPointRect.x + BottomRightPointRect.x)/2;
    this->Y = (TopLeftPointRect.y + BottomRightPointRect.y)/2;
}

MassCenter::MassCenter(MassCenter Center1, MassCenter Center2)
{
    this->X = (Center1.X + Center2.X)/2;
    this->Y = (Center1.Y + Center2.Y)/2;
}

void MovementCalculation::findAngle(MassCenter blob1, MassCenter blob2, Point2i Dest)
{
    int CenterX = int(blob1.X + blob2.X)/2;
    int CenterY = int(blob1.Y + blob2.Y)/2;
    this -> AngleInRadian = ((blob1.X - CenterX) * (CenterX - Dest.x) + (blob1.Y - CenterY) * (CenterY - Dest.y))/
                            (sqrt(pow((blob1.X - CenterX), 2) + pow((blob1.Y - CenterY), 2)) * sqrt(pow((CenterX - Dest.x), 2) + pow((CenterY - Dest.y), 2)));
}

void MovementCalculation::findDistanceToDestination(MassCenter AverageCenter, Point2i Dest)
{
    this -> DistanceInPixel = (AverageCenter.X - Dest.x) - (AverageCenter.Y - Dest.y);
}

void SetColor(Mat Frame)
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
        Mat FrameHsv;
        cvtColor(Frame, FrameHsv, COLOR_BGR2HSV);
        int h1 = getTrackbarPos("h1", "Settings");
        int s1 = getTrackbarPos("s1", "Settings");
        int v1 = getTrackbarPos("v1", "Settings");
        int h2 = getTrackbarPos("h2", "Settings");
        int s2 = getTrackbarPos("s2", "Settings");
        int v2 = getTrackbarPos("v2", "Settings");
        vector <int> HsvMin = {h1, s1, v1};
        vector <int> HsvMax = {h2, s2, v2};
        Mat thresh;
        inRange(FrameHsv, HsvMin, HsvMax, thresh);
        imshow("Result", thresh);

        char ch = waitKey(1000);
        if (ch == 27)
            break;
    }
    destroyAllWindows;
}

Mat TakeThresholdOfBlob(Mat FrameHSV, Color Color)
{
    Mat Threshold;
    //vectors of HSV color that need to find. Make a range using Offset
    uint8_t Offset = 5;
    vector <uint8_t> HsvMin = {static_cast<uint8_t>(Color.Hue - Offset),100,100};
    vector <uint8_t> HsvMax = {static_cast<uint8_t>(Color.Hue + Offset),255,255};
    
    //Make result image matrix with found color
    inRange(FrameHSV, HsvMin, HsvMax, Threshold);
    //Creating the image in white&black with area of needed color 
    // imwrite( "FrameResult.jpg", Threshold );
    return Threshold;
}

Rect detectBlob(Mat Threshold)
{
    Rect Rectangle;
    vector<vector<Point>> Contours;
    vector<Vec4i> Hierarchy;

    findContours(Threshold, Contours, RETR_TREE, CHAIN_APPROX_SIMPLE);
    
    vector<RotatedRect> minRect(Contours.size());
    for(size_t i = 0; i < Contours.size(); i++)
            minRect[i] = minAreaRect(Contours[i]);
    
    for(size_t i = 0; i < Contours.size(); i++)
    {
        int area = minRect[i].size.height*minRect[i].size.width;
        if(area > 10'000)
        {
            Rectangle = minRect[i].boundingRect();
            //Uncomment if you want to see Rectangles on image
            // Point2f rect_points[4];
            // minRect[i].points( rect_points );
            // for ( int j = 0; j < 4; j++ )
            // {
            //     line( Threshold, rect_points[j], rect_points[(j+1)%4], (0,0,255) );
            // }
        }
    }
    // imshow("Result", Threshold);
    // waitKey();
    return Rectangle;
}

int main()
{
    Mat Frame = imread("img_0.jpg");
    Mat FrameHSV;
    //converting the Frame to FrameHsv color model
    cvtColor(Frame, FrameHSV, COLOR_BGR2HSV);
    Point2i Destination(960,1280);
    Mat Threshold = TakeThresholdOfBlob(FrameHSV, MagentaHSV);
    Rect RectangleOfMagenta = detectBlob(Threshold);
    //just for tests
    // cout << RectangleOfMagenta << endl;

    Threshold = TakeThresholdOfBlob(FrameHSV,BlueHSV);
    Rect RectangleOfBlue = detectBlob(Threshold);
    //just for tests
    // cout << RectangleOfBlue << endl;

    MassCenter MCBlue(RectangleOfBlue);
    MassCenter MCMagenta(RectangleOfMagenta);
    MassCenter MCAverage(MCBlue, MCMagenta);    
    MovementCalculation MoveBot;
    MoveBot.findAngle(MCBlue, MCMagenta, Destination);
    MoveBot.findDistanceToDestination(MCAverage, Destination);
    cout << "Angle: " << MoveBot.AngleInRadian << " Distance: " << MoveBot.DistanceInPixel << endl;

    return 0;
}
