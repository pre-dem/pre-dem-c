#include "CppUTest/TestHarness.h"

#include "predem_curl.h"

#include "cJSON.h"

#include <stdio.h>
#include <string.h>

TEST_GROUP(PREDEMCURL){
    void setup(){
        predem_curl_init("http://bhk5aaghth5n.predem.qiniuapi.com", "B0vM04UGNMWbSSWsECKYgFlN");
    }

    void teardown(){}};

TEST(PREDEMCURL, connect) {
    PREDEM_EnvInfo* info = (PREDEM_EnvInfo*)malloc(sizeof(PREDEM_EnvInfo));
    info->app_name = "sdktest";
    info->app_version = "0.0.1";
    info->device_model = "arm";
    info->manufacturer = "qiniu";
    info->device_id = "abcdef";
    info->os_platform = "macos";
    info->os_version = "999";
    info->tag = __FILE__;

    cJSON* obj = cJSON_CreateObject();
    cJSON_AddStringToObject(obj, "test", "GOGOGO");

    const char* c = cJSON_PrintUnformatted(obj);

    int res = predem_curl_send_event("testsend", c, info);
    if (res != PREDEM_CURL_OK) {
        printf("code %d\n", res);
        FAIL("send event failed");
    }

    free(info);
    cJSON_free((void*)c);
    cJSON_Delete(obj);
}

// TEST_GROUP(SOCKET){
//     void setup(){}

//     void teardown(){}};

// TEST(SOCKET, create) {
//     net_address ai = NULL;
//     int r = net_getaddrinfo("www.baidu.com", 80, SOCK_TCP, &ai);
//     LONGS_EQUAL(0, r);
//     if (ai == NULL) {
//         FAIL("get addrinfo NULL");
//         return;
//     }
//     struct sock_ret ret = socket_addrinfo_new(ai);
//     if (ret.value <= 0) {
//         printf("%d\n", ret.error_code);
//         FAIL("create socket failed");
//     } else {
//         socket_close(ret.value);
//     }

//     net_freeaddrinfo(ai);
//     return;
// }
