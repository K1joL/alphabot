#include "Subscriber.h"
#include <iostream>

using namespace std;

int main()
{
    MosquittoPub pub;

    while(true)
    {
        pub.Publish("(60,100,40)");
        cout << "sended" << endl;
        sleep(5);
        pub.SendToServer("exit");
    }
    mosquitto_lib_cleanup();
    return 0;
}
