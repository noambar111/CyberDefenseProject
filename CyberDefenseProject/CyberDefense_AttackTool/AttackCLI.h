#pragma once
#include "IAttack.h"
#include <memory>

class AttackCLI
{
public:
	void run();

private:
	void printMenu() const;
	int readChoice() const;
	AttackConfig readConfig() const;
	std::unique_ptr<IAttack> createAttack(int choice) const;

};
