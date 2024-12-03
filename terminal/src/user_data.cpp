#include "../headers/user_data.h"

UserName::UserName(const std::string& user_name,
                   const std::string& user_surname) {
  this->name = user_name;
  this->surname = user_surname;
}
User::User(const std::string& user_name, const std::string& user_surname) {
  this->client = UserName(user_name, user_surname);
  this->WasOnlyNameInit = true;
}
User::User(const std::string& user_name, const std::string& user_surname,
           const std::string& user_address, size_t id)
    : User(user_name, user_surname) {
  this->WasOnlyNameInit = false;
  this->WasFullInit = true;
  this->passport_id = id;
  this->address = user_address;
}
void User::GetAdress(const std::string& user_address) {
  this->address = user_address;
  this->WasAdressed = true;
  if (this->HasId) {
    this->WasFullInit = true;
  }
}
void User::GetPassportId(size_t id) {
  this->passport_id = id;
  this->HasId = true;
  if (this->WasAdressed) {
    this->WasFullInit = true;
  }
}
bool operator<(const User& a, const User& b) { return a.client < b.client; }
bool operator<(const UserName& a, const UserName& b) {
  if (a.name == b.name) {
    return a.surname < b.surname;
  }
  return a.name < b.name;
}