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

* Redis 5.0부터 추가된 자료구조로, log 파일처럼 append only로 저장되는 구조를 가지고 있다.
    * append-only란 새롭게 데이터를 추가하는 목적으로 사용되는 로그를 의미한다.
    * 이 로그에 저장된 데이터는 DB에 새로운 데이터를 추가할 수 있지만 기존의 데이터를 변경하지는 못한다.
* 메시징 시스템인 Kafka와 비슷하게 동작하며, 메시지를 읽는 Consumer와 여러 Consumer를 관리하는 Consumer Group 또한 지원한다.
* pub/sub과 비교하면, Redis stream은 메시지 전달 후 사라지는 것이 아니고 수신 여부를 구분지을 수 있다.
* 또한, 다중 Consumer 존재 시 Redis pub/sub과 달리 모든 Consumer에게 메시지를 전달하는 것이 아니라 각각 나눠서 처리 가능하다.
* 각 스트림 항목에 대한 고유 ID를 생성하고, 이를 바탕으로 관련 항목을 검색하거나 처리한다.
    * 이 때 생성되는 ID는 시간과 관련되어 있다.

## 구조상의 유의점

### 처리 순서

* 하나의 stream을 여러 consumer가 병렬 처리하는 구조로 동작한다.
    * consumer 그룹 당 single stream이 존재한다.
    * 전통적인 대기열과 같은 구조이다.
* 이 구조의 특징은 **처리 순서를 보장할 수 없다는 것이다.**
    * 각 consumer가 스트림에서 가져온 데이터를 처리완료하는 시점은 충분히 다를 수 있기 때문이다.
    * 즉, stream에 순차적으로 쌓인 데이터를 병럴처리한다는 개념이다.

### In-Memory

* stream의 메시지는 아래와 같은 세 가지 상태를 가지고 있다.
    * DELIVERED: 메시지가 stream에 전달된 상태
    * PENDING: 메시지가 consumer에 전달된 상태 (consumer가 stream에서 읽은 상태)
    * ACK: 메시지가 consumer에 의해 처리 완료된 상태 (consumer가 stream에 ACK을 보내야만 한다.)
* stream은 메시지가 유실되지 않도록 처리 완료되지 않은 메시지를 보관하는 리스트 자료 구조인 PEL(Pending Entry List)을 가지고 있다.
    * 만약 ACK을 받지 못해서 PENDING 상태로 남은 메시지가 점점 쌓이면 메모리 이슈를 발생시킬 가능성이 높다.

## 동시 처리와 재처리

* 중복 없이 동시 처리가 가능한 이유는 consumer 그룹이 single stream을 두고 메시지를 각 consumer가 하나씩 꺼내 처리하기 때문이다.
* 재처리가 가능한 이유는 메시지마다 id를 두고 관리해, ACK 명령을 통해 메시지 처리 완료를 지원하고, 만약 ACK을 받지 못한 메시지는 PENDING처리하기 때문이다.

## 참고 자료

* https://kingjakeu.github.io/page2/
* https://velog.io/@ddings73/Redis-Streams
* https://splendidlolli.tistory.com/762
