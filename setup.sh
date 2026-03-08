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

# Update system
print_status "Updating system packages..."
sudo apt update
sudo apt install -y net-tools
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
sudo apt update && sudo apt install -y curl
export ROS_APT_SOURCE_VERSION=$(curl -s https://api.github.com/repos/ros-infrastructure/ros-apt-source/releases/latest | grep -F "tag_name" | awk -F\" '{print $4}')
curl -L -o /tmp/ros2-apt-source.deb "https://github.com/ros-infrastructure/ros-apt-source/releases/download/${ROS_APT_SOURCE_VERSION}/ros2-apt-source_${ROS_APT_SOURCE_VERSION}.$(. /etc/os-release && echo ${UBUNTU_CODENAME:-${VERSION_CODENAME}})_all.deb"
sudo dpkg -i /tmp/ros2-apt-source.deb

sudo apt update
sudo apt upgrade -y
sudo apt install -y ros-humble-desktop
print_success "ROS2 Humble installed"

# Source ROS2
export ROS_DISTRO=humble
source /opt/ros/$ROS_DISTRO/setup.bash

# Install VS Code
print_status "Downloading VS Code for ARM64..."
wget -O /tmp/vscode.deb "https://code.visualstudio.com/sha/download?build=stable&os=linux-deb-arm64"
print_success "VS Code downloaded"

print_status "Installing VS Code..."
sudo apt install -y /tmp/vscode.deb
print_success "VS Code installed"

# Clean up VS Code deb file
rm /tmp/vscode.deb

print_success "Environment configured"

echo ""
echo -e "${GREEN}========================================${NC}"
echo -e "${GREEN}  Setup Complete!${NC}"
echo -e "${GREEN}========================================${NC}"
echo ""
echo "Installed software:"
echo "  - ROS2 Humble"
# echo "  - MoveIt2 (from source)"
echo "  - VS Code"
# echo "  - CycloneDDS"
echo ""
echo "Please run: source ~/.bashrc"
echo "Or restart your terminal to apply changes."
echo ""
echo "Workspace location: ~/ws_moveit2/"
echo ""

sudo apt install ros-humble-moveit-resources-panda-moveit-config
ros2 launch moveit_resources_panda_moveit_config demo.launch.py rviz_config:=panda_moveit_config_demo.rviz

sudo apt install ros-humble-moveit
