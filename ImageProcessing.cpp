#include "ImageProcessing.h"

float Detector::findAngle(const cv::Point2i &massCenter1, const cv::Point2i &massCenter2,  const cv::Point2i &massCenter3)
{
    //normal vectors n(A,B)
    float A1, A2, B1, B2;
    A1 = massCenter1.x - massCenter2.x;
    A2 = massCenter2.x - massCenter3.x;
    B1 = massCenter1.y - massCenter2.y;
    B2 = massCenter2.y - massCenter3.y;

    float CosOfAngle = static_cast<float>(
        (A1 * A2 + B1 * B2) /
        (sqrt(pow(A1, 2) + pow(B1, 2)) * sqrt(pow(A2, 2) + pow(B2, 2))));
    return CosOfAngle;
}

int Detector::findDistanceToDestination(const cv::Point2i &averageCenter, const cv::Point2i &destination)
{
    int distanceInPixel = sqrt(pow((averageCenter.x - destination.x),2) + pow((averageCenter.y - destination.y),2));
    return distanceInPixel;
}

cv::Point2i Detector::GetMassCenter(const cv::Rect &rectangle)
{
    cv::Point2i TopLeftPointRect = rectangle.tl();
    cv::Point2i BottomRightPointRect = rectangle.br();
    int X = (TopLeftPointRect.x + BottomRightPointRect.x)/2;
    int Y = (TopLeftPointRect.y + BottomRightPointRect.y)/2;
    return cv::Point2i(X, Y);
}

cv::Point2i Detector::GetMassCenter(const cv::Point2i &center1, const cv::Point2i &center2)
{
    int X = (center1.x + center2.x) / 2;
    int Y = (center1.y + center2.y) / 2;
    return cv::Point2i(X, Y);
}

cv::Mat Detector::TakeThresholdOfBlob(const cv::Mat &frameHSV, Color color)
{
    cv::Mat threshold;
    //vectors of HSV color that need to find. Make a range using Offset
    uint8_t Offset = 15;
    std::vector<uint8_t> HsvMin = {static_cast<uint8_t>(color[0] - Offset), static_cast<uint8_t>(color[1] - Offset), static_cast<uint8_t>(color[2] - Offset)};
    std::vector<uint8_t> HsvMax = {static_cast<uint8_t>(color[0] + Offset), 255, 255};

    //Make result image matrix with found color
    inRange(frameHSV, HsvMin, HsvMax, threshold);
    //Creating the image in white&black with area of needed color
    return threshold;
}

cv::Rect Detector::detectBlob(cv::Mat &frame, const cv::Mat &threshold)
{
    cv::Rect rectangle;
    std::vector<std::vector<cv::Point>> contours;

    findContours(threshold, contours, cv::RETR_TREE, cv::CHAIN_APPROX_SIMPLE);
    
    std::vector<cv::RotatedRect> minRect(contours.size());
    for(size_t i = 0; i < contours.size(); i++)
            minRect[i] = minAreaRect(contours[i]);
    
    for(size_t i = 0; i < contours.size(); i++)
    {
        int area = minRect[i].size.area();
        int minArea = 1500;
        if(area > minArea)
        {
            rectangle = minRect[i].boundingRect();
            cv::Point2f rect_points[4];
            minRect[i].points( rect_points );
            for ( int j = 0; j < 4; j++ )
            {
                line( frame, rect_points[j], rect_points[(j+1)%4], (0,0,0) );
            }
        }
    }
    return rectangle;
}

cv::Point2i Detector::SteppedDetection(cv::Mat &frame, Color colorHSV)
{
    cv::Mat frameHSV, threshold;
    cv::Rect rectangleOfColor;

    cvtColor(frame, frameHSV, cv::COLOR_BGR2HSV);
    threshold = TakeThresholdOfBlob(frameHSV, colorHSV);
    rectangleOfColor = detectBlob(frame, threshold);

    return GetMassCenter(rectangleOfColor);
}

