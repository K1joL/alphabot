#ifndef REQUESTPROCESSING_H
#define REQUESTPROCESSING_H

#include "opencv2/opencv.hpp"
#include <iostream>

#include "ImageProcessing.h"

class MosquittoPub;
class MosquittoSub;

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
        cv::Point2i GetDestination()                                { return destination_; }
        TypesOfRequest GetType()                                    { return type_; }
};

class Controller
{
    private:
        Color tailColorHsv{100, 0, 0};
        Color headColorHsv{166, 0, 0};
        States state = Running;

    public:
        void MakeRequest(Request &req, Color colorPuf, TypesOfRequest type);
        void FinishRequest(Request &req);
        void Move(int distance, MosquittoPub &mosPub);
        void Rotate(float angle, MosquittoPub &mosPub);
        void GoHome();
        void FiniteAutomate(cv::VideoCapture &cap);
};

#endif
