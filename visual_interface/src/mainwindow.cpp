#include "../headers/mainwindow.h"

#include <QInputDialog>
#include <QMessageBox>
#include <QDebug> 
#include <QHeaderView> // Необходимо для работы QHeaderView
#include <QTableView>  // Необходимо для QTableView 
#include <QDoubleSpinBox>
#include <QPushButton>
#include <QVBoxLayout>
#include <QLabel>

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
    // Validate input
    if (nameEdit->text().isEmpty() || surnameEdit->text().isEmpty()) {
      QMessageBox::warning(this, "Error", "Name and Surname are required!");
      return;
    }

    // Validate passport - must be non-empty
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
        QString clientPassport = clientsModel->item(row, 3)->text(); // Предполагаем, что паспорт в 0-й колонке

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

  //   connect(addAccountBtn, &QPushButton::clicked, this,
  //           &MainWindow::showAddAccountDialog);
  //   connect(depositBtn, &QPushButton::clicked, this,
  //           &MainWindow::showDepositDialog);
  //   connect(withdrawBtn, &QPushButton::clicked, this,
  //           &MainWindow::showWithdrawDialog);

  layout->addLayout(buttonsLayout);
  layout->addWidget(accountsTableView);
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

    // Check if the account is a deposit account
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
        &dialog
    );
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

    // Connect the "Deposit" button action
    connect(depositBtn, &QPushButton::clicked, [&]() {
        qDebug() << "Deposit button clicked";  // Отладка для проверки, что обработчик работает

        bool ok;
        double amount = amountEdit->text().toDouble(&ok);

        if (!ok || amount <= 0) {
            QMessageBox::warning(this, "Error", "Invalid deposit amount.");
            return;
        }

        // Get the current balance and update it
        QStandardItem *balanceItem = accountsModel->item(row, 3);
        double currentBalance = balanceItem->text().toDouble();

        // Update the balance with the deposited amount
        balanceItem->setText(QString::number(currentBalance + amount));

        qDebug() << "New balance: " << currentBalance + amount;  // Отладка, чтобы проверить обновление

        QMessageBox::information(this, "Deposit Successful", 
                                 QString("Deposited %1. New balance: %2")
                                     .arg(amount)
                                     .arg(currentBalance + amount));

        dialog.accept();
    });

    // Connect the "Cancel" button action
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

// void MainWindow::showAddAccountDialog() {
//     QDialog dialog(this);
//     dialog.setWindowTitle("Add New Account");

//     QFormLayout *formLayout = new QFormLayout();

//     QComboBox *accountTypeCombo = new QComboBox(&dialog);
//     accountTypeCombo->addItems({"Debit", "Credit", "Deposit"});

//     QComboBox *clientCombo = new QComboBox(&dialog);
//     for (const auto &[name, user] : userInitMap) {
//         clientCombo->addItem(
//             QString::fromStdString(name.name + " " + name.surname));
//     }

//     QLineEdit *balanceEdit = new QLineEdit(&dialog);
//     QLineEdit *limitEdit = new QLineEdit(&dialog);
    
//     // Период для депозита
//     QLineEdit *periodEdit = new QLineEdit(&dialog);
//     periodEdit->setPlaceholderText("Enter deposit period in days");

//     formLayout->addRow("Account Type:", accountTypeCombo);
//     formLayout->addRow("Client:", clientCombo);
//     formLayout->addRow("Initial Balance:", balanceEdit);
//     formLayout->addRow("Credit Limit (if applicable):", limitEdit);
//     formLayout->addRow("Deposit Period (days):", periodEdit);  // добавили поле для периода

//     QHBoxLayout *buttonsLayout = new QHBoxLayout();
//     QPushButton *saveBtn = new QPushButton("Save");
//     QPushButton *cancelBtn = new QPushButton("Cancel");
//     buttonsLayout->addWidget(saveBtn);
//     buttonsLayout->addWidget(cancelBtn);

//     QVBoxLayout *dialogLayout = new QVBoxLayout(&dialog);
//     dialogLayout->addLayout(formLayout);
//     dialogLayout->addLayout(buttonsLayout);

//     dialog.setLayout(dialogLayout);

//     connect(saveBtn, &QPushButton::clicked, [&]() {
//         // Validate input
//         if (clientCombo->currentIndex() == -1 || balanceEdit->text().isEmpty()) {
//             QMessageBox::warning(this, "Error", 
//                 "Client and Initial Balance are required!");
//             return;
//         }

//         try {
//             // Validate balance
//             bool balanceOk;
//             double balance = balanceEdit->text().toDouble(&balanceOk);
//             if (!balanceOk) {
//                 QMessageBox::warning(this, "Error", "Invalid balance amount!");
//                 return;
//             }

//             // Get the selected client's name
//             QString selectedClient = clientCombo->currentText();

//             // Parse the client name
//             QStringList nameParts = selectedClient.split(" ");
//             if (nameParts.size() < 2) {
//                 QMessageBox::warning(this, "Error", "Invalid client name!");
//                 return;
//             }

//             QString clientName = nameParts[0];
//             QString clientSurname = nameParts[1];

//             // Optional: Add credit limit for credit accounts
//             QString limit = (accountTypeCombo->currentText() == "Credit" && !limitEdit->text().isEmpty()) 
//                             ? limitEdit->text() : "Is not Credit";

//             // Get the deposit period
//             size_t depositPeriod = 0;
//             if (accountTypeCombo->currentText() == "Deposit") {
//                 bool periodOk;
//                 depositPeriod = periodEdit->text().toUInt(&periodOk);
//                 if (!periodOk || depositPeriod <= 0) {
//                     QMessageBox::warning(this, "Error", "Invalid deposit period!");
//                     return;
//                 }
//             }

//             // Generate a unique account ID 
//             int row = accountsModel->rowCount();
//             QString accountId = QString::number(row + 1);

//             // Add full details to the accounts table
//             accountsModel->setItem(row, 0, new QStandardItem(accountId)); // ID
//             accountsModel->setItem(row, 1, new QStandardItem(accountTypeCombo->currentText())); // Type
//             accountsModel->setItem(row, 2, new QStandardItem(selectedClient)); // Owner
//             accountsModel->setItem(row, 3, new QStandardItem(QString::number(balance))); // Balance
//             accountsModel->setItem(row, 4, new QStandardItem(limit)); // Limit
//             accountsModel->setItem(row, 5, new QStandardItem("Active")); // Status

//             // Для депозитного счета создаем объект Deposit
//             if (accountTypeCombo->currentText() == "Deposit") {
//                 Bank* rootBank = GetRootBank();  // Предполагаем, что есть метод для получения rootBank
//                 Deposit* deposit = new Deposit(depositPeriod, rootBank); // Пример использования
//                 // Можно хранить deposit в аккаунте или использовать его для дальнейших операций
//             }

//             // Update the number of accounts for this client
//             updateClientAccountCount();

//             dialog.accept();
//         } catch (const std::exception &e) {
//             QMessageBox::critical(this, "Error",
//                 QString("Failed to add account: %1").arg(e.what()));
//         }
//     });

//     connect(cancelBtn, &QPushButton::clicked, &dialog, &QDialog::reject);
//     dialog.exec();
// }

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

    // Кнопка для добавления банка (сверху)
    QPushButton *addBankButton = new QPushButton("Add Bank", this);
    connect(addBankButton, &QPushButton::clicked, this, &MainWindow::showAddBankDialog);

    // Таблица для отображения банков
    banksModel = new QStandardItemModel(0, 2, this); // Колонки: ID, Name
    banksModel->setHeaderData(0, Qt::Horizontal, "Bank ID");
    banksModel->setHeaderData(1, Qt::Horizontal, "Bank Name");

    banksTableView = new QTableView(this);
    banksTableView->setModel(banksModel);
    banksTableView->horizontalHeader()->setStretchLastSection(true);

    // Добавляем сначала кнопку, потом таблицу
    layout->addWidget(addBankButton);
    layout->addWidget(banksTableView);

    parent->setLayout(layout);
}


void MainWindow::showAddTransactionDialog() {
  QMessageBox::information(this, "Add Transaction",
                           "Transaction dialog not implemented yet.");
}

void MainWindow::showAddBankDialog() {
    QDialog dialog(this);
    dialog.setWindowTitle("Add New Bank");

    QFormLayout *formLayout = new QFormLayout();

    // Поле для имени банка
    QLineEdit *bankNameEdit = new QLineEdit(&dialog);
    formLayout->addRow("Bank Name:", bankNameEdit);

    // Поле для лимита
    QLineEdit *limitEdit = new QLineEdit(&dialog);
    formLayout->addRow("Unidentified User Limit:", limitEdit);

    // Поле для кредитного лимита
    QLineEdit *creditLimitEdit = new QLineEdit(&dialog);
    formLayout->addRow("Credit Limit:", creditLimitEdit);

    // Поле для комиссии
    QDoubleSpinBox *commissionEdit = new QDoubleSpinBox(&dialog);
    formLayout->addRow("Commission:", commissionEdit);

    // Поле для периода депозита
    QLineEdit *depPeriodEdit = new QLineEdit(&dialog);
    formLayout->addRow("Deposit Period (days):", depPeriodEdit);

    // Кнопки для сохранения и отмены
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
    // Получаем данные из полей ввода
    std::string bankName = bankNameEdit->text().toStdString();
    bool limitOk, creditLimitOk, depPeriodOk;
    size_t limit = limitEdit->text().toUInt(&limitOk);
    size_t creditLimit = creditLimitEdit->text().toUInt(&creditLimitOk);
    size_t depPeriod = depPeriodEdit->text().toUInt(&depPeriodOk);

    // Получаем дробное значение комиссии напрямую через value()
    double commission = commissionEdit->value(); // QDoubleSpinBox::value() возвращает double

    // Проверяем, что все значения введены корректно
    if (!limitOk || !creditLimitOk || !depPeriodOk) {
        QMessageBox::warning(this, "Input Error", "Please enter valid numbers for all fields.");
        return;
    }

    // Создаем новый объект банка
    Bank newBank(limit, creditLimit, commission, depPeriod, bankName);

    // Добавляем банк в данные
    bankData.push_back(newBank);

    // Обновляем таблицу банков
    populateBanksTable();

    dialog.accept();
    });

    connect(cancelBtn, &QPushButton::clicked, &dialog, &QDialog::reject);
    dialog.exec();
}

void MainWindow::updateClientAccountCount() {
    // Create a map to count accounts per client
    std::map<QString, int> accountCountMap;

    // Count accounts for each client
    for (int row = 0; row < accountsModel->rowCount(); ++row) {
        QString clientName = accountsModel->item(row, 2)->text();
        accountCountMap[clientName]++;
    }

    // Update the clients table
    for (int row = 0; row < clientsModel->rowCount(); ++row) {
        QString clientName = clientsModel->item(row, 0)->text();
        QString clientSurname = clientsModel->item(row, 1)->text();
        QString fullName = clientName + " " + clientSurname;

        // Set the number of accounts
        QStandardItem* accountsItem = clientsModel->item(row, 4);
        if (accountsItem) {
            accountsItem->setText(QString::number(accountCountMap[fullName]));
        }
    }
}

void MainWindow::populateClientsTable() {
    clientsModel->setRowCount(0);  // Clear the table before adding new data

    for (const auto &pair : userInitMap) {
        const User &user = pair.second;
        QList<QStandardItem *> row;

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

        // Accounts (initially set to 0, will be updated)
        row.append(new QStandardItem("0"));

        // Status
        row.append(new QStandardItem(
            user.HasId ? "Confirmed" : "Incomplete"));

        clientsModel->appendRow(row);
    }

    // Update account count after populating the table
    updateClientAccountCount();
}

void MainWindow::populateTransactionsTable() {
  // Placeholder for populating transactions table
}

void MainWindow::populateBanksTable() {
    banksModel->clear();

    // Устанавливаем заголовки таблицы
    banksModel->setHorizontalHeaderLabels({"Bank Name", "User Limit", "Credit Limit", "Commission", "Deposit Period"});

    // Заполняем таблицу данными
    for (const Bank &bank : bankData) {
        QList<QStandardItem *> rowItems;
        rowItems.append(new QStandardItem(QString::fromStdString(bank.bank_name)));
        rowItems.append(new QStandardItem(QString::number(bank.unidentified_user_limit)));
        rowItems.append(new QStandardItem(QString::number(bank.default_credit.credit_limit)));
        rowItems.append(new QStandardItem(QString::number(bank.default_credit.commission) + "%"));
        rowItems.append(new QStandardItem(QString::number(bank.default_deposit.period)));
        banksModel->appendRow(rowItems);
    }

    banksTableView->resizeColumnsToContents();
}
