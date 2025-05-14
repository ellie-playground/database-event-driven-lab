#include <stdio.h>
#include <hiredis/hiredis.h>
#include <sqlite3.h>

int main(void) {
    char query[1024];

    // Redis connection
    redisContext *redis = redisConnect("127.0.0.1", 6379);

    if (redis == NULL || redis->err) {
        if (redis) {
            printf("Error: %s\n", redis->errstr);
        } else {
            printf("Can't allocate redis context\n");
        }
    }

    // SQL connection
    sqlite3* db;

    // :memory: 를 인자로 넘겨주면 인메모리 데이터베이스로 동작한다.
    if (sqlite3_open(":memory:", &db) != SQLITE_OK) {
        printf("Error: Cannot open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        redisFree(redis);
        return -1;
    }

    while (1) {
        printf("Enter SQL Query: ");
        fgets(query, sizeof(query), stdin);

        // 1. 쿼리 실행

        // 2. INSERT, UPDATE, DELETE 여부 확인
        char event_message[512];
        char channel[50];

        if (strncasecmp(query, "INSERT", 6) == 0) {
            char *table = strcasestr(query, "INTO");

            if (table == NULL) {
                printf("Error: You must Input table name. Please try again.\n");
                continue;
            }

            snprintf(event_message, sizeof(event_message), "{\"table\": \"%s\", \"change\": \"insert\"}", table);
            snprintf(channel, sizeof(channel), table);
        } else if (strncasecmp(query, "UPDATE", 6) == 0) {
            char *table = strcasestr(query, "UPDATE");

            if (table == NULL) {
                printf("Error: You must Input table name. Please try again.\n");
                continue;
            }

            snprintf(event_message, sizeof(event_message), "{\"table\": \"%s\", \"change\": \"update\"}", table);
            snprintf(channel, sizeof(channel), table);
        } else if (strncasecmp(query, "DELETE", 6) == 0) {
            char *table = strcasestr(query, "FROM");

            if (table == NULL) {
                printf("Error: You must Input table name. Please try again.\n");
                continue;
            }

            snprintf(event_message, sizeof(event_message), "{\"table\": \"%s\", \"change\": \"delete\"}", table);
            snprintf(channel, sizeof(channel), table);
        }

        redisReply *reply = redisCommand(redis, "PUBLISH %s %s", channel, event_message);
        if (reply == NULL) {
            printf("Can't publish command");
        }

        freeReplyObject(reply);
    }

    redisFree(redis);
}
