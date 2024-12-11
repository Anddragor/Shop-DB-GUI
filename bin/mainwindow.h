#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTableWidget>

#include "UserManager.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_tabWidget_currentChanged(int index);

    //tab_1
    void on_label_3_clicked();
    void on_pushButton_clicked();

    //tab_3
    void on_tableWidget_2_cellChanged(int row, int column);
    void on_pushButton_2_clicked();

    //tab_4
    void on_pushButton_3_clicked();
    void on_pushButton_4_clicked();
    void on_tableWidget_3_cellChanged(int row, int column);

    //tab_5
    void on_pushButton_5_clicked();
    void on_pushButton_6_clicked();

private:
    Controller controller;
    Role client;
    UserManager userManager;
    Ui::MainWindow *ui;

    void setTable(QSqlQuery&& queryResult, QTableWidget* tableWidget);

    void updateWarehouseTables();
    void updateGoodsTable();
    void updateSalesTable();

    void fullOutComboBox();

    void deleteRow(QTableWidget* table, size_t row);
};
#endif // MAINWINDOW_H
