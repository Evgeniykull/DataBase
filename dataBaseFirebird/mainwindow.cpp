#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QSqlError>
#include <QSqlRecord>
#include <QSqlQuery>
#include <QByteArray>
#include <QDateTime>
#include <QTimer>
#include <QDebug>

#include "dialogs/adduserdialog.h"
#include "models/modeluser.h"
#include "models/modelfuels.h"
#include "models/modeltanks.h"
#include "models/modelpoints.h"
#include "models/modellimit.h"
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

    settings = new QSettings("settings.conf", QSettings::IniFormat);
    readSettings();
    if (!ui->lePath->text().isEmpty()) {
        onConnectClick();
    }

    connect(ui->pbConnect, SIGNAL(clicked(bool)), SLOT(onConnectClick()));
    connect(ui->teUserInfo, &QTextEdit::textChanged, this, &MainWindow::updateLimits);

    //connect to device
    port = new Port();
    card_reader = new Port(1);
    ui->verticalLayout->addWidget(port);
    ui->verticalLayout->addWidget(card_reader);
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

    connect(ui->tabWidget, SIGNAL(currentChanged(int)), SLOT(currentTabChanged(int)));
    connect(ui->pbAddUser, SIGNAL(clicked(bool)), SLOT(onAddUsersClick()));
    connect(ui->pbDeleteUser, SIGNAL(clicked(bool)), SLOT(deleteUsers()));
    connect(ui->pbEditUser, SIGNAL(clicked(bool)), SLOT(onEditUsersClick()));

    connect(ui->tabWidget_2, SIGNAL(currentChanged(int)), SLOT(currentTab2Changed(int)));
    connect(ui->pbAddFuel, SIGNAL(clicked(bool)), SLOT(addFuels()));
    connect(ui->pbDeleteFuel, SIGNAL(clicked(bool)), SLOT(deleteFuels()));
    connect(ui->pbEditFuel, SIGNAL(clicked(bool)), SLOT(onEditFuelsClick()));

    connect(ui->pbAddTanks, SIGNAL(clicked(bool)), SLOT(addTanks()));
    connect(ui->pbDeleteTanks, SIGNAL(clicked(bool)), SLOT(deleteTanks()));
    connect(ui->pbEditTanks, SIGNAL(clicked(bool)), SLOT(onEditTanksClick()));

    connect(ui->pbAddPoints, SIGNAL(clicked(bool)), SLOT(addPoints()));
    connect(ui->pbDeletePoints, SIGNAL(clicked(bool)), SLOT(deletePoints()));
    connect(ui->pbEditPoints, SIGNAL(clicked(bool)), SLOT(onEditPointsClick()));
    connect(ui->pbChangePointsNum, SIGNAL(clicked(bool)), SLOT(changeCurrentPointNum()));

    connect(ui->pbAddLimits, SIGNAL(clicked(bool)), SLOT(addLimits()));
    connect(ui->pbDeleteLimits, SIGNAL(clicked(bool)), SLOT(deleteLimits()));
    connect(ui->pbEditLimits, SIGNAL(clicked(bool)), SLOT(editLimits()));

    connect(ui->actConfigurate, SIGNAL(triggered(bool)), SLOT(startConfigurate()));
    connect(ui->actUpdUsers, SIGNAL(triggered(bool)), SLOT(getUserIndex()));

    connect(ui->pbGetUserCard, SIGNAL(clicked(bool)), SLOT(getUserCard()));
}

void MainWindow::currentTabChanged(int tabNum) {
    switch (tabNum) {
    case 0:
        updateUsers();
        break;
    case 1:
        currentTab2Changed(last_select_tab2);
    default:
        break;
    }
}

void MainWindow::currentTab2Changed(int tabNum) {
    last_select_tab2 = tabNum;
    switch (tabNum) {
    case 0:
        updateFuels();
        break;
    case 1:
        updateTanks();
        break;
    case 2:
        updatePoints();
        break;
    default:
        break;
    }
}

void MainWindow::writeSettings() {
    settings->beginGroup("MainWindows");
    settings->setValue("size", size());
    settings->setValue("pos", pos());
    settings->endGroup();

    settings->beginGroup("DataBase");
    settings->setValue("path", ui->lePath->text());
    settings->setValue("userName", ui->leUserName->text());
    settings->setValue("pass", ui->lePassword->text());
    settings->setValue("azsNum", ui->leAZSNum->text());
    settings->endGroup();
}

void MainWindow::readSettings() {
    settings->beginGroup("MainWindows");
    QPoint pos = settings->value("pos").toPoint();
    QSize size = settings->value("size").toSize();
    settings->endGroup();
    move(pos);
    resize(size);

    settings->beginGroup("DataBase");
    QString path = settings->value("path").toString();
    QString name = settings->value("userName").toString();
    QString pass = settings->value("pass").toString();
    azsNum = settings->value("azsNum").toString();
    ui->lePath->setText(path);
    ui->leUserName->setText(name);
    ui->lePassword->setText(pass);
    ui->leAZSNum->setText(azsNum);
    settings->endGroup();
}

void MainWindow::closeEvent(QCloseEvent *event) {
    writeSettings();
    QMessageBox::StandardButton resBtn = QMessageBox::question( this, "dataBaseFirebird",
       "Вы уверены, что хотите выйти?\n",
       QMessageBox::No | QMessageBox::Yes,
       QMessageBox::Yes);
   if (resBtn != QMessageBox::Yes) {
       event->ignore();
   } else {
       event->accept();
   }
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
    //db.setDatabaseName("F:/DataBase/TESTBASE.FDB"); //ui->lePath->text()
    db.setDatabaseName(ui->lePath->text());
    db.setUserName(ui->leUserName->text());
    db.setPassword(ui->lePassword->text());
    //db.setUserName("SYSDBA");
    //db.setPassword("123");
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
    model_users = new QSqlRelationalTableModel(0, db);
    model_users->setTable("users");
    model_users->setEditStrategy(QSqlTableModel::OnManualSubmit);
    model_users->select();

    model = new TreeModel(*model_users, ui->teUserInfo);
    ui->treeView->setModel(model);
    connect(ui->treeView, SIGNAL(clicked(QModelIndex)), model, SLOT(Clicked(QModelIndex)));
    //отслеживать изменение info
    //connect(ui->treeView, SIGNAL(clicked(QModelIndex)), SLOT(onUserClick(QModelIndex)));

    ui->teUserInfo->clear();
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
    model_tanks->setRelation(1, QSqlRelation("FUELS", "FUELDID", "NAME"));
    model_tanks->setEditStrategy(QSqlTableModel::OnManualSubmit);
    QString filter = QString("objectid=%1").arg(azsNum);
    model_tanks->setFilter(filter);
    model_tanks->select();
    ui->tableTanks->setModel(model_tanks);
    ui->tableTanks->setColumnHidden(3, true);
    setTableFormat(ui->tableTanks);
}

void MainWindow::addTanks() {
    ModelTanks *tank_model = new ModelTanks(db, azsNum);
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

    ModelTanks *tanks_model = new ModelTanks(db, azsNum);
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

    ModelTanks *tanks_model = new ModelTanks(db, azsNum);
    connect(tanks_model, SIGNAL(needUpdate()), SLOT(updateTanks()));
    int tankid = model_tanks->record(idx).value("id").toInt();
    tanks_model->editTanks(tankid);
}

void MainWindow::updatePoints() {
    QSqlQuery* query = new QSqlQuery(db);
    QString statament = QString("SELECT dispsid from POINTS WHERE objectid=%1").arg(azsNum);
    query->exec(statament);

    int i = 0;
    //ui->cbTankNum->clear(); //no!
    while(query->next()) {
        if (ui->cbTankNum->findText(query->record().value("dispsid").toString()) == -1) {
            ui->cbTankNum->insertItem(i, query->record().value("dispsid").toString());
        }
        if (pointNum.isEmpty()) {
            pointNum = query->record().value("dispsid").toString();
        }
        i += 1;
    }
    //ui->cbTankNum->setCurrentText(pointNum);

    model_points = new QSqlRelationalTableModel(0, db);
    model_points->setTable("points");
    model_points->setEditStrategy(QSqlTableModel::OnManualSubmit);
    QString filter = QString("objectid=%1 and dispsid=%2").arg(azsNum).arg(pointNum);
    model_points->setFilter(filter);
    model_points->select();
    ui->tablePoints->setModel(model_points);
    ui->tablePoints->setColumnHidden(1, true);
    setTableFormat(ui->tablePoints);
}

void MainWindow::addPoints() {
    ModelPoints *point_model = new ModelPoints(db, azsNum);
    connect(point_model, SIGNAL(needUpdate()), SLOT(updatePoints()));
    point_model->addPoint();
}

void MainWindow::deletePoints() {
    QModelIndex iid = ui->tablePoints->currentIndex();
    int idx = iid.r;
    if (idx < 0) {
        return;
    }

    ModelPoints *point_model = new ModelPoints(db, azsNum);
    connect(point_model, SIGNAL(needUpdate()), SLOT(updatePoints()));
    int pointid = model_points->record(idx).value("id").toInt();
    point_model->deletePoint(pointid);
}

void MainWindow::onEditPointsClick() {
    int idx = ui->tablePoints->currentIndex().r;
    if (idx < 0) {
        return;
    }

    ModelPoints *point_model = new ModelPoints(db, azsNum);
    connect(point_model, SIGNAL(needUpdate()), SLOT(updatePoints()));
    int pointid = model_points->record(idx).value("id").toInt();
    point_model->editPoint(pointid);
}

void MainWindow::changeCurrentPointNum() {
    pointNum = ui->cbTankNum->currentText();
    updatePoints();
}

void MainWindow::updateLimits() {
    QString info_text = ui->teUserInfo->toPlainText();
    int pos1 = info_text.indexOf(":");
    int pos2 = info_text.indexOf("\n");
    QString user_id = info_text.mid(pos1+2, pos2-pos1-2);

    if (user_id.isEmpty()){
        return;
    }

    model_limits = new QSqlRelationalTableModel(0, db);
    model_limits->setTable("limits");
    //model_limits->setRelation(1, QSqlRelation("USERS", "USERID", "VIEWNAME"));
    model_limits->setRelation(2, QSqlRelation("FUELS", "FUELDID", "NAME"));
    model_limits->setEditStrategy(QSqlTableModel::OnManualSubmit);
    QString filter = QString("userid=%1").arg(user_id);
    model_limits->setFilter(filter);
    model_limits->select();
    ui->tableLimits->setModel(model_limits);
    ui->tableLimits->setColumnHidden(1, true);
    setTableFormat(ui->tableLimits);
}

void MainWindow::addLimits() {
    QString info_text = ui->teUserInfo->toPlainText();
    if (info_text.isEmpty()) return;
    int pos1 = info_text.indexOf(":");
    int pos2 = info_text.indexOf("\n");
    QString user_id = info_text.mid(pos1+2, pos2-pos1-2);
    if (user_id.isEmpty()) return;

    ModelLimit *limits_modes = new ModelLimit(db, user_id);
    connect(limits_modes, SIGNAL(needUpdate()), SLOT(updateLimits()));
    limits_modes->addLimits();
}

void MainWindow::deleteLimits() {
    QModelIndex iid = ui->tableLimits->currentIndex();
    int idx = iid.r;
    if (idx < 0) {
        return;
    }

    QString info_text = ui->teUserInfo->toPlainText();
    if (info_text.isEmpty()) return;
    int pos1 = info_text.indexOf(":");
    int pos2 = info_text.indexOf("\n");
    QString user_id = info_text.mid(pos1+2, pos2-pos1-2);
    if (user_id.isEmpty()) return;

    ModelLimit *limits_model = new ModelLimit(db, user_id);
    connect(limits_model, SIGNAL(needUpdate()), SLOT(updateLimits()));
    int limitsid = model_limits->record(idx).value("id").toInt();
    limits_model->deleteLimits(limitsid);
}

void MainWindow::editLimits() {
    int idx = ui->tableLimits->currentIndex().r;
    if (idx < 0) {
        return;
    }

    QString info_text = ui->teUserInfo->toPlainText();
    if (info_text.isEmpty()) return;
    int pos1 = info_text.indexOf(":");
    int pos2 = info_text.indexOf("\n");
    QString user_id = info_text.mid(pos1+2, pos2-pos1-2);
    if (user_id.isEmpty()) return;

    ModelLimit *limit_model = new ModelLimit(db, user_id);
    connect(limit_model, SIGNAL(needUpdate()), SLOT(updateLimits()));
    int limitid = model_limits->record(idx).value("id").toInt();
    limit_model->editLimits(limitid);
}

void MainWindow::startConfigurate() {
    ModelFuels *fuel_model = new ModelFuels(db);
    QList<QString> fuel_data = fuel_model->configureFuels();

    ModelTanks *tank_model = new ModelTanks(db, azsNum);
    QList<QString> tank_data = tank_model->configureTanks();

    ModelPoints *point_model = new ModelPoints(db, azsNum);
    QList<QString> point_data = point_model->configurePoints();

    if (fuel_data.isEmpty() || tank_data.isEmpty() || point_data.isEmpty()) {
        mbx = new QMessageBox();
        mbx->setWindowTitle("Конфигурация");
        mbx->setText("Таблицы FUEL, TANK, POINT пустые");
        mbx->exec();
        qDebug() << "BD not connected!";
        return;
    }

    bool ok = port->changeAccess(1);

    if (ok) {
        for(int i=0; i<fuel_data.length(); i++){
            QByteArray conf_answ = port->writeData(fuel_data.at(i).toUtf8());
            if (conf_answ.indexOf("OK") < 0) {
                qDebug() << "error on configurate fuel #" << i;
            }
        }

        for(int i=0; i<tank_data.length(); i++){
            QByteArray conf_answ = port->writeData(tank_data.at(i).toUtf8());
            if (conf_answ.indexOf("OK") < 0) {
                qDebug() << "error on configurate tank #" << i;
            }
        }

        for(int i=0; i<point_data.length(); i++){
            QByteArray conf_answ = port->writeData(point_data.at(i).toUtf8());
            if (conf_answ.indexOf("OK") < 0) {
                qDebug() << "error on configurate point #" << i;
            }
        }
    } else {
        mbx = new QMessageBox();
        mbx->setWindowTitle("Конфигурация");
        mbx->setText("Не удалось перевести устройство в режим администратора");
        mbx->exec();
        return;
    }

    port->changeAccess(0);

    mbx = new QMessageBox();
    mbx->setWindowTitle("Конфигурация");
    mbx->setText("Конфигурация прошла успешно");
    mbx->exec();
}

QString convertData(QString data) {
    QString newData = data.split("-")[2];
    newData.append(".");
    newData.append(data.split("-")[1]);
    newData.append(".");
    //newData.append(data.split("-")[0].mid(2,2));
    newData.append(data.split("-")[0]);
    return newData;
}

void MainWindow::getUserIndex() {
    /*QByteArray query3 = "get UserArr[7]";
    QByteArray answ3 = port->writeData(query3);
    qDebug() << answ3;
    return;*/

    QByteArray query = "get UserIndex";
    QByteArray answ = port->writeData(query);
    int arr_start, arr_len;
    parseArray(answ, arr_start, arr_len);

    //цикл от start с перебором всех
    QVector<int> *indexes = getDataByLoop(query, arr_start, arr_len, "UserID");
    QMap<int, QJsonObject> *device_users = getUsersOnDevice(*indexes);

    QVector<int> *all_user_in_db = new QVector<int>;
    QSqlQuery* query0 = new QSqlQuery(db);
    QString statament0 = QString("SELECT userid FROM users");
    query0->exec(statament0);
    while (query0->next()) {
        all_user_in_db->push_back(query0->value(0).toInt());
    }

    //как получили map, проходимся по его ключам, сопоставляем json и записи из БД
    QList<int> keys = device_users->keys();
    for (int i = 0; i < keys.length(); i++) {
        all_user_in_db->remove(all_user_in_db->indexOf(keys[i]));
        updateUserOnDevice(keys[i]);
    }

    QList<int> keys2 = all_user_in_db->toList();
    for (int i = 0; i < keys2.length(); i++) {
        all_user_in_db->remove(all_user_in_db->indexOf(keys2[i]));
        updateUserOnDevice(keys2[i]);
    }
}

void MainWindow::updateUserOnDevice(int userId) {
    QSqlQuery* query = new QSqlQuery(db);
    QString statament = QString("SELECT * FROM users WHERE userid=%1").arg(QString::number(userId));
    query->exec(statament);
    //по идее только 1 запись в выборке
    QString userNewData = QString("set UserArr[%1]:{UserID:0 ParentID:0 ViewName:\"\" Flags:0 CardID:0}").arg(QString::number(userId));
    QString userNewLimits = "";
    while (query->next()) {
        if (query->value("REFSLIM").toInt() == 0) {
            userNewData = QString("set UserArr[%1]:{ UserID:%2 ParentID:%3 ViewName:\"%4\" Flags:%5 CardID:%6 SLData:\"%7\" RefrLim:%8 }")
                    .arg(QString::number(userId))
                    .arg(query->value(0).toString())
                    .arg(query->value("PARENTID").toString())
                    .arg(query->value("VIEWNAME").toString())
                    .arg(query->value("FLAGS").toString())
                    .arg(query->value("CARDID").toString())
                    .arg(convertData(query->value("SLDATE").toString()))
                    .arg(query->value("REFSLIM").toString());
            continue;
        } else {
            //userNewData = QString("set UserArr[%1]:{UserID:%2 ParentID:%3 ViewName:\"%4\" Flags:%5 CardID:%6 SLData:\"%7\" RefrLim:%8 Limits[]:[")
            userNewData = QString("set UserArr[%1]:{UserID:%2 ParentID:%3 ViewName:\"%4\" Flags:%5 CardID:%6 SLData:\"%7\" RefrLim:%8}")
                    .arg(QString::number(userId))
                    .arg(query->value(0).toString())
                    .arg(query->value("PARENTID").toString())
                    .arg(query->value("VIEWNAME").toString())
                    .arg(query->value("FLAGS").toString())
                    .arg(query->value("CARDID").toString())
                    .arg(convertData(query->value("SLDATE").toString()))
                    .arg(query->value("REFSLIM").toString());
        }

        userNewLimits = QString("set UserArr[%1].Limits[]:[").arg(QString::number(userId));
        QSqlQuery* limits_query = new QSqlQuery(db);
        QString limits_statament = QString("SELECT * FROM limits WHERE userid=%1").arg(QString::number(userId));
        limits_query->exec(limits_statament);

        QString userLimits = "";
        while(limits_query->next()) {
            userLimits += QString("{fID:%1 Tp:%2 Dy:%3 Vl:%5},")
                    .arg(limits_query->value("FUELID").toString())
                    .arg(limits_query->value("TYPED").toString())
                    .arg(limits_query->value("DAYS").toString())
                    .arg(limits_query->value("VALUEL").toString());
        }
        userLimits = userLimits.mid(0, userLimits.length()-1);

        userNewLimits.append(userLimits);
        userNewLimits.append("]");
        //userNewData.append(userLimits);
        //userNewData.append("]}");
    }

    QByteArray user_answ = port->writeData(userNewData.toUtf8());
    qDebug() << "Trrrrr";
    if (user_answ.indexOf("OK") < 0) {
        qDebug() << "Error: " << userNewData;
        qDebug() << "error on user set #" << userId;
    } else {
        if (!userNewLimits.isEmpty()) {
            QByteArray user_answ2 = port->writeData(userNewLimits.toUtf8());
            if (user_answ2.indexOf("OK") < 0) {
                qDebug() << "Error: " << userNewLimits;
                return;
            }
        }

        QSqlQuery* query2 = new QSqlQuery(db);
        QString statament2 = QString("UPDATE users SET refslim=0 WHERE userid=%1").arg(QString::number(userId));
        query2->exec(statament2);
    }
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
    return device_users;
}

#include "dialogs/getcarddialog.h"
void MainWindow::getUserCard() {
    QString info = ui->teUserInfo->toPlainText();
    int pos1 = info.indexOf(":");
    if (pos1 < 0) {
        return;
    }

    int pos2 = info.indexOf("\n");
    int len = pos2 - pos1;
    QString subInf = info.mid(pos1+2, len-2);
    selected_user_id = subInf.toInt();

    GetCardDialog *card_dialog = new GetCardDialog();
    connect(card_dialog, SIGNAL(onAcceptClick()), this, SLOT(changeUserCard()));
    card_dialog->show();
}

void MainWindow::changeUserCard() {
    int state = 0;
    timer_end = 0;
    QByteArray answ;
    QDateTime dt = QDateTime::currentDateTime();
    while (state < 1) {
        //таймер на 5 секунд
        if (QDateTime::currentDateTime().toSecsSinceEpoch() - dt.toSecsSinceEpoch() > 50) break;
        answ = card_reader->writeData("get RFInfo");
        state = getValueFromState(answ, "State:").toInt();
    }
    qDebug() << answ;

    if (state != 2) {
        qDebug() << "State not 2";
        return;
    }

    QString ID = getValueFromState(answ, "ID:");
    QString card_id = dt.toString("dhhmmss");
    //запись в БД
    QSqlQuery* change_card_id_query = new QSqlQuery(db);
    QString statament = QString("UPDATE users SET cardid=%1 WHERE userid=%2").arg(card_id).arg(selected_user_id);
    change_card_id_query->exec(statament);

    if (change_card_id_query->lastError().isValid()) {
        qDebug() << change_card_id_query->lastError().databaseText();
    }

    QString convertCartInt = getCartMemoryData(card_id.toInt());
    QString cart_data = convertCartInt + "00000000" + getCartMemoryData(selected_user_id) + "01000000";
    QString query_to_card = QString("run RFWrite:{ Key:\"B\" Addr:1 ID:\"%1\" Data:\"%2\" }").arg(ID).arg(cart_data);
    qDebug() << query_to_card;

    //запись в картридер
    answ = card_reader->writeData(query_to_card.toUtf8());
    qDebug() << answ;

    answ = card_reader->writeData("get RFInfo");
    qDebug() << answ;
    state = getValueFromState(answ, "State:").toInt();

    if (state < 10) {
        qDebug() << "State < 10";
        return;
    }

    answ = card_reader->writeData("run RFRead:{ Key:\"B\" Addr:1}");
    answ = card_reader->writeData("get RFInfo");

    QString _final = getValueFromState(answ, "Data:");

    if (QString::compare(cart_data, _final)) {
        qDebug() << "Error. Date not equal.";
    }

    qDebug() << "Success. Card changed.";
}

QString MainWindow::getCartMemoryData(int k) {
    QString int_16 = QString("%1").arg(k,0,16).toUpper();

    //если пришло число 1-9
    int_16 = "0000000" + int_16;

    QString subA = int_16.right(2);
    if (subA.length() == 1) {
        subA = "0" + subA;
    }

    int_16 = int_16.mid(0, int_16.length() - 2);
    QString subB = int_16.right(2);
    int_16 = int_16.mid(0, int_16.length() - 2);
    QString subC = int_16.right(2);
    int_16 = int_16.mid(0, int_16.length() - 2);
    QString subD = int_16.right(2);

    QString finally = subA + subB + subC + subD;
    //qDebug() << subA << subB << subC << subD;
    return finally;
}

QString MainWindow::getValueFromState(QString source, QString fnd) {
    int pos1 = source.indexOf(fnd);
    QString answ = source.mid(pos1);
    int pos2 = answ.indexOf("\r");
    answ = answ.mid(fnd.length(), pos2 - fnd.length());

    //если есть кавычки \" то обрезаем
    if (answ.indexOf("\"") > -1) {
        answ = answ.left(answ.length() - 1);
        answ = answ.right(answ.length() - 1);
    }

    return answ;
}

void MainWindow::timerEnd() {
    timer_end = 1;
}
