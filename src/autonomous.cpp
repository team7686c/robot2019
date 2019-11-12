#include "main.h"

/**
 * Runs the user autonomous code. This function will be started in its own task
 * with the default priority and stack size whenever the robot is enabled via
 * the Field Management System or the VEX Competition Switch in the autonomous
 * mode. Alternatively, this function may be called in initialize or opcontrol
 * for non-competition testing purposes.
 *
 * If the robot is disabled or communications is lost, the autonomous task
 * will be stopped. Re-enabling the robot will restart the task, not re-start it
 * from where it left off.
 */

void autonomous() {
    std::cout << "Autonomous start\n";

    RobotDeviceInterfaces *robot = global_robot;

    // Start the rollers running
    robot->roller->move_velocity(100);

    // Move the arm up
    robot->arm->move_angle(0.2)->block();

    // keep the rollers running for one second
    pros::delay(1000);
    robot->roller->move_velocity(0);

    // Move the arm back down
    robot->arm->move_angle(-0.05)->block();

    // Start the rollers
    robot->roller->move_velocity(100);

    // Drive forward then backward to push the cube into the goal zone
    robot->straight_drive->move_distance(12)->block();
    robot->straight_drive->move_distance(-12)->block();

    // Stop the rollers
    robot->roller->move_velocity(0);

    std::cout << "Autonomous finish\n";
}
