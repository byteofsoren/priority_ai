cmake_minimum_required(VERSION 2.8.3)
project(priority_ai)

## Compile as C++11, supported in ROS Kinetic and newer
# add_compile_options(-std=c++11)
#add_compile_options(-lsfml-audio)
#add_compile_options(-lsfml-system)


## Find catkin macros and libraries
## if COMPONENTS list like find_package(catkin REQUIRED COMPONENTS xyz)
## is used, also find other catkin packages
find_package(catkin REQUIRED COMPONENTS
  roscpp
  std_msgs
  message_generation
  yolo_depth_fusion
  blindy_findy
  roslib
)

## System dependencies are found with CMake's conventions
# find_package(Boost REQUIRED COMPONENTS system)



## Generate added messages and services with any dependencies listed here
generate_messages(
  DEPENDENCIES
  #yolo_depth_fusion
  std_msgs
)
catkin_package(
   CATKIN_DEPENDS
      message_runtime
)


include(CheckCXXCompilerFlag)
check_cxx_compiler_flag(-std=c++11 HAS_STD_CPP11_FLAG)
if(HAS_STD_CPP11_FLAG)
  add_compile_options(-std=c++11)
endif()


include_directories(
# include
  ${catkin_INCLUDE_DIRS}
)

## Declare a C++ library
# add_library(${PROJECT_NAME}
#   src/${PROJECT_NAME}/priority_ai.cpp
# )

## Add cmake target dependencies of the library
## as an example, code may need to be generated before libraries
## either from message generation or dynamic reconfigure
# add_dependencies(${PROJECT_NAME} ${${PROJECT_NAME}_EXPORTED_TARGETS} ${catkin_EXPORTED_TARGETS})

## Declare a C++ executable
## With catkin_make all packages are built within a single CMake context
## The recommended prefix ensures that target names across packages don't collide
add_executable(${PROJECT_NAME}
   src/main.cpp
   src/sound.cpp
   src/fuzzy_logic.cpp
   src/test.cpp
   src/rostopic.cpp
)

## Rename C++ executable without prefix
## The above recommended prefix causes long target names, the following renames the
## target back to the shorter version for ease of user use
## e.g. "rosrun someones_pkg node" instead of "rosrun someones_pkg someones_pkg_node"
# set_target_properties(${PROJECT_NAME}_node PROPERTIES OUTPUT_NAME node PREFIX "")

## Add cmake target dependencies of the executable
## same as for the library above
add_dependencies(${PROJECT_NAME} ${${PROJECT_NAME}_EXPORTED_TARGETS} ${catkin_EXPORTED_TARGETS})

## Specify libraries to link a library or executable target against
target_link_libraries(${PROJECT_NAME}
   sfml-audio
   sfml-system
   ${catkin_LIBRARIES}
)


