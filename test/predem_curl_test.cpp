#include "CppUTest/TestHarness.h"

#include "predem_curl.h"

#include "cJSON.h"

#include <curl/curl.h>

#include <stdio.h>
#include <string.h>

TEST_GROUP(PREDEMCURL){
    void setup(){
        curl_global_init(CURL_GLOBAL_ALL);
        predem_curl_init("http://bhk5aaghth5n.predem.qiniuapi.com", "B0vM04UGNMWbSSWsECKYgFlN");
    }

    void teardown(){
        curl_global_cleanup();
    }};

TEST(PREDEMCURL, connect) {
    predem_curl_set_env("sdktest", "0.0.1", "arm", "qiniu", "abcdef", "macos", "999", NULL, __FILE__);

    cJSON* obj = cJSON_CreateObject();
    cJSON_AddStringToObject(obj, "test", "GOGOGO-VIC");

    const char* c = cJSON_PrintUnformatted(obj);

    int res = predem_curl_send_event("testsend", c);
    if (res != PREDEM_CURL_OK) {
        printf("code %d\n", res);
        FAIL("send event failed");
    }

    cJSON_free((void*)c);
    cJSON_Delete(obj);
}
