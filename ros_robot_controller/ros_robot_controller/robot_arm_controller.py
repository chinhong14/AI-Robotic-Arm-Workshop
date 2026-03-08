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
                pulse_1 = int((joint_angle / 3.142) * (880-120) + 500)

                # self.get_logger().info(f"Joint_1: {joint_angle:.3f} rad -> Servo1: {pulse_1} pulse")

                # Define servo commands for IDs 1 and 2
                servo1 = BusServoState()
                servo1.present_id = [1,1]
                servo1.position = [1,pulse_1]
                
                bus_servos.append(servo1)

            if joint_name == "joint_2":
                # Convert radians to pulses
                pulse_2 = int((joint_angle / 3.142) * (880-120) + 120)
                pulse_3 = 880 - int((joint_angle / 3.142) * (880-120))  # Inverted

                # self.get_logger().info(f"Joint_2: {joint_angle:.3f} rad -> Servo2: {pulse_2} pulse, Servo3: {pulse_3} pulse")

                # Define servo commands for IDs 1 and 2
                servo2 = BusServoState()
                servo2.present_id = [1,2]
                servo2.position = [1,pulse_2]

                servo3 = BusServoState()
                servo3.present_id = [1,3]
                servo3.position = [1,pulse_3]

                
                bus_servos.append(servo2)
                bus_servos.append(servo3)    

            if joint_name == "joint_3":
                # Convert radians to pulses
                # print(joint_angle)
                pulse_4 = 880 + int((joint_angle / 3.142) * (880-120))
                pulse_5 = 120 - int((joint_angle / 3.142) * (880-120))  # Inverted
                # print (int((abs(joint_angle) / 3.142) * (full_range_pulse)))

                # self.get_logger().info(f"Joint_3: {joint_angle:.3f} rad -> Servo4: {pulse_4} pulse, Servo5: {pulse_5} pulse")

                # Define servo commands for IDs 1 and 2
                servo4 = BusServoState()
                servo4.present_id = [1,4]
                servo4.position = [1,pulse_4]

                servo5 = BusServoState()
                servo5.present_id = [1,5]
                servo5.position = [1,pulse_5]

                
                bus_servos.append(servo4)
                bus_servos.append(servo5)   

            if joint_name == "joint_4":
                # Convert radians to pulses
                pulse_6 = int((joint_angle / 3.142) * (865-105) + 500)

                # self.get_logger().info(f"Joint_4: {joint_angle:.3f} rad -> Servo6: {pulse_6} pulse")

                # Define servo commands for IDs 1 and 2
                servo6 = BusServoState()
                servo6.present_id = [1,6]
                servo6.position = [1,pulse_6]

                bus_servos.append(servo6)       

            if joint_name == "joint_5":
                # Convert radians to pulses
                pulse_7 = int((joint_angle / 3.142) * (865-120) + 500)

                # self.get_logger().info(f"Joint_5: {joint_angle:.3f} rad -> Servo7: {pulse_7} pulse")

                # Define servo commands for IDs 1 and 2
                servo7 = BusServoState()
                servo7.present_id = [1,7]
                servo7.position = [1,pulse_7]

                bus_servos.append(servo7)

            if joint_name == "right_gripper_joint":
                # Convert radians to pulses
                # pulse_8 = 565 - int((joint_angle / 0.7) * (565-395))
                pulse_8 = 533 - int((joint_angle / 0.7) * (533-400))
                # self.get_logger().info(f"right_gripper_joint: {joint_angle:.3f} rad -> Servo8: {pulse_8} pulse")

                # Define servo commands for IDs 1 and 2
                servo8 = BusServoState()
                servo8.present_id = [1,8]
                servo8.position = [1,pulse_8]

                bus_servos.append(servo8)

            if joint_name == "right_cutter_joint":
                # Convert radians to pulses
                pulse_9 = int((joint_angle / 0.7) * (515-280) + 280)

                # self.get_logger().info(f"right_cutter_joint: {joint_angle:.3f} rad -> Servo9: {pulse_9} pulse")

                # Define servo commands for IDs 1 and 2
                servo9 = BusServoState()
                servo9.present_id = [1,9]
                servo9.position = [1,pulse_9]

                bus_servos.append(servo9)

            # if joint_name == "right_cutter_joint":
            #     # Convert radians to pulses
            #     pulse_9 = int((joint_angle / 0.7) * (555-360) + 360)

            #     self.get_logger().info(f"right_cutter_joint: {joint_angle:.3f} rad -> Servo9: {pulse_9} pulse")

            #     # Define servo commands for IDs 1 and 2
            #     servo9 = BusServoState()
            #     servo9.present_id = [1,9]
            #     servo9.position = [1,pulse_9]

            #     bus_servos.append(pulse_9)         

            # if servo_config['type'] == 'pwm':
            #         # Create PWM servo command
            #         servo = PWMServoState()
            #         servo.id = [servo_config['id']]
            #         servo.position = [pulse]
            #         servo.offset = []
            #         pwm_servos.append(servo)

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



