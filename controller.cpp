#include "controller.h"
#include <thread>

const double SECONDS_TO_MOVE = 1.0;
const double SPEED_OF_BOT = 0.6;
const Color TAIL_COLOR_LOW(100, 120, 140);
const Color TAIL_COLOR_HIGH(125,255,255);
const Color HEAD_COLOR_LOW(145, 120, 140);
const Color HEAD_COLOR_HIGH(175,255,255);


void Controller::FinishRequest(Request &request)
{
    delete &request;
}

void Controller::Move()
{
    m_mosq->Publish({"forward", SECONDS_TO_MOVE, SPEED_OF_BOT});
}

void Controller::Rotate()
{
    std::string cmd;
    if (m_CosOfAngle > 0)
    {
        if (m_tailPoint.x < m_headPoint.x)
            if (m_tailPoint.y < m_headPoint.y)
                cmd = "left";
            else
                cmd = "right";
        else if (m_tailPoint.y > m_headPoint.y)
            cmd = "left";
        else
            cmd = "right";
    }
    else
    {
        if (m_tailPoint.x < m_headPoint.x)
            if (m_tailPoint.y > m_headPoint.y)
                cmd = "left";
            else
                cmd = "right";
        else if (m_tailPoint.y < m_headPoint.y)
            cmd = "left";
        else
            cmd = "right";
    }
    m_mosq->Publish({cmd.c_str(), SECONDS_TO_MOVE, SPEED_OF_BOT});
}

void Controller::ToHome()
{
    //Здесь нужно формировать запрос на отправление домой
    //Например:
    //m_requestQueue.push(Request(HomePoint));

    std::cout << "going to home.." << std::endl;
}

void WrapperForMosqSub(std::shared_ptr<MyMosquitto> mosq)
{
    mosq->Subscribe();
}

int Controller::ProcessEvents(cv::VideoCapture capture)
{
    m_mosq->Subscribe();
    bool botIsOn = true;
    Mat frame;
    Request *activeRequest;
    Detector detector;
    while (botIsOn)
    {
        switch (m_state)
        {
        case Wait:
        {
            //Формирование запроса для выполнения
            if (!m_requestQueue.empty())
            {
                activeRequest = new Request(m_requestQueue.front());

                assert(activeRequest != nullptr);
                m_requestQueue.pop();
                m_state = Delivery;
                break;
            }

            int flag = 0;
            //Пока не найдётся запрос и не прошло 10 секунд
            while (m_requestQueue.empty() && flag != 10)
            {
                //Проверяем очередь заявок на формирование запроса
                if (!m_mosq->QueueEmpty())
                {
                    // Предполагается, что через mqtt передаются
                    // нижние пороговые значения цвета точки доставки
                    Color DestinationLow(m_mosq->GetNextColor());
                    Color DestinationHigh(DestinationLow[0] + 20, 255, 255);
                    capture >> frame;
                    // frame = imread("target1.png");
                    if (!frame.empty())
                    //Перед формированием запроса клиента нужно будет сформировать запрос
                    //на отправку бота к наполняющей станции
                    //Например:
                    //m_requestQueue.push(Request(StationPoint));
                        m_requestQueue.push(Request(DestinationLow, DestinationHigh, frame));
                    else
                        std::cout << "Frame is incorrect!" << std::endl;
                }
                sleep(1);
                flag++;
            }
            //Если через 10 секунд не найдено заявок или запросов, 
            //то формируем запрос на отправление "Домой"
            if (flag == 10)
                ToHome();

            break;
        }
        case Delivery:
        {
            capture >> frame;
            // frame = imread("target1.png");
            if (frame.empty())
                m_state = EState::Stop;

            // Обнаружение Бота
            m_headPoint = detector.SteppedDetection(frame, HEAD_COLOR_LOW, HEAD_COLOR_HIGH);
            m_tailPoint = detector.SteppedDetection(frame, TAIL_COLOR_LOW, TAIL_COLOR_HIGH);
            m_midPoint = detector.GetMassCenter(m_headPoint, m_tailPoint);
            // Вычисление косинуса угла поворота бота до точки назначения
            m_CosOfAngle = detector.GetAngleToPoint(m_headPoint, m_midPoint, 
                                                    activeRequest->GetDestination());
            // Вычисление дистанции до точки назначения
            m_distanceToDestination = detector.GetDistanceToPoint(m_midPoint,
                                                                  activeRequest->GetDestination());
            // Отображение угла на кадре
            detector.DrawAngle(frame, m_headPoint, m_midPoint, activeRequest->GetDestination());
            // Вывод изменённого кадра
            cv::resize(frame, frame, Size(frame.cols / 3, frame.rows / 3));
            cv::imshow("result", frame);
            cv::waitKey(50);

            // Задаем точность, с которой робот будет доставлять
            float angleAccuracy = 5;
            float angle = acos(m_CosOfAngle)/M_PIf*180; //Перевод в градусы
            int distanceAccuracy = 300;
            // Проверка на точность текущей позиции или угла направления
            if (angle > angleAccuracy)
            {
                std::cout << "Rotate" << std::endl;
                m_state = EState::Rotate;
                break;
            }
            else if (m_distanceToDestination > distanceAccuracy)
            {
                std::cout << "Move" << std::endl;
                m_state = EState::Move;
                break;
            }
            else
            {
                m_state = EState::Wait;
                break;
            }

            break;
        }
        case EState::Move:
            Move();
            m_state = EState::Delivery;
            break;

        case EState::Rotate:
            Rotate();
            m_state = EState::Delivery;
            break;
        case EState::Stop:
        {
            std::cout << "Enter any key to continue. (e)xit." << std::endl;
            char c;
            std::cin >> c;
            if (c == 'e')
                botIsOn = false;
            m_state = EState::Delivery;
            break;
        }
        default:
            return -1;
        }
    }
    return 0;
}