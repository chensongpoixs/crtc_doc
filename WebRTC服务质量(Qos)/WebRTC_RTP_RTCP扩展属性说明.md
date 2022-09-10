
# WebRTC 中协议扩展属性 


## 1、 urn:ietf:params:rtp-hdrext:sdes:mid


```
//  在 unified SDP 描述中 ‘a=mid’ 是每个 audio/video line 的必要元素，这个 header extension 将 SDP 中 ‘a=mid’ 后信息保存，
// 用于标识一个 RTP packet 的 media 信息，可以作为一个 media 的唯一标识 
{ "urn:ietf:params:rtp-hdrext:sdes:mid",                                       RtpHeaderExtensionUri::Type::MID                    },
```

## 2、 	urn:ietf:params:rtp-hdrext:sdes:rtp-stream-id

```
// 1. Media Source 等同于 WebRTC 中 Track 的概念，在 SDP 描述中可以使用 mid 作为唯一标识
// 2. RTP Stream 是 RTP 流传输的最小流单位，例如在 Simulcast 或 SVC 场景中，一个 Media Source 中包含多个 RTP Stream，
//这时 SDP 中使用 ‘a=rid’ 来描述每个 RTP Stream
{ "urn:ietf:params:rtp-hdrext:sdes:rtp-stream-id",                             RtpHeaderExtensionUri::Type::RTP_STREAM_ID          },
```

## 3、 	urn:ietf:params:rtp-hdrext:sdes:repaired-rtp-stream-id

```
	// 用于声明重传时使用的 rid 标识	
		{ "urn:ietf:params:rtp-hdrext:sdes:repaired-rtp-stream-id",                    RtpHeaderExtensionUri::Type::REPAIRED_RTP_STREAM_ID },
```

## 4、 http://www.webrtc.org/experiments/rtp-hdrext/abs-send-time

```
	
// 1. abs-send-time 为 一个 3 bytes 的时间数据
// 2. REMB 计算需要 RTP 报文扩展头部 abs-send-time 的支持，用以记录 RTP 数据包在发送端的绝对发送时间
{ "http://www.webrtc.org/experiments/rtp-hdrext/abs-send-time",                RtpHeaderExtensionUri::Type::ABS_SEND_TIME          },
//Transport-wide Congestion Control

```

## 5、 http://www.ietf.org/id/draft-holmer-rmcat-transport-wide-cc-extensions-01
		
```
///////////////////////////////////////////////////////////////////////////////////////////////////
// 有效数据只有 16 bit，记录了一个 sequence number 称为 transport-wide sequence number
// 发送端在发送 RTP 数据包时，在 RTP 头部扩展中设置传输层序列号 TransportSequenceNumber；
// 数据包到达接收端后记录该序列号和包到达时间，然后接收端基于此构造 TransportCC 报文返回到发送端；
// 发送端解析该报文并执行 SendSide-BWE 算法，计算得到基于延迟的码率；
// 最终和基于丢包率的码率进行比较得到最终目标码率
{ "http://www.ietf.org/id/draft-holmer-rmcat-transport-wide-cc-extensions-01", RtpHeaderExtensionUri::Type::TRANSPORT_WIDE_CC_01   },
// NOTE: Remove this once framemarking draft becomes RFC.
```

## 6、 	http://tools.ietf.org/html/draft-ietf-avtext-framemarking-07

```
{ "http://tools.ietf.org/html/draft-ietf-avtext-framemarking-07",              RtpHeaderExtensionUri::Type::FRAME_MARKING_07       },
```	
		
## 7、 urn:ietf:params:rtp-hdrext:framemarking

```
// 由于 WebRTC 中 RTP payload 通过 SRTP 进行加密，这样导致 RTP packet 在经过交换节点或转发节点时，
// 有些场景下需要知道当前 RTP packet 的编码信息，framemarking 用于给定该编码信息 
{ "urn:ietf:params:rtp-hdrext:framemarking",                                   RtpHeaderExtensionUri::Type::FRAME_MARKING          },


// Frame Marking.
//
// Meta-information about an RTP stream outside the encrypted media payload,
// useful for an RTP switch to do codec-agnostic selective forwarding
// without decrypting the payload.
//
// For non-scalable streams:
//    0                   1
//    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5
//   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//   |  ID   | L = 0 |S|E|I|D|0 0 0 0|
//   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//
// For scalable streams:
//    0                   1                   2                   3
//    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
//   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//   |  ID   | L = 2 |S|E|I|D|B| TID |      LID      |   TL0PICIDX   |
//   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
///////////////////////////////////////////////////////////////////////////////////////
//	各个字段解释如下：
//	
//		S				: Start of Frame(1 bit) ，一帧的开始。 
//		E				: End of Frame(1 bit) ，一帧的结束。 
//		I				: Independent Frame(1 bit) ，是否是独立可解码的一帧，如264的IDR。 
//		D				: Discardable Frame(1 bit) ，可以被丢弃的帧，如时域分层中high layer，丢弃后其他帧仍然能正常解码。 
//		B				: Base Layer Sync(1 bit) ，当TID不为0，发送端知道该帧只依赖base layer，则填1，否则填0
//	    TID				: Temporal ID(3 bits) ，时域分层中的层ID，0表示base layer。短的extension格式中，填0.
//		LID				: Layer ID(8 bits) ，空域或者质量分层的层ID，0标识base layer。 
//		TL0PICIDX		: Temporal Layer 0 Picture Index(8 bits) ，时域0层picture index。
//						  因为只有8个bit，所以是循环计数的。TID = 0 LID = 0标识该帧ID，TID != 0或者LID != 0标识该帧依赖哪一个picture index。
//						  如果不存在时域分层或者计数未知，这个字段可以省略。
//	
///////////////////////////////////////////////////////////////////////////////////////
```

### ①、Frame Marking有什么用？

为什么要在RTP扩展头中传输基本信息，RTP的payload中不是已经包含了视频帧信息吗？先举一些应用场景的例子：

active spaker切换的时候，视频也需要做相应切换。此时需要找到一个时间点，视频帧可以不依赖之前的数据单独解码。这里需要知道知道视频是否是I帧，参考关系怎么样？

当遇到网络拥塞，server需要进行丢帧。因此需要知道哪些帧是可以丢弃的。

SVC中，需要知道转发哪一层的数据。

如果要在server上获取这些信息，以前的做法一般需要解析视频码流，如果存在SRTP还需要解SRTP。使用Frame Marking会让逻辑变得更简单，

我们不用解开视频数据就可以得到视频基本信息。这样一个纯转发的server不需要任何处理就能拿到基本信息，直接根据这些信息做流切换、丢帧、转发等。







## 8、urn:ietf:params:rtp-hdrext:ssrc-audio-level

```
// 用于音量调节
{ "urn:ietf:params:rtp-hdrext:ssrc-audio-level",                               RtpHeaderExtensionUri::Type::SSRC_AUDIO_LEVEL       },
```

## 9、	urn:3gpp:video-orientation

```
// 接收方收到以后，也会在响应中加入该 tag 属性，这样就完成了该功能的协商。Tag 中的 7 是扩展数据帧的扩展 id，
// 可以是 1-15 中的任何一个 , 用来标识该方向数据的位置，具体参考 RFC5285。更多描述参考 RCC.07- 2.7.1.2.2。
// 以上所述的视频方向其实包含了数据的方向和发送方 camera 的选项（前置或者后置），为了方便起见，一下都称为视频方向数据。
{ "urn:3gpp:video-orientation",                                                RtpHeaderExtensionUri::Type::VIDEO_ORIENTATION      },
```

## 10、 urn:ietf:params:rtp-hdrext:toffset

```	
// 传输时间偏移 (Transmission Time Offset)，offset 为 RTP packet 中 timestamp 与 实际发送时间的偏移
{ "urn:ietf:params:rtp-hdrext:toffset",                                        RtpHeaderExtensionUri::Type::TOFFSET 
		
```