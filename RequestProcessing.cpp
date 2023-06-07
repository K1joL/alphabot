#include "RequestProcessing.h"
#include <string.h>

using TypesOfRequest = Request::TypesOfRequest;

void Request::SetColor(double hue, double saturation, double value)
{
    colorPuf_[0] = hue;
    colorPuf_[1] = saturation;
    colorPuf_[2] = value;
}

void Request::SetDestination(cv::Point2i point)
{
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

void Controller::Move(int distance)
{
    const char *cmd = "forward";
    myMosq::MqttMessage mes{cmd, (1.0)};
    mosq_->Publish(&mes);
}

void Controller::Rotate(float cosOfAngle)
{
    const char* cmd = "right";
    myMosq::MqttMessage mes{cmd, 1.0};
    mosq_->Publish(&mes);
}

void Controller::GoHome()
{
    std::cout << "Going Home.." << std::endl;
}

void Controller::FiniteAutomate(cv::VideoCapture &cap)
{
    mosq_ = std::make_shared<myMosq>("localhost", "/telega", "abot/command/alex");
    Detector detector;
    float angle = 0;
    int distance = 0;
    cv::namedWindow("result", cv::WINDOW_NORMAL);

    Request currentRequest;
    bool isOn = true;
    while (isOn)
    {
        Controller *controller = this;
        switch (state_)
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
                mosq_->Subscribe();

                for (int i = 0; i < 3; i++)
                {
                    std::cout << mosq_->GetMessage()[i] << " \n";
                }
                sleep(2);
                currentRequest.SetColor(mosq_->GetMessage());
                controller->MakeRequest(currentRequest, currentRequest.GetColor(), TypesOfRequest::Deliver);
            }
            else if (c == 'e')
            {
                state_ = Disabling;
            }

            if (currentRequest.GetType() == TypesOfRequest::System)
                state_ = Disabling;
            else if (currentRequest.GetType() == TypesOfRequest::Deliver)
                state_ = DoDeliver;
            else
                controller->GoHome();
            break;
        }
        case States::Disabling:
            isOn = false;
            break;

        case States::DoDeliver:
        {
            cv::Mat frame;
            // Capture frame-by-frame
            cap >> frame;

            // If the frame is empty, break immediately
            if (frame.empty())
                state_ = States::Running;

            // Detecting robot
            bot_.head = detector.SteppedDetection(frame, headColorHsv_);
            bot_.tail = detector.SteppedDetection(frame, tailColorHsv_);
            cv::Point2i botCenter = detector.GetMassCenter(bot_.head, bot_.tail);

            // Setting destination
            
            if (!currentRequest.GetDestination().x || !currentRequest.GetDestination().y)
                currentRequest.SetDestination(detector.SteppedDetection(frame, currentRequest.GetColor()));
            std::cout << currentRequest.GetDestination() << std::endl;
            std::cout << "Bot: H: " << bot_.head << " T: " << bot_.tail << std::endl;

            angle = acos(detector.findAngle(botCenter, bot_.head, currentRequest.GetDestination()));
            distance = detector.findDistanceToDestination(botCenter, currentRequest.GetDestination());
            std::cout << "Angle : " << angle << " Distance: " << distance << std::endl;

            // draw angle lines
            line(frame, static_cast<cv::Point>(bot_.head), static_cast<cv::Point>(botCenter), (255, 0, 0));
            line(frame, static_cast<cv::Point>(botCenter), static_cast<cv::Point>(currentRequest.GetDestination()), (0, 255, 0));

            cv::resize(frame, frame, cv::Size(frame.cols/3,frame.rows/3));
            cv::imshow("result", frame);
            cv::waitKey(100);

            // Deviation for if`s
            float errorAngle = 15;
            angle = angle * 180 / M_PIf;
            int errorDist = 200;
            if ( angle > errorAngle )
            {
                state_ = States::Rotate;
                break;
            }
            else if (distance > errorDist)
            {
                state_ = States::Move;
                break;
            }
            else
            {
                state_ = Waiting;
                break;
            }

            break;
        }

        case States::Move:
            std::cout << "Moving " << distance << std::endl;
            state_ = DoDeliver;
            controller->Move(distance);
            break;

        case States::Rotate:
            std::cout << "Rotating to " << angle * 180 / M_PIf << std::endl;
            state_ = DoDeliver;
            controller->Rotate(angle);
            break;

        case States::Waiting:
            // while(CupOnTheBot())
            // {
            //     if(CloseToClient())
            //         cout << "Take your drink!" << endl;
            //     if(CloseToDispenser())
            //         cout << "The drink is being poured!" << endl;
            // }
            state_ = Running;
            controller->FinishRequest(currentRequest);
            break;
        }
    }
    cv::destroyAllWindows;
}