#include <stdio.h>
#include <hiredis/hiredis.h>
#include <sqlite3.h>

int main(void) {
    char query[2048];

    // Redis connection
    redisContext *redis = redisConnect("127.0.0.1", 6379);

    if (redis == NULL) {
        printf("Can't allocate redis context\n");
        return -1;
    }

    if (redis->err) {
        printf("Error: %s\n", redis->errstr);
        return -1;
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
        printf("Enter SQL Query (Please input only in uppercase letters): ");
        fgets(query, sizeof(query), stdin);

        // 1. 쿼리 실행

        // 2. INSERT, UPDATE, DELETE 여부 확인
        char event_message[512];
        char channel[50];
        char table[30];

        // 어떤 테이블에 해당 이벤트가 발생했는지 확인
        // TODO: 소문자 고려
        if (strncmp(query, "INSERT INTO ", 12) == 0) {
            sscanf(query + 12, "%s", table);
            snprintf(event_message, sizeof(event_message), "{\"table\": \"%s\", \"change\": \"insert\"}", table);
        } else if (strncmp(query, "UPDATE ", 7) == 0) {
            sscanf(query + 7, "%s", table);
            snprintf(event_message, sizeof(event_message), "{\"table\": \"%s\", \"change\": \"update\"}", table);
        } else if (strncmp(query, "DELETE FROM ", 12) == 0) {
            sscanf(query + 12, "%s", table);
            snprintf(event_message, sizeof(event_message), "{\"table\": \"%s\", \"change\": \"delete\"}", table);
        }

        printf("%s\n\n", event_message);

        redisReply *reply = redisCommand(redis, "PUBLISH %s %s", "apt", event_message);
        if (reply == NULL) {
            printf("Can't publish command");
        }

        freeReplyObject(reply);
    }

    redisFree(redis);
}