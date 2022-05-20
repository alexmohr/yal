//
// Copyright (c) 2022 Alexander Mohr
// Licensed under the terms of the MIT License
//

#ifndef YAL_YAL_HPP
#define YAL_YAL_HPP

#include <yal/Level.hpp>
#include <yal/abstraction.hpp>
#include <cstddef>
#include <functional>
#include <limits>
#include <sstream>
#include <vector>

namespace yal {

using Appender = std::function<void(const Level& level, const std::string& text)>;
using AppenderId = std::size_t;
static constexpr const auto AppenderIdNotSet = std::numeric_limits<AppenderId>::max();
using GetTime = std::function<std::string()>;

class Logger {
  public:
  Logger() = default;
  explicit Logger(std::string ctx);
  Logger(const Logger&) = delete;
  Logger(Logger&&) = default;
  void operator=(const Logger&) = delete;

  static AppenderId addAppender(Appender&& appender);
  static void removeAppender(AppenderId appenderId);

  static void setTimeFunc(GetTime&& func);

  static void setLevel(const Level& level);
  [[nodiscard]] static const Level& level();

  void log(const Level& level, const char* const text) const
  {
    // discard message is level is turned off
    if (!levelEnabled(level)) {
      return;
    }

    auto ss = messagePrefix(level);
    ss << text;
    const auto msg = ss.str();
    for (const auto& appender : s_appender) {
      appender(level, msg);
    }
  }

  template<typename T, typename... Targs>
  void log(const Level& level, const char* format, T value, Targs... args) const
  {
    // discard message is level is turned off
    if (!levelEnabled(level)) {
      return;
    }

    auto ss = messagePrefix(level);
    buildMessage(ss, format, value, args...);

    const auto str = ss.str();
    for (const auto& appender : s_appender) {
      appender(level, str);
    }
  }

  private:
  [[nodiscard]] static bool levelEnabled(const Level& level)
  {
    return s_level >= level && level <= Level::OFF;
  }

  template<typename T, typename... Targs>
  void buildMessage(std::stringstream& ss, const char* format, T value, Targs... args)
    const
  {
    for (; *format != '\0'; format++) {
      if (*format == '%') {
        ss << value;
        buildMessage(ss, format + 1, args...); // recursive call
        return;
      }
      ss << *format;
    }
  }

  static void buildMessage(std::stringstream& stream, const char* format) // base function
  {
    if (*format != '\0') {
      stream << *format;
    }
  }

  [[nodiscard]] std::stringstream messagePrefix(const Level& level) const;

  static inline Level s_defaultLevel = Level::DEBUG;
  static inline GetTime s_getTime = []() { return std::to_string(millis()); };
  static inline std::vector<Appender> s_appender;
  static inline Level s_level = s_defaultLevel;
  std::string m_context;
};
} // namespace yal

#endif // YAL_YAL_HPP
