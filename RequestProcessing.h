#ifndef REQUESTPROCESSING_H
#define REQUESTPROCESSING_H

#include "opencv2/opencv.hpp"
#include <iostream>
#include "ImageProcessing.h"
#include <cstring>
#include <stdio.h>
#include <mosquitto.h>
#include <json-c/json.h>
#include <unistd.h>

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
class MosquittoPub
{
    private:
        char *MQTT_SERVER = "localhost";
        int KEEP_ALIVE = 60;
        int MQTT_PORT = 1883;
        char *MQTT_PUB_TOPIC = "/funmqqt";
        int MQTT_QOS_LEVEL = 2;
        int MSG_MAX_SIZE = 512;

    public:
        void SendToServer (const char* data);
        void *Publish (const char* message);
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
        void Move(int distance, MosquittoPub &MosPub);
        void Rotate(float angle, MosquittoPub &MosPub);
        void GoHome();
        void FiniteAutomate(Request &request, cv::VideoCapture &Cap);
};



#endif
