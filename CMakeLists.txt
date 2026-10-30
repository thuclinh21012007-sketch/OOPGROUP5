cmake_minimum_required(VERSION 3.14)
project(MultiCloudCostOptimizer LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/common
    ${CMAKE_CURRENT_SOURCE_DIR}/tv1
    ${CMAKE_CURRENT_SOURCE_DIR}/tv2
    ${CMAKE_CURRENT_SOURCE_DIR}/tv3
    ${CMAKE_CURRENT_SOURCE_DIR}/tv4
    ${CMAKE_CURRENT_SOURCE_DIR}/tv5
)

# Common library files
set(SOURCE_FILES
    tv2/PricingAdapters.cpp
    tv3/CostCalculator.cpp
    tv3/SimulationEngine.cpp
    tv3/MultiCloudOptimizationService.cpp
    tv3/TestSuite.cpp
    tv4/CheapestProviderStrategy.cpp
    tv4/RightSizingStrategy.cpp
    tv4/PricingModelStrategy.cpp
    tv5/ConsoleUI.cpp
)

# Main integrated executable
add_executable(multi_cloud_optimizer main.cpp ${SOURCE_FILES})

# Individual member demos
add_executable(demo_tv1 tv1/main_tv1.cpp)
add_executable(demo_tv2 tv2/main_tv2.cpp tv2/PricingAdapters.cpp)
add_executable(demo_tv3 tv3/main_tv3.cpp tv2/PricingAdapters.cpp tv3/CostCalculator.cpp tv3/SimulationEngine.cpp tv3/TestSuite.cpp)
add_executable(demo_tv4 tv4/main_tv4.cpp tv2/PricingAdapters.cpp tv3/CostCalculator.cpp tv3/SimulationEngine.cpp tv4/CheapestProviderStrategy.cpp tv4/RightSizingStrategy.cpp tv4/PricingModelStrategy.cpp)
add_executable(demo_tv5 tv5/main_tv5.cpp ${SOURCE_FILES})
