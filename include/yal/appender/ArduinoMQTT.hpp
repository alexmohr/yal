//
// Copyright (c) 2022 Alexander Mohr
// Licensed under the terms of the MIT License
//

#ifndef YAL_SERIALAPPENDER_H
#define YAL_SERIALAPPENDER_H

#include <yal/abstraction.hpp>
#include <yal/appender/Base.hpp>
#include <yal/yal.hpp>
#include <queue>
#include <utility>

namespace yal::appender {

// using String because MQTT library is expecting them
// this avoids conversions
struct MqttMessage {
  String message;
  String topic;
};

template<typename MQTT>
class ArduinoMQTT : public Base {
  public:
  explicit ArduinoMQTT(MQTT* mqtt, String topic) :
      m_mqtt(std::move(mqtt)), m_topic(std::move(topic))
  {
  }

  ArduinoMQTT(const ArduinoMQTT&) = delete;

  ~ArduinoMQTT() override
  {
    flush();
  }

  /**
   * Flush buffered messages to mqtt.
   * Only call this when not running an ISR
   */
  void flush()
  {
    while (!m_mqtt_msg_queue.empty()) {
      const auto& msg = m_mqtt_msg_queue.front();
      m_mqtt->publish(msg.topic, msg.message);
      m_mqtt_msg_queue.pop();
    }
  }

  std::queue<MqttMessage>& queue()
  {
    return m_mqtt_msg_queue;
  }

  protected:
  void append(const Level& level, const String& text) override
  {
    m_mqtt_msg_queue.push({m_topic, text});
  }

  private:
  MQTT* const m_mqtt;
  const String m_topic;

  std::queue<MqttMessage> m_mqtt_msg_queue;
};

} // namespace yal::appender

#endif // YAL_SERIALAPPENDER_H
