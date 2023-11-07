#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "detector.h"
#include "mqtt.h"
#include <memory>

enum EState{ Wait, Delivery, Move, Rotate, Stop, ESTATEMAX };

using namespace cv;
class Request
{
private:
    Point2i m_destination{0, 0};
    Color m_destinationColorLow{0, 0, 0};
    Color m_destinationColorHigh{0, 0, 0};

public:
//В данной реализации класса Request предполагается,
//что камера закреплена и точка назначения не может измениться
    Request(const Color &destColorL, const Color &destColorH, const Point2i &dest)
        : m_destination{dest}, m_destinationColorLow{destColorL}, m_destinationColorHigh{destColorH}
    {
    }
    Request(const Color &destColorL, const Color &destColorH, const Mat &frame)
    {
        Detector det;
        m_destinationColorLow = destColorL;
        m_destinationColorHigh = destColorH;
        m_destination = det.SteppedDetection(frame, m_destinationColorLow, m_destinationColorHigh);
    }
    //Сеттеры
    void SetColorL(Color color) { this->m_destinationColorLow = color; }
    void SetColorH(Color color) { this->m_destinationColorHigh = color; }
    void SetDestination(cv::Point2i point) { m_destination = point; }
    //Геттеры
    const Color &GetColorL() const { return m_destinationColorLow; }
    const Color &GetColorH() const { return m_destinationColorHigh; }
    const cv::Point2i &GetDestination() const { return m_destination; }
}; 

class Controller
{
private:
    EState m_state = EState::Wait;
    std::queue<Request> m_requestQueue;
    std::shared_ptr<MyMosquitto> m_mosq;
    Point2i m_headPoint, m_tailPoint, m_midPoint;
    float m_CosOfAngle;
    int m_distanceToDestination;

public:
    Controller(const char *mqttServer, const char *mqttSubTopic = "tgbot/request",
               const char *mqttPubTopic = "abot/command/alex", const char *id = NULL,
               bool clean_session = true)
    {
        m_mosq = std::make_shared<MyMosquitto>(mqttServer, mqttSubTopic,
                                               mqttPubTopic, id, clean_session);
    }
    void FinishRequest(Request &request);
    void Move();
    void Rotate();
    void ToHome();
    int ProcessEvents(cv::VideoCapture capture);

};

#endif //CONTROLLER_H