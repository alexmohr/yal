set(TARGET unit_tests)
find_package(GTest REQUIRED)
pkg_check_modules(GMock REQUIRED "gmock")
enable_testing()

add_executable(
        ${TARGET}
        LoggerTest.cpp
        ArduinoSerialTest.cpp
        ArduinoMQTTTest.cpp
)

target_link_libraries(
        ${TARGET}
        GTest::GTest GTest::Main ${GMock_LIBRARIES}
        yal
)

target_include_directories(${TARGET} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

include(GoogleTest)
gtest_discover_tests(${TARGET})
