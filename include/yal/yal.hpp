//
// Copyright (c) 2022 Alexander Mohr
// Licensed under the terms of the MIT License
//

#ifndef YAL_YAL_HPP
#define YAL_YAL_HPP

#include <yal/Level.hpp>
#include <yal/abstraction.hpp>
#include <array>
#include <cstddef>
#include <functional>
#include <iomanip>
#include <limits>
#include <map>
#include <sstream>
namespace yal {

using AppenderId = std::size_t;
static constexpr const auto AppenderIdNotSet = std::numeric_limits<AppenderId>::max();
using TimeFunc = std::function<std::string()>;

class Appender;
class AppenderStorage {
  public:
  [[nodiscard]] virtual AppenderId addAppender(Appender* appender) = 0;
  virtual void removeAppender(AppenderId appenderId) = 0;
};

class Appender {
  public:
  explicit Appender(AppenderStorage* storage) :
      m_appenderStore(storage), m_appenderId(m_appenderStore->addAppender(this))
  {
  }
  virtual ~Appender()
  {
    unregister();
  }
  virtual void append(const Level& level, const char* text) = 0;

  void unregister()
  {
    if (m_appenderId != AppenderIdNotSet) {
      m_appenderStore->removeAppender(m_appenderId);
      m_appenderId = AppenderIdNotSet;
    }
  }

  protected:
  AppenderStorage* const m_appenderStore{};
  AppenderId m_appenderId{};
};

class Logger : public AppenderStorage {
  public:
  Logger() = default;
  explicit Logger(std::string ctx);
  Logger(const Logger&) = delete;
  Logger(Logger&&) = default;
  void operator=(const Logger&) = delete;

  // Impl of AppenderStorage
  [[nodiscard]] AppenderId addAppender(Appender* appender) override;
  void removeAppender(AppenderId appenderId) override;

  static void setTimeFunc(TimeFunc&& func);
  static void setLevel(const Level& level);
  [[nodiscard]] static const Level& level();

  void log(const Level& level, const char* text) const
  {
    log(level, text, nullptr);
  }

  static constexpr const auto FORMAT_TIME = 't';
  static constexpr const auto FORMAT_MSG = 'm';
  static constexpr const auto FORMAT_CONTEXT = 'c';
  static constexpr const auto FORMAT_LEVEL = 'l';
  const char* const s_format = "[%t][%l][%c] %m";

  template<typename T, typename... Targs>
  void log(const Level& level, const char* format, T value, Targs... args) const
  {
    // discard message is level is turned off
    if (!levelEnabled(level)) {
      return;
    }

    std::stringstream ss;
    // auto ss = messagePrefix(level);
    auto msgFormat = s_format;
    for (; *msgFormat != '\0'; ++msgFormat) {
      if (*msgFormat != '%' || *(msgFormat + 1) == '\0') {
        ss << *msgFormat;
        continue;
      }

      const auto formatChar = *(++msgFormat);
      switch (formatChar) {
      case FORMAT_MSG:
        buildMessage(ss, format, value, args...);
        break;
      case FORMAT_TIME:
        static constexpr const auto timeWidth = 20;
        ss << std::setfill('0') << std::setw(timeWidth) << s_getTime();
        break;
      case FORMAT_LEVEL:
        ss << level.str();
        break;
      case FORMAT_CONTEXT:
        if (!m_context.empty()) {
          ss << m_context;
        }
        break;
      default:
        break;
      }
    }

    sendMsgToAppenders(level, ss);
  }

  static void sendMsgToAppenders(const Level& level, const std::stringstream& ss)
  {
    const auto str = ss.str();
    const auto cstr = str.c_str();
    for (const auto& appender : s_appender) {
      appender.second->append(level, cstr);
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
    for (; *format != '\0'; ++format) {
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
    if (format != nullptr && *format != '\0') {
      stream << *format;
    }
  }
  static inline Level s_defaultLevel = Level::DEBUG;
  static inline TimeFunc s_getTime = []() { return std::to_string(millis()); };
  static inline std::map<AppenderId, Appender*> s_appender;
  static inline Level s_level = s_defaultLevel;
  std::string m_context;
};

} // namespace yal

#endif // YAL_YAL_HPP
