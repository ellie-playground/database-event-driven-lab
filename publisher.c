#include <stdio.h>
#include <hiredis/hiredis.h>

int main(void)
{
    redisContext *redis = redisConnect("127.0.0.1", 6379);
    if (redis == NULL || redis->err) {
        if (redis) {
            printf("Error: %s\n", redis->errstr);
        } else {
            printf("Can't allocate redis context\n");
        }
    }

    // redis 특정 채널에 메시지 전송
    const char *event_json = "{\"table\": \"board\", \"change\": \"insert\"}";

    redisReply *reply = redisCommand(redis, "PUBLISH %s %s", "apt", event_json);
    if (reply == NULL) {
        printf("Can't publish command");
    }

    freeReplyObject(reply);
    redisFree(redis);
}
