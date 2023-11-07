#ifndef DETECTOR_H
#define DETECTOR_H

#include "opencv2/opencv.hpp"
#include <vector>
#include <cassert>
class Color
{
private:
    uint8_t color[3];

public:
    Color(uint8_t first, uint8_t second, uint8_t third)
        : color{first, second, third}
    {
    }
    Color(std::vector<uint8_t> color)
    {
        assert(color.size() == 3);
        for(int i = 0; i < color.size(); i++)
            this->color[i] = color[i];
    }
    const uint8_t &operator[](int index) const { return color[index]; }
    std::vector<uint8_t> GetVector() const { return std::vector<uint8_t>{color[0], color[1], color[2]}; }
};

using namespace cv;class Detector
{
public:
    float GetAngleToPoint(const Point2i &frstPoint,
                          const Point2i &scndPoint, const Point2i &thrdPoint);
    int GetDistanceToPoint(const Point2i &frstPoint, const Point2i &scndPoint);
    Point2i GetMassCenter(const Rect &rectangle) const;
    Point2i GetMassCenter(const Point2i &frstPoint, const Point2i &scndPoint) const;
    Mat GetMask(const Mat &frame, const Color &colorLow, const Color &colorHigh) const;
    Rect GetRectOfColor(const Mat &mask, Mat *frame = nullptr) const;
    Point2i SteppedDetection(Mat &frame, const Color &colorHSVLow, const Color &colorHSVHigh);
    Point2i SteppedDetection(const Mat &frame, const Color &colorHSVLow, const Color &colorHSVHigh) const;
    void DrawAngle(Mat &frame, const Point2i &frstPoint,
                   const Point2i &scndPoint, const Point2i &thrdPoint);
};

#endif //DETECTOR_H