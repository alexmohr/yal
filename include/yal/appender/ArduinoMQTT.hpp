//
// Copyright (c) 2022 Alexander Mohr
// Licensed under the terms of the MIT License
//

#ifndef YAL_MQTTAPPENDER
#define YAL_MQTTAPPENDER

#include <yal/abstraction.hpp>
#include <yal/yal.hpp>
#include <queue>
#include <utility>

namespace yal::appender {

// using std::string because MQTT library is expecting them
// this avoids conversions
struct MqttMessage {
  String topic;
  String message;
};

template<typename MQTT>
class ArduinoMQTT : public Appender {
 public:
  /**
   * Create a new ArduinoMQTT
   * @param storage pointer to appender storage, might be instance of logger
   * @param mqtt pointer to the MQTT instance
   * @param topic topic as const char* const
   *              to be compatible with std::string and String
   */
  explicit ArduinoMQTT(
    AppenderStorage* storage,
    MQTT* mqtt,
    const char* const topic,
    const std::string& format = yal::Logger::DEFAULT_FORMAT) :
      Appender(storage, format),
      m_mqtt(std::move(mqtt)),
      m_topic(topic),
      m_logger(Logger("ArduinoMQTT")) {
  }

  ArduinoMQTT(const ArduinoMQTT&) = delete;
  ArduinoMQTT(const ArduinoMQTT&&) = delete;

  ~ArduinoMQTT() override {
    flush();
  }

  /**
   * Register a mqtt topic which changes the logging level
   * To change the topic via MQTT:
   * mosquitto_pub -h $HOST -t "$TOPIC/debug/loglevel" -m "$LEVEL" -r
   * Level can be a value between 0 (the highest log level = all logs)
   * and 6 (lowest log level = no logs)
   * You have to call onMessageReceived in your MQTT callback
   * @param topic This is the MQTT topic which the logger subscribes to
   */
  void registerChangeLevelTopic(const char* const topic) {
    m_mqtt->subscribe(topic);
    m_changeLevelTopic = topic;
  }

  /**
   * Call this method, if you've enabled a topic handler,
   * in your mqtt message callback.
   * @param topic
   * @param text
   */
  void onMessageReceived(const char* topic, const char* const text) {
    if (topic == m_changeLevelTopic) {
      changeLevel(text);
    }
  }

  /**
   * Flush buffered messages to mqtt.
   * Only call this when not running an ISR
   */
  void flush() {
    while (!m_mqtt_msg_queue.empty()) {
      const auto& msg = m_mqtt_msg_queue.front();
      m_mqtt->publish(msg.topic, msg.message);
      m_mqtt_msg_queue.pop();
    }
  }

  /**
   * Get the mqtt message queue
   * You can use this to append other message aside from logging,
   * so only one queue is necessary
   * Send the queue by calling flush when it's safe to do so.
   * Do not run this in an ISR (i.e. when the message received callback is
   * running)
   */
  std::queue<MqttMessage>& queue() {
    return m_mqtt_msg_queue;
  }

  /**
   * Set the MQTT topic which will be used for logging
   */
  void setTopic(const std::string& topic) {
    m_topic = topic;
  }

  /**
   * Get the currently set mqtt topic
   */
  const char* topic() {
    return m_topic.c_str();
  }

 protected:
  void append(const Level& level, const char* text) override {
    m_mqtt_msg_queue.push({String(m_topic.c_str()), String(text)});
  }

 private:
  void changeLevel(const char* const levelValue) {
    std::stringstream ss(levelValue);
    int level;
    if (!(ss >> level)) {
      m_logger.log(Level::ERROR, "% is not a valid log level", levelValue);
      return;
    }

    yal::Logger::setLevel(static_cast<Level::Value>(level));
  }

  MQTT* const m_mqtt;
  std::string m_topic;
  std::string m_changeLevelTopic;

  std::queue<MqttMessage> m_mqtt_msg_queue;
  Logger m_logger;
};

}  // namespace yal::appender

#endif  // YAL_MQTTAPPENDER
