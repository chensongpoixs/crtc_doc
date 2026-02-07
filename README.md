# crtc_doc

rtc 协议学习文档

![rtc 中文翻译](https://rfc2cn.com/)


## [Wireshark抓包工具使用说明](wireshark.md)





# 一、代码分析

## 1、 视频模块编码后发送数据方法

[1、编码后的队列的时间戳检查三种策略](https://github.com/chensongpoixs/cwebrtc/commit/efe2ffd80678c5b95325814830c5f722e27f464e)


调用流程

VideoStreamEncoder类中方法OnFrame方法调用MaybeEncodeVideoFrame-> EncodeVideoFrame-> 接口 在调用编码器中接口Encode中编码完成后回调 注册videoStreamEncode类中OnEncodedImage中encoder_stats_observer_对象是 SendStatisticsProxy类 调用方法OnSendEncodedImage->[uma_container_->InsertEncodedFrame]在InsertEncodedFrame方法中三种数据的检查

中有sink类是VideoSendStreamImpl发送数据的类中接口OnEncodedImage中进行






# 一、ICE/STUN/TURN协议

1.STUN
STUN（Session Traversal Utilities for NAT） 是一种可以让位于NAT后的设备可以查找自己的公网IP的技术。在WebRTC通信中，当两个客户端处于不同的NAT或防火墙之后时，它们需要通过STUN服务器来获取对方的公网地址，从而建立直接的点对点（Peer-to-Peer）连接。

1.1 STUN服务原理

STUN服务的原理主要包括以下几个步骤：

- 客户端向STUN服务器发送请求，请求中包含客户端的私有IP地址和端口号。

- STUN服务器收到请求后，会将请求中的私有IP地址和端口号作为响应返回给客户端。

- 客户端收到响应后，通过比较响应中的IP地址和端口号与自己的私有IP地址和端口号，可以判断出自己是否在NAT之后，以及NAT的类型。

- 如果客户端在NAT之后，它会使用STUN服务器返回的公网IP地址和端口号来告诉其他客户端自己的位置，从而建立点对点连接。

1.2 STUN 结构

STUN 消息有固定的头，总共20个字节：


```javascript 

    // Writing new length of the STUN message @ Message Length in temp buffer.
    //      0                   1                   2                   3
    //      0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
    //     +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    //     |0 0|     STUN Message Type     |         Message Length        |
    //     +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ 
    //     |                       Magic Cookie  32bit                     |
    //     +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    //     |                                                               |
    //     |                     Transaction Id  事务ID                    |
    //     |                                                               |
    //     +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
```

- STUN Message Type：14位，消息类型。
- Message Len：16位，消息长度，不包含消息头。
- Magic Cookie：32位，固定为0x2112A442
- Transaction ID：96位，事务ID

1.3 功能

- 公共地址发现：STUN客户端发送请求到STUN服务器，服务器通过响应告知客户端其NAT映射到的公共IP地址和端口。

- NAT类型检测：STUN服务器的响应还可以帮助客户端确定NAT的类型（如完全锥形NAT、对称NAT、端口限制对称NAT等），从而更好地理解设备之间的通信限制。

- 心跳保持：STUN可以用于保持与服务器的连接状态，防止NAT映射过期，从而确保设备之间的通信持续稳定。


2.TURN

TURN（Traversal Using Relays around NAT），是STUN 的中继扩展。简单的说，TURN 是通过两方通讯的“中间人”方式实现穿透。

当 STUN 服务检测，发现直接以 peer-to-peer 的形式连接时，就走 TURN 方式，使用中间网点提供的中继连接服务。TURN 协议就是用来允许主机控制中继的操作并且使用中继与对端交换数据。TURN 与其它中继控制协议不同的是它能够允许一个客户端使用一个中继地址与多个对端连接。

2.1 功能

- 数据中继：当设备之间的直接对等连接不可行时（比如两端都处于对称NAT后面），TURN服务器充当中继，接收来自设备的数据包并将其转发给其他设备，从而实现数据的传递。
- 动态分配公共地址：如果设备无法获得直接对等连接的公共IP地址，TURN服务器可以为设备分配临时的公共地址和端口，使其能够进行通信。
- 加密和安全：TURN服务器支持加密和身份验证，以确保通信的安全性和私密性。



3.ICE

ICE的全称是Interactive Connectivity Establishment，即交互式连接建立。它是一种端到端交互的技术，可以让两个终端相互知道对方的公网IP，可以不借助一个公网server完成端到端（Peer to peer，P2P）的通信。

3.1 功能

- 候选地址收集：ICE通过获取设备的所有可能的IP地址（本地和公共），包括通过不同类型的NAT和防火墙公开的地址，以创建候选地址列表。

- 连接检测：ICE使用STUN协议检测候选地址的可用性，确定哪些地址可以成功用于通信。
- 优先级排序：对候选地址进行优先级排序，以便在发起对等连接时选择最佳的通信路径。
- 对等连接和中继：如果直接对等连接不可行（比如两个设备都在NAT后面），ICE可以选择使用中继服务器（如TURN服务器）作为备选方案，以确保通信的成功。
- 协商：在通信的开始阶段，ICE允许设备之间协商最佳的通信路径，从而提高连接的成功率和质量。

3.2 ICE角色

- offer (主动发起)的一方为 controlling 角色

- answer (被动接受)的一方为 controlled 角色

3.3 ICE模式

- FULL ICE：此模式既可以收 binding request，也可以发 binding request。此模式由ice 客户端实现
- Lite ICE：此模式只接受并回复 binding request 请求，不会主动发送 binding request 请求给对方。只需回应 response 消息，对于部署在公网的设备比较常用

3.4 Candidate

- ICE Candidate是ICE框架中重要的概念，它描述了设备能够用于通信的各种网络地址和传输协议信息，是实现设备之间通信连接的关键步骤之一。

- 媒体传输候选地址，组成 candidate pair 做连通性检查，确定传输路径.

Candidate的类型：Host,Srvflx,Relay,Prflx。

- Host (Host Candidate)：该地址是一个真实的主机，参数中的地址和端口对应一个真实的主机地址，该地址来源于本地的物理网卡或逻辑网卡上的地址，对于具有公网地址或者同一内网的端可以用；
- Srvflx (Server Reflexive Candidate)：该地址是通过 Cone NAT (锥形 NAT) 反射的类型，参数中的地址和端口是端发送 Binding 请求到 STUN/TURN server 经过 NAT 时，NAT 上分配的地址和端口
- Relay (Relayed Candidate)：该地址是端发送 Allocate 请求到 TURN server，由 TURN server 用于中继的地址和端口，该地址和端口是 TURN 服务用于在两个对等点之间转发数据的地址和端口，是一个中继地址端口；( 可能是本机或 NAT 地址)；
- Prflx(Peer Reflexive Candidate)：该地址是通过发送 STUN Binding 时，通过 Binding 获取到的地址；在建立连接检查期间新发生，参数中的地址和端口是端发送 Binding 请求到 STUN/TURN server 经过 NAT 时，NAT 上分配的地址和端口

# 二、WebRTC中GCC算法

梯度算法的损失函数

1. 最小二乘法  离使的变成直线
2. 极大似然估计
3. 交叉熵


congestion_controller-> DelayBasedBwe ： 核心控制器， 接收网络反馈并输入带宽评估值


remote_bitrate_estimator->InterArrival: 计算组包间到达时间差和发送时间差

congestion_controller-> TrendlineEstimator:通过线性回归分析延迟趋势

remote_bitrate_estimator-> AimdRateControl：基于AIMD(加性增/乘性减去)策略调整码率



‌Chromium Tracing‌：

通过chrome://tracing可视化带宽调整过程


<font color='red'>1. GCC通过两种机制综合评估网络带宽</font>




1. 基于延迟的带宽评估（Delay-Based）：接收端通过分析数据包到达时间的延迟梯度(如线性回归或者卡尔曼率滤波)， 预测网络拥塞程度

2. 基于丢包率的带宽评估(Loss-Based): 发送端根据接收端反馈的丢包率（RTCP RR报文）调整码率， 例如丢包率> 10%时大辐降低码率

最终带宽取两者的最小值: ${A = min(A_{delay}, B_{loss})}$


2. GCC算法中关键模块与流程

①  接收端处理

- 延迟梯度计算： 接收端记录每个包的到达时间，通过算法（如Trendline线性回归）计算延迟变化趋势，判断网络状态（拥塞或者空闲）

- 宽带反馈： 通过RTCP REMB报文将预测带宽反馈给发送端

2. 发送端处理

- 动态码率调整：
    
      SendSideBandwidthEstimation模块： 结合丢包率和延迟反馈计算目标码率，触发码率增减

      AIMD原则（加性增、乘性减）: 网络空闲时线性增加码率，拥塞时指数级降低码率

- 探测机制（Probe）：在网络空闲或者恢复时，主动发送高码率探测包（如3倍当前码率），快速探测链路容量上限


3. 其它辅助模块

- PacedSender： 平滑发送速率，避免突发流量冲击网络
- CongestionWindowPushback: 基于RTT限制发送窗口， 防止队列堆积


# 三、弱网优化


## 1、带宽动态调控

- 智能带宽估计

GCC算法

- 码率分配优化

- 分层传输： 对视频基础层（Base Layer）分配70%带宽，增强层（Enhancemnt layer）占30%， 弱网时优先丢弃增强层

- 音频优先： 音频流分配20%~30%带宽， 确保语言连续性

## 2. 抗丢包和纠错技术（FEC）NACK、ARQ

- 动态FEC与NACK混合策略

丢包率< 5% 时禁止FEC，依赖NACK快速重传（RTT < 200ms场景）

丢包率>= 5% 启用FlexFEC， 冗余度安公司动态计算：

公式： $  \text{冗余度} = \min(50\%, \text{丢包率} \times 2 + 10\%)  $

## 3. 编码器优化

- 高效编解码选择

音频: 优先使用Opus比那么， 支持6kbps-510kps动态码率， 弱网下启动DTX（静音检测）减少带宽中用

视频： 低带宽场景(<1Mbps)使用VP9或H.264 SVC 分层编码，基图层码率控制在500kbps以内


## 4. 网络传输层优化

- Jitter buffer动态调整

基于网络抖动实时计算缓冲区大小

公式： $ \text{Buffer Size} = \text{基线延迟} + 3 \times \text{当前抖动}  $


# 四、 WEBRTC/SIP/RTP/RTSP/RTMP

# 五、 SIP 信令协议

# 六、RTSP 协议支持的核心指令如下：


一、‌指令通用格式‌

RTSP 指令基于文本协议，采用类似 HTTP 的请求-响应模型，包含‌请求行‌、‌头部字段‌和‌消息体‌（部分指令需要）。以下为通用结构：

‌客户端请求格式‌

```javascript
<方法> <URL> RTSP/1.0\r\n
CSeq: <序列号>\r\n
[其他头部字段]\r\n
\r\n
[消息体]（如 DESCRIBE 携带 SDP）
```

‌方法‌：如 OPTIONS、DESCRIBE、SETUP 等

‌CSeq‌：递增的序列号，用于匹配请求与响应‌

‌URL‌：资源地址，如 rtsp://192.168.1.100:554/live‌

‌服务器响应格式‌

```javascript
RTSP/1.0 <状态码> <状态描述>\r\n
CSeq: <序列号>\r\n
[其他头部字段]\r\n
\r\n
[消息体]（如 DESCRIBE 响应中的 SDP 描述）
```

‌状态码‌：如 200 OK（成功）、401 Unauthorized（认证失败）‌

二、‌基础控制指令‌




‌1. OPTIONS‌：客户端向服务器查询支持的方法（如 PLAY、PAUSE），用于协议能力协商‌

请求示例‌：

```javascript
OPTIONS rtsp://example.com/media RTSP/1.0\r\n
CSeq: 1\r\n
\r\n
```

‌响应示例‌：

```javascript
RTSP/1.0 200 OK\r\n
CSeq: 1\r\n
Public: DESCRIBE, SETUP, PLAY, PAUSE, TEARDOWN\r\n
\r\n


```

‌Public‌：服务器支持的方法列表‌


‌2. DESCRIBE‌：客户端请求媒体流的元数据（SDP 格式），获取编解码、分辨率、传输地址等参数‌

请求示例‌：

```javascript
DESCRIBE rtsp://example.com/media RTSP/1.0\r\n
CSeq: 2\r\n
Accept: application/sdp\r\n
\r\n

```

‌响应示例‌：

```javascript
RTSP/1.0 200 OK\r\n
CSeq: 2\r\n
Content-Type: application/sdp\r\n
Content-Length: 128\r\n
\r\n
v=0\r\n
o=- 123456 1 IN IP4 192.168.1.100\r\n
...（SDP 内容）
 
```

Accept‌：指定客户端支持的描述格式（如 SDP）‌

‌3. SETUP‌：协商媒体传输参数（如端口号、传输协议），建立 RTP/RTCP 传输通道。每个媒体流在 PLAY 前必须执行此命令‌

请求示例‌：

```javascript
SETUP rtsp://example.com/media/track1 RTSP/1.0\r\n
CSeq: 3\r\n
Transport: RTP/AVP;unicast;client_port=5000-5001\r\n
\r\n

```

‌响应示例‌：

```javascript
RTSP/1.0 200 OK\r\n
CSeq: 3\r\n
Transport: RTP/AVP;unicast;server_port=6000-6001\r\n
Session: 12345678\r\n
\r\n


```

- ‌Transport‌：协商传输参数（如端口号、协议类型）

- Session‌：会话标识符，后续指令需携带‌

‌4. PLAY‌：启动媒体流传输，支持指定播放时间范围（通过 Range 参数）。服务器开始通过 RTP 发送数据‌

请求示例‌：

```javascript
PLAY rtsp://example.com/media RTSP/1.0\r\n
CSeq: 4\r\n
Session: 12345678\r\n
Range: npt=0-\r\n
\r\n

```

‌响应示例‌：

```javascript
RTSP/1.0 200 OK\r\n
CSeq: 4\r\n
Session: 12345678\r\n
Range: npt=0-60\r\n
\r\n



```

‌Range‌：指定播放时间范围（如 npt=10-30 表示从 10 秒到 30 秒）‌


‌5. PAUSE‌：暂停媒体流传输，保留会话状态以便后续恢复播放‌

请求示例‌：

```javascript
PAUSE rtsp://example.com/live/stream RTSP/1.0\r\n
CSeq: 834\r\n
Session: 12345678\r\n
\r\n

```

- 暂停会话 12345678 对应的媒体流，序列号为 834‌16。
- 未携带 Range 时默认暂停当前播放时间点‌

‌响应示例‌：

```javascript
RTSP/1.0 200 OK\r\n
CSeq: <序列号>\r\n
Session: <会话ID>\r\n
\r\n 
```

- ‌状态码‌：200 OK 表示暂停成功‌
- Session‌：保持与请求一致的会话标识符‌

‌6.TEARDOWN‌： 终止会话并释放资源，关闭 RTP/RTCP 传输通道‌

请求示例‌：

```javascript
TEARDOWN rtsp://example.com/media RTSP/1.0\r\n
CSeq: 5\r\n
Session: 12345678\r\n
\r\n

```

‌响应示例‌：

```javascript
RTSP/1.0 200 OK\r\n
CSeq: 5\r\n
Session: 12345678\r\n
\r\n


```

必须携带 ‌Session‌ 标识符以终止指定会话‌

三、‌可选与扩展指令‌

1. ‌RECORD‌： 用于向服务器发起录制请求，将媒体流存储到指定位置（需服务器支持）‌

‌2. ANNOUNCE‌：客户端或服务器主动发送媒体描述信息（如更新 SDP 参数），用于动态调整会话属性‌

‌3. GET_PARAMETER‌：查询服务器或客户端的会话状态参数（如网络延迟、带宽使用情况）‌

‌4. REDIRECT‌：服务器通知客户端重定向到新的媒体源地址（适用于负载均衡或故障转移场景）‌

指令交互流程示例

‌初始化会话‌：OPTIONS → DESCRIBE → SETUP → PLAY

‌控制操作‌：PAUSE → PLAY（恢复播放）

‌结束会话‌：TEARDOWN

技术细节

‌传输方式‌：

‌UDP‌：默认传输层协议，需配合 RTP/RTCP 实现低延迟传输‌28。

‌TCP‌：通过 RTP over RTSP 模式实现可靠传输（适用于网络不稳定场景）‌38。

‌会话标识‌：通过 Session 头字段维护状态，确保指令在同一个会话上下文中执行‌


# 七、RTMP协议

一、Rtmp握手实现

- 随机数生成
- 握手包大小
- 复杂握手digest计算和验证
- 客户端握手状态机
- 服务端握手状态机




scp openssl-1.0.1.tar.gz     chensong@192.168.163.129:/home/chensong/Work/


# 八、HLS

# 九、HTTP-FLV

# 十、VP8、VP9、H264、H265、AV1 知识

1. 基础对比与核心差异

|标准|推出时间/组织|核心特点‌	|‌应用场景|
|:---:|:---:|:---:|:---:|
|H264‌	|2003年 / ITU-T & MPEG|	- 基于宏块划分（16x16）和帧间预测，支持多参考帧和CABAC熵编码‌3- 压缩率适中，硬件兼容性极佳，专利授权费用较高‌3|	直播、视频会议、蓝光视频等主流场景‌5|
|‌H.265‌|	2013年 / ITU-T & MPEG|	- 引入CTU（最大64x64块）、更复杂的运动补偿和SAO滤波，压缩效率提升约50%‌3 \n- 计算复杂度高，硬件支持逐步普及，专利授权争议较多‌|	4K/8K超高清视频、存储优化场景‌3
|‌VP9‌|	2013年 / Google|	- 支持10位色深、动态分辨率切换，无专利费用‌ - 压缩效率接近H.265，但硬件解码支持有限，主要用于WebRTC和YouTube‌7|	浏览器端视频、开源生态场景‌7
|‌AV1‌|	2018年 / AOMedia联盟|	- 基于VP9改进，引入更复杂的预测模式（仿射变换、帧内块复制）和CDEF滤波，压缩率提升30%‌4 - 完全免版权费，生态快速扩展（Netflix、YouTube）‌|流媒体、低带宽传输、开源项目‌

2、‌高频面试考点‌

<font color='red'>‌H.264 vs H.265 的压缩效率提升原理‌</font>

- 块划分优化‌：H.265 使用更大的 CTU（64x64）和递归四叉树划分，减少冗余信息‌
- 预测增强‌：支持更多帧内预测方向（35种 vs H.264的9种）和运动矢量精度（1/4像素 vs 1/2像素）‌
- ‌熵编码优化‌：采用更高效的CABAC和并行处理设计‌
‌

<font color='red'>AV1 的核心优势与挑战‌</font>

- ‌技术优势‌：通过多级动态分割（128x128块）、多参考帧和帧内预测模式（超200种）提升压缩效率‌
- 生态挑战‌：硬件解码普及度低（依赖软解），编码复杂度极高（比H.265高10倍）‌

<font color='red'>VP9 与 H.265 的适用场景差异‌</font>

- ‌VP9‌：适合开源生态（如WebRTC）和免授权场景，但硬件支持弱于H.265‌
‌
- H.265‌：在超高清广播和专业制作领域占优，但需应对专利授权成本‌

3、‌编码工具与延迟问题‌

<font color='red'>‌编码工具对比‌</font>

- H.264‌：宏块划分、CAVLC/CABAC熵编码、去块滤波器‌
- ‌AV1‌：帧内块复制（Intra Block Copy）、动态时间轴滤波（Alt-Ref）、多线程编码‌

‌
<font color='red'>直播场景的延迟优化‌</font>


- H.264/H.265‌：通过缩短GOP长度（如5秒）和快速关键帧（I帧）定位实现秒开‌
- ‌AV1/VP9‌：依赖分层编码（SVC）和动态码率适配，但实时编码延迟较高‌



# 十二、编解码

1、请详细解释 H.264 编码中的熵编码方式（CAVLC 和 CABAC），它们的区别和适用场景是什么？

考察重点：

- H.264 编码原理深度理解
- 熵编码算法掌握
- 性能和压缩率权衡意识

参考答案：

CAVLC (Context-adaptive variable-length coding) 和 CABAC (Context-adaptive binary arithmetic coding) 是两种熵编码方式：

1、CAVLC 特点：

- 基于变长编码
- 计算复杂度较低
- 压缩效率中等
- 适合 Baseline Profile

2、CABAC 特点：

- 基于算术编码
- 计算复杂度高
- 压缩效率高（比 CAVLC 提升 10-15%）

适合 Main/High Profile

选择建议：

实时场景：优先 CAVLC

存储场景：优先 CABAC

低延迟：CAVLC

高压缩比：CABAC
 

2、解释 H.264/H.265 中的帧内预测模式，以及它们在编码器优化中的应用？


考察重点：

- 帧内预测基本原理
- 预测模式选择策略
- 编码器优化经验

参考答案：

帧内预测基本原理：

1、H.264 帧内预测：

- 亮度：9种模式（4×4/16×16）
- 色度：4种模式
- 基于相邻已重建像素

2、H.265 帧内预测：

- 35种角度预测模式
- DC模式和Planar模式
- 支持多种块大小（4×4到64×64）

优化策略：

```javascript
class IntraPrediction {
private:
    float evaluateMode(int mode, Block* block) {
        float cost = 0;
        // RD cost 计算
        cost = calculateRDCost(mode, block);
        // 考虑相邻块使用的模式
        cost += getModePenalty(mode, neighborModes);
        return cost;
    }
    
    int fastModeDecision(Block* block) {
        // 快速模式选择算法
        int bestMode = 0;
        float minCost = FLT_MAX;
        
        // 1. 粗筛选：基于边缘检测
        vector<int> candidateModes = getEdgeBasedModes(block);
        
        // 2. 精筛选：RD cost对比
        for (int mode : candidateModes) {
            float cost = evaluateMode(mode, block);
            if (cost < minCost) {
                minCost = cost;
                bestMode = mode;
            }
        }
        return bestMode;
    }
};

```

3、优化技巧：

- 基于边缘检测的模式筛选
- RD cost快速计算
- 相邻块模式关联性利用
- SIMD 指令优化预测计算

3、请详细解释 H.265 中的 CTU 和 CU 结构，以及它们对编码效率的影响？

考察重点：

- H.265 编码结构理解
- 块划分策略掌握
- 性能优化经验

参考答案：

1、基本概念：

- CTU (Coding Tree Unit)：最大 64×64
- CU (Coding Unit)：可递归四叉树分割
- PU (Prediction Unit)：预测单元
- TU (Transform Unit)：变换单元

2、划分策略：

```javascript
class CodingTreeUnit {
    void split(CTU* ctu) {
        if (shouldSplit(ctu)) {
            // 四叉树分割
            for (int i = 0; i < 4; i++) {
                CU* subCU = splitIntoCU(ctu, i);
                analyzeCU(subCU);
            }
        } else {
            // 作为单个CU编码
            encodeCU(ctu);
        }
    }
    
    bool shouldSplit(CTU* ctu) {
        // 分割决策因素：
        // 1. 图像复杂度
        float complexity = calculateComplexity(ctu);
        // 2. 运动剧烈程度
        float motion = estimateMotion(ctu);
        // 3. 码率约束
        float rateCost = estimateRateCost(ctu);
        
        return evaluateSplitDecision(complexity, motion, rateCost);
    }
};

```


3、性能影响：

- 灵活的块大小适应
- 更精确的预测单元
- 计算复杂度提升
- 压缩率提升15-30%

4、优化建议：

- 自适应深度控制
- 快速分割决策
- 并行处理优化
- 硬件加速支持

4、在视频编码中，如何实现和优化运动估计算法？

考察重点：

- 运动估计算法原理
- 优化策略设计
- 实际实现经验

参考答案：

1、基本算法实现：

```javascript
class MotionEstimation {
private:
    // 运动搜索范围
    int searchRange;
    // 块大小
    int blockSize;
    
    // 三步搜索法
    MotionVector threeStepSearch(MacroBlock* cur, MacroBlock* ref) {
        Point center(0, 0);
        int step = searchRange / 2;
        
        while (step >= 1) {
            Point bestPoint = searchPoints(cur, ref, center, step);
            center = bestPoint;
            step /= 2;
        }
        return MotionVector(center);
    }
    
    // 钻石搜索
    MotionVector diamondSearch(MacroBlock* cur, MacroBlock* ref) {
        // 大钻石模式
        Point center = largeDiamondSearch(cur, ref);
        // 小钻石模式
        return smallDiamondSearch(cur, ref, center);
    }
};

```


2、优化策略：

- 早停机制
- 预测器优化
- 搜索模式自适应
- 多分辨率搜索

3、硬件加速：

- SIMD 指令优化
- GPU 并行计算
- 专用硬件加速

4、评价指标：

- SAD/SSD 计算
- 计算复杂度
- 编码效率
- 运动矢量代价

5、请解释音频编码中的心理声学模型，以及它在 AAC 编码中的应用？

考察重点：

- 心理声学原理
- AAC 编码器实现
- 音质优化经验

参考答案：

1、心理声学模型原理：

- 听觉掩蔽效应
- 绝对听阈
- 临界带宽
- 时域掩蔽

2、AAC 实现：

```javascript

class PsychoacousticModel {
    struct CriticalBand {
        float startFreq;
        float endFreq;
        float energy;
    };
    
    void calculateMaskingThreshold() {
        // 1. FFT 分析
        vector<float> spectrum = performFFT(signal);
        
        // 2. 临界带划分
        vector<CriticalBand> bands = getBands(spectrum);
        
        // 3. 掩蔽计算
        for (auto& band : bands) {
            // 音调识别
            bool isTonal = detectTonal(band);
            // 掩蔽曲线计算
            float maskingCurve = calculateMasking(band, isTonal);
            // 全局掩蔽阈值
            updateGlobalThreshold(maskingCurve);
        }
    }
};
```

3、优化方向：

- 计算复杂度降低
- 掩蔽精度提升
- 低延迟处理
- 特殊场景优化


# 十一、 AAC、OPUS 知识

# 二、H264的SVC编解码、OPS(DTS)


一、‌分层编码机制‌

H.264 SVC（可分级视频编码）通过在编码过程中对视频流进行分层处理，实现 ‌时间、空间、质量‌ 三个维度的可扩展性，具体原理如下：

‌时间可适性（Temporal Scalability）‌

- ‌原理‌：将视频帧划分为 ‌基础层（Base Layer）‌ 和 ‌增强层（Enhancement Layer）‌。
- 基础层‌：传输关键帧（I帧），提供最低帧率的连续播放能力‌
- 增强层‌：补充中间帧（P/B帧），提升帧率至目标值‌

- 应用‌：网络带宽不足时，仅解码基础层可维持低帧率流畅播放；带宽充足时叠加增强层实现高帧率‌12。

‌空间可适性（Spatial Scalability）‌

- 原理‌：通过分层下采样生成不同分辨率的视频层。
‌
- 基础层‌：低分辨率版本（如 360p），保证基本清晰度 

- 增强层‌：高分辨率层（如 720p/1080p），叠加细节信息 
- 应用‌：接收端根据设备屏幕或网络条件动态选择解码层级 
‌
质量可适性（SNR/Quality Scalability）‌

- 原理‌：通过调整量化参数（QP）或编码比特率分层，生成不同质量的视频流。
- 基础层‌：高压缩率、低码率，牺牲细节保留主体内容 
‌
- 增强层‌：低压缩率、高码率，补充纹理与高频信息 
- ‌应用‌：弱网环境下优先传输基础层，确保视频连贯性 

二、‌编码结构与动态调整‌

‌分层编码结构‌

- ‌NALU 组织‌：SVC 流由多个网络抽象层单元（NALU）组成，每个 NALU 对应不同层级数据，通过标识符区分 
‌
- 依赖关系‌：增强层数据依赖基础层，解码时必须按顺序处理（如先解码基础层，再叠加增强层） 
‌
动态适配机制‌

- ‌带宽感知‌：通过 RTCP 反馈实时监测网络状态（如丢包率、延迟），动态丢弃或优先传输增强层数据 
- ‌兼容性处理‌：服务器可将 SVC 流转换为传统 AVC 流（如丢弃增强层），适配不支持 SVC 的解码器 

三、‌核心优势与典型应用‌

‌优势‌

- ‌抗弱网能力‌：分层传输降低卡顿风险，提升实时通信鲁棒性‌17。
‌
- 存储效率‌：单一 SVC 文件可动态提取不同层数据，减少多版本存储开销‌8。
‌
应用场景‌

‌- 实时通信‌（如 WebRTC）：结合 NACK/FEC 机制，优先保障基础层传输质量‌
‌
- 流媒体服务‌：根据用户设备性能分发适配层级（如手机端接收低分辨率层）‌

# 三、MediaSoup的媒体服务的优化

# 四、 视频平台 搭建 


sip、rtmp、rtsp、webrtc、HLS





面向对象

1. 封装
2. 继承
3. 多态



•熟练掌握精通socket编程，具备高性能网络并发开发经验,掌握libevent、libuv等开源

•掌握 MySQL 关系数据库基本操作，熟悉 redis，memcache 等缓存数据库

•熟悉高并发网络服务器，反向代理与负载均衡服务器，分布式存储原理

•掌握TCP/IP协议栈通信原理, 熟悉WEBRTC/SIP/RTP/RTSP/RTMP等流媒体协议

•熟悉ICE、STUN、TURN等协议原理

•熟悉音视频编解码原理, 掌握H.264/H.265/VP8/VP9/AV1/AAC/Opus等常用音视频编码解码

•熟悉音视频传输的底层逻辑，并掌握弱网对抗的技术原理，并具有实际的弱网优化经验 

•熟悉FFmpeg框架及filter、WebRTC、Obs-studio、OpenCV、ZLMediaKit、MediaSoup多媒体开源项目



#  gb28121 主动和被动模式


主动连接 ：服务器连接设备 [TCP]
被动连接：设备连接服务[TCP, UDP ]
