#include "main.h"
#include <tuple>
#include <vector>

// https://stackoverflow.com/a/1082938
int eucmod(int x, int m){
    return (x%m + m)%m;
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

void four_point_autonomous(RobotDeviceInterfaces *robot, bool is_reversed){
    unfold(robot);

    robot->roller->move_velocity(-100);

    double d = 36; // Distance to drive forward to pick up first stack

    // Drive forward at 55RPM
    robot->straight_drive->set_speed(50);
    robot->straight_drive->move_distance(d)->block();

    // Drive back at 80RPM and keep the block command for later
    robot->straight_drive->set_speed(100);
    auto drive_back = robot->straight_drive->move_distance(-d + 15);

    // Wait half a bit then stop the rollers.
    pros::delay(100);
    robot->roller->move_velocity(0);

    robot->roller->set_speed(50);
    if(is_reversed){
        robot->roller->move_distance(4)->block();
        robot->roller->move_distance(-4)->block();
    } else {
        robot->roller->move_distance(3)->block();
        robot->roller->move_distance(-3)->block();
    }

    // Wait unitl the drive backward is done.
    drive_back->block();

    if(is_reversed){
        robot->turn_drive->move_angle(-0.38)->block();
        robot->straight_drive->move_distance(7.25)->block();
    } else {
        robot->turn_drive->move_angle(0.385)->block();
        robot->straight_drive->move_distance(6.75)->block();
    }


    robot->roller->set_speed(50);
    robot->roller->move_distance(5.5)->block();
    robot->roller->move_distance(-3.0)->block();
    pros::delay(250);

    robot->tray->move_angle(0.23)->block();

    robot->roller->move_distance(0.5)->block();
    pros::delay(250);

    robot->stack_setdown->set_speed(50);
    robot->stack_setdown->move_distance(8)->block();
    robot->stack_setdown->set_speed(100);

    // TODO: Put the tray back into the neutral position
}

void setdown(RobotDeviceInterfaces *robot){
    robot->roller->set_speed(50);
    robot->roller->move_distance(5.5)->block();
    robot->roller->move_distance(-3.0)->block();
    pros::delay(250);

    robot->tray->move_angle(0.23)->block();

    robot->roller->move_distance(0.5)->block();
    pros::delay(250);

    robot->stack_setdown->set_speed(50);
    robot->stack_setdown->move_distance(8)->block();
    robot->stack_setdown->set_speed(100);
}

const int default_autonomous_selection = 4;

int autonomous_selection;
std::vector<std::tuple<std::string, void (*)(RobotDeviceInterfaces*)>> autonomous_programs = {
    {"None", [](RobotDeviceInterfaces* robot){}},
    {"1 point autonomous", [](RobotDeviceInterfaces *robot){
        // Drive forward then backward to push a cube into the goal zone.
        robot->straight_drive->move_distance(12)->block();
        robot->straight_drive->move_distance(-12)->block();

        unfold(robot);
    }},
    {"red 4 point autonomous", [](RobotDeviceInterfaces *robot){
        four_point_autonomous(robot, false);
    }},
    {"blue 4 point autonomous", [](RobotDeviceInterfaces *robot){
        four_point_autonomous(robot, true);
    }},
    {"big auto", [](RobotDeviceInterfaces *robot){
        // Drive forward then backward to push a cube into the goal zone.
        unfold(robot);

        robot->straight_drive->move_distance(25)->block();

        robot->turn_drive->move_angle(0.09)->block();

        robot->roller->move_velocity(-100);
        robot->straight_drive->set_speed(200);
        robot->straight_drive->move_distance(15)->block();
        robot->roller->move_velocity(0);

        robot->straight_drive->set_speed(100);

        robot->turn_drive->move_angle(-0.475)->block();

        robot->roller->move_velocity(-100);
        robot->straight_drive->move_distance(30)->block();
        robot->roller->move_velocity(0);

        // Setdown Proc
        setdown(robot);
    }},
    {"small auto", [](RobotDeviceInterfaces *robot){
        unfold(robot);

        robot->roller->move_velocity(-100);
        robot->straight_drive->move_distance(12)->block();
        robot->roller->move_velocity(0);

        robot->turn_drive->move_angle(0.2);

        robot->roller->move_velocity(-100);
        robot->straight_drive->move_distance(12);
        robot->roller->move_velocity(0);


        // setdown(robot);


    }},
    {"Unfold", [](RobotDeviceInterfaces *robot){
        unfold(robot);
    }}
};

void draw_unselect(int i){
    pros::lcd::print(i + 1, "  %s", std::get<0>(autonomous_programs[i]));
}

void draw_select(int i){
    pros::lcd::print(i + 1, "> %s", std::get<0>(autonomous_programs[i]));
}

void competition_initialize() {
	std::cout << "Competition initialize\n";

	pros::lcd::initialize();
	pros::lcd::print(0, "Select autonomous:");

    // Don't ask me why the extra text is required. It works so don't change it.
    pros::lcd::print(7, "   Up                                       Down   kjascdjknsa");

    for(int i = 0; i < autonomous_programs.size(); i++){
        draw_unselect(i);
    }
    autonomous_selection = default_autonomous_selection;
    draw_select(autonomous_selection);

    int button_state = 0;

	while(true){
        if(button_state == 0){
            if(pros::lcd::read_buttons() & LCD_BTN_LEFT){
                button_state = -1;
            } else if(pros::lcd::read_buttons() & LCD_BTN_RIGHT){
                button_state = 1;
            } else {
                button_state = 0;
            }

            if(button_state != 0){
                draw_unselect(autonomous_selection);
                autonomous_selection = eucmod(autonomous_selection + button_state, autonomous_programs.size());
                draw_select(autonomous_selection);
            }
        } else {
            if(pros::lcd::read_buttons() == 0){
                button_state = 0;
            }
        }

		pros::delay(10);
	}

}

/**
 * Runs the user autonomous code. This function will be started in its own task
 * with the default priority and stack size whenever the robot is enabled via
 * the Field Management System or the VEX Competition Switch in the autonomous
 * mode. Alternatively, this function may be called in initialize or opcontrol
 * for non-competition testing purposes.
 *
 * If the robot is disabled or communications is lost, the autonomous task
 * will be stopped. Re-enabling the robot will restart the task, not re-start it
 * from where it left off.
 */

void autonomous() {
    std::cout << "Autonomous start\n";

    RobotDeviceInterfaces *robot = global_robot;
    robot->activate_brakes();

    std::get<1>(autonomous_programs[autonomous_selection])(global_robot);

    std::cout << "Autonomous finish\n";
}
