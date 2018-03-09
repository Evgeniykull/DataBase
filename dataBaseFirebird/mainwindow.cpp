#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QActionGroup>
#include <QLabel>
#include <QSqlError>
#include <QSqlRecord>
#include <QSqlQuery>
#include <QByteArray>
#include <QThread>
#include <QDebug>

#include "dialogs/adduserdialog.h"
#include "models/modeluser.h"
#include "models/modelfuels.h"
#include "utils/treadworker.h"
#include "utils/jsonconvertor.h"

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
    port = new Port();
    ui->verticalLayout->addWidget(port);
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
    connect(ui->pbEditFuel, SIGNAL(clicked(bool)), SLOT(onEditFuelsClick()));

    connect(ui->pbUpdateTanks, SIGNAL(clicked(bool)), SLOT(updateTanks()));
    connect(ui->pbAddTanks, SIGNAL(clicked(bool)), SLOT(addTanks()));
    connect(ui->pbDeleteTanks, SIGNAL(clicked(bool)), SLOT(deleteTanks()));
    connect(ui->pbEditTanks, SIGNAL(clicked(bool)), SLOT(onEditTanksClick()));

    connect(ui->pbUpdatePoints, SIGNAL(clicked(bool)), SLOT(updatePoints()));
    connect(ui->pbAddPoints, SIGNAL(clicked(bool)), SLOT(addPoints()));
    connect(ui->pbDeletePoints, SIGNAL(clicked(bool)), SLOT(deletePoints()));
    connect(ui->pbEditPoints, SIGNAL(clicked(bool)), SLOT(onEditPointsClick()));

    connect(ui->actConfigurate, SIGNAL(triggered(bool)), SLOT(getFuelIndex()));
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
    tab->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
}

void MainWindow::updateUsers() {
    /*model_users = new QSqlRelationalTableModel(0, db);
    model_users->setTable("users");
    model_users->setEditStrategy(QSqlTableModel::OnManualSubmit);
    model_users->select();

    model = new TreeModel(*model_users, ui->teUserInfo);
    ui->treeView->setModel(model);
    connect(ui->treeView, SIGNAL(clicked(QModelIndex)), model, SLOT(Clicked(QModelIndex)));
    connect(ui->treeView, SIGNAL(clicked(QModelIndex)), SLOT(onUserClick(QModelIndex)));

    ui->teUserInfo->clear();*/

    //работа с устройством
    getUserIndex();
}

void MainWindow::onAddUsersClick() {
    ModelUser *user_model = new ModelUser(db);
    connect(user_model, SIGNAL(needUpdate()), SLOT(updateUsers()));
    user_model->addUser();
}

void MainWindow::onEditUsersClick() {
    if (ui->treeView->currentIndex().row() < 0) {
        return;
    }

    int userid = ui->teUserInfo->toPlainText().split("\n").at(0).split(":")[1].toInt();
    ModelUser *user_model = new ModelUser(db);
    connect(user_model, SIGNAL(needUpdate()), SLOT(updateUsers()));
    user_model->editUsers(userid);
}

//удаление происходит сразу
//TODO добавить подтверждение
void MainWindow::deleteUsers()
{
    if (ui->treeView->currentIndex().row() < 0) {
        return;
    }

    int userid = ui->teUserInfo->toPlainText().split("\n").at(0).split(":")[1].toInt();
    ModelUser *user_model = new ModelUser(db);
    connect(user_model, SIGNAL(needUpdate()), SLOT(updateUsers()));
    user_model->deleteUsers(userid);
}

void MainWindow::updateFuels() {
    model_fuels = new QSqlRelationalTableModel(0, db);
    model_fuels->setTable("fuels");
    model_fuels->setEditStrategy(QSqlTableModel::OnManualSubmit);
    model_fuels->select();
    //model_fuels->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableFuel->setModel(model_fuels);
    setTableFormat(ui->tableFuel);
}

void MainWindow::addFuels() {
    ModelFuels *fuel_model = new ModelFuels(db);
    connect(fuel_model, SIGNAL(needUpdate()), SLOT(updateFuels()));
    fuel_model->addFuels();
}

void MainWindow::deleteFuels() {
    QModelIndex iid = ui->tableFuel->currentIndex();
    //работает, но!
    //r было приватным
    int idx = iid.r;
    if (idx < 0) {
        return;
    }

    ModelFuels *fuel_model = new ModelFuels(db);
    connect(fuel_model, SIGNAL(needUpdate()), SLOT(updateFuels()));
    //а если рекорд достать через QModelIndex?
    int fuelid = model_fuels->record(idx).value("fueldid").toInt();
    fuel_model->deleteFuels(fuelid);
}

void MainWindow::onEditFuelsClick() {
    int idx = ui->tableFuel->currentIndex().r;
    if (idx < 0) {
        return;
    }

    ModelFuels *fuel_model = new ModelFuels(db);
    connect(fuel_model, SIGNAL(needUpdate()), SLOT(updateFuels()));
    int fuelid = model_fuels->record(idx).value("fueldid").toInt();
    fuel_model->editFuels(fuelid);
}

void MainWindow::updateTanks() {
    model_tanks = new QSqlRelationalTableModel(0, db);
    model_tanks->setTable("tanks");
    model_tanks->setEditStrategy(QSqlTableModel::OnManualSubmit);
    model_tanks->select();
    ui->tableTanks->setModel(model_tanks);
    setTableFormat(ui->tableTanks);
}

#include "models/modeltanks.h"
void MainWindow::addTanks() {
    ModelTanks *tank_model = new ModelTanks(db);
    connect(tank_model, SIGNAL(needUpdate()), SLOT(updateTanks()));
    tank_model->addTanks();
}

void MainWindow::deleteTanks() {
    QModelIndex iid = ui->tableTanks->currentIndex();
    //работает, но!
    //r было приватным
    int idx = iid.r;
    if (idx < 0) {
        return;
    }

    ModelTanks *tanks_model = new ModelTanks(db);
    connect(tanks_model, SIGNAL(needUpdate()), SLOT(updateTanks()));
    //а если рекорд достать через QModelIndex?
    int tankid = model_tanks->record(idx).value("id").toInt();
    tanks_model->deleteTanks(tankid);
}

void MainWindow::onEditTanksClick() {
    int idx = ui->tableTanks->currentIndex().r;
    if (idx < 0) {
        return;
    }

    ModelTanks *tanks_model = new ModelTanks(db);
    connect(tanks_model, SIGNAL(needUpdate()), SLOT(updateTanks()));
    int tankid = model_tanks->record(idx).value("id").toInt();
    tanks_model->editTanks(tankid);
}

void MainWindow::updatePoints() {
    model_points = new QSqlRelationalTableModel(0, db);
    model_points->setTable("points");
    model_points->setEditStrategy(QSqlTableModel::OnManualSubmit);
    model_points->select();
    ui->tablePoints->setModel(model_points);
    setTableFormat(ui->tablePoints);
}

#include "models/modelpoints.h"
void MainWindow::addPoints() {
    ModelPoints *point_model = new ModelPoints(db);
    connect(point_model, SIGNAL(needUpdate()), SLOT(updatePoints()));
    point_model->addPoint();
}

void MainWindow::deletePoints() {
    QModelIndex iid = ui->tablePoints->currentIndex();
    int idx = iid.r;
    if (idx < 0) {
        return;
    }

    ModelPoints *point_model = new ModelPoints(db);
    connect(point_model, SIGNAL(needUpdate()), SLOT(updatePoints()));
    int pointid = model_points->record(idx).value("id").toInt();
    point_model->deletePoint(pointid);
}

void MainWindow::onEditPointsClick() {
    int idx = ui->tablePoints->currentIndex().r;
    if (idx < 0) {
        return;
    }

    ModelPoints *point_model = new ModelPoints(db);
    connect(point_model, SIGNAL(needUpdate()), SLOT(updatePoints()));
    int pointid = model_points->record(idx).value("id").toInt();
    point_model->editPoint(pointid);
}

//по идее нужен только set
void MainWindow::getFuelIndex() {
    //конфигурация топлива
    /*ModelFuels *fuel_model = new ModelFuels(db);
    QList<QString> data = fuel_model->configureFuels();

    if (data.isEmpty()) {
        qDebug() << "BD not connected!";
        return;
    }

    bool ok = port->changeAccess(1);
    if (ok) {
        for(int i=0; i<data.length(); i++){
            QByteArray conf_answ = port->writeData(data.at(i).toUtf8());
            if (conf_answ.indexOf("OK") < 0) {
                qDebug() << "error on configurate fuel #" << i;
            }
        }
    }
    port->changeAccess(0);*/

    //ui->textEdit->setText(port->writeData("get Установки.Резервуары[0]"));
    ui->textEdit->setText(port->writeData("get Установки.Колонки[0]"));


    /*QByteArray query = "get Установки.ВидыТоплива";
    QByteArray answ = port->writeData(query);
    int arr_start, arr_len;
    parseArray(answ, arr_start, arr_len);
    QVector<int> *indexes = getDataByLoop(query, arr_start, arr_len, "Топливо");
    qDebug() << indexes;*/
}

void MainWindow::getUserIndex() {
    //qDebug() << port->writeData("get UserIndex[0]");
    //qDebug() << port->writeData("get Установки.ВидыТоплива[0]");
    QByteArray query = "get UserIndex";
    QByteArray answ = port->writeData(query);
    int arr_start, arr_len;
    parseArray(answ, arr_start, arr_len);
    //цикл от start с перебором всех
    QVector<int> *indexes = getDataByLoop(query, arr_start, arr_len, "UserID");
    QMap<int, QJsonObject> *device_users = getUsersOnDevice(*indexes);
    //как получили map, проходимся по его ключам, сопоставляем json и записи из БД
    QList<int> keys = device_users->keys();
    for (int i = 0; i < keys.length(); i++) {
        //обращение к базе с get userid=keys[i]
        qDebug() << QString::number(keys[i]);
        QSqlQuery* query = new QSqlQuery(db);
        QString statament = QString("SELECT * FROM users WHERE userid=%1").arg(QString::number(keys[i]));
        query->exec(statament);
        query->next();
        QString answ_from_base = query->value(0).toString();
        qDebug() << answ_from_base;
        //если пусто, если нет...
    }
}

void mid_function() {
    //TODO
    //значение лучше из файла брать
    QString empty_user = "{\"UserID\":2,\"ParentID\":0,\"ViewName\":\"User2\",\"Flags\":0,\"CardID\":0,\"Limits[]\":[{\"fID\":0,\"Tp\":0,\"Dy\":0,\"Dt\":\"00.00.00 \",\"Vl\":0},{\"fID\":0,\"Tp\":0,\"Dy\":0,\"Dt\":\"00.00.00 \",\"Vl\":0},{\"fID\":0,\"Tp\":0,\"Dy\":0,\"Dt\":\"00.00.00 \",\"Vl\":0},{\"fID\":0,\"Tp\":0,\"Dy\":0,\"Dt\":\"00.00.00 \",\"Vl\":0},{\"fID\":0,\"Tp\":0,\"Dy\":0,\"Dt\":\"00.00.00 \",\"Vl\":0},{\"fID\":0,\"Tp\":0,\"Dy\":0,\"Dt\":\"00.00.00 \",\"Vl\":0},{\"fID\":0,\"Tp\":0,\"Dy\":0,\"Dt\":\"00.00.00 \",\"Vl\":0},{\"fID\":0,\"Tp\":0,\"Dy\":0,\"Dt\":\"00.00.00 \",\"Vl\":0}],\"СLimits[]\":[{\"fID\":0,\"Tp\":0,\"Dy\":0,\"Dt\":\"00.00.00 \",\"Vl\":0,\"Cn\":0},{\"fID\":0,\"Tp\":0,\"Dy\":0,\"Dt\":\"00.00.00 \",\"Vl\":0,\"Cn\":0},{\"fID\":0,\"Tp\":0,\"Dy\":0,\"Dt\":\"00.00.00 \",\"Vl\":0,\"Cn\":0},{\"fID\":0,\"Tp\":0,\"Dy\":0,\"Dt\":\"00.00.00 \",\"Vl\":0,\"Cn\":0},{\"fID\":0,\"Tp\":0,\"Dy\":0,\"Dt\":\"00.00.00 \",\"Vl\":0,\"Cn\":0},{\"fID\":0,\"Tp\":0,\"Dy\":0,\"Dt\":\"00.00.00 \",\"Vl\":0,\"Cn\":0},{\"fID\":0,\"Tp\":0,\"Dy\":0,\"Dt\":\"00.00.00 \",\"Vl\":0,\"Cn\":0},{\"fID\":0,\"Tp\":0,\"Dy\":0,\"Dt\":\"00.00.00 \",\"Vl\":{\"type\":\"ERROR\"},\"Cn\":0}]}";

}


#include <QJsonDocument>
#include <QJsonObject>
void MainWindow::parseArray(QByteArray dataArr, int &start, int &len) {
    JsonConvertor *conv = new JsonConvertor();
    QString json = conv->dataToJson(dataArr);
    QJsonDocument jsonDoc = QJsonDocument::fromJson(json.toUtf8());
    QJsonObject obj = jsonDoc.object();
    start = obj["start"].toInt();
    len = obj["length"].toInt();
}

QVector<int>* MainWindow::getDataByLoop(QByteArray query, int start, int len, QString field) {
    QVector<int> *vct = new QVector<int>;
    JsonConvertor *conv = new JsonConvertor();
    for (int i = start; i < start + len; i++) {
        QByteArray middle_answ = port->writeData(query + "[" + QByteArray::number(i) + "]");
        QString json = conv->dataToJson(middle_answ);
        QJsonObject obj = QJsonDocument::fromJson(json.toUtf8()).object();
        vct->append(obj[field].toInt());
    }
    return vct;
}

//перебор пользователей
QMap<int, QJsonObject>* MainWindow::getUsersOnDevice(QVector<int> users_ids) {
    //лучше в структуру записывать <int (ID), JSON>
    QMap<int, QJsonObject> *device_users = new QMap<int, QJsonObject>;
    QByteArray answer = "";
    QByteArray query = "get UserArr";
    JsonConvertor *conv = new JsonConvertor();
    for (int i=0; i<users_ids.length(); i++) {
        QByteArray middle_answ = port->writeData(query + "[" + QByteArray::number(users_ids[i]) + "]");
        QString json = conv->dataToJson(middle_answ);
        QJsonParseError *err = new QJsonParseError();
        QJsonDocument doc = QJsonDocument::fromJson(json.toUtf8(), err);
        if (doc.isEmpty()) {
            qDebug() << "111";
        }
        QJsonObject obj = doc.object(); //не получается в json =(
        device_users->insert(obj["UserID"].toInt(), obj);
        answer += json;
    }
    ui->textEdit->setText(answer); //это не нужно
    return device_users;
}
