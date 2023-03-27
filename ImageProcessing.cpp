#include "ImageProcessing.h"

float MovementCalculation::findAngle(cv::Point2i massCenter1, cv::Point2i massCenter2, cv::Point2i dest)
{
    int CenterX = int(massCenter1.x + massCenter2.x)/2;
    int CenterY = int(massCenter1.y + massCenter2.y)/2;
    float angleInRadian = static_cast<float>(acos(((massCenter1.x - CenterX) * (CenterX - dest.x) + (massCenter1.y - CenterY) * (CenterY - dest.y))/
                            (sqrt(pow((massCenter1.x - CenterX), 2) + pow((massCenter1.y - CenterY), 2)) * sqrt(pow((CenterX - dest.x), 2) + pow((CenterY - dest.y), 2)))));
    return angleInRadian;
}

int MovementCalculation::findDistanceToDestination(cv::Point2i averageCenter, cv::Point2i dest)
{
    int distanceInPixel = sqrt(pow((averageCenter.x - dest.x),2) + pow((averageCenter.y - dest.y),2));
    return distanceInPixel;
}

cv::Point2i Detector::GetMassCenter(cv::Rect rectangle)
{
    cv::Point2i TopLeftPointRect = rectangle.tl();
    cv::Point2i BottomRightPointRect = rectangle.br();
    int X = (TopLeftPointRect.x + BottomRightPointRect.x)/2;
    int Y = (TopLeftPointRect.y + BottomRightPointRect.y)/2;
    return cv::Point2i(X, Y);
}

cv::Point2i Detector::GetMassCenter(cv::Point2i *center1, cv::Point2i *center2)
{
    int X = (center1->x + center2->x) / 2;
    int Y = (center1->y + center2->y) / 2;
    return cv::Point2i(X, Y);
}

cv::Mat Detector::TakeThresholdOfBlob(const cv::Mat &frameHSV, Color color)
{
    cv::Mat threshold;
    //vectors of HSV color that need to find. Make a range using Offset
    uint8_t Offset = 5;
    std::vector <uint8_t> HsvMin = {static_cast<uint8_t>(color[0] - Offset),100,100};
    std::vector <uint8_t> HsvMax = {static_cast<uint8_t>(color[0] + Offset),255,255};
    
    //Make result image matrix with found color
    inRange(frameHSV, HsvMin, HsvMax, threshold);
    //Creating the image in white&black with area of needed color 
    // imshow( "FrameResult.jpg", threshold );
    return threshold;
}

cv::Rect Detector::detectBlob(const cv::Mat &threshold)
{
    cv::Rect rectangle;
    std::vector<std::vector<cv::Point>> contours;
    std::vector<cv::Vec4i> hierarchy;

    findContours(threshold, contours, cv::RETR_TREE, cv::CHAIN_APPROX_SIMPLE);
    
    std::vector<cv::RotatedRect> minRect(contours.size());
    for(size_t i = 0; i < contours.size(); i++)
            minRect[i] = minAreaRect(contours[i]);
    
    for(size_t i = 0; i < contours.size(); i++)
    {
        int area = minRect[i].size.height*minRect[i].size.width;
        int minArea = 10'000;
        if(area > minArea)
        {
            rectangle = minRect[i].boundingRect();
            //Uncomment if you want to see Rectangles on image
            // Point2f rect_points[4];
            // minRect[i].points( rect_points );
            // for ( int j = 0; j < 4; j++ )
            // {
            //     line( threshold, rect_points[j], rect_points[(j+1)%4], (0,0,255) );
            // }
        }
    }
    // imshow("Result", threshold);
    // waitKey();
    return rectangle;
}

void Detector::SteppedDetection(const cv::Mat &frame, cv::Point2i *massCenter1, cv::Point2i *massCenter2, cv::Point2i *massCenterAverage, const Color &colorHsv1, const Color &colorHsv2)
{
    cv::Mat frameHSV, threshold;

    // converting the Frame to FrameHsv color model
    cvtColor(frame, frameHSV, cv::COLOR_BGR2HSV);

    threshold = TakeThresholdOfBlob(frameHSV, colorHsv1);
    cv::Rect rectangleOfColor = detectBlob(threshold);
    *massCenter1 = GetMassCenter(rectangleOfColor);
    // just for tests
    //  cout << RectangleOfMagenta << endl;

    threshold = TakeThresholdOfBlob(frameHSV, colorHsv2);
    rectangleOfColor = detectBlob(threshold);
    *massCenter2 = GetMassCenter(rectangleOfColor);
    // just for tests
    //  cout << RectangleOfBlue << endl;
    *massCenterAverage = GetMassCenter(massCenter1, massCenter2);
}

Color::Color()
{
    colorHsv[0] = 0;
    colorHsv[1] = 0;
    colorHsv[2] = 0;
}

Color::Color(double hue, double saturation, double value)
{
    colorHsv[0] = hue;
    colorHsv[1] = saturation;
    colorHsv[2] = value;
}

double& Color::operator[](int index)
{
    return colorHsv[index];
}
