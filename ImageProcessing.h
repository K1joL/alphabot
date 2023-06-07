#ifndef IMAGEPROCESSING_H
#define IMAGEPROCESSING_H

#include "opencv2/opencv.hpp"

class Color
{
    private:
        std::vector<double> color_ =  {0,0,0};
    public:
        Color(std::vector<double> color) : color_{color} 
        {
        }
        Color(double hue, double saturation, double value)
            : color_{hue, saturation, value}
        {
        }
        double &operator[](int index) { return color_[index]; }
};

class Detector
{
    public:
        float findAngle(const cv::Point2i &massCenter1, const cv::Point2i &massCenter2, const cv::Point2i &massCenter3);
        int findDistanceToDestination(const cv::Point2i &averageCenter, const cv::Point2i &destination);
        cv::Mat TakeThresholdOfBlob(const cv::Mat &frameHSV, Color color);
        cv::Rect detectBlob(cv::Mat &frame, const cv::Mat &threshold);
        cv::Point2i SteppedDetection(cv::Mat &frame, Color colorHSV);
        cv::Point2i GetMassCenter(const cv::Rect &rectangle);
        cv::Point2i GetMassCenter(const cv::Point2i &center1, const cv::Point2i &center2);
};

#endif