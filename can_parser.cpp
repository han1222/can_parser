#include <bitset>
#include <ros/ros.h>
#include <iostream>
#include <can_msgs/Frame.h>
#include <std_msgs/Float32.h>


class CAN_PARSER
{
public:
    CAN_PARSER(ros::NodeHandle& n);
    ~CAN_PARSER();

    void CanCallback(const can_msgs::Frame& msg);

private:
    ros::NodeHandle nh;
    ros::Subscriber raw_can_sub;
    ros::Publisher parsed_data_pub;
    std_msgs::Float32 can_signalname1;
    std_msgs::Float32 can_signalname2;
};

CAN_PARSER::CAN_PARSER(ros::Nodehandle& n)
{
    ROS_INFO("CAN_PARSER is created");
    nh = n;
    raw_can_sub = nh.subscribe("/received_messages" 10, &CAN_PARSER::CanCallback, this);
    parsed_data_pub = nh.advertise<std_msgs::Float32>("/parsed_can_msg", 5);
}
CAN_PARSER::~CAN_PARSER()
{
    ROS_INFO("CAN_PASER in destructed");
}

void CAN_PARSER::CanCallback(const can_msgs::Frame& msg)
{
    if (msg.id == 0x520)
    {
        //star bit�� 0�̰� signal length�� 24�� �� 
        bitset<24> temp = bitset<24>(msg.data[2]) << 16;
        temp = temp | bitset<24>(msg.data[1]) << 8;
        temp = temp | bitset<24>(msg.data[0]);
        double data = (double)(temp.to_ulong());
        data = data * 0.1; //���⼭ offset�� 0�̰� factor�� 0.1�̴� 
        can_signalname1.data = data;
        parsed_data_pub.publish(can_signalname1);
    }
    else if (msg.id == 0x170)
    {
        //star bit�� 11�̰� signal length�� 4�� �� 
        bitset<7> temp1 = bitset<7>(msg.data[1]) >> 3;
        double long data1 = (double)(temp1.to_ulong());
        can_signalname2.data = data; ////���⼭ offset�� factor�� 0�̴� 
        parsed_data_pub.publish(can_signalname2);
    }

    int main(int argc, char** argv)
    {
        ros::init(argc, argv, "CAN_PARSER")
            ros::NodeHandle _nh;
        printf("Initiate CAN_PARSER\n");

        CAN_PARSER parser(_nh);
        ros::spin();

        printf("CAN_PARSER is Terminated\n");
        return 0;
    }