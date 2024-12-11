#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QSqlRecord>
#include <QMessageBox>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QDateTimeAxis>
#include <QtCharts/QValueAxis>
#include <QPrinter>
#include <QPrintDialog>
#include <QPainter>
#include <QFileDialog>



MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->menubar->hide();
    ui->tabWidget->setStyleSheet("QTabBar::tab {width: 0; height: 0; margin: 0; padding: 0; border: none;} ");
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_label_3_clicked()
{
    if (ui->label_3->text() == "Sign up!")
    {
        ui->label->setText("Sign up with new Username");
        ui->pushButton->setText("Sign up");
        ui->label_2->setText("Already have an account?");
        ui->label_3->setText("Sign in!");
    }
    else if (ui->label_3->text() == "Sign in!")
    {
        ui->label->setText("Sign in with Username");
        ui->pushButton->setText("Sign in");
        ui->label_2->setText("Don't have an account?");
        ui->label_3->setText("Sign up!");
    }
}

void MainWindow::on_pushButton_clicked()
{
    try
    {
        if (ui->pushButton->text() == "Sign in")
            client = userManager.signIn(ui->lineEdit->text(), ui->lineEdit_2->text());
        else if (ui->pushButton->text() == "Sign up")
            client = userManager.signUp(ui->lineEdit->text(), ui->lineEdit_2->text());

        ui->tabWidget->setTabEnabled(0, false);
        ui->tabWidget->setStyleSheet("");
        ui->tabWidget->setStyleSheet("QTabBar::tab::disabled {width: 0; height: 0; margin: 0; padding: 0; border: none;} ");
        ui->menubar->show();
        ui->menuUSERNAME->setTitle(ui->lineEdit->text());
    }
    catch (std::exception &e) 
    {
        ui->label_4->setText(e.what());
        client = Role();
    }
}

void MainWindow::on_tabWidget_currentChanged(int index)
{
    switch (index) 
    {
    case 1:
        this->updateWarehouseTables();
        break;
    case 2:
        this->updateGoodsTable();
        break;
    case 3:
        this->updateSalesTable();
        break;
    case 4:
        this->fullOutComboBox();
        break;
    default:
        break;
    }
}

void MainWindow::on_tableWidget_2_cellChanged(int row, int column)
{
    if (column != 0)
    {
        QString id = ui->tableWidget_2->item(row, 0)->text();
        QString newValue = ui->tableWidget_2->item(row, column)->text();
        QString columnName;

        switch (column) 
        {
        case 1:
            columnName = "name";
            break;
        case 2:
            columnName = "priority";
            break;
        default:
            break;
        }

        controller.modifyGoods(id, newValue, columnName);
    }
}

void MainWindow::on_tableWidget_3_cellChanged(int row, int column)
{
    if (column != 0)
    {
        QString id = ui->tableWidget_3->item(row, 0)->text();
        QString newValue = ui->tableWidget_3->item(row, column)->text();
        QString columnName;

        switch (column) 
        {
        case 1:
            columnName = "good_id";
            break;
        case 2:
            columnName = "good_count";
            break;
        case 3:
            columnName = "create_date";
            break;
        default:
            break;
        }

        controller.modifySells(id, newValue, columnName);
    }
}

void MainWindow::on_pushButton_2_clicked()
{
    if (client.getRights() == 0)
    {
        QMessageBox messageBox;
        messageBox.critical(0,"Error","Client doesn't have rights to add new elements");
        messageBox.setFixedSize(500,200);
    }
    else
    {
        try
        {
            auto *item1 = ui->tableWidget_5->item(0, 0);
            auto *item2 = ui->tableWidget_5->item(0, 1);
            if (item1 == nullptr || item2 == nullptr)
                throw std::exception("Table widget item is nullptr");

            QString name = item1->text();
            QString priority = item2->text();
            controller.addGood(name, priority.toDouble());
            ui->label_7->setText("");
            this->updateGoodsTable();
        }
        catch(std::runtime_error &e)
        {
            qDebug() << e.what();
            ui->label_7->setText("There is imposible to add this sale");
        }
        catch(std::exception &e)
        {
            qDebug() << e.what();
            ui->label_7->setText("To add new good you have to fill in all the values");
        }
    }
}


void MainWindow::on_pushButton_3_clicked()
{
    if (client.getRights() == 0)
    {
        QMessageBox messageBox;
        messageBox.critical(0,"Error","Client doesn't have rights to add new elements");
        messageBox.setFixedSize(500,200);
    }
    else
    {
        try
        {
            auto *item1 = ui->tableWidget_6->item(0, 0);
            auto *item2 = ui->tableWidget_6->item(0, 1);
            auto *item3 = ui->tableWidget_6->item(0, 2);
            if (item1 == nullptr || item2 == nullptr || item3 == nullptr)
                throw std::exception("Table widget item is nullptr");

            QString good_id = item1->text();
            QString good_count = item2->text();
            QString create_date = item3->text();
            controller.addSale(good_id.toInt(), good_count.toInt(), create_date);
            ui->label_8->setText("");
            this->updateSalesTable();
        }
        catch(std::runtime_error &e)
        {
            qDebug() << e.what();
            ui->label_8->setText("There is imposible to add this good");
        }
        catch(std::exception &e)
        {
            qDebug() << e.what();
            ui->label_8->setText("To add new sale you have to fill in all the values");
        }
    }
}


void MainWindow::on_pushButton_4_clicked()
{
    QSqlQuery queryResult;

    queryResult = controller.getMostPopularGoods();
    this->setTable(std::move(queryResult), ui->tableWidget_7);
}


void MainWindow::on_pushButton_5_clicked()
{
    try
    {
        QLayoutItem *item;
        while ((item = ui->verticalLayout->takeAt(0)) != nullptr)
        {
            delete item->widget();
            delete item;
        }

        QLineSeries *line = new QLineSeries();
        QString text = ui->comboBox->currentText();
        QString modifiedText = text.chopped(5);

        QSqlQuery queryResult = controller.getGraphTable(modifiedText);

        queryResult.first();

        if (queryResult.value(0) == NULL) throw(modifiedText.toStdString() + "doesn't have any sales");

        QString dateString = queryResult.value(1).toString();
        QDateTime startDate = QDateTime::fromString(dateString, "yyyy-mm-dd");

        int i = 0;
        do
        {
            i++;
            int yValue = queryResult.value(0).toInt();
            dateString = queryResult.value(1).toString();
            QDateTime dateTime = QDateTime::fromString(dateString, "yyyy-MM-dd");

            if (dateTime.isValid()) line->append(dateTime.toMSecsSinceEpoch(), yValue);
            else qDebug() << "Invalid date format: " << dateString;
        } while (queryResult.next());

        QDateTime endDate = QDateTime::fromString(dateString, "yyyy-mm-dd");

        if (startDate == endDate) throw std::runtime_error(modifiedText.toStdString() + "doesn't have enough sales");

        QChart *chart = new QChart();
        chart->addSeries(line);
        chart->legend()->hide();
        chart->setVisible(true);

        auto axisX = new QDateTimeAxis;
        axisX->setTickCount(i);
        axisX->rangeChanged(startDate, endDate);
        axisX->setFormat("yyyy-MM-dd");
        axisX->setTitleText("Date");
        chart->addAxis(axisX, Qt::AlignBottom);
        line->attachAxis(axisX);

        auto axisY = new QValueAxis;
        axisY->setTickCount(i);
        axisY->setLabelFormat("%i");
        axisY->setTitleText("Amount");
        chart->addAxis(axisY, Qt::AlignLeft);
        line->attachAxis(axisY);

        QChartView *chartView = new QChartView(chart);
        chartView->setRenderHint(QPainter::Antialiasing);

        ui->verticalLayout->addWidget(chartView);

        QString fileName = QFileDialog::getSaveFileName(this, "Report", "", "PDF Files (*.pdf)");
        if (!fileName.isEmpty())
        {
            QPrinter printer(QPrinter::HighResolution);
            printer.setOutputFormat(QPrinter::PdfFormat);
            printer.setOutputFileName(fileName);
            printer.setPageSize(QPageSize::A4);
            //printer.setOrientation(QPrinter::Portrait);

            QPainter painter(&printer);
            chartView->render(&painter);
            painter.end();
        }
    }
    catch (std::exception &e)
    {
        QMessageBox messageBox;
        qDebug() << e.what();
        messageBox.critical(0,"Error",e.what());
        messageBox.setFixedSize(500,200);
    }

}

void MainWindow::on_pushButton_6_clicked()
{
    try 
    {
        QString text = ui->comboBox->currentText();
        QString modifiedText = text.chopped(5);
        QString startDate = ui->dateEdit->text();
        QString endDate = ui->dateEdit_2->text();
        size_t changeAmount = controller.getSellsChange(modifiedText, startDate, endDate);

        ui->label_9->setText("Change over period: " + QString::number(changeAmount));
    }
    catch (std::exception &e)
    {
        qDebug() << e.what();
    }
}

void MainWindow::setTable(QSqlQuery&& queryResult, QTableWidget* tableWidget)
{
    try {
        tableWidget->setRowCount(0);
        int colCount = queryResult.record().count();
        
        if (tableWidget == ui->tableWidget_2 || tableWidget == ui->tableWidget_3)
            tableWidget->setColumnCount(colCount + 1);
        else
            tableWidget->setColumnCount(colCount);

        for (int col = 0; col < colCount; ++col)
            tableWidget->setHorizontalHeaderItem(col, new QTableWidgetItem(queryResult.record().fieldName(col)));

        if (tableWidget == ui->tableWidget || tableWidget == ui->tableWidget_4)
        {
            for (int col = 0; col < colCount; ++col)
                tableWidget->setColumnWidth(col, 70);
        }
        else if (tableWidget == ui->tableWidget_2)
        {
            tableWidget->setColumnWidth(0, 60);
            tableWidget->setColumnWidth(2, 60);
        }
        else if (tableWidget == ui->tableWidget_3)
        {
            for (int col = 0; col < colCount - 1; ++col)
                tableWidget->setColumnWidth(col, 60);
        }

        int row = 0;
        while (queryResult.next())
        {
            tableWidget->insertRow(row);
            for (int col = 0; col < colCount; ++col)
            {
                QString value = queryResult.value(col).toString();
                tableWidget->setItem(row, col, new QTableWidgetItem(value));
            }

            if (tableWidget == ui->tableWidget_2 || tableWidget == ui->tableWidget_3)
            {
                tableWidget->setHorizontalHeaderItem(colCount, new QTableWidgetItem(""));
                tableWidget->setColumnWidth(colCount, 40);

                QPushButton* button = new QPushButton("-");
                button->setFixedSize(30, 30);
                connect(button, &QPushButton::clicked, this, [this, tableWidget, row](){
                    this->deleteRow(tableWidget, row);
                });

                tableWidget->setCellWidget(row, colCount, button);
            }

            ++row;
        }

        if (this->client.getRights() == 0)
            tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    }
    catch (const std::runtime_error& e)
    {
        qDebug() << "Error: " << e.what();
    }
}


void MainWindow::updateWarehouseTables()
{
    QSqlQuery queryResult;

    queryResult = controller.getWarehouseStatistic(1);
    this->setTable(std::move(queryResult), ui->tableWidget);
    queryResult = controller.getWarehouseStatistic(2);
    ui->tableWidget_4->verticalHeader()->setVisible(false);
    this->setTable(std::move(queryResult), ui->tableWidget_4);
}

void MainWindow::updateGoodsTable()
{
    QSqlQuery queryResult;

    queryResult = controller.getGoods();
    this->setTable(std::move(queryResult), ui->tableWidget_2);
}

void MainWindow::updateSalesTable()
{
    QSqlQuery queryResult;

    queryResult = controller.getSales();
    this->setTable(std::move(queryResult), ui->tableWidget_3);
}

void MainWindow::fullOutComboBox()
{
    QSqlQuery queryResult;

    queryResult = controller.getGoods();

    while (queryResult.next())
    {
        ui->comboBox->addItem(queryResult.value(1).toString() + " [#" + queryResult.value(0).toString() + "]");
    }
}

void MainWindow::deleteRow(QTableWidget* table, size_t row)
{
    if (client.getRights() == 0)
    {
        QMessageBox messageBox;
        messageBox.critical(0,"Error","Client doesn't have rights to delete");
        messageBox.setFixedSize(500,200);
    }
    else
    {
        if (table == ui->tableWidget_2)
        {
            controller.removeGood(table->item(row, 0)->text().toInt());
        }
        else if (table == ui->tableWidget_3)
        {
            controller.closeSale(table->item(row, 0)->text().toInt());
        }
        (*table).removeRow(row);
    }
}
