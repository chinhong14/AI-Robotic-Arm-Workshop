#include <rclcpp/rclcpp.hpp>
#include <moveit/move_group_interface/move_group_interface.h>

class MoveItPredefinedPosesNode : public rclcpp::Node
{
public:
    MoveItPredefinedPosesNode() : Node("moveit_predefined_poses")
    {
        // Initialize MoveIt groups
        auto node_ptr = std::shared_ptr<rclcpp::Node>(this, [](rclcpp::Node*) {}); 
        arm_group_ = std::make_shared<moveit::planning_interface::MoveGroupInterface>(node_ptr, "arm_group");
        gripper_group_ = std::make_shared<moveit::planning_interface::MoveGroupInterface>(node_ptr, "gripper_group");

        // Set velocity and acceleration scaling
        arm_group_->setMaxVelocityScalingFactor(1.0);
        arm_group_->setMaxAccelerationScalingFactor(0.3);
        gripper_group_->setMaxVelocityScalingFactor(0.1);
        gripper_group_->setMaxAccelerationScalingFactor(0.1);

    }
    
    void executeMotionSequence()
    {
        // Move to the "rest" pose
        RCLCPP_INFO(this->get_logger(), "Moving to 'rest' pose...");
        arm_group_->setNamedTarget("rest");
        executePlan(arm_group_);
        rclcpp::sleep_for(std::chrono::seconds(1)); // Sleep for a moment to ensure the robot has settled   
        
        // Move to the "initial_pose" pose
        RCLCPP_INFO(this->get_logger(), "Moving to 'initial_pose' pose...");
        arm_group_->setNamedTarget("initial_pose");
        executePlan(arm_group_);
        rclcpp::sleep_for(std::chrono::seconds(1));

        // Open gripper
        RCLCPP_INFO(this->get_logger(), "Moving to 'gripper_open' pose...");
        gripper_group_->setNamedTarget("open");
        executePlan(gripper_group_);
        rclcpp::sleep_for(std::chrono::seconds(1));

        // Move to the "pose_1" pose
        RCLCPP_INFO(this->get_logger(), "Moving to 'pose_1' pose...");
        arm_group_->setNamedTarget("pose_1");
        executePlan(arm_group_);
        rclcpp::sleep_for(std::chrono::seconds(3));

        // hold gripper
        RCLCPP_INFO(this->get_logger(), "Moving to 'hold' pose...");
        gripper_group_->setNamedTarget("hold");
        executePlan(gripper_group_);
        rclcpp::sleep_for(std::chrono::seconds(1));

        // Move to the "pose_2" pose
        RCLCPP_INFO(this->get_logger(), "Moving to 'pose_2' pose...");
        arm_group_->setNamedTarget("pose_2");
        executePlan(arm_group_);

        // Move to the "pose_3" pose
        RCLCPP_INFO(this->get_logger(), "Moving to 'pose_3' pose...");
        arm_group_->setNamedTarget("pose_3");
        executePlan(arm_group_);

        // Move to the "pose_4" pose
        RCLCPP_INFO(this->get_logger(), "Moving to 'pose_4' pose...");
        arm_group_->setNamedTarget("pose_4");
        executePlan(arm_group_);

        // Move to the "pose_5" pose
        RCLCPP_INFO(this->get_logger(), "Moving to 'pose_5' pose...");
        arm_group_->setNamedTarget("pose_5");
        executePlan(arm_group_);
        rclcpp::sleep_for(std::chrono::seconds(1));

        // open gripper
        RCLCPP_INFO(this->get_logger(), "Moving to 'open' pose...");
        gripper_group_->setNamedTarget("open");
        executePlan(gripper_group_);
        rclcpp::sleep_for(std::chrono::seconds(1));

        // close gripper
        RCLCPP_INFO(this->get_logger(), "Moving to 'close' pose...");
        gripper_group_->setNamedTarget("close");
        executePlan(gripper_group_);
        rclcpp::sleep_for(std::chrono::seconds(1));

        // Move to the "rest" pose
        RCLCPP_INFO(this->get_logger(), "Moving to 'rest' pose...");
        arm_group_->setNamedTarget("rest");
        executePlan(arm_group_);
        rclcpp::sleep_for(std::chrono::seconds(1));

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

private:
    std::shared_ptr<moveit::planning_interface::MoveGroupInterface> arm_group_;
    std::shared_ptr<moveit::planning_interface::MoveGroupInterface> gripper_group_;

    
};

int main(int argc, char** argv)
{
    rclcpp::init(argc, argv);

    auto node = std::make_shared<MoveItPredefinedPosesNode>();

    // Run once
    node->executeMotionSequence();

    rclcpp::spin(node);

    rclcpp::shutdown();
    return 0;
}
