//
// Copyright (c) 2022 Alexander Mohr
// Licensed under the terms of the MIT License
//

#ifndef YAL_LEVEL_HPP
#define YAL_LEVEL_HPP

#include <yal/abstraction.hpp>
#include <array>
#include <string>

namespace yal {

class Level {
  public:
  enum Value { TRACE = 0, DEBUG, INFO, WARNING, ERROR, FATAL, OFF };

  constexpr Level(Value value) : m_level(value)
  {
  }

  [[nodiscard]] std::string str() const
  {
    return s_log_level_name[m_level];
  }

  inline bool operator>(const Level& other) const
  {
    return static_cast<int>(m_level) > static_cast<int>(other.m_level);
  }
  inline bool operator>=(const Level& other) const
  {
    return static_cast<int>(m_level) >= static_cast<int>(other.m_level);
  }
  inline bool operator<(const Level& other) const
  {
    return static_cast<int>(m_level) < static_cast<int>(other.m_level);
  }
  inline bool operator<=(const Level& other) const
  {
    return static_cast<int>(m_level) <= static_cast<int>(other.m_level);
  }
  inline bool operator==(const Level& other) const
  {
    return m_level == other.m_level;
  }

  explicit operator unsigned int() const
  {
    return static_cast<unsigned int>(m_level);
  }

  [[nodiscard]] const Value& value() const
  {
    return m_level;
  }

  private:
  Value m_level;

  static constexpr std::array s_log_level_name{
    "TRACE",
    "DEBUG",
    "INFO ",
    "WARN",
    "ERROR",
    "FATAL",
    "OFF  ",
  };
};
} // namespace yal

#endif // YAL_LEVEL_HPP
