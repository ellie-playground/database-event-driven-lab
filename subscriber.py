import redis
import json

def main():
    r = redis.Redis(host="127.0.0.1", port=6379, decode_responses=True)
    pubsub = r.pubsub()
    pubsub.subscribe("apt")


    print("Wait to Event...")

    for message in pubsub.listen():
        if message["type"] == "message":
            print(json.loads(message["data"]))

    
    print("Success!")

if __name__ == "__main__":
    main()
