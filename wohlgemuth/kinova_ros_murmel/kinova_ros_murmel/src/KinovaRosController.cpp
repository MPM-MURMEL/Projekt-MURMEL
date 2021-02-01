#include "kinova_ros_controller/KinovaRosController.h"

namespace kinova_ros_murmel {

KinovaRosController::KinovaRosController(ros::NodeHandle &nodeHandle)
    : nodeHandle_(nodeHandle), 
    joint_angles_client_("j2n6s300_driver/joints_action/joint_angles", true), 
    tool_pose_client_("tool_pose", true) // get correct address from running kinova node
{
    //possibly delay startup, since kinova node is likely to take longer than this node to start

    //provide option to get parameters from parameter server at the start
    if (!readParameters())
    {
        ROS_INFO("Parameters not found");
        ros::requestShutdown();
    }

    // ROS communication setup with camera
    camera_mode_client = nodeHandle_.serviceClient<kinova_ros_murmel::CameraMode>("set_camera_mode");
    camera_data_client = nodeHandle_.serviceClient<kinova_ros_murmel::CameraData>("get_corrdinates");

    // ROS communication setup with kinova
    home_arm_client_ = nodeHandle_.serviceClient<kinova_ros_murmel::HomeArm>("j2n6s300_driver/in/home_arm");
    cartesian_velocity_publisher_ = nodeHandle_.advertise<kinova_ros_murmel::PoseVelocity>("j2n6s300_driver/in/cartesian_velocity", 1);
    kinova_coordinates_subscriber_ = nodeHandle_.subscribe("j2n6s300_driver/out/cartesian_command", 1, &KinovaRosController::kinovaCoordinatesCallback, this);
    kinova_angles_subscriber_ = nodeHandle_.subscribe("j2n6s300_driver/out/joint_angles", 1, &KinovaRosController::kinovaAnglesCallback, this);

    // create Exp-filters and PID controllers
    f_prob = f_x = f_y = f_z = f_theta_x = f_theta_y = ExponentialFilter(exp_w);
    p_x = p_y = p_z = PIDController(pid_p, pid_i, pid_d);
    p_theta_x = p_theta_y = PIDController(0.01, pid_i, pid_d);

    is_first_init = true;
}

bool KinovaRosController::readParameters(){
    return nodeHandle_.getParam("op_state", op_state_);
}

void KinovaRosController::kinovaMotion(){
    if (op_state_ == "ready"){
        ros::Duration(2).sleep();
    }
    else if (op_state_ == "calibrate"){
        initHome();
        is_first_init = false;
    }
    else if (op_state_ == "retracted"){ // sendRetracted (joint_angles_client) does not have self collision check
        // check if arm is homed, if not -> home for calibration
        if(is_first_init){
            initHome();
            is_first_init = false;
        }
        sendRetracted();
    }
    else if (op_state_ == "open"){
        // start from homeposition to make sure arm does not self-collide on moving to retracted position

        // move to retracted to position camera onto keyhole
        sendRetracted();
        openTrashcanDemo();
        sendRetracted();
    }

    return;        
}

// does not produce same results after using sendRetracted() -> change sendRetraceted() to communicating trough Quaternions or implement own homing function
void KinovaRosController::initHome() {
    kinova_ros_murmel::HomeArm srv;
    if (home_arm_client_.call(srv))
    {
        ROS_INFO("Arm returned to home position.");
        ros::Duration(5).sleep();       // obsolete if returning true to call() means homing arm has completely finished
    }
    else
    {
        ROS_ERROR("Failed to return arm to home position.");
    }

    op_state_ = "ready";
}

void KinovaRosController::sendRetracted() {
    ROS_INFO("Moving arm to retracted position.");
    ROS_INFO("Waiting for joint_angles_action server to start.");
    joint_angles_client_.waitForServer(ros::Duration(5));

    ROS_INFO("joint_angles_action server reached.");

    kinova_ros_murmel::ArmJointAnglesGoal goal;
    goal.angles.joint1 = actuator1_;
    goal.angles.joint2 = actuator2_;
    goal.angles.joint3 = actuator3_;
    goal.angles.joint4 = actuator4_;
    goal.angles.joint5 = actuator5_;
    goal.angles.joint6 = actuator6_;
    goal.angles.joint7 = actuator7_;
    joint_angles_client_.sendGoal(goal);

    bool finished_before_timeout = joint_angles_client_.waitForResult(ros::Duration(10));
    if(finished_before_timeout){
        actionlib::SimpleClientGoalState state = joint_angles_client_.getState();
        ROS_INFO("Action finished: %s", state.toString().c_str());
    }
    else {
        ROS_INFO("Action did not finish before timeout.");
    }

    op_state_ = "ready";
}

void KinovaRosController::openTrashcanDemo(){
    //----------------------------------------------
    // A P P R O A C H 
    //----------------------------------------------

    ROS_INFO("Starting approaching phase.");

    //send tracking command to camera
    kinova_ros_murmel::CameraMode mode_srv;
    mode_srv.request.request = "tracking";
    if(!camera_mode_client.call(mode_srv)){
        ROS_INFO("Could not send tracking mode to camera.");
        return;
    }

    // counter is used to disregard first few camera values, since they are bogus
    // remove with integration of new camera if possible
    int counter = 0;

    while(true){
        double probability = 0;
        double dx = 0;
        double dy = 0;
        double dz = 0;
        double theta_x = 0;
        double theta_y = 0;


        // receive Point coordinates from camera
        kinova_ros_murmel::CameraData data_srv;
        if(camera_data_client.call(data_srv)){
            probability = data_srv.response.probability;
            dx = data_srv.response.coordinates.x / 1000;          // convert to meters
            dy = data_srv.response.coordinates.y / 1000;
            dz = -1 * data_srv.response.coordinates.z / 1000;     // needs to be inverted
            theta_x = data_srv.response.theta_x;
            theta_y = data_srv.response.theta_y;
        }
        else {
            ROS_INFO("Could not receive camera data.");
            return;
        }           
        
        if(counter++ > 10 && probability > 0){
            // calculate filtered values, which are distances [m]
            probability = f_prob.calculate(probability);
            dx = f_x.calculate(dx);
            dy = f_y.calculate(dy);
            dz = f_z.calculate(dz);
            theta_x = f_theta_x.calculate(theta_x);
            theta_y = f_theta_y.calculate(theta_y);

            // calculate control values
            // values are speed values from here on [m/s]
            dx = p_x.calculate(controller_offset_x - dx);
            dy = p_y.calculate(controller_offset_y - dy);
            dz = p_z.calculate(controller_offset_z - dz);
            theta_x = p_theta_x.calculate(controller_offset_theta_x - theta_x);
            theta_y = p_theta_y.calculate(controller_offset_theta_y - theta_y);

            /** dz being negative results from the algorithm on the camera, meaning that the distance to keyhole is less
             * than 1m, as a result the depth sensor is not working properly anymore
             */
            if(dz <= 0)
            {
                break;  // move forward to insertion
            }

            /** only move forward if robot is centered on keyhole
             * This condition checks, if the keyhole is in a field of tolerance. If not, corrections have to be made in dx
             * and dy and no further apporaching the muelleimer -> dz=0
             */
            if(!((abs(dx) < x_y_thresh && abs(dy) < x_y_thresh))) 
            {  
                dz = 0;
            }
            /** Has nothing to do with previous if-statement. It just checks, if dz is too small for use. Else is needed to
             * not overwrite the previously set dz=0
             */
            else if(dz < 0.2)
            {
                // speed below 0.1 are not usable for servoing
                dz = 0.2;
            }

            // create PoseVelocity object containing target velocities
            PoseVelocity target_velocity;
            target_velocity.twist_linear_x = dx;
            target_velocity.twist_linear_y = dy;
            target_velocity.twist_linear_z = dz;
            target_velocity.twist_angular_x = theta_x;
            target_velocity.twist_angular_y = theta_y;
            target_velocity.twist_angular_z = 0;

            /** Get current coordinates by calling kinovaCoordinatesCallback(). Since spinOnce() is blocking, coordinates will
             * be updated upon next line
             * If callbackqueue gets to long, seperate callback queue exclusively for kinovaCoordinatesCallback() could be created, 
             * so that this step does not take to much time.
             * Alternatively use timed callback through timer object
             */
            ros::spinOnce();

            convertReferenceFrame(target_velocity);
            
            cartesian_velocity_publisher_.publish(target_velocity);
        }
    }

    //----------------------------------------------
    // I N S E R T
    //----------------------------------------------
    ROS_INFO("Starting insertino phase.");

    KinovaPose target_pos;

    target_pos.X = correction_offset_x;
    target_pos.Y = correction_offset_z;
    target_pos.Y = 0;
    target_pos.ThetaX = 0;
    target_pos.ThetaY = 0;
    target_pos.ThetaZ = 0;

    // get current cartesian position
    ros::spinOnce();

    convertReferenceFrame(target_pos);

    target_pos.X += kinova_coordinates_.X;
    target_pos.Y += kinova_coordinates_.Y;
    target_pos.Z += kinova_coordinates_.Z;
    target_pos.ThetaX = kinova_coordinates_.ThetaX;
    target_pos.ThetaY = kinova_coordinates_.ThetaY;
    target_pos.ThetaZ = kinova_coordinates_.ThetaZ;

    geometry_msgs::PoseStamped command_pos = EulerXYZ2Quaternions(target_pos);

    ROS_INFO("Waiting for arm_pose_action server to start.");
    tool_pose_client_.waitForServer();
    ROS_INFO("tool_pose_action server reached.");

    kinova_ros_murmel::ArmPoseGoal xy_offset_goal;
    xy_offset_goal.pose.pose.position.x = command_pos.pose.position.x;
    xy_offset_goal.pose.pose.position.y = command_pos.pose.position.y;
    xy_offset_goal.pose.pose.position.z = command_pos.pose.position.z;
    xy_offset_goal.pose.pose.orientation.x = command_pos.pose.orientation.x;
    xy_offset_goal.pose.pose.orientation.y = command_pos.pose.orientation.y;
    xy_offset_goal.pose.pose.orientation.z = command_pos.pose.orientation.z;
    xy_offset_goal.pose.pose.orientation.w = command_pos.pose.orientation.w;

    tool_pose_client_.sendGoal(xy_offset_goal);

    bool finished_before_timeout = tool_pose_client_.waitForResult(ros::Duration(10));
    if(finished_before_timeout){
        actionlib::SimpleClientGoalState state = tool_pose_client_.getState();
        ROS_INFO("Correction of x&y offset: %s", state.toString().c_str());
    }
    else
        ROS_INFO("Correction of x&y offset did not finish before timeout.");


    // inserting tool
    target_pos.X = 0;
    target_pos.Y = 0;
    target_pos.Y = correction_offset_z;
    target_pos.ThetaX = 0;
    target_pos.ThetaY = 0;
    target_pos.ThetaZ = 0;

    ros::spinOnce();

    convertReferenceFrame(target_pos);

    target_pos.X += kinova_coordinates_.X;
    target_pos.Y += kinova_coordinates_.Y;
    target_pos.Z += kinova_coordinates_.Z;
    target_pos.ThetaX = kinova_coordinates_.ThetaX;
    target_pos.ThetaY = kinova_coordinates_.ThetaY;
    target_pos.ThetaZ = kinova_coordinates_.ThetaZ;

    command_pos = EulerXYZ2Quaternions(target_pos);

    ROS_INFO("Waiting for arm_pose_action server to start.");
    tool_pose_client_.waitForServer();
    ROS_INFO("tool_pose_action server reached.");

    kinova_ros_murmel::ArmPoseGoal insert_goal;
    insert_goal.pose.pose.position.x = command_pos.pose.position.x;
    insert_goal.pose.pose.position.y = command_pos.pose.position.y;
    insert_goal.pose.pose.position.z = command_pos.pose.position.z;
    insert_goal.pose.pose.orientation.x = command_pos.pose.orientation.x;
    insert_goal.pose.pose.orientation.y = command_pos.pose.orientation.y;
    insert_goal.pose.pose.orientation.z = command_pos.pose.orientation.z;
    insert_goal.pose.pose.orientation.w = command_pos.pose.orientation.w;

    tool_pose_client_.sendGoal(insert_goal);

    finished_before_timeout = tool_pose_client_.waitForResult(ros::Duration(10));
    if (finished_before_timeout)
    {
        actionlib::SimpleClientGoalState state = tool_pose_client_.getState();
        ROS_INFO("Insertion of tool: %s", state.toString().c_str());
    }
    else
        ROS_INFO("Insertion of tool did not finish before timeout.");


    //----------------------------------------------
    // O P E N
    //----------------------------------------------

    ROS_INFO("Starting opening phase.");

    ros::spinOnce();

    JointAngles target_angles;
    target_angles.joint1 = kinova_angles_.joint1;
    target_angles.joint2 = kinova_angles_.joint2;
    target_angles.joint3 = kinova_angles_.joint3;
    target_angles.joint4 = kinova_angles_.joint4;
    target_angles.joint5 = kinova_angles_.joint5;
    target_angles.joint6 = kinova_angles_.joint6 + 90;
    target_angles.joint7 = kinova_angles_.joint7;

    ROS_INFO("Waiting for joint_angles_action server to start.");
    joint_angles_client_.waitForServer();
    ROS_INFO("joint_angles_action server reached.");

    kinova_ros_murmel::ArmJointAnglesGoal open_keyhole_goal;
    open_keyhole_goal.angles.joint1 = target_angles.joint1;
    open_keyhole_goal.angles.joint2 = target_angles.joint2;
    open_keyhole_goal.angles.joint3 = target_angles.joint3;
    open_keyhole_goal.angles.joint4 = target_angles.joint4;
    open_keyhole_goal.angles.joint5 = target_angles.joint5;
    open_keyhole_goal.angles.joint6 = target_angles.joint6;
    open_keyhole_goal.angles.joint7 = target_angles.joint7;     // might cause error, alternatively set to 0

    joint_angles_client_.sendGoal(open_keyhole_goal);

    finished_before_timeout = joint_angles_client_.waitForResult(ros::Duration(10));
    if(finished_before_timeout){
        actionlib::SimpleClientGoalState state = joint_angles_client_.getState();
        ROS_INFO("Opening of keyhole: %s", state.toString().c_str());
    }
    else
        ROS_INFO("Opening of keyhole did not finish before timeout");


    //----------------------------------------------
    // E X T R A C T
    //----------------------------------------------

    ROS_INFO("Starting extraction phase.");

    ros::spinOnce();

    target_angles.joint1 = kinova_angles_.joint1;
    target_angles.joint2 = kinova_angles_.joint2;
    target_angles.joint3 = kinova_angles_.joint3;
    target_angles.joint4 = kinova_angles_.joint4;
    target_angles.joint5 = kinova_angles_.joint5;
    target_angles.joint6 = kinova_angles_.joint6 - 90;
    target_angles.joint7 = kinova_angles_.joint7;

    ROS_INFO("Waiting for joint_angles_action server to start.");
    joint_angles_client_.waitForServer();
    ROS_INFO("joint_angles_action server reached.");

    kinova_ros_murmel::ArmJointAnglesGoal close_keyhole_goal;
    close_keyhole_goal.angles.joint1 = target_angles.joint1;
    close_keyhole_goal.angles.joint2 = target_angles.joint2;
    close_keyhole_goal.angles.joint3 = target_angles.joint3;
    close_keyhole_goal.angles.joint4 = target_angles.joint4;
    close_keyhole_goal.angles.joint5 = target_angles.joint5;
    close_keyhole_goal.angles.joint6 = target_angles.joint6;
    close_keyhole_goal.angles.joint7 = target_angles.joint7;    // might cause error, alternatively set to 0

    joint_angles_client_.sendGoal(close_keyhole_goal);

    finished_before_timeout = joint_angles_client_.waitForResult(ros::Duration(10));
    if (finished_before_timeout)
    {
        actionlib::SimpleClientGoalState state = joint_angles_client_.getState();
        ROS_INFO("Closing of keyhole: %s", state.toString().c_str());
    }
    else
        ROS_INFO("Closing of keyhole did not finish before timeout");

    // retracting tool
    target_pos.X = 0;
    target_pos.Y = 0;
    target_pos.Y = -correction_offset_z;
    target_pos.ThetaX = 0;
    target_pos.ThetaY = 0;
    target_pos.ThetaZ = 0;

    ros::spinOnce();

    convertReferenceFrame(target_pos);

    target_pos.X += kinova_coordinates_.X;
    target_pos.Y += kinova_coordinates_.Y;
    target_pos.Z += kinova_coordinates_.Z;
    target_pos.ThetaX = kinova_coordinates_.ThetaX;
    target_pos.ThetaY = kinova_coordinates_.ThetaY;
    target_pos.ThetaZ = kinova_coordinates_.ThetaZ;

    command_pos = EulerXYZ2Quaternions(target_pos);

    ROS_INFO("Waiting for arm_pose_action server to start.");
    tool_pose_client_.waitForServer();
    ROS_INFO("tool_pose_action server reached.");

    kinova_ros_murmel::ArmPoseGoal retract_goal;
    retract_goal.pose.pose.position.x = command_pos.pose.position.x;
    retract_goal.pose.pose.position.y = command_pos.pose.position.y;
    retract_goal.pose.pose.position.z = command_pos.pose.position.z;
    retract_goal.pose.pose.orientation.x = command_pos.pose.orientation.x;
    retract_goal.pose.pose.orientation.y = command_pos.pose.orientation.y;
    retract_goal.pose.pose.orientation.z = command_pos.pose.orientation.z;
    retract_goal.pose.pose.orientation.w = command_pos.pose.orientation.w;

    tool_pose_client_.sendGoal(retract_goal);

    finished_before_timeout = tool_pose_client_.waitForResult(ros::Duration(10));
    if (finished_before_timeout)
    {
        actionlib::SimpleClientGoalState state = tool_pose_client_.getState();
        ROS_INFO("Retraction of tool: %s", state.toString().c_str());
    }
    else
        ROS_INFO("Retraction of tool did not finish before timeout.");

    ROS_INFO("Opening sequence finished completely.");
}

geometry_msgs::PoseStamped KinovaRosController::EulerXYZ2Quaternions(const KinovaPose &target_pos) {

    float sx = sin(0.5*target_pos.ThetaX);
    float cx = cos(0.5*target_pos.ThetaX);
    float sy = sin(0.5*target_pos.ThetaY);
    float cy = cos(0.5*target_pos.ThetaY);
    float sz = sin(0.5*target_pos.ThetaZ);
    float cz = cos(0.5*target_pos.ThetaZ);

    float qx, qy, qz, qw;
    qx = sx*cy*cz + cx*sy*sz;
    qy = -sx*cy*sz + cx*sy*cz;
    qz = sx*sy*cz + cx*cy*sz;
    qw = -sx*sy*sz + cx*cy*cz;
    
    geometry_msgs::PoseStamped command_pos;
    command_pos.pose.position.x = target_pos.X;
    command_pos.pose.position.y = target_pos.Y;
    command_pos.pose.position.z = target_pos.Z;
    command_pos.pose.orientation.x = qx;
    command_pos.pose.orientation.y = qy;
    command_pos.pose.orientation.z = qz;
    command_pos.pose.orientation.w = qw;

    return command_pos;
}

void KinovaRosController::convertReferenceFrame(PoseVelocity &target_velocity){
    // transformation matrix between end effector and base frame is given by the end effector orientation
    Eigen::Matrix3f orientation_robot;
    orientation_robot = Eigen::AngleAxisf(kinova_coordinates_.ThetaX, Eigen::Vector3f::UnitX())
                        * Eigen::AngleAxisf(kinova_coordinates_.ThetaY, Eigen::Vector3f::UnitY())
                        * Eigen::AngleAxisf(kinova_coordinates_.ThetaZ, Eigen::Vector3f::UnitZ());
    
    // translation vector in end effector frame
    Eigen::Vector3f translation_vector_ee_frame(target_velocity.twist_linear_x, target_velocity.twist_linear_y, target_velocity.twist_linear_z);

    // translation vector in base frame
    Eigen::Vector3f translation_vector_base_frame(orientation_robot * translation_vector_ee_frame);

    // update the x-y-z- part of the command
    target_velocity.twist_linear_x = translation_vector_base_frame(0);
    target_velocity.twist_linear_y = translation_vector_base_frame(1);
    target_velocity.twist_linear_z = translation_vector_base_frame(2);
}

void KinovaRosController::convertReferenceFrame(KinovaPose &target_position){
    // transformation matrix between end effector and base frame is given by the end effector orientation
    Eigen::Matrix3f orientation_robot;
    orientation_robot = Eigen::AngleAxisf(kinova_coordinates_.ThetaX, Eigen::Vector3f::UnitX()) // Thetas have to be normalized -> implement function similar to kinovas
                        * Eigen::AngleAxisf(kinova_coordinates_.ThetaY, Eigen::Vector3f::UnitY()) * Eigen::AngleAxisf(kinova_coordinates_.ThetaZ, Eigen::Vector3f::UnitZ());

    // translation vector in end effector frame
    Eigen::Vector3f translation_vector_ee_frame(target_position.X, target_position.Y, target_position.Z);

    // translation vector in base frame
    Eigen::Vector3f translation_vector_base_frame(orientation_robot * translation_vector_ee_frame);

    // update the x-y-z- part of the command
    target_position.X = translation_vector_base_frame(0);
    target_position.Y = translation_vector_base_frame(1);
    target_position.Z = translation_vector_base_frame(2);
}

void KinovaRosController::kinovaCoordinatesCallback(const KinovaPose &coordinates){
    kinova_coordinates_.X = coordinates.X;
    kinova_coordinates_.Y = coordinates.Y;
    kinova_coordinates_.Z = coordinates.Z;
    kinova_coordinates_.ThetaX = coordinates.ThetaX;
    kinova_coordinates_.ThetaY = coordinates.ThetaY;
    kinova_coordinates_.ThetaZ = coordinates.ThetaZ;
}

void KinovaRosController::kinovaAnglesCallback(const JointAngles &angles){
    kinova_angles_.joint1 = angles.joint1;
    kinova_angles_.joint2 = angles.joint2;
    kinova_angles_.joint3 = angles.joint3;
    kinova_angles_.joint4 = angles.joint4;
    kinova_angles_.joint5 = angles.joint5;
    kinova_angles_.joint6 = angles.joint6;
    kinova_angles_.joint7 = angles.joint7;
}
}