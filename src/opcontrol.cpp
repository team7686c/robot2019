#include "main.h"
#include <vector>

/*
Program ideas:
- Have a match clock on the controller screen
- Have a vibration alarm for 30 seconds and 5 seconds
- Use a class to control robot state
- make a better feedback loop system
*/

class Robot;

class DrivetrainController;
class FeedbackController;


class FeedbackController {
public:
	virtual void measure(Robot* robot);
	virtual void act(Robot* robot);
};

class Robot {
private:
	std::vector<FeedbackController*> feedbackControllers;

public:
	pros::Controller* controller;

	pros::Motor* left_drive;
	pros::Motor* right_drive;
	pros::Motor* arm;

	Robot(std::vector<FeedbackController*> feedbackControllers){
		this->controller = new pros::Controller(pros::E_CONTROLLER_MASTER);

		this->left_drive = new pros::Motor(11, pros::E_MOTOR_GEARSET_18, false, pros::E_MOTOR_ENCODER_DEGREES);
		this->left_drive = new pros::Motor(20, pros::E_MOTOR_GEARSET_18, false, pros::E_MOTOR_ENCODER_DEGREES);
		this->arm = new pros::Motor(1, pros::E_MOTOR_GEARSET_18, false, pros::E_MOTOR_ENCODER_DEGREES);

		this->feedbackControllers = feedbackControllers;
	}

	void measure() {
		std::cout << "Robot measure\n";

		for(int i = 0; i< this->feedbackControllers.size(); i++){
			this->feedbackControllers[i]->measure(this);
		}
	}

	void act() {
		std::cout << "Robot act\n";

		for(int i = 0; i < this->feedbackControllers.size(); i++){
			this->feedbackControllers[i]->act(this);
		}
	}
};

// use "pros::Motor Robot::*output" as a parameter for a generic motor controller

class DrivetrainController: public FeedbackController {
	int left_speed;
	int right_speed;

	// pros::Motor& left_motor;
	// pros::Motor& right_motor;
	// pros::Controller& controller;

public:

	DrivetrainController(){
		this->left_speed = 0;
		this->right_speed = 0;
	}

	void measure(Robot* robot){
		this->left_speed = robot->controller->get_analog(pros::controller_analog_e_t::E_CONTROLLER_ANALOG_LEFT_Y);
		this->right_speed = robot->controller->get_analog(pros::controller_analog_e_t::E_CONTROLLER_ANALOG_RIGHT_Y);

		std::cout << "Speed: " << this->right_speed << std::endl;
	}

	void act(Robot* robot){
		robot->left_drive->move_velocity(this->left_speed);
		robot->right_drive->move_velocity(this->right_speed);
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

const float polling_rate = 50.0; // in Hz

void opcontrol(){
	std::cout << "Pre init\n";

	pros::Controller controller (pros::E_CONTROLLER_MASTER);

	pros::Motor left_drive (11, pros::E_MOTOR_GEARSET_18, false, pros::E_MOTOR_ENCODER_DEGREES);
	pros::Motor right_drive (20, pros::E_MOTOR_GEARSET_18, true, pros::E_MOTOR_ENCODER_DEGREES);

	pros::Motor arm (1, pros::E_MOTOR_GEARSET_18, true, pros::E_MOTOR_ENCODER_DEGREES);

	pros::Motor roller_left (2, pros::E_MOTOR_GEARSET_18, true, pros::E_MOTOR_ENCODER_DEGREES);
	pros::Motor roller_right (9, pros::E_MOTOR_GEARSET_18, false, pros::E_MOTOR_ENCODER_DEGREES);


	// auto robot = new Robot({ /* new DrivetrainController() */ });

	std::cout << "Post init\n";

	int left_drive_speed;
	int right_drive_speed;
	int arm_speed;
	int roller_speed;

	while (true) {

		// Measure phase
		// robot->measure();
		std::cout << "Start measure phase\n";
		left_drive_speed = controller.get_analog(ANALOG_LEFT_Y) * 200 / 128;
		right_drive_speed = controller.get_analog(ANALOG_RIGHT_Y) * 200 / 128;
		arm_speed = (controller.get_digital(DIGITAL_L1) - controller.get_digital(DIGITAL_L2)) * 100;
		roller_speed = (controller.get_digital(DIGITAL_R2) - controller.get_digital(DIGITAL_R1)) * 100;

		// Act phase
		// robot->act();
		std::cout << "Start act phase\n";
		left_drive.move_velocity(left_drive_speed);
		right_drive.move_velocity(right_drive_speed);
		arm.move_velocity(arm_speed);
		roller_left.move_velocity(roller_speed);
		roller_right.move_velocity(roller_speed);

		// Wait for next cycle to save power
		pros::delay(1000 / 50);

		std::cout << "Finish loop\n";
	}
}
