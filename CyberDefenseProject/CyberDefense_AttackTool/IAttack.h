#pragma once
#include <string>

struct AttackConfig {
	std::string host;
	unsigned short port;
	int durationSeconds;
	int ratePerSecond;
};

class IAttack
{
public:
	virtual ~IAttack() = default;
	virtual void run(const AttackConfig& config) = 0;
	virtual const char* name() const = 0;
};
