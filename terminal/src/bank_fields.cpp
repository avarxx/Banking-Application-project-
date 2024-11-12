#include "../headers/bank_fields.h"

TransferData::TransferData(BankAcc& source, BankAcc& destination, size_t delta, Bank* src_b, Bank* dest_b,
                           const User& client):
  src(&source), dest(&destination), value(delta),
  src_bank(src_b), dest_bank(dest_b), client(client) {
    ID = Transfers;
    ++Transfers;
  }