//
// Copyright (c) 2022 Alexander Mohr
// Licensed under the terms of the MIT License
//

#include "yal/Appender.hpp"
#include <gtest/gtest.h>
#include <yal/yal.hpp>
#include <string>
#include <utility>

class LoggerTest : public testing::Test {
  void SetUp() override
  {
    yal::Logger::setTimeFunc([]() { return "123456789"; });
  }

  protected:
  static inline const auto m_formattedExpect
    = "[00000000000123456789][DEBUG][test] logger test 42 bar 3.15";
};

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
  static inline const std::string m_expectedTime = "[00000000000123456789]";
};

TEST_F(LoggerTest, loggerNoFormat)
{
  const auto ctx = "no-format";
  yal::Logger logger(ctx);
  const auto loggerText = "this logger has no format";
  const TestAppender appender(&logger);
  const auto expected = TestAppender::time() + "[DEBUG][" + ctx + "] " + loggerText;
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
    const auto maxLevel = static_cast<yal::Level::Value>(level);
    yal::Logger::setLevel(maxLevel);
    EXPECT_EQ(yal::Logger::level().value(), maxLevel);
    for (auto smallerLevel = 0; smallerLevel <= maxLevel; ++smallerLevel) {
      appender.resetCalled();
      logger.log(static_cast<yal::Level::Value>(smallerLevel), "test");
      EXPECT_TRUE(appender.called());
    }

    appender.resetCalled();
    logger.log(static_cast<yal::Level::Value>(maxLevel + 1), "test");
    EXPECT_FALSE(appender.called());
  }
}
