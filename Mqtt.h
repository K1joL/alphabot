#ifndef MYMQTT_H
#define MYMQTT_H

#include <stdio.h>
#include <unistd.h>
#include <mosquittopp.h>
#include <json-c/json.h>
#include <queue>
#include <vector>

class MyMosquitto : public mosqpp::mosquittopp
{
private:
    const char* MQTT_SERVER = "localhost";
    int KEEP_ALIVE = 60;
    int MQTT_PORT = 1883;
    const char *MQTT_SUB_TOPIC = "tgbot/request";
    const char *MQTT_PUB_TOPIC = "abot/command/alex";

    //Переменные состояния
    bool isConnected = false;
    bool isSubscribed = false;
    bool isRunning = true;
    bool inProcess = true;

    virtual void on_connect(int rc);
    virtual void on_subscribe(int mid, int qos_count, const int *granted_qos);
    virtual void on_message(const struct mosquitto_message *msg);
    
    //Очередь цветов запросов
    std::queue<std::vector<uint8_t>> m_colorQueue;

public:
    struct Message
    {
        const char* command;
        double value;
        double speed;
    };

public:
    MyMosquitto(const char *mqttServer, const char *mqttSubTopic, const char *mqttPubTopic,
                const char *id = NULL, bool clean_session = true)
        : mosquittopp(id, clean_session),
          MQTT_SERVER{mqttServer}, MQTT_SUB_TOPIC{mqttSubTopic}, MQTT_PUB_TOPIC{mqttPubTopic}
    {
        mosqpp::lib_init();
        loop_start();
    }

    ~MyMosquitto()
    {
        loop_stop(true);
        mosqpp::lib_cleanup();
    }
    //Returns true if the queue is empty.
    bool QueueEmpty() {return m_colorQueue.empty(); }
    std::vector<uint8_t> GetNextColor();
    void Subscribe();
    void SendToServer(const char* data);
    void Publish(const Message &message);

};


#endif //MYMQTT_H