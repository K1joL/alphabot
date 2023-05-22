#include "Mqtt.h"

void myMosq::on_connect(int rc)
{
    if (rc != 0)
    {
        fprintf(stderr, "Connect failed\n");
    }
    else
    {
        isConnected_ = true;
    }
}

void myMosq::on_subscribe(int mid, int qos_count, const int *granted_qos)
{
    isSubscribed_ = true;
}

void myMosq::on_message(const struct mosquitto_message *msg)
{
    std::string RecievedMsg = static_cast<std::string>(static_cast<char *>(msg->payload));
    if (RecievedMsg.compare("exit") != 0)
    {
        std::vector<double> colorTemp;
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
                        colorTemp.push_back(stod(temp));
                        temp = "";
                        i++;
                        continue;
                    }
                    temp += RecievedMsg[i];
                    i++;
                }
                colorTemp.push_back(stod(temp));
                break;
            }
        }
        colorRequest_ = move(colorTemp);
    }
    else
        isRunning_ = false;

    fflush(stdout);
}

void myMosq::Subscribe()
{
    printf( ">> Robot Online <<\n" );

    if (connect_async(MQTT_SERVER, MQTT_PORT, KEEP_ALIVE)) 
    {
        fprintf( stderr, "Unable to connect.\n" );
        return ;
    }

    while (!isConnected_) 
    {
		usleep(100000);
    }
    
    subscribe( NULL, MQTT_SUB_TOPIC);
    
    while (!isSubscribed_) 
    {
		usleep(100000);
    }


    while(isRunning_) 
    {
        usleep(100000);
    }

    return ;
}


void myMosq::SendToServer(const char *data)
{
    connect_async(MQTT_SERVER, MQTT_PORT, KEEP_ALIVE);
    publish(NULL, MQTT_PUB_TOPIC, strlen(data), data, 0, 0);
}

void *myMosq::Publish(const MqttMessage* message)
{
    json_object *json_obj;
    json_obj = json_tokener_parse("{}");
    const char *json_str;

    json_object_object_add(json_obj, "cmd", json_object_new_string(message->command));
    json_object_object_add(json_obj, "val", json_object_new_double(message->seconds));
    json_object_object_add(json_obj, "spd", json_object_new_double(0.6));

    json_str = json_object_get_string(json_obj);
    SendToServer(json_str);

    return NULL;
}