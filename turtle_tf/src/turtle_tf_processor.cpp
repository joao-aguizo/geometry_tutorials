#include <ros/ros.h>
#include <tf/transform_listener.h>
#include <tf/transform_broadcaster.h>
#include <geometry_msgs/Twist.h>
#include <geometry_msgs/TransformStamped.h>
#include <turtlesim/Spawn.h>

int main(int argc, char** argv){
  ros::init(argc, argv, "my_tf_processor");

  ros::NodeHandle node;

  // ---- Turtle spawn ----
  ros::service::waitForService("spawn");
  ros::ServiceClient add_turtle =
    node.serviceClient<turtlesim::Spawn>("spawn");
  turtlesim::Spawn srv;
  add_turtle.call(srv);

  // ---- Process the static TF ----

  // publisher for debug
  ros::Publisher pub_tfs = node.advertise<geometry_msgs::TransformStamped>(
    "out_tfs", 1, true);

  tf::StampedTransform transform;
  tf::TransformListener listener;
  static tf::TransformBroadcaster broadcaster;

  // wait and lookup TF
  try{
    // long timeout is set on purpose
    listener.waitForTransform("/turtle2", "/turtle1", 
                              ros::Time(0), ros::Duration(10000.0));
    listener.lookupTransform("/turtle2", "/turtle1",
                              ros::Time(0), transform);
  }
  catch (tf::TransformException &ex) {
    ROS_ERROR("%s",ex.what());
  }

  /*
  ###########################################################################
  SHOULD NOT GET PAST THIS POINT BEFORE THE TIMEOUT WITHOUT A VALID TRANSFORM 
  ###########################################################################
  */

  // convert to msg and publish for inspection
  geometry_msgs::TransformStamped msg;
  tf::transformStampedTFToMsg(transform, msg);
  pub_tfs.publish(msg);

  // dummy processing of TF
  ros::Rate rate(10.0);
  while (node.ok()){
    tf::StampedTransform tf = tf::StampedTransform(transform, ros::Time().now(),
                                                   "/world", "/test");

    // broadcaster.sendTransform(tf);
    rate.sleep();
  }
  return 0;
};
