//
// Copyright (c) 2022 Alexander Mohr
// Licensed under the terms of the MIT License
//

#ifndef YAL_MQTTAPPENDER
#define YAL_MQTTAPPENDER

#include <yal/abstraction.hpp>
#include <yal/appender/Base.hpp>
#include <yal/yal.hpp>
#include <queue>
#include <utility>

namespace yal::appender {

// using std::string because MQTT library is expecting them
// this avoids conversions
struct MqttMessage {
  String message;
  String topic;
};

template<typename MQTT>
class ArduinoMQTT : public Base {
  public:
  explicit ArduinoMQTT(MQTT* mqtt, std::string topic) :
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
  void append(const Level& level, const std::string& text) override
  {
    m_mqtt_msg_queue.push({String(m_topic.c_str()), String(text.c_str())});
  }

  private:
  MQTT* const m_mqtt;
  const std::string m_topic;

  std::queue<MqttMessage> m_mqtt_msg_queue;
};

} // namespace yal::appender

#endif // YAL_MQTTAPPENDER
