#include "../headers/mainwindow.h"

#include <QDebug>
#include <QDoubleSpinBox>
#include <QHeaderView>
#include <QInputDialog>
#include <QLabel>
#include <QMessageBox>
#include <QPushButton>
#include <QTableView>
#include <QVBoxLayout>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
  setWindowTitle("Banking System Management");
  resize(1200, 800);

  QWidget *centralWidget = new QWidget(this);
  QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);

  QTabWidget *tabWidget = new QTabWidget();

  QWidget *clientsTab = new QWidget();
  QWidget *accountsTab = new QWidget();
  QWidget *transactionsTab = new QWidget();
  QWidget *banksTab = new QWidget();

  createClientsTab(clientsTab);
  createAccountsTab(accountsTab);
  createBanksTab(banksTab);

  tabWidget->addTab(clientsTab, "Clients");
  tabWidget->addTab(accountsTab, "Accounts");
  tabWidget->addTab(banksTab, "Banks");

  mainLayout->addWidget(tabWidget);
  setCentralWidget(centralWidget);
}

void MainWindow::createMenuBar() {
  QMenuBar *menuBar = new QMenuBar(this);
  setMenuBar(menuBar);

  QMenu *userMenu = menuBar->addMenu("&User");
  QAction *loginAction = new QAction("Login", this);
  QAction *logoutAction = new QAction("Logout", this);
  userMenu->addAction(loginAction);
  userMenu->addAction(logoutAction);

  QMenu *settingsMenu = menuBar->addMenu("&Settings");
  QAction *preferencesAction = new QAction("Preferences", this);
  settingsMenu->addAction(preferencesAction);
}

void MainWindow::createClientsTab(QWidget *parent) {
  QVBoxLayout *layout = new QVBoxLayout(parent);

  QHBoxLayout *buttonsLayout = new QHBoxLayout();
  QPushButton *addClientBtn = new QPushButton("Add Client");
  QPushButton *editClientBtn = new QPushButton("Edit Client");
  QPushButton *deleteClientBtn = new QPushButton("Delete Client");
  buttonsLayout->addWidget(addClientBtn);
  buttonsLayout->addWidget(editClientBtn);
  buttonsLayout->addWidget(deleteClientBtn);

  clientsTableView = new QTableView();
  clientsModel = new QStandardItemModel(0, 6, this);
  clientsModel->setHorizontalHeaderLabels(
      {"Name", "Surname", "Address", "Passport", "Accounts", "Status"});
  clientsTableView->setModel(clientsModel);

  connect(addClientBtn, &QPushButton::clicked, this,
          &MainWindow::showAddClientDialog);
  connect(editClientBtn, &QPushButton::clicked, this,
          &MainWindow::showEditClientDialog);
  connect(deleteClientBtn, &QPushButton::clicked, this,
          &MainWindow::deleteClient);

  layout->addLayout(buttonsLayout);
  layout->addWidget(clientsTableView);
}

void MainWindow::showAddClientDialog() {
  QDialog dialog(this);
  dialog.setWindowTitle("Add New Client");
  QFormLayout *formLayout = new QFormLayout();

  QLineEdit *nameEdit = new QLineEdit(&dialog);
  QLineEdit *surnameEdit = new QLineEdit(&dialog);
  QLineEdit *addressEdit = new QLineEdit(&dialog);
  QLineEdit *passportEdit = new QLineEdit(&dialog);

  formLayout->addRow("Name:", nameEdit);
  formLayout->addRow("Surname:", surnameEdit);
  formLayout->addRow("Address:", addressEdit);
  formLayout->addRow("Passport ID:", passportEdit);

  QHBoxLayout *buttonsLayout = new QHBoxLayout();
  QPushButton *saveBtn = new QPushButton("Save");
  QPushButton *cancelBtn = new QPushButton("Cancel");
  buttonsLayout->addWidget(saveBtn);
  buttonsLayout->addWidget(cancelBtn);

  QVBoxLayout *dialogLayout = new QVBoxLayout(&dialog);
  dialogLayout->addLayout(formLayout);
  dialogLayout->addLayout(buttonsLayout);

  dialog.setLayout(dialogLayout);

  connect(saveBtn, &QPushButton::clicked, [&]() {
    if (nameEdit->text().isEmpty() || surnameEdit->text().isEmpty()) {
      QMessageBox::warning(this, "Error", "Name and Surname are required!");
      return;
    }

    if (passportEdit->text().isEmpty()) {
      QMessageBox::warning(this, "Error", "Passport ID is required!");
      return;
    }

    try {
      User newUser(nameEdit->text().toStdString(),
                   surnameEdit->text().toStdString());

      if (!addressEdit->text().isEmpty()) {
        newUser.GetAdress(addressEdit->text().toStdString());
      }

      bool ok;
      size_t passportId = passportEdit->text().toULong(&ok);
      if (!ok) {
        QMessageBox::warning(this, "Error", "Invalid Passport ID!");
        return;
      }
      newUser.GetPassportId(passportId);

      UserKey userKey{nameEdit->text().toStdString(),
                      surnameEdit->text().toStdString(), passportId};
      userInitMap[userKey] = newUser;

      populateClientsTable();

      dialog.accept();
    } catch (const std::exception &e) {
      QMessageBox::critical(this, "Error",
                            QString("Failed to add client: %1").arg(e.what()));
    }
  });

  connect(cancelBtn, &QPushButton::clicked, &dialog, &QDialog::reject);

  dialog.exec();
}

void MainWindow::showEditClientDialog() {
  bool ok;
  QString userIdInput = QInputDialog::getText(
      this, "Enter User Passport",
      "Enter the Passport of the client to edit:", QLineEdit::Normal, "", &ok);

  if (!ok || userIdInput.isEmpty()) {
    QMessageBox::critical(this, "Error", "User Passport is required.");
    return;
  }

  bool idOk;
  size_t userId = userIdInput.toULong(&idOk);
  if (!idOk || userId == 0) {
    QMessageBox::critical(this, "Error", "Invalid Passport entered.");
    return;
  }

  User *user = nullptr;
  UserKey foundKey;
  for (auto &pair : userInitMap) {
    if (pair.second.passport_id == userId) {
      user = &pair.second;
      foundKey = pair.first;
      break;
    }
  }

  if (!user) {
    QMessageBox::critical(this, "Error",
                          "Client not found with the provided Passport.");
    return;
  }

  QDialog dialog(this);
  dialog.setWindowTitle("Edit Client");

  QFormLayout *formLayout = new QFormLayout();

  QLineEdit *nameEdit = new QLineEdit(&dialog);
  nameEdit->setText(QString::fromStdString(user->client.name));

  QLineEdit *surnameEdit = new QLineEdit(&dialog);
  surnameEdit->setText(QString::fromStdString(user->client.surname));

  QLineEdit *addressEdit = new QLineEdit(&dialog);
  if (user->WasAdressed) {
    addressEdit->setText(QString::fromStdString(user->address));
  }

  QLineEdit *passportEdit = new QLineEdit(&dialog);
  passportEdit->setText(QString::number(user->passport_id));
  passportEdit->setReadOnly(true);

  formLayout->addRow("Name:", nameEdit);
  formLayout->addRow("Surname:", surnameEdit);
  formLayout->addRow("Address:", addressEdit);
  formLayout->addRow("Passport ID:", passportEdit);

  QHBoxLayout *buttonsLayout = new QHBoxLayout();
  QPushButton *saveBtn = new QPushButton("Save");
  QPushButton *cancelBtn = new QPushButton("Cancel");
  buttonsLayout->addWidget(saveBtn);
  buttonsLayout->addWidget(cancelBtn);

  QVBoxLayout *dialogLayout = new QVBoxLayout(&dialog);
  dialogLayout->addLayout(formLayout);
  dialogLayout->addLayout(buttonsLayout);

  dialog.setLayout(dialogLayout);

  connect(saveBtn, &QPushButton::clicked, [&]() {
    try {
      userInitMap.erase(foundKey);

      User updatedUser(nameEdit->text().toStdString(),
                       surnameEdit->text().toStdString());

      if (!addressEdit->text().isEmpty()) {
        updatedUser.GetAdress(addressEdit->text().toStdString());
      }

      updatedUser.GetPassportId(user->passport_id);

      UserKey updatedKey{nameEdit->text().toStdString(),
                         surnameEdit->text().toStdString(), user->passport_id};

      userInitMap[updatedKey] = updatedUser;

      populateClientsTable();
      dialog.accept();
    } catch (const std::exception &e) {
      QMessageBox::critical(
          this, "Error", QString("Failed to update client: %1").arg(e.what()));
    }
  });

  connect(cancelBtn, &QPushButton::clicked, &dialog, &QDialog::reject);

  dialog.exec();
}

void MainWindow::deleteClient() {
  if (!clientsModel) {
    QMessageBox::critical(this, "Delete Client",
                          "Clients model is not initialized.");
    return;
  }

  bool ok;
  QString passport = QInputDialog::getText(
      this, "Delete Client", "Enter Passport Number:", QLineEdit::Normal, "",
      &ok);

  if (!ok || passport.isEmpty()) {
    QMessageBox::information(this, "Delete Client", "Operation canceled.");
    return;
  }

  bool clientFound = false;
  for (int row = 0; row < clientsModel->rowCount(); ++row) {
    QString clientPassport = clientsModel->item(row, 3)->text();

    if (clientPassport == passport) {
      clientFound = true;

      QMessageBox::StandardButton reply;
      reply = QMessageBox::question(
          this, "Delete Client", "Are you sure you want to delete this client?",
          QMessageBox::Yes | QMessageBox::No);

      if (reply == QMessageBox::Yes) {
        for (auto it = userInitMap.begin(); it != userInitMap.end(); ++it) {
          if (it->second.passport_id == passport.toULong()) {
            userInitMap.erase(it);
            break;
          }
        }

        clientsModel->removeRow(row);

        QMessageBox::information(this, "Delete Client",
                                 "Client deleted successfully.");
      } else {
        QMessageBox::information(this, "Delete Client", "Operation canceled.");
      }

      break;
    }
  }

  if (!clientFound) {
    QMessageBox::warning(
        this, "Delete Client",
        "Client with the specified passport number was not found.");
  }
}

void MainWindow::deleteAccount() {
  QModelIndexList selectedIndexes =
      accountsTableView->selectionModel()->selectedRows();

  if (selectedIndexes.isEmpty()) {
    QMessageBox::warning(this, "Delete Account",
                         "Please select an account to delete.");
    return;
  }

  int row = selectedIndexes.first().row();
  QString accountType = accountsModel->item(row, 1)->text();
  double balance = accountsModel->item(row, 3)->text().toDouble();
  QString creditLimit = accountsModel->item(row, 4)->text();

  if (accountType == "Credit") {
    double limit = creditLimit.toDouble();

    if (balance < limit) {
      QMessageBox::warning(
          this, "Delete Account",
          "Cannot delete credit account: balance is below credit limit.");
      return;
    }
  }

  QMessageBox::StandardButton reply;
  reply = QMessageBox::question(this, "Delete Account",
                                "Are you sure you want to delete this account?",
                                QMessageBox::Yes | QMessageBox::No);

  if (reply == QMessageBox::Yes) {
    accountsModel->removeRow(row);

    updateClientAccountCount();

    QMessageBox::information(this, "Delete Account",
                             "Account deleted successfully.");
  }
}

void MainWindow::showTransferDialog() {
  QDialog dialog(this);
  dialog.setWindowTitle("Transfer Money Between Accounts");

  QVBoxLayout *dialogLayout = new QVBoxLayout(&dialog);
  QFormLayout *formLayout = new QFormLayout();

  QComboBox *sourceAccountCombo = new QComboBox(&dialog);
  QComboBox *destAccountCombo = new QComboBox(&dialog);

  for (int i = 0; i < accountsModel->rowCount(); ++i) {
    QString accountId = accountsModel->item(i, 0)->text();
    QString accountType = accountsModel->item(i, 1)->text();
    QString owner = accountsModel->item(i, 2)->text();
    QString balance = accountsModel->item(i, 3)->text();
    QString limitOrPeriod = accountsModel->item(i, 4)->text();

    QString displayText = QString("%1 - %2 (%3) - Balance: %4")
                              .arg(accountId, accountType, owner, balance);

    if (accountType != "Deposit" || limitOrPeriod.toInt() == 0) {
      sourceAccountCombo->addItem(displayText, i);
    }
    destAccountCombo->addItem(displayText, i);
  }

  formLayout->addRow("Source Account:", sourceAccountCombo);
  formLayout->addRow("Destination Account:", destAccountCombo);

  QLineEdit *amountEdit = new QLineEdit(&dialog);
  formLayout->addRow("Transfer Amount:", amountEdit);

  QLabel *commissionLabel = new QLabel("Commission:", &dialog);
  QLabel *commissionValueLabel = new QLabel("0.0%", &dialog);
  formLayout->addRow(commissionLabel, commissionValueLabel);

  QHBoxLayout *buttonsLayout = new QHBoxLayout();
  QPushButton *transferBtn = new QPushButton("Transfer");
  QPushButton *cancelBtn = new QPushButton("Cancel");
  buttonsLayout->addWidget(transferBtn);
  buttonsLayout->addWidget(cancelBtn);

  dialogLayout->addLayout(formLayout);
  dialogLayout->addLayout(buttonsLayout);
  dialog.setLayout(dialogLayout);

  auto updateCommission = [&]() {
    int sourceRow = sourceAccountCombo->currentData().toInt();
    QString sourceType = accountsModel->item(sourceRow, 1)->text();
    QString sourceBank = accountsModel->item(sourceRow, 6)->text();

    double commissionRate = 0.0;
    if (sourceType == "Credit") {
      for (const Bank &bank : bankData) {
        if (bank.bank_name == sourceBank.toStdString()) {
          commissionRate = bank.default_credit.commission;
          break;
        }
      }
      commissionLabel->show();
      commissionValueLabel->show();
      commissionValueLabel->setText(QString("%1%").arg(commissionRate));
    } else {
      commissionLabel->hide();
      commissionValueLabel->hide();
    }
  };

  updateCommission();

  connect(sourceAccountCombo,
          QOverload<int>::of(&QComboBox::currentIndexChanged),
          updateCommission);

  connect(transferBtn, &QPushButton::clicked, [&]() {
    bool ok;
    double amount = amountEdit->text().toDouble(&ok);

    if (!ok || amount <= 0) {
      QMessageBox::warning(&dialog, "Error", "Invalid transfer amount.");
      return;
    }

    int sourceRow = sourceAccountCombo->currentData().toInt();
    int destRow = destAccountCombo->currentData().toInt();

    QString sourceType = accountsModel->item(sourceRow, 1)->text();
    QString destType = accountsModel->item(destRow, 1)->text();
    QString sourceBank = accountsModel->item(sourceRow, 6)->text();
    QString srcLimitOrPeriod = accountsModel->item(sourceRow, 4)->text();

    if (sourceType == "Deposit" && srcLimitOrPeriod.toInt() > 0) {
      QMessageBox::warning(
          &dialog, "Error",
          "Cannot transfer from a deposit account before the period ends.");
      return;
    }

    QStandardItem *sourceBalanceItem = accountsModel->item(sourceRow, 3);
    QStandardItem *destBalanceItem = accountsModel->item(destRow, 3);

    double sourceBalance = sourceBalanceItem->text().toDouble();
    double commission = 0.0;

    if (sourceType == "Credit") {
      for (const Bank &bank : bankData) {
        if (bank.bank_name == sourceBank.toStdString()) {
          commission = amount * (bank.default_credit.commission / 100);
          break;
        }
      }
    }

    double totalDeduction = amount + commission;

    if (totalDeduction > sourceBalance) {
      QMessageBox::warning(&dialog, "Error", "Insufficient funds.");
      return;
    }

    sourceBalanceItem->setText(QString::number(sourceBalance - totalDeduction));

    double destBalance = destBalanceItem->text().toDouble();
    destBalanceItem->setText(QString::number(destBalance + amount));

    if (commission > 0) {
      QMessageBox::information(&dialog, "Transfer Successful",
                               QString("Transferred %1. Commission: %2")
                                   .arg(amount)
                                   .arg(commission));
    } else {
      QMessageBox::information(&dialog, "Transfer Successful",
                               QString("Transferred %1").arg(amount));
    }

    dialog.accept();
  });

  connect(cancelBtn, &QPushButton::clicked, &dialog, &QDialog::reject);

  dialog.exec();
}

void MainWindow::createAccountsTab(QWidget *parent) {
  QVBoxLayout *layout = new QVBoxLayout(parent);
  QHBoxLayout *buttonsLayout = new QHBoxLayout();
  QPushButton *addAccountBtn = new QPushButton("Add Account");
  QPushButton *deleteAccountBtn = new QPushButton("Delete Account");
  QPushButton *transferBtn = new QPushButton("Transfer");
  buttonsLayout->addWidget(addAccountBtn);
  buttonsLayout->addWidget(deleteAccountBtn);
  buttonsLayout->addWidget(transferBtn);

  accountsTableView = new QTableView();
  accountsModel = new QStandardItemModel(0, 7, this);
  accountsModel->setHorizontalHeaderLabels(
      {"ID", "Type", "Owner", "Balance", "Limit/Period", "Rate", "Bank"});
  accountsTableView->setModel(accountsModel);

  connect(addAccountBtn, &QPushButton::clicked, this,
          &MainWindow::showAddAccountDialog);
  connect(deleteAccountBtn, &QPushButton::clicked, this,
          &MainWindow::deleteAccount);
  connect(transferBtn, &QPushButton::clicked, this,
          &MainWindow::showTransferDialog);

  layout->addLayout(buttonsLayout);
  layout->addWidget(accountsTableView);
}

void MainWindow::showAddAccountDialog() {
  if (bankData.empty()) {
    QMessageBox::warning(this, "Add Account",
                         "Please add a bank first before creating an account.");
    return;
  }

  QDialog typeDialog(this);
  typeDialog.setWindowTitle("Select Account Type");

  QVBoxLayout *typeLayout = new QVBoxLayout(&typeDialog);
  QLabel *typeLabel = new QLabel("Select Account Type:", &typeDialog);

  QComboBox *accountTypeCombo = new QComboBox(&typeDialog);
  accountTypeCombo->addItems({"Debit", "Credit", "Deposit"});

  QPushButton *nextButton = new QPushButton("Next", &typeDialog);

  typeLayout->addWidget(typeLabel);
  typeLayout->addWidget(accountTypeCombo);
  typeLayout->addWidget(nextButton);
  typeDialog.setLayout(typeLayout);

  connect(nextButton, &QPushButton::clicked, [&]() {
    QString selectedType = accountTypeCombo->currentText();
    typeDialog.accept();

    QDialog dialog(this);
    dialog.setWindowTitle("Add New " + selectedType + " Account");

    QFormLayout *formLayout = new QFormLayout();

    QComboBox *bankCombo = new QComboBox(&dialog);
    for (const Bank &bank : bankData) {
      bankCombo->addItem(QString::fromStdString(bank.bank_name));
    }
    formLayout->addRow("Select Bank:", bankCombo);

    QComboBox *clientCombo = new QComboBox(&dialog);
    for (const auto &[name, user] : userInitMap) {
      clientCombo->addItem(
          QString::fromStdString(name.name + " " + name.surname));
    }
    formLayout->addRow("Select Client:", clientCombo);

    QLineEdit *balanceEdit = new QLineEdit(&dialog);
    formLayout->addRow("Initial Balance:", balanceEdit);

    QLineEdit *limitEdit = nullptr;
    QLabel *limitLabel = nullptr;
    QLabel *periodLabel = nullptr;
    double creditRate = 0.0;
    size_t depositPeriod = 0;
    size_t creditLimit = 0;

    auto updateBankDetails = [&](const QString &bankName) {
      for (const Bank &bank : bankData) {
        if (bank.bank_name == bankName.toStdString()) {
          if (selectedType == "Credit") {
            creditRate = bank.default_credit.commission;
            creditLimit = bank.default_credit.credit_limit;
            if (limitEdit) {
              limitEdit->setText(QString::number(creditLimit));
            }
            if (limitLabel) {
              limitLabel->setText(QString::number(creditRate) + "%");
            }
          } else if (selectedType == "Deposit") {
            depositPeriod = bank.default_deposit.period;
            if (periodLabel) {
              periodLabel->setText(QString::number(depositPeriod) + " days");
            }
          }
          break;
        }
      }
    };

    if (selectedType == "Credit") {
      limitEdit = new QLineEdit(&dialog);
      limitLabel = new QLabel(&dialog);
      formLayout->addRow("Credit Limit:", limitEdit);
      formLayout->addRow("Credit Rate:", limitLabel);
    } else if (selectedType == "Deposit") {
      periodLabel = new QLabel(&dialog);
      formLayout->addRow("Deposit Period:", periodLabel);
    }

    connect(bankCombo, &QComboBox::currentTextChanged, updateBankDetails);

    updateBankDetails(bankCombo->currentText());

    QHBoxLayout *buttonsLayout = new QHBoxLayout();
    QPushButton *saveBtn = new QPushButton("Save");
    QPushButton *cancelBtn = new QPushButton("Cancel");
    buttonsLayout->addWidget(saveBtn);
    buttonsLayout->addWidget(cancelBtn);

    QVBoxLayout *dialogLayout = new QVBoxLayout(&dialog);
    dialogLayout->addLayout(formLayout);
    dialogLayout->addLayout(buttonsLayout);
    dialog.setLayout(dialogLayout);

    connect(saveBtn, &QPushButton::clicked, [&]() {
      if (clientCombo->currentIndex() == -1 || balanceEdit->text().isEmpty()) {
        QMessageBox::warning(this, "Error",
                             "Client and Initial Balance are required!");
        return;
      }

      bool balanceOk;
      double balance = balanceEdit->text().toDouble(&balanceOk);
      if (!balanceOk || balance < 0) {
        QMessageBox::warning(this, "Error", "Invalid balance amount!");
        return;
      }

      if (selectedType == "Credit") {
        bool limitOk;
        double creditLimit = limitEdit->text().toDouble(&limitOk);
        if (!limitOk) {
          QMessageBox::warning(this, "Error", "Invalid credit limit!");
          return;
        }

        if (balance > creditLimit) {
          QMessageBox::warning(this, "Error",
                               "Initial balance cannot exceed credit limit!");
          return;
        }
      }

      QString selectedBank = bankCombo->currentText();
      Bank *chosenBank = nullptr;
      for (Bank &bank : bankData) {
        if (bank.bank_name == selectedBank.toStdString()) {
          chosenBank = &bank;
          break;
        }
      }

      if (!chosenBank) {
        QMessageBox::warning(this, "Error", "Bank selection failed!");
        return;
      }

      QString selectedClient = clientCombo->currentText();
      QStringList nameParts = selectedClient.split(" ");
      if (nameParts.size() < 2) {
        QMessageBox::warning(this, "Error", "Invalid client name!");
        return;
      }

      int row = accountsModel->rowCount();
      QString accountId = QString::number(row + 1);

      QString limitOrPeriodValue;
      QString rateValue = "0";
      if (selectedType == "Credit") {
        limitOrPeriodValue = limitEdit ? limitEdit->text() : "";
        rateValue = limitLabel ? limitLabel->text().replace("%", "") : "0";
      } else if (selectedType == "Deposit") {
        limitOrPeriodValue =
            periodLabel ? periodLabel->text().replace(" days", "") : "";
      } else {
        limitOrPeriodValue = "";
      }

      QList<QStandardItem *> accountRow;
      accountRow << new QStandardItem(accountId)                 // ID
                 << new QStandardItem(selectedType)              // Type
                 << new QStandardItem(selectedClient)            // Owner
                 << new QStandardItem(QString::number(balance))  // Balance
                 << new QStandardItem(limitOrPeriodValue)        // Limit/Period
                 << new QStandardItem(rateValue)                 // Rate
                 << new QStandardItem(selectedBank);             // Bank

      accountsModel->appendRow(accountRow);

      updateClientAccountCount();

      dialog.accept();
    });

    connect(cancelBtn, &QPushButton::clicked, &dialog, &QDialog::reject);

    dialog.exec();
  });

  typeDialog.exec();
}

void MainWindow::showAddBankDialog() {
  QDialog dialog(this);
  dialog.setWindowTitle("Add New Bank");

  QFormLayout *formLayout = new QFormLayout();

  QLineEdit *bankNameEdit = new QLineEdit(&dialog);
  formLayout->addRow("Bank Name:", bankNameEdit);

  QLineEdit *limitEdit = new QLineEdit(&dialog);
  formLayout->addRow("Unidentified User Limit:", limitEdit);

  QLineEdit *creditLimitEdit = new QLineEdit(&dialog);
  formLayout->addRow("Credit Limit:", creditLimitEdit);

  QDoubleSpinBox *commissionEdit = new QDoubleSpinBox(&dialog);
  formLayout->addRow("Commission:", commissionEdit);

  QLineEdit *depPeriodEdit = new QLineEdit(&dialog);
  formLayout->addRow("Deposit Period (days):", depPeriodEdit);

  QHBoxLayout *buttonsLayout = new QHBoxLayout();
  QPushButton *saveBtn = new QPushButton("Save");
  QPushButton *cancelBtn = new QPushButton("Cancel");
  buttonsLayout->addWidget(saveBtn);
  buttonsLayout->addWidget(cancelBtn);

  QVBoxLayout *dialogLayout = new QVBoxLayout(&dialog);
  dialogLayout->addLayout(formLayout);
  dialogLayout->addLayout(buttonsLayout);

  dialog.setLayout(dialogLayout);

  connect(saveBtn, &QPushButton::clicked, [&]() {
    std::string bankName = bankNameEdit->text().toStdString();
    bool limitOk, creditLimitOk, depPeriodOk;
    size_t limit = limitEdit->text().toUInt(&limitOk);
    size_t creditLimit = creditLimitEdit->text().toUInt(&creditLimitOk);
    size_t depPeriod = depPeriodEdit->text().toUInt(&depPeriodOk);

    double commission = commissionEdit->value();

    if (!limitOk || !creditLimitOk || !depPeriodOk) {
      QMessageBox::warning(this, "Input Error",
                           "Please enter valid numbers for all fields.");
      return;
    }

    Bank newBank(limit, creditLimit, commission, depPeriod, bankName);

    bankData.push_back(newBank);

    populateBanksTable();

    dialog.accept();
  });

  connect(cancelBtn, &QPushButton::clicked, &dialog, &QDialog::reject);
  dialog.exec();
}

void MainWindow::showDepositDialog() {
  QModelIndexList selectedIndexes =
      accountsTableView->selectionModel()->selectedRows();

  if (selectedIndexes.isEmpty()) {
    QMessageBox::warning(this, "Deposit",
                         "Please select an account to deposit.");
    return;
  }

  int row = selectedIndexes.first().row();
  QString accountType = accountsModel->item(row, 1)->text();

  if (accountType != "Deposit") {
    QMessageBox::warning(this, "Deposit",
                         "Deposits are only allowed on Deposit-type accounts.");
    return;
  }

  QDialog dialog(this);
  dialog.setWindowTitle("Deposit Money");

  QFormLayout *formLayout = new QFormLayout();
  QLineEdit *amountEdit = new QLineEdit(&dialog);
  formLayout->addRow("Deposit Amount:", amountEdit);

  QLabel *depositRulesLabel = new QLabel(
      "Deposit Rules:\n"
      "- Deposits can be made at any time.\n"
      "- Withdrawals are not allowed before account maturity.",
      &dialog);
  formLayout->addRow(depositRulesLabel);

  QHBoxLayout *buttonsLayout = new QHBoxLayout();
  QPushButton *depositBtn = new QPushButton("Deposit");
  QPushButton *cancelBtn = new QPushButton("Cancel");
  buttonsLayout->addWidget(depositBtn);
  buttonsLayout->addWidget(cancelBtn);

  QVBoxLayout *dialogLayout = new QVBoxLayout(&dialog);
  dialogLayout->addLayout(formLayout);
  dialogLayout->addLayout(buttonsLayout);
  dialog.setLayout(dialogLayout);

  connect(depositBtn, &QPushButton::clicked, [&]() {
    qDebug() << "Deposit button clicked";

    bool ok;
    double amount = amountEdit->text().toDouble(&ok);

    if (!ok || amount <= 0) {
      QMessageBox::warning(this, "Error", "Invalid deposit amount.");
      return;
    }

    QStandardItem *balanceItem = accountsModel->item(row, 3);
    double currentBalance = balanceItem->text().toDouble();

    balanceItem->setText(QString::number(currentBalance + amount));

    qDebug() << "New balance: " << currentBalance + amount;

    QMessageBox::information(this, "Deposit Successful",
                             QString("Deposited %1. New balance: %2")
                                 .arg(amount)
                                 .arg(currentBalance + amount));

    dialog.accept();
  });

  connect(cancelBtn, &QPushButton::clicked, &dialog, &QDialog::reject);

  dialog.exec();
}

void MainWindow::createBanksTab(QWidget *parent) {
  QVBoxLayout *layout = new QVBoxLayout(parent);

  QPushButton *addBankButton = new QPushButton("Add Bank", this);
  connect(addBankButton, &QPushButton::clicked, this,
          &MainWindow::showAddBankDialog);

  banksModel = new QStandardItemModel(0, 2, this);
  banksModel->setHeaderData(0, Qt::Horizontal, "Bank ID");
  banksModel->setHeaderData(1, Qt::Horizontal, "Bank Name");

  banksTableView = new QTableView(this);
  banksTableView->setModel(banksModel);
  banksTableView->horizontalHeader()->setStretchLastSection(true);

  layout->addWidget(addBankButton);
  layout->addWidget(banksTableView);
  parent->setLayout(layout);
}

void MainWindow::updateClientAccountCount() {
  std::map<QString, int> accountCountMap;

  for (int row = 0; row < accountsModel->rowCount(); ++row) {
    QString clientName = accountsModel->item(row, 2)->text();
    accountCountMap[clientName]++;
  }

  for (int row = 0; row < clientsModel->rowCount(); ++row) {
    QString clientName = clientsModel->item(row, 0)->text();
    QString clientSurname = clientsModel->item(row, 1)->text();
    QString fullName = clientName + " " + clientSurname;

    QStandardItem *accountsItem = clientsModel->item(row, 4);
    if (accountsItem) {
      accountsItem->setText(QString::number(accountCountMap[fullName]));
    }
  }
}

void MainWindow::populateClientsTable() {
  clientsModel->setRowCount(0);
  for (const auto &pair : userInitMap) {
    const User &user = pair.second;
    QList<QStandardItem *> row;

    row.append(new QStandardItem(QString::fromStdString(user.client.name)));

    row.append(new QStandardItem(QString::fromStdString(user.client.surname)));

    row.append(new QStandardItem(
        user.WasAdressed ? QString::fromStdString(user.address) : "N/A"));

    row.append(new QStandardItem(user.HasId ? QString::number(user.passport_id)
                                            : "N/A"));

    row.append(new QStandardItem("0"));

    row.append(new QStandardItem(user.HasId ? "Confirmed" : "Incomplete"));

    clientsModel->appendRow(row);
  }

  updateClientAccountCount();
}

void MainWindow::populateBanksTable() {
  banksModel->clear();

  banksModel->setHorizontalHeaderLabels({"Bank Name", "User Limit",
                                         "Credit Limit", "Commission",
                                         "Deposit Period"});

  for (const Bank &bank : bankData) {
    QList<QStandardItem *> rowItems;
    rowItems.append(new QStandardItem(QString::fromStdString(bank.bank_name)));
    rowItems.append(
        new QStandardItem(QString::number(bank.unidentified_user_limit)));
    rowItems.append(
        new QStandardItem(QString::number(bank.default_credit.credit_limit)));
    rowItems.append(new QStandardItem(
        QString::number(bank.default_credit.commission) + "%"));
    rowItems.append(
        new QStandardItem(QString::number(bank.default_deposit.period)));
    banksModel->appendRow(rowItems);
  }

  banksTableView->resizeColumnsToContents();
}
