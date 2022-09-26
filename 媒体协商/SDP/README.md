# SDP 协议


## 一、SDP的结构

这个非常容易理解SDP


![SDP的结构](https://github.com/chensongpoixs/crtc_doc/blob/master/%E5%AA%92%E4%BD%93%E5%8D%8F%E5%95%86/img/sdp_struct.jpg)


## 二、SDP规范

### 1、SDP格式
    
```
<key>=<value>
```

### 2、Key： m、a、c、v、s、o
   
    
### 3、Value： rfc4566文档
    

### 4、SDP例子
    
```
 // 一. 协议的版本
v=0 // v=proto version
// 二、// WebRTC中使用下面三个字段
//    1. username: 名字 默认[-]
//    2. session-id：会话id
//    3. sess-version: 会话版本 ， 例如第一次创建offer是2 第二次是就是4 、、
0=- 123123 2 IN IP4 127.0.0.1 // o=<username> <sess-id> <sess-version> <nettype> <addrtype>   

// 三、  port: 是0时表示两个用户协商没有成功
m=video 7 UDP/TLS/RTP/SAVPF 100 101  // m=<media> <port> <proto> <fmt>

//四、对上面m行补充说明 mid的值
a=mid:0  // mid-attribute      = "a=mid:" identification-tag 

// 五、对RTP协议扩展 中payload_type = 127 是H264时钟频率90000
a=rtpmap:127 H264/90000 // a=rtpmap:<payload type> <encoding name>/<clock rate>[/<encodingparameters>]
、、
```

## 三、ICE-FULL与ICE-LITE 在标准的SDP中协议中是不包含的

### 1、a=ice-lite

### 2、是否是双方都发送stun request/stun response


<font color='red'>ice-lite： 是客户端向服务端发送stun request请求，服务端返回 stun response 就可以了 不需要服务端发送stun request 请求 以优化服务端的性能 </font>

### 3、没有a=ice-lite就默认ice-full

双方都需要发送stun request请求

### 4、WebRTC默认使用ice-full


## 四、PlanB与UnifiedPlan

早期WebRTC使用PlanB


区别是有多个m行


### 1、PlanB 格式

多个相同媒体信息同用一个m行

使用a=ssrc区分多给m行的视频，他们的ssrc是不一样的

```
m=video 7 UDP/TLS/RTP/SAVPF 127 128
...
a=ssrc:1234 cname:wwwww
a=ssrc:1634 cname:wwwww
..

```

### 2、 UnifiedPlan格式


多个相同媒体信息同不共用一个m行



```
m=video 7 UDP/TLS/RTP/SAVPF 127
...
a=ssrc:1234 cname:probator

...
m=video 7 UDP/TLS/RTP/SAVPF 100 101
...
a=ssrc:751214125 cname:3468df6d

```



