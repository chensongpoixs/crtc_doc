标签：sdp nack 90000 fb fmtp rtpmap 详解 rtcp webrtc


# 1、SDP协议简介

SDP(Session Description Protocol) 是一种会话描述协议，基于文本，其本身并不属于传输协议，需要依赖其它的传输协议（比如 SIP 和 HTTP）来交换必要的媒体信息，用于两个会话实体之间的媒体协商，详细的标注规范可参考RFC2327和RFC4566。WebRTC 中使用 Offer-Answer 模型交换 SDP，本文详细介绍SDP各属性在WebRTC中的应用。

# 2、SDP组成

SDP是由多行文本组成的一个纯文本协议，如果将SDP从语义上分解成不同组件来描述一个多媒体会话信息，那么SDP主要由以下部分组成：会话信息、网络信息、媒体信息、安全信息、服务质量和分组信息、DTLS角色、ICE策略等，具体的如下图所示：

SDP属性.png

# 3、offer SDP

```
offer sdp：
v=0
//version：sdp协议版本号，值固定为0
o=- 5558452382581391502 2 IN IP4 127.0.0.1
//origin：会话的发起者

user name：没有用“-”代替
session id：会话id
session version：会话版本，如果在会话过程中有改变编码之类的操作，重新生成sdp时,sess-id不变，sess-version加1
net type：Internet
addr type：IP4 or IP6
addr：IP地址
s=-
//Session name：会话名称，没有用“-”替代
t=0 0
//t（timing）=
webrtc始终是0,0代表没有限制
a=group:BUNDLE 0 1
// 表示需要共用一个传输通道传输的媒体，通过ssrc进行区分不同的流。如果没有这一行，音视频数据就会分别用单独udp端口来发送。也可表示为a=group:BUNDLE audio video。
a=extmap-allow-mixed
Chrome自从M71版本就开始支持SDP协议属性extmap-allow-mixed，但是如果提供了extmap-allow-mixed，M71之前版本Chrome的SDP协商将会失败。从Chrome M89版本开始，extmap-allow-mixed 将被默认提供。
a=msid-semantic: WMS（a=msid-semantic: WMS live/123）
msid：media stream id
WMS：WebRTC Media Stream
m=audio 9 UDP/TLS/RTP/SAVPF 111 103 104 9 0 8 106 105 13 110 112 113 126
Media Descriptions：媒体描述
m= …
audio：表示音频
9：代表音频使用端口9来传输，但是在webrtc中现在一般不使用，如果设置为0，代表不传输音频。
UDP/TLS/RTP/SAVPF：表示用户支持来传输音频的协议，udp,tls,rtp代表使用udp，来传输rtp包，并使用tls加密。
SAVPF：secret audio video protocol family，表示使用srtcp的反馈机制来控制通信过程。
111…126：表示本会话音频支持的编码
c=IN IP4 0.0.0.0
connection：表示要用来接收或者发送音频使用的IP地址，webrtc使用ice传输，不使用这个地址，该属性webrtc并没有使用。
a=rtcp:9 IN IP4 0.0.0.0
用来传输rtcp的地址和端口，webrtc中不使用
a=ice-ufrag:mlU2
ICE short-term 认证算法用到的用户名
a=ice-pwd:LefAeJLo7QQba6T17xMv4gVb
ICE short-term 认证算法用到的密码
a=ice-options:trickle
支持trickle，即sdp里面只描述媒体信息, ice候选项的信息另行通知。即说明 SDP 中没有包含 candidate 信息，Candidate 是通过信令单独交换的，这样可以做到 Connectivity checks 和 Candidate harvesting 并行处理，提高会话建立的速度。
a=fingerprint:sha-256 42:3B:E0:D9:3D:CD:85:1C:2C:30:BD:D9:45:5A:B1:B2:79:92:80:DD:C1:16:03:59:56:87:2A:06:D1:39:9B:31
dtls协商过程中需要的认证信息,sha-256加密算法。即fingerprint 是 DTLS 过程中的 Certificate 证书的签名，防止客户端和服务器的证书被篡改。
a=setup:actpass
setup：指的是 DTLS 的角色，也就是谁是 DTLS Client(active)，谁是 DTLS Server(passive)，如果自己两个都可以那就是 actpass。这里我们是 actpass，那么就要由对方在 Answer 中确定最终的 DTLS 角色。
a=mid:0
表示media的名字，用于查找具体的media。前面BUNDLE行中用到的媒体标识。
a=extmap:1 urn:ietf:params:rtp-hdrext:ssrc-audio-level
a=extmap:2 http://www.webrtc.org/experiments/rtp-hdrext/abs-send-time
a=extmap:3 http://www.ietf.org/id/draft-holmer-rmcat-transport-wide-cc-extensions-01
a=extmap:4 urn:ietf:params:rtp-hdrext:sdes:mid
a=extmap:5 urn:ietf:params:rtp-hdrext:sdes:rtp-stream-id
a=extmap:6 urn:ietf:params:rtp-hdrext:sdes:repaired-rtp-stream-id
RTP 扩展项，接收者可以自己解码获取里面的数据
a=recvonly
媒体流的方向有四种，分别是 sendonly、recvonly、sendrecv、inactive，它们既可以出现在会话级别描述中也可以出现在媒体级别的描述中。
sendonly ：表示只发送数据，比如客户端推流到 SFU，那么会在自己的 Offer(or Answer) 中携带 senonly 属性
revonly ：表示只接收数据，比如客户端向 SFU 订阅流，那么会在自己的 Offer(or Answer) 中携带 recvonly 属性
sendrecv ：表示可以双向传输，比如客户端加入到视频会议中，既要发布自己的流又要订阅别人的流，那么就需要在自己的 Offer(or Answer) 中携带 sendrecv 属性
inactive ：表示禁止发送数据，比如在基于 RTP 的视频会议中，主持人暂时禁掉用户 A 的语音，那么用户 A 的关于音频的媒体级别描述应该携带 inactive 属性，表示不能再发送音频数据。
a=rtcp-mux
指出rtp,rtcp包使用同一个端口来传输，即RTCP 和 RTP 复用，表示 Sender 使用一个传输通道（单一端口）发送 RTP 和 RTCP
a=rtpmap:111 opus/48000/2
rtpmap: / [/]
对m=audio这一行的媒体编码补充说明，指出了编码采用的编号，采样率，声道等信息
a=rtcp-fb:111 transport-cc
说明opus编码支持使用rtcp来控制拥塞，参考https://tools.ietf.org/html/draft-holmer-rmcat-transport-wide-cc-extensions-01
a=fmtp:111 minptime=10;useinbandfec=1
对opus编码可选的补充说明,minptime代表最小打包时长是10ms，useinbandfec=1代表使用opus编码内置fec特性
a=rtpmap:103 ISAC/16000
a=rtpmap:104 ISAC/32000
a=rtpmap:9 G722/8000
a=rtpmap:0 PCMU/8000
a=rtpmap:8 PCMA/8000
a=rtpmap:106 CN/32000
a=rtpmap:105 CN/16000
a=rtpmap:13 CN/8000
a=rtpmap:110 telephone-event/48000
a=rtpmap:112 telephone-event/32000
a=rtpmap:113 telephone-event/16000
a=rtpmap:126 telephone-event/8000
m=video 9 UDP/TLS/RTP/SAVPF 96 97 98 99 100 101 122 102 121 127 120 125 107 108 109 35 36 124 119 123 118 114 115 116 37
c=IN IP4 0.0.0.0
a=rtcp:9 IN IP4 0.0.0.0
a=ice-ufrag:mlU2
a=ice-pwd:LefAeJLo7QQba6T17xMv4gVb
a=ice-options:trickle
a=fingerprint:sha-256 42:3B:E0:D9:3D:CD:85:1C:2C:30:BD:D9:45:5A:B1:B2:79:92:80:DD:C1:16:03:59:56:87:2A:06:D1:39:9B:31
a=setup:actpass
a=mid:1
a=extmap:14 urn:ietf:params:rtp-hdrext:toffset
a=extmap:2 http://www.webrtc.org/experiments/rtp-hdrext/abs-send-time
a=extmap:13 urn:3gpp:video-orientation
a=extmap:3 http://www.ietf.org/id/draft-holmer-rmcat-transport-wide-cc-extensions-01
a=extmap:12 http://www.webrtc.org/experiments/rtp-hdrext/playout-delay
a=extmap:11 http://www.webrtc.org/experiments/rtp-hdrext/video-content-type
a=extmap:7 http://www.webrtc.org/experiments/rtp-hdrext/video-timing
a=extmap:8 http://www.webrtc.org/experiments/rtp-hdrext/color-space
a=extmap:4 urn:ietf:params:rtp-hdrext:sdes:mid
a=extmap:5 urn:ietf:params:rtp-hdrext:sdes:rtp-stream-id
a=extmap:6 urn:ietf:params:rtp-hdrext:sdes:repaired-rtp-stream-id
a=recvonly
a=rtcp-mux
a=rtcp-rsize
// 尽可能的减少rtcp包的发送，只发丢包
a=rtpmap:96 VP8/90000
//payload type的描述,编码器VP8,时钟采样率90000
a=rtcp-fb:96 goog-remb
// 对rtpmap 96的描述,google标准的接收端带宽评估
a=rtcp-fb:96 transport-cc
// 对rtpmap 96的描述,传输端的带宽评估
a=rtcp-fb:96 ccm fir
对rtpmap 96的描述,支持客户端请求i帧
ccm：codec control using RTCP feedback message
fir：Full Intra Request
a=rtcp-fb:96 nack
// 支持丢包重传
a=rtcp-fb:96 nack pli
// 支持i帧重传
a=rtpmap:97 rtx/90000
// rtx丢包重传
RTX 表示是重传，比如 video 的 97，就是 apt=96 的重传。也就是说如果用的是 97 这个编码格式，它是在 96(VP8) 基础上加了重传功能
a=fmtp:97 apt=96
// apt关联
a=rtpmap:98 VP9/90000
a=rtcp-fb:98 goog-remb
a=rtcp-fb:98 transport-cc
a=rtcp-fb:98 ccm fir
a=rtcp-fb:98 nack
a=rtcp-fb:98 nack pli
a=fmtp:98 profile-id=0
a=rtpmap:99 rtx/90000
a=fmtp:99 apt=98
a=rtpmap:100 VP9/90000
a=rtcp-fb:100 goog-remb
a=rtcp-fb:100 transport-cc
a=rtcp-fb:100 ccm fir
a=rtcp-fb:100 nack
a=rtcp-fb:100 nack pli
a=fmtp:100 profile-id=2
a=rtpmap:101 rtx/90000
a=fmtp:101 apt=100
a=rtpmap:122 VP9/90000
a=rtcp-fb:122 goog-remb
a=rtcp-fb:122 transport-cc
a=rtcp-fb:122 ccm fir
a=rtcp-fb:122 nack
a=rtcp-fb:122 nack pli
a=fmtp:122 profile-id=1
a=rtpmap:102 H264/90000
a=rtcp-fb:102 goog-remb
a=rtcp-fb:102 transport-cc
a=rtcp-fb:102 ccm fir
a=rtcp-fb:102 nack
a=rtcp-fb:102 nack pli
a=fmtp:102 level-asymmetry-allowed=1;packetization-mode=1;profile-level-id=42001f
//fmtp详解参见：https://blog.csdn.net/epubcn/article/details/102802108
level-asymmetry-allowed：表示是否允许两端编码的Level不一致。注意必须两端的SDP中该值都为1才生效。

packetization-mode：表示图像数据包分拆发送的方式
0: Single NAL (Network Abstraction Layer)，每帧图像数据全部放在一个NAL单元传送；
1: Not Interleaved，每帧图像数据被拆放到多个NAL单元传送，这些NAL单元传送的顺序是按照解码的顺序发送；
2: Interleaved，每帧图像数据被拆放到多个NAL单元传送，但是这些NAL单元传送的顺序可以不按照解码的顺序发送
实际上，只有I帧可以被拆分发送，P帧和B帧都不能被拆分发送。所以如果packetization-mode=1，则意味着I帧会被拆分发送。

profile-level-id：3字节的16进制数，是H.264 SPS信息的头三个字节，用来标识可支持的H.264的profile和level。

a=rtpmap:121 rtx/90000
a=fmtp:121 apt=102
a=rtpmap:127 H264/90000
a=rtcp-fb:127 goog-remb
a=rtcp-fb:127 transport-cc
a=rtcp-fb:127 ccm fir
a=rtcp-fb:127 nack
a=rtcp-fb:127 nack pli
a=fmtp:127 level-asymmetry-allowed=1;packetization-mode=0;profile-level-id=42001f
a=rtpmap:120 rtx/90000
a=fmtp:120 apt=127
a=rtpmap:125 H264/90000
a=rtcp-fb:125 goog-remb
a=rtcp-fb:125 transport-cc
a=rtcp-fb:125 ccm fir
a=rtcp-fb:125 nack
a=rtcp-fb:125 nack pli
a=fmtp:125 level-asymmetry-allowed=1;packetization-mode=1;profile-level-id=42e01f
a=rtpmap:107 rtx/90000
a=fmtp:107 apt=125
a=rtpmap:108 H264/90000
a=rtcp-fb:108 goog-remb
a=rtcp-fb:108 transport-cc
a=rtcp-fb:108 ccm fir
a=rtcp-fb:108 nack
a=rtcp-fb:108 nack pli
a=fmtp:108 level-asymmetry-allowed=1;packetization-mode=0;profile-level-id=42e01f
a=rtpmap:109 rtx/90000
a=fmtp:109 apt=108
a=rtpmap:35 AV1X/90000
a=rtcp-fb:35 goog-remb
a=rtcp-fb:35 transport-cc
a=rtcp-fb:35 ccm fir
a=rtcp-fb:35 nack
a=rtcp-fb:35 nack pli
a=rtpmap:36 rtx/90000
a=fmtp:36 apt=35
a=rtpmap:124 H264/90000
a=rtcp-fb:124 goog-remb
a=rtcp-fb:124 transport-cc
a=rtcp-fb:124 ccm fir
a=rtcp-fb:124 nack
a=rtcp-fb:124 nack pli
a=fmtp:124 level-asymmetry-allowed=1;packetization-mode=1;profile-level-id=4d001f
a=rtpmap:119 rtx/90000
a=fmtp:119 apt=124
a=rtpmap:123 H264/90000
a=rtcp-fb:123 goog-remb
a=rtcp-fb:123 transport-cc
a=rtcp-fb:123 ccm fir
a=rtcp-fb:123 nack
a=rtcp-fb:123 nack pli
a=fmtp:123 level-asymmetry-allowed=1;packetization-mode=1;profile-level-id=64001f
a=rtpmap:118 rtx/90000
a=fmtp:118 apt=123
a=rtpmap:114 red/90000
a=rtpmap:115 rtx/90000
a=fmtp:115 apt=114
a=rtpmap:116 ulpfec/90000
a=rtpmap:37 flexfec-03/90000
a=rtcp-fb:37 goog-remb
a=rtcp-fb:37 transport-cc
a=fmtp:37 repair-window=10000000

4、answer SDP
v=0
o=SRS/4.0.122(Leo) 17222016 2 IN IP4 0.0.0.0
s=SRSPlaySession
t=0 0
a=ice-lite
// ice模式: full / lite, 设置lite表示始终为controlled,不需要发起STUN binding探测，只需要回复 binding response, 在开发SFU服务时非常有用, 只需要服务器一方设置为lite (都为full时, 双向探测；都为lite时, 互不探测)
a=group:BUNDLE 0 1
a=msid-semantic: WMS live/123
m=audio 9 UDP/TLS/RTP/SAVPF 111
c=IN IP4 0.0.0.0
a=ice-ufrag:2x232387
a=ice-pwd:989644lp9q1846743n17nisj62wtwuo5
a=fingerprint:sha-256 E0:69:30:D8:42:A0:78:A8:9C:24:5E:AC:1A:54:BF:1E:5A:E9:4F:2E:FE:88:45:9D:60:DA:20:DC:01:F5:52:99
a=setup:passive
a=mid:0
a=sendonly
a=rtcp-mux
a=rtcp-rsize
a=rtpmap:111 opus/48000/2
a=ssrc:44151474 cname:9x0x3z0bg443m3if
a=ssrc:44151474 label:audio-289f15o9
a=candidate:0 1 udp 2130706431 172.20.1.161 8000 typ host generation 0
m=video 9 UDP/TLS/RTP/SAVPF 125
c=IN IP4 0.0.0.0
a=ice-ufrag:2x232387
a=ice-pwd:989644lp9q1846743n17nisj62wtwuo5
a=fingerprint:sha-256 E0:69:30:D8:42:A0:78:A8:9C:24:5E:AC:1A:54:BF:1E:5A:E9:4F:2E:FE:88:45:9D:60:DA:20:DC:01:F5:52:99
a=setup:passive
a=mid:1
a=sendonly
a=rtcp-mux
a=rtcp-rsize
a=rtpmap:125 H264/90000
a=fmtp:125 level-asymmetry-allowed=1;packetization-mode=1;profile-level-id=42e01f
a=ssrc:44151475 cname:9x0x3z0bg443m3if
a=ssrc:44151475 label:video-j181o097
a=candidate:0 1 udp 2130706431 172.20.1.161 8000 typ host generation 0

host 主机候选者，主机自己可以提供。优先级高
srflx 反射候选者，STUN提供。优先级中
relay 中继候选者，TURN提供。优先级低
```


# 4、SDP中的几个重要属性

## 1、Bundle

媒体数据传输时，复用媒体通道，音频和视频数据采用统一通道。音频和视频复用时，最后只会用一个 Candidate 传输。即最终 audio 和 video 尽管可能有独立的 Candidate，但是如果对方也是 BUNDLE，那么最终只会用一个 Candidate。

## 2、rtcp-mux

RTCP 和 RTP 复用，表示 Sender 使用一个传输通道（单一端口）发送 RTP 和 RTCP。Receiver 必须准备好在 RTP 端口上接收 RTCP 数据，并需要预留一些资源，比如 RTCP 带宽。

rtcp-mux 将 RTP 和 RTCP 复用到单一的端口进行传输，这简化了 NAT traversal，而 BUNDLE 又将多路媒体流复用到同一端口进行传输，这不仅使 candidate harvesting 等 ICE 相关的 SDP 属性变得简单，而且又进一步简化了 NAT traversal。

rtcp-mux 是与 RTC 传输相关的重要的 SDP 属性，关于它的 SDP 协商的原则如下：

（1）如果 Offer 携带 rtcp-mux 属性，并且 Answer 方希望复用 RTP 和 RTCP 到单一端口，那么 Answer 必须也要携带该属性。

（2）如果 Offer 没有携带 rtcp-mux 属性，那么 Answer 也一定不能携带 rtcp-mux 属性，而且 Answer 方禁止 RTP 和 RTCP 复用单一端口。

（3）rtcp-mux 的协商和使用必须是双向的。

例如，客户端去订阅服务器的流，客户端的 Offer 没有携带 rtcp-mux 属性，那么服务器会认为客户端不支持 rtcp-mux，也不会走 rtcp 复用的流程。相反，服务器会分别创建 RTP 和 RTCP 两个传输通道，只有当两个通道的 ICE 和 DTLS 都成功，才会认为本次订阅的传输通道建立成功，继而向客户端发流。

## 3、ICE Connectivity

ufrag 和 pwd 就是 ICE short-term 认证算法用到的用户名和密码。而 trickle 说明 SDP 中没有包含 candidate 信息，Candidate 是通过信令单独交换的，这样可以做到 Connectivity checks 和 Candidate harvesting 并行处理，提高会话建立的速度。

## 4、DTLS

（1）fingerprint 是 DTLS 过程中的 Certificate 证书的签名，防止客户端和服务器的证书被篡改。
（2）setup 指的是 DTLS 的角色，也就是谁是 DTLS Client(active)，谁是 DTLS Server(passive)，如果自己两个都可以那就是 actpass。这里我们是 actpass，那么就要由对方在 Answer 中确定最终的 DTLS 角色

## 5、Stream Direction

媒体流的方向有四种，分别是 sendonly、recvonly、sendrecv、inactive，它们既可以出现在会话级别描述中也可以出现在媒体级别的描述中。

## 6、PlanB and UnifiedPlan

WebRTC 1.0版本目前是采用SDP标准格式、Unified Plan的W3C推荐协议，被所有主流浏览器所支持。Chrome中Plan B SDP已被弃用，将来会被彻底删除，PlanB and UnifiedPlan的具体区别如下：

（1）在Plan-b中， 音频只有一个m=, 视频只有一个m＝ 。 当有多路媒体流时， 根据ssrc 区分。
（2）在Unified-plan 中， 每路流都有一个m=。
（3）当只有一路视频流，一路音频流时，两者格式兼容。

PlanB 和 UnifiedPlan 其实就是 WebRTC 在多路媒体源（multi media source）场景下的两种不同的 SDP 协商方式。如果引入 Stream 和 Track 的概念，那么一个 Stream 可能包含 AudioTrack 和 VideoTrack，当有多路 Stream 时，就会有更多的 Track，如果每一个 Track 唯一对应一个自己的 M 描述，那么这就是 UnifiedPlan，如果每一个 M line 描述了多个 Track(track id)，那么这就是 Plan B。


## 7、candidate

Candidate 就是传输的候选人，客户端会生成多个 Candidate，比如有 host 类型的、有 relay 类型的、有 UDP 和 TCP 的。当然对方也会有很多 Candidate，接下来就是自己的 Candidates 和对方的 Candidates 匹配连通（ICE Connectivity Checks），形成 CandidatePair 也就是传输通道。

8、RTCP Feedback

表示媒体会话能够对哪些 RTCP 消息进行反馈，是一个和 QoS 相关的重要的 SDP 属性。

```
m=video 9 UDP/TLS/RTP/SAVPF 96
a=mid:video
a=rtpmap:96 VP8/90000
a=rtcp-fb:96 transport-cc
a=rtcp-fb:96 ccm fir
a=rtcp-fb:96 nack pli
```

如上 SDP 信息，这是一个视频的 M 描述，VP8 编码，payload type 是 96。最后的 3 个 rtcp-fb 属性则说明了对于 96 这个 media codec 来讲，在网络拥塞控制方面支持 twcc；在 ARQ 方面支持 nack 处理，能够重传丢失的 RTP 包；在关键帧方面支持 fir 和 pli 处理，有能力进行关键帧的发送。

rtcp-fb 不能用于会话级别的描述中，只能用于媒体级别的描述，而且其 M 描述的 proto 字段一定要指定 AVPF