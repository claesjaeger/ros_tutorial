#include <ros/ros.h>
#include <tf/transform_listener.h>
//#include <turtlesim/Velocity.h>
#include <geometry_msgs/Twist.h>
#include <turtlesim/Spawn.h>

int main(int argc, char** argv){
  ros::init(argc, argv, "my_tf_listener");

  ros::NodeHandle node;

  ros::service::waitForService("spawn");
  ros::ServiceClient add_turtle = 
    node.serviceClient<turtlesim::Spawn>("spawn");
  turtlesim::Spawn srv;
  add_turtle.call(srv);

  ros::Publisher turtle_vel = 
    node.advertise<geometry_msgs::Twist>("turtle2/cmd_vel", 10);

  tf::TransformListener listener;
 

  ros::Rate rate(10.0);
  while (node.ok()){
    tf::StampedTransform transform;
    try{
		ros::Time now = ros::Time::now();
		ros::Time past = ros::Time::now() - ros::Duration(5.0);
		 //https://answers.ros.org/question/148121/something-wrong-with-tf-tutorial/
		/*listener.waitForTransform("/turtle2", "/turtle1",
							past, ros::Duration(3.0));
		listener.lookupTransform("/turtle2", "/turtle1",  
                            past, transform);
          */                  
        listener.waitForTransform("/turtle2", now,
								  "/turtle1", past,
                                  "/world", ros::Duration(1.0));
		listener.lookupTransform("/turtle2", now,
                                 "/turtle1", past,
                                 "/world", transform);
    }
    catch (tf::TransformException ex){
      ROS_ERROR("%s",ex.what());
      ros::Duration(1.0).sleep();
    }

    geometry_msgs::Twist vel_msg;
    vel_msg.angular.z = 4.0 * atan2(transform.getOrigin().y(),
								transform.getOrigin().x());
    vel_msg.linear.x = 0.5 * sqrt(pow(transform.getOrigin().x(), 2) +
                                pow(transform.getOrigin().y(), 2));
    turtle_vel.publish(vel_msg);

    rate.sleep();
  }
  return 0;
};
