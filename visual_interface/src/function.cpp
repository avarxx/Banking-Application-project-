#include "../headers/function.h"

#include <iostream>
#include <limits>
#include <stdexcept>

Bank InitionalizationByHand() {
  std::string name;
  size_t limit;
  size_t credit_lim;
  size_t commission;
  size_t dep_period;
  std::cout << "Type the name of the bank:\n";
  std::cin >> name;
  std::cout << "The bank's credit limit:\n";
  std::cin >> credit_lim;
  std::cout << "The bank's credit commission:\n";
  std::cin >> commission;
  std::cout << "The bank's deposit period:\n";
  std::cin >> dep_period;
  std::cout << "And finally, bank's unidentified user's transfer limit:\n";
  std::cin >> limit;
  return Bank(limit, credit_lim, commission, dep_period, name);
}

User InitUserByHand() {
  std::string name;
  std::string surname;
  std::string address;
  int id;
  std::cout << "Type name and surname:\n";
  std::cin >> name >> surname;
  std::cout << "Type address ('skip' to skip):\n";
  std::cin >> address;
  std::cout << "Type passport ID (integer of '-1' to skip):\n";
  std::cin >> id;
  UserName init_name(name, surname);
  User client(name, surname);
  if (address != "skip" && address != "Skip") {
    client.GetAdress(address);
  }
  if (id != -1) {
    client.GetPassportId(static_cast<size_t>(id));
  }
  std::cout << "Client added successfully!\n";
  return client;
}

bool BankExists(std::vector<Bank>& data, const std::string& name) {
  for (auto& bank : data) {
    if (bank.bank_name == name) {
      return true;
    }
  }
  return false;
}

Bank& FindBank(std::vector<Bank>& data, const std::string& name) {
  for (auto& bank : data) {
    if (bank.bank_name == name) {
      return bank;
    }
  }
  throw std::runtime_error("Wrong name");
}

void OpenCredit(std::vector<Bank>& data, const std::string& bank_name,
                User& client, size_t start_capital) {
  try {
    if (!BankExists(data, bank_name)) {
      std::cout << "Error: Bank '" << bank_name << "' does not exist.\n";
      return;
    }

    Bank& src = FindBank(data, bank_name);

    if (src.UserHasAcc(client)) {
      std::cout << "Error: " << client.client.name << " "
                << client.client.surname << " already has an account in "
                << bank_name << " bank.\n";
      return;
    }

    if (!client.WasFullInit) {
      std::cout << "Error: User must provide both address and passport ID "
                   "before opening an account.\n";
      std::cout << "Please complete user registration first.\n";
      return;
    }

    client.total_money = start_capital;
    src.MakeBlancAcc(client);

    std::cout << "Successfully opened credit account for " << client.client.name
              << " " << client.client.surname << " in " << bank_name
              << " bank.\n";
  } catch (const std::exception& e) {
    std::cout << "An unexpected error occurred: " << e.what() << std::endl;
  }
}

void OpenCreditByHand(std::vector<Bank>& data,
                      std::map<UserName, User>& user_init) {
  try {
    std::string bank_name, username, usersurname;
    size_t start_capital = 0;

    std::cout << "Enter the bank name:\n";
    std::cin >> bank_name;

    std::cout << "Now enter client name and surname:\n";
    std::cin >> username >> usersurname;

    auto user_it = user_init.find(UserName(username, usersurname));
    if (user_it == user_init.end()) {
      std::cout << "Error: Invalid user!\n";
      std::cout << "You need to create a user account first using "
                   "InitUserByHand().\n";
      return;
    }

    User& the_user = user_it->second;

    while (true) {
      std::cout << "Enter the start capital (must be non-negative):\n";
      std::cin >> start_capital;

      if (std::cin.fail()) {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "Invalid input. Please enter a valid number.\n";
        continue;
      }

      break;
    }

    OpenCredit(data, bank_name, the_user, start_capital);
  } catch (const std::exception& e) {
    std::cout << "An unexpected error occurred: " << e.what() << std::endl;
  }
}
