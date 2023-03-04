#include "opencv2/opencv.hpp"
#include <iostream>
#include <math.h>
#include <queue>

using namespace std;
using namespace cv;

struct Color
{
    uint8_t Hue = 0;
    uint8_t Saturation = 0;
    uint8_t Value = 0;
    Color(const Color & other)
    {
        this->Hue = other.Hue;
        this->Saturation = other.Saturation;
        this->Value = other.Value;
    }
    Color(uint8_t H, uint8_t S, uint8_t V)
    {
        Hue = H;
        Saturation = S;
        Value = V;
    }
    Color &operator = (Color& other)
    {
        this->Hue = other.Hue;
        this->Saturation = other.Saturation;
        this->Value = other.Value;
        return *this;
    }

}BlueHSV(100,90,70), MagentaHSV(167, 80, 90);

class MassCenter
{
    public:
        int X;
        int Y;
        MassCenter(Rect Rectangle);
        MassCenter(MassCenter Center1, MassCenter Center2);
};

class MovementCalculation
{
    public:
        float AngleInRadian;
        int DistanceInPixel;
        void findAngle(MassCenter blob1, MassCenter blob2, Point2i Destination);
        void findDistanceToDestination(MassCenter AverageCenter, Point2i Destination);
        float GetAngle(){ return AngleInRadian; };
        int GetDistance(){ return DistanceInPixel; };

};

enum class TypesOfRequest 
{
    System = -1, 
    None = 0, 
    Deliver
    };
class Request
{
    public:
        Point2i Destination {960,1280};
        TypesOfRequest Type;
        Color ColorPuf{0,0,0};
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
        TypesOfRequest GetType(){ return Type; }
};

class Controller
{
    public:
        void MakeRequest(Request &Req, Color ColorPuf, TypesOfRequest Type)
        {
            Req.ColorPuf = ColorPuf;
            Req.Type = Type;
        }
        void FinishRequest(Request &Req)
        {
            Req.Type = TypesOfRequest::None;
        }
        void Move(int Distance)
        {
            cout << "Moving " << Distance << endl;
            cout << "Movement Completed!" << endl;
        }
        void Rotate(float Angle)
        {
            cout << "Rotating to " << Angle*180/M_PIf << endl;
            cout << "Rotation Completed!" << endl;
        }
        void GoHome()
        {
            cout << "Going Home.." << endl;
        }
};

MassCenter::MassCenter(Rect Rectangle)
{
    Point2i TopLeftPointRect = Rectangle.tl();
    Point2i BottomRightPointRect = Rectangle.br();
    this->X = (TopLeftPointRect.x + BottomRightPointRect.x)/2;
    this->Y = (TopLeftPointRect.y + BottomRightPointRect.y)/2;
}

MassCenter::MassCenter(MassCenter Center1, MassCenter Center2)
{
    this->X = (Center1.X + Center2.X)/2;
    this->Y = (Center1.Y + Center2.Y)/2;
}

void MovementCalculation::findAngle(MassCenter blob1, MassCenter blob2, Point2i Dest)
{
    int CenterX = int(blob1.X + blob2.X)/2;
    int CenterY = int(blob1.Y + blob2.Y)/2;
    this -> AngleInRadian = static_cast<float>(acos(((blob1.X - CenterX) * (CenterX - Dest.x) + (blob1.Y - CenterY) * (CenterY - Dest.y))/
                            (sqrt(pow((blob1.X - CenterX), 2) + pow((blob1.Y - CenterY), 2)) * sqrt(pow((CenterX - Dest.x), 2) + pow((CenterY - Dest.y), 2)))));
}

void MovementCalculation::findDistanceToDestination(MassCenter AverageCenter, Point2i Dest)
{
    this -> DistanceInPixel = sqrt(pow((AverageCenter.X - Dest.x),2) + pow((AverageCenter.Y - Dest.y),2));
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

Mat TakeThresholdOfBlob(Mat FrameHSV, Color Color)
{
    Mat Threshold;
    //vectors of HSV color that need to find. Make a range using Offset
    uint8_t Offset = 5;
    vector <uint8_t> HsvMin = {static_cast<uint8_t>(Color.Hue - Offset),100,100};
    vector <uint8_t> HsvMax = {static_cast<uint8_t>(Color.Hue + Offset),255,255};
    
    //Make result image matrix with found color
    inRange(FrameHSV, HsvMin, HsvMax, Threshold);
    //Creating the image in white&black with area of needed color 
    // imwrite( "FrameResult.jpg", Threshold );
    return Threshold;
}

Rect detectBlob(Mat Threshold)
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
        if(area > 10'000)
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

int main()
{
    enum States{
        Running = 1,
        DoDeliver,
        Move,
        Rotate,
        Waiting,
        Disabling
    }State {Running};

    Request Request;
    Controller Controller;
    Mat Frame, FrameHSV, Threshold;
    Rect RectangleOfMagenta, RectangleOfBlue;
    MovementCalculation MoveBot;

    MoveBot.AngleInRadian = -0.685;
    MoveBot.DistanceInPixel = 555;
    // VideoCapture cap("Cap");
    //Deviation for if`s
    float ErrorAngle = 0.1;
    int ErrorDist = 100;
    
    while(true)
    {
        switch(State)
        {
            case Running:
            {
                //Making a request
                char c;
                cin >> c;
                if(c=='d')
                {
                    int Hue = 0, Saturation = 0, Value = 0;
                    cout << "Hue: ";
                    cin >> Hue;
                    cout << "Saturation: ";
                    cin >> Saturation;
                    cout << "Value: ";
                    cin >> Value;

                    Color ColorPuf(Hue, Saturation, Value);
                    
                    Controller.MakeRequest(Request, ColorPuf, TypesOfRequest::Deliver);
                }

                if(Request.GetType() == TypesOfRequest::System)
                    State = Disabling;
                else if(Request.GetType() == TypesOfRequest::Deliver)
                    State = DoDeliver;
                else
                    Controller.GoHome();
                break;
            }
            case DoDeliver:
            {
            // Capture frame-by-frame
                // cap >> frame;
            
            // If the frame is empty, break immediately
                // if (Frame.empty())
                //   State = Running;

            //Imitation of capturing camera

                Frame = imread("img_0.jpg");
                
            //converting the Frame to FrameHsv color model
                cvtColor(Frame, FrameHSV, COLOR_BGR2HSV);
                

                Threshold = TakeThresholdOfBlob(FrameHSV, MagentaHSV);
                RectangleOfMagenta = detectBlob(Threshold);
            //just for tests
                // cout << RectangleOfMagenta << endl;

                Threshold = TakeThresholdOfBlob(FrameHSV,BlueHSV);
                Rect RectangleOfBlue = detectBlob(Threshold);
            //just for tests
                // cout << RectangleOfBlue << endl;

                MassCenter MCBlue(RectangleOfBlue);
                MassCenter MCMagenta(RectangleOfMagenta);
                MassCenter MCAverage(MCBlue, MCMagenta);    
                
                // MoveBot.findAngle(MCBlue, MCMagenta, Request.Destination);
                // MoveBot.findDistanceToDestination(MCAverage, Request.Destination);
                if(MoveBot.GetAngle() > ErrorAngle || MoveBot.GetAngle() < (-ErrorAngle))
                    {
                        State = Rotate;
                        cout << "ifRotate";
                        break;
                    }

                if(MoveBot.GetDistance() > ErrorDist)
                    {
                        State = Move;
                        break;
                    }
                    else
                    {
                        State = Waiting;
                        break;
                    }

                break;
            }

            case Move:
                State = DoDeliver;
                Controller.Move(MoveBot.GetDistance());
                MoveBot.DistanceInPixel = 0;
                break;

            case Rotate:
                State = DoDeliver;
                Controller.Rotate(MoveBot.GetAngle());
                MoveBot.AngleInRadian = 0;
                break;

            case Waiting:
                State = Running;
                Controller.FinishRequest(Request);
                break;
        }
    }

    return 0;
}
