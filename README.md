# Banking System

## Description
### Brief:
Implementation of a system for financial operations and data storage.
### Detailed:
There are several Banks that provide financial services for money operations. Each bank has Accounts and Clients. A Client has a first name, last name, address, and passport number (first name and last name are mandatory, the rest are optional).

Accounts are of three types:
* Debit Account
* Deposit
* Credit Account  

Each account belongs to a specific client.  
* **Debit Account** – A regular account: money can be withdrawn at any time, but overdrafts are not allowed. No commission fees.
* **Deposit** – An account from which money cannot be withdrawn or transferred until its term expires (it can be replenished). No commission fees.
* **Credit Account** – Has a credit limit within which overdrafts are allowed (positive balances are also possible). There is a fixed commission fee for usage if the client has a negative balance.

## Implementation Details
Each account provides mechanisms for withdrawing, depositing, and transferring money (each account has a unique identifier). A Client is created step by step. First, the first name and last name are specified (mandatory), then the address (optional), and finally passport details (optional). If a client does not provide an address or passport number when creating an account, such an account is considered suspicious, and withdrawals or transfers above a certain amount are prohibited (each bank sets its own limit). If the client later provides all the necessary information, the account is no longer considered suspicious and can be used without restrictions. Additionally, there is a feature to cancel transactions. If it is discovered that a transaction was conducted by a fraudster, such a transaction will be reversed.

## User Guide for Managing Client Accounts

This project is designed for convenient storage and management of client accounts. It is flexible and easy to use, with numerous features that simplify performing various operations. Here's what you can do with it:

### Account Types:

Create three different types of accounts depending on your needs.

### Account Operations:

* Deposit funds into an account.
* Withdraw money.
* Transfer funds between your accounts.
* Send money to other clients' accounts, even in other banks.

### Fees and Restrictions:

All operations consider fees and restrictions, ensuring accuracy and security.

### Convenient Tools:

Simple functions for displaying information and managing accounts make working with the project easy and intuitive.

### Why Use This Project:

The project is easily adaptable to your tasks and is suitable not only for storing account data but also for performing various financial operations. It is an excellent tool for managing client accounts and expanding your business functionality.

## Build and Run

Clone the repository and run the following commands in the terminal:

```
cd terminal
./build.sh
```
