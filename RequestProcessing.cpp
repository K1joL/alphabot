#include "RequestProcessing.h"
        #include <string.h>

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

void Controller::Move(int distance)
{
    const char *cmd = ChooseTheCommand(distance);
    myMosq::MqttMessage mes{cmd, (1.0)};
    
    mosq_->Publish(&mes);
}

void Controller::Rotate(float cosOfAngle)
{
    // const char* cmd = ChooseTheCommand(cosOfAngle);
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
    float cosOfAngle = 0;
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
                isOn = false;
                break;
            }

            if (currentRequest.GetType() == TypesOfRequest::System)
                state_ = Disabling;
            else if (currentRequest.GetType() == TypesOfRequest::Deliver)
                state_ = DoDeliver;
            else
                controller->GoHome();
            break;
        }

        case States::DoDeliver:
        {
            cv::Mat frame;
            // Capture frame-by-frame
            cap >> frame;
            // std::string base = "target.png";
            // std::string file = base;
            // frame = cv::imread(file.insert(6, std::to_string(i)));

            // If the frame is empty, break immediately
            if (frame.empty())
                state_ = States::Running;

            // Detecting robot
            bot_.head = detector.SteppedDetection(frame, headColorHsv_);
            bot_.tail = detector.SteppedDetection(frame, tailColorHsv_);
            cv::Point2i massCenterAverage = detector.GetMassCenter(bot_.head, bot_.tail);

            // Setting destination
            currentRequest.SetDestination(detector.SteppedDetection(frame, currentRequest.GetColor()));
            std::cout << currentRequest.GetDestination() << std::endl;
            std::cout << "Bot: H: " << bot_.head << " T: " << bot_.tail << std::endl;

            MovementCalculation moveBot;
            cosOfAngle = moveBot.findAngle(bot_.head, massCenterAverage, currentRequest.GetDestination());
            distance = moveBot.findDistanceToDestination(massCenterAverage, currentRequest.GetDestination());
            std::cout << "Angle : " << cosOfAngle << " Distance: " << distance << std::endl;

            // draw angle lines
            line(frame, static_cast<cv::Point>(bot_.head), static_cast<cv::Point>(massCenterAverage), (255, 0, 0));
            line(frame, static_cast<cv::Point>(massCenterAverage), static_cast<cv::Point>(currentRequest.GetDestination()), (0, 255, 0));

            cv::resize(frame, frame, cv::Size(frame.cols/3,frame.rows/3));
            cv::imshow("result", frame);
            cv::waitKey(100);

            // Deviation for if`s
            float errorAngle = 15;
            float angle = acos(cosOfAngle) * 180 / M_PIf;
            int errorDist = 100;
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
            std::cout << "Rotating to " << atan(cosOfAngle) * 180 / M_PIf << std::endl;
            state_ = DoDeliver;
            controller->Rotate(cosOfAngle);
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

const char *Controller::ChooseTheCommand(int distance)
{
    return "forward";
}

const char *Controller::ChooseTheCommand(float cosOfAngle)
{
    if (cosOfAngle > 0)
    {
        if (bot_.tail.x < bot_.head.x)
            if (bot_.tail.y < bot_.head.y)
                return "left";
            else
                return "right";
        else if (bot_.tail.y > bot_.head.y)
            return "left";
        else
            return "right";
    }
    else
    {
        if (bot_.tail.x < bot_.head.x)
            if (bot_.tail.y > bot_.head.y)
                return "left";
            else
                return "right";
        else if (bot_.tail.y < bot_.head.y)
            return "left";
        else
            return "right";
    }
}
