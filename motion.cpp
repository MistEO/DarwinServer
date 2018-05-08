#ifdef DARWIN
#include "motion.h"
#include <cassert>
#include <cmath>
#include <unistd.h>

#include <DARwIn.h>
#include <LinuxCM730.h>
#include <LinuxDARwIn.h>

using namespace Robot;

const char* INI_FILE_PATH = "/darwin/Data/config.ini";
const char* U2D_DEV_NAME = "/dev/ttyUSB0";
const char* MOTION_FILE_PATH = "/darwin/Data/motion_4096.bin";

Motion Motion::unique_instance;

Motion& Motion::ins()
{
    return unique_instance;
}

Motion::Motion()
    : ini(new minIni(INI_FILE_PATH))
    , linux_cm730(U2D_DEV_NAME)
    , cm730(&linux_cm730)
    , mlock(PTHREAD_MUTEX_INITIALIZER)
{
    Action::GetInstance()->LoadFile(const_cast<char*>(MOTION_FILE_PATH));

    assert(MotionManager::GetInstance()->Initialize(&cm730));
    Walking::GetInstance()->Initialize();
    MotionManager::GetInstance()->LoadINISettings(ini);
    Walking::GetInstance()->LoadINISettings(ini);

    MotionManager::GetInstance()->AddModule((MotionModule*)Action::GetInstance());
    MotionManager::GetInstance()->AddModule((MotionModule*)Head::GetInstance());
    MotionManager::GetInstance()->AddModule((MotionModule*)Walking::GetInstance());

    motion_timer = new LinuxMotionTimer(MotionManager::GetInstance());
    motion_timer->Start();
    MotionManager::GetInstance()->SetEnable(true);
    //Slowly stand up
    Action::GetInstance()->m_Joint.SetEnableBody(true, true);
    Action::GetInstance()->Start(15);
    while (Action::GetInstance()->IsRunning() == true)
        usleep(8000);

    //Head::GetInstance()->m_Joint.SetEnableHeadOnly(true, true);
    //Walking::GetInstance()->m_Joint.SetEnableBodyWithoutHead(true, true);
    //Head::GetInstance()->MoveByAngle(0, 60);
}

Motion::~Motion()
{
    Walking::GetInstance()->Stop();
    while (Walking::GetInstance()->IsRunning())
        usleep(8 * 1000);
    MotionManager::GetInstance()->RemoveModule((MotionModule*)Walking::GetInstance());
    Action::GetInstance()->Start(15);
    while (Action::GetInstance()->IsRunning())
        usleep(8 * 1000);
    delete motion_timer;
    delete ini;
    pthread_mutex_destroy(&mlock);
}

void Motion::walk_start()
{
    pthread_mutex_lock(&mutex);
    //Slowly stand up
    Action::GetInstance()->m_Joint.SetEnableBody(true, true);
    Action::GetInstance()->Start(9);
    while (Action::GetInstance()->IsRunning() == true)
        usleep(8000);
    Walking::GetInstance()->m_Joint.SetEnableBodyWithoutHead(true, true);
    Robot::Walking::GetInstance()->Start();
    pthread_mutex_unlock(&mutex);
}

void Motion::walk_stop()
{
    pthread_mutex_lock(&mutex);
    Walking::GetInstance()->Stop();
    while (Walking::GetInstance()->IsRunning())
        usleep(8 * 1000);
    pthread_mutex_unlock(&mutex);
}

void Motion::walk(int x_move, int a_move, int msec)
{
    pthread_mutex_lock(&mutex);
    if (msec == 0) {
        Robot::Walking::GetInstance()->X_MOVE_AMPLITUDE = x_move; //Straight speed
        Robot::Walking::GetInstance()->A_MOVE_AMPLITUDE = a_move; //Turn speed
    } else {
        int pre_amp = Robot::Walking::GetInstance()->X_MOVE_AMPLITUDE;
        int iterval = msec / abs(pre_amp - x_move);
        for (int i = pre_amp; i != x_move;) {
            Robot::Walking::GetInstance()->X_MOVE_AMPLITUDE = i;
            usleep(iterval * 1000);
            if (pre_amp > x_move) {
                --i;
            } else {
                ++i;
            }
        }
    }
    pthread_mutex_unlock(&mutex);
}

bool Motion::fall_up()
{
    pthread_mutex_lock(&mutex);
    if (MotionStatus::FALLEN != STANDUP) {
        Walking::GetInstance()->Stop();
        while (Walking::GetInstance()->IsRunning() == 1)
            usleep(8000);

        Action::GetInstance()->m_Joint.SetEnableBody(true, true);

        if (MotionStatus::FALLEN == FORWARD)
            Action::GetInstance()->Start(10); //Forwrad getup
        else if (MotionStatus::FALLEN == BACKWARD)
            Action::GetInstance()->Start(11); //Backward getup

        while (Action::GetInstance()->IsRunning() == 1)
            usleep(8000);

        Head::GetInstance()->m_Joint.SetEnableHeadOnly(true, true);
        Walking::GetInstance()->m_Joint.SetEnableBodyWithoutHead(true, true);
        pthread_mutex_unlock(&mutex);
        return true;
    }
    pthread_mutex_unlock(&mutex);
    return false;
}

void Motion::head_move(int x, int y, bool home)
{
    pthread_mutex_lock(&mutex);
    Head::GetInstance()->m_Joint.SetEnableHeadOnly(true, true);
#ifndef NDEBUG
    printf("Head MoveByAngle: %d, %d\n", x, y);
#endif
    if (home)
        Head::GetInstance()->MoveToHome();
    Head::GetInstance()->MoveByAngle(x, y);
    pthread_mutex_unlock(&mutex);
}

void Motion::action(int index, const std::string& audio)
{
    pthread_mutex_lock(&mutex);
    Walking::GetInstance()->Stop();
    while (Walking::GetInstance()->IsRunning() == 1)
        usleep(8000);
    Action::GetInstance()->m_Joint.SetEnableBody(true, true);

    Action::GetInstance()->Start(index);
    if (!audio.empty()) {
        Robot::LinuxActionScript::PlayMP3(audio.c_str());
    }

    while (Action::GetInstance()->IsRunning() == 1)
        usleep(8000);
    pthread_mutex_unlock(&mutex);
}

#endif //DARWIN
