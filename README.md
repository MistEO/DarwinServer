# 达尔文服务器端

## 使用方法

使用`HTTP/1.1`协议访问`1680`端口，仅支持`GET`方法

- `/image`
    - 获取摄像头图片
    - 响应数据返回`(char*)cv::Mat::data`
    - 若无法获取到图像，则响应`500 Internal Server Error`

- `/audio`
    - 使用`mplayer`播放服务器本地音频文件，支持所有主流音频格式
    - 请求参数`filepath`传递文件的绝对路径（必须，字符串）
    - 若文件不存在或不可读，则响应`500 Internal Server Error`
    - 使用方法举例：`http://127.0.0.1:1680/audio?filepath=/darwin/Data/mp3/Introduction.mp3`

- `/stop_audio`
    - 停止播放音频文件

- `/motor/walk_start`
    - 控制机器人开始行走（原地踏步）

- `/motor/walk`
    - 控制机器人行走速度
    - 请求参数`x`表示直行速度（可选，整数，默认为5）；`y`表示转向速度（可选， 整数，默认为0）；`msec`表示加速延时（可选，整数，单位毫秒，默认为2000）
    - 使用方法举例：`http://127.0.0.1:1680/walk?x=30&msec=5000`

- `/motor/walk_stop`
    - 控制机器人停止行走

- `/motor/fall_up`
    - 控制机器人跌倒爬起

- `/motor/head`
    - 控制机器人头部运动
    - 请求参数`x`表示横向移动角度（可选，整数，默认为0）；`y`表示纵向移动角度（可选，整数，默认为60）；`home`表示相对或绝对移动（可选，0表示相对，1表示绝对，默认为1）

- `/motor/action/<index>`
    - 控制机器人执行动作，`<index>`为动作序号
    - 请求参数`audio`，做动作同时播放服务器本地音频文件，仅支持mp3格式，需传递文件的绝对路径（可选，字符串）
    - 使用方法举例：`http://127.0.0.1:1680/motor/action/9?audio=/darwin/Data/mp3/Introduction.mp3`


**注意**：所有电机控制方法互斥，在服务器内部加了互斥锁