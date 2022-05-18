//
// Copyright (c) 2022 Alexander Mohr
// Licensed under the terms of the MIT License
//

#ifndef YAL_ABSTRACTION_HPP
#define YAL_ABSTRACTION_HPP

#if !(HAVE_ARDUINO || YAL_ARDUINO_SUPPORT)

#ifndef delay
void delay(unsigned long millis);
#endif

#ifndef String
#include <string>
using String = std::string;
#endif

#endif

#endif // YAL_ABSTRACTION_HPP
