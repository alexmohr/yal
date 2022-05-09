//
// Copyright (c) 2022 Alexander Mohr
// Licensed under the terms of the MIT License
//

#include <yal/abstraction.hpp>

namespace yal {
#ifndef delay
#include <chrono>
#include <thread>
void delay(unsigned long millis)
{
  std::this_thread::sleep_for(std::chrono::milliseconds(millis));
}
#endif

} // namespace yal
