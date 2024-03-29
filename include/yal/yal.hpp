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
#include <string>
#include <utility>
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
  Appender(AppenderStorage* storage, std::string format) :
      m_appenderStore(storage),
      m_appenderId(m_appenderStore->addAppender(this)),
      m_format(std::move(format)) {
  }

  Appender(Appender&&) = default;
  Appender(const Appender&) = delete;
  // must be deleted due to const fields
  Appender& operator=(Appender&& other) = delete;

  virtual ~Appender() {
    unregister();
  }

  virtual void append(const Level& level, const char* text) = 0;

  void unregister() {
    if (m_appenderId != AppenderIdNotSet) {
      m_appenderStore->removeAppender(m_appenderId);
      m_appenderId = AppenderIdNotSet;
    }
  }

  [[nodiscard]] virtual const std::string& format() const {
    return m_format;
  }

  void setFormat(const std::string& format) {
    m_format = format;
  }

 protected:
  AppenderStorage* const m_appenderStore{};
  AppenderId m_appenderId{};
  std::string m_format;
};

class Logger : public AppenderStorage {
 public:
  static constexpr const auto FORMAT_TIME = 't';
  static constexpr const auto FORMAT_MSG = 'm';
  static constexpr const auto FORMAT_CONTEXT = 'c';
  static constexpr const auto FORMAT_LEVEL = 'l';
  static inline std::string DEFAULT_FORMAT = "[%t][%l][%c] %m";

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

  void log(const Level& level, const char* text) const {
    log(level, text, nullptr);
  }

  template<typename T, typename... Targs>
  void log(const Level& level, const char* format, T value, Targs... args) const {
    // discard message is level is turned off
    if (!levelEnabled(level)) {
      return;
    }

    for (const auto& appenderPair : s_appender) {
      std::stringstream ss;
      const auto& appender = appenderPair.second;
      const auto& appenderFormat = appender->format();
      if (appenderFormat.empty()) {
        continue;
      }

      for (auto i = 0U; i < appenderFormat.size(); ++i) {
        const auto& currentChar = appenderFormat.at(i);
        if (currentChar != '%' || i + 1 == appenderFormat.size()) {
          ss << currentChar;
          continue;
        }

        const auto& formatChar = appenderFormat.at(++i);
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
            ss << "%" << formatChar;
            break;
        }
      }
      appender->append(level, ss.str().c_str());
    }
  }

 private:
  [[nodiscard]] static bool levelEnabled(const Level& level) {
    return level >= s_level && level <= Level::OFF;
  }

  template<typename T, typename... Targs>
  void buildMessage(std::stringstream& ss, const char* format, T value, Targs... args)
    const {
    for (; *format != '\0'; ++format) {
      if (*format == '%') {
        ss << value;
        buildMessage(ss, format + 1, args...);  // recursive call
        return;
      }
      ss << *format;
    }
  }

  static void buildMessage(
    std::stringstream& stream,
    const char* format)  // base function
  {
    if (format != nullptr && *format != '\0') {
      stream << *format;
    }
  }
  static inline Level s_defaultLevel = Level::DEBUG;
  static inline TimeFunc s_getTime = []() { return std::to_string(millis()); };
  static inline std::map<AppenderId, Appender*> s_appender;
  static inline Level s_level = s_defaultLevel;

  std::string m_context = "default";
};

}  // namespace yal

#endif  // YAL_YAL_HPP
