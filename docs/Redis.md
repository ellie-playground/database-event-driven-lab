# Redis Publish/Subscribe (pub/sub)

* 특정 주제(Topic/Channel)를 구독한 구독자에게 메시지를 발행하는 통신 방법
* publisher가 메시지를 publish하면, 해당 Topic을 구독한 여러 subscriber가 메시지를 받을 수 있는 구조이다.
    * 유튜브 채널 구독을 예시로 들면, 구독과 좋아요(subscribe)를 누르면 나중에 크리에이터가 새로운 영상을 게시(publish)했을 때 구독자에게만 알림이 오게 되는 원리라고 보면 된다.
* Redis는 In-Memory 기반이므로 네트워크 통신이 필요한 웹 소켓을 이용하는 것보다 매우 빠르게 메시지를 주고받을 수 있으나, **구조가 단순해 publish한 메시지는 따로 보관되지 않는다.**
    * 채널을 구독한 모든 구독자에게 알림이 전송되지만, 메시지를 던지는 시스템이므로 메시지를 따로 보관하지 않는다.
    * 반대로, Kafka의 경우 Topic에 publish 되는 메시지를 저장한다.
* 또한, subscriber의 수신 여부 역시 확인하지 않기 때문에 **메시지 유실률이 높다.**
    * 즉, 구독자가 메시지를 받는 것을 보장하지 않아, 구독 대상이 하나도 없는 상황에서 메시지를 publish해도 메시지는 사라진다.
    * 따라서, 메시지를 저장할 필요가 없으며, 100% 전송 보장이 되지 않아도 되는 경우 사용하기 좋다.
* Redis의 pub/sub 기능은 주로 채팅 기능이나 푸시 알림 등에 사용된다.

## 사용 시 주의할 점

* Redis에서 pub/sub으로 메시지를 보내고자 할 때, **반드시 클라이언트가 사전에 채널을 먼저 subscribe 해야한다.**
    * Redis 클라이언트가 Redis 내 채널을 생성하면, 메시지를 보내는 쪽이 해당 채널에 메시지를 publish할 수 있다.
    * 메시지를 저장하지 않기 때문에 구독자가 존재하지 않으면 메시지가 사라지기 때문이다.

## 참고 자료

* https://lucas-owner.tistory.com/60
* https://oliveyoung.tech/2023-08-07/async-process-of-coupon-issuance-using-redis/

# Redis stream

