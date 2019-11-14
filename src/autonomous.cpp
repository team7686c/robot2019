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

    // Move the tray forward
    robot->tray->move_angle(0.25)->block();

    // Move the arms up then down
    robot->arm->move_angle(0.1)->block();
    robot->arm->move_angle(-0.1)->block();

    // Run the roller
    robot->roller->move_velocity(100);
    pros::delay(1000);

    // Move the tray back
    robot->tray->move_angle(-0.25)->block();

    robot->roller->move_velocity(0);

    // Finished unfolding


    // Drive forward then backward to push a cube into the goal zone.
    // robot->straight_drive->move_distance(12)->block();
    // robot->straight_drive->move_distance(-12)->block();

    std::cout << "Autonomous finish\n";
}
