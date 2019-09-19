#include "main.h"
#include <vector>


/*
Program ideas:
- Have a match clock on the controller screen
- Have a vibration alarm for 30 seconds and 5 seconds
- Use a class to control robot state
- make a better feedback loop system
*/
//
// class Robot;
//
// class DrivetrainController;
// class FeedbackController;
//
//
// class FeedbackController {
// public:
// 	virtual void measure(Robot* robot);
// 	virtual void act(Robot* robot);
// };
//
//
// // use "pros::Motor Robot::*output" as a parameter for a generic motor controller
//
// class DrivetrainController: public FeedbackController {
// 	int left_speed;
// 	int right_speed;
//
// 	// pros::Motor& left_motor;
// 	// pros::Motor& right_motor;
// 	// pros::Controller& controller;
//
// public:
//
// 	DrivetrainController(){
// 		this->left_speed = 0;
// 		this->right_speed = 0;
// 	}
//
// 	void measure(Robot* robot){
// 		this->left_speed = robot->controller->get_analog(pros::controller_analog_e_t::E_CONTROLLER_ANALOG_LEFT_Y);
// 		this->right_speed = robot->controller->get_analog(pros::controller_analog_e_t::E_CONTROLLER_ANALOG_RIGHT_Y);
//
// 		std::cout << "Speed: " << this->right_speed << std::endl;
// 	}
//
// 	void act(Robot* robot){
// 		robot->left_drive->move_velocity(this->left_speed);
// 		robot->right_drive->move_velocity(this->right_speed);
// 	}
// };




/**
 * Runs the operator control code. This function will be started in its own task
 * with the default priority and stack size whenever the robot is enabled via
 * the Field Management System or the VEX Competition Switch in the operator
 * control mode.
 *
 * If no competition control is connected, this function will run immediately
 * following initialize().
 *
 * If the robot is disabled or communications is lost, the
 * operator control task will be stopped. Re-enabling the robot will restart the
 * task, not resume it from where it left off.
 */


void opcontrol(){
	std::cout << "Driver control\n";

	std::cout << std::to_string((int)globalState::robotDeviceInterfaces);

	pros::delay(500);

	auto robot = globalState::robotDeviceInterfaces;

	std::cout << "Post init\n";

	int left_drive_speed;
	int right_drive_speed;
	// int arm_speed;
	int roller_speed;

	while (true) {

		// Measure phase
		// robot->measure();
		std::cout << "Start measure phase\n";
		left_drive_speed = robot->controller->get_analog(ANALOG_LEFT_Y) * 200 / 128;
		right_drive_speed = robot->controller->get_analog(ANALOG_RIGHT_Y) * 200 / 128;
		// arm_speed = (robot->controller->get_digital(DIGITAL_L1) - robot->controller->get_digital(DIGITAL_L2)) * 50;
		roller_speed = (robot->controller->get_digital(DIGITAL_R2) - robot->controller->get_digital(DIGITAL_R1)) * 100;

		// Act phase
		// robot->act();
		std::cout << "Start act phase\n";
		robot->left_drive_motor->move_velocity(left_drive_speed);
		robot->right_drive_motor->move_velocity(right_drive_speed);
		// arm.move_velocity(arm_speed);
		robot->left_roller_motor->move_velocity(roller_speed);
		robot->right_roller_motor->move_velocity(roller_speed);

		// Wait for next cycle to save power
		pros::delay(1000 / 50);

		std::cout << "Finish loop\n";
	}
}
