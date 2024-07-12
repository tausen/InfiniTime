#include "components/timer/Timer.h"

using namespace Pinetime::Controllers;

Timer::Timer(void* const timerData, TimerCallbackFunction_t timerCallbackFunction) {
  pausedAtTimer = std::chrono::seconds(0);
  state = Stopped;
  timer = xTimerCreate("Timer", 1, pdFALSE, timerData, timerCallbackFunction);
}

std::chrono::milliseconds Timer::GetTimeRemaining() {
  TickType_t remainingTime = 0;

  if (state == Paused)
    return pausedAtTimer;

  if (IsRunning())
    remainingTime = xTimerGetExpiryTime(timer) - xTaskGetTickCount();
  else
    remainingTime = xTaskGetTickCount() - xTimerGetExpiryTime(timer);

  return std::chrono::milliseconds(remainingTime * 1000 / configTICK_RATE_HZ);
}

Timer::TimerState Timer::GetState() {
  return state;
}

void Timer::Start(const std::chrono::milliseconds duration) {
  TimerStart(duration);
  state = Running;
}

void Timer::Stop() {
  TimerStop();
  state = Stopped;
}

void Timer::Pause() {
  pausedAtTimer = GetTimeRemaining();
  TimerStop();
  state = Paused;
}

void Timer::Resume() {
  TimerStart(pausedAtTimer);
  state = Running;
}

void Timer::Ring() {
  TimerStop();
  state = Ringing;
}

bool Timer::IsRunning() {
  return (xTimerIsTimerActive(timer) == pdTRUE);
};

void Timer::TimerStart(const std::chrono::milliseconds duration) {
  xTimerChangePeriod(timer, pdMS_TO_TICKS(duration.count()), 0);
  xTimerStart(timer, 0);
}

void Timer::TimerStop() {
  xTimerStop(timer, 0);
}
