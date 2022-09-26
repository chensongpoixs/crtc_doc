# 一、WebRTC中信号状态枚举

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