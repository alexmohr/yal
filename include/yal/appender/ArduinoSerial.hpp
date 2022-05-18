//
// Copyright (c) 2022 Alexander Mohr
// Licensed under the terms of the MIT License
//

#ifndef YAL_SERIALAPPENDER_H
#define YAL_SERIALAPPENDER_H
#include <yal/abstraction.hpp>
#include <yal/appender/Base.hpp>
#include <yal/yal.hpp>

namespace yal::appender {
template<typename HardwareSerial>
class ArduinoSerial : public Base {
  public:
  ArduinoSerial(HardwareSerial* serial, bool colored) :
      Base(), m_serial(serial), m_colored(colored)
  {
  }

  ArduinoSerial(const ArduinoSerial&) = delete;

  ~ArduinoSerial() override
  {
    unregister();
  }

  /**
   * Can be used to initialize the serial port
   * @param baud baud rate to use
   */
  void begin(const unsigned long baud)
  {
    m_serial->begin(baud);
    while (!m_serial) {
      delay(50);
    }
    // print new empty line to clear output
    m_serial->println("");
  }

  protected:
  void append(const Level& level, const std::string& text) override
  {
    if (m_colored) {
      m_serial->print(LEVEL_COLOR[static_cast<unsigned int>(level)]);
    }

    m_serial->println(text.c_str());
  }

  private:
  HardwareSerial* m_serial = nullptr;
  bool m_colored;

  static constexpr const char* const COLOR_TRACE = "\033[1;37m";
  static constexpr const char* const COLOR_DEBUG = "\033[1;37m";
  static constexpr const char* const COLOR_INFO = "\033[1;32m";
  static constexpr const char* const COLOR_WARNING = "\033[1;33m";
  static constexpr const char* const COLOR_ERROR = "\033[1;31m";
  static constexpr const char* const COLOR_FATAL = "\033[1;31m";
  static constexpr const char* const COLOR_OFF = "\033[1;31m";
  static constexpr const char* const LEVEL_COLOR[]{
    COLOR_TRACE,
    COLOR_DEBUG,
    COLOR_INFO,
    COLOR_WARNING,
    COLOR_ERROR,
    COLOR_FATAL,
    COLOR_OFF,
  };
};

} // namespace yal::appender

#endif // YAL_SERIALAPPENDER_H
