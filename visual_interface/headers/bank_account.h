#pragma once
#include <ctime> 
#include <iostream>
#include <map>
#include <set>
#include <stdexcept>
#include <string>
#include <vector>

struct Bank;
enum Operation : int { Nothing = 1, WithDraw = 2, Replenishment = 3 };

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
  size_t period;  
  size_t money = 0;
  size_t startTime = 0;  
  std::pair<Operation, size_t> last_opertion = {Nothing, -1};

  Deposit(size_t the_period, Bank* the_bank);
  Bank* GetRootBank();
  bool canWithdraw() const;  
  void TakeMoney(size_t value);
  void AddMoney(size_t value);
  void GetWithDrawCanceled(size_t value);
  void GetReplenishmentCanceled(size_t value);
  void CancelLastOperation();
};

struct DebitAcc : BankAcc {
  Bank* root_bank;
  size_t money = 0;
  std::pair<Operation, size_t> last_opertion = {Nothing, -1};

  DebitAcc(Bank* the_bank);
  Bank* GetRootBank();
  void TakeMoney(size_t value);
  void AddMoney(size_t value);
  void GetWithDrawCanceled(size_t value);
  void GetReplenishmentCanceled(size_t value);
  void CancelLastOperation();
};

struct Credit : BankAcc {
  Bank* root_bank;
  Bank* GetRootBank();
  const size_t suspicious_limit; //убрал const 
  const size_t credit_limit; //убрал const 
  const double commission = 0;  //убрал const 
  Credit(size_t user_limit, size_t credit_lim, double percent, Bank* the_bank);
  int GetCommision(size_t value);
  std::pair<Operation, size_t> last_opertion = {Nothing, -1};
  int money = 0;
  void TakeMoney(size_t value);
  void AddMoney(size_t value);
  void GetWithDrawCanceled(size_t value);
  void GetReplenishmentCanceled(size_t value);
  void CancelLastOperation();
};
