#include "RequestProcessing.h"
#include <pthread.h>
#include "Mqtt.h"

void Request::SetColor(double hue, double saturation, double value)
{
    colorPuf_[0] = hue;
    colorPuf_[1] = saturation;
    colorPuf_[2] = value;
}

void Request::SetDestination(cv::Point2i point)
{
    if (!this->destination_.x || !this->destination_.y)
        this->destination_ = point;
}

Request &Request::operator=(Request &req)
{
    this->colorPuf_ = req.colorPuf_;
    this->type_ = req.type_;
    return *this;
}

void Controller::MakeRequest(Request &req, Color colorPuf, TypesOfRequest type)
{
    req.SetColor(colorPuf);
    req.SetType(type);
}

void Controller::FinishRequest(Request &req)
{
    req.SetType(TypesOfRequest::None);
}

void Controller::Move(int distance, MosquittoPub &mosPub)
{
    std::cout << "Moving " << distance << std::endl;
    mosPub.Publish("Moving");
}

void Controller::Rotate(float angle, MosquittoPub &mosPub)
{
    std::cout << "Rotating to " << angle * 180/ M_PIf << std::endl;
    mosPub.Publish("Rotating");
}

void Controller::GoHome()
{
    std::cout << "Going Home.." << std::endl;
}

void Controller::FiniteAutomate(cv::VideoCapture &cap)
{
    MosquittoPub mosPub;
    Detector detector;
    float angle = 0;
    int distance = 0;
    cv::namedWindow("result", cv::WINDOW_AUTOSIZE);
    
    Request currentRequest;
    bool isOn = true;
    while (isOn)
    {
        Controller *controller = this;
        switch (state)
        {
        case States::Running:
        {
            // Making a request
            char c;
            std::cout << "Enter \'d\' to deliver\n";
            std::cout << "Enter \'e\' to exit\n";
            std::cin >> c;
            if (c == 'd')
            {
                MosquittoSub ServerSub("localhost", "/telega");
                ServerSub.Subscribe();

                for(int i = 0; i < 3; i++)
                {
                    std::cout << ServerSub.GetReturned().GetColor()[i] << " \n";
                }
                sleep(2);
                currentRequest.SetColor(ServerSub.GetReturned().GetColor());
                controller->MakeRequest(currentRequest, currentRequest.GetColor(), TypesOfRequest::Deliver);

            }else if(c == 'e')
            {
                isOn = false;
                break;
            }

            if (currentRequest.GetType() == TypesOfRequest::System)
                state = Disabling;
            else if (currentRequest.GetType() == TypesOfRequest::Deliver)
                state = DoDeliver;
            else
                controller->GoHome();
            break;
        }
        case States::DoDeliver:
        {
            cv::Mat frame;
            // Capture frame-by-frame
            cap >> frame;

            // If the frame is empty, break immediately
            if (frame.empty())
                state = States::Running;

            cv::Point2i massCenterHead = detector.SteppedDetection(frame, headColorHsv), 
            massCenterTail = detector.SteppedDetection(frame, tailColorHsv), 
            massCenterAverage = detector.GetMassCenter(&massCenterHead, &massCenterTail);

            //Setting destination
            currentRequest.SetDestination(detector.SteppedDetection(frame, currentRequest.GetColor()));
            std::cout << currentRequest.GetDestination() << std::endl;

            MovementCalculation moveBot;
            angle = moveBot.findAngle(massCenterTail, massCenterHead, currentRequest.GetDestination());
            distance = moveBot.findDistanceToDestination(massCenterAverage, currentRequest.GetDestination());
            std::cout << "Angle : " << angle << " Distance: " << distance << std::endl;

            cv::imshow("result", frame);
            cv::waitKey(500);

            // Deviation for if`s
            float errorAngle = 0.1;
            int errorDist = 100;
            if (angle > errorAngle || angle < (-errorAngle))
            {
                state = States::Rotate;
                break;
            }
            else if (distance > errorDist)
            {
                state = States::Move;
                break;
            }
            else
            {
                state = Waiting;
                break;
            }

            break;
        }

        case States::Move:
            state = DoDeliver;
            controller->Move(distance, mosPub);
            break;

        case States::Rotate:
            state = DoDeliver;
            controller->Rotate(angle, mosPub);
            break;

        case States::Waiting:
            // while(CupOnTheBot())
            // {
            //     if(CloseToClient())
            //         cout << "Take your drink!" << endl;
            //     if(CloseToDispenser())
            //         cout << "The drink is being poured!" << endl;
            // }
            state = Running;
            controller->FinishRequest(currentRequest);
            break;
        }
    }
    cv::destroyAllWindows;
}
