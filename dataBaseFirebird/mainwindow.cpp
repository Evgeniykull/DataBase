#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QActionGroup>
#include <QLabel>
#include <QSqlError>
#include <QSqlRecord>
#include <QSqlQuery>
#include <QByteArray>
#include <QSerialPortInfo>
#include <QThread>
#include <QDebug>

#include "adduserdialog.h"
#include "modeluser.h"
#include "utils/treadworker.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(ui->actExit, SIGNAL(triggered()), SLOT(close()));
    connect(ui->pbView, SIGNAL(clicked()), SLOT(onViewClick()));
    renderToolbar();
    ui->tabWidget->tabBar()->hide(); //для скрытия табов
    status_bar = new QLabel(this);    
    port_status = new QLabel(this);
    ui->statusBar->addWidget(status_bar); //для строки состояния
    ui->statusBar->addWidget(port_status); //для строки состояния

    connect(ui->pbConnect, SIGNAL(clicked(bool)), SLOT(onConnectClick()));
    connect(ui->actSave, SIGNAL(triggered(bool)), SLOT(saveUsers()));
    connect(ui->actionf, SIGNAL(triggered(bool)), SLOT(saveFuels()));

    //connect to device
    port = new QSerialPort(this);
    port_ = new Port();
    ui->verticalLayout->addWidget(port_);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::renderToolbar() {
    QActionGroup *menu_bar = new QActionGroup(this);
    menu_bar->addAction(ui->actUser);
    menu_bar->addAction(ui->actDirectory);
    menu_bar->addAction(ui->actConnect);
    menu_bar->setExclusive(true);
    ui->mainToolBar->addActions(menu_bar->actions());

    connect(ui->actUser, SIGNAL(triggered(bool)), SLOT(onActUserClick()));
    connect(ui->actDirectory, SIGNAL(triggered(bool)), SLOT(onActDictionaryClick()));
    connect(ui->actConnect, SIGNAL(triggered(bool)), SLOT(onActConnectClick()));

    connect(ui->pbUpdateUser, SIGNAL(clicked(bool)), SLOT(updateUsers()));
    connect(ui->pbAddUser, SIGNAL(clicked(bool)), SLOT(onAddUsersClick()));
    connect(ui->pbDeleteUser, SIGNAL(clicked(bool)), SLOT(deleteUsers()));
    connect(ui->pbEditUser, SIGNAL(clicked(bool)), SLOT(onEditUsersClick()));

    connect(ui->pbUpdateFuel, SIGNAL(clicked(bool)), SLOT(updateFuels()));
    connect(ui->pbAddFuel, SIGNAL(clicked(bool)), SLOT(addFuels()));
    connect(ui->pbDeleteFuel, SIGNAL(clicked(bool)), SLOT(deleteFuels()));
}

void MainWindow::onViewClick() {
    QString path_to_base = QFileDialog::getOpenFileName(this, "Выбор БД", QDir::currentPath());
    ui->lePath->setText(path_to_base);
}

void MainWindow::onActUserClick() {
    ui->tabWidget->setCurrentIndex(0);
}

void MainWindow::onActDictionaryClick() {
    ui->tabWidget->setCurrentIndex(1);
}

void MainWindow::onActConnectClick() {
    ui->tabWidget->setCurrentIndex(2);
}

void MainWindow::onConnectClick() {
    db = QSqlDatabase::addDatabase("QIBASE");
    db.setDatabaseName("F:/DataBase/TESTBASE.FDB"); //ui->lePath->text()
    db.setUserName("SYSDBA"); //ui->leUserName->text()
    db.setPassword("123"); //ui->lePassword->text()
    bool is_open = db.open();

    QString db_status = is_open ? "Подключено" : db.lastError().text();
    status_bar->setText(db_status);
}

void setTableFormat(QTableView *tab)
{
    tab->setAlternatingRowColors(true);         // Подсветка строк разными цветами
    tab->resizeRowsToContents();
    tab->resizeColumnsToContents();
    tab->setSortingEnabled(true);               // Сортировка таблицы
    tab->sortByColumn(1,Qt::AscendingOrder);    // Порядок сортировки по умолчанию
    tab->setColumnHidden(0, true);              // Скрываем колонку с индексом
}

void MainWindow::updateUsers() {
    //пока не требуется для отладки
    /*model_users = new QSqlRelationalTableModel(0, db);
    model_users->setTable("users");
    model_users->setEditStrategy(QSqlTableModel::OnManualSubmit);
    model_users->select();
    ui->tableUser->setModel(model_users);
    setTableFormat(ui->tableUser);

    model = new TreeModel(*model_users, ui->teUserInfo);
    ui->treeView->setModel(model);
    connect(ui->treeView, SIGNAL(clicked(QModelIndex)), model, SLOT(Clicked(QModelIndex)));

    ui->teUserInfo->clear();*/

    getUserIndex();
}

void MainWindow::onAddUsersClick() {
    ModelUser *user_model = new ModelUser(db);
    connect(user_model, SIGNAL(needUpdate()), SLOT(updateUsers()));
    user_model->addUser();
}

void MainWindow::onEditUsersClick() {
    int current_row_num = ui->tableUser->currentIndex().row();
    if (current_row_num < 0) {
        return;
    }

    ModelUser *user_model = new ModelUser(db);
    int userid = model_users->record(current_row_num).value("userid").toInt();
    connect(user_model, SIGNAL(needUpdate()), SLOT(updateUsers()));
    user_model->editUsers(userid);
}

//удаление происходит сразу
void MainWindow::deleteUsers()
{
    int current_row_num = ui->tableUser->currentIndex().row();
    if (current_row_num < 0) {
        return;
    }

    ModelUser *user_model = new ModelUser(db);
    connect(user_model, SIGNAL(needUpdate()), SLOT(updateUsers()));
    int userid = model_users->record(current_row_num).value("userid").toInt();
    user_model->deleteUsers(userid);
}

void MainWindow::updateFuels() {
    model_fuels = new QSqlRelationalTableModel(0, db);
    model_fuels->setTable("fuels");
    model_fuels->setEditStrategy(QSqlTableModel::OnManualSubmit);
    model_fuels->select();
    ui->tableFuel->setModel(model_fuels);
    setTableFormat(ui->tableFuel);
}

void MainWindow::addFuels() {
    model_fuels->insertRow(model_fuels->rowCount());
}

void MainWindow::deleteFuels() {
    QModelIndex index = ui->tableFuel->currentIndex();
    model_fuels->removeRow(index.row());
}

void MainWindow::saveFuels()
{
    model_fuels->database().transaction();
    if(model_fuels->submitAll()) {
        model_fuels->database().commit();
    } else {
        qDebug() << model_fuels->lastError();
        model_fuels->database().rollback();
        model_fuels->revertAll();
    }
}


void MainWindow::openPort()
{
    port->setPortName(SettingsPort.name);
    if(port->isOpen()) {
        return;
    }
    if (port->open(QIODevice::ReadWrite)) {
        if (port->setBaudRate(SettingsPort.baudRate)
                && port->setDataBits(SettingsPort.dataBits)
                && port->setParity(SettingsPort.parity)
                && port->setStopBits(SettingsPort.stopBits)
                && port->setFlowControl(SettingsPort.flowControl)) {
            if (port->isOpen()) {
                port_status->setText("Порт открыт");
            }
        } else {
            port->close();
            port_status->setText(port->errorString());
        }
    } else {
        port->close();
        port_status->setText(port->errorString());
    }
}

void MainWindow::closePort()
{
    if (port->isOpen()) {
        port->close();
    }
    port_status->setText("Port is closed");
}

//не нужная функция
void MainWindow::changePort(QString name)
{
    if (name.isEmpty()) return;
}


void MainWindow::getUserIndex() {
    /*openPort();
    if (!port->isOpen()) {
        ui->teUserInfo->setText("Port not open");
        return;
    }

    QThread *thread = new QThread;
    treadWorker *worker = new treadWorker();
    worker->setTransferParams("get UserIndex[0]",
                              port,
                              0,
                              //ui->leAddres->text().toInt(),
                              byteOnPackage
                             );

    connect(thread, SIGNAL(started()), worker, SLOT(transferData()));
    connect(worker, SIGNAL(finished(QByteArray)), this, SLOT(endTransmitData(QByteArray)));
    worker->moveToThread(thread);
    thread->start();*/
    //новый класс port
    port_->writeData("get UserIndex[0]");
}

#include "utils/jsonconvertor.h"
void MainWindow::endTransmitData(QByteArray data) {
    qDebug() << data;
    JsonConvertor *convertor = new JsonConvertor();
    ui->teUserInfo->setText(convertor->dataToJson(data));
    //port->close();
    //массив получил, теперь разобрать
}
