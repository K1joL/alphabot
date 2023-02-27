#include "opencv2/opencv.hpp"
#include <iostream>

using namespace std;
using namespace cv;

struct Color
{
    int Red;
    int Green;
    int Blue;
};

class MassCenter
{
    protected:
        int X;
        int Y;
    public:
        MassCenter(Rect Rectangle);
        MassCenter(MassCenter Center1, MassCenter Center2);
};

class MovementCalculation
{
    public:
        float AngleInRadian;
        int DistanceInPixel;

        float findAngle(MassCenter blob1, MassCenter blob2, Point2i Destination);
        int findDistanceToDestination(MassCenter AverageCenter, Point2i Destination);

        Rect detectBlob(Mat Frame, Color c);
};

int main()
{
    cout << "Hello Robot!" << endl;

    return 0;
}

MassCenter::MassCenter(Rect Rectangle)
{
    Point TopLeftPointRect = Rectangle.tl();
    Point BottomRightPointRect = Rectangle.br();
    this->X = (TopLeftPointRect.x + BottomRightPointRect.x)/2;
    this->Y = (TopLeftPointRect.y + BottomRightPointRect.y)/2;
}

MassCenter::MassCenter(MassCenter Center1, MassCenter Center2)
{
    this->X = (Center1.x + Center2.x)/2;
    this->Y = (Center1.y + Center2.y)/2;
}

Rect MovementCalculation::detectBlob(Mat Frame, Color c)
{
    
}