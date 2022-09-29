
 
# 一、WebRTC媒体协商


## 1、SDP协议

[WebRTC的SDP详细](SDP/WebRTC的SDP详细.md)

<font color='red'>SDP中WebRTC做了哪些修改</font>


## 2、编解码器信息收集的步骤

composite：混合成的

### ①、 创建PeerConnectionFactory

### ②、 创建并初始化PeerConnection

### ③、 信息存放于MediaSessionDescriptionFacfory中(重点)


WebRtcSessionDescriptionFactory 中certificate_request_state_ 生成certificate 四种状态

```
enum CertificateRequestState {
    CERTIFICATE_NOT_NEEDED,
    CERTIFICATE_WAITING,
    CERTIFICATE_SUCCEEDED,
    CERTIFICATE_FAILED,
  };
```

证书的收集流程步骤

在WebRtcSessionDescriptionFactory构造函数中 修改certificate_request_state_的状态 为CERTIFICATE_WAITING ， 后创建一个与RTCCertificateGenerationTask任务处理(证书生成使用的工作线程， 通知使用的信号线程通知的)WebRtcCertificateGeneratorCallback

在RTCCertificateGenerationTask任务有任务使用工作线程创建证书， 生成后使用信号线程通知WebRtcCertificateGeneratorCallback中OnSuccess方法 中在回调WebRtcSessionDescriptionFactory中的方法 SetCertificate中修改 certificate_request_state_ 的状态为CERTIFICATE_SUCCEEDED 通知应用层、保持证书的数据

然后检查应用层是否有创建Offer或者Answer的情况、 如果有的话说明create_session_description_requests_队列中是有数据的、<font color='red'> 一般情况下是没有数据的， 原因是只有的在应用层在生成证书之前调用创建Offer或者创建Answer的接口，create_session_description_requests_队列中才有数据，  </font>
























### ④、 最后创建SessionDescrioption


## 2、Offer的创建

### ①、生成证书

1. 生成Offer(SessionDescrtionption)之前要先有证书
2. 证书是在pc->Initialize()中创建
3. 当收到证书Ready信号(slot)时, 生成Offer
4. Operationchain队列顺序化队列中队列大小为0时原理 size == 1机制 , Run();



## 3、Answer的创建


## 4、SetLocalDescription做了哪些事儿？


## 5、SetRemoteDescription又做了啥？
 
# 二、WebRTC中信号状态枚举

｀｀｀
enum RTCSignalingState {
  "stable",
  "have-local-offer",
  "have-remote-offer",
  "have-local-pranswer",
  "have-remote-pranswer",
  "closed"
};

｀｀｀

状态转换图

！［］（．／ｉｍｇ／rtc_signaling_state.png）



＃＃　１、一组示例转换可能是：


呼叫者转换：

新连接（）： ”stable"

setLocal描述（报价）： ”have-local-offer"

setRemoteScription（pranswer）： ”have-remote-pranswer"

setRemote描述（答案）： ”stable"

被叫方转换：

新连接（）： ”stable"

setRemote描述（报价）： ”have-remote-offer"

setLocalDescription（普兰斯wer）： ”have-local-pranswer"

set本地描述（答案）： ”stable"