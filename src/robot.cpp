#include "main.h"
#include <math.h>

/*
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
*/

class WheelMotorSystem: public LinearMotorSystem {
private:
    pros::Motor *motor;
    double diameter;

public:
    void move_velocity(double velocity){
        this->motor->move_velocity(velocity);
    }

    void move_distance(double distance){
        this->motor->move_relative(distance / (diameter * M_PI), 100);
    }

    WheelMotorSystem(pros::Motor *motor, double diameter) {
        this->motor = motor;
        this->diameter = diameter;
    }
};

class RollerMotorSystem: public LinearMotorSystem {
private:
    pros::Motor *left_motor, *right_motor;

public:
    void move_velocity(double velocity){
        this->left_motor->move_velocity(velocity);
        this->right_motor->move_velocity(velocity);
    }

    void move_distance(double distance){
        // TODO: Implement this with the diameter being the distance between
        // sides of the roller belt.
    }

    RollerMotorSystem(pros::Motor *left_motor, pros::Motor *right_motor) {
        this->left_motor = left_motor;
        this->right_motor = right_motor;
    }
};

class TrayMotorSystem: public AngularMotorSystem {
private:
    pros::Motor *motor;

public:
    void move_velocity(double velocity){
        this->motor->move_velocity(velocity);
    }

    void move_angle(double angle){
        this->motor->move_relative(angle, 50);
    }

    TrayMotorSystem(pros::Motor *motor){
        this->motor = motor;
    }
};

class ArmMotorSystem: public AngularMotorSystem {
private:
    pros::Motor *motor;

public:
    void move_velocity(double velocity){
        this->motor->move_velocity(velocity);
    };

    void move_angle(double angle){
        this->motor->move_relative(angle, 100);
    }

    ArmMotorSystem(pros::Motor *motor){
        this->motor = motor;
    }
};

RobotDeviceInterfaces::RobotDeviceInterfaces() {
    this->left_drive_motor = new pros::Motor(11, MOTOR_GEARSET_18, false, MOTOR_ENCODER_ROTATIONS);
    this->right_drive_motor = new pros::Motor(20, MOTOR_GEARSET_18, true, MOTOR_ENCODER_ROTATIONS);
    this->arm_motor = new pros::Motor(1, MOTOR_GEARSET_36, false, MOTOR_ENCODER_ROTATIONS);
    this->tray_motor = new pros::Motor(19, MOTOR_GEARSET_36, true, MOTOR_ENCODER_ROTATIONS);
    this->left_roller_motor = new pros::Motor(2, MOTOR_GEARSET_18, true, MOTOR_ENCODER_ROTATIONS);
    this->right_roller_motor = new pros::Motor(9, MOTOR_GEARSET_18, false, MOTOR_ENCODER_ROTATIONS);

    this->left_drive = new WheelMotorSystem(this->left_drive_motor, 3.25);
    this->right_drive = new WheelMotorSystem(this->right_drive_motor, 3.25);
    this->roller = new RollerMotorSystem(this->left_roller_motor, this->right_roller_motor);
    this->tray = new TrayMotorSystem(this->tray_motor);
    this->arm = new ArmMotorSystem(this->arm_motor);
}
