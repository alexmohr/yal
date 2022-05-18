//
// Copyright (c) 2022 Alexander Mohr
// Licensed under the terms of the MIT License
//

#include <Arduino.h>
#include <MQTT.h>
#include <yal/appender/ArduinoMQTT.hpp>
#include <yal/appender/ArduinoSerial.hpp>
#include <yal/yal.hpp>

yal::Logger m_logger;
MQTTClient m_mqttClient;
const yal::appender::ArduinoSerial<HardwareSerial> m_serialAppender(&Serial, true);
yal::appender::ArduinoMQTT<MQTTClient> m_mqttAppender(&m_mqttClient, "/logTopic");

void setup()
{
  m_mqttClient.connect("mqtt-broker");
  m_logger.log(yal::Level::DEBUG, "setup test");
  m_logger.log(yal::Level::TRACE, "setup test");
  m_logger.log(yal::Level::DEBUG, "setup test");
  m_logger.log(yal::Level::INFO, "setup test");
  m_logger.log(yal::Level::WARNING, "setup test");
  m_logger.log(yal::Level::ERROR, "setup test");
  m_logger.log(yal::Level::FATAL, "setup test");

  // remember to flush the appender when it's safe to do so
  // Do NOT call this while in an ISR
  m_mqttAppender.flush();
}

void loop()
{
  m_logger.log(yal::Level::DEBUG, "loop test");
  m_logger.log(yal::Level::TRACE, "loop test");
  m_logger.log(yal::Level::DEBUG, "loop test");
  m_logger.log(yal::Level::INFO, "loop test");
  m_logger.log(yal::Level::WARNING, "loop test");
  m_logger.log(yal::Level::ERROR, "loop test");
  m_logger.log(yal::Level::FATAL, "loop test");

  // remember to flush the appender when it's safe to do so
  // Do NOT call this while in an ISR
  m_mqttAppender.flush();

  delay(100);
}
