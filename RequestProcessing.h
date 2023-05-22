#ifndef REQUESTPROCESSING_H
#define REQUESTPROCESSING_H

#include "opencv2/opencv.hpp"
#include <iostream>
#include <memory>

#include "ImageProcessing.h"
#include "Mqtt.h"

const double PIXELS_PER_SECOND = 100;

enum class TypesOfRequest{
    System = -1, 
    None = 0, 
    Deliver
    };

enum States{
        Running = 1,
        DoDeliver,
        Move,
        Rotate,
        Waiting,
        Disabling
    };

struct AlphaBot
        {
            cv::Point2i head = {0,0};
            cv::Point2i tail = {0,0};
        };

class Request
{
    private:
        cv::Point2i destination_ {0,0};
        TypesOfRequest type_;
        Color colorPuf_{0,0,0};
    public:
        Request() : type_ {TypesOfRequest::None} 
        {            
        }
        Request(Color colorPuf, TypesOfRequest type = TypesOfRequest::None) 
            : colorPuf_{colorPuf}, type_{type}
        {
        }

        Request &operator= (Request &req);
        void SetColor(double hue, double saturation, double value);
        void SetDestination(cv::Point2i point);
        void SetColor(Color color)                                  { this->colorPuf_ = color; }
        void SetType(TypesOfRequest type)                           { this->type_ = type; }
        Color GetColor()                                            { return colorPuf_; }
        cv::Point2i& GetDestination()                                { return destination_; }
        TypesOfRequest GetType()                                    { return type_; }
};

class Controller
{
    private:
        Color tailColorHsv_{110, 140, 100};
        Color headColorHsv_{160, 140, 100};
        States state_ = Running;
        std::shared_ptr<myMosq> mosq_;
        AlphaBot bot_;

    public:
        void MakeRequest(Request &req, Color colorPuf, TypesOfRequest type);
        void FinishRequest(Request &req);
        void Move(int distance);
        void Rotate(float angle);
        void GoHome();
        void FiniteAutomate(cv::VideoCapture &cap);
        const char *ChooseTheCommand(int distance);
        const char *ChooseTheCommand(float cosOfAngle);
};

#endif
