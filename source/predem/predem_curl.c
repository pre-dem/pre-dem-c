#include "predem_curl.h"

#include "cJSON.h"

#include <stdio.h>
#include <string.h>
#include <curl/curl.h>

static char g_domain[128];
static char g_app_key[64];

static char g_app_id[9];

static int inited = 0;

#define SDK_VERSION "0.2.0"

static const char* g_UA = "PREDEM-C/"SDK_VERSION;

static int has_http(char* domain){
    if (strncmp("http://", domain, 7) == 0){
        return 1;
    }
    return 0;
}

#define STRINGLEN 64

typedef struct PREDEM_EnvInfo_Internal {
    char app_name[STRINGLEN];
    char app_version[STRINGLEN]; 

    char device_model[STRINGLEN];
    char manufacturer[STRINGLEN];
    char device_id[STRINGLEN];   
                      
    char os_platform[STRINGLEN];
    char os_version[STRINGLEN]; 

    char sdk_id[STRINGLEN];
    char tag[STRINGLEN];
} PREDEM_EnvInfo_Internal;

static PREDEM_EnvInfo_Internal info = {};

void predem_curl_set_env(PREDEM_EnvInfo inf) {
    if (inf.app_name != NULL) {
        strncpy(info.app_name, inf.app_name, STRINGLEN-1);
    }

    if (inf.app_version != NULL) {
        strncpy(info.app_version, inf.app_version, STRINGLEN-1);
    }

    if (inf.device_model != NULL) {
        strncpy(info.device_model, inf.device_model, STRINGLEN-1);
    }

    if (inf.manufacturer != NULL) {
        strncpy(info.manufacturer, inf.manufacturer, STRINGLEN-1);
    }

    if (inf.device_id != NULL) {
        strncpy(info.device_id, inf.device_id, STRINGLEN-1);
    }

    if (inf.os_platform != NULL) {
        strncpy(info.os_platform, inf.os_platform, STRINGLEN-1);
    }

    if (inf.os_version != NULL) {
        strncpy(info.os_version, inf.os_version, STRINGLEN-1);
    }

    if (inf.sdk_id != NULL) {
        strncpy(info.sdk_id, inf.sdk_id, STRINGLEN-1);
    }

    if (inf.tag != NULL) {
        strncpy(info.tag, inf.tag, STRINGLEN-1);
    }
}

static cJSON* build_event(const char *name, const char* json_string){
    cJSON *root = cJSON_CreateObject();
    cJSON_AddStringToObject(root, "content", json_string);
    cJSON_AddStringToObject(root, "name", name);
    cJSON_AddStringToObject(root, "type", "custom");
    cJSON_AddStringToObject(root, "app_bundle_id", "embed_system");
    cJSON_AddStringToObject(root, "sdk_version", SDK_VERSION);

    if (info.app_name[0] != '\0') {
        cJSON_AddStringToObject(root, "app_name", info.app_name);
    }

    if (info.app_version[0] != '\0') {
        cJSON_AddStringToObject(root, "app_version", info.app_version);
    }

    if (info.device_model[0] != '\0') {
        cJSON_AddStringToObject(root, "device_model", info.device_model);
    }

    if (info.manufacturer[0] != '\0') {
        cJSON_AddStringToObject(root, "manufacturer", info.manufacturer);
    }

    if (info.device_id[0] != '\0') {
        cJSON_AddStringToObject(root, "device_id", info.device_id);
    }

    if (info.os_platform[0] != '\0') {
        cJSON_AddStringToObject(root, "os_platform", info.os_platform);
    }

    if (info.os_version[0] != '\0') {
        cJSON_AddStringToObject(root, "os_version", info.os_version);
    }

    if (info.sdk_id[0] != '\0') {
        cJSON_AddStringToObject(root, "sdk_id", info.sdk_id);
    }

    if (info.tag[0] != '\0') {
        cJSON_AddStringToObject(root, "tag", info.tag);
    }

    return root;
}

void predem_curl_init(const char* domain, const char* app_key) {
    inited = 1;
    memset(g_domain, 0, sizeof(g_domain));
    memset(g_app_key, 0, sizeof(g_app_key));
    memset(g_app_id, 0, sizeof(g_app_id));
    strncpy(g_domain, domain, sizeof(g_domain)-1);
    strncpy(g_app_key, app_key, sizeof(g_app_key)-1);
    memcpy(g_app_id, app_key, 8);
}

PREDEM_CURL_CODE predem_curl_send_event(const char *name, const char* json_string){
    CURL *curl;
    CURLcode res;
    struct curl_slist *list = NULL;
    char url_buff[512];
    if (inited == 0) {
        return PREDEM_CURL_NOT_INIT;
    }

    if (json_string == NULL || name == NULL) {
        return PREDEM_CURL_INVALID_DATA;
    }

    /* get a curl handle */ 
    curl = curl_easy_init();
    if (curl == NULL ) {
        return PREDEM_CURL_NO_MEMORY;
    }

    memset(url_buff, 0, sizeof(url_buff));

    int pos = 0;

    if (has_http(g_domain) == 0) {
        memcpy(url_buff, "http://", 7);
        pos += 7;
    }

    snprintf(url_buff+pos, sizeof(url_buff)-pos, "%s/v2/%s/custom-events", g_domain, g_app_id);

    curl_easy_setopt(curl, CURLOPT_URL, url_buff);

    curl_easy_setopt(curl, CURLOPT_USERAGENT, g_UA);

    list = curl_slist_append(list, "Content-Type: application/json");
 
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, list);

    cJSON* event = build_event(name, json_string);

    const char* c = cJSON_PrintUnformatted(event);

    curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, strlen(c));

    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, c);

    res = curl_easy_perform(curl);

    curl_easy_cleanup(curl);

    curl_slist_free_all(list);

    cJSON_free((void*)c);
    cJSON_Delete(event);
 
    if(res != CURLE_OK) {
        return (PREDEM_CURL_CODE)res;
    }

    return PREDEM_CURL_OK;
}

