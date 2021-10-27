// colcon build --packages-select my_robot_tutorials

#include "rclcpp/rclcpp.hpp"

class MyNode : public rclcpp::Node
{
public:
    MyNode() : Node("my_node_name") {
        timer_ = this->create_wall_timer(
            std::chrono::milliseconds(1000),
            std::bind(&MyNode::timerCallback, this));

        this->declare_parameter<std::string>("my_parameter_str", "world");
        this->declare_parameter<int>("my_parameter_int", 123);
    }

private:
    void timerCallback()
    {
        this->get_parameter("my_parameter_str", parameter_string_);
        this->get_parameter("my_parameter_int", parameter_int_);
        RCLCPP_INFO(this->get_logger(), "Hello from ROS2 %s %d", parameter_string_.c_str(), parameter_int_);
    }

    rclcpp::TimerBase::SharedPtr timer_;
    std::string parameter_string_;
    int parameter_int_;
};

int main(int argc, char **argv)
{
    rclcpp::init(argc, argv);
    auto node = std::make_shared<MyNode>();
    rclcpp::spin(node);
    rclcpp::shutdown();
}
