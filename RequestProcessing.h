#ifndef REQUESTPROCESSING_H
#define REQUESTPROCESSING_H

#include "opencv2/opencv.hpp"
#include <iostream>
#include "ImageProcessing.h"

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
        cv::Point2i Destination {960,1280};
        TypesOfRequest Type;
        Color ColorPuf{0,0,0};
    public:
        Request();
        Request(Color colorPuf, TypesOfRequest type = TypesOfRequest::None);
        Request &operator= (Request &req);
        void SetColor(Color Color);
        void SetDestination(cv::Point2i Point);
        void SetType(TypesOfRequest Type);
        Color GetColor();
        cv::Point2i GetDestination();
        TypesOfRequest GetType();
};

class Controller
{
    private:
    Color BlueHsv{100, 0, 0};
    Color MagentaHsv{166, 0, 0};
    States state = Running;

    public:
        void MakeRequest(Request &req, Color ColorPuf, TypesOfRequest Type);
        void FinishRequest(Request &req);
        void Move(int &distance);
        void Rotate(float &angle);
        void GoHome();
        void FiniteAutomate(Request &request);
};
#endif