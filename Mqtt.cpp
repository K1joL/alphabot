#include "mqtt.h"
#include <iostream>
#include <string.h>

void MyMosquitto::on_connect(int rc)
{
    if (rc != 0)
    {
        std::cout << stderr << "Connect failed\n";
    }
    else
    {
        isConnected = true;
    }
}

void MyMosquitto::on_subscribe(int mid, int qos_count, const int *granted_qos)
{
    isSubscribed = true;
}

void MyMosquitto::on_message(const mosquitto_message *msg)
{
    std::string RecievedMsg = static_cast<std::string>(static_cast<char *>(msg->payload));
    if (RecievedMsg.compare("exit") != 0)
    {
        std::vector<uint8_t> color;
        std::string temp = "";
        for (int i = 0; i < RecievedMsg.length(); i++)
        {
            if (RecievedMsg[i] == '(')
            {
                i++;
                while (RecievedMsg[i] != ')')
                {
                    if (RecievedMsg[i] == ',')
                    {
                        color.push_back(stod(temp));
                        temp = "";
                        i++;
                        continue;
                    }
                    temp += RecievedMsg[i];
                    i++;
                }
                color.push_back(stod(temp));
                break;
            }
        }
        m_colorQueue.push(color);
    }
    else
        isRunning = false;

    fflush(stdout);
}

std::vector<uint8_t> MyMosquitto::GetNextColor()
{
    std::vector<uint8_t> temp = move(m_colorQueue.front());
    m_colorQueue.pop();
    return temp;
}

void MyMosquitto::Subscribe()
{
    if (connect_async(MQTT_SERVER, MQTT_PORT, KEEP_ALIVE)) 
    {
        std::cout << stderr << "Unable to connect.\n";
        return ;
    }

    while (!isConnected) 
    {
		usleep(100000);
    }
    
    subscribe( NULL, MQTT_SUB_TOPIC);
    
    while (!isSubscribed) 
    {
		usleep(100000);
    }

    while(isRunning) 
    {
        usleep(100000);
    }

    return ;
}

void MyMosquitto::SendToServer(const char *data)
{
    connect_async(MQTT_SERVER, MQTT_PORT, KEEP_ALIVE);
    publish(NULL, MQTT_PUB_TOPIC, strlen(data), data);
}

void MyMosquitto::Publish(const Message &message)
{
    json_object *json_obj;
    json_obj = json_tokener_parse("{}");
    const char *json_str;
    
    json_object_object_add(json_obj, "cmd", json_object_new_string(message.command));
    json_object_object_add(json_obj, "val", json_object_new_double(message.value));
    json_object_object_add(json_obj, "spd", json_object_new_double(message.speed));

    json_str = json_object_get_string(json_obj);
    SendToServer(json_str);

    return ;
}
