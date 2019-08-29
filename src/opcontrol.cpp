#include "main.h"

using namespace pros;

/*
Program ideas:
- Have a match clock on the controller screen
- Have a vibration alarm for 30 seconds and 5 seconds
*/

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

void opcontrol() {
	Controller master(E_CONTROLLER_MASTER);
	Motor left_mtr(11);
	Motor right_mtr(20);

	while (true) {
		int left = master.get_analog(ANALOG_LEFT_Y);
		int right = master.get_analog(ANALOG_RIGHT_Y);

		left_mtr.move_velocity(left);
		right_mtr.move_velocity(right);

		delay((long)(1000 / polling_rate));
	}
}
