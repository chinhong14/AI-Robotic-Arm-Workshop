#include <rclcpp/rclcpp.hpp>
#include <moveit/move_group_interface/move_group_interface.h>
#include <sensor_msgs/msg/joy.hpp>

class MoveItPredefinedPosesNode : public rclcpp::Node
{
public:
    MoveItPredefinedPosesNode() : Node("moveit_predefined_poses")
    {
        // Initialize MoveIt groups
        auto node_ptr = std::shared_ptr<rclcpp::Node>(this, [](rclcpp::Node*) {}); 
        arm_group_ = std::make_shared<moveit::planning_interface::MoveGroupInterface>(node_ptr, "arm_group");
        gripper_group_ = std::make_shared<moveit::planning_interface::MoveGroupInterface>(node_ptr, "gripper_group");
        cutter_group_ = std::make_shared<moveit::planning_interface::MoveGroupInterface>(node_ptr, "cutter_group");

        // Set velocity and acceleration scaling
        arm_group_->setMaxVelocityScalingFactor(0.5);
        arm_group_->setMaxAccelerationScalingFactor(0.1);
        gripper_group_->setMaxVelocityScalingFactor(0.5);
        gripper_group_->setMaxAccelerationScalingFactor(0.1);
        cutter_group_->setMaxVelocityScalingFactor(0.5);
        cutter_group_->setMaxAccelerationScalingFactor(0.1);

        // Subscribe to joy topic
        joy_sub_ = this->create_subscription<sensor_msgs::msg::Joy>(
            "/ros_robot_controller/joy",
            10,
            std::bind(&MoveItPredefinedPosesNode::joyCallback, this, std::placeholders::_1));

        RCLCPP_INFO(this->get_logger(), "Waiting for X button press (buttons[3]) to execute motions...");
    }

private:
    std::shared_ptr<moveit::planning_interface::MoveGroupInterface> arm_group_;
    std::shared_ptr<moveit::planning_interface::MoveGroupInterface> gripper_group_;
    std::shared_ptr<moveit::planning_interface::MoveGroupInterface> cutter_group_;
    rclcpp::Subscription<sensor_msgs::msg::Joy>::SharedPtr joy_sub_;
    bool motion_executed_ = false;

    void joyCallback(const sensor_msgs::msg::Joy::SharedPtr joy_msg)
    {
        // Check if X button (buttons[3]) is pressed and we haven't executed the motions yet
        if (joy_msg->buttons.size() > 3 && joy_msg->buttons[3] == 1.0 && !motion_executed_)
        {
            motion_executed_ = true;
            RCLCPP_INFO(this->get_logger(), "X button pressed. Starting motion sequence...");
            
            // Execute all motions in sequence
            executeMotionSequence();
        }
        else if (joy_msg->buttons.size() > 3 && joy_msg->buttons[3] != 1.0)
        {
            // Reset the flag when button is released
            motion_executed_ = false;
        }
    }

    void executeMotionSequence()
    {
        // Move to the "rest" pose
        RCLCPP_INFO(this->get_logger(), "Moving to 'rest' pose...");
        arm_group_->setNamedTarget("rest");
        executePlan(arm_group_);

        // Move to the "up_forward" pose
        RCLCPP_INFO(this->get_logger(), "Moving to 'up_forward' pose...");
        arm_group_->setNamedTarget("up_forward");
        executePlan(arm_group_);

        // Move to the "full_up" pose
        RCLCPP_INFO(this->get_logger(), "Moving to 'full_up' pose...");
        arm_group_->setNamedTarget("full_up");
        executePlan(arm_group_);

        // Move to the "up_forward" pose
        RCLCPP_INFO(this->get_logger(), "Moving to 'up_forward' pose...");
        arm_group_->setNamedTarget("up_forward");
        executePlan(arm_group_);

        // Move to the "left_up_forward" pose
        RCLCPP_INFO(this->get_logger(), "Moving to 'left_up_forward' pose...");
        arm_group_->setNamedTarget("left_up_forward");
        executePlan(arm_group_);
        
        // Move to the "right_up_forward" pose
        RCLCPP_INFO(this->get_logger(), "Moving to 'right_up_forward' pose...");
        arm_group_->setNamedTarget("right_up_forward");
        executePlan(arm_group_);

        // Move to the "detect" pose
        RCLCPP_INFO(this->get_logger(), "Moving to 'detect' pose...");
        arm_group_->setNamedTarget("detect");
        executePlan(arm_group_);

        // Move to the "right_move" pose
        RCLCPP_INFO(this->get_logger(), "Moving to 'right_move' pose...");
        arm_group_->setNamedTarget("right_move");
        executePlan(arm_group_);

        // Move to the "left_move" pose
        RCLCPP_INFO(this->get_logger(), "Moving to 'left_move' pose...");
        arm_group_->setNamedTarget("left_move");
        executePlan(arm_group_);

        // Move to the "detect" pose
        RCLCPP_INFO(this->get_logger(), "Moving to 'detect' pose...");
        arm_group_->setNamedTarget("detect");
        executePlan(arm_group_);

        // Open gripper
        RCLCPP_INFO(this->get_logger(), "Moving to 'gripper_open' pose...");
        gripper_group_->setNamedTarget("open");
        executePlan(gripper_group_);

        // Open cutter
        RCLCPP_INFO(this->get_logger(), "Moving to 'cutter_open' pose...");
        cutter_group_->setNamedTarget("open");
        executePlan(cutter_group_);

        // Move to the "up_forward" pose
        RCLCPP_INFO(this->get_logger(), "Moving to 'up_forward' pose...");
        arm_group_->setNamedTarget("up_forward");
        executePlan(arm_group_);

        // Hold gripper
        RCLCPP_INFO(this->get_logger(), "Moving to 'gripper_hold' pose...");
        gripper_group_->setNamedTarget("hold");
        executePlan(gripper_group_);

        // Close cutter
        RCLCPP_INFO(this->get_logger(), "Moving to 'cutter_close' pose...");
        cutter_group_->setNamedTarget("close");
        executePlan(cutter_group_);

        // Move to the "detect" pose
        RCLCPP_INFO(this->get_logger(), "Moving to 'detect' pose...");
        arm_group_->setNamedTarget("detect");
        executePlan(arm_group_);

        // Move to the "rest" pose
        RCLCPP_INFO(this->get_logger(), "Moving to 'rest' pose...");
        arm_group_->setNamedTarget("rest");
        executePlan(arm_group_);

        // Hold gripper
        RCLCPP_INFO(this->get_logger(), "Moving to 'gripper_close' pose...");
        gripper_group_->setNamedTarget("close");
        executePlan(gripper_group_);

        RCLCPP_INFO(this->get_logger(), "Motion sequence completed!");
    }

    void executePlan(std::shared_ptr<moveit::planning_interface::MoveGroupInterface> move_group)
    {
        moveit::planning_interface::MoveGroupInterface::Plan plan;
        bool success = (move_group->plan(plan) == moveit::core::MoveItErrorCode::SUCCESS);

        if (success)
        {
            RCLCPP_INFO(this->get_logger(), "Executing...");
            move_group->execute(plan);
        }
        else
        {
            RCLCPP_ERROR(this->get_logger(), "Planning failed!");
        }
    }
};

int main(int argc, char** argv)
{
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<MoveItPredefinedPosesNode>());
    rclcpp::shutdown();
    return 0;
}

// #include <rclcpp/rclcpp.hpp>
// #include <moveit/move_group_interface/move_group_interface.h>

// class MoveItPredefinedPosesNode : public rclcpp::Node
// {
// public:
//     MoveItPredefinedPosesNode() : Node("moveit_predefined_poses")
//     {
//         // Fix shared_ptr issue
//         auto node_ptr = std::shared_ptr<rclcpp::Node>(this, [](rclcpp::Node*) {}); 
//         auto arm_group = std::make_shared<moveit::planning_interface::MoveGroupInterface>(node_ptr, "arm_group");
//         auto gripper_group = std::make_shared<moveit::planning_interface::MoveGroupInterface>(node_ptr, "gripper_group");
//         auto cutter_group = std::make_shared<moveit::planning_interface::MoveGroupInterface>(node_ptr, "cutter_group");

//         // Set velocity and acceleration scaling to move faster
//         arm_group->setMaxVelocityScalingFactor(0.5);  // 1.0 = Full speed
//         arm_group->setMaxAccelerationScalingFactor(0.1);  // 1.0 = Max acceleration

//         gripper_group->setMaxVelocityScalingFactor(0.5);  // 1.0 = Full speed
//         gripper_group->setMaxAccelerationScalingFactor(0.1);  // 1.0 = Max acceleration

//         cutter_group->setMaxVelocityScalingFactor(0.5);  // 1.0 = Full speed
//         cutter_group->setMaxAccelerationScalingFactor(0.1);  // 1.0 = Max acceleration

//         // Move to the "rest" pose
//         RCLCPP_INFO(this->get_logger(), "Moving to 'rest' pose...");
//         arm_group->setNamedTarget("rest");
//         executePlan(arm_group);

//         // Move to the "rest" pose
//         RCLCPP_INFO(this->get_logger(), "Moving to 'up_forward' pose...");
//         arm_group->setNamedTarget("up_forward");
//         executePlan(arm_group);

//         // Move to the "rest" pose
//         RCLCPP_INFO(this->get_logger(), "Moving to 'full_up' pose...");
//         arm_group->setNamedTarget("full_up");
//         executePlan(arm_group);

//         // Move to the "rest" pose
//         RCLCPP_INFO(this->get_logger(), "Moving to 'up_forward' pose...");
//         arm_group->setNamedTarget("up_forward");
//         executePlan(arm_group);

//         // Move to the "rest" pose
//         RCLCPP_INFO(this->get_logger(), "Moving to 'left_up_forward' pose...");
//         arm_group->setNamedTarget("left_up_forward");
//         executePlan(arm_group);
        
//         // Move to the "rest" pose
//         RCLCPP_INFO(this->get_logger(), "Moving to 'right_up_forward' pose...");
//         arm_group->setNamedTarget("right_up_forward");
//         executePlan(arm_group);

//         // Move to the "rest" pose
//         RCLCPP_INFO(this->get_logger(), "Moving to 'detect' pose...");
//         arm_group->setNamedTarget("detect");
//         executePlan(arm_group);

//         // Move to the "pick" pose
//         RCLCPP_INFO(this->get_logger(), "Moving to 'right_move' pose...");
//         arm_group->setNamedTarget("right_move");
//         executePlan(arm_group);

//         // Move to the "pick" pose
//         RCLCPP_INFO(this->get_logger(), "Moving to 'left_move' pose...");
//         arm_group->setNamedTarget("left_move");
//         executePlan(arm_group);

//         // Move to the "rest" pose
//         RCLCPP_INFO(this->get_logger(), "Moving to 'detect' pose...");
//         arm_group->setNamedTarget("detect");
//         executePlan(arm_group);

//         // Move to the "pick" pose
//         RCLCPP_INFO(this->get_logger(), "Moving to 'gripper_open' pose...");
//         gripper_group->setNamedTarget("open");
//         executePlan(gripper_group);

//         // Move to the "pick" pose
//         RCLCPP_INFO(this->get_logger(), "Moving to 'cutter_open' pose...");
//         cutter_group->setNamedTarget("open");
//         executePlan(cutter_group);

//         // Move to the "rest" pose
//         RCLCPP_INFO(this->get_logger(), "Moving to 'up_forward' pose...");
//         arm_group->setNamedTarget("up_forward");
//         executePlan(arm_group);

//         // Move to the "pick" pose
//         RCLCPP_INFO(this->get_logger(), "Moving to 'gripper_hold' pose...");
//         gripper_group->setNamedTarget("hold");
//         executePlan(gripper_group);

//         // Move to the "pick" pose
//         RCLCPP_INFO(this->get_logger(), "Moving to 'cutter_close' pose...");
//         cutter_group->setNamedTarget("close");
//         executePlan(cutter_group);

//         // Move to the "pick" pose
//         RCLCPP_INFO(this->get_logger(), "Moving to 'detect' pose...");
//         arm_group->setNamedTarget("detect");
//         executePlan(arm_group);

//     }

// private:
//     void executePlan(std::shared_ptr<moveit::planning_interface::MoveGroupInterface> move_group)
//     {
//         moveit::planning_interface::MoveGroupInterface::Plan plan;
//         bool success = (move_group->plan(plan) == moveit::core::MoveItErrorCode::SUCCESS);

//         if (success)
//         {
//             RCLCPP_INFO(this->get_logger(), "Executing...");
//             move_group->execute(plan);
//         }
//         else
//         {
//             RCLCPP_ERROR(this->get_logger(), "Planning failed!");
//         }
//     }
// };

// int main(int argc, char** argv)
// {
//     rclcpp::init(argc, argv);
//     rclcpp::spin(std::make_shared<MoveItPredefinedPosesNode>());
//     rclcpp::shutdown();
//     return 0;
// }
