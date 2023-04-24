#ifndef SUBSCRIBER_H
#define SUBSCRIBER_H

#include <cstring>
#include <string>
#include <stdio.h>
#include <mosquitto.h>
#include <json-c/json.h>
#include <unistd.h>
#include <vector>
#include "RequestProcessing.h"

class Request;

class MosquittoSub
{
private:
    const char *MQTT_SERVER = "localhost";
    int KEEP_ALIVE = 60;
    int MQTT_PORT = 1883;
    const char *MQTT_SUB_TOPIC = "/telega";
    int MQTT_QOS_LEVEL = 2;
    int MSG_MAX_SIZE = 512;

    Request returnReq;
public:
    MosquittoSub() 
    {
    }
    MosquittoSub(const char* mqttServer, const char* mqttPubTopic) : MQTT_SERVER{mqttServer}, MQTT_SUB_TOPIC{mqttPubTopic}
    {
    }
    void *Subscribe();
    Request GetReturned();
    static void *WrapperSubscribe(void *object);
    static void SetState(bool &state, bool mode){state = mode;}
};

struct MqttMessage
{
    char* command;
    double seconds;
};

namespace MqttSub
{
    void on_connect_cb(struct mosquitto *mosq, void *userdata, int result);
    void on_subscribe_cb(struct mosquitto *mosq, void *userdata, int mid, int qos_count, const int *granted_qos);
    void on_message_cb(struct mosquitto *mosq, void *userdata, const struct mosquitto_message *msg);
}

class MosquittoPub
{
private:
    const char *MQTT_SERVER = "localhost";
    int KEEP_ALIVE = 60;
    int MQTT_PORT = 1883;
    const char *MQTT_PUB_TOPIC = "/alphabot";
    int MQTT_QOS_LEVEL = 2;
    int MSG_MAX_SIZE = 512;

public:
    MosquittoPub()
    {        
    }
    MosquittoPub(const char* mqttServer, const char* mqttPubTopic) : MQTT_SERVER{mqttServer}, MQTT_PUB_TOPIC{mqttPubTopic}
    {
    }
    void SendToServer(const char *data);
    void *Publish(const MqttMessage* message);
};


#endif
