#include "opencv2/opencv.hpp"
#include "opencv2/viz/types.hpp"
#include <iostream>
#include <math.h>
#include <queue>

using namespace std;
using namespace cv;
using namespace viz;

Point2i GetMassCenter(Rect Rectangle)
{
    Point2i TopLeftPointRect = Rectangle.tl();
    Point2i BottomRightPointRect = Rectangle.br();
    int X = (TopLeftPointRect.x + BottomRightPointRect.x)/2;
    int Y = (TopLeftPointRect.y + BottomRightPointRect.y)/2;
    return Point2i(X, Y);
}

Point2i GetMassCenter(Point2i *Center1, Point2i *Center2)
{
    int X = (Center1->x + Center2->x) / 2;
    int Y = (Center1->y + Center2->y) / 2;
    return Point2i(X, Y);
}

Mat TakeThresholdOfBlob(const Mat &FrameHSV, Color Color)
{
    Mat Threshold;
    //vectors of HSV color that need to find. Make a range using Offset
    uint8_t Offset = 5;
    vector <uint8_t> HsvMin = {static_cast<uint8_t>(Color[0] - Offset),100,100};
    vector <uint8_t> HsvMax = {static_cast<uint8_t>(Color[0] + Offset),255,255};
    
    //Make result image matrix with found color
    inRange(FrameHSV, HsvMin, HsvMax, Threshold);
    //Creating the image in white&black with area of needed color 
    // imwrite( "FrameResult.jpg", Threshold );
    return Threshold;
}

Rect detectBlob(const Mat &Threshold)
{
    Rect Rectangle;
    vector<vector<Point>> Contours;
    vector<Vec4i> Hierarchy;

    findContours(Threshold, Contours, RETR_TREE, CHAIN_APPROX_SIMPLE);
    
    vector<RotatedRect> minRect(Contours.size());
    for(size_t i = 0; i < Contours.size(); i++)
            minRect[i] = minAreaRect(Contours[i]);
    
    for(size_t i = 0; i < Contours.size(); i++)
    {
        int area = minRect[i].size.height*minRect[i].size.width;
        int minArea = 10'000;
        if(area > minArea)
        {
            Rectangle = minRect[i].boundingRect();
            //Uncomment if you want to see Rectangles on image
            // Point2f rect_points[4];
            // minRect[i].points( rect_points );
            // for ( int j = 0; j < 4; j++ )
            // {
            //     line( Threshold, rect_points[j], rect_points[(j+1)%4], (0,0,255) );
            // }
        }
    }
    // imshow("Result", Threshold);
    // waitKey();
    return Rectangle;
}

void SteppedDetection(const Mat &Frame, Point2i *MassCenter1, Point2i *MassCenter2, Point2i *MassCenterAverage, const Color &ColorHsv1, const Color &ColorHsv2)
{
    Mat FrameHSV, Threshold;

    // converting the Frame to FrameHsv color model
    cvtColor(Frame, FrameHSV, COLOR_BGR2HSV);

    Threshold = TakeThresholdOfBlob(FrameHSV, ColorHsv1);
    Rect RectangleOfColor = detectBlob(Threshold);
    *MassCenter1 = GetMassCenter(RectangleOfColor);
    // just for tests
    //  cout << RectangleOfMagenta << endl;

    Threshold = TakeThresholdOfBlob(FrameHSV, ColorHsv2);
    RectangleOfColor = detectBlob(Threshold);
    *MassCenter2 = GetMassCenter(RectangleOfColor);
    // just for tests
    //  cout << RectangleOfBlue << endl;
    *MassCenterAverage = GetMassCenter(MassCenter1, MassCenter2);
}

class MovementCalculation
{
    public:
        float findAngle(Point2i MassCenter1, Point2i MassCenter2, Point2i Destination);
        int findDistanceToDestination(Point2i AverageCenter, Point2i Destination);
};

enum class TypesOfRequest 
{
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
        Point2i Destination {960,1280};
        TypesOfRequest Type;
        Color ColorPuf{0,0,0};
    public:
        Request()
        {
            this->Type = TypesOfRequest::None;
        }
        Request(Color ColorPuf, TypesOfRequest Type = TypesOfRequest::None)
        {
            this->Type = Type;
            this->ColorPuf = ColorPuf;
        }
        Request &operator = (Request &req)
        {
            this->ColorPuf = req.ColorPuf;
            this->Type = req.Type;
            return *this;
        }
        void SetColor(Color Color){ this -> ColorPuf = Color; }
        void SetDestination(Point2i Point){ this -> Destination = Point; }
        void SetType(TypesOfRequest Type){ this -> Type = Type; }
        Color GetColor(){ return ColorPuf; }
        Point2i GetDestination(){ return Destination; }
        TypesOfRequest GetType(){ return Type; }
};

class Controller
{
    private:
    Color BlueHsv{100, 0, 0};
    Color MagentaHsv{166, 0, 0};

    public:
        void MakeRequest(Request &Req, Color ColorPuf, TypesOfRequest Type)
        {
            Req.SetColor(ColorPuf);
            Req.SetType(Type);
        }
        void FinishRequest(Request &Req)
        {
            Req.SetType(TypesOfRequest::None);
        }
        void Move(int &Distance)
        {
            cout << "Moving " << Distance << endl;
            Distance = 0;
            cout << "Movement Completed!" << endl;
        }
        void Rotate(float &Angle)
        {
            cout << "Rotating to " << Angle*180/M_PIf << endl;
            Angle = 0;
            cout << "Rotation Completed!" << endl;
        }
        void GoHome()
        {
            cout << "Going Home.." << endl;
        }
        void FiniteAutomate(States &State, Request &Request)
        {
            float Angle = 0;
            int Distance = 0;
            while (true)
            {
                Controller *Controller = this;
                switch (State)
                {
                    case States::Running:
                    {
                        // Making a request
                        char c;
                        cin >> c;
                        if (c == 'd')
                        {
                            cout << "Enter color of ottoman" << endl;
                            int Hue = 0, Saturation = 0, Value = 0;
                            cout << "Hue: ";
                            cin >> Hue;
                            cout << "Saturation: ";
                            cin >> Saturation;
                            cout << "Value: ";
                            cin >> Value;

                            Color ColorPuf(Hue, Saturation, Value);

                            Controller->MakeRequest(Request, ColorPuf, TypesOfRequest::Deliver);
                        }

                        if (Request.GetType() == TypesOfRequest::System)
                            State = Disabling;
                        else if (Request.GetType() == TypesOfRequest::Deliver)
                            State = DoDeliver;
                        else
                            Controller->GoHome();
                        break;
                    }
                    case States::DoDeliver:
                    {
                        // VideoCapture Cap("0");
                        // Capture frame-by-frame
                        // Cap >> Frame;

                        // If the frame is empty, break immediately
                        // if (Frame.empty())
                        //   State = Running;

                        // Imitation of capturing camera
                        Mat Frame = imread("img_0.jpg");

                        Point2i MassCenterBlue, MassCenterMagenta, MassCenterAverage;

                        SteppedDetection(Frame, &MassCenterBlue, &MassCenterMagenta, &MassCenterAverage, BlueHsv, MagentaHsv);

                        MovementCalculation MoveBot;
                        Angle = MoveBot.findAngle(MassCenterBlue, MassCenterMagenta, Request.GetDestination());
                        Distance = MoveBot.findDistanceToDestination(MassCenterAverage, Request.GetDestination());
                        cout << "Angle : " << Angle << " Distance: " << Distance << endl;

                        // Deviation for if`s
                        float ErrorAngle = 0.1;
                        int ErrorDist = 100;
                        if (Angle > ErrorAngle || Angle < (-ErrorAngle))
                        {
                            State = States::Rotate;
                            break;
                        }
                        else if (Distance > ErrorDist)
                        {
                            State = States::Move;
                            break;
                        }
                        else
                        {
                            State = Waiting;
                            break;
                        }

                        break;
                    }

                    case States::Move:
                        State = DoDeliver;
                        Controller->Move(Distance);
                        break;

                    case States::Rotate:
                        State = DoDeliver;
                        Controller->Rotate(Angle);
                        break;

                    case States::Waiting:
                        // while(CupOnTheBot())
                        // {
                        //     if(CloseToClient())
                        //         cout << "Take your drink!" << endl;
                        //     if(CloseToDispenser())
                        //         cout << "The drink is being poured!" << endl;
                        // }
                        State = Running;
                        Controller->FinishRequest(Request);
                        break;
                }
            }
        }
};

float MovementCalculation::findAngle(Point2i MassCenter1, Point2i MassCenter2, Point2i Dest)
{
    int CenterX = int(MassCenter1.x + MassCenter2.x)/2;
    int CenterY = int(MassCenter1.y + MassCenter2.y)/2;
    float AngleInRadian = static_cast<float>(acos(((MassCenter1.x - CenterX) * (CenterX - Dest.x) + (MassCenter1.y - CenterY) * (CenterY - Dest.y))/
                            (sqrt(pow((MassCenter1.x - CenterX), 2) + pow((MassCenter1.y - CenterY), 2)) * sqrt(pow((CenterX - Dest.x), 2) + pow((CenterY - Dest.y), 2)))));
    return AngleInRadian;
}

int MovementCalculation::findDistanceToDestination(Point2i AverageCenter, Point2i Dest)
{
    int DistanceInPixel = sqrt(pow((AverageCenter.x - Dest.x),2) + pow((AverageCenter.y - Dest.y),2));
    return DistanceInPixel;
}

void SetColor(Mat Frame)
{
    namedWindow("Result");
    namedWindow("Settings");
    createTrackbar("h1", "Settings", 0, 255);
    createTrackbar("s1", "Settings", 0, 255);
    createTrackbar("v1", "Settings", 0, 255);
    createTrackbar("h2", "Settings", 0, 255);
    createTrackbar("s2", "Settings", 0, 255);
    createTrackbar("v2", "Settings", 0, 255);
    while(true)
    {
        Mat FrameHsv;
        cvtColor(Frame, FrameHsv, COLOR_BGR2HSV);
        int h1 = getTrackbarPos("h1", "Settings");
        int s1 = getTrackbarPos("s1", "Settings");
        int v1 = getTrackbarPos("v1", "Settings");
        int h2 = getTrackbarPos("h2", "Settings");
        int s2 = getTrackbarPos("s2", "Settings");
        int v2 = getTrackbarPos("v2", "Settings");
        vector <int> HsvMin = {h1, s1, v1};
        vector <int> HsvMax = {h2, s2, v2};
        Mat thresh;
        inRange(FrameHsv, HsvMin, HsvMax, thresh);
        imshow("Result", thresh);

        char ch = waitKey(1000);
        if (ch == 27)
            break;
    }
    destroyAllWindows;
}



int main()
{
    Request Request;
    Controller Controller;
    Rect RectangleOfMagenta, RectangleOfBlue;

    //Turning on the Bot
    States State = Running;
    

    Controller.FiniteAutomate(State, Request);

    return 0;
}
