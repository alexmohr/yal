//
// Copyright (c) 2022 Alexander Mohr
// Licensed under the terms of the MIT license
//
#ifndef YAL_BASE_HPP
#define YAL_BASE_HPP

#include "yal/yal.hpp"
namespace yal::appender {
class Base {
  public:
  void unregister()
  {
    if (m_appenderId != AppenderIdNotSet) {
      yal::Logger::removeAppender(m_appenderId);
      m_appenderId = AppenderIdNotSet;
    }
  }

  protected:
  virtual ~Base()
  {
    unregister();
  }

  Base() :
      m_appenderId(Logger::addAppender(
        [this](const Level& level, const char* text) { append(level, text); }))
  {
  }

  virtual void append(const Level& level, const char* text) = 0;

  AppenderId m_appenderId;
};
} // namespace yal::appender

#endif // YAL_BASE_HPP
