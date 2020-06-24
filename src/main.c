#include "azero_iot_sdk_api.h"
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

static azero_iot_handler_t handler = NULL;
static void startiot();
/* 设备支持的控制策略, 和Azero IoT平台控制策略一致 */
static char *state_name[] = {"brightness","lightMode"};
static int   state_size = 2;

#define ProductID        "speaker_sai"         /* 一类设备的product Id */
#define DeviceSN         "AE:DE:C2:61:3F:18"   /* 本设备的SN, 本类设备中每个设备唯一, 可以为MAC或其他唯一标识 */
#define AccountPrefix    "32141iz1awewfq"      /* 一类设备的标识, IoT平台注册产生 */

// 断开连接后的重连处理
static bool reconnecting = false;
static void * _reconnectThread(void* args)
{
    pthread_detach(pthread_self());
    reconnecting = true;
    azero_iot_handler_release(handler);
    handler = NULL;
    startiot();
    sleep(2);
    reconnecting = false;
    return NULL;
}

//断开连接的回调
static void disconnect_cb(disconnect_reason_t reason)
{
    printf("disconnect reason %d\n",reason);
    if(reason == AZERO_IOT_MQTT_KEEP_ALIVE_TIMEOUT | AZERO_IOT_MQTT_NET_POLL_TIMEOUT){
        if(false == reconnecting)
        {
            reconnecting = true;
            pthread_t ptid;
            pthread_create(&ptid, NULL, _reconnectThread,NULL);
        }
    }
}

static void deal_data_cb(const char *key, const char* value,size_t size, bool need_report)
{
    printf("get state= %s, value = %.*s,need report = %s\n",key,size,value,need_report?"true":"false");

    if(need_report){
        char *format = "{\"%s\":%.*s}";
        size_t length = strlen(format) + size + strlen(key);
        char data[length+1];
        sprintf(data,format,key,size,value);
        /* 端上状态上报, 如果是本地主动更改的状态, 最后一个参数传false, APP端发起的改变, 最后一个参数传true */
        azero_iot_shadow_update(handler,data,true);
    }
}

/*shadow服务接收到的消息回调*/
static void shadow_data_cb( shadow_data_type_t type, const char *pDocument, size_t documentLength)
{
    printf("shadow msg cb: %.*s\n",documentLength,pDocument);
    switch (type)
    {
    case AZERO_IOT_SHADOW_GET:
    case AZERO_IOT_SHADOW_DELTA:
        {
            bool stateFound = false, keyFound = false;
            const char * pState = NULL;
            size_t stateLength = 0;

            /* 查找json消息中是否有"state" key. */
            stateFound = azero_iot_parser_value( pDocument,
                                                    documentLength,
                                                    "state",
                                                    &pState,
                                                    &stateLength );
            if(!stateFound)
            {
                printf("pDocument is error\n");
                return;
            }
            /* 查找是否有已经定义好的策略 */
            printf("state = %.*s\n",stateLength,pState);
            for(int i=0; i<state_size; i++){
                
                const char * pData = NULL;
                size_t dataLength = 0;
                const char * keyName = state_name[i];
                keyFound = azero_iot_parser_value( pState,
                                                    stateLength,
                                                    keyName,
                                                    &pData,
                                                    &dataLength );
                printf(" find key = %s, result = %d\n",state_name[i],keyFound);
                if(keyFound)
                {
                    /* 对于get到的消息, 需要设备改变状态至相应的状态, 和IoT平台最新消息保持一致,
                    一般用于设备开机或者断网重连, 对APP端做出的改变进行同步;
                    delta 消息为APP下发控制指令后收到的消息, 需要将设备改变至相应状态, 同时将改变后状态上报IoT平台,
                    平台更新最新状态.
                    */
                    deal_data_cb(keyName,pData,dataLength,type==AZERO_IOT_SHADOW_DELTA);
                }
            }

            break;
        }
    case AZERO_IOT_SHADOW_DOCUMENT:
        {
            /* update后云端当前状态和上一个状态的通知, 可以依次查看是否上报错误 */
            break;
        }
    }
}

/* shadow 内容update结果回调, 成功或者失败*/
static void update_result_cb(const char* result)
{
    printf("update result is %s\n",result);
}

static void startiot()
{
    while(handler == NULL){
        /* global val, SDK will use this info later */
        static azero_iot_callback_t cbs = { .disconnect = disconnect_cb,\
                                            .param = shadow_data_cb,\
                                            .update_result = update_result_cb};
        
        static azero_device_info_t device_info = { .phostname = AccountPrefix".iot-dev.bj.soundai.cn",\ 
                                                    /* IoT 平台产生域名 */
                                                    .port = 8883,\
                                                    .accountprefix = AccountPrefix,\
                                                    .pIdentifier = AccountPrefix"-"DeviceSN,\
                                                    /* pIdentifier 每个设备唯一, 前缀必须为AccountPrefix */
                                                    .pThingName = ProductID""DeviceSN,\
                                                    /* pThingName 每个设备唯一, 必须为ProductIDDeviceSN拼接 */
                                                    /* IoT 平台产生以下key */
                                                    .inputCertPath = true,\
                                                    .pRootCa = "1323214-ca.crt",\
                                                    .pClientCert = "1323214-cert.pem",\
                                                    .pPrivateKey = "1323214-private.key.pem"};
        device_info.pAttributeName = state_name;
        device_info.attributeSize = state_size;
        for(int i=0;i<state_size;i++)
        {
            printf("pAttributeName[%d] = %s\n",i,device_info.pAttributeName[i]);
        }

        handler = azero_iot_handler_create(&device_info,&cbs);
        sleep(2);        
    }
}

int main()
{
    startiot();

    azero_iot_shadow_get(handler);
    while(1){
        sleep(rand()%50+5);
        /* azero_iot_shadow_update(handler,"{\"brightness\":33}",false);*/
    };
    /*azero_iot_shadow_clear(handler);
    清空IoT 平台所有状态, 用于恢复出厂设置或者重置
    */
    /* 正常退出, 释放资源 */
    azero_iot_handler_release(handler);
    handler = NULL;
}
