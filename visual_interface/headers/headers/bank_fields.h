#pragma once
#include <iostream>
#include <map>
#include <set>
#include <string>
#include <stdexcept>
#include <vector>
#include "user_data.h"
#include "bank_account.h"

struct Bank;
struct TransferData {
  size_t ID;
  size_t Transfers = 0;
  BankAcc* src;
  BankAcc* dest;
  size_t value;
  Bank* src_bank;
  Bank* dest_bank;
  User client;
  TransferData(BankAcc& source, BankAcc& destination, size_t delta, Bank* src_b, Bank* dest_b,
               const User& client);
};
struct UserFinance {
  Credit credit;
  Deposit deposit;
  DebitAcc deb_acc;
  UserFinance(const Credit& cred, const Deposit& dep, const DebitAcc& deb):
              credit(cred), deposit(dep), deb_acc(deb) {}
  UserFinance(const UserFinance& other) = default;
  UserFinance& operator=(const UserFinance& other) = default;
};
