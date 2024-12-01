#pragma once
#include <iostream>
#include <map>
#include <set>
#include <string>
#include <stdexcept>
#include <vector>

struct Bank;
enum Operation : int {
  Nothing = 1,
  WithDraw = 2,
  Replenishment = 3
};
struct BankAcc {
  virtual Bank* GetRootBank() = 0;
  virtual void TakeMoney(size_t value) = 0;
  virtual void AddMoney(size_t value) = 0;
  virtual void GetWithDrawCanceled(size_t value) = 0;
  virtual void GetReplenishmentCanceled(size_t value) = 0;
  virtual void CancelLastOperation() = 0;
  ~BankAcc() = default;
};
struct Deposit : BankAcc {
  Bank* root_bank;
  Bank* GetRootBank();
  std::pair<Operation, size_t> last_opertion = {Nothing, -1};
  const size_t suspicious_limit;
  const size_t period = 0;
  size_t money = 0;
  Deposit(size_t the_period, size_t limit, Bank* the_bank);
  void TakeMoney(size_t value);
  void AddMoney(size_t value);
  void GetWithDrawCanceled(size_t value);
  void GetReplenishmentCanceled(size_t value);
  void CancelLastOperation();
};
struct DebitAcc : BankAcc {
  Bank* root_bank;
  Bank* GetRootBank();
  const size_t suspicious_limit;
  std::pair<Operation, size_t> last_opertion = {Nothing, -1};
  size_t money = 0;
  DebitAcc(size_t limit, Bank* the_bank);
  void TakeMoney(size_t value);
  void AddMoney(size_t value);
  void GetWithDrawCanceled(size_t value);
  void GetReplenishmentCanceled(size_t value);
  void CancelLastOperation();
};
struct Credit : BankAcc {
  Bank* root_bank;
  Bank* GetRootBank();
  const size_t suspicious_limit;
  const size_t credit_limit;
  const size_t commission_frac_up = 0; // процент коммиссии
  const size_t commission_frac_down = 100;
  Credit(size_t user_limit, size_t credit_lim, size_t percent, Bank* the_bank);
  int GetCommision(size_t value);
  std::pair<Operation, size_t> last_opertion = {Nothing, -1};
  int money = 0;
  void TakeMoney(size_t value);
  void AddMoney(size_t value);
  void GetWithDrawCanceled(size_t value);
  void GetReplenishmentCanceled(size_t value);
  void CancelLastOperation();
};
