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

    // Move the arms up then down
    robot->arm->move_angle(0.1)->block();
    robot->arm->move_angle(-0.1)->block();

    // Start the roller
    robot->roller->move_velocity(150);
    pros::delay(1500);

    // Move the tray back
    robot->tray->move_angle(-0.25)->block();

    // Stop the rollers
    robot->roller->move_velocity(0);

    // Finished unfolding
}

const int default_autonomous_selection = 1;

int autonomous_selection;
std::vector<std::tuple<std::string, void (*)(RobotDeviceInterfaces*)>> autonomous_programs = {
    {"None", [](RobotDeviceInterfaces* robot){}},
    {"1 point autonomous", [](RobotDeviceInterfaces *robot){
        unfold(robot);

        robot->arm->move_angle(0.15)->block();

        // Drive forward then backward to push a cube into the goal zone.
        robot->straight_drive->move_distance(12)->block();
        robot->straight_drive->move_distance(-12)->block();
    }},
    {"4 point autonomous", [](RobotDeviceInterfaces *robot){
        unfold(robot);

        // Drive the robot forward
        robot->straight_drive->move_distance(12)->block();
    }},
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

    std::get<1>(autonomous_programs[autonomous_selection])(global_robot);

    std::cout << "Autonomous finish\n";
}
