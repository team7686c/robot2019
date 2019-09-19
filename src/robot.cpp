#include "main.h"

using namespace pros;

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
    this->controller = new Controller(E_CONTROLLER_MASTER);

    this->left_drive_motor = new Motor(11, E_MOTOR_GEARSET_18, false, E_MOTOR_ENCODER_DEGREES);
    this->right_drive_motor = new Motor(20, E_MOTOR_GEARSET_18, true, E_MOTOR_ENCODER_DEGREES);
    this->arm_motor = new Motor(1, E_MOTOR_GEARSET_36, true, E_MOTOR_ENCODER_DEGREES);
    this->left_roller_motor = new Motor(2, E_MOTOR_GEARSET_18, true, E_MOTOR_ENCODER_DEGREES);
    this->right_roller_motor = new Motor(9, E_MOTOR_GEARSET_18, false, E_MOTOR_ENCODER_DEGREES);
}
