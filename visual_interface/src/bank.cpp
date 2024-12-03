#include "../headers/bank.h"

Bank::Bank(size_t limit, size_t credit_lim, double commission,
           size_t dep_period, const std::string& name)
    : bank_name(name),
      unidentified_user_limit(limit),
      default_deposit(Deposit(dep_period, this)),
      default_credit(Credit(limit, credit_lim, commission, this)),
      default_debit_acc(DebitAcc(this)) {}

bool Bank::UserHasAcc(const User& client) {
  return client_base.find(client) != client_base.end();
}

void Bank::MakeBlancAcc(const User& client) {
  UserFinance account(default_credit, default_deposit, default_debit_acc);
  client_base.insert(client);
  client_data.insert({client, account});
}

void GetTransfered(BankAcc* src, BankAcc* dest, size_t value, User& client) {
  src->TakeMoney(value);
  dest->AddMoney(value);
  auto src_ptr = src->GetRootBank();
  auto dest_ptr = dest->GetRootBank();
  TransferData note(*src, *dest, value, src_ptr, dest_ptr, client);
  src_ptr->transfer_history.push_back(note);
  dest_ptr->transfer_history.push_back(note);
}

void PrintClientData(const User& user) {
  std::cout << "Name: " << user.client.name << "\n";
  std::cout << "Surname: " << user.client.surname << "\n";
  if (user.HasId) {
    std::cout << "Passport ID: " << user.passport_id << "\n";
  } else {
    std::cout << "No passport ID added\n";
  }
  if (user.WasAdressed) {
    std::cout << "Address: " << user.address << "\n";
  } else {
    std::cout << "No address added\n";
  }
  std::cout << "Money: " << user.total_money << "\n";
}

const std::string PrintAccType(const BankAcc& other) {
  auto ptr_cred = dynamic_cast<const Credit*>(&other);
  auto ptr_deb = dynamic_cast<const DebitAcc*>(&other);
  if (ptr_cred != nullptr) {
    return "Credit";
  }
  if (ptr_deb != nullptr) {
    return "Debit account";
  }
  return "Deposit";
}

void Bank::ShowTransfers() {
  for (auto& transfer : transfer_history) {
    std::cout << "Transfer ID: " << transfer.ID << "\n";
    std::cout << "Amount of money transfered: " << transfer.value << "\n";
    std::cout << "Source: " << PrintAccType(*transfer.src) << " in bank ";
    std::cout << transfer.src_bank->bank_name << "\nTo "
              << PrintAccType(*transfer.dest);
    std::cout << " in bank " << transfer.dest_bank->bank_name << std::endl;
    std::cout << "Client data: \n";
    PrintClientData(transfer.client);
  }
}

void Bank::AddMoney(BankAcc* ptr, size_t value, User& user) {
  ptr->AddMoney(value);
  user.total_money += value;
}

void Bank::TakeMoney(BankAcc* ptr, size_t value, User& user) {
  if (value <= user.total_money) {
    ptr->TakeMoney(value);
    user.total_money -= value;
    return;
  }
  std::cout << "User doesn't have enough money\n";
}

void GetTransfered(Bank* src_bank, Bank* dest_bank, size_t value,
                   BankAcc* src_acc, BankAcc* dest_acc, User& user) {
  if (!src_bank->UserHasAcc(user) || !dest_bank->UserHasAcc(user)) {
    std::cout << "Invalid transfer\n";
    return;
  }
  try {
    src_bank->TakeMoney(src_acc, value, user);
    dest_bank->AddMoney(dest_acc, value, user);
    user.total_money -= value;
  } catch (...) {
    std::cout << "Not enough money on account\n";
    return;
  }
}

void Bank::CancelTransfer(size_t transfer_ID) {
  TransferData* the_transfer = nullptr;
  for (auto& transfer : transfer_history) {
    if (transfer.ID == transfer_ID) {
      the_transfer = &transfer;
      break;
    }
  }
  if (the_transfer != nullptr) {
    GetTransfered(the_transfer->dest_bank, the_transfer->src_bank,
                  the_transfer->value, the_transfer->src, the_transfer->dest,
                  the_transfer->client);
  } else {
    std::cout << "Transfer not found\n";
  }
}
