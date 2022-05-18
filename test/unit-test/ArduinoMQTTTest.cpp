//
// Copyright (c) 2022 Alexander Mohr
// Licensed under the terms of the MIT License
//
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <yal/appender/ArduinoMQTT.hpp>
#include <yal/yal.hpp>
#include <string>

using std::string_literals::operator""s;

class ArduinoMQTTTest : public testing::Test {
  void SetUp() override
  {
    yal::Logger::setTimeFunc([]() { return "123456789"; });
  }

  protected:
  static inline const auto m_expectedTime = "[00000000000123456789]"s;
};

class MQTT {
  public:
  MQTT() = default;
  MQTT(MQTT&) = delete;
  MOCK_METHOD2(publish, void(std::string, std::string));
};

TEST_F(ArduinoMQTTTest, expectFlushAtDestruction)
{
  MQTT mqtt;
  yal::Logger logger;
  EXPECT_CALL(mqtt, publish(testing::_, testing::_)).Times(1);

  const auto topic = "/test";
  {
    yal::appender::ArduinoMQTT<MQTT> appender(&mqtt, topic);
    logger.log(yal::Level::DEBUG, "bar");
  }
}
