#ifndef _ROBOT_HPP_
#define _ROBOT_HPP_

#include "api.h"

#ifdef __cplusplus
extern "C" {
#endif

class BlockCommand {
public:
	pros::Motor* motor;
	double target_position;

	virtual bool check();
	BlockCommand(pros::Motor*, double);
	void block();
};

class MultiBlockCommand;


class MotorSystem {
public:
	virtual void move_velocity(double velocity){
		std::cout << "Motor System virtual\n";
	}
};

class AngularMotorSystem: public MotorSystem {
public:
	virtual BlockCommand *move_angle(double angle){
		// Angle should be in rotations, positive for clockwise, negative for counter-clockwise.
		std::cout << "Angular Motor System virtual\n";
		return NULL;
	}
};

class LinearMotorSystem: public MotorSystem {
public:
	virtual BlockCommand *move_distance(double distance){
		// Distance should be in inches.
		std::cout << "Linear Motor System virtual\n";
		return NULL;
	}
};

// In future, add an AbsoluteAngularMotorSystem interface with a move_to_angle
// function and an AbsoluteLinearMotorSystem interface with a move_to_distance
// function. This is a more advanced extension of MotorSystems that isn't a top
// priority.


// Implementation classes:
class WheelMotorSystem;
class TurnDriveMotorSystem;
class StraightDriveMotorSystem;
class RollerMotorSystem;
class TrayMotorSystem;

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
	LinearMotorSystem *straight_drive;
	AngularMotorSystem *turn_drive;
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
