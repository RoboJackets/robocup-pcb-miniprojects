add_library(driver driver.cpp)
target_include_directories(driver PUBLIC ${CMAKE_CURRENT_LIST_DIR})
target_link_libraries(driver pico_stdlib hardware_i2c)