# 一、音频数据采集

1. 音频数据采集与播放概述
2. ADM的创建
3. CoreAudio基本开概念
4. CoreAudio-API
5. AudioDeviceWindowsCore的构造函数
6. ADM初始化
7. 源码分析-ADM初始化之枚举音频设备
8. ADM初始化之InitSpeaker
9. ADM初始化之设置通道数
10. ADM初始化之InitMicrophone
11. AudioState
12. Engine_PeerConnection_Call等对象之间的关系
13. 打开播放设备
14. InitPlay基础知识最近学习
15. InitPlayout源码分析
16. 播放声音的基础API
17. 播放声音的具体流程
18. 源码分析-StartPlayout 
19. 再论音频DMO 
20. 源码分析-InitRecording 
21. StartRecording处理逻辑 
22. 源码分析-StartRecording 

# 二、视频数据采集

1. 视频采集概述 
2. DirectShow基础知识 
3. WebRTC视频采集整体架构 
4. 视频处理流程的建立 
5. 源码分析-视频处理流程的建立 
6. 构造设备信息对象 
7. 获音视频设备信息基础知识 
8. 源码分析-获取视频设备信息 
9. 创建并初始化VideoCapture 
10. 源码分析-构造并初始化VideoCapture 
11. 获取CaptureFilter 
12. 获取CaptureFilter的输出Pin 
13. 构造SinkFilter 
14. 源码分析-SinkFilter的构造 
15. 获取SinkFilter的输入Pin 
16. Filter之间的连接 
17. SetCameraOutput 
18. 源码分析-连接Filter（一） 
19. 源码分析-连接Filter（二） 
20. 输出采集后的视频数据 


# 三、音频引擎（音频编解码）
 
1. 音频引擎章节概述 
2. 音频引擎架构 
3. 创建音频引擎 
4. 音频初始化之编解码器的收集 
5. 音频初始化之AudioState对象的创建 
6. 音频引擎初始化之获取音频数据 
7. Channel-Stream与编解码器 
8. 创建音频编码器之一 
9. 创建Opus编码器 
10. 音频编码 
11. 音频解码器的创建 
12. 音频解码 

# 四、视频引擎（视频编解码）
 
1. 视频引擎章节概述 
2. 视频数据采集的时间 
3. 视频分发器VideoBroadcaster 
4. 视频数据是如何进入视频分发器的 
5. 视频引擎及其作用 
6. 视频编码器的创建与视频编码流程 
7. VideoStreamEncoder的创建 
8. 获取编解码器参数 
9. 应用视频编码参数 
10. 创建WebRtcVideoSendStream的时机 
11. 创建内部VideoSendStream 
12. VP8编码器的创建及编码 
13. 应用视频解码器参数 
14. 编解码器CodecID的设置 
15. SessionDescription 
16. 创建WebRtcVideoReceiveStream 
17. 创建解码器及初始化 
18. 视频解码 

# 深入理解WebRTC网络传输
 
1. 深入理解WebRTC网络传输-概述 
2. 网络设备管理 
3. 读取网卡信息的重要API 
4. 源码分析-CreateNetworks 
5. 获了本地默认IP地址和端口 
6. 获取本地默认IP地址 
7. ICE 
8. Candiate 
9. 创建PortAllocator 
10. 创建PortAllocatorSession 
11. 创建AllocationSequence 
12. 收集Candidate 
13. 获取本地Canidadate 
14. STUN协议 
15. 发送StunBindingRequest消息 
16. 收集Srflx类型的Candidate 
17. TURN协议基本原理 
18. TurnClient与TurnServer的连接过程 
19. Turn协议数据传输机制 
20. 收集Turn类型Candidate(一） 
21. 收集Turn类型Candidate(二) 
22. 收集TCP类型的Candidate 
23. 将获得的Candidate上抛给应用层 
24. WebRTC网络连接的建立 
25. Connection排序 
26. 选择Connection 
27. Connection的裁剪 
28. ICE提名 
29. ICE-FULL与ICE-LITE 
30. 连通性检测 
31. 网络传输对象的创建与数据传输 

# WebRTC服务质量（Qos）
 
1. WebRTC服务质量概述 
2. WebRTC服务质量综述 
3. RTP协议 
4. RTP扩展头 
5. RTCP协议一 
6. RTCP协议二-SDES作用和报文件格式 
7. RTCP协议三-其它类型的RTCP报文 
8. RTCP协议四-CompoundRTCP 
9. 丢包重传NACK与RTX 
10. 判断包位置的关键算法 
11. WebRTC中NACK的处理流程 
12. 判断是否丢包的逻辑 
13. 找到真正的丢包 
14. VP8关键帧的判断 
15. NACK格式 
16. WebRTC接收NACK消息的过程 
17. RTX协议 
18. WebRTC发送RTX包的过程 
19. Pacer 
20. RoundRobinPacketQueue 
21. IntervalBudget 
22. 向Pacer中插入数据 
23. JitterBuffer整体架构 
24. PacketBuffer的实现 
25. 查找完整的帧 
26. ReferenceFinder的作用及创建 
27. 查找参考帧 
28. FrameBuffer 
29. FEC基础知识和原理 
30. WebRTC中FEC的创建 
31. 为媒体包产生冗余数据 
32. FEC保护因子的计算 

#  五、NetEQ
 
1. NetEq在WebRTC中的位置 
2. 抖动消除的基本原理 
3. NetEq整体架构 
4. NetEq中的几种缓冲区 
5. 新版NetEq中的MCU和DSP


# 六、Simulcast与SVC
 
1. 什么是Simulcast 
2. 开启Simulcast的三种方法 
3. Simulcast在WebRTC中的实现 
4. 什么是SVC 
5. WebRTC开启SVC的方式 
6. VP9RTP包结构