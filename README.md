# crtc_doc
webrtc 协议学习文档


# 一、代码分析

## 1、 视频模块编码后发送数据方法

[1、编码后的队列的时间戳检查三种策略](https://github.com/chensongpoixs/cwebrtc/commit/efe2ffd80678c5b95325814830c5f722e27f464e)


调用流程

VideoStreamEncoder类中方法OnFrame方法调用MaybeEncodeVideoFrame-> EncodeVideoFrame-> 接口 在调用编码器中接口Encode中编码完成后回调 注册videoStreamEncode类中OnEncodedImage中encoder_stats_observer_对象是 SendStatisticsProxy类 调用方法OnSendEncodedImage->[uma_container_->InsertEncodedFrame]在InsertEncodedFrame方法中三种数据的检查

中有sink类是VideoSendStreamImpl发送数据的类中接口OnEncodedImage中进行