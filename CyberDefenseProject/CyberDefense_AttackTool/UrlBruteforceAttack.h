#pragma once
#include "IAttack.h"
#include <vector>
#include <string>
#include <iostream>

class UrlBruteforceAttack : public IAttack
{
public:
	UrlBruteforceAttack();
	void run(const AttackConfig& config) override;
	const char* name() const override { return "URL brute-force"; }
private:
	std::vector<std::string> m_paths;
};