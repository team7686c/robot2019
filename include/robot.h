#ifndef _ROBOT_HPP_
#define _ROBOT_HPP_

#include "api.h"

#ifdef __cplusplus
extern "C" {
#endif

class BlockCommand {
public:
	virtual bool check() = 0;
	void block();
};

class MotorBlockCommand;
class MultiBlockCommand;

class MotorSystem {
public:
	// Velocity in RPM
	virtual void move_velocity(double velocity) = 0;
};

class AngularMotorSystem: public MotorSystem {
public:
	// Angle should be in rotations
	virtual BlockCommand *move_angle(double angle) = 0;
	virtual void set_speed(double speed) = 0;
};

class LinearMotorSystem: public MotorSystem {
public:
	// Distance should be in inches
	virtual BlockCommand *move_distance(double distance) = 0;
	virtual void set_speed(double speed) = 0;
};

// In future, add an AbsoluteAngularMotorSystem interface with a move_to_angle
// function and an AbsoluteLinearMotorSystem interface with a move_to_distance
// function. This is a more advanced extension of MotorSystems that isn't a top
// priority.

class AbsoluteAngularMotorSystem: public AngularMotorSystem {
public:
	virtual BlockCommand *move_to_angle(double angle) = 0;
};

// Implementation classes:
class WheelMotorSystem;
class TurnDriveMotorSystem;
class StraightDriveMotorSystem;
class RollerMotorSystem;
class TrayMotorSystem;

class RobotDeviceInterfaces {
private:
	pros::Motor *left_drive_motor, *right_drive_motor;
	pros::Motor *left_arm_motor, *right_arm_motor;
	pros::Motor *tray_motor;
	pros::Motor *left_roller_motor, *right_roller_motor;

public:
	LinearMotorSystem *left_drive, *right_drive;
	LinearMotorSystem *straight_drive;
	AngularMotorSystem *turn_drive;
	AbsoluteAngularMotorSystem *tray;
	AngularMotorSystem *arm;
	LinearMotorSystem *roller;
	LinearMotorSystem *stack_setdown;

	pros::Controller *controller;

    RobotDeviceInterfaces();

	void activate_brakes();
	void deactivate_brakes();
};

void unfold(RobotDeviceInterfaces*);

#ifdef __cplusplus
}
#endif

#endif // _ROBOT_HPP_
