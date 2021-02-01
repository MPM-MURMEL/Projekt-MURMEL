#include <ros/ros.h>
#include "kinova_ros_controller/KinovaRosController.h"

int main(int argc, char** argv)
{
    ros::init(argc, argv, "kinova_ros_murmel_node");
    ros::NodeHandle nodeHandle("~");

    kinova_ros_murmel::KinovaRosController kinovaRosController(nodeHandle);

    while(ros::ok()){
        kinovaRosController.readParameters();
        ROS_INFO_STREAM("Node running. Op_state: " << kinovaRosController.getOpstate());
        kinovaRosController.kinovaMotion();
        /** standard combination of ros::spinOnce() and rate.sleep() does not apply here
         * spinOnce() gets called on demand and a constant rate is not sensible since op_states like "ready" and "open"
         * are so different in time consumption
         * sleep() is implemented only for "ready" */
    }
    
    return 0;
}