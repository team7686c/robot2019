#ifndef _BLOCK_HPP_
#define _BLOCK_HPP_

#include "api.h"

#ifdef __cplusplus
extern "C" {
#endif

#include <vector>

class BlockCommand {
public:
	virtual bool check(){
		return true;
	}

	void block(){
		while(!this->check()){
	        pros::delay(2);
	    }
	}
};

class MotorBlockCommand: public BlockCommand {
private:
    pros::Motor *motor;
    double target_position;

public:
	MotorBlockCommand(pros::Motor* motor, double target_position);
    bool check() override;
};


class MultiBlockCommand: public BlockCommand {
private:
	std::vector<BlockCommand> commands;

public:
	MultiBlockCommand(std::vector<BlockCommand> commands);
	bool check() override;
};

#ifdef __cplusplus
}
#endif
#endif // _BLOCK_H_
