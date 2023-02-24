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
        MassCenter(MassCenter Cntr1, MassCenter Cntr2);
};

class MovementCalculation
{
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