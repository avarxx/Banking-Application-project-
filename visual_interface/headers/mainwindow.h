#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QComboBox>
#include <QPushButton>
#include <QLineEdit>
#include <QFormLayout>
#include <QMessageBox> 
#include <QMainWindow>
#include <QTableView>
#include <QStandardItemModel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QTabWidget>
#include <QMenuBar>
#include <QAction>
#include <QDialog>
#include <vector>
#include <map>
#include "../headers/user_data.h"
#include "../headers/bank.h"

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow() = default;  // Add default destructor

private:
    // Models for different sections
    QStandardItemModel *clientsModel;
    QStandardItemModel *accountsModel;
    QStandardItemModel *transactionsModel;
    QStandardItemModel *banksModel;

    // Table views
    QTableView *clientsTableView;
    QTableView *accountsTableView;
    QTableView *transactionsTableView;
    QTableView *banksTableView;

    // Main data structures
    std::vector<Bank> bankData;
    std::map<UserName, User> userInitMap;

    // Setup methods
    void createMenuBar();
    void createClientsTab(QWidget *parent);
    void createAccountsTab(QWidget *parent);
    void createTransactionsTab(QWidget *parent);
    void createBanksTab(QWidget *parent);

    // Action methods
    void showAddClientDialog();
    void showAddAccountDialog();
    void showAddBankDialog();
    void showAddTransactionDialog();

    // Data population methods
    void populateClientsTable();
    void populateAccountsTable();
    void populateTransactionsTable();
    void populateBanksTable();
};

#endif // MAINWINDOW_H