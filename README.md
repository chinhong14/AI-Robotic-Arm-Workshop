# AI-Robotic-Arm-Workshop

# Robotic Arm Setup Script

This repository contains an automated setup script for deploying ROS2 Humble with MoveIt2 on Raspberry Pi (Ubuntu 22.04) for robotic arm mass production.

## 📋 Prerequisites

### Hardware Requirements
- Raspberry Pi 4 (4GB RAM minimum, 8GB recommended)
- MicroSD card (32GB minimum, 64GB recommended)
- Stable internet connection
- Power supply for Raspberry Pi

### Setup Method and Process

```bash
# Install git if not already installed
sudo apt update
sudo apt install -y git

# Clone the repository
git clone https://github.com/yourusername/robotarm.git
cd robotarm

# Make the script executable
chmod +x setup.sh

# Run the script
./setup.sh
```

## 📦 What Gets Installed

The setup script automatically installs and configures:

1. **System Utilities**
   - net-tools
   - curl, wget
   - build-essential

2. **ROS2 Humble Desktop** (Full installation)
   - Core ROS2 packages
   - Visualization tools (RViz)
   - Development tools

3. **MoveIt2**
   - Binary package installation (faster than building from source)
   - Panda arm configuration and resources

4. **Visual Studio Code**
   - ARM64 version optimized for Raspberry Pi
   - Ready for ROS2 development

5. **Development Environment**
   - Locale configuration (UTF-8)
   - ROS2 environment variables
   - rosdep initialization

## 📝 Post-Installation Steps

After the script completes:

1. **Reload your environment:**
   ```bash
   source ~/.bashrc
   ```
   Or simply restart your terminal/reconnect SSH.

2. **Verify ROS2 installation:**
   ```bash
   ros2 --version
   ```
   Expected output: `ros2 doctor version 0.10.x`

3. **Test MoveIt2:**
   ```bash
   ros2 launch moveit2_tutorials demo.launch.py
   ```

4. **Clone your robot-specific code** (if not done automatically):
   ```bash
   cd ~/workspace/
   git clone https://github.com/yourusername/robotarm.git
   ```