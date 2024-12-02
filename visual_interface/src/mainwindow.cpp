#include "../headers/mainwindow.h"

#include <QInputDialog>
#include <QMessageBox>

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
  createTransactionsTab(transactionsTab);
  createBanksTab(banksTab);

  tabWidget->addTab(clientsTab, "Clients");
  tabWidget->addTab(accountsTab, "Accounts");
  tabWidget->addTab(transactionsTab, "Transactions");
  tabWidget->addTab(banksTab, "Banks");

  mainLayout->addWidget(tabWidget);
  setCentralWidget(centralWidget);

  // createMenuBar();
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
  clientsModel = new QStandardItemModel(0, 7, this);
  clientsModel->setHorizontalHeaderLabels(
      {"Id", "Name", "Surname", "Address", "Passport", "Accounts", "Status"});
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
    // Validate input
    if (nameEdit->text().isEmpty() || surnameEdit->text().isEmpty()) {
      QMessageBox::warning(this, "Error", "Name and Surname are required!");
      return;
    }

    try {
      User newUser(nameEdit->text().toStdString(),
                   surnameEdit->text().toStdString());

      if (!addressEdit->text().isEmpty()) {
        newUser.GetAdress(addressEdit->text().toStdString());
      }

      if (!passportEdit->text().isEmpty()) {
        bool ok;
        size_t passportId = passportEdit->text().toULong(&ok);
        if (ok) {
          newUser.GetPassportId(passportId);
        }
      }

      UserName userName(newUser.client.name, newUser.client.surname);
      userInitMap[userName] = newUser;

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
  for (auto &pair : userInitMap) {
    if (pair.second.passport_id == userId) {
      user = &pair.second;
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
  if (user->HasId) {
    passportEdit->setText(QString::number(user->passport_id));
  }

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
      UserName updatedUserName(nameEdit->text().toStdString(),
                               surnameEdit->text().toStdString());

      userInitMap.erase(UserName(
          user->client.name, user->client.surname)); 

      User updatedUser(nameEdit->text().toStdString(),
                       surnameEdit->text().toStdString());

      if (!addressEdit->text().isEmpty()) {
        updatedUser.GetAdress(addressEdit->text().toStdString());
      }

      if (!passportEdit->text().isEmpty()) {
        bool ok;
        size_t passportId = passportEdit->text().toULong(&ok);
        if (ok) {
          updatedUser.GetPassportId(passportId);
        }
      }

      userInitMap[updatedUserName] = updatedUser;

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
    // Убедимся, что модель корректно инициализирована
    if (!clientsModel) {
        QMessageBox::critical(this, "Delete Client", "Clients model is not initialized.");
        return;
    }

    // Открываем окно для ввода номера паспорта
    bool ok;
    QString passport = QInputDialog::getText(this, "Delete Client",
                                             "Enter Passport Number:",
                                             QLineEdit::Normal, "", &ok);

    // Если пользователь нажал "Отмена" или не ввёл ничего
    if (!ok || passport.isEmpty()) {
        QMessageBox::information(this, "Delete Client", "Operation canceled.");
        return;
    }

    // Проверяем, есть ли клиент с таким паспортом в базе
    bool clientFound = false;
    for (int row = 0; row < clientsModel->rowCount(); ++row) {
        QString clientPassport = clientsModel->item(row, 0)->text(); // Предполагаем, что паспорт в 0-й колонке

        if (clientPassport == passport) {
            clientFound = true;

            // Подтверждение удаления
            QMessageBox::StandardButton reply;
            reply = QMessageBox::question(this, "Delete Client",
                                          "Are you sure you want to delete this client?",
                                          QMessageBox::Yes | QMessageBox::No);

            if (reply == QMessageBox::Yes) {
                // Удаляем клиента из базы данных (userInitMap)
                QString name = clientsModel->item(row, 1)->text();    // Имя в колонке 1
                QString surname = clientsModel->item(row, 2)->text(); // Фамилия в колонке 2

                UserName userToDelete(name.toStdString(), surname.toStdString());
                userInitMap.erase(userToDelete);

                // Удаляем строку из модели
                clientsModel->removeRow(row);

                // Сообщение об успешном удалении
                QMessageBox::information(this, "Delete Client",
                                         "Client deleted successfully.");
            } else {
                QMessageBox::information(this, "Delete Client",
                                         "Operation canceled.");
            }

            break; // Прерываем цикл, так как клиент найден
        }
    }

    // Если клиент с таким паспортом не найден
    if (!clientFound) {
        QMessageBox::warning(this, "Delete Client",
                             "Client with the specified passport number was not found.");
    }
}


void MainWindow::createAccountsTab(QWidget *parent) {
  QVBoxLayout *layout = new QVBoxLayout(parent);
  QHBoxLayout *buttonsLayout = new QHBoxLayout();
  QPushButton *addAccountBtn = new QPushButton("Add Account");
  QPushButton *depositBtn = new QPushButton("Deposit");
  QPushButton *withdrawBtn = new QPushButton("Withdraw");
  buttonsLayout->addWidget(addAccountBtn);
  buttonsLayout->addWidget(depositBtn);
  buttonsLayout->addWidget(withdrawBtn);

  accountsTableView = new QTableView();
  accountsModel = new QStandardItemModel(0, 6, this);
  accountsModel->setHorizontalHeaderLabels(
      {"ID", "Type", "Owner", "Balance", "Limit", "Status"});
  accountsTableView->setModel(accountsModel);

  connect(addAccountBtn, &QPushButton::clicked, this,
          &MainWindow::showAddAccountDialog);
  //   connect(depositBtn, &QPushButton::clicked, this,
  //           &MainWindow::showDepositDialog);
  //   connect(withdrawBtn, &QPushButton::clicked, this,
  //           &MainWindow::showWithdrawDialog);

  layout->addLayout(buttonsLayout);
  layout->addWidget(accountsTableView);
}

void MainWindow::createTransactionsTab(QWidget *parent) {
  QVBoxLayout *layout = new QVBoxLayout(parent);
  QHBoxLayout *buttonsLayout = new QHBoxLayout();
  QPushButton *addTransactionBtn = new QPushButton("Add Transaction");
  buttonsLayout->addWidget(addTransactionBtn);

  transactionsTableView = new QTableView();
  transactionsModel = new QStandardItemModel(0, 5, this);
  transactionsModel->setHorizontalHeaderLabels(
      {"ID", "Type", "Amount", "Date", "Description"});
  transactionsTableView->setModel(transactionsModel);

  connect(addTransactionBtn, &QPushButton::clicked, this,
          &MainWindow::showAddTransactionDialog);

  layout->addLayout(buttonsLayout);
  layout->addWidget(transactionsTableView);
}

void MainWindow::createBanksTab(QWidget *parent) {
  QVBoxLayout *layout = new QVBoxLayout(parent);
  QHBoxLayout *buttonsLayout = new QHBoxLayout();
  QPushButton *addBankBtn = new QPushButton("Add Bank");
  buttonsLayout->addWidget(addBankBtn);

  banksTableView = new QTableView();
  banksModel = new QStandardItemModel(0, 4, this);
  banksModel->setHorizontalHeaderLabels({"ID", "Name", "Address", "Phone"});
  banksTableView->setModel(banksModel);

  connect(addBankBtn, &QPushButton::clicked, this,
          &MainWindow::showAddBankDialog);

  layout->addLayout(buttonsLayout);
  layout->addWidget(banksTableView);
}

void MainWindow::showDepositDialog() {
  QModelIndexList selectedIndexes =
      accountsTableView->selectionModel()->selectedRows();

  if (selectedIndexes.isEmpty()) {
    QMessageBox::warning(this, "Deposit",
                         "Please select an account to deposit.");
    return;
  }

  QDialog dialog(this);
  dialog.setWindowTitle("Deposit Money");

  QFormLayout *formLayout = new QFormLayout();
  QLineEdit *amountEdit = new QLineEdit(&dialog);
  formLayout->addRow("Deposit Amount:", amountEdit);

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
    bool ok;
    double amount = amountEdit->text().toDouble(&ok);

    if (!ok || amount <= 0) {
      QMessageBox::warning(this, "Error", "Invalid deposit amount.");
      return;
    }

    int row = selectedIndexes.first().row();
    QStandardItem *balanceItem = accountsModel->item(row, 3);
    double currentBalance = balanceItem->text().toDouble();

    balanceItem->setText(QString::number(currentBalance + amount));

    dialog.accept();
  });

  connect(cancelBtn, &QPushButton::clicked, &dialog, &QDialog::reject);

  dialog.exec();
}

void MainWindow::showWithdrawDialog() {
  QModelIndexList selectedIndexes =
      accountsTableView->selectionModel()->selectedRows();

  if (selectedIndexes.isEmpty()) {
    QMessageBox::warning(this, "Withdraw",
                         "Please select an account to withdraw from.");
    return;
  }

  QDialog dialog(this);
  dialog.setWindowTitle("Withdraw Money");

  QFormLayout *formLayout = new QFormLayout();
  QLineEdit *amountEdit = new QLineEdit(&dialog);
  formLayout->addRow("Withdraw Amount:", amountEdit);

  QHBoxLayout *buttonsLayout = new QHBoxLayout();
  QPushButton *withdrawBtn = new QPushButton("Withdraw");
  QPushButton *cancelBtn = new QPushButton("Cancel");
  buttonsLayout->addWidget(withdrawBtn);
  buttonsLayout->addWidget(cancelBtn);

  QVBoxLayout *dialogLayout = new QVBoxLayout(&dialog);
  dialogLayout->addLayout(formLayout);
  dialogLayout->addLayout(buttonsLayout);
  dialog.setLayout(dialogLayout);

  connect(withdrawBtn, &QPushButton::clicked, [&]() {
    bool ok;
    double amount = amountEdit->text().toDouble(&ok);

    if (!ok || amount <= 0) {
      QMessageBox::warning(this, "Error", "Invalid withdraw amount.");
      return;
    }

    int row = selectedIndexes.first().row();
    QStandardItem *balanceItem = accountsModel->item(row, 3);
    double currentBalance = balanceItem->text().toDouble();

    if (amount > currentBalance) {
      QMessageBox::warning(this, "Error", "Insufficient funds.");
      return;
    }

    balanceItem->setText(QString::number(currentBalance - amount));

    dialog.accept();
  });

  connect(cancelBtn, &QPushButton::clicked, &dialog, &QDialog::reject);

  dialog.exec();
}

void MainWindow::showAddAccountDialog() {
  QDialog dialog(this);
  dialog.setWindowTitle("Add New Account");

  QFormLayout *formLayout = new QFormLayout();

  QComboBox *accountTypeCombo = new QComboBox(&dialog);
  accountTypeCombo->addItems({"Debit", "Credit", "Deposit"});

  QComboBox *clientCombo = new QComboBox(&dialog);
  for (const auto &[name, user] : userInitMap) {
    clientCombo->addItem(
        QString::fromStdString(name.name + " " + name.surname));
  }

  QLineEdit *balanceEdit = new QLineEdit(&dialog);
  QLineEdit *limitEdit = new QLineEdit(&dialog);

  formLayout->addRow("Account Type:", accountTypeCombo);
  formLayout->addRow("Client:", clientCombo);
  formLayout->addRow("Initial Balance:", balanceEdit);
  formLayout->addRow("Credit Limit (if applicable):", limitEdit);

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

    try {
      bool balanceOk;
      double balance = balanceEdit->text().toDouble(&balanceOk);
      if (!balanceOk) {
        QMessageBox::warning(this, "Error", "Invalid balance amount!");
        return;
      }

      int row = accountsModel->rowCount();
      accountsModel->setItem(row, 0,
                             new QStandardItem(QString::number(row + 1)));
      accountsModel->setItem(
          row, 1, new QStandardItem(accountTypeCombo->currentText()));
      accountsModel->setItem(row, 2,
                             new QStandardItem(clientCombo->currentText()));
      accountsModel->setItem(row, 3,
                             new QStandardItem(QString::number(balance)));

      dialog.accept();
    } catch (const std::exception &e) {
      QMessageBox::critical(this, "Error",
                            QString("Failed to add account: %1").arg(e.what()));
    }
  });

  connect(cancelBtn, &QPushButton::clicked, &dialog, &QDialog::reject);
  dialog.exec();
}

void MainWindow::showAddTransactionDialog() {
  QMessageBox::information(this, "Add Transaction",
                           "Transaction dialog not implemented yet.");
}

void MainWindow::showAddBankDialog() {
  QMessageBox::information(this, "Add Bank",
                           "Bank dialog not implemented yet.");
}

void MainWindow::populateClientsTable() {
  clientsModel->setRowCount(0);  // Очистка таблицы перед добавлением новых данных

  for (const auto &pair : userInitMap) {
    const User &user = pair.second;
    QList<QStandardItem *> row;

    // ID
    row.append(new QStandardItem(QString::number(user.passport_id)));

    // Name
    row.append(new QStandardItem(QString::fromStdString(user.client.name)));

    // Surname
    row.append(new QStandardItem(QString::fromStdString(user.client.surname)));

    // Address
    row.append(new QStandardItem(
        user.WasAdressed ? QString::fromStdString(user.address) : "N/A"));

    // Passport
    row.append(new QStandardItem(
        user.HasId ? QString::number(user.passport_id) : "N/A"));

    // // Accounts (например, можно показывать количество счетов)
    // row.append(new QStandardItem(QString::number(user.accounts.size())));

    // // Status (например, активен или нет)
    // row.append(new QStandardItem(
    //     user.IsActive ? "Active" : "Inactive"));

    clientsModel->appendRow(row);
  }
}


void MainWindow::populateAccountsTable() {
  // Implement similar to populateClientsTable when you have the account data
  // structure
}

void MainWindow::populateTransactionsTable() {
  // Placeholder for populating transactions table
}

void MainWindow::populateBanksTable() {
  // Placeholder for populating banks table
}