#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QActionGroup>
#include <QLabel>
#include <QSqlError>
#include <QSqlRecord>
#include <QSqlQuery>
#include <QByteArray>
#include <QDebug>

#include "adduserdialog.h"

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
    ui->statusBar->addWidget(status_bar); //для строки состояния

    connect(ui->pbConnect, SIGNAL(clicked(bool)), SLOT(onConnectClick()));
    connect(ui->actSave, SIGNAL(triggered(bool)), SLOT(saveUsers()));
    connect(ui->actionf, SIGNAL(triggered(bool)), SLOT(saveFuels()));
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
    model_users = new QSqlRelationalTableModel(0, db);
    model_users->setTable("users");
    model_users->setEditStrategy(QSqlTableModel::OnManualSubmit);
    model_users->select();
    ui->tableUser->setModel(model_users);
    setTableFormat(ui->tableUser);
    //qDebug() << model_users->record(model_users->rowCount()-1);
    /*for (int i=0; i<model_users->rowCount()-1; i++) {
        qDebug() << model_users->record(i).value("parentid");
    }*/
}

void MainWindow::onAddUsersClick() {
    addUserDialog * add_user = new addUserDialog(this);
    add_user->show();
    connect(add_user, SIGNAL(onOkClick(int,int,QString,QString,int)), SLOT(addUser(int,int,QString,QString,int)));
}

void MainWindow::addUser(int parentId, int cardId, QString name, QString shortName, int indexId) {
    QSqlQuery* query = new QSqlQuery(db);
    QString statament = "INSERT INTO users (parentId, shortname, viewname, cardid, indexid) VALUES (";
    statament.append(QString::number(parentId) + ", ");
    statament.append("'" + shortName + "', ");
    statament.append("'" + name + "', ");
    statament.append(QString::number(cardId) + ", ");
    statament.append(QString::number(indexId));
    statament.append(")");

    query->exec(statament);
    if (query->lastError().isValid()) {
        //добавить QMessage
        qDebug() << "AddUser:" << query->lastError();
    }

    updateUsers();
}

void MainWindow::onEditUsersClick() {
    int current_row_num = ui->tableUser->currentIndex().row();
    if (current_row_num < 0) {
        return;
    }
    int userid = model_users->record(current_row_num).value("userid").toInt();

    addUserDialog * add_user = new addUserDialog(userid, db, this);
    add_user->show();
    connect(add_user, SIGNAL(onOkClick(int, int,int,QString,QString,int)), SLOT(editUsers(int,int,int,QString,QString,int)));
}

void MainWindow::editUsers(int userId, int parentId, int cardId, QString name, QString shortName, int indexId) {
    QSqlQuery* query = new QSqlQuery(db);
    QString statament = QString("UPDATE users SET parentid=%1, shortname=%2, viewname=%3, cardid=%4, indexid=%5 WHERE userid=%6")
            .arg(QString::number(parentId))
            .arg("'" +  shortName + "'")
            .arg("'" + name + "'")
            .arg(QString::number(cardId))
            .arg(QString::number(indexId))
            .arg(QString::number(userId));

    query->exec(statament);
    if (query->lastError().isValid()) {
        qDebug() << "Error:" << query->lastError();
    }

    updateUsers();
}

//удаление происходит сразу
void MainWindow::deleteUsers()
{
    int current_row_num = ui->tableUser->currentIndex().row();
    if (current_row_num < 0) {
        return;
    }
    int userid = model_users->record(current_row_num).value("userid").toInt();

    QSqlQuery* query = new QSqlQuery(db);
    QString statament = "DELETE FROM users WHERE userid=";
    statament.append(QString::number(userid));

    query->exec(statament);
    if (query->lastError().isValid()) {
        qDebug() << "RemoveRow:" << query->lastError();
    }

    updateUsers();
}

void MainWindow::saveUsers()
{
    model_users->database().transaction();
    if(model_users->submitAll()) {
        model_users->database().commit();
    } else {
        qDebug() << model_users->lastError();
        model_users->database().rollback();
        model_users->revertAll();
    }
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
