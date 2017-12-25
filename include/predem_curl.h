#ifndef PREDEM_CURL_H
#define PREDEM_CURL_H

#ifdef __cplusplus
extern "C" {
#endif

/*
返回码
*/
typedef enum {
  PREDEM_CURL_OK = 0,            /* 正常 */
  PREDEM_CURL_NOT_INIT = -1,     /* 没有调用 init 函数 */
  PREDEM_CURL_INVALID_DATA = -2, /* 传入错误的数据*/
  PREDEM_CURL_NO_MEMORY = -3     /* 内存不足 */
} PREDEM_CURL_CODE;

/*
初始化用来上报的配置
domain: 从 predem.qiniu.com 的 app 设置页面获取的域名
app_key: 从 predem.qiniu.com 的 app 设置页面获取的key
*/

void predem_curl_init(const char *domain, const char *app_key);

typedef struct PREDEM_EnvInfo {
  const char *app_name;    /*app 名称, 可选*/
  const char *app_version; /*app 版本, 可选*/

  const char *device_model; /*机型, 可选*/
  const char *manufacturer; /* 制造商, 可选*/
  const char *device_id;    /* 设备id,可选, 和sdk_id
                               尽量要有一个配置好,便于后期查找终端*/

  const char *os_platform; /* 操作系统, 可选*/
  const char *os_version;  /* 操作系统版本, 可选*/

  const char *sdk_id; /* 每个设备上的app id, 建议生成文件存下来, 下次读取*/
  const char *tag; /* 关联的自定义标志,下次读取 比如app 用户id, 可选*/
} PREDEM_EnvInfo;

/**
设置APP环境信息, 建议配置上, 便于在服务端进行查找
PREDEM_EnvInfo info 系统环境信息
*/

void predem_curl_set_env(PREDEM_EnvInfo info);

/*
发送自定义事件到服务端
name: 事件名称, 不能为NULL
json_string: 事件内容, 已经序列化为json string, 不能为NULL
info: 环境信息, 不能为NULL

return: PREDEM_CURL_CODE 和 CURL error code
*/

PREDEM_CURL_CODE predem_curl_send_event(const char *name,
                                        const char *json_string);

#ifdef __cplusplus
}
#endif

#endif
