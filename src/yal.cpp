//
// Copyright (c) 2022 Alexander Mohr
// Licensed under the terms of the MIT License
//

#include <yal/yal.hpp>
#include <iomanip>

namespace yal {

Logger::Logger(std::string ctx) : m_context(std::move(ctx))
{
}

std::size_t Logger::addAppender(Appender* appender)
{
  // last elements key + 1
  const auto newAppenderId = s_appender.empty() ? 1 : s_appender.rbegin()->first + 1;
  s_appender.insert({newAppenderId, appender});
  return newAppenderId;
}

void Logger::removeAppender(const AppenderId appenderId)
{
  const auto element = s_appender.find(appenderId);
  if (element != s_appender.end()) {
    s_appender.erase(element);
  }
}

void Logger::setTimeFunc(TimeFunc&& func)
{
  s_getTime = std::move(func);
}

void Logger::setLevel(const Level& level)
{
  s_level = level;
}

const Level& Logger::level()
{
  return s_level;
}

} // namespace yal
