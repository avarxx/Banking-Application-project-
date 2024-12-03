#include "../headers/bank_account.h"

Bank* Deposit::GetRootBank() { return this->root_bank; }
Deposit::Deposit(size_t the_period, size_t limit, Bank* the_bank)
    : suspicious_limit(limit), period(the_period), root_bank(the_bank) {}
void Deposit::TakeMoney(size_t value) {
  if (this->money < value) {
    std::cout << "U cant do it\n";
    throw;
  }
  this->money -= value;
  this->last_opertion = {WithDraw, value};
}
void Deposit::AddMoney(size_t value) {
  this->money += value;
  this->last_opertion = {Replenishment, value};
}
void Deposit::GetWithDrawCanceled(size_t value) {
  this->money += value;
  this->last_opertion = {Nothing, -1};
}
void Deposit::GetReplenishmentCanceled(size_t value) {
  this->money -= value;
  this->last_opertion = {Nothing, -1};
}
void Deposit::CancelLastOperation() {
  if (this->last_opertion.first == WithDraw) {
    this->TakeMoney(last_opertion.second);
    this->last_opertion = {Nothing, -1};
    return;
  }
  if (this->last_opertion.first == Replenishment) {
    this->AddMoney(last_opertion.second);
    this->last_opertion = {Nothing, -1};
    return;
  }
  std::cout << "There were no operations yet!\n";
}

Bank* DebitAcc::GetRootBank() { return this->root_bank; }
DebitAcc::DebitAcc(size_t limit, Bank* the_bank)
    : suspicious_limit(limit), root_bank(the_bank) {}
void DebitAcc::TakeMoney(size_t value) {
  if (this->money < value) {
    std::cout << "U cant do it\n";
    throw;
  }
  this->money -= value;
  this->last_opertion = {WithDraw, value};
}
void DebitAcc::AddMoney(size_t value) {
  this->money += value;
  this->last_opertion = {Replenishment, value};
}
void DebitAcc::GetWithDrawCanceled(size_t value) {
  this->money += value;
  this->last_opertion = {Nothing, -1};
}
void DebitAcc::GetReplenishmentCanceled(size_t value) {
  this->money -= value;
  this->last_opertion = {Nothing, -1};
}
void DebitAcc::CancelLastOperation() {
  if (this->last_opertion.first == WithDraw) {
    this->TakeMoney(last_opertion.second);
    this->last_opertion = {Nothing, -1};
    return;
  }
  if (this->last_opertion.first == Replenishment) {
    this->AddMoney(last_opertion.second);
    this->last_opertion = {Nothing, -1};
    return;
  }
  std::cout << "No operations yet!\n";
}

Bank* Credit::GetRootBank() { return this->root_bank; }

Credit::Credit(size_t user_limit, size_t credit_lim, size_t percent,
               Bank* the_bank)
    : suspicious_limit(user_limit),
      credit_limit(credit_lim),
      commission_frac_up(percent),
      root_bank(the_bank) {}
int Credit::GetCommision(size_t value) {
  return (value * commission_frac_up) / commission_frac_down;
}
void Credit::TakeMoney(size_t value) {
  this->money -= value + GetCommision(value);
  this->last_opertion = {WithDraw, value};
}
void Credit::AddMoney(size_t value) {
  this->money -= value - GetCommision(value);
  this->last_opertion = {Replenishment, value};
}
void Credit::GetWithDrawCanceled(size_t value) {
  this->money += value;
  this->last_opertion = {Nothing, -1};
}
void Credit::GetReplenishmentCanceled(size_t value) {
  this->money -= value;
  this->last_opertion = {Nothing, -1};
}
void Credit::CancelLastOperation() {
  if (this->last_opertion.first == WithDraw) {
    this->TakeMoney(last_opertion.second);
    this->last_opertion = {Nothing, -1};
    return;
  }
  if (this->last_opertion.first == Replenishment) {
    this->AddMoney(last_opertion.second);
    this->last_opertion = {Nothing, -1};
    return;
  }
  std::cout << "No operations yet!\n";
}