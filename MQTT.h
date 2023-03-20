#ifndef MQTT_H
#define MQTT_H

#include <cstring>
#include <stdio.h>
#include <mosquitto.h>
#include <json-c/json.h>
#include <unistd.h>

class MosquittoPub
{
    private:
        char *MQTT_SERVER = "localhost";
        int KEEP_ALIVE = 60;
        int MQTT_PORT = 1883;
        char *MQTT_PUB_TOPIC = "/funmqqt";
        int MQTT_QOS_LEVEL = 2;
        int MSG_MAX_SIZE = 512;

    public:
        void SendToServer (const char* data);
        void *Publish (const char* message);
};

class MosquittoSub
{
    private:
        char *MQTT_SERVER = "localhost";
        int KEEP_ALIVE = 60;
        int MQTT_PORT = 1883;
        char *MQTT_SUB_TOPIC = "/funmqqt";
        int MQTT_QOS_LEVEL = 2;
        int MSG_MAX_SIZE = 512;

    public:
        void subscribe();
        void Command();
        
};
bool isConnected = false;
bool isSubscribed = false;
bool isRunning = true;
bool inProcess = false;

void on_connect_cb(struct mosquitto *mosq, void *userdata, int result);
void on_subscribe_cb(struct mosquitto *mosq, void *userdata, int mid, int qos_count, const int *granted_qos);
void on_message_cb(struct mosquitto *mosq, void *userdata, const struct mosquitto_message *msg);


//razdelit classi po faylam
//sub v request

#endif