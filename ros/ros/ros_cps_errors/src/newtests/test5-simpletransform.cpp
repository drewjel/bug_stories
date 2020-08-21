
#include "ros/ros.h"
#include "geometry_msgs/Vector3Stamped.h"
#include "geometry_msgs/Vector3.h"
#include "geometry_msgs/Transform.h"
#include "geometry_msgs/TransformStamped.h"
#include <tf/transform_datatypes.h>
#include <tf2_geometry_msgs/tf2_geometry_msgs.h>
#include "nav_msgs/MapMetaData.h"
#include "nav_msgs/OccupancyGrid.h"
#include "nav_msgs/GetMap.h"

#include <cmath>

/*
Test 5

Name -  Simple Transformation Example
Description - This is a doctored and somewhat truncated version of our earlier Frame example.
    How do we deal with annotating Frame Values? Right now, I'm treating it like 16 numbers
    The annotations say "North, West,South...etc". Does this make sense? How is that going to get handled?

    How do we treat the result of "TFToMsg" - etc? This isn't implemented yet - but it should be treated as an assignment
    Similar expectation of "doTransform" and like void calls that have side effects
Expected Outcome -
Implementation Gaps - 
  -Layer 1 (Parsers) :
    I have not supported nav_msgs yet. But probably easy
  -Layer 2 (Peirce) :
  -Layer 3 (Lang) :
    No Frames/Transforms
  -Layer 4 (Phys) :
    No Frames/Transforms
  -Layer 5 (CharlieLayer) :
    No Frames/Transforms
*/

int main(int argc, char **argv){
    //Initialize the ROS node and retrieve a handle to it
    ros::init(argc, argv, "relative_frames");   // "annotations" is ROS node name
    ros::NodeHandle node;                   // provides ROS utility functions
    //Allow debug messages to show up in console
    ros::console::set_logger_level(ROSCONSOLE_DEFAULT_NAME, ros::console::levels::Debug);

    //1 : @@EuclideanGeometry worldGeometry("worldGeometry", 3)

   //2 :  @@EuclideanGeometryFrame world_frame_(worldGeometry, <origin=<0,0,0>,orientation=<x="north", y="east", z="up">>, geom3.stdFrame);
   //3 :  @@EuclideanGeometryFrame base_link_frame_(worldGeometry, <origin=<computed>,orientation=<x="north",y="east",z="up">>, world_frame_)

    ros::service::waitForService("/static_map");
    ros::ServiceClient cl = node.serviceClient<nav_msgs::GetMap>("/static_map");

    nav_msgs::GetMap gm;

    cl.call(gm);

    //annotate this?
    nav_msgs::OccupancyGrid world_map = gm.response.map;
    
    //4: EuclideanGeometryOrientation (worldGeometry, Value=<UNK>, worldGeometry.stdFrame)
    geometry_msgs::Pose map_pose = world_map.info.origin;
    
    //5: EuclideanGeometryOrientation (worldGeometry, Value=<Origin=<1, 1, 1>>, worldGeometry.stdFrame)
    geometry_msgs::PoseStamped robot_pose_in_map;
    robot_pose_in_map.header.stamp = ros::Time::now();
    robot_pose_in_map.header.frame_id = world_map.header.frame_id;
    robot_pose_in_map.pose.position.x = 1;
    robot_pose_in_map.pose.position.y = 1;
    robot_pose_in_map.pose.position.z = 1;
    
    //6: @@GeometricOrientation (geom3d, <origin=<computed>,orientation=<computed>>, geom3d.stdFrame)
    tf::Quaternion map_to_robot_rotation;
    map_to_robot_rotation.setRPY(-1.5, 0, 1.5);
    map_to_robot_rotation.normalize();

    //9 : @@GeometricOrientation (geom3d, <origin=<computed>,orientation=<computed>>, geom3d.stdFrame)
    tf::quaternionTFToMsg(map_to_robot_rotation, robot_pose_in_map.pose.orientation);
    
    //10 : @@GeometricFrameTransform (geom3d, Value=<computed>, world_frame_->base_link_frame_)
    tf::StampedTransform tf_map_to_robot_transform(
        tf::Transform(
                tf::Quaternion(
                    robot_pose_in_map.pose.orientation.x,
                    robot_pose_in_map.pose.orientation.y,
                    robot_pose_in_map.pose.orientation.z,
                    robot_pose_in_map.pose.orientation.w
                ),
                tf::Vector3(
                    robot_pose_in_map.pose.position.x,
                    robot_pose_in_map.pose.position.y,
                    robot_pose_in_map.pose.position.z
                )
            ).inverse(),
        robot_pose_in_map.header.stamp,
        robot_pose_in_map.header.frame_id,
        "robot_base_link"
    );


    //11 : @@GeometricFrameTransform (geom3d, <computed>, world_frame_->base_link_frame_)
    geometry_msgs::TransformStamped map_to_robot_transform;

    //12 :  @@GeometricFrameTransform (geom3d, <computed>, world_frame_->base_link_frame_)
    tf::transformStampedTFToMsg(tf_map_to_robot_transform, map_to_robot_transform);

    //13: @@GeometricOrientation (geom3d, <origin=<computed>, orientation=<computed>>, base_link_frame_)
    geometry_msgs::PoseStamped robot_origin_in_robot_frame;

    //14: @@GeometricOrientation (geom3d, <origin=<computed>, orientation=<computed>>, base_link_frame_)
    tf2::doTransform(robot_pose_in_map, robot_origin_in_robot_frame, map_to_robot_transform);
}