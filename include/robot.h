#ifndef _ROBOT_HPP_
#define _ROBOT_HPP_

#include "api.h"

#ifdef __cplusplus
extern "C" {
#endif

class MotorSystem {
public:
	virtual void move_velocity(double velocity){};
};

class AngularMotorSystem: public MotorSystem {
public:
	virtual void move_angle(double angle){};
};

class LinearMotorSystem: public MotorSystem {
public:
	virtual void move_distance(double distance){};
};


// In future, add an AbsoluteAngularMotorSystem interface with a move_to_angle
// function and an AbsoluteLinearMotorSystem interface with a move_to_distance
// function. This is a more advanced extension of MotorSystems that isn't a top
// priority.


class RobotDeviceInterfaces {
private:
	pros::Motor *left_drive_motor;
	pros::Motor *right_drive_motor;
	pros::Motor *arm_motor;
	pros::Motor *tray_motor;
	pros::Motor *left_roller_motor;
	pros::Motor *right_roller_motor;

public:
	LinearMotorSystem *left_drive, *right_drive;
	AngularMotorSystem *tray;
	AngularMotorSystem *arm;
	LinearMotorSystem *roller;

	pros::Controller *controller;

    RobotDeviceInterfaces();
};

#ifdef __cplusplus
}
#endif

#endif // _ROBOT_HPP_
