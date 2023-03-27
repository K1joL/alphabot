#include "RequestProcessing.h"

void Request::SetColor(Color color)
{
    this->ColorPuf = color;
}

void Request::SetDestination(cv::Point2i point)
{
    this->Destination = point;
}

void Request::SetType(TypesOfRequest type)
{
    this->Type = type;
}

Color Request::GetColor()
{
    return ColorPuf;
}

cv::Point2i Request::GetDestination()
{
    return Destination;
}

TypesOfRequest Request::GetType()
{
    return Type;
}

Request &Request::operator=(Request &req)
{
    this->ColorPuf = req.ColorPuf;
    this->Type = req.Type;
    return *this;
}

Request::Request(Color colorPuf, TypesOfRequest type)
{
    this->Type = type;
    this->ColorPuf = colorPuf;
}

Request::Request()
{
    this->Type = TypesOfRequest::None;
}

void Controller::MakeRequest(Request &req, Color ColorPuf, TypesOfRequest Type)
{
    req.SetColor(ColorPuf);
    req.SetType(Type);
}

void Controller::FinishRequest(Request &req)
{
    req.SetType(TypesOfRequest::None);
}

void Controller::Move(int &distance)
{
    std::cout << "Moving " << distance << std::endl;
    distance = 0;
    std::cout << "Movement Completed!" << std::endl;
}

void Controller::Rotate(float &angle)
{
    std::cout << "Rotating to " << angle * 180 / M_PIf << std::endl;
    angle = 0;
    std::cout << "Rotation Completed!" << std::endl;
}

void Controller::GoHome()
{
    std::cout << "Going Home.." << std::endl;
}

void Controller::FiniteAutomate(Request &request)
{
    float angle = 0;
    int distance = 0;
    Detector detector;
    while (true)
    {
        Controller *controller = this;
        switch (state)
        {
        case States::Running:
        {
            // Making a request
            char c;
            std::cin >> c;
            if (c == 'd')
            {
                std::cout << "Enter color of ottoman" << std::endl;
                int Hue = 0, Saturation = 0, Value = 0;
                std::cout << "Hue: ";
                std::cin >> Hue;
                std::cout << "Saturation: ";
                std::cin >> Saturation;
                std::cout << "Value: ";
                std::cin >> Value;

                Color colorPuf(Hue, Saturation, Value);

                controller->MakeRequest(request, colorPuf, TypesOfRequest::Deliver);
            }

            if (request.GetType() == TypesOfRequest::System)
                state = Disabling;
            else if (request.GetType() == TypesOfRequest::Deliver)
                state = DoDeliver;
            else
                controller->GoHome();
            break;
        }
        case States::DoDeliver:
        {
            // VideoCapture Cap("0");
            // Capture frame-by-frame
            // Cap >> frame;

            // If the frame is empty, break immediately
            // if (frame.empty())
            //   State = Running;

            // Imitation of capturing camera
            cv::Mat frame = cv::imread("img_0.jpg");

            cv::Point2i massCenterBlue, massCenterMagenta, massCenterAverage;

            detector.SteppedDetection(frame, &massCenterBlue, &massCenterMagenta, &massCenterAverage, BlueHsv, MagentaHsv);

            MovementCalculation moveBot;
            angle = moveBot.findAngle(massCenterBlue, massCenterMagenta, request.GetDestination());
            distance = moveBot.findDistanceToDestination(massCenterAverage, request.GetDestination());
            std::cout << "Angle : " << angle << " Distance: " << distance << std::endl;

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
            controller->Move(distance);
            break;

        case States::Rotate:
            state = DoDeliver;
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
            state = Running;
            controller->FinishRequest(request);
            break;
        }
    }
}
