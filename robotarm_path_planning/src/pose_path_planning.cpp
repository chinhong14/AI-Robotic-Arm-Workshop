// #include <rclcpp/rclcpp.hpp>
// #include <moveit/move_group_interface/move_group_interface.h>
// #include <moveit/planning_scene_interface/planning_scene_interface.h>

// int main(int argc, char** argv)
// {
//   // Initialize ROS 2
//   rclcpp::init(argc, argv);
//   auto node = std::make_shared<rclcpp::Node>("move_arm_node");

//   // Create a ROS logger
//   auto logger = node->get_logger();

//   // Create the MoveGroupInterface for the arm
//   auto move_group_interface = moveit::planning_interface::MoveGroupInterface(node, "arm_group");

//   // Set a target joint state
//   std::vector<double> target_joint_positions = {0.0, 0.7, -0.7, 0.0, 0.45}; // Example for Panda arm
//   move_group_interface.setJointValueTarget(target_joint_positions);

//   // Plan and execute the motion
//   moveit::planning_interface::MoveGroupInterface::Plan plan;
//   bool success = (move_group_interface.plan(plan) == moveit::core::MoveItErrorCode::SUCCESS);

//   if (success)
//   {
//     RCLCPP_INFO(logger, "Planning succeeded, moving the arm.");
//     move_group_interface.execute(plan);
//   }
//   else
//   {
//     RCLCPP_ERROR(logger, "Planning failed!");
//   }

//   // Shutdown ROS 2
//   rclcpp::shutdown();
//   return 0;
// }

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// #include <rclcpp/rclcpp.hpp>
// #include <moveit/move_group_interface/move_group_interface.h>
// #include <moveit/planning_scene_interface/planning_scene_interface.h>
// #include <moveit_msgs/msg/collision_object.hpp>
// #include <moveit_msgs/msg/allowed_collision_matrix.hpp>
// #include <moveit_msgs/msg/planning_scene.hpp>
// #include <geometry_msgs/msg/pose.hpp>
// #include <random_numbers/random_numbers.h>

// int main(int argc, char** argv)
// {
//   rclcpp::init(argc, argv);
//   auto node = std::make_shared<rclcpp::Node>("move_arm_to_pose");

//   // Initialize interfaces with longer timeout
//   auto move_group_interface = moveit::planning_interface::MoveGroupInterface(node, "arm_group");
//   auto planning_scene_interface = moveit::planning_interface::PlanningSceneInterface();

//   // Set longer state monitoring timeout (10 seconds)
//   if (!move_group_interface.startStateMonitor(10.0)) {
//     RCLCPP_ERROR(node->get_logger(), "Failed to get current robot state");
//     return 1;
//   }

//   // Get current state and validate
//   auto current_state = move_group_interface.getCurrentState();
//   if (!current_state->satisfiesBounds()) {
//     RCLCPP_ERROR(node->get_logger(), "Start state violates joint limits!");
    
//     // Try to find a nearby valid state
//     current_state->setToRandomPositions();
//     if (!current_state->satisfiesBounds()) {
//       RCLCPP_ERROR(node->get_logger(), "Could not find valid start state");
//       return 1;
//     }
//     move_group_interface.setStartState(*current_state);
//   }

//   // Define target pose
//   geometry_msgs::msg::Pose target_pose;
//   target_pose.position.x = 0.478;
//   target_pose.position.y = -0.116;
//   target_pose.position.z = 0.325;
//   target_pose.orientation.x = -0.00;
//   target_pose.orientation.y = -0.0;
//   target_pose.orientation.z = -0.187;
//   target_pose.orientation.w = 0.982;

//   // Set up target object
//   moveit_msgs::msg::CollisionObject target_object;
//   target_object.header.frame_id = move_group_interface.getPlanningFrame();
//   target_object.id = "target_object";
  
//   shape_msgs::msg::SolidPrimitive primitive;
//   primitive.type = primitive.BOX;
//   primitive.dimensions = {0.05, 0.05, 0.05};
  
//   geometry_msgs::msg::Pose object_pose = target_pose;
//   target_object.primitives.push_back(primitive);
//   target_object.primitive_poses.push_back(object_pose);
//   target_object.operation = target_object.ADD;

//   // Configure ACM properly
//   moveit_msgs::msg::AllowedCollisionMatrix acm;
  
//   // Add all default entries first
//   acm.default_entry_names.push_back("ALL");
//   acm.default_entry_values.push_back(false); // Default to not allowing collisions
  
//   // Then add specific allowed collisions
//   std::vector<std::string> gripper_links = {
//     "left_gripper_link",
//     "right_gripper_link",
//     "left_cutter_link",
//     "cutter_cutter_link",
//     "camera_link",
//     "tcp_link"
//   };
  
//   // Add both the object and links to entry names
//   acm.entry_names.push_back("target_object");
//   for (const auto& link : gripper_links) {
//     acm.entry_names.push_back(link);
//   }
  
//   // Create entries allowing collisions between target_object and each gripper link
//   for (size_t i = 0; i < gripper_links.size() + 1; ++i) {
//     moveit_msgs::msg::AllowedCollisionEntry entry;
//     // Allow collision between target_object and each gripper link
//     for (size_t j = 0; j < gripper_links.size() + 1; ++j) {
//       entry.enabled.push_back((i == 0 && j > 0) || (j == 0 && i > 0));
//     }
//     acm.entry_values.push_back(entry);
//   }

//   // Create and apply planning scene
//   moveit_msgs::msg::PlanningScene planning_scene;
//   planning_scene.is_diff = true;
//   planning_scene.world.collision_objects.push_back(target_object);
//   planning_scene.allowed_collision_matrix = acm;
  
//   // Apply the planning scene
//   planning_scene_interface.applyPlanningScene(planning_scene);

//   // Configure planning parameters
//   move_group_interface.setPlanningTime(20.0);  // Increased planning time
//   move_group_interface.setPoseTarget(target_pose);
//   move_group_interface.setMaxVelocityScalingFactor(0.5);  // Slower movement
//   move_group_interface.setMaxAccelerationScalingFactor(0.5);
//   move_group_interface.setGoalTolerance(0.01);  // Tighter tolerance

//   // Attempt planning with retries
//   moveit::planning_interface::MoveGroupInterface::Plan plan;
//   int max_attempts = 3;
//   int attempt = 0;
//   bool success = false;
//   random_numbers::RandomNumberGenerator rng;

//   while (attempt < max_attempts && !success) {
//     attempt++;
//     RCLCPP_INFO(node->get_logger(), "Planning attempt %d/%d", attempt, max_attempts);
    
//     auto const plan_result = move_group_interface.plan(plan);
//     success = (plan_result == moveit::core::MoveItErrorCode::SUCCESS);

//     if (!success) {
//       RCLCPP_WARN(node->get_logger(), "Attempt %d failed with code %d", attempt, plan_result.val);
      
//       // Try randomizing start state slightly (FIXED version)
//       current_state->setToRandomPositionsNearBy(
//         move_group_interface.getJointModelGroup("arm_group"),
//         *move_group_interface.getCurrentState(),
//         0.1,  // 10% randomization
//         rng
//       );
//       move_group_interface.setStartState(*current_state);
//     }
//   }

//   if (success) {
//     RCLCPP_INFO(node->get_logger(), "Planning succeeded, executing...");
//     move_group_interface.execute(plan);
//   } else {
//     RCLCPP_ERROR(node->get_logger(), "All planning attempts failed!");
//     RCLCPP_INFO(node->get_logger(), "Detailed diagnostics:");
//     RCLCPP_INFO(node->get_logger(), "- Current joint values:");
//     std::vector<double> joint_values;
//     current_state->copyJointGroupPositions("arm_group", joint_values);
//     for (size_t i = 0; i < joint_values.size(); ++i) {
//       RCLCPP_INFO(node->get_logger(), "  Joint %zu: %.3f", i, joint_values[i]);
//     }
//     RCLCPP_INFO(node->get_logger(), "- Target pose: x=%.3f, y=%.3f, z=%.3f",
//                target_pose.position.x, target_pose.position.y, target_pose.position.z);
//   }

//   rclcpp::shutdown();
//   return success ? 0 : 1;
// }
#include <rclcpp/rclcpp.hpp>
#include <moveit/move_group_interface/move_group_interface.h>
#include <geometry_msgs/msg/pose.hpp>

int main(int argc, char** argv)
{
  rclcpp::init(argc, argv);
  auto node = std::make_shared<rclcpp::Node>("move_arm_to_pose");

  auto move_group_interface = moveit::planning_interface::MoveGroupInterface(node, "arm_group");

  // // Define a target pose for the end effector
  // geometry_msgs::msg::Pose target_pose;
  // target_pose.position.x = 0.5094624112272489 - 0.125; // Your robot's x position
  // target_pose.position.y = -8.933854118682659e-05; // Your robot's y position -0.00015181916551501362
  // target_pose.position.z = 0.42470638790325854 + 0.03; // Your robot's z position
  
  // // Set the orientation (quaternion)
  // target_pose.orientation.x = -1.2829364959154774e-05; // Your robot's quaternion x
  // target_pose.orientation.y = -3.210690693074917e-05; // Your robot's quaternion y
  // target_pose.orientation.z = -2.221486049328619e-05; // Your robot's quaternion z
  // target_pose.orientation.w = 0.9999999980626958; // Your robot's quaternion w

  // // Define a target pose for the end effector
  // geometry_msgs::msg::Pose target_pose;
  // target_pose.position.x = 0.47408770475619194; //0.491-0.125; // Your robot's x position
  // target_pose.position.y = 0.07354882574405273; // Your robot's y position -0.00015181916551501362
  // target_pose.position.z = 0.30063151335558636; //-0.12783528410777292+0.03; // Your robot's z position
  
  // // Set the orientation (quaternion)
  // target_pose.orientation.x = 8.724172562031693e-05;//0.0; // Your robot's quaternion x
  // target_pose.orientation.y = -0.0012219434179526474; //0.0; // Your robot's quaternion y
  // target_pose.orientation.z = 0.07695449752934035;//0.09557429675609919; // Your robot's quaternion z
  // target_pose.orientation.w = 0.997033853263354; //0.9954222992276077; // Your robot's quaternion w

  // // Define a target pose for the end effector
  // geometry_msgs::msg::Pose target_pose;
  // target_pose.position.x = 0.5327-0.125; //0.491-0.125; // Your robot's x position
  // target_pose.position.y = -0.04142; // Your robot's y position -0.00015181916551501362
  // target_pose.position.z = 0.1519+0.03; //-0.12783528410777292+0.03; // Your robot's z position
  
  // // Set the orientation (quaternion)
  // target_pose.orientation.x = -0.0;//0.0; // Your robot's quaternion x
  // target_pose.orientation.y = 0.0; //0.0; // Your robot's quaternion y
  // target_pose.orientation.z = -0.0;//0.09557429675609919; // Your robot's quaternion z
  // target_pose.orientation.w = 1.0; //0.9954222992276077; // Your robot's quaternion w

    // Define a target pose for the end effector
    geometry_msgs::msg::Pose target_pose;
    target_pose.position.x = 0.478;
    target_pose.position.y = -0.116;    // Your robot's y position 
    target_pose.position.z = 0.325; 
    // [INFO] [1743403556.273622225] [arm_pose_node]: Position: x=0.324, y=-0.247, z=0.424
    // [INFO] [1743403556.274356425] [arm_pose_node]: Orientation: x=-0.006, y=-0.016, z=-0.320, w=0.947
    // Transform for detected_cup_12 relative to world:
    // [INFO] [1743405110.383993084] [arm_pose_node]: Position: x=0.615, y=-0.047, z=0.176
    // [INFO] [1743405110.384677330] [arm_pose_node]: Orientation: x=-0.000, y=-0.000, z=-0.061, w=0.998

    // [INFO] [1743495988.248978154] [arm_pose_node]: Position: x=0.478, y=-0.116, z=0.325
    // [INFO] [1743495988.255644264] [arm_pose_node]: Orientation: x=-0.000, y=-0.000, z=-0.187, w=0.982


    // Set the orientation (quaternion)
    target_pose.orientation.x = -0.00;
    target_pose.orientation.y = -0.0;
    target_pose.orientation.z = -0.187;
    target_pose.orientation.w = 0.982;

  move_group_interface.setPoseTarget(target_pose);

  // Plan and execute
  moveit::planning_interface::MoveGroupInterface::Plan plan;
  bool success = (move_group_interface.plan(plan) == moveit::core::MoveItErrorCode::SUCCESS);

  if (success)
  {
    RCLCPP_INFO(node->get_logger(), "Planning to pose succeeded, moving the arm.");
    move_group_interface.execute(plan);
  }
  else
  {
    RCLCPP_ERROR(node->get_logger(), "Planning to pose failed!");
  }

  rclcpp::shutdown();
  return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// #include <rclcpp/rclcpp.hpp>
// #include <moveit/move_group_interface/move_group_interface.h>
// #include <geometry_msgs/msg/pose.hpp>
// #include <vector>
// #include <iostream> // For std::cin and std::cout

// int main(int argc, char** argv)
// {
//   rclcpp::init(argc, argv);
//   auto node = std::make_shared<rclcpp::Node>("cartesian_path_planning");

//   auto move_group_interface = moveit::planning_interface::MoveGroupInterface(node, "arm_group");

//   // Get the current pose of the end effector
//   geometry_msgs::msg::Pose start_pose = move_group_interface.getCurrentPose().pose;
//   RCLCPP_INFO(node->get_logger(), "Current pose: x=%.2f, y=%.2f, z=%.2f",
//               start_pose.position.x, start_pose.position.y, start_pose.position.z);

//   // Ask the user for the target x position
//   double target_x;
//   std::cout << "Enter the target x position (in meters): ";
//   std::cin >> target_x;

//   // Define waypoints for the Cartesian path
//   std::vector<geometry_msgs::msg::Pose> waypoints;
//   geometry_msgs::msg::Pose target_pose = start_pose;
//   target_pose.position.z = target_x; // Set the target x position from user input
//   waypoints.push_back(target_pose);

//   // Compute the Cartesian path
//   moveit_msgs::msg::RobotTrajectory trajectory;
//   double fraction = move_group_interface.computeCartesianPath(waypoints, 0.01, 0.0, trajectory);

//   if (fraction >= 0.0)
//   {
//     RCLCPP_INFO(node->get_logger(), "Cartesian path planned (%.2f%% achieved).", fraction * 100.0);
//     move_group_interface.execute(trajectory);
//   }
//   else
//   {
//     RCLCPP_ERROR(node->get_logger(), "Cartesian path planning failed!");
//   }

//   rclcpp::shutdown();
//   return 0;
// }

// #include <rclcpp/rclcpp.hpp>
// #include <moveit/move_group_interface/move_group_interface.h>
// #include <geometry_msgs/msg/pose.hpp>
// #include <vector>

// int main(int argc, char** argv)
// {
//   rclcpp::init(argc, argv);
//   auto node = std::make_shared<rclcpp::Node>("cartesian_path_planning_xyz");

//   auto move_group_interface = moveit::planning_interface::MoveGroupInterface(node, "arm_group");

//   // Get the current pose of the end effector
//   geometry_msgs::msg::Pose start_pose = move_group_interface.getCurrentPose().pose;

//   // Define waypoints for the Cartesian path
//   std::vector<geometry_msgs::msg::Pose> waypoints;

//   // First waypoint: Move in the x-direction
//   geometry_msgs::msg::Pose target_pose = start_pose;
//   target_pose.position.z += 2; // Move 20 cm in the x-direction
//   waypoints.push_back(target_pose);

//   // Second waypoint: Move in the y-direction
//   target_pose.position.z -= 2; // Move 20 cm in the y-direction
//   waypoints.push_back(target_pose);

// //   // Third waypoint: Move in the z-direction
// //   target_pose.position.z += 0.2; // Move 20 cm in the z-direction
// //   waypoints.push_back(target_pose);

// //   // Fourth waypoint: Move back in the y-direction
// //   target_pose.position.y -= 0.2; // Move back 20 cm in the y-direction
// //   waypoints.push_back(target_pose);

// //   // Fifth waypoint: Move back in the z-direction
// //   target_pose.position.z -= 0.2; // Move back 20 cm in the z-direction
// //   waypoints.push_back(target_pose);

// //   // Sixth waypoint: Move back in the x-direction
// //   target_pose.position.x -= 0.2; // Move back 20 cm in the x-direction
// //   waypoints.push_back(target_pose);

//   // Compute the Cartesian path
//   moveit_msgs::msg::RobotTrajectory trajectory;
//   double fraction = move_group_interface.computeCartesianPath(waypoints, 0.01, 0.0, trajectory);

//   if (fraction >= 0.0)
//   {
//     RCLCPP_INFO(node->get_logger(), "Cartesian path planned (%.2f%% achieved).", fraction * 100.0);
//     move_group_interface.execute(trajectory);
//   }
//   else
//   {
//     RCLCPP_ERROR(node->get_logger(), "Cartesian path planning failed!");
//   }

//   rclcpp::shutdown();
//   return 0;
// }



// #include <rclcpp/rclcpp.hpp>
// #include <moveit/move_group_interface/move_group_interface.h>
// #include <geometry_msgs/msg/pose.hpp>

// int main(int argc, char** argv)
// {
//   rclcpp::init(argc, argv);
//   auto node = std::make_shared<rclcpp::Node>("move_arm_to_pose");

//   auto move_group_interface = moveit::planning_interface::MoveGroupInterface(node, "arm_group");

//   // Increase planning time
//   move_group_interface.setPlanningTime(5.0);

//   // Set a specific planner
//   move_group_interface.setPlannerId("TRAC-IK");

//   // Clear constraints
//   move_group_interface.clearPathConstraints();

//   // Define a target pose for the end effector
//   geometry_msgs::msg::Pose target_pose;
//   target_pose.position.x = 0.352;
//   target_pose.position.y = 0.411;    // Your robot's y position 
//   target_pose.position.z = 0.369; 
// //   [INFO] [1743398761.484722878] [arm_pose_node]: Position: x=0.352, y=0.411, z=0.369
// //   [INFO] [1743398761.485997123] [arm_pose_node]: Orientation: x=0.000, y=0.000, z=0.384, w=0.923
//   // Set the orientation (quaternion)
//   target_pose.orientation.x = 0.0;
//   target_pose.orientation.y = 0.0;
//   target_pose.orientation.z = 0.384;
//   target_pose.orientation.w = 0.923;
  
//   // Try planning with only position target
//   move_group_interface.setPositionTarget(0.352, 0.411, 0.369);

//   // Debugging: Print IK solver used
//   RCLCPP_INFO(node->get_logger(), "Using IK solver: %s", move_group_interface.getPlannerId().c_str());

//   // Plan
//   moveit::planning_interface::MoveGroupInterface::Plan plan;
//   bool success = (move_group_interface.plan(plan) == moveit::core::MoveItErrorCode::SUCCESS);

//   if (success)
//   {
//     RCLCPP_INFO(node->get_logger(), "Planning to pose succeeded! Printing joint values:");
    
//     // Debugging: Print planned joint values
//     for (const auto& point : plan.trajectory_.joint_trajectory.points) {
//       RCLCPP_INFO(node->get_logger(), "Joint values: [%.3f, %.3f, %.3f, %.3f, %.3f, %.3f]",
//                   point.positions[0], point.positions[1], point.positions[2],
//                   point.positions[3], point.positions[4], point.positions[5]);
//     }
    
//     // Execute the planned trajectory
//     move_group_interface.execute(plan);
//   }
//   else
//   {
//     RCLCPP_ERROR(node->get_logger(), "Planning to pose failed! Trying joint-based planning...");
    
//     // Try joint-based planning
//     std::vector<double> joint_values = {0.0, -1.2, 1.0, -1.5, 0.8, 0.0}; // Adjust values accordingly
//     move_group_interface.setJointValueTarget(joint_values);
    
//     success = (move_group_interface.plan(plan) == moveit::core::MoveItErrorCode::SUCCESS);
    
//     if (success) {
//       RCLCPP_INFO(node->get_logger(), "Joint-based planning succeeded! Executing...");
//       move_group_interface.execute(plan);
//     } else {
//       RCLCPP_ERROR(node->get_logger(), "Joint-based planning also failed!");
//     }
//   }

//   rclcpp::shutdown();
//   return 0;
// }

// #include <rclcpp/rclcpp.hpp>
// #include <moveit/move_group_interface/move_group_interface.h>
// #include <geometry_msgs/msg/pose.hpp>
// #include <tf2/LinearMath/Transform.h>
// #include <tf2/LinearMath/Quaternion.h>
// #include <tf2_geometry_msgs/tf2_geometry_msgs.hpp>

// int main(int argc, char** argv) {
//     rclcpp::init(argc, argv);
//     auto node = std::make_shared<rclcpp::Node>("move_arm_to_pose");
//     auto move_group_interface = moveit::planning_interface::MoveGroupInterface(node, "arm_group");
    
//     // Get the current end effector link
//     std::string end_effector_link = move_group_interface.getEndEffectorLink();
//     RCLCPP_INFO(node->get_logger(), "End effector link: %s", end_effector_link.c_str());
    
//     // Transform data from your logger output
//     // link_5 position relative to world
//     tf2::Vector3 link5_position(0.408, -0.000, 0.182);
//     tf2::Quaternion link5_orientation(-0.000, 0.000, 0.000, 1.000);
    
//     // gripper_center position relative to world
//     tf2::Vector3 gripper_position(0.533, 0.000, 0.152);
//     tf2::Quaternion gripper_orientation(-0.000, 0.000, 0.000, 1.000);
    
//     // Calculate the offset from link_5 to gripper_center
//     tf2::Vector3 offset = gripper_position - link5_position;
//     RCLCPP_INFO(node->get_logger(), "Offset from link_5 to gripper_center: x=%f, y=%f, z=%f",
//                 offset.x(), offset.y(), offset.z());
    
//     // detected_cup position relative to world
//     tf2::Vector3 cup_position(0.847, -0.041, 0.169);
//     tf2::Quaternion cup_orientation(-0.000, 0.000, 0.064, 0.998);
    
//     // Calculate the target position for link_5 to place gripper_center at the cup
//     tf2::Vector3 target_link5_position = cup_position - offset;
    
//     // Define a target pose for the end effector (link_5)
//     geometry_msgs::msg::Pose target_pose;
//     target_pose.position.x = target_link5_position.x();
//     target_pose.position.y = target_link5_position.y();
//     target_pose.position.z = target_link5_position.z();
    
//     // Use the cup's orientation for the gripper
//     target_pose.orientation.x = cup_orientation.x();
//     target_pose.orientation.y = cup_orientation.y();
//     target_pose.orientation.z = cup_orientation.z();
//     target_pose.orientation.w = cup_orientation.w();
    
//     RCLCPP_INFO(node->get_logger(), "Target pose for link_5: x=%f, y=%f, z=%f, orientation: x=%f, y=%f, z=%f, w=%f",
//                 target_pose.position.x, target_pose.position.y, target_pose.position.z,
//                 target_pose.orientation.x, target_pose.orientation.y, target_pose.orientation.z, target_pose.orientation.w);
                
//     // Add an approach pose that's slightly away from the target
//     double approach_distance = 0.10; // 10cm approach distance
//     geometry_msgs::msg::Pose approach_pose = target_pose;
//     approach_pose.position.x -= approach_distance; // Assuming approach from the negative x direction
    
//     RCLCPP_INFO(node->get_logger(), "Approach pose for link_5: x=%f, y=%f, z=%f",
//                 approach_pose.position.x, approach_pose.position.y, approach_pose.position.z);
    
//     // First plan to the approach pose
//     move_group_interface.setPoseTarget(approach_pose);
    
//     // Plan and execute
//     moveit::planning_interface::MoveGroupInterface::Plan approach_plan;
//     bool approach_success = (move_group_interface.plan(approach_plan) == moveit::core::MoveItErrorCode::SUCCESS);
    
//     if (approach_success) {
//         RCLCPP_INFO(node->get_logger(), "Planning to approach pose succeeded, moving the arm.");
//         move_group_interface.execute(approach_plan);
        
//         // Now plan to the final target pose
//         move_group_interface.setPoseTarget(target_pose);
//         moveit::planning_interface::MoveGroupInterface::Plan final_plan;
//         bool final_success = (move_group_interface.plan(final_plan) == moveit::core::MoveItErrorCode::SUCCESS);
        
//         if (final_success) {
//             RCLCPP_INFO(node->get_logger(), "Planning to final pose succeeded, moving the arm.");
//             move_group_interface.execute(final_plan);
//         } else {
//             RCLCPP_ERROR(node->get_logger(), "Planning to final pose failed!");
//         }
//     } else {
//         RCLCPP_ERROR(node->get_logger(), "Planning to approach pose failed!");
//     }
    
//     rclcpp::shutdown();
//     return 0;
// }

// #include <rclcpp/rclcpp.hpp>
// #include <moveit/move_group_interface/move_group_interface.h>
// #include <moveit/robot_trajectory/robot_trajectory.h>
// #include <moveit_msgs/msg/orientation_constraint.hpp>
// #include <moveit_msgs/msg/constraints.hpp>
// #include <geometry_msgs/msg/pose.hpp>
// #include <tf2/LinearMath/Transform.h>
// #include <tf2/LinearMath/Quaternion.h>
// #include <tf2_geometry_msgs/tf2_geometry_msgs.hpp>

// int main(int argc, char** argv) {
//     rclcpp::init(argc, argv);
//     auto node = std::make_shared<rclcpp::Node>("move_arm_to_pose");
//     auto move_group_interface = moveit::planning_interface::MoveGroupInterface(node, "arm_group");
    
//     // Get the current end effector link
//     std::string end_effector_link = move_group_interface.getEndEffectorLink();
//     RCLCPP_INFO(node->get_logger(), "End effector link: %s", end_effector_link.c_str());
    
//     // Set planning time
//     move_group_interface.setPlanningTime(5.0); // Give it 5 seconds to find a solution
    
//     // Transform data from your logger output
//     // link_5 position relative to world
//     tf2::Vector3 link5_position(0.094, -0.000, 0.164);
//     tf2::Quaternion link5_orientation(-0.000, -0.000, 0.000, 1.000);
    
//     // gripper_center position relative to world
//     tf2::Vector3 gripper_position(0.219, -0.000, 0.134);
//     tf2::Quaternion gripper_orientation(-0.000, -0.000, 0.000, 1.000);
    
//     // Calculate the offset from link_5 to gripper_center
//     tf2::Vector3 offset = gripper_position - link5_position;
//     RCLCPP_INFO(node->get_logger(), "Offset from link_5 to gripper_center: x=%f, y=%f, z=%f",
//                 offset.x(), offset.y(), offset.z());
    
//     // detected_cup position relative to world
//     tf2::Vector3 cup_position(0.533, -0.041, 0.150);
//     tf2::Quaternion cup_orientation(-0.000, -0.000, -0.064, 0.998);
    
//     // Calculate the target position for link_5 to place gripper_center at the cup
//     tf2::Vector3 target_link5_position = cup_position - offset;
    
//     // Define a target pose for the end effector (link_5)
//     geometry_msgs::msg::Pose target_pose;
//     target_pose.position.x = target_link5_position.x();
//     target_pose.position.y = target_link5_position.y();
//     target_pose.position.z = target_link5_position.z();
    
//     // Use the cup's orientation for the gripper
//     target_pose.orientation.x = cup_orientation.x();
//     target_pose.orientation.y = cup_orientation.y();
//     target_pose.orientation.z = cup_orientation.z();
//     target_pose.orientation.w = cup_orientation.w();
    
//     RCLCPP_INFO(node->get_logger(), "Target pose for link_5: x=%f, y=%f, z=%f, orientation: x=%f, y=%f, z=%f, w=%f",
//                 target_pose.position.x, target_pose.position.y, target_pose.position.z,
//                 target_pose.orientation.x, target_pose.orientation.y, target_pose.orientation.z, target_pose.orientation.w);
                
//     // Add an approach pose that's slightly away from the target
//     double approach_distance = 0.10; // 10cm approach distance
//     geometry_msgs::msg::Pose approach_pose = target_pose;
//     approach_pose.position.x -= approach_distance; // Assuming approach from the negative x direction
    
//     RCLCPP_INFO(node->get_logger(), "Approach pose for link_5: x=%f, y=%f, z=%f",
//                 approach_pose.position.x, approach_pose.position.y, approach_pose.position.z);
    
//     // Create orientation constraints with tolerance
//     moveit_msgs::msg::OrientationConstraint ocm;
//     ocm.link_name = end_effector_link;
//     ocm.header.frame_id = move_group_interface.getPlanningFrame();
//     ocm.orientation = target_pose.orientation;
//     ocm.absolute_x_axis_tolerance = 0.3; // Allow some rotation around x (in radians)
//     ocm.absolute_y_axis_tolerance = 0.3; // Allow some rotation around y (in radians)
//     ocm.absolute_z_axis_tolerance = 0.3; // Allow some rotation around z (in radians)
//     ocm.weight = 1.0;
    
//     // Add this constraint to a Constraints message
//     moveit_msgs::msg::Constraints constraints;
//     constraints.orientation_constraints.push_back(ocm);
    
//     // Try the direct approach first
//     RCLCPP_INFO(node->get_logger(), "Attempting direct planning to target pose...");
    
//     // Set the constraints on the move group
//     move_group_interface.setPathConstraints(constraints);
//     move_group_interface.setPoseTarget(target_pose);
    
//     // Plan and check if planning is successful
//     moveit::planning_interface::MoveGroupInterface::Plan direct_plan;
//     bool direct_success = (move_group_interface.plan(direct_plan) == moveit::core::MoveItErrorCode::SUCCESS);
    
//     if (direct_success) {
//         RCLCPP_INFO(node->get_logger(), "Direct planning succeeded, moving the arm.");
//         move_group_interface.execute(direct_plan);
//     } else {
//         RCLCPP_INFO(node->get_logger(), "Direct planning failed, trying Cartesian path planning...");
        
//         // Clear constraints for Cartesian planning
//         move_group_interface.clearPathConstraints();
        
//         // Try Cartesian path planning
//         std::vector<geometry_msgs::msg::Pose> waypoints;
        
//         // Get current pose as starting point
//         geometry_msgs::msg::PoseStamped current_pose = move_group_interface.getCurrentPose();
        
//         // Create intermediate waypoint that's higher up to avoid obstacles
//         geometry_msgs::msg::Pose via_pose = current_pose.pose;
//         via_pose.position.z += 0.1; // Move 10cm up from current position
//         waypoints.push_back(via_pose);
        
//         // Add approach pose as waypoint
//         waypoints.push_back(approach_pose);
        
//         // Add target pose as waypoint
//         waypoints.push_back(target_pose);
        
//         moveit_msgs::msg::RobotTrajectory trajectory;
//         const double jump_threshold = 0.0;
//         const double eef_step = 0.01;
//         double fraction = move_group_interface.computeCartesianPath(waypoints, eef_step, jump_threshold, trajectory);
        
//         if (fraction > 0.8) { // Accept if at least 80% of the path is valid
//             RCLCPP_INFO(node->get_logger(), "Cartesian path computed successfully (%.2f%% achieved)", fraction * 100.0);
            
//             // Execute the trajectory
//             moveit::planning_interface::MoveGroupInterface::Plan cartesian_plan;
//             cartesian_plan.trajectory_ = trajectory;
//             move_group_interface.execute(cartesian_plan);
//         } else {
//             RCLCPP_ERROR(node->get_logger(), "Cartesian path computation failed: only %.2f%% of the path was computed", fraction * 100.0);
            
//             // Try a fallback approach: move to a "ready position" first
//             RCLCPP_INFO(node->get_logger(), "Attempting to move to a ready position first...");
            
//             // Define a "ready position" that gives good maneuverability
//             geometry_msgs::msg::Pose ready_pose;
//             ready_pose.position.x = 0.382;  // Adjust these coordinates based on your robot
//             ready_pose.position.y = -0.119;
//             ready_pose.position.z = 0.3;
//             ready_pose.orientation.x = 0.0;
//             ready_pose.orientation.y = 0.0;
//             ready_pose.orientation.z = -0.151;
//             ready_pose.orientation.w = 1.0;
            
//             move_group_interface.setPoseTarget(ready_pose);
//             moveit::planning_interface::MoveGroupInterface::Plan ready_plan;
//             bool ready_success = (move_group_interface.plan(ready_plan) == moveit::core::MoveItErrorCode::SUCCESS);
            
//             if (ready_success) {
//                 RCLCPP_INFO(node->get_logger(), "Successfully planned to ready position, executing...");
//                 move_group_interface.execute(ready_plan);
                
//                 // Now try the target again from the ready position
//                 move_group_interface.setPoseTarget(target_pose);
//                 moveit::planning_interface::MoveGroupInterface::Plan final_plan;
//                 bool final_success = (move_group_interface.plan(final_plan) == moveit::core::MoveItErrorCode::SUCCESS);
                
//                 if (final_success) {
//                     RCLCPP_INFO(node->get_logger(), "Successfully planned from ready position to target, executing...");
//                     move_group_interface.execute(final_plan);
//                 } else {
//                     RCLCPP_ERROR(node->get_logger(), "Failed to plan from ready position to target.");
//                 }
//             } else {
//                 RCLCPP_ERROR(node->get_logger(), "Failed to plan to ready position.");
//             }
//         }
//     }
    
//     rclcpp::shutdown();
//     return 0;
// }