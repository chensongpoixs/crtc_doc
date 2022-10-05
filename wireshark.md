# wireshark使用说明


## 1、按照协议过滤


```

udp
tcp
```


## 2、按照IP过滤


目的ip地址过滤

```
ip.dst==192.168.1.1
```

源IP地址过滤

```
ip.src==192.168.1.1
```

IP地址过滤


```
ip.addr == 192.168.1.1
```

## 3、按照port过滤


```
tcp.port == 8080

udp.port == 3478

udp.dstport == 3478

udp.srcport == 3478
```


## 4、过滤长度

```
udp.length < 30


tcp.length < 30

http.content_length < 30

```





与and关键字



```
upd.srcport==3478 and ip.src==39.105.185.198


upd.srcport==3478 and !(ip.src==39.105.185.198)
```
