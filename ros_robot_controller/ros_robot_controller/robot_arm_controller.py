import rclpy
from rclpy.node import Node
from ros_robot_controller_msgs.msg import (
    SetBusServoState, 
    BusServoState,
    SetPWMServoState,
    PWMServoState
)
from sensor_msgs.msg import JointState
import numpy as np

class RobotArmController(Node):
    def __init__(self):
        super().__init__('robot_arm_controller')
        
        # Publishers for servos
        self.bus_publisher = self.create_publisher(
            SetBusServoState, 
            '/ros_robot_controller/bus_servo/set_state', 
            1
        )
        self.pwm_publisher = self.create_publisher(
            SetPWMServoState,
            '/ros_robot_controller/pwm_servo/set_state',
            10
        )
        
        # Subscribe to joint states from MoveIt2
        self.joint_states_subscriber = self.create_subscription(
            JointState,
            '/joint_states',
            self.joint_states_callback,
            10
        )

    def joint_states_callback(self, msg: JointState):
        # Create messages for both servo types
        bus_msg = SetBusServoState()
        bus_msg.duration = 0.01
        
        pwm_msg = SetPWMServoState()
        pwm_msg.duration = 0.01

        bus_servos = []
        pwm_servos = []

        # full_range_pulse = 875-125
        full_range_pulse = 750
        # Process each joint
        for joint_name, joint_angle in zip(msg.name, msg.position):
            if joint_name == "joint_1":
                # Convert radians to pulses
                pulse_1 = 500 - int((joint_angle / 3.142) * (880-120))

                # self.get_logger().info(f"Joint_1: {joint_angle:.3f} rad -> Servo1: {pulse_1} pulse")

                # Define servo commands for IDs 1 and 2
                servo1 = BusServoState()
                servo1.present_id = [1,1]
                servo1.position = [1,pulse_1]
                
                bus_servos.append(servo1)

            if joint_name == "joint_2":
                pwm_val2 = 825 + int((2220-825) * (joint_angle / 3.142))

                # self.get_logger().info(f"Joint_2: {joint_angle:.3f} rad -> Servo3: {pwm_val2} pulse")

                pwm_servo_2 = PWMServoState()
                pwm_servo_2.id = [2]
                pwm_servo_2.position = [pwm_val2]
                pwm_servo_2.offset = [0]
                pwm_servos.append(pwm_servo_2)


            if joint_name == "joint_3":
                pwm_val3 = 2220 - int((2220-825) * (joint_angle / 3.142))

                # self.get_logger().info(f"Joint_3: {joint_angle:.3f} rad -> Servo4: {pwm_val3} pulse")

                pwm_servo_3 = PWMServoState()
                pwm_servo_3.id = [3]
                pwm_servo_3.position = [pwm_val3]
                pwm_servo_3.offset = []
                pwm_servos.append(pwm_servo_3)

            if joint_name == "joint_4":
                # Convert radians to pulses
                pulse_4 = 500 - int((joint_angle / 3.142) * (880-110))

                # self.get_logger().info(f"Joint_4: {joint_angle:.3f} rad -> Servo6: {pulse_4} pulse")

                # Define servo commands for IDs 1 and 2
                servo4 = BusServoState()
                servo4.present_id = [1,4]
                servo4.position = [1,pulse_4]

                bus_servos.append(servo4)       

            if joint_name == "joint_5":
                # Convert radians to pulses
                pulse_5 = int((joint_angle / 3.142) * (880-110) + 500)

                # self.get_logger().info(f"Joint_5: {joint_angle:.3f} rad -> Servo7: {pulse_5} pulse")

                # Define servo commands for IDs 1 and 2
                servo5 = BusServoState()
                servo5.present_id = [1,5]
                servo5.position = [1,pulse_5]

                bus_servos.append(servo5)

            if joint_name == "joint_7":
                # Convert radians to pulses
                pulse_6 = 850 + int((joint_angle/0.04) * (850-120))
                # self.get_logger().info(f"right_gripper_joint: {joint_angle:.3f} rad -> Servo8: {pulse_6} pulse")

                # Define servo commands for IDs 1 and 2
                servo6= BusServoState()
                servo6.present_id = [1,6]
                servo6.position = [1,pulse_6]

                bus_servos.append(servo6)

        # Publish if we have servos to move
        if bus_servos:
            bus_msg.state = bus_servos
            self.bus_publisher.publish(bus_msg)
            
        if pwm_servos:
            pwm_msg.state = pwm_servos
            self.pwm_publisher.publish(pwm_msg)
            
        # Log the movement
        # self.get_logger().info(f'Moving joints: {msg.name} to positions: {msg.position}')

def main(args=None):
    rclpy.init(args=args)
    controller = RobotArmController()
    
    try:
        rclpy.spin(controller)
    except KeyboardInterrupt:
        print('\nShutting down robot arm controller...')
    finally:
        # Move to safe position before shutdown
        controller.destroy_node()
        rclpy.shutdown()

if __name__ == '__main__':
    main()



