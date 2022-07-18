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
  void SetUp() override {
    yal::Logger::setTimeFunc([]() { return "123456789"; });
  }
};

class MQTT {
 public:
  MQTT() = default;
  MQTT(MQTT&) = delete;
  MOCK_METHOD2(publish, void(std::string, std::string));
  MOCK_METHOD1(subscribe, void(const std::string&));
};

TEST_F(ArduinoMQTTTest, expectFlushAtDestruction) {
  MQTT mqtt;
  yal::Logger logger;
  const auto topic = "/test";
  EXPECT_CALL(
    mqtt, publish(topic, testing::StrEq("[00000000000123456789][DEBUG][default] bar")))
    .Times(1);
  {
    yal::appender::ArduinoMQTT<MQTT> appender(&logger, &mqtt, topic);
    logger.log(yal::Level::DEBUG, "bar");
  }
}

TEST_F(ArduinoMQTTTest, changeLevel) {
  MQTT mqtt;
  yal::Logger logger;
  const auto initLevel = yal::Level::FATAL;
  yal::Logger::setLevel(initLevel);
  yal::appender::ArduinoMQTT<MQTT> appender(&logger, &mqtt, "/log");

  // run expects in sequence
  testing::InSequence seq;

  const auto changeLevelTopic = "changeLevel";

  EXPECT_CALL(mqtt, subscribe(changeLevelTopic));
  appender.registerChangeLevelTopic(changeLevelTopic);

  // wrong topic
  appender.onMessageReceived("not-log-topic", "Foobar");
  EXPECT_EQ(logger.level().value(), initLevel);

  // wrong value
  appender.onMessageReceived(changeLevelTopic, "Foobar");
  EXPECT_EQ(logger.level().value(), initLevel);

  // string instead int
  appender.onMessageReceived(changeLevelTopic, "DEBUG");
  EXPECT_EQ(logger.level().value(), initLevel);

  // correct call
  yal::Level level(yal::Level::DEBUG);
  appender.onMessageReceived(
    changeLevelTopic, std::to_string(static_cast<int>(level.value())).c_str());
  EXPECT_EQ(logger.level().value(), level.value());
}

TEST_F(ArduinoMQTTTest, setTopic) {
  MQTT mqtt;
  yal::Logger logger;
  yal::appender::ArduinoMQTT<MQTT> appender(&logger, &mqtt, "/log");
  const std::string newTopic = "bar";
  appender.setTopic(newTopic);
  EXPECT_STREQ(newTopic.c_str(), appender.topic());
}
