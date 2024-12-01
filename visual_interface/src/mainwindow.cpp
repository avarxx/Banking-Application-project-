#include "../headers/mainwindow.h"
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    // Set up main window properties
    setWindowTitle("Banking System Management");
    resize(1200, 800);

    // Create central widget and main layout
    QWidget *centralWidget = new QWidget(this);
    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
    
    // Create tab widget
    QTabWidget *tabWidget = new QTabWidget();
    
    // Create tab widgets
    QWidget *clientsTab = new QWidget();
    QWidget *accountsTab = new QWidget();
    QWidget *transactionsTab = new QWidget();
    QWidget *banksTab = new QWidget();

    // Set up tabs
    createClientsTab(clientsTab);
    createAccountsTab(accountsTab);
    createTransactionsTab(transactionsTab);
    createBanksTab(banksTab);

    // Add tabs to tab widget
    tabWidget->addTab(clientsTab, "Clients");
    tabWidget->addTab(accountsTab, "Accounts");
    tabWidget->addTab(transactionsTab, "Transactions");
    tabWidget->addTab(banksTab, "Banks");

    // Set up main layout
    mainLayout->addWidget(tabWidget);
    setCentralWidget(centralWidget);

    // Create menu bar
    createMenuBar();
}

void MainWindow::createMenuBar() {
    // Create menu bar
    QMenuBar *menuBar = new QMenuBar(this);
    setMenuBar(menuBar);

    // User menu
    QMenu *userMenu = menuBar->addMenu("&User");
    QAction *loginAction = new QAction("Login", this);
    QAction *logoutAction = new QAction("Logout", this);
    userMenu->addAction(loginAction);
    userMenu->addAction(logoutAction);

    // Settings menu
    QMenu *settingsMenu = menuBar->addMenu("&Settings");
    QAction *preferencesAction = new QAction("Preferences", this);
    settingsMenu->addAction(preferencesAction);
}

void MainWindow::createClientsTab(QWidget *parent) {
    // Create layout for clients tab
    QVBoxLayout *layout = new QVBoxLayout(parent);

    QHBoxLayout *buttonsLayout = new QHBoxLayout();
    QPushButton *addClientBtn = new QPushButton("Add Client");
    QPushButton *editClientBtn = new QPushButton("Edit Client");
    QPushButton *deleteClientBtn = new QPushButton("Delete Client");
    buttonsLayout->addWidget(addClientBtn);
    buttonsLayout->addWidget(editClientBtn);
    buttonsLayout->addWidget(deleteClientBtn);

    // Create table view for clients
    clientsTableView = new QTableView();
    clientsModel = new QStandardItemModel(0, 7, this);
    clientsModel->setHorizontalHeaderLabels({"ID", "Name", "Surname", "Address", "Passport", "Accounts", "Status"});
    clientsTableView->setModel(clientsModel);

    // Connect add client button
    connect(addClientBtn, &QPushButton::clicked, this, &MainWindow::showAddClientDialog);
    connect(editClientBtn, &QPushButton::clicked, this, [this]() {
        QMessageBox::information(this, "Edit Client", "Edit client functionality not implemented yet.");
    });
    connect(deleteClientBtn, &QPushButton::clicked, this, [this]() {
        QMessageBox::information(this, "Delete Client", "Delete client functionality not implemented yet.");
    });

    // Add widgets to layout
    layout->addLayout(buttonsLayout);
    layout->addWidget(clientsTableView);
}

void MainWindow::createAccountsTab(QWidget *parent) {
    // Create layout for accounts tab
    QVBoxLayout *layout = new QVBoxLayout(parent);

    // Create buttons layout
    QHBoxLayout *buttonsLayout = new QHBoxLayout();
    QPushButton *addAccountBtn = new QPushButton("Add Account");
    QPushButton *depositBtn = new QPushButton("Deposit");
    QPushButton *withdrawBtn = new QPushButton("Withdraw");
    buttonsLayout->addWidget(addAccountBtn);
    buttonsLayout->addWidget(depositBtn);
    buttonsLayout->addWidget(withdrawBtn);

    // Create table view for accounts
    accountsTableView = new QTableView();
    accountsModel = new QStandardItemModel(0, 6, this);
    accountsModel->setHorizontalHeaderLabels({"ID", "Type", "Owner", "Balance", "Limit", "Status"});
    accountsTableView->setModel(accountsModel);

    // Connect buttons
    connect(addAccountBtn, &QPushButton::clicked, this, &MainWindow::showAddAccountDialog);
    connect(depositBtn, &QPushButton::clicked, this, [this]() {
        QMessageBox::information(this, "Deposit", "Deposit functionality not implemented yet.");
    });
    connect(withdrawBtn, &QPushButton::clicked, this, [this]() {
        QMessageBox::information(this, "Withdraw", "Withdraw functionality not implemented yet.");
    });

    // Add widgets to layout
    layout->addLayout(buttonsLayout);
    layout->addWidget(accountsTableView);
}

void MainWindow::createTransactionsTab(QWidget *parent) {
    // Create layout for transactions tab
    QVBoxLayout *layout = new QVBoxLayout(parent);

    // Create buttons layout
    QHBoxLayout *buttonsLayout = new QHBoxLayout();
    QPushButton *addTransactionBtn = new QPushButton("Add Transaction");
    buttonsLayout->addWidget(addTransactionBtn);

    // Create table view for transactions
    transactionsTableView = new QTableView();
    transactionsModel = new QStandardItemModel(0, 5, this);
    transactionsModel->setHorizontalHeaderLabels({"ID", "Type", "Amount", "Date", "Description"});
    transactionsTableView->setModel(transactionsModel);

    // Connect add transaction button
    connect(addTransactionBtn, &QPushButton::clicked, this, &MainWindow::showAddTransactionDialog);

    // Add widgets to layout
    layout->addLayout(buttonsLayout);
    layout->addWidget(transactionsTableView);
}

void MainWindow::createBanksTab(QWidget *parent) {
    // Create layout for banks tab
    QVBoxLayout *layout = new QVBoxLayout(parent);

    // Create buttons layout
    QHBoxLayout *buttonsLayout = new QHBoxLayout();
    QPushButton *addBankBtn = new QPushButton("Add Bank");
    buttonsLayout->addWidget(addBankBtn);

    // Create table view for banks
    banksTableView = new QTableView();
    banksModel = new QStandardItemModel(0, 4, this);
    banksModel->setHorizontalHeaderLabels({"ID", "Name", "Address", "Phone"});
    banksTableView->setModel(banksModel);

    // Connect add bank button
    connect(addBankBtn, &QPushButton::clicked, this, &MainWindow::showAddBankDialog);

    // Add widgets to layout
    layout->addLayout(buttonsLayout);
    layout->addWidget(banksTableView);
}

void MainWindow::showAddClientDialog() {
    // Create dialog for adding a new client
    QDialog dialog(this);
    dialog.setWindowTitle("Add New Client");

    // Create form layout
    QFormLayout *formLayout = new QFormLayout();

    // Create input fields
    QLineEdit *nameEdit = new QLineEdit(&dialog);
    QLineEdit *surnameEdit = new QLineEdit(&dialog);
    QLineEdit *addressEdit = new QLineEdit(&dialog);
    QLineEdit *passportEdit = new QLineEdit(&dialog);

    // Add fields to form layout
    formLayout->addRow("Name:", nameEdit);
    formLayout->addRow("Surname:", surnameEdit);
    formLayout->addRow("Address:", addressEdit);
    formLayout->addRow("Passport ID:", passportEdit);

    // Create buttons
    QHBoxLayout *buttonsLayout = new QHBoxLayout();
    QPushButton *saveBtn = new QPushButton("Save");
    QPushButton *cancelBtn = new QPushButton("Cancel");
    buttonsLayout->addWidget(saveBtn);
    buttonsLayout->addWidget(cancelBtn);

    // Create dialog layout
    QVBoxLayout *dialogLayout = new QVBoxLayout(&dialog);
    dialogLayout->addLayout(formLayout);
    dialogLayout->addLayout(buttonsLayout);

    dialog.setLayout(dialogLayout);

    // Connect save button
    connect(saveBtn, &QPushButton::clicked, [&]() {
        // Validate input
        if (nameEdit->text().isEmpty() || surnameEdit->text().isEmpty()) {
            QMessageBox::warning(this, "Error", "Name and Surname are required!");
            return;
        }

        // Create user (placeholder, adjust based on your User class implementation)
        try {
            User newUser(nameEdit->text().toStdString(), 
                         surnameEdit->text().toStdString());

            // Optional fields
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

            // Add to user map
            UserName userName(newUser.client.name, newUser.client.surname);
            userInitMap[userName] = newUser;

            // Update clients table
            populateClientsTable();

            // Close dialog
            dialog.accept();
        }
        catch (const std::exception& e) {
            QMessageBox::critical(this, "Error", QString("Failed to add client: %1").arg(e.what()));
        }
    });

    // Connect cancel button
    connect(cancelBtn, &QPushButton::clicked, &dialog, &QDialog::reject);

    // Show dialog
    dialog.exec();
}

void MainWindow::showAddAccountDialog() {
    // Create dialog for adding a new account
    QDialog dialog(this);
    dialog.setWindowTitle("Add New Account");

    // Create form layout
    QFormLayout *formLayout = new QFormLayout();

    // Create input fields
    QComboBox *accountTypeCombo = new QComboBox(&dialog);
    accountTypeCombo->addItems({"Debit", "Credit", "Deposit"});

    QComboBox *clientCombo = new QComboBox(&dialog);
    for (const auto& [name, user] : userInitMap) {
        clientCombo->addItem(QString::fromStdString(name.name + " " + name.surname));
    }

    QLineEdit *balanceEdit = new QLineEdit(&dialog);
    QLineEdit *limitEdit = new QLineEdit(&dialog);

    // Add fields to form layout
    formLayout->addRow("Account Type:", accountTypeCombo);
    formLayout->addRow("Client:", clientCombo);
    formLayout->addRow("Initial Balance:", balanceEdit);
    formLayout->addRow("Credit Limit (if applicable):", limitEdit);

    // Create buttons
    QHBoxLayout *buttonsLayout = new QHBoxLayout();
    QPushButton *saveBtn = new QPushButton("Save");
    QPushButton *cancelBtn = new QPushButton("Cancel");
    buttonsLayout->addWidget(saveBtn);
    buttonsLayout->addWidget(cancelBtn);

    // Create dialog layout
    QVBoxLayout *dialogLayout = new QVBoxLayout(&dialog);
    dialogLayout->addLayout(formLayout);
    dialogLayout->addLayout(buttonsLayout);

    dialog.setLayout(dialogLayout);

    // Connect save button
    connect(saveBtn, &QPushButton::clicked, [&]() {
        // Validate input
        if (clientCombo->currentIndex() == -1 || balanceEdit->text().isEmpty()) {
            QMessageBox::warning(this, "Error", "Client and Initial Balance are required!");
            return;
        }

        try {
            // Parse balance
            bool balanceOk;
            double balance = balanceEdit->text().toDouble(&balanceOk);
            if (!balanceOk) {
                QMessageBox::warning(this, "Error", "Invalid balance amount!");
                return;
            }

            // For now, just update the table
            int row = accountsModel->rowCount();
            accountsModel->setItem(row, 0, new QStandardItem(QString::number(row + 1)));
            accountsModel->setItem(row, 1, new QStandardItem(accountTypeCombo->currentText()));
            accountsModel->setItem(row, 2, new QStandardItem(clientCombo->currentText()));
            accountsModel->setItem(row, 3, new QStandardItem(QString::number(balance)));
            
            // Close dialog
            dialog.accept();
        }
        catch (const std::exception& e) {
            QMessageBox::critical(this, "Error", QString("Failed to add account: %1").arg(e.what()));
        }
    });

    // Connect cancel button
    connect(cancelBtn, &QPushButton::clicked, &dialog, &QDialog::reject);

    // Show dialog
    dialog.exec();
}

void MainWindow::showAddTransactionDialog() {
    QMessageBox::information(this, "Add Transaction", "Transaction dialog not implemented yet.");
}

void MainWindow::showAddBankDialog() {
    QMessageBox::information(this, "Add Bank", "Bank dialog not implemented yet.");
}

void MainWindow::populateClientsTable() {
    // Clear existing data
    clientsModel->clear();
    clientsModel->setHorizontalHeaderLabels({"ID", "Name", "Surname", "Address", "Passport", "Accounts", "Status"});

    // Populate table with users from userInitMap
    int row = 0;
    for (const auto& [name, user] : userInitMap) {
        clientsModel->setItem(row, 0, new QStandardItem(QString::number(row + 1)));
        clientsModel->setItem(row, 1, new QStandardItem(QString::fromStdString(name.name)));
        clientsModel->setItem(row, 2, new QStandardItem(QString::fromStdString(name.surname)));
        
        // Add additional fields if available
        // Modify these based on your actual User class implementation
        row++;
    }
}

void MainWindow::populateAccountsTable() {
    // Placeholder for populating accounts table
    // Implement similar to populateClientsTable when you have the account data structure
}

void MainWindow::populateTransactionsTable() {
    // Placeholder for populating transactions table
}

void MainWindow::populateBanksTable() {
    // Placeholder for populating banks table
}