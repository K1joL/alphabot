#include "ImageProcessing.h"

float MovementCalculation::findAngle(cv::Point2i massCenter1, cv::Point2i massCenter2, cv::Point2i dest)
{
    int CenterX = int(massCenter1.x + massCenter2.x)/2;
    int CenterY = int(massCenter1.y + massCenter2.y)/2;
    float angleInCos = static_cast<float>(((massCenter1.x - CenterX) * (CenterX - dest.x) + (massCenter1.y - CenterY) * (CenterY - dest.y))/
                            (sqrt(pow((massCenter1.x - CenterX), 2) + pow((massCenter1.y - CenterY), 2)) * sqrt(pow((CenterX - dest.x), 2) + pow((CenterY - dest.y), 2))));
    return angleInCos;
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
    uint8_t Offset = 10;
    std::vector <uint8_t> HsvMin = {static_cast<uint8_t>(color[0] - Offset),100,100};
    std::vector <uint8_t> HsvMax = {static_cast<uint8_t>(color[0] + Offset),255,255};
    
    //Make result image matrix with found color
    inRange(frameHSV, HsvMin, HsvMax, threshold);
    //Creating the image in white&black with area of needed color 
    // imshow( "FrameResult.jpg", threshold );
    return threshold;
}

cv::Rect Detector::detectBlob(cv::Mat &frame, const cv::Mat &threshold)
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
        int minArea = 5'000;
        if(area > minArea)
        {
            rectangle = minRect[i].boundingRect();
            //Uncomment if you want to see Rectangles on image
            cv::Point2f rect_points[4];
            minRect[i].points( rect_points );
            for ( int j = 0; j < 4; j++ )
            {
                line( frame, rect_points[j], rect_points[(j+1)%4], (0,0,255) );
            }
        }
    }
    // imshow("result", threshold);
    // waitKey();
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

