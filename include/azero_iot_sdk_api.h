/**
 *  SoundAI Azero IOT SDK APIs
 *
 *  Copyright (C) 2020 SoundAI.com
 *  All Rights Reserved.
 */

#ifndef SOUNDAI_AZERO_IOT_API_H
#define SOUNDAI_AZERO_IOT_API_H

#ifndef SAI_API
    #if defined(_WIN32) && defined(DLL_EXPORT)
        #define SAI_API __declspec(dllexport)
    #elif defined(_WIN32)
        #define SAI_API __declspec(dllimport)
    #elif defined(__GNUC__) && defined(DLL_EXPORT)
        #define SAI_API __attribute__((visibility("default")))
    #else
        #define SAI_API
    #endif
#endif

#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>

#if defined(__cplusplus)
extern "C" 
{
#endif

/**
 * @ingroup mqtt_datatypes_enums
 * @brief Types of MQTT operations.
 *
 * The function @ref mqtt_function_operationtype can be used to get an operation
 * type's description.
 */
typedef enum azero_iot_mqtt_operation_type
{
    AZERO_IOT_MQTT_CONNECT,           /**< Client-to-server CONNECT. */
    AZERO_IOT_MQTT_PUBLISH_TO_SERVER, /**< Client-to-server PUBLISH. */
    AZERO_IOT_MQTT_PUBACK,            /**< Client-to-server PUBACK. */
    AZERO_IOT_MQTT_SUBSCRIBE,         /**< Client-to-server SUBSCRIBE. */
    AZERO_IOT_MQTT_UNSUBSCRIBE,       /**< Client-to-server UNSUBSCRIBE. */
    AZERO_IOT_MQTT_PINGREQ,           /**< Client-to-server PINGREQ. */
    AZERO_IOT_MQTT_DISCONNECT         /**< Client-to-server DISCONNECT. */
} azero_iot_mqtt_operation_type_e;

typedef enum azero_iot_mqtt_qos
{
    AZERO_IOT_MQTT_QOS_0 = 0, /**< Delivery at most once. */
    AZERO_IOT_MQTT_QOS_1 = 1, /**< Delivery at least once.*/
    AZERO_IOT_MQTT_QOS_2 = 2  /**< Delivery exactly once. */
} azero_iot_mqtt_qos_e;

#define AZERO_IOT_MQTT_MSG_INITIALIZER     { .pTopicName = NULL, .topicNameLength = 0, .pPayload = NULL, .payloadLength = 0}

typedef struct mqtt_payload_msg
{
    const char * pTopicName;          /**< @brief Topic name of PUBLISH. no longer than 23  */
    uint16_t topicNameLength;         /**< @brief Length of pTopicName. */

    const void * pPayload;            /**< @brief Payload of PUBLISH. */
    uint16_t payloadLength;             /**< @brief Length ofpPayload.*/
}mqtt_payload_msg_t;

typedef struct azero_iot_mqtt_info
{
    union
    {
        struct
        {
            
            const char* operationType;      /* string of @ref azero_iot_mqtt_operation_type_e */
            const char* operationResult;    /* `SUCCESS` or other error string */
        }operation; /* Operation complete callbacks. */

        mqtt_payload_msg_t subscrib; /* subscribed topics's callbacks. */
    } u;

}azero_iot_mqtt_info_t;

typedef void ( *mqttInfoCallback )( azero_iot_mqtt_info_t info );


typedef enum disconnect_reason
{
    AZERO_IOT_MQTT_DISCONNECT_CALLED,   /** @ref azero_iot_handler_release was invoked. */
    AZERO_IOT_MQTT_BAD_PACKET_RECEIVED, /** An invalid packet was received from the network. */
    AZERO_IOT_MQTT_KEEP_ALIVE_TIMEOUT,  /** Keep-alive response was not received. */
    AZERO_IOT_MQTT_NET_POLL_TIMEOUT     /** net layer pull data timeout**/
} disconnect_reason_t;

typedef void (*disconnectReasonCallback)( disconnect_reason_t reason);

typedef enum shadow_data_type
{
    AZERO_IOT_SHADOW_GET,       /** Get param saved in server. */
    AZERO_IOT_SHADOW_DELTA,     /** App ask to change device's state. */
    AZERO_IOT_SHADOW_DOCUMENT   /** Server update param success, send previous and current state. */
} shadow_data_type_t;

/**
 * @brief shadow's param callback
 */

typedef void ( *shadowParamCallback )( shadow_data_type_t type, const char *pDocument, size_t documentLength);

/**
 * result callback of @ref azero_iot_shadow_update
 */
typedef void ( *shadowUpdateResultCallback )( const char* result);

typedef struct azero_iot_callback
{
    mqttInfoCallback                operation_complete;
    mqttInfoCallback                subscrib_message;
    disconnectReasonCallback        disconnect;
    shadowParamCallback             param;
    shadowUpdateResultCallback      update_result;
} azero_iot_callback_t;

typedef struct azero_device_info
{
    char* phostname;        /* host name */
    uint16_t port;          /* port of host name */
    char* accountprefix;    /* created in azero iot web */
    char* pIdentifier;      /* device's clientId, must be unique, with azero server, combined accountprefix-sn */
    char* pThingName;       /* device's name, must be unique */
    bool  inputCertPath;    /* not support file read, make it false and input document directly, otherwise input path and make it true*/
    char* pRootCa;          /* trusted server root certificate*/
    char* pClientCert;      /* path of *cert.pem */
    char* pPrivateKey;      /* path of *private.key */
    char** pAttributeName;  /* such as color, brightness*/
    uint16_t attributeSize; /* size of attribute */
}azero_device_info_t;

typedef void * azero_iot_handler_t;

/**
 * @brief Create iot SDK handler.
 * @param [in] device_info, cbs must be global variable, SDK will use the info later
 * 
 * @return NULL if failed, other when success.
 */
SAI_API azero_iot_handler_t azero_iot_handler_create( azero_device_info_t* device_info, azero_iot_callback_t* cbs );

/**
 * @brief Destory iot SDK handler.
 * @param [in] handler
 */
SAI_API void azero_iot_handler_release( azero_iot_handler_t handler );

/**
 * @brief Start iot SDK.
 * @param [in] azero_iot_handler_t
 * @param [in] azero_iot_mqtt_operation_type_e, only use AZERO_IOT_MQTT_SUBSCRIBE and AZERO_IOT_MQTT_UNSUBSCRIBE.
 * @param [in] topics, a array of string
 * @param [in] qos, a array of azero_iot_mqtt_qos_e, using with same pos topic
 * @param [in] size of topics and qos
 * 
 * @return EXIT_SUCCESS when success, EXIT_FAILURE otherwise.
 */
SAI_API int azero_iot_mqtt_modify_subscriptions( azero_iot_handler_t handler,
                                                azero_iot_mqtt_operation_type_e operation, 
                                                const char ** topics, 
                                                const azero_iot_mqtt_qos_e qos[], 
                                                const int topicSize );

/**
 * @brief Publish msg async.
 * @param [in] azero_iot_mqtt_msg.
 *
 * @return EXIT_SUCCESS when publish success, EXIT_FAILURE if failed.
 */
SAI_API int  azero_iot_mqtt_publish_async( azero_iot_handler_t handler, 
                                            mqtt_payload_msg_t msg );

/**
 * @brief Publish msg sync.
 * @param [in] azero_iot_mqtt_msg.
 * @param [in] timeout microseconds, default 5000ms.
 *
 * @return EXIT_SUCCESS when publish success, EXIT_FAILURE if failed.
 */
SAI_API int  azero_iot_mqtt_publish_sync( azero_iot_handler_t handler, 
                                            mqtt_payload_msg_t msg,
                                            uint32_t timeoutMs );

/**
 * @brief Retrieves the latest state stored from server
 * 
 * * @return EXIT_SUCCESS when get success, EXIT_FAILURE if failed.
 */
SAI_API int azero_iot_shadow_get( azero_iot_handler_t handler);

/**
 * @brief Update shadow state async.
 * @brief Suggest reporting state with this method.
 * @param [in] pUpdateDocument, such as `{"color":"red"}`  or  `{"color":"red","power":"on"}`.
 * @param [in] resposeDesire, if is a response for desire.
 *             if device change the state after @ref changeDeviceStateDocumentCallback, 
 *             if true; otherwise is false.
 *
 * @return EXIT_SUCCESS when publish success, EXIT_FAILURE if failed.
 */
SAI_API int azero_iot_shadow_update( azero_iot_handler_t handler, const char* pUpdateDocument, bool resposeDesire );

/**
 * @brief Clear shadow document.
 * 
 */
SAI_API void azero_iot_shadow_clear( azero_iot_handler_t handler);

/**
 * @brief Parses a key in the pDeltaDocument.
 *
 * @param[in] pDeltaDocument The Shadow delta document to parse.
 * @param[in] deltaDocumentLength The length of `pDeltaDocument`.
 * @param[in] pDeltaKey The key in the delta document to find. Must be NULL-terminated.
 * @param[out] pDelta Set to the first character in the delta key.
 * @param[out] pDeltaLength The length of the delta key.
 *
 * @return `true` if the given delta key is found; `false` otherwise.
 */
SAI_API bool azero_iot_parser_value( const char * pDeltaDocument,
                                        size_t deltaDocumentLength,
                                        const char * pDeltaKey,
                                        const char ** pDelta,
                                        size_t * pDeltaLength );

/**
 * @brief Get the SDK version info.
 *
 * @return the SDK version info.
 */
SAI_API const char* azero_iot_get_sdk_version(void);

#if defined(__cplusplus)
}
#endif
#endif //SOUNDAI_AZERO_IOT_API_H