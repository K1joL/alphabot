#include "Subscriber.h"

//States to manage
bool isConnected = false;
bool isSubscribed = false;
bool isRunning = true;
bool inProcess = false;

void MqttSub::on_connect_cb(struct mosquitto *mosq, void *userdata, int result)
{
    if (result != 0)
    {
        fprintf(stderr, "Connect failed\n");
    }
    else
    {
        isConnected = true;
    }
}

void MqttSub::on_subscribe_cb(struct mosquitto *mosq, void *userdata, int mid, int qos_count, const int *granted_qos)
{
    isSubscribed = true;
}

void MqttSub::on_message_cb(struct mosquitto *mosq, void *userdata, const struct mosquitto_message *msg)
{
    if (inProcess)
    {
        printf((char *)msg->payload);
        
        fflush(stdout);
    }
}

void *MosquittoSub::Subscribe()
{
    bool clean_session = true;
    struct mosquitto *mosq = NULL;
    printf( ">> Robot Online <<\n" );
    mosquitto_lib_init();
    mosq = mosquitto_new( NULL, clean_session, NULL );
    
    if( !mosq ) 
    {
        printf( "Create client failed..\n");
        mosquitto_lib_cleanup( );
        return NULL;
    }
    
    mosquitto_connect_callback_set( mosq, MqttSub::on_connect_cb );
    mosquitto_subscribe_callback_set( mosq, MqttSub::on_subscribe_cb );
    mosquitto_message_callback_set( mosq, MqttSub::on_message_cb );

    if (mosquitto_connect(mosq, MQTT_SERVER, MQTT_PORT, KEEP_ALIVE)) 
    {
        fprintf( stderr, "Unable to connect.\n" );
        return NULL;
    }
    
    int result;
    result = mosquitto_loop_start(mosq); 
    
    if (result != MOSQ_ERR_SUCCESS) 
    {
        printf("mosquitto loop error\n");
        return NULL;
    }

    while (!isConnected) 
    {
		usleep(100000);
    }
    
    mosquitto_subscribe( mosq, NULL, MQTT_SUB_TOPIC, MQTT_QOS_LEVEL );
    
    while (!isSubscribed) 
    {
		usleep(100000);
    }


    while(isRunning) 
    {
        usleep(100000);
    }
  
    
    if (mosq)
    {
       mosquitto_destroy(mosq);
    }
    mosquitto_lib_cleanup();
}

void *MosquittoSub::Command()
{
    while(isRunning)
    {
        printf(" >>>> COMMAND LIST <<<<\n");
        printf("    start stop exit    \n"); 
        char stdinbuf[1024];
        printf("\n");
        scanf("%s",stdinbuf);
        printf("\n");

        if(!strcmp(stdinbuf, "exit"))
        {
            isRunning = false;
        }
        else if(!strcmp(stdinbuf, "start"))
        {
            if(inProcess)
            {
                printf("ALREADY START\n");
                printf("\n");
            }
            else
            {
            inProcess = true;
            printf("START RECIEVEING MESSAGE...\n");
            printf("\n");
            }
        }
        else if(!strcmp(stdinbuf, "stop"))
        {
            if(!inProcess)
            {
                printf("ALREADY STOP\n");
                printf("\n");
            }
            else
            {
            inProcess = false;
            printf("STOP RECIEVEING MESSAGE...\n");
            printf("\n");
            }
        }

        else 
        {
            printf("     UNKNOW COMMAND\n");
            printf("\n");
        }
    }
}

void *MosquittoSub::WrapperSubscribe(void *object)
{
    return reinterpret_cast<MosquittoSub*>(object)->Subscribe();
}

void *MosquittoSub::WrapperCommand(void *object)
{
    return reinterpret_cast<MosquittoSub*>(object)->Command();
}

void MosquittoPub::SendToServer(const char *data)
{
    bool clean_session = true;
    struct mosquitto *mosq = NULL;
    mosq = mosquitto_new(NULL, clean_session, NULL);
    mosquitto_connect(mosq, MQTT_SERVER, MQTT_PORT, KEEP_ALIVE);
    mosquitto_publish(mosq, NULL, MQTT_PUB_TOPIC, strlen(data), data, 0, 0);
    mosquitto_destroy(mosq);
}

void *MosquittoPub::Publish(const char *message)
{
    json_object *json_obj;
    json_obj = json_tokener_parse("{}");
    const char *json_str;

    json_object_object_add(json_obj, "Message", json_object_new_string(message));
    json_str = json_object_get_string(json_obj);
    SendToServer(json_str);

    return NULL;
}
