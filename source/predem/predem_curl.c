#include "predem_curl.h"

#include "cJSON.h"

#include <stdio.h>
#include <string.h>
#include <curl/curl.h>

static char g_domain[128];
static char g_app_key[64];

static char g_app_id[9];

static int inited = 0;

#define SDK_VERSION "0.0.1"

static const char* g_UA = "PREDEM-C/"SDK_VERSION;

static int has_http(char* domain){
    if (strncmp("http://", domain, 7) == 0){
        return 1;
    }
    return 0;
}

static cJSON* build_event(const char *name, const char* json_string, const PREDEM_EnvInfo* const info){
    cJSON *root = cJSON_CreateObject();
    cJSON_AddStringToObject(root, "content", json_string);
    cJSON_AddStringToObject(root, "name", name);
    cJSON_AddStringToObject(root, "type", "custom");
    cJSON_AddStringToObject(root, "app_bundle_id", "embed_system");
    cJSON_AddStringToObject(root, "sdk_version", SDK_VERSION);

    if (info->app_name != NULL) {
        cJSON_AddStringToObject(root, "app_name", info->app_name);
    }

    if (info->app_version != NULL) {
        cJSON_AddStringToObject(root, "app_version", info->app_version);
    }

    if (info->device_model != NULL) {
        cJSON_AddStringToObject(root, "device_model", info->device_model);
    }

    if (info->manufacturer != NULL) {
        cJSON_AddStringToObject(root, "manufacturer", info->manufacturer);
    }

    if (info->device_id != NULL) {
        cJSON_AddStringToObject(root, "device_id", info->device_id);
    }

    if (info->os_platform != NULL) {
        cJSON_AddStringToObject(root, "os_platform", info->os_platform);
    }

    if (info->os_version != NULL) {
        cJSON_AddStringToObject(root, "os_version", info->os_version);
    }

    if (info->tag != NULL) {
        cJSON_AddStringToObject(root, "tag", info->tag);
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

PREDEM_CURL_CODE predem_curl_send_event(const char *name, const char* json_string, const PREDEM_EnvInfo* const info){
    CURL *curl;
    CURLcode res;
    struct curl_slist *list = NULL;
    char url_buff[512];
    if (inited == 0) {
        return PREDEM_CURL_NOT_INIT;
    }

    if (json_string == NULL || name == NULL || info == NULL) {
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

    cJSON* event = build_event(name, json_string, info);

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

