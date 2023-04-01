#ifndef SUBSCRIBER_H
#define SUBSCRIBER_H

#include <cstring>
#include <stdio.h>
#include <mosquitto.h>
#include <json-c/json.h>
#include <unistd.h>
#include "RequestProcessing.h"

class MosquittoSub
{
private:
    char *MQTT_SERVER = "localhost";
    int KEEP_ALIVE = 60;
    int MQTT_PORT = 1883;
    char *MQTT_SUB_TOPIC = "/alphabot";
    int MQTT_QOS_LEVEL = 2;
    int MSG_MAX_SIZE = 512;

public:
    MosquittoSub() 
    {
    }
    MosquittoSub(char* mqttServer, char* mqttPubTopic) : MQTT_SERVER{mqttServer}, MQTT_SUB_TOPIC{mqttPubTopic}
    {
    }
    void *Subscribe();
    void *Command();
    static void *WrapperSubscribe(void *object);
    static void *WrapperCommand(void *object);
    static void SetState(bool &state, bool mode){state = mode;}
};

namespace MqttSub
{
    Request request;
    void on_connect_cb(struct mosquitto *mosq, void *userdata, int result);
    void on_subscribe_cb(struct mosquitto *mosq, void *userdata, int mid, int qos_count, const int *granted_qos);
    void on_message_cb(struct mosquitto *mosq, void *userdata, const struct mosquitto_message *msg);
}

class MosquittoPub
{
private:
    char *MQTT_SERVER = "localhost";
    int KEEP_ALIVE = 60;
    int MQTT_PORT = 1883;
    char *MQTT_PUB_TOPIC = "/alphabot";
    int MQTT_QOS_LEVEL = 2;
    int MSG_MAX_SIZE = 512;

public:
    MosquittoPub()
    {        
    }
    MosquittoPub(char* mqttServer, char* mqttPubTopic) : MQTT_SERVER{mqttServer}, MQTT_PUB_TOPIC{mqttPubTopic}
    {
    }
    void SendToServer(const char *data);
    void *Publish(const char *message);
};



#endif
