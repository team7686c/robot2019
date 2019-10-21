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

    // Move the arm up
    robot->arm_motor->move_velocity(50);
    pros::delay(1500);
    robot->arm_motor->move_velocity(0);

    // Run the rollers for one second
    robot->left_roller_motor->move_velocity(100);
    robot->right_roller_motor->move_velocity(100);
    pros::delay(1000);
    robot->left_roller_motor->move_velocity(0);
    robot->right_roller_motor->move_velocity(0);

    // Move the arm back down
    robot->arm_motor->move_velocity(-50);
    pros::delay(500);
    robot->arm_motor->move_velocity(0);

    // Start the rollers
    robot->left_roller_motor->move_velocity(100);
    robot->right_roller_motor->move_velocity(100);

    // Drive forward and backward, twice, to ensure that the rollers are down
    for(int i = 0; i < 1; i++){
        robot->left_drive_motor->move_velocity(100);
        robot->right_drive_motor->move_velocity(100);
        pros::delay(1000);
        robot->left_drive_motor->move_velocity(-100);
        robot->right_drive_motor->move_velocity(-100);
        pros::delay(1000);
    }

    // Stop the drive
    robot->left_drive_motor->move_velocity(0);
    robot->right_drive_motor->move_velocity(0);

    robot->left_roller_motor->move_velocity(0);
    robot->right_roller_motor->move_velocity(0);

    std::cout << "Autonomous finish\n";
}
