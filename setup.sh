#!/bin/bash
set -e  # Exit on any error

echo "=== Starting Robotic Arm Setup ==="

# Color codes for better output
GREEN='\033[0;32m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

print_status() {
    echo -e "${BLUE}>>> $1${NC}"
}

print_success() {
    echo -e "${GREEN}✓ $1${NC}"
}

# Remove unnecessary packages to save space
print_status "Removing unnecessary packages..."
sudo apt purge -y libreoffice* || true
sudo apt purge -y thunderbird || true
sudo apt purge -y aisleriot gnome-mahjongg gnome-mines gnome-sudoku || true
sudo apt autoremove -y 
sudo apt clean -y 
print_success "Unnecessary packages removed"

# Update system
print_status "Updating system packages..."
sudo apt update
sudo apt install -y net-tools wget curl git-all
print_success "System packages updated"

# Setup locale for ROS2
print_status "Setting up locale for ROS2..."
sudo apt install -y locales
sudo locale-gen en_US en_US.UTF-8
sudo update-locale LC_ALL=en_US.UTF-8 LANG=en_US.UTF-8
export LANG=en_US.UTF-8
print_success "Locale configured"

# Add universe repository
print_status "Adding universe repository..."
sudo apt install -y software-properties-common
sudo add-apt-repository -y universe
print_success "Universe repository added"

# Install ROS2
print_status "Installing ROS2 Humble..."
sudo apt update
export ROS_APT_SOURCE_VERSION=$(curl -s https://api.github.com/repos/ros-infrastructure/ros-apt-source/releases/latest | grep -F "tag_name" | awk -F\" '{print $4}')
curl -L -o /tmp/ros2-apt-source.deb "https://github.com/ros-infrastructure/ros-apt-source/releases/download/${ROS_APT_SOURCE_VERSION}/ros2-apt-source_${ROS_APT_SOURCE_VERSION}.$(. /etc/os-release && echo ${UBUNTU_CODENAME:-${VERSION_CODENAME}})_all.deb"
sudo dpkg -i /tmp/ros2-apt-source.deb

sudo apt update
sudo apt upgrade -y
sudo apt install -y ros-humble-desktop
sudo apt install -y python3-colcon-common-extensions
print_success "ROS2 Humble installed"

# Source ROS2
export ROS_DISTRO=humble
source /opt/ros/$ROS_DISTRO/setup.bash

# Install MoveIt2
print_status "Installing MoveIt2..."
sudo apt install -y ros-humble-moveit 
sudo apt install -y ros-humble-ros2-control ros-humble-ros2-controllers
print_success "MoveIt2 installed"

# Install MoveIt2 Panda Arm Resources
print_status "Installing MoveIt2 Panda Arm Resources..."
sudo apt install -y ros-humble-moveit-resources-panda-moveit-config
print_success "MoveIt2 Panda Arm resources installed"

# Install VS Code
print_status "Downloading VS Code for ARM64..."
wget -O /tmp/vscode.deb "https://code.visualstudio.com/sha/download?build=stable&os=linux-deb-arm64"
print_success "VS Code downloaded"

print_status "Installing VS Code..."
sudo apt install -y /tmp/vscode.deb
print_success "VS Code installed"

# Clean up VS Code deb file
rm /tmp/vscode.deb

# Clone Robotarm Repo
print_status "Cloning Robotarm repository..."
mkdir -p ~/workspace/
cd ~/workspace/

# Clone your repository
git clone https://github.com/chinhong14/AI-Robotic-Arm-Workshop.git
print_success "Robotarm repository cloned"

# Build the workspace
print_status "Building ROS2 workspace..."
cd ~/workspace/AI-Robotic-Arm-Workshop
colcon build
print_success "Workspace built successfully"

# Setup environment variables in bashrc
print_status "Configuring environment variables..."
cat >> ~/.bashrc << 'EOF'

# ROS2 Setup
export ROS_DISTRO=humble
source /opt/ros/$ROS_DISTRO/setup.bash

# Robot Workspace Setup
source ~/workspace/AI-Robotic-Arm-Workshop/install/setup.bash
EOF

print_success "Environment configured"

# Source the workspace for current session
source ~/workspace/AI-Robotic-Arm-Workshop/install/setup.bash

# Add current user 
sudo usermod -aG dialout $USER
newgrp dialout

echo ""
echo -e "${GREEN}========================================${NC}"
echo -e "${GREEN}  Setup Complete!${NC}"
echo -e "${GREEN}========================================${NC}"
echo ""
echo "Installed software:"
echo "  - ROS2 Humble Desktop"
echo "  - MoveIt2 (binary package)"
echo "  - MoveIt2 Panda Resources"
echo "  - ROS2 Control & Controllers"
echo "  - VS Code"
echo "  - AI Robotic Arm Workshop (built)"
echo ""
echo "Workspace location: ~/workspace/AI-Robotic-Arm-Workshop"
echo ""
echo "Next steps:"
echo "  1. Run: source ~/.bashrc"
echo "  2. Or restart your terminal"
echo "  3. Test with: ros2 pkg list | grep robot"
echo ""
