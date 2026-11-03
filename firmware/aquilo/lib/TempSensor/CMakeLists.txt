# add TempSensor header and code files as a library
add_library(TempSensor STATIC
  TempSensor.hpp
  TempSensor.cpp
)

# include the source directory of the drivers
target_include_directories(TempSensor PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}
)

# link dependencies of TempSensor
target_link_libraries(TempSensor PUBLIC
  hardware_i2c
)
