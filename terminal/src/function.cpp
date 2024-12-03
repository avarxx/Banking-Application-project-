#include "../headers/function.h"

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
  std::cout << "Client added succesfully!\n";
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
  if (!BankExists(data, bank_name)) {
    std::cout << "Bank don't exist!\n";
    return;
  }
  Bank src = FindBank(data, bank_name);
  if (src.UserHasAcc(client)) {
    std::cout << client.client.name << " " << client.client.surname
              << " already has account in this bank.\n";
    return;
  }
  client.total_money = start_capital;
  src.MakeBlancAcc(client);
}
void OpenCreditByHand(std::vector<Bank>& data,
                      std::map<UserName, User>& user_init) {
  std::string bank_name = "";
  std::string username = "";
  std::string usersurname = "";
  std::cout << "Enter the bank name:\n";
  std::cin >> bank_name;
  std::cout << "Now enter client name and surname:\n";
  std::cin >> username >> usersurname;
  if (user_init.find(UserName(username, usersurname)) == user_init.end()) {
    std::cout << "Not valid user!\n You need to open user account first.\n";
    return;
  }
  auto the_user = user_init[UserName(username, usersurname)];
  std::cout << "Enter the start capital:\n";
  size_t start_capital = 0;
  std::cin >> start_capital;
  OpenCredit(data, bank_name, the_user, start_capital);
}