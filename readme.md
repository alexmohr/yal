# YAL (Yet another logger)

This repo contains a logger library which I created for my personal arduino / ESP projects

- Why should I use this instead of other established loggers?
  - You really shouldn't.

## Building
This logger requires C++17 and thus a specific xtensa and platformio
version has to be used.

### ESP8266
In your `platformio.ini` insert the following lines: 
```ini
platform: espressif8266@3.2.0
platform_packages = toolchain-xtensa@~2.100300.0
```

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
The format of the logger can be configured by calling `setFormat` wit 
a given format string. 
Possible options are:
 * `%t` time
 * `%m` message
 * `%c` context
 * `%l` level

For example to configure a format which prints the level and the message
call `setFormat("%l %m)`.
Log output for an info message with text `test` will be `INFO test`

The format defaults to `[%t][%l][%c] %m`.
If no context is given for a logger `default` will be used
