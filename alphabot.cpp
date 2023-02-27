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

int main()
{
    cout << "Hello Robot!" << endl;

    return 0;
}

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

Rect detectBlob(Mat Frame, Color c)
{
    
}