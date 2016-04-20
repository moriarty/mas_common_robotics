/*
 * Copyright 2016 Bonn-Rhein-Sieg University
 *
 * Author: Santosh Thoduka
 *
 */
#ifndef MCR_SCENE_SEGMENTATION_WORKSPACE_FINDER_NODE_H
#define MCR_SCENE_SEGMENTATION_WORKSPACE_FINDER_NODE_H

#include <ros/ros.h>
#include <mcr_scene_segmentation/aliases.h>
#include <mcr_scene_segmentation/planar_polygon_visualizer.h>

#include <pcl_msgs/ModelCoefficients.h>
#include <geometry_msgs/PolygonStamped.h>
#include <mcr_perception_msgs/PlanarPolygon.h>
#include <std_msgs/String.h>

#include <message_filters/sync_policies/exact_time.h>
#include <message_filters/subscriber.h>

typedef message_filters::sync_policies::ExactTime<geometry_msgs::PolygonStamped,
                                                  pcl_msgs::ModelCoefficients> PlanarPolygonSyncPolicy;

using mcr::visualization::PlanarPolygonVisualizer;
using mcr::visualization::Color;

/**
 * This node subscribes to a PolygonStamped and ModelCoefficients message and publishes a
 * PlanarPolygon message.
 *
 * The PolygonStamped and ModelCoefficients messages are generated by the workspace_finder_manager
 * nodelet and they represent a plane perpendicular to the z-axis of base-link.
 *
 * Events:
 * 1) Input Events: "event_in" topic
 *   a) e_trigger  publishes one PlanarPolygon
 *   b) e_start    continuously publishes PlanarPolygons until e_stop is received
 *   c) e_stop     stops publishing
 * 2) Output Events: "event_out" topic
 *   a) e_done     published if workspace was found
 *
 * Subscribes:
 * 1) "input/polygon"
 *      input PolygonStamped message
 * 2) "input/coefficients"
 *      input coefficients of the plane
 *
 * Publishes:
 * 1) "polygon"
 *      A mcr_perception_msgs::PlanarPolygon message
 * 2) "workspace_polygon"
 *      An RViz marker that visualizes the polygon that defines the detected
 *      plane.
 */
class WorkspaceFinderNode
{
public:
    WorkspaceFinderNode();
    virtual ~WorkspaceFinderNode();

    void update();

private:
    void eventInCallback(const std_msgs::String &msg);
    void synchronizedCallback(const geometry_msgs::PolygonStamped::ConstPtr &polygon_msg,
                              const pcl_msgs::ModelCoefficients::ConstPtr &coefficients_msg);

private:
    ros::NodeHandle nh;

    ros::Subscriber sub_event_in_;

    message_filters::Subscriber<geometry_msgs::PolygonStamped> sub_polygon_;
    message_filters::Subscriber<pcl_msgs::ModelCoefficients> sub_coefficients_;

    ros::Publisher pub_event_out_;
    ros::Publisher pub_polygon_;

    boost::shared_ptr<message_filters::Synchronizer<PlanarPolygonSyncPolicy> > sync_input_;

    bool trigger_workspace_finder_;
    bool run_workspace_finder_;

    bool sync_message_received_;
    /*
     * maximum allowed time difference (in seconds) between synchronized message
     * and current ros::Time
     */
    ros::Duration message_age_threshold_;

    PlanarPolygonVisualizer polygon_visualizer_;

    geometry_msgs::PolygonStamped::ConstPtr polygon_msg_;
    pcl_msgs::ModelCoefficients::ConstPtr coefficients_msg_;
};
#endif  // MCR_SCENE_SEGMENTATION_WORKSPACE_FINDER_NODE_H
