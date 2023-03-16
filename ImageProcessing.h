#ifndef IMAGEPROCESSING_H
#define IMAGEPROCESSING_H

#include "opencv2/opencv.hpp"
#include <vector>

class Color
{
    private:
        double colorHsv[3];
    public:
        Color();
        Color(double hue, double saturation, double value);
        double& operator[](int index);
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
        cv::Rect detectBlob(const cv::Mat &threshold);
        void SteppedDetection(const cv::Mat &frame, cv::Point2i *massCenter1, cv::Point2i *massCenter2, cv::Point2i *massCenterAverage, const Color &colorHsv1, const Color &colorHsv2);
        cv::Point2i GetMassCenter(cv::Rect rectangle);
        cv::Point2i GetMassCenter(cv::Point2i *center1, cv::Point2i *center2);
};

#endif