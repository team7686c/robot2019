#include "main.h"
#include <vector>
#include <algorithm>
#include <math.h>

void BlockCommand::block() {
    while(!this->check()){
        pros::delay(2);
    }
}

class MotorBlockCommand: public BlockCommand {
private:
    pros::Motor* motor;
    double target_position;

public:
	virtual bool check() override {
        const double TARGET_SIZE = 0.015;

        auto pos = this->motor->get_position();
        return (pos < target_position + TARGET_SIZE) && (pos > target_position - TARGET_SIZE);
    }

	MotorBlockCommand(pros::Motor *motor, double target_position){
        this->motor = motor;
        this->target_position = target_position;
    }
};

class MotorStallBlockCommand: public BlockCommand {
private:
    pros::Motor* motor;
    double last_pos;

public:
    virtual bool check() override {
        const double TARGET_SIZE = 0;

        double next_pos = this->motor->get_position();

        if(next_pos - this->last_pos == 0){
            this->motor->move_velocity(0);
            return true;
        } else {
            this->last_pos = next_pos;
            return false;
        }
    }

    MotorStallBlockCommand(pros::Motor *motor){
        this->motor = motor;
        this->last_pos = 0;
    }
};

class MultiBlockCommand: public BlockCommand {
public:
	BlockCommand *c1, *c2;

	virtual bool check() override {
		return c1->check() && c2->check();
	}

	MultiBlockCommand(BlockCommand* c1, BlockCommand* c2){
		this->c1 = c1;
        this->c2 = c2;
	}
};

class WheelMotorSystem: public LinearMotorSystem {
private:
    pros::Motor *motor;
    double diameter;
    double speed;
    // TODO: Add adjustable drive speed form move_distance

public:

    void move_velocity(double velocity) override {
        this->motor->move_velocity(velocity);
    }

    virtual BlockCommand *move_distance(double distance) override {
        double target_distance = distance / (diameter * M_PI);
        this->motor->move_relative(target_distance, this->speed);

        return new MotorBlockCommand(this->motor, this->motor->get_position() + target_distance);
    }

    virtual void set_speed(double speed) override {
        this->speed = speed;
    }

    WheelMotorSystem(pros::Motor *motor, double diameter, double speed = 100) {
        this->motor = motor;
        this->diameter = diameter;
        this->speed = speed;
    }
};

class TurnDriveMotorSystem: public AngularMotorSystem {
private:
    LinearMotorSystem *left_drive, *right_drive;
    double inter_wheel_distance; // in inches

public:
    void move_velocity(double velocity) override {
        this->left_drive->move_velocity(velocity);
        this->right_drive->move_velocity(-velocity);
    }

    void set_speed(double speed) override {
        this->left_drive->set_speed(speed);
        this->right_drive->set_speed(speed);
    }

    BlockCommand *move_angle(double angle) override {
        // Angle should be in rotations positive for clockwise, negative for counter-clockwise

        return new MultiBlockCommand(
            this->left_drive->move_distance(this->inter_wheel_distance * M_PI * angle),
            this->right_drive->move_distance(-this->inter_wheel_distance * M_PI * angle)
        );
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
    void move_velocity(double velocity) override {
        this->left_drive->move_velocity(velocity);
        this->right_drive->move_velocity(velocity);
    }

    BlockCommand *move_distance(double distance) override {
        return new MultiBlockCommand(
            this->left_drive->move_distance(distance),
            this->right_drive->move_distance(distance)
        );
    }

    virtual void set_speed(double speed) override {
        this->left_drive->set_speed(speed);
        this->right_drive->set_speed(speed);
    }

    StraightDriveMotorSystem(LinearMotorSystem *left_drive, LinearMotorSystem *right_drive){
        this->left_drive = left_drive;
        this->right_drive = right_drive;
    }
};

class RollerMotorSystem: public LinearMotorSystem {
private:
    LinearMotorSystem *left_roller, *right_roller;
    double roller_radius;

public:
    void move_velocity(double velocity) override {
        this->left_roller->move_velocity(velocity);
        this->right_roller->move_velocity(velocity);
    }

    BlockCommand *move_distance(double distance) override {
        return new MultiBlockCommand(
            this->left_roller->move_distance(distance),
            this->right_roller->move_distance(distance)
        );
    }

    virtual void set_speed(double speed) override {
        this->left_roller->set_speed(speed);
        this->right_roller->set_speed(speed);
    }

    RollerMotorSystem(pros::Motor *left_motor, pros::Motor *right_motor, double roller_radius) {
        this->left_roller = new WheelMotorSystem(left_motor, roller_radius * 2);
        this->right_roller = new WheelMotorSystem(right_motor, roller_radius * 2);
        this->roller_radius = roller_radius;
    }
};

class TrayMotorSystem: public AbsoluteAngularMotorSystem {
private:
    pros::Motor *motor;
    double speed;

public:
    virtual void move_velocity(double velocity) override {
        this->motor->move_velocity(velocity);
    }

    virtual void set_speed(double speed) override {
        // Should not excede 50 rpm
        this->speed = speed;
    }

    virtual BlockCommand *move_angle(double angle) override {
        double target_angle = angle * 7;
        this->motor->move_relative(target_angle, this->speed);

        return new MotorBlockCommand(this->motor, this->motor->get_position() + target_angle);
    }

    virtual BlockCommand *move_to_angle(double angle) override {
        double target_angle = angle * 7;
        this->motor->move_absolute(target_angle, this->speed);

        return new MotorBlockCommand(this->motor, target_angle);
    }

    TrayMotorSystem(pros::Motor *motor){
        this->motor = motor;
        this->speed = 50;
    }
};

class ArmMotorSystem: public AngularMotorSystem {
private:
    pros::Motor *left_motor, *right_motor;
    double speed;

public:
    void move_velocity(double velocity) override {
        this->left_motor->move_velocity(velocity);
        this->right_motor->move_velocity(velocity);
    }

    BlockCommand *move_angle(double angle) override {
        double target_angle = angle * 7;

        this->left_motor->move_relative(target_angle, this->speed);
        this->right_motor->move_relative(target_angle, this->speed);

        return new MultiBlockCommand(
            new MotorBlockCommand(this->left_motor, this->left_motor->get_position() + target_angle),
            new MotorBlockCommand(this->right_motor, this->right_motor->get_position() + target_angle)
        );
    }

    virtual void set_speed(double speed) override {
        this->speed = speed;
    }

    ArmMotorSystem(pros::Motor *left_motor, pros::Motor *right_motor){
        this->left_motor = left_motor;
        this->right_motor = right_motor;
        this->speed = 75;
    }
};

class StackSetdownSystem: public LinearMotorSystem {
private:
    LinearMotorSystem *roller, *drive;

public:
    void move_velocity(double velocity) override {
        this->drive->move_velocity(-velocity);
        this->roller->move_velocity(velocity);
    }

    BlockCommand *move_distance(double distance) override {
        return new MultiBlockCommand(
            this->drive->move_distance(-distance),
            this->roller->move_distance(distance)
        );
    }

    virtual void set_speed(double speed) override {
        this->drive->set_speed(speed);
        this->roller->set_speed(speed);
    }

    StackSetdownSystem(LinearMotorSystem* drive, LinearMotorSystem* roller){
        this->drive = drive;
        this->roller = roller;
    }
};

void RobotDeviceInterfaces::activate_brakes() {
    std::cout << "Activated brakes\n";
    this->left_drive_motor->set_brake_mode(MOTOR_BRAKE_BRAKE);
    this->right_drive_motor->set_brake_mode(MOTOR_BRAKE_BRAKE);
    this->left_arm_motor->set_brake_mode(MOTOR_BRAKE_HOLD);
    this->right_arm_motor->set_brake_mode(MOTOR_BRAKE_HOLD);
    this->tray_motor->set_brake_mode(MOTOR_BRAKE_HOLD);
}

void RobotDeviceInterfaces::deactivate_brakes() {
    std::cout << "Deactivated brakes\n";
    this->left_drive_motor->set_brake_mode(MOTOR_BRAKE_COAST);
    this->right_drive_motor->set_brake_mode(MOTOR_BRAKE_COAST);
    this->left_arm_motor->set_brake_mode(MOTOR_BRAKE_COAST);
    this->right_arm_motor->set_brake_mode(MOTOR_BRAKE_COAST);
    this->tray_motor->set_brake_mode(MOTOR_BRAKE_COAST);
}

RobotDeviceInterfaces::RobotDeviceInterfaces() {
    this->left_drive_motor = new pros::Motor(11, MOTOR_GEARSET_18, false, MOTOR_ENCODER_ROTATIONS);
    this->right_drive_motor = new pros::Motor(20, MOTOR_GEARSET_18, true, MOTOR_ENCODER_ROTATIONS);

    this->left_arm_motor = new pros::Motor(1, MOTOR_GEARSET_36, false, MOTOR_ENCODER_ROTATIONS);
    this->right_arm_motor = new pros::Motor(10, MOTOR_GEARSET_36, true, MOTOR_ENCODER_ROTATIONS);
    this->tray_motor = new pros::Motor(19, MOTOR_GEARSET_36, true, MOTOR_ENCODER_ROTATIONS);
    this->left_roller_motor = new pros::Motor(2, MOTOR_GEARSET_36, true, MOTOR_ENCODER_ROTATIONS);
    this->right_roller_motor = new pros::Motor(9, MOTOR_GEARSET_36, false, MOTOR_ENCODER_ROTATIONS);

    this->left_drive = new WheelMotorSystem(this->left_drive_motor, 3.25);
    this->right_drive = new WheelMotorSystem(this->right_drive_motor, 3.25);
    this->straight_drive = new StraightDriveMotorSystem(this->left_drive, this->right_drive);
    this->turn_drive = new TurnDriveMotorSystem(this->left_drive, this->right_drive, 10.125);

    this->roller = new RollerMotorSystem(this->left_roller_motor, this->right_roller_motor, 1.875);
    this->tray = new TrayMotorSystem(this->tray_motor);
    this->arm = new ArmMotorSystem(this->left_arm_motor, this->right_arm_motor);
    this->stack_setdown = new StackSetdownSystem(this->straight_drive, this->roller);
}

void unfold(RobotDeviceInterfaces *robot){
    // Move the tray forward
    robot->tray->move_angle(0.25)->block();

    // Start the roller
    robot->roller->move_velocity(100);
    pros::delay(1000);

    // Move the tray back
    robot->tray->move_angle(-0.25);

    // Stop the rollers
    robot->roller->move_velocity(0);

    // Finished unfolding
}
