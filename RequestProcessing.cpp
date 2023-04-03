#include "RequestProcessing.h"
#include <pthread.h>

extern Request requestGlobal;

void Request::SetColor(double hue, double saturation, double value)
{
    colorPuf_[0] = hue;
    colorPuf_[1] = saturation;
    colorPuf_[2] = value;
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
    MosquittoSub *ptrServerSub = new MosquittoSub("localhost", "/telega");
    pthread_t thread_id[2];
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
                // std::cout << "Enter color of ottoman" << std::endl;
                // int hue = 0, saturation = 0, value = 0;
                // std::cout << "Hue: ";
                // std::cin >> hue;
                // std::cout << "Saturation: ";
                // std::cin >> saturation;
                // std::cout << "Value: ";
                // std::cin >> value;

                // Color colorPuf(hue, saturation, value);
                if (pthread_create(&thread_id[0], NULL, &MosquittoSub::WrapperSubscribe, ptrServerSub))
                {
                    printf("FAILED......");
                    exit(1);
                }

                if (pthread_create(&thread_id[1], NULL, &MosquittoSub::WrapperCommand, ptrServerSub))
                {
                    printf("FAILED......");
                    exit(1);
                }
                pthread_join(thread_id[0], NULL);
                pthread_join(thread_id[1], NULL);

                controller->MakeRequest(requestGlobal, requestGlobal.GetColor(), TypesOfRequest::Deliver);
                
            }

            if (requestGlobal.GetType() == TypesOfRequest::System)
                state = Disabling;
            else if (requestGlobal.GetType() == TypesOfRequest::Deliver)
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

            // Imitation of capturing camera
            // cv::Mat frame = cv::imread("img_0.jpg");

            cv::Point2i massCenterHead = detector.SteppedDetection(frame, headColorHsv), 
            massCenterTail = detector.SteppedDetection(frame, tailColorHsv), 
            massCenterAverage = detector.GetMassCenter(&massCenterHead, &massCenterTail);
            // detector.SteppedDetection(frame, &massCenterTail, &massCenterHead, &massCenterAverage, tailColorHsv, headColorHsv);


            //Setting destination
            requestGlobal.SetDestination(detector.SteppedDetection(frame, requestGlobal.GetColor()));

            MovementCalculation moveBot;
            angle = moveBot.findAngle(massCenterTail, massCenterHead, requestGlobal.GetDestination());
            distance = moveBot.findDistanceToDestination(massCenterAverage, requestGlobal.GetDestination());
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
            controller->FinishRequest(requestGlobal);
            break;
        }
    }
}
