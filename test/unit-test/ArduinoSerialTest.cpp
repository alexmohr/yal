//
// Copyright (c) 2022 Alexander Mohr
// Licensed under the terms of the MIT License
//
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <yal/appender/ArduinoSerial.hpp>
#include <yal/yal.hpp>
#include <string>
using std::string_literals::operator""s;

class HardwareSerial {
  public:
  MOCK_METHOD1(begin, void(unsigned long));
  MOCK_METHOD1(print, void(const char* const text));
  MOCK_METHOD1(println, void(const char* const text));
};

class SerialAppenderTest : public testing::Test {
  void SetUp() override
  {
    yal::Logger::setTimeFunc([]() { return "123456789"; });
  }

  protected:
  static inline const auto m_expectedMsg = "[00000000000123456789][DEBUG] test"s;
  yal::Logger m_logger = yal::Logger();

  HardwareSerial m_serial;
};

TEST_F(SerialAppenderTest, coloredLogger)
{
  const yal::appender::ArduinoSerial<HardwareSerial> appender(&m_serial, true);

  // color for debug
  EXPECT_CALL(m_serial, print("\x1B[1;37m"));
  EXPECT_CALL(m_serial, println(testing::StrEq(m_expectedMsg)));
  m_logger.log(yal::Level::DEBUG, "test");
}

TEST_F(SerialAppenderTest, nonColoredLogger)
{
  const yal::appender::ArduinoSerial<HardwareSerial> appender(&m_serial, false);
  EXPECT_CALL(m_serial, print(testing::_)).Times(0);
  EXPECT_CALL(m_serial, println(testing::StrEq(m_expectedMsg)));
  m_logger.log(yal::Level::DEBUG, "test");
}

TEST_F(SerialAppenderTest, unregister)
{
  yal::appender::ArduinoSerial<HardwareSerial> appender(&m_serial, true);
  appender.unregister();

  {
    const yal::appender::ArduinoSerial<HardwareSerial> appender2(&m_serial, true);
  }

  EXPECT_CALL(m_serial, println(::testing::_)).Times(0);
  m_logger.log(yal::Level::DEBUG, "test");
}
