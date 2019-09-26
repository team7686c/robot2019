#include "main.h"

// class RobotDeviceInterfaces {
// public:
// 	pros::Motor *left_drive_motor;
// 	pros::Motor *right_drive_motor;
// 	pros::Motor *arm_motor;
// 	pros::Motor *tray_motor;
// 	pros::Motor *left_roller_motor;
// 	pros::Motor *right_roller_motor;
//
// 	pros::Controller *controller;
//
//     RobotDeviceInterfaces();
// };

RobotDeviceInterfaces::RobotDeviceInterfaces() {
    this->left_drive_motor = new pros::Motor(11, MOTOR_GEARSET_18, false, MOTOR_ENCODER_DEGREES);
    this->right_drive_motor = new pros::Motor(20, MOTOR_GEARSET_18, true, MOTOR_ENCODER_DEGREES);
    this->arm_motor = new pros::Motor(1, MOTOR_GEARSET_36, true, MOTOR_ENCODER_DEGREES);
    this->tray_motor = new pros::Motor(19, MOTOR_GEARSET_18, true, MOTOR_ENCODER_DEGREES);
    this->left_roller_motor = new pros::Motor(2, MOTOR_GEARSET_18, true, MOTOR_ENCODER_DEGREES);
    this->right_roller_motor = new pros::Motor(9, MOTOR_GEARSET_18, false, MOTOR_ENCODER_DEGREES);
}
