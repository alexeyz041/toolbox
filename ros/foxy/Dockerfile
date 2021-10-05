FROM ubuntu:20.04
ARG DEBIAN_FRONTEND=noninteractive
RUN apt update
RUN apt install -y mc curl gnupg2 lsb-release
RUN curl -sSL https://raw.githubusercontent.com/ros/rosdistro/master/ros.key -o /usr/share/keyrings/ros-archive-keyring.gpg
RUN echo "deb [arch=$(dpkg --print-architecture) signed-by=/usr/share/keyrings/ros-archive-keyring.gpg] http://packages.ros.org/ros2/ubuntu $(lsb_release -cs) main" | tee /etc/apt/sources.list.d/ros2.list > /dev/null

RUN mkdir -p /ros2_foxy
WORKDIR /ros2_foxy
RUN apt install -y wget
RUN cd /ros2_foxy && wget https://github.com/ros2/ros2/releases/download/release-foxy-20210902/ros2-foxy-20210902-linux-focal-amd64.tar.bz2
RUN cd /ros2_foxy && tar -xf ros2-foxy-20210902-linux-focal-amd64.tar.bz2
RUN apt update
RUN apt install -y python3-rosdep
RUN rosdep init
RUN rosdep update
RUN rosdep install --from-paths /ros2_foxy/ros2-linux/share --ignore-src --rosdistro foxy -y --skip-keys "console_bridge fastcdr fastrtps osrf_testing_tools_cpp poco_vendor rmw_connextdds rti-connext-dds-5.3.1 tinyxml_vendor tinyxml2_vendor urdfdom urdfdom_headers"
RUN apt install -y libpython3-dev python3-pip
RUN pip3 install -U argcomplete
