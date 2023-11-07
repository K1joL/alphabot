#include "detector.h"

using namespace cv;

// Находит угол между двумя прямыми построенными по двум точкам (frstPoint и scndPoint) и (scndPoint и thrdPoint)
float Detector::GetAngleToPoint(const Point2i &frstPoint, 
                                const Point2i &scndPoint, const Point2i &thrdPoint)
{
    float CosOfAngle = static_cast<float>(
        ((scndPoint.x - frstPoint.x) * (frstPoint.x - thrdPoint.x) +
         (scndPoint.y - frstPoint.y) * (frstPoint.y - thrdPoint.y)) /
        (sqrt(pow((scndPoint.x - frstPoint.x), 2) + pow((scndPoint.y - frstPoint.y), 2)) *
         sqrt(pow((frstPoint.x - thrdPoint.x), 2) + pow((frstPoint.y - thrdPoint.y), 2))));
    return CosOfAngle;
}

int Detector::GetDistanceToPoint(const Point2i &frstPoint, const Point2i &scndPoint)
{
    int distance = sqrt(pow((frstPoint.x - scndPoint.x), 2) + pow((frstPoint.y - scndPoint.y), 2));
    return distance;
}

Point2i Detector::GetMassCenter(const Rect &rectangle) const
{
    Point2i TopLeftPointRect = rectangle.tl();
    Point2i BottomRightPointRect = rectangle.br();
    int X = (TopLeftPointRect.x + BottomRightPointRect.x) / 2;
    int Y = (TopLeftPointRect.y + BottomRightPointRect.y) / 2;
    return Point2i(X, Y);
}

Point2i Detector::GetMassCenter(const Point2i &frstPoint, const Point2i &scndPoint) const
{
    int X = (frstPoint.x + scndPoint.x) / 2;
    int Y = (frstPoint.y + scndPoint.y) / 2;
    return cv::Point2i(X, Y);
}

Mat Detector::GetMask(const Mat &frame, const Color &colorLow, const Color &colorHigh) const
{
    Mat mask;
    inRange(frame, colorLow.GetVector(), colorHigh.GetVector(), mask);
    return mask;
}

Rect Detector::GetRectOfColor(const Mat &mask, Mat *frame) const
{
    cv::Rect rect;
    std::vector<std::vector<cv::Point>> contours;

    findContours(mask, contours, cv::RETR_TREE, cv::CHAIN_APPROX_SIMPLE);

    std::vector<Rect> boundRects(contours.size());
    for (size_t i = 0; i < contours.size(); i++)
        boundRects[i] = boundingRect(contours[i]);

    for (size_t i = 0; i < contours.size(); i++)
    {
        int area = boundRects[i].area();
        int minArea = 10000;
        if (area > minArea)
        {
            rect = boundRects[i];

            if (frame)
                rectangle(*frame, boundRects[i].tl(), boundRects[i].br(), (0, 255, 0), 3);
        }
    }

    return rect;
}

Point2i Detector::SteppedDetection(Mat &frame, const Color &colorHSVLow, const Color &colorHSVHigh)
{
    cv::Mat frameHSV, mask;
    cv::Rect rectangleOfColor;

    cvtColor(frame, frameHSV, cv::COLOR_BGR2HSV);
    uint8_t offset = 15;
    mask = GetMask(frameHSV, colorHSVLow, colorHSVHigh);
    rectangleOfColor = GetRectOfColor(mask, &frame);

    return GetMassCenter(rectangleOfColor);
}

Point2i Detector::SteppedDetection(const Mat &frame, const Color &colorHSVLow,
                                   const Color &colorHSVHigh) const
{
    cv::Mat frameHSV, mask;
    cv::Rect rectangleOfColor;

    cvtColor(frame, frameHSV, cv::COLOR_BGR2HSV);
    uint8_t offset = 15;
    mask = GetMask(frameHSV, colorHSVLow, colorHSVHigh);
    rectangleOfColor = GetRectOfColor(mask);

    return GetMassCenter(rectangleOfColor);
}

void Detector::DrawAngle(Mat &frame, const Point2i &frstPoint,
                         const Point2i &scndPoint, const Point2i &thrdPoint)
{
    line(frame, static_cast<cv::Point>(frstPoint), static_cast<cv::Point>(scndPoint), (0, 255, 0), 3);
    line(frame, static_cast<cv::Point>(scndPoint), static_cast<cv::Point>(thrdPoint), (0, 255, 0), 3);
}
