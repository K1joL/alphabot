#ifndef IMAGEPROCESSING_H
#define IMAGEPROCESSING_H

#include "opencv2/opencv.hpp"

class Color
{
    private:
        double colorHsv[3];
    public:
        Color() : colorHsv{} 
        {
        }
        Color(double hue, double saturation, double value)
            : colorHsv{hue, saturation, value}
        {
        }
        double &operator[](int index) { return colorHsv[index]; }
};

class MovementCalculation
{
    public:
        float findAngle(cv::Point2i massCenter1, cv::Point2i massCenter2, cv::Point2i destination);
        int findDistanceToDestination(cv::Point2i averageCenter, cv::Point2i destination);
};

class Detector
{
    public:
        cv::Mat TakeThresholdOfBlob(const cv::Mat &frameHSV, Color Color);
        cv::Rect detectBlob(cv::Mat &frame, const cv::Mat &threshold);
        cv::Point2i SteppedDetection(cv::Mat &frame, Color colorHSV);
        cv::Point2i GetMassCenter(cv::Rect rectangle);
        cv::Point2i GetMassCenter(cv::Point2i *center1, cv::Point2i *center2);
};

#endif