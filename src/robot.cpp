#include "main.h"
#include <math.h>

void block_to_position(pros::Motor *motor, double target_position){
    const double target_size = 0.05;

    while(!( (motor->get_position() < target_position + target_size) && (motor->get_position() > target_position - target_size) )){
        pros::delay(2);
    }
}

class WheelMotorSystem: public LinearMotorSystem {
private:
    pros::Motor *motor;
    double diameter;

public:
    void move_velocity(double velocity){
        this->motor->move_velocity(velocity);
    }

    void move_distance(double distance, bool block){
        double target_distance = distance / (diameter * M_PI);
        this->motor->move_relative(target_distance, 100);

        if(block){
            block_to_position(this->motor, this->motor->get_position() + target_distance);
        }
    }

    WheelMotorSystem(pros::Motor *motor, double diameter) {
        this->motor = motor;
        this->diameter = diameter;
    }
};

class TurnDriveMotorSystem: public AngularMotorSystem {
private:
    LinearMotorSystem *left_drive, *right_drive;
    double inter_wheel_distance; // in inches

public:
    void move_velocity(double velocity){
        this->left_drive->move_velocity(velocity);
        this->right_drive->move_velocity(-velocity);
    }

    void move_angle(double angle, bool block){
        // Angle should be in rotations positive for clockwise, negative for counter-clockwise
        this->left_drive->move_distance(this->inter_wheel_distance * M_PI * angle, false);
        this->right_drive->move_distance(-this->inter_wheel_distance * M_PI * angle, block);
    }

    TurnDriveMotorSystem(LinearMotorSystem *left_drive, LinearMotorSystem *right_drive, double inter_wheel_distance){
        this->left_drive = left_drive;
        this->right_drive = right_drive;
        this->inter_wheel_distance = inter_wheel_distance;
    }
};

class StraightDriveMotorSystem: public LinearMotorSystem {
private:
    LinearMotorSystem *left_drive, *right_drive;

public:
    void move_velocity(double velocity){
        this->left_drive->move_velocity(velocity);
        this->right_drive->move_velocity(velocity);
    }

    void move_distance(double distance, bool block){
        this->left_drive->move_distance(distance, false);
        this->right_drive->move_distance(distance, block);
    }

    StraightDriveMotorSystem(LinearMotorSystem *left_drive, LinearMotorSystem *right_drive){
        this->left_drive = left_drive;
        this->right_drive = right_drive;
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

    void move_distance(double distance, bool block){
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

    void move_angle(double angle, bool block){
        double target_angle = angle;
        this->motor->move_relative(target_angle, 50);

        if(block) {
            block_to_position(this->motor, this->motor->get_position() + target_angle);
        }
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
    }

    void move_angle(double angle, bool block){
        double target_angle = angle * 7;

        this->motor->move_relative(target_angle, 50);

        if(block) {
            block_to_position(this->motor, this->motor->get_position() + target_angle);
        }
    }

    ArmMotorSystem(pros::Motor *motor){
        this->motor = motor;
    }
};

RobotDeviceInterfaces::RobotDeviceInterfaces() {
    this->left_drive_motor = new pros::Motor(11, MOTOR_GEARSET_18, false, MOTOR_ENCODER_ROTATIONS);
    this->left_drive_motor->set_brake_mode(MOTOR_BRAKE_BRAKE);
    this->right_drive_motor = new pros::Motor(20, MOTOR_GEARSET_18, true, MOTOR_ENCODER_ROTATIONS);
    this->right_drive_motor->set_brake_mode(MOTOR_BRAKE_BRAKE);

    this->arm_motor = new pros::Motor(1, MOTOR_GEARSET_36, false, MOTOR_ENCODER_ROTATIONS);
    this->arm_motor->set_brake_mode(MOTOR_BRAKE_HOLD);
    this->tray_motor = new pros::Motor(19, MOTOR_GEARSET_36, true, MOTOR_ENCODER_ROTATIONS);
    this->tray_motor->set_brake_mode(MOTOR_BRAKE_HOLD);
    this->left_roller_motor = new pros::Motor(2, MOTOR_GEARSET_18, true, MOTOR_ENCODER_ROTATIONS);
    this->right_roller_motor = new pros::Motor(9, MOTOR_GEARSET_18, false, MOTOR_ENCODER_ROTATIONS);

    this->left_drive = new WheelMotorSystem(this->left_drive_motor, 3.25);
    this->right_drive = new WheelMotorSystem(this->right_drive_motor, 3.25);
    this->straight_drive = new StraightDriveMotorSystem(this->left_drive, this->right_drive);
    this->turn_drive = new TurnDriveMotorSystem(this->left_drive, this->right_drive, 10.125);

    this->roller = new RollerMotorSystem(this->left_roller_motor, this->right_roller_motor);
    this->tray = new TrayMotorSystem(this->tray_motor);
    this->arm = new ArmMotorSystem(this->arm_motor);
}
