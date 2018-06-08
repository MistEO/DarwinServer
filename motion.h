#ifndef MOTION_H
#define MOTION_H

/* MOTION，达尔文电机控制
 * 宏定义DARWIN后即控制点击运动，否则仅输出
 * 宏定义具体见Makefile中DARWINFLAGS
 * 达尔文的电机控制库只能使用stdc++98标准编译
 * 所以该文件也只能使用stdc++98标准
 */

#if defined(DARWIN) || defined(ROBOTIS)
#include <LinuxCM730.h>
#endif // DARWIN

#include <string>

#include <bits/pthreadtypes.h>

class minIni;
namespace Robot {
class LinuxMotionTimer;
}

class Motion {
public:
    ~Motion();
    static Motion& ins(); //返回唯一静态单例

    bool walk_start(); //开始行走
    //设置行走速度，三个参数分别为直行振幅（速度），左右振幅，缓慢加速时间（为0即不缓慢加速）
    bool walk_stop();
    bool walk(int x_move, int a_move, int msec = 0);
    //检测跌倒，若跌倒会自动爬起，返回true；没跌倒返回false
    bool fall_up();
    //移动头部，三个参数分别为左右移动角度，上下移动角度，绝对/相对移动（true为绝对移动，false为相对移动）
    bool head_move(int x, int y, bool home = true);
    //根据序号进行动作
    bool action(int index, const std::string& audio = std::string());
    //void audio(const std::string & path);
private:
    Motion();
#if defined(DARWIN) || defined(ROBOTIS)
    minIni* ini;
    Robot::LinuxCM730 linux_cm730;
    Robot::CM730 cm730;
    Robot::LinuxMotionTimer* motion_timer;
#endif // DARWIN || ROBOTIS
    // static Motion unique_instance;
    pthread_mutex_t mlock;
};

#define motion Motion::ins()

#endif // !MOTION_H
