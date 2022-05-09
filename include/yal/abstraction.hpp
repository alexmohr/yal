//
// Copyright (c) 2022 Alexander Mohr
// Licensed under the terms of the MIT License
//

#ifndef YAL_ABSTRACTION_HPP
#define YAL_ABSTRACTION_HPP

#ifdef HAVE_ARDUINO
#include <Arduino.h>
#endif

#ifndef delay
#include <chrono>
#include <thread>
void delay(unsigned long millis);
#endif

// alias string to String if String does not exist
// This way we use the Arduino String implementation if it's available instead of
// std::string

#ifndef String
using String = std::string;
#endif

#endif // YAL_ABSTRACTION_HPP
