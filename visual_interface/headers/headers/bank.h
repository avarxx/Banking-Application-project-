#pragma once
#include <iostream>
#include <map>
#include <set>
#include <stdexcept>
#include <string>
#include <vector>

#include "bank_fields.h"

struct Bank {
  const std::string bank_name;
  const size_t unidentified_user_limit;
  std::vector<TransferData> transfer_history;
  std::set<User> client_base;
  std::map<User, UserFinance> client_data;
  const Deposit default_deposit;
  const Credit default_credit;
  const DebitAcc default_debit_acc;
  bool UserHasAcc(const User& client);
  Bank& operator=(const Bank& other) = default;
  void MakeBlancAcc(const User& client);
  void ShowTransfers();
  void CancelTransfer(size_t ID);
  void AddMoney(BankAcc* ptr, size_t value, User& user);
  void TakeMoney(BankAcc* ptr, size_t value, User& user);
  Bank(const Bank& other) = default;
  Bank(size_t limit, size_t credit_lim, size_t commission, size_t dep_period,
       const std::string& name);
};

void PrintClientData(const User& user);
void GetTransfered(BankAcc* src, BankAcc* dest, size_t value, User& client);
const std::string PrintAccType(const BankAcc& other);
void GetTransfered(Bank* src_bank, Bank* dest_bank, size_t value,
                   BankAcc* src_acc, BankAcc* dest_acc, User& user);