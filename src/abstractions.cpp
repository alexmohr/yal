//
// Copyright (c) 2022 Alexander Mohr
// Licensed under the terms of the MIT License
//

#if !(HAVE_ARDUINO || YAL_ARDUINO_SUPPORT)
#include <yal/abstraction.hpp>
#include <chrono>
#include <thread>

void delay(unsigned long millis) {
  std::this_thread::sleep_for(std::chrono::milliseconds(millis));
}

unsigned long millis() {
  return std::chrono::steady_clock::now().time_since_epoch().count();
}

#endif
