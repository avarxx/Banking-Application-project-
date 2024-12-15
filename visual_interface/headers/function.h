#pragma once
#include <iostream>
#include <map>
#include <set>
#include <stdexcept>
#include <string>
#include <vector>

#include "bank.h"

Bank InitionalizationByHand();
User InitUserByHand();
bool BankExists(std::vector<Bank>& data, const std::string& name);
Bank& FindBank(std::vector<Bank>& data, const std::string& name);
void OpenCredit(std::vector<Bank>& data, const std::string& bank_name,
                User& client, size_t start_capital);
void OpenCreditByHand(std::vector<Bank>& data,
                      std::map<UserName, User>& user_init);