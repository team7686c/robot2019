#include "main.h"
#include <vector>

// The controller poll rate determines how long the controller will wait between
// iterations of the control loop.
const int CONTROLLER_POLL_RATE = 1000 / 30;

// This class is an interface for the feedback control loop in the main
// operator control function. The class splits up the measure phase (reading
// controller input) and the act phase (setting motor speeds) so that the robot
// control loop code can be clearly separated. In those classes, the variables,
// measure functions and act functions are clearly defined.
class FeedbackController {
public:
	// The measure function is used to store controller state in a member
	// variable
	virtual void measure(pros::Controller *controller) = 0;

	// The act function is used to change the state of the motors based on the
	// member variable
	virtual void act(RobotDeviceInterfaces *robot) = 0;
};

float cubic_control(float input){
	return input * input * input;
}

class DrivetrainController: public FeedbackController {
public:
	int drive_speed, turn_speed; // in RPM

	const int BASE_DRIVE_SPEED = 200;
	const int BASE_TURN_SPEED = 200;

	void measure(pros::Controller *controller) override {
		// Analog Joystick input come in an integer in the range -127..127. The
		// top motor speed desired is 200rpm.
		double drive_input = controller->get_analog(ANALOG_LEFT_Y) / 128.0;
		double turn_input = controller->get_analog(ANALOG_LEFT_X) / 128.0;

		this->drive_speed = cubic_control(drive_input) * BASE_DRIVE_SPEED;
		this->turn_speed = cubic_control(turn_input) * BASE_TURN_SPEED;
	}

	void act(RobotDeviceInterfaces *robot) override {
		// left_drive and right_drive are used individually so both controls can
		// be used at the same time.

		robot->left_drive->move_velocity(this->drive_speed + this->turn_speed);
		robot->right_drive->move_velocity(this->drive_speed - this->turn_speed);
	}
};

class RollerController: public FeedbackController {
public:
	int roller_speed; // in RPM

	void measure(pros::Controller *controller) override {
		// When R2 is pressed, the roller will spin forwards, and when R1 is
		// pressed the roller will spin backwards. The speed is set to 100rpm
		this->roller_speed = (controller->get_digital(DIGITAL_R2) - controller->get_digital(DIGITAL_R1)) * 100;
	}

	void act(RobotDeviceInterfaces* robot) override {
		robot->roller->move_velocity(this->roller_speed);
	}
};

class ArmController: public FeedbackController {
public:
	int arm_speed; // in RPM

	void measure(pros::Controller *controller) override {
		this->arm_speed = (controller->get_digital(DIGITAL_L1) - controller->get_digital(DIGITAL_L2)) * 100;
	}

	void act(RobotDeviceInterfaces *robot) override {
		robot->arm->move_velocity(this->arm_speed);
	}
};

class ArmRecenterController: public FeedbackController {
public:
	int command;

	void measure(pros::Controller *controller) override {
		if(controller->get_digital(DIGITAL_X)){
			this->command = 1;
			std::cout << "Recenter commanded\n";
		}
	}

	void act(RobotDeviceInterfaces *robot) override {
		if(this->command == 1){
			robot->arm->recenter();
			this->command = 0;
		}
	}
};

// The LCDController is currently unused because we were unable to make the
// controller API work.
class LCDController: public FeedbackController {
public:
	int time;
	int count;

	void measure(pros::Controller *controller) override {
		this->count++;
	}

	void act(RobotDeviceInterfaces *robot) override {
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

// The TrayController controls the cube intake tray, eventually this controller
// will be replaced by a controller that takes advantage of the motor encoders
// to automatically move to the correct position (upright or back).
class TrayController: public FeedbackController {
public:
	bool command;
	bool flush;
	int tray_velocity;

	void measure(pros::Controller *controller) override {
		bool a = controller->get_digital(DIGITAL_A);
		bool b = controller->get_digital(DIGITAL_B);

		if(a){
			this->tray_velocity = 50;
			this->command = true;
		}

		if(b){
			this->tray_velocity = -50;
			this->command = true;
		}

		if(!a && !b && this->command){
			this->tray_velocity = 0;
			this->flush = true;
		}
	}

	void act(RobotDeviceInterfaces* robot) override {
		if(this->command){
			robot->tray->move_velocity(this->tray_velocity);
			if(this->flush){
				this->command = false;
				this->flush = false;
			}
		}
	}
};

// The AutoBackupController is meant to be used when placing a stack of cubes
// inside the scoring zone. After the tray is tilted forwards, we found that it
// was hard to back away from the stack while making sure it is also properly
// freed from the rollers. We found that this was best done by slowly backing up
// while spinning the rollers at the same rate.
class AutoBackupController: public FeedbackController {
public:
	int button_state;

	void measure(pros::Controller *controller) override {
		this->button_state = (controller->get_digital(DIGITAL_DOWN) - controller->get_digital(DIGITAL_RIGHT));
	}

	void act(RobotDeviceInterfaces* robot) override {
		if(this->button_state == 1){
			robot->stack_setdown->move_velocity(50);
		} else if(this->button_state == -1){
			robot->stack_setdown->move_velocity(-50);
		}
	}
};

class AutoStackController: public FeedbackController {
public:
	int command;

	void measure(pros::Controller *controller) override {
		if(controller->get_digital(DIGITAL_UP)){
			this->command = 1;
		} else if(controller->get_digital(DIGITAL_LEFT)){
			this->command = -1;
		}
	}

	void act(RobotDeviceInterfaces *robot) override {
		if(this->command == -1){
			robot->tray->move_to_angle(0.04);
		} else if(this->command == 1){
			robot->tray->move_to_angle(0.25);
		}

		this->command = 0;
	}
};

class AutoUnfoldController: public FeedbackController {
public:
	int command = 0;

	void measure(pros::Controller *controller) override {
		if(this->command == 0 && controller->get_digital(DIGITAL_Y)){
			this->command = 1;
		}
	}

	void act(RobotDeviceInterfaces *robot) override {
		if(this->command == 1){
			std::uint32_t time_before = pros::millis();
			unfold(robot);
			std::uint32_t time_after = pros::millis();
			std::cout << "Unfold time taken: " << time_after - time_before << "\n";
			this->command = 2;
		}
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

	// Grab the global state pointers
	RobotDeviceInterfaces *robot = global_robot;
	robot->activate_brakes();

	pros::Controller *controller = global_controller;

	// Collect the FeedbackController implementations into a vector for
	// iteration
	std::vector<FeedbackController*> feedbackControllers = {
		new DrivetrainController(),
		new RollerController(),
		new ArmController(),
		new ArmRecenterController(),
		new TrayController(),
		new AutoBackupController(),
		new AutoStackController(),
		new AutoUnfoldController(),
	};

	while (true) {
		std::uint32_t time = pros::millis();

		// Measure phase
		for(auto feedbackController: feedbackControllers){
			feedbackController->measure(controller);
		}

		// Act phase
		for(auto feedbackController: feedbackControllers){
			feedbackController->act(robot);
		}

		// Wait for next cycle to save power
		pros::Task::delay_until(&time, CONTROLLER_POLL_RATE);
	}
}
