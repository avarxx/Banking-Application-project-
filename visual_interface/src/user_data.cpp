#include "../headers/user_data.h"

UserName::UserName(const std::string& user_name, const std::string& user_surname) {
  name = user_name;
  surname = user_surname;
}

User::User(const std::string& user_name, const std::string& user_surname) {
  client = UserName(user_name, user_surname);
  WasOnlyNameInit = true;
}

User::User(const std::string& user_name, const std::string& user_surname,
      const std::string& user_address, size_t id)
      : User(user_name, user_surname) {
  WasOnlyNameInit = false;
  WasFullInit = true;
  passport_id = id;
  address = user_address;
}

void User::GetAdress(const std::string& user_address) {
  address = user_address;
  WasAdressed = true;
  if (HasId) {
    WasFullInit = true;
  }
}

void User::GetPassportId(size_t id) {
  passport_id = id;
  HasId = true;
  if (WasAdressed) {
    WasFullInit = true;
  }
}

bool operator<(const User& a, const User& b) {
  return a.client < b.client;
}

bool operator<(const UserName& a, const UserName& b) {
  if (a.name == b.name) {
    return a.surname < b.surname;
  }
  return a.name < b.name;
}