#include <iostream>
#include <map>
#include <set>
#include <string>
#include <stdexcept>
#include <vector>
#include "headers/function.h"

int main() {
  std::string input = "";
  std::vector<Bank> banks_list;
  std::vector<User> users;
  std::map<UserName, User> user_init;
  std::cout << "Type help for commands list\n";
  while(input != "exit") {
    std::cin >> input;
    if (input == "Help" || input == "help") {
      std::cout << "->init_bank: add new bank\n";
      std::cout << "->banks: get banks list\n";
      std::cout << "->add_user: add new client\n";
      std::cout << "->users: get users list\n";
      std::cout << "->open_bank_account: get user registrated in some bank\n";
    } else if (input == "init_bank") {
      banks_list.push_back(InitionalizationByHand());
      std::cout << "Bank's added succesfully!\n";
    } else if (input == "banks" || input == "Banks") {
      for (const auto& bank: banks_list) {
        std::cout << bank.bank_name << std::endl;
      }
    } else if (input == "add_user") {
      auto tmp = InitUserByHand();
      user_init.insert({tmp.client, tmp});
      users.push_back(tmp);
    } else if (input == "users") {
      if (users.size() == 0) {
        std::cout << "No users yet(\n";
        continue;
      }
      for (const auto& user : users) {
        PrintClientData(user);
      }
    } else if (input == "open_bank_account") {
      OpenCreditByHand(banks_list, user_init);
    }
  }
}