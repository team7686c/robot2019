#include "main.h"

// class RobotDeviceInterfaces {
// public:
// 	Controller *controller;
//
// 	Motor *left_drive_motor;
// 	Motor *right_drive_motor;
// 	Motor *arm_motor;
// 	Motor *left_roller_motor;
// 	Motor *right_roller_motor;
// };

RobotDeviceInterfaces::RobotDeviceInterfaces() {
    this->left_drive_motor = new pros::Motor(11, MOTOR_GEARSET_18, false, MOTOR_ENCODER_DEGREES);
    this->right_drive_motor = new pros::Motor(20, MOTOR_GEARSET_18, true, MOTOR_ENCODER_DEGREES);
    this->arm_motor = new pros::Motor(1, MOTOR_GEARSET_36, true, MOTOR_ENCODER_DEGREES);
    this->left_roller_motor = new pros::Motor(2, MOTOR_GEARSET_18, true, MOTOR_ENCODER_DEGREES);
    this->right_roller_motor = new pros::Motor(9, MOTOR_GEARSET_18, false, MOTOR_ENCODER_DEGREES);

    this->controller = new pros::Controller(CONTROLLER_MASTER);
}
