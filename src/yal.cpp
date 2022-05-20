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

std::size_t Logger::addAppender(Appender&& appender)
{
  s_appender.emplace_back(std::move(appender));
  return s_appender.size();
}

void Logger::removeAppender(AppenderId appenderId)
{
  const auto element = s_appender.begin() + static_cast<long>(appenderId);
  s_appender.erase(element);
}

void Logger::setTimeFunc(GetTime&& func)
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

std::stringstream Logger::messagePrefix(const Level& level) const
{
  std::stringstream ss;
  static constexpr const auto timeWidth = 20;
  const auto time = s_getTime();
  if (!time.empty()) {
    ss << "[" << std::setfill('0') << std::setw(timeWidth) << s_getTime() << "]";
  }
  ss << "[" << level.str() << "]";
  if (!m_context.empty()) {
    ss << "[" << m_context << "]";
  }
  ss << " ";

  return ss;
}

} // namespace yal
