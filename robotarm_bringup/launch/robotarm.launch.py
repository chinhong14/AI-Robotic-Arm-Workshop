import os
from launch import LaunchDescription
from launch.actions import IncludeLaunchDescription
from ament_index_python.packages import get_package_share_directory
from launch_ros.actions import Node

def generate_launch_description():
    
    moveit = IncludeLaunchDescription(
            os.path.join(
                get_package_share_directory("robotarm_moveit_config"),
                "launch",
                "demo.launch.py"
            ),
            launch_arguments={"is_sim": "False"}.items()
        )

    ros_robot_controller = IncludeLaunchDescription(
            os.path.join(
                get_package_share_directory("ros_robot_controller"),
                "launch",
                "ros_robot_controller.launch.xml"
            ),
        )

    robot_arm_controller = Node(
        package='ros_robot_controller',
        executable='robot_arm_controller',
        name='robot_arm_controller',
        output='screen'
    )

    return LaunchDescription([
        ros_robot_controller,
        robot_arm_controller,
        moveit,
    ])