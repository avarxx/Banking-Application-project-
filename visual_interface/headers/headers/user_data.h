#pragma once
#include <iostream>
#include <map>
#include <set>
#include <string>
#include <stdexcept>
#include <vector>

struct UserName {
  std::string name = "";
  std::string surname = "";
  UserName& operator=(const UserName& other) = default; 
  UserName() = default;
  UserName(const std::string& user_name, const std::string& user_surname);
};
bool operator<(const UserName& a, const UserName& b);
struct User {
  size_t total_money = 0;
  UserName client;
  std::string address = "";
  bool WasOnlyNameInit = false;
  bool WasFullInit = false;
  bool WasAdressed = false;
  bool HasId = false;
  size_t passport_id = 0;
  User() = default;
  User(const std::string& user_name, const std::string& user_surname);
  User(const std::string& user_name, const std::string& user_surname,
       const std::string& user_address, size_t id);
  void GetAdress(const std::string& user_address);
  void GetPassportId(size_t id);
};

bool operator<(const User& a, const User& b);