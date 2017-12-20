#ifndef PREDEM_CURL_H
#define PREDEM_CURL_H

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
  PREDEM_CURL_OK = 0,
  PREDEM_CURL_NOT_INIT = -1,
  PREDEM_CURL_INVALID_DATA = -2,
  PREDEM_CURL_NO_MEMORY = -3
} PREDEM_CURL_CODE;

typedef struct PREDEM_EnvInfo{
    const char* app_name;
    const char* app_version;

    const char* device_model;
    const char* manufacturer;
    const char* device_id;

    const char* os_platform;
    const char* os_version;

    const char* sdk_id;
    const char* tag;

} PREDEM_EnvInfo;

void predem_curl_init(const char *domain, const char *app_key);

PREDEM_CURL_CODE predem_curl_send_event(const char *name, const char* json_string, const PREDEM_EnvInfo* const info);

#ifdef __cplusplus
}
#endif

#endif
