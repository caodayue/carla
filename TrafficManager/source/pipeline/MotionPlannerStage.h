#pragma once

#include <chrono>
#include <vector>

#include "carla/client/Vehicle.h"
#include "carla/rpc/Actor.h"

#include "MessengerAndDataTypes.h"
#include "PIDController.h"
#include "PipelineStage.h"

namespace traffic_manager {

namespace chr = std::chrono;

  using Actor = carla::SharedPtr<carla::client::Actor>;
  using ActorId = carla::rpc::ActorId;

  /// The class is responsible for aggregating information from various stages
  /// like the localization stage, traffic light stage, collision detection stage
  /// and actuation signals from the PID controller and makes decisions on
  /// how to move the vehicle to follow it's trajectory safely.
  class MotionPlannerStage : public PipelineStage {

  private:

    /// Selection key to switch between the output frames.
    bool frame_selector;
    /// Variables to remember messenger states.
    int localization_messenger_state;
    int control_messenger_state;
    int collision_messenger_state;
    int traffic_light_messenger_state;
    /// Pointers to data frames to be shared with the batch control stage
    std::shared_ptr<PlannerToControlFrame> control_frame_a;
    std::shared_ptr<PlannerToControlFrame> control_frame_b;
    /// Pointers to data frames received from various stages.
    std::shared_ptr<LocalizationToPlannerFrame> localization_frame;
    std::shared_ptr<CollisionToPlannerFrame> collision_frame;
    std::shared_ptr<TrafficLightToPlannerFrame> traffic_light_frame;
    /// Pointers to messenger objects connecting to various stages.
    std::shared_ptr<LocalizationToPlannerMessenger> localization_messenger;
    std::shared_ptr<PlannerToControlMessenger> control_messenger;
    std::shared_ptr<CollisionToPlannerMessenger> collision_messenger;
    std::shared_ptr<TrafficLightToPlannerMessenger> traffic_light_messenger;
    /// Array to store states for integral and differential components
    /// of the PID controller.
    std::shared_ptr<std::vector<StateEntry>> pid_state_vector;
    /// Configuration parameters for the PID controller.
    std::vector<float> longitudinal_parameters;
    std::vector<float> highway_longitudinal_parameters;
    std::vector<float> lateral_parameters;
    /// Target velocities.
    float urban_target_velocity;
    float highway_target_velocity;
    /// Controller object.
    PIDController controller;

  public:

    MotionPlannerStage(
        std::shared_ptr<LocalizationToPlannerMessenger> localization_messenger,
        std::shared_ptr<CollisionToPlannerMessenger> collision_messenger,
        std::shared_ptr<TrafficLightToPlannerMessenger> traffic_light_messenger,
        std::shared_ptr<PlannerToControlMessenger> control_messenger,
        uint number_of_vehicles,
        uint pool_size,
        float urban_target_velocity,
        float highway_target_velocity,
        std::vector<float> longitudinal_parameters,
        std::vector<float> highway_longitudinal_parameters,
        std::vector<float> lateral_parameters);
    ~MotionPlannerStage();

    void DataReceiver() override;

    void Action(const uint start_index, const uint end_index) override;

    void DataSender() override;

  };

}
