//
// Copyright (c) 2022 Alexander Mohr
// Licensed under the terms of the MIT License
//

#include <gtest/gtest.h>
#include <yal/yal.hpp>
#include <string>

using std::string_literals::operator""s;

class TestAppender : public yal::Appender {
  public:
  explicit TestAppender(yal::AppenderStorage* storage) : yal::Appender(storage)
  {
  }

  [[nodiscard]] yal::AppenderId id() const
  {
    return m_appenderId;
  }

  [[nodiscard]] const std::string& lastMsg() const
  {
    return m_lastMsg;
  }

  [[nodiscard]] static const std::string& time()
  {
    return m_expectedTime;
  }

  [[nodiscard]] bool called() const
  {
    return m_called;
  }

  void resetCalled()
  {
    m_called = false;
  }

  protected:
  void append(const yal::Level& level, const char* text) override
  {
    m_called = true;
    m_lastMsg = text;
  }

  private:
  std::string m_ctx;
  std::string m_loggerText;
  std::string m_lastMsg;
  bool m_called = false;
  static inline const std::string m_expectedTime = "00000000000123456789";
};

class LoggerTest : public testing::Test {
  protected:
  void SetUp() override
  {
    yal::Logger::setFormat("[%t][%l][%c] %m");
    yal::Logger::setTimeFunc([]() { return "123456789"; });
  }

  static void setFormatAndExpectLogEqual(
    const std::string& format,
    const std::string& expected)
  {
    yal::Logger logger(m_formatTestCtx);
    const TestAppender appender(&logger);
    yal::Logger::setFormat(format.c_str());
    logger.log(yal::Level::INFO, m_formatTestMsg.c_str());
    EXPECT_TRUE(appender.called());
    EXPECT_STREQ(expected.c_str(), appender.lastMsg().c_str());
  }

  static inline const auto m_formattedExpect
    = "[00000000000123456789][DEBUG][test] logger test 42 bar 3.15";

  static inline const std::string m_formatTestMsg = "test";
  static inline const std::string m_formatTestCtx = "ctx";
};

TEST_F(LoggerTest, loggerNoFormat)
{
  const auto ctx = "no-format";
  yal::Logger logger(ctx);
  const auto loggerText = "this logger has no format";
  const TestAppender appender(&logger);
  const auto expected
    = "[" + TestAppender::time() + "][DEBUG][" + ctx + "] " + loggerText;
  logger.log(yal::Level::DEBUG, loggerText);
  EXPECT_EQ(appender.lastMsg(), expected);
  EXPECT_NE(0, appender.id());
  logger.removeAppender(appender.id());
}

TEST_F(LoggerTest, loggerFormat)
{
  yal::Logger logger("test");
  const TestAppender appender(&logger);

  logger.log(yal::Level::DEBUG, "logger test % bar %", 42, 3.15);
  EXPECT_EQ(appender.lastMsg(), m_formattedExpect);
}

TEST_F(LoggerTest, multipleAppenders)
{
  yal::Logger logger("test");
  const TestAppender appender1(&logger);
  const TestAppender appender2(&logger);
  EXPECT_NE(appender1.id(), appender2.id());
  logger.log(yal::Level::DEBUG, "logger test % bar %", 42, 3.15);
  EXPECT_EQ(appender1.lastMsg(), m_formattedExpect);
  EXPECT_EQ(appender2.lastMsg(), m_formattedExpect);
  logger.removeAppender(appender1.id());
  logger.removeAppender(appender1.id());
}

TEST_F(LoggerTest, setLimit)
{
  yal::Logger logger;
  TestAppender appender(&logger);

  for (auto level = static_cast<int>(yal::Level::TRACE);
       level < static_cast<int>(yal::Level::OFF);
       ++level) {
    const auto minLevel = static_cast<yal::Level::Value>(level);
    yal::Logger::setLevel(minLevel);
    EXPECT_EQ(yal::Logger::level().value(), minLevel);

    const auto resetAndLog = [&](const int level, bool result) {
      appender.resetCalled();
      logger.log(static_cast<yal::Level::Value>(level), "test");
      EXPECT_EQ(appender.called(), result);
    };

    for (auto smallerLevel = 0; smallerLevel < minLevel; ++smallerLevel) {
      resetAndLog(smallerLevel, false);
    }

    for (auto largerLevel = static_cast<int>(minLevel);
         largerLevel < static_cast<int>(yal::Level::OFF);
         ++largerLevel) {
      resetAndLog(largerLevel, true);
    }
  }

  yal::Logger::setLevel(yal::Level::TRACE);
}

TEST_F(LoggerTest, formatEmpty)
{
  setFormatAndExpectLogEqual("", "");
}

TEST_F(LoggerTest, formatMsg)
{
  setFormatAndExpectLogEqual("%m", m_formatTestMsg);
}

TEST_F(LoggerTest, formatTime)
{
  setFormatAndExpectLogEqual("%t", TestAppender::time());
}

TEST_F(LoggerTest, formatContext)
{
  setFormatAndExpectLogEqual("%c", m_formatTestCtx);
}

TEST_F(LoggerTest, formatLevel)
{
  yal::Level level(yal::Level::INFO);
  setFormatAndExpectLogEqual("%l", level.str());
}

TEST_F(LoggerTest, formatPercent)
{
  setFormatAndExpectLogEqual("%", "%");
}

TEST_F(LoggerTest, formatCombined)
{
  const auto expected = TestAppender::time() + " FIXED % bar " + m_formatTestMsg;
  setFormatAndExpectLogEqual("%t FIXED % bar %m", expected);
}

// todo write operator tests for level
