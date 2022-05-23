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
