#include "../headers/bank_account.h"

Deposit::Deposit(size_t the_period, Bank* the_bank)
    : root_bank(the_bank), period(the_period) {
  startTime = std::time(nullptr);
}

bool Deposit::canWithdraw() const {
  time_t currentTime = time(nullptr);
  double elapsedTime = difftime(currentTime, startTime);
  return elapsedTime >= period * 24 * 60 * 60;
}

void Deposit::TakeMoney(size_t value) {
  if (!canWithdraw()) {
    std::cout << "You can't withdraw money before the deposit period ends.\n";
    throw std::runtime_error("Cannot withdraw before the deposit period ends.");
  }

  if (money < value) {
    std::cout << "Insufficient funds.\n";
    throw std::runtime_error("Insufficient funds.");
  }

  money -= value;
  last_opertion = {WithDraw, value};
}

void Deposit::AddMoney(size_t value) {
  money += value;
  last_opertion = {Replenishment, value};
}

void Deposit::GetWithDrawCanceled(size_t value) {
  money += value;
  last_opertion = {Nothing, -1};
}

void Deposit::GetReplenishmentCanceled(size_t value) {
  money -= value;
  last_opertion = {Nothing, -1};
}

void Deposit::CancelLastOperation() {
  if (last_opertion.first == WithDraw) {
    AddMoney(last_opertion.second);
    last_opertion = {Nothing, -1};
    return;
  }
  if (last_opertion.first == Replenishment) {
    TakeMoney(last_opertion.second);
    last_opertion = {Nothing, -1};
    return;
  }
  std::cout << "No operations to cancel.\n";
}

Bank* Deposit::GetRootBank() { return this->root_bank; }

DebitAcc::DebitAcc(Bank* the_bank) : root_bank(the_bank) {}

void DebitAcc::TakeMoney(size_t value) {
  if (money < value) {
    std::cout << "Insufficient funds.\n";
    throw std::runtime_error("Insufficient funds.");
  }

  money -= value;
  last_opertion = {WithDraw, value};
}

void DebitAcc::AddMoney(size_t value) {
  money += value;
  last_opertion = {Replenishment, value};
}

void DebitAcc::GetWithDrawCanceled(size_t value) {
  money += value;
  last_opertion = {Nothing, -1};
}

void DebitAcc::GetReplenishmentCanceled(size_t value) {
  money -= value;
  last_opertion = {Nothing, -1};
}

void DebitAcc::CancelLastOperation() {
  if (last_opertion.first == WithDraw) {
    AddMoney(last_opertion.second);
    last_opertion = {Nothing, -1};
    return;
  }
  if (last_opertion.first == Replenishment) {
    TakeMoney(last_opertion.second);
    last_opertion = {Nothing, -1};
    return;
  }
  std::cout << "No operations to cancel.\n";
}

Bank* DebitAcc::GetRootBank() { return this->root_bank; }

Credit::Credit(size_t user_limit, size_t credit_lim, double percent,
               Bank* the_bank)
    : root_bank(the_bank),
      suspicious_limit(user_limit),
      credit_limit(credit_lim),
      commission(percent) {}

int Credit::GetCommision(size_t value) {
  if (money >= 0) {
    return 0;
  }
  return (value * commission);
}

void Credit::TakeMoney(size_t value) {
  int total_available = money + credit_limit;

  if (total_available < static_cast<int>(value)) {
    std::cout << "Insufficient funds, including credit limit.\n";
    throw std::runtime_error("Insufficient funds, including credit limit.");
  }

  int commission = GetCommision(value);
  money -= (value + commission);
  last_opertion = {WithDraw, value};
}

void Credit::AddMoney(size_t value) {
  money += value;
  last_opertion = {Replenishment, value};
}

void Credit::GetWithDrawCanceled(size_t value) {
  int commission = GetCommision(value);
  money += (value + commission);
  last_opertion = {Nothing, -1};
}

void Credit::GetReplenishmentCanceled(size_t value) {
  money -= value;
  last_opertion = {Nothing, -1};
}

void Credit::CancelLastOperation() {
  if (last_opertion.first == WithDraw) {
    AddMoney(last_opertion.second);
    last_opertion = {Nothing, -1};
    return;
  }
  if (last_opertion.first == Replenishment) {
    TakeMoney(last_opertion.second);
    last_opertion = {Nothing, -1};
    return;
  }
  std::cout << "No operations to cancel.\n";
}

Bank* Credit::GetRootBank() { return this->root_bank; }