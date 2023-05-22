#ifndef SUBSCRIBER_H
#define SUBSCRIBER_H

#include <cstring>
#include <string>
#include <stdio.h>
#include <mosquittopp.h>
#include <json-c/json.h>
#include <unistd.h>
#include <vector>

class myMosq : public mosqpp::mosquittopp
{
public:
    struct MqttMessage
    {
        const char *command;
        double seconds;
    };

private:
    const char *MQTT_SERVER = "localhost";
    const char *MQTT_SUB_TOPIC = "/telega";
    const char *MQTT_PUB_TOPIC = "abot/command/alex";
    int KEEP_ALIVE = 60;
    int MQTT_PORT = 1883;

    std::vector<double> colorRequest_;

    bool isConnected_ = false;
    bool isSubscribed_ = false;
    bool isRunning_ = true;
    bool inProcess_ = true;

    virtual void on_connect(int rc);
    virtual void on_subscribe(int mid, int qos_count, const int *granted_qos);
    virtual void on_message(const struct mosquitto_message *msg);

public:
    myMosq(const char* mqttServer, const char* mqttSubTopic, const char* mqttPubTopic, const char *id = NULL, bool clean_session = true)
     : mosquittopp(id, clean_session),
        MQTT_SERVER{mqttServer}, MQTT_SUB_TOPIC{mqttSubTopic}, MQTT_PUB_TOPIC{mqttPubTopic}
    {
        mosqpp::lib_init();
        loop_start();
    }

    ~myMosq()
    {
        loop_stop(true);
        mosqpp::lib_cleanup();
    }

    void Subscribe();
    void SendToServer(const char *data);
    void *Publish(const MqttMessage* message);
    std::vector<double> GetMessage() const { return colorRequest_; }
};

#endif
