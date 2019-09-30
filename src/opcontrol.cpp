#include "main.h"
#include <vector>

/*
Program ideas:
- Have a match clock on the controller screen
- Have a vibration alarm for 30 seconds and 5 seconds
*/

class FeedbackController {
public:
	virtual void measure(pros::Controller *controller){};
	virtual void act(RobotDeviceInterfaces *robot){};
};

class DrivetrainController: public FeedbackController {
public:
	int left_drive_speed;
	int right_drive_speed;

	void measure(pros::Controller *controller){
		this->left_drive_speed = controller->get_analog(ANALOG_LEFT_Y) * 200 / 128;
		this->right_drive_speed = controller->get_analog(ANALOG_RIGHT_Y) * 200 / 128;
	}

	void act(RobotDeviceInterfaces *robot){
		robot->left_drive_motor->move_velocity(this->left_drive_speed);
		robot->right_drive_motor->move_velocity(this->right_drive_speed);
	}
};

class RollerController: public FeedbackController {
public:
	int roller_speed;

	void measure(pros::Controller *controller){
		this->roller_speed = (controller->get_digital(DIGITAL_R2) - controller->get_digital(DIGITAL_R1)) * 100;
	}

	void act(RobotDeviceInterfaces* robot){
		robot->left_roller_motor->move_velocity(this->roller_speed);
		robot->right_roller_motor->move_velocity(this->roller_speed);
	}
};

class ArmController: public FeedbackController {
public:
	int arm_speed;

	void measure(pros::Controller *controller){
		this->arm_speed = (controller->get_digital(DIGITAL_L1) - controller->get_digital(DIGITAL_L2)) * 50;
	}

	void act(RobotDeviceInterfaces *robot){
		robot->arm_motor->move_velocity(this->arm_speed);
	}
};

class LCDController: public FeedbackController {
public:
	int time;
	int count;

	void measure(pros::Controller *controller){
		this->count++;
	}

	void act(RobotDeviceInterfaces *robot){
		if(this->count % 10 == 0){
			std::cout << "running controller feedback\n";
			std::cout << "Clear result: " << std::to_string(robot->controller->clear_line(1)) << "\n";
			std::cout << "Print result: " << std::to_string(robot->controller->set_text(1, 1, "Something")) << "\n";
		}
	}

	LCDController(){
		this->count = 0;
		this->time = pros::millis();
	}
};

class TrayController: public FeedbackController {
public:
	int tray_velocity;

	void measure(pros::Controller *controller){
		this->tray_velocity = (controller->get_digital(DIGITAL_A) - controller->get_digital(DIGITAL_B)) * 50;
	}

	void act(RobotDeviceInterfaces* robot){
		robot->tray_motor->move_velocity(this->tray_velocity);
	}
};

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

	RobotDeviceInterfaces *robot = global_robot;
	pros::Controller *controller = global_controller;

	std::vector<FeedbackController*> feedbackControllers = {
		new DrivetrainController(),
		new RollerController(),
		new ArmController(),
		// new LCDController(),
		new TrayController(),
	};

	while (true) {
		// Measure phase
		std::cout << "Start measure phase\n";
		for(auto feedbackController: feedbackControllers){
			feedbackController->measure(controller);
		}

		// Act phase
		std::cout << "Start act phase\n";
		for(auto feedbackController: feedbackControllers){
			feedbackController->act(robot);
		}

		// Wait for next cycle to save power
		pros::delay(1000 / 30);
	}
}
