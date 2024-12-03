#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QAction>
#include <QComboBox>
#include <QDialog>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMainWindow>
#include <QMenuBar>
#include <QMessageBox>
#include <QPushButton>
#include <QStandardItemModel>
#include <QTabWidget>
#include <QTableView>
#include <QVBoxLayout>
#include <map>
#include <vector>

#include "../headers/bank.h"
#include "../headers/user_data.h"

class MainWindow : public QMainWindow {
  Q_OBJECT

 public:
  MainWindow(QWidget *parent = nullptr);
  ~MainWindow() = default;

  Bank *GetRootBank();

 private:
  QStandardItemModel *clientsModel;
  QStandardItemModel *accountsModel;
  QStandardItemModel *transactionsModel;
  QStandardItemModel *banksModel;

  QTableView *clientsTableView;
  QTableView *accountsTableView;
  QTableView *transactionsTableView;
  QTableView *banksTableView;

  std::vector<Bank> bankData;
  std::map<UserName, User> userInitMap;

  Bank *currentBank;

  void createMenuBar();
  void createClientsTab(QWidget *parent);
  void createAccountsTab(QWidget *parent);
  void createTransactionsTab(QWidget *parent);
  void createBanksTab(QWidget *parent);

  void showAddClientDialog();
  void showAddAccountDialog();
  void showAddBankDialog();
  void showAddTransactionDialog();

  void populateClientsTable();
  void updateClientAccountCount();
  void populateAccountsTable();
  void populateTransactionsTable();
  void populateBanksTable();

  void showEditClientDialog();
  void deleteClient();
  void showDepositDialog();
  void showWithdrawDialog();

  void deleteAccount();
  void deleteBank();
  void showTransferDialog();
};

#endif