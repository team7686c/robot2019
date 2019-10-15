#ifndef _ROBOT_HPP_
#define _ROBOT_HPP_

#include "api.h"

#ifdef __cplusplus
extern "C" {
#endif

class RobotDeviceInterfaces {
public:
	pros::Motor *left_drive_motor;
	pros::Motor *right_drive_motor;
	pros::Motor *arm_motor;
	pros::Motor *tray_motor;
	pros::Motor *left_roller_motor;
	pros::Motor *right_roller_motor;

	pros::Controller *controller;

    RobotDeviceInterfaces();
};

#ifdef __cplusplus
}
#endif

#endif // _ROBOT_HPP_
