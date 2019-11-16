#include "block.h"
#include <vector>
#include <algorithm>

MotorBlockCommand::MotorBlockCommand(pros::Motor* motor, double target_position){
    this->motor = motor;
    this->target_position = target_position;
}

bool MotorBlockCommand::check() {
   const double TARGET_SIZE = 0.05;

   auto pos = this->motor->get_position();
   return (pos < target_position + TARGET_SIZE) && (pos > target_position - TARGET_SIZE);
}

MultiBlockCommand::MultiBlockCommand(std::vector<BlockCommand> commands){
    this->commands = commands;
}

bool MultiBlockCommand::check() {
   return std::all_of(commands.begin(), commands.end(), [](BlockCommand command){
       return command.check();
   });
}
