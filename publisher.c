#include <stdio.h>

int main(void) {
    redisContext *redis = redisConnect("127.0.0.1", 6379);

    if (redis == NULL) {
        printf("Can't allocate redis context\n");
        return -1;
    }
    
    if (redis->err) {
        printf("Error: %s\n", redis->err);
        return -1;
    }

    redisReply *reply = redisCommand(redis, "")
}