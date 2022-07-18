# YAL (Yet another logger)

This repo contains a logger library which I created for my personal arduino / ESP projects

This logger requires C++17 and thus a specific xtensa and platformio
version has to be used.

### ESP8266
In your `platformio.ini` insert the following lines: 

```ini
build_flags =
    -std=gnu++17
    -DYAL_ARDUINO_SUPPORT=true
```
* YAL_ARDUINO_SUPPORT must set so `yal` is not defining arduino functions 
  which are normally defined for testing
* -std=gnu++17 sets the compiler compatibility to C++17

The pinned version is necessary to have the correct compiler versions 
for C++17 support.

An example platformio project can be found in the `platformio` subfolder

### Other platforms
No official support for other platforms exist. 
If you need support, feel free to open an issue.

## Available Loggers 
* Arduino MQTT
  * This depends on the `MQTT` library 
* Arduino Serial
  * No deps are required

## Format
Each appender can be configured with its own format.

Possible options are:
 * `%t` time
 * `%m` message
 * `%c` context
 * `%l` level

For example to configure a format which prints the level and the message

The format defaults to `[%t][%l][%c] %m`.
If no context is given for an appender `default` will be used
