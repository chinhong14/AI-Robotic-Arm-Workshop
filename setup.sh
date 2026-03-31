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
sudo apt purge -y libreoffice*
sudo apt purge -y thunderbird
sudo apt purge -y qt*
sudo apt purge -y aisleriot gnome-mahjongg gnome-mines gnome-sudoku gnome-games*
sudo apt autoremove -y 
sudo apt clean -y 

# Update system
print_status "Updating system packages..."
sudo apt update
sudo apt install -y net-tools wget curl
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
sudo apt install python3-colcon-common-extensions
print_success "ROS2 Humble installed"

# Source ROS2
export ROS_DISTRO=humble
source /opt/ros/$ROS_DISTRO/setup.bash

# # Initialize rosdep
# print_status "Initializing rosdep..."
# sudo rosdep init || true  # Don't fail if already initialized
# rosdep update
# print_success "rosdep initialized"

# Install MoveIt2
print_status "Installing MoveIt2..."
sudo apt install -y ros-humble-moveit 
sudo apt install ros-humble-ros2-control ros-humble-ros2-controllers
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

# Setup environment variables in bashrc
print_status "Configuring environment variables..."
cat >> ~/.bashrc << 'EOF'

# ROS2 Setup
export ROS_DISTRO=humble
source /opt/ros/$ROS_DISTRO/setup.bash
EOF

print_success "Environment configured"

# Clone Robotarm Repo
print_status "Cloning Robotarm repository..."
# Create a directory for the workspace
mkdir -p ~/workspace/
cd ~/workspace/

# TODO: Replace with your actual GitHub repository URL
sudo apt install -y git-all
git clone git clone https://github.com/chinhong14/AI-Robotic-Arm-Workshop.git
cd ~/workspace/AI-Robotic-Arm-Workshop
colcon build

# For now, skip if repo doesn't exist
if [ -n "${ROBOT_REPO_URL}" ]; then
    git clone ${ROBOT_REPO_URL}
    print_success "Robotarm repository cloned"
else
    echo "ROBOT_REPO_URL not set - skipping repository clone"
    echo "To clone your repo later, run:"
    echo "  cd ~/workspace/ && git clone https://github.com/chinhong14/AI-Robotic-Arm-Workshop.git"
fi

source ~/workspace/AI-Robotic-Arm-Workshop/install/setup.bash
echo ""
echo -e "${GREEN}========================================${NC}"
echo -e "${GREEN}  Setup Complete!${NC}"
echo -e "${GREEN}========================================${NC}"
echo ""
echo "Installed software:"
echo "  - ROS2 Humble Desktop"
echo "  - MoveIt2 (binary package)"
echo "  - MoveIt2 Panda Resources"
echo "  - VS Code"
echo ""
echo "Workspace location: ~/workspace/"
echo ""
echo "Next steps:"
echo "  1. Run: source ~/.bashrc"
echo "  2. Set your robot repo URL and clone:"
echo "     export ROBOT_REPO_URL=https://github.com/yourusername/robotarm.git"
echo "     cd ~/workspace/ && git clone \$ROBOT_REPO_URL"
echo ""
