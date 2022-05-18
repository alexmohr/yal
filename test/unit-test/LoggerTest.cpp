//
// Copyright (c) 2022 Alexander Mohr
// Licensed under the terms of the MIT License
//

#include <gtest/gtest.h>
#include <yal/yal.hpp>
#include <string>

using std::string_literals::operator""s;

class LoggerTest : public testing::Test {
  void SetUp() override
  {
    yal::Logger::setTimeFunc([]() { return "123456789"; });
  }

  protected:
  static inline const auto m_expectedTime = "[00000000000123456789]"s;
  static inline const auto m_formattedExpect
    = "[00000000000123456789][DEBUG][test] logger test 42 bar 3.15";
};

TEST_F(LoggerTest, loggerNoFormat)
{
  const auto ctx = "no-format";
  const yal::Logger logger(ctx);
  const auto loggerText = "this logger has no format";
  auto appenderId
    = yal::Logger::addAppender([&](const yal::Level& level, const std::string& text) {
        const auto expected = m_expectedTime + "[DEBUG][" + ctx + "] " + loggerText;
        EXPECT_EQ(text, expected);
      });

  EXPECT_NE(0, appenderId);
  logger.log(yal::Level::DEBUG, loggerText);
  yal::Logger::removeAppender(appenderId);
}

TEST_F(LoggerTest, loggerFormat)
{
  yal::Logger logger("test");
  auto appenderId
    = yal::Logger::addAppender([&](const yal::Level& level, const std::string& text) {
        EXPECT_EQ(text, m_formattedExpect);
      });
  logger.log(yal::Level::DEBUG, "logger test % bar %", 42, 3.15);
  yal::Logger::removeAppender(appenderId);
}

TEST_F(LoggerTest, multipleAppenders)
{
  yal::Logger logger("test");
  auto appenderId1
    = yal::Logger::addAppender([&](const yal::Level& level, const std::string& text) {
        EXPECT_EQ(text, m_formattedExpect);
      });
  auto appenderId2
    = yal::Logger::addAppender([&](const yal::Level& level, const std::string& text) {
        EXPECT_EQ(text, m_formattedExpect);
      });
  logger.log(yal::Level::DEBUG, "logger test % bar %", 42, 3.15);
  yal::Logger::removeAppender(appenderId1);
  yal::Logger::removeAppender(appenderId2);
}
