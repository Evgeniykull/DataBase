#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QSqlError>
#include <QSqlRecord>
#include <QSqlQuery>
#include <QByteArray>
#include <QDateTime>
#include <QTimer>
#include <QJsonDocument>
#include <QJsonObject>
#include <QProgressDialog>
#include <QDebug>

#include "dialogs/adduserdialog.h"
#include "utils/treadworker.h"
#include "utils/jsonconvertor.h"
#include "utils/utils.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(ui->actExit, SIGNAL(triggered()), SLOT(close()));
    connect(ui->pbView, SIGNAL(clicked()), SLOT(onViewClick()));
    renderToolbar();
    ui->tabWidget->tabBar()->hide(); //для скрытия табов
    ui->menuBar->hide();
    status_bar = new QLabel(this);    
    port_status = new QLabel(this);
    ui->statusBar->addWidget(status_bar); //для строки состояния
    ui->statusBar->addWidget(port_status); //для строки состояния

    settings = new QSettings("settings.conf", QSettings::IniFormat);
    readSettings();
    changeAccess();
    onConnectClick();

    connect(ui->pbConnect, SIGNAL(clicked(bool)), SLOT(onConnectClick()));
    connect(ui->teUserInfo, &QTextEdit::textChanged, this, &MainWindow::updateLimits);

    //connect to device
    card_reader = new Port(1);
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
    menu_bar->addAction(ui->actAbout);
    menu_bar->setExclusive(true);
    ui->mainToolBar->addActions(menu_bar->actions());

    connect(ui->actUser, SIGNAL(triggered(bool)), SLOT(onActUserClick()));
    connect(ui->actDirectory, SIGNAL(triggered(bool)), SLOT(onActDictionaryClick()));
    connect(ui->actConnect, SIGNAL(triggered(bool)), SLOT(onActConnectClick()));
    connect(ui->actAbout, SIGNAL(triggered(bool)), SLOT(onActAboutClick()));

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

    connect(ui->pbAddObj, SIGNAL(clicked(bool)), SLOT(addObject()));
    connect(ui->pbDeleteObj, SIGNAL(clicked(bool)), SLOT(deleteObject()));
    connect(ui->pbEditObj, SIGNAL(clicked(bool)), SLOT(editObject()));
    connect(ui->tableObject, SIGNAL(clicked(QModelIndex)), SLOT(changedObject(QModelIndex)));
    connect(ui->pbSetObj, SIGNAL(clicked(bool)), SLOT(setObjectSettings()));

    connect(ui->pbGetHistory, SIGNAL(clicked(bool)), SLOT(getHistory()));

    connect(ui->pbConfigure, SIGNAL(clicked(bool)), SLOT(startConfigurate()));
    connect(ui->pbUserUpdate, SIGNAL(clicked(bool)), SLOT(getUserIndex()));

    connect(ui->pbGetUserCard, SIGNAL(clicked(bool)), SLOT(getUserCard()));
    connect(ui->cbAdminAccess, SIGNAL(clicked(bool)), SLOT(changeAccess()));
}

void MainWindow::declOfVaribles() {
    model_users = new QSqlRelationalTableModel(0, db);
    model_users->setTable("users");
    model_users->setEditStrategy(QSqlTableModel::OnManualSubmit);
    add_user_model = new ModelUser(db);
    connect(add_user_model, SIGNAL(needUpdate()), SLOT(updateUsers()));

////////
// Всё что касается окна редактирования топлива
    model_fuels = new QSqlRelationalTableModel(0, db);
    model_fuels->setTable("fuels");
    model_fuels->setEditStrategy(QSqlTableModel::OnManualSubmit);
    add_fuels = new AddFuelsDialog();
    connect(add_fuels, SIGNAL(needUpdate()), SLOT(updateFuels()));
    connect(add_fuels, SIGNAL(toUser(QString,QString)), SLOT(MessageToUser(QString,QString)));
///////
// Окно редактирования резервуаров
    model_tanks = new QSqlRelationalTableModel(0, db);
    add_tanks = new AddTanksDialog();
    connect(add_tanks, SIGNAL(needUpdate()), SLOT(updateTanks()));
    connect(add_tanks, SIGNAL(toUser(QString,QString)), SLOT(MessageToUser(QString,QString)));

    model_points = new QSqlRelationalTableModel(0, db);
    add_points = new AddPointsDialog();
    connect(add_points, SIGNAL(needUpdate()), SLOT(updatePoints()));
    connect(add_points, SIGNAL(toUser(QString,QString)), SLOT(MessageToUser(QString,QString)));

    model_limits = new QSqlRelationalTableModel(0, db);
    model_history = new QSqlRelationalTableModel(0, db);

    model_object = new QSqlRelationalTableModel(0, db);
    add_object = new AddObjectDialog();
    connect(add_object, SIGNAL(toUser(QString,QString)), SLOT(MessageToUser(QString,QString)));
    connect(add_object, SIGNAL(needUpdate()), SLOT(updateObject()));

    card_dialog = new GetCardDialog();
    connect(card_dialog, SIGNAL(onAcceptClick()), this, SLOT(changeUserCard()));

    add_limits_model = new ModelLimit(db);
    connect(add_limits_model, SIGNAL(needUpdate()), SLOT(updateLimits()));

    mbx = new QMessageBox();

    port_settings = new PortSettings();
    connect(port_settings, SIGNAL(settingsIsChanged()), SLOT(writeObjectSettings()));

    model = new TreeModel(*model_users, ui->teUserInfo);
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
    case 3:
        updateObject();
    case 4:
        updateHistory();
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
    settings->setValue("isAdmin", ui->cbAdminAccess->isChecked());
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
    bool adm = settings->value("isAdmin").toBool();
    ui->lePath->setText(path);
    ui->leUserName->setText(name);
    ui->lePassword->setText(pass);
    ui->cbAdminAccess->setChecked(adm);
    settings->endGroup();
}

void MainWindow::MessageToUser(QString title,QString text) {
  QMessageBox msgBox;
  msgBox.setWindowTitle(title);
  msgBox.setText(text);
  msgBox.setStandardButtons(QMessageBox::Yes);
  msgBox.setDefaultButton(QMessageBox::Yes);
  msgBox.exec();
}

void MainWindow::closeEvent(QCloseEvent *event) {
// Временно
    writeSettings();

    event->accept();
    return;

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

void MainWindow::changeAccess() {
    isAdmin = ui->cbAdminAccess->isChecked();
    accessCheck();
}

void MainWindow::accessCheck() {
    if (isAdmin) {
        ui->pbAddFuel->setDisabled(false);
        ui->pbEditFuel->setDisabled(false);
        ui->pbDeleteFuel->setDisabled(false);
    } else {
        ui->pbAddFuel->setDisabled(true);
        ui->pbEditFuel->setDisabled(true);
        ui->pbDeleteFuel->setDisabled(true);
    }

    if (azsNum > 0) {
        ui->pbUserUpdate->setDisabled(false);
        ui->pbGetHistory->setDisabled(false);
    } else {
        ui->pbUserUpdate->setDisabled(true);
        ui->pbGetHistory->setDisabled(true);
    }

    if (isAdmin && (azsNum > 0)) {
        ui->pbAddObj->setDisabled(false);
        ui->pbDeleteObj->setDisabled(false);
        ui->pbEditObj->setDisabled(false);
        ui->pbSetObj->setDisabled(false);
        ui->pbAddPoints->setDisabled(false);
        ui->pbDeletePoints->setDisabled(false);
        ui->pbEditPoints->setDisabled(false);
        ui->pbAddTanks->setDisabled(false);
        ui->pbDeleteTanks->setDisabled(false);
        ui->pbEditTanks->setDisabled(false);
        ui->pbConfigure->setDisabled(false);
    } else {
        ui->pbAddObj->setDisabled(true);
        ui->pbDeleteObj->setDisabled(true);
        ui->pbEditObj->setDisabled(true);
        ui->pbSetObj->setDisabled(true);
        ui->pbAddPoints->setDisabled(true);
        ui->pbDeletePoints->setDisabled(true);
        ui->pbEditPoints->setDisabled(true);
        ui->pbAddTanks->setDisabled(true);
        ui->pbDeleteTanks->setDisabled(true);
        ui->pbEditTanks->setDisabled(true);
        ui->pbConfigure->setDisabled(true);
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

void MainWindow::onActAboutClick() {
    ui->tabWidget->setCurrentIndex(3);
}

void MainWindow::onConnectClick() {
    db = QSqlDatabase::addDatabase("QIBASE");
    QString path = ui->lePath->text().isEmpty() ? "TESTBASE.FDB" : ui->lePath->text();
    db.setDatabaseName(path);
    db.setUserName(ui->leUserName->text()); //SYSDBA
    db.setPassword(ui->lePassword->text()); //123
    bool is_open = db.open();

    db_status = is_open ? "Подключено" : db.lastError().text();

    if (is_open) {
        status_bar->setText(db_status);
        ui->actUser->setDisabled(false);
        ui->actDirectory->setDisabled(false);
        ui->pbConnect->hide();
    } else {
        ui->actUser->setDisabled(true);
        ui->actDirectory->setDisabled(true);

        ui->actUser->setChecked(false);
        ui->actDirectory->setChecked(false);
        ui->actConnect->setChecked(true);

        ui->tabWidget->setCurrentIndex(2);
    }
    //по идее нужно удалять model при каждом подключении,
    //но если сейчас подключиться к другой базе можно только при входе, то все ок
    declOfVaribles();
}

void setTableFormat(QTableView *tab, int autoresize = 1)
{
    tab->setAlternatingRowColors(true);         // Подсветка строк разными цветами
    if (autoresize) {
        tab->resizeRowsToContents();
        tab->resizeColumnsToContents();
        tab->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    }
    tab->setSortingEnabled(true);               // Сортировка таблицы
    tab->sortByColumn(1,Qt::AscendingOrder);    // Порядок сортировки по умолчанию
    tab->setColumnHidden(0, true);              // Скрываем колонку с индексом
}

void MainWindow::updateUsers() {
    model_users->select();
    //пока treeModel оставлю
    if (model != nullptr) {
        delete model;
    }

    model = new TreeModel(*model_users, ui->teUserInfo);
    ui->treeView->setModel(model);
    connect(ui->treeView, SIGNAL(clicked(QModelIndex)), model, SLOT(Clicked(QModelIndex)));
    ui->teUserInfo->clear();
    //ui->tableLimits->clearSelection();
}

void MainWindow::onAddUsersClick() {
    add_user_model->addUser();
}

void MainWindow::onEditUsersClick() {
    if (ui->treeView->currentIndex().row() < 0) {
        return;
    }
    int userid = ui->teUserInfo->toPlainText().split("\n").at(0).split(":")[1].toInt();
    add_user_model->editUsers(userid);
}

//TODO добавить подтверждение
void MainWindow::deleteUsers()
{
    if (ui->treeView->currentIndex().row() < 0) {
        return;
    }

    int userid = ui->teUserInfo->toPlainText().split("\n").at(0).split(":")[1].toInt();
    add_user_model->deleteUsers(userid);
}

///////////////////////////////////////////////
/// Слоты, относящиеся к редактированию топлива
void MainWindow::updateFuels() {
  model_fuels->select();
  ui->tableFuel->setModel(model_fuels);
  setTableFormat(ui->tableFuel);
  ui->tableFuel->update();
}

void MainWindow::addFuels() {
    add_fuels->ShowFuelData(-1,db);
}

void MainWindow::deleteFuels() {
    int idx = ui->tableFuel->currentIndex().row();
    if (idx < 0) return;
    int fuelid = model_fuels->record(idx).value("fueldid").toInt();
    add_fuels->DeleteFuelData(fuelid,db);
}

void MainWindow::onEditFuelsClick() {
    int idx = ui->tableFuel->currentIndex().row();
    if (idx < 0) return;
    int fuelid = model_fuels->record(idx).value("fueldid").toInt();
    add_fuels->ShowFuelData(fuelid,db);
}
////////////////////////////////////////

void MainWindow::updateTanks() {
    model_tanks->setTable("tanks");
    model_tanks->setRelation(1, QSqlRelation("FUELS", "FUELDID", "NAME"));
    model_tanks->setEditStrategy(QSqlTableModel::OnManualSubmit);
    QString filter = QString("objectid=%1").arg(azsNum);
    model_tanks->setFilter(filter);
    model_tanks->select();
    ui->tableTanks->setModel(model_tanks);
    ui->tableTanks->setColumnHidden(3, true);
    setTableFormat(ui->tableTanks);
  ui->tableTanks->update();
}

void MainWindow::addTanks() {
  add_tanks->ShowTankDialog(-1,azsNum,db);
}

void MainWindow::deleteTanks() {
  QModelIndex iid = ui->tableTanks->currentIndex();
  int idx = iid.row();
  if (idx < 0) return;
  int tankid = model_tanks->record(idx).value("id").toInt();
  add_tanks->DeleteTank(tankid,azsNum,db);
}

void MainWindow::onEditTanksClick() {
  int idx = ui->tableTanks->currentIndex().row();
  if (idx < 0) return;
  int tankid = model_tanks->record(idx).value("id").toInt();
  add_tanks->ShowTankDialog(tankid,azsNum,db);
}

void MainWindow::updatePoints() {
    QSqlQuery query(db);      
    QString statement;
    QString filter;
    statement = QString("SELECT DISPSID FROM POINTS WHERE objectid=%1").arg(azsNum);
    query.exec(statement);
    int i = 0;
    //ui->cbTankNum->clear(); //no!
    while(query.next()) {
        if (ui->cbTankNum->findText(query.record().value("dispsid").toString()) == -1) {
            ui->cbTankNum->insertItem(i, query.record().value("dispsid").toString());
        }
        if (pointNum.isEmpty()) {
            pointNum = query.record().value("dispsid").toString();
        }
        i += 1;
    }
    //ui->cbTankNum->setCurrentText(pointNum);
    model_points->setTable("points");
    model_points->setEditStrategy(QSqlTableModel::OnManualSubmit);
    add_points->SetDID(pointNum.toInt());
    filter = QString("objectid=%1 and dispsid=%2").arg(azsNum).arg(pointNum);
    model_points->setFilter(filter);
    model_points->select();
    ui->tablePoints->setModel(model_points);
    ui->tablePoints->setColumnHidden(1, true);
    setTableFormat(ui->tablePoints);
    ui->tablePoints->update();
}

void MainWindow::addPoints() {
    add_points->ShowPointsDialog(-1,azsNum,db);
}

void MainWindow::deletePoints() {
    QModelIndex iid = ui->tablePoints->currentIndex();
    int idx = iid.row();
    if (idx < 0) return;
    int pointid = model_points->record(idx).value("id").toInt();
    add_points->DeletePoint(pointid,azsNum,db);
}

void MainWindow::onEditPointsClick() {
    QModelIndex iid = ui->tablePoints->currentIndex();
    int idx = iid.row();
    if (idx < 0) return;
    int pointid = model_points->record(idx).value("id").toInt();
    add_points->ShowPointsDialog(pointid,azsNum,db);
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

    model_limits->setTable("limits");
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
    add_limits_model->setUserId(user_id);
    add_limits_model->addLimits();
}

void MainWindow::deleteLimits() {
    QModelIndex iid = ui->tableLimits->currentIndex();
    int idx = iid.row();
    if (idx < 0) {
        return;
    }

    QString info_text = ui->teUserInfo->toPlainText();
    if (info_text.isEmpty()) return;
    int pos1 = info_text.indexOf(":");
    int pos2 = info_text.indexOf("\n");
    QString user_id = info_text.mid(pos1+2, pos2-pos1-2);
    if (user_id.isEmpty()) return;

    int limitsid = model_limits->record(idx).value("id").toInt();
    add_limits_model->setUserId(user_id);
    add_limits_model->deleteLimits(limitsid);
}

void MainWindow::editLimits() {
    int idx = ui->tableLimits->currentIndex().row();
    if (idx < 0) {
        return;
    }

    QString info_text = ui->teUserInfo->toPlainText();
    if (info_text.isEmpty()) return;
    int pos1 = info_text.indexOf(":");
    int pos2 = info_text.indexOf("\n");
    QString user_id = info_text.mid(pos1+2, pos2-pos1-2);
    if (user_id.isEmpty()) return;

    int limitid = model_limits->record(idx).value("id").toInt();
    add_limits_model->setUserId(user_id);
    add_limits_model->editLimits(limitid);
}

void MainWindow::updateHistory() {
    model_history->setTable("UNASSMHISTORY");
    model_history->setEditStrategy(QSqlTableModel::OnManualSubmit);
    model_history->select();
    ui->tableHistory->setModel(model_history);
    setTableFormat(ui->tableHistory, 0);
    ui->tableHistory->setColumnHidden(1, true);
    ui->tableHistory->setColumnHidden(2, true);
    ui->tableHistory->setColumnWidth(5, 500);
}

void MainWindow::getHistory() {
    Port port(port_settings);
    int i;
    int curid,endid;
    QString json;
    QJsonDocument jsonDoc;
    QJsonObject obj;
//
    if (!port.StartComm()) return;
//синхронизируем время
    QString anw1 = port.write("get Состояние.Время");
    QString dev_time = anw1.mid(anw1.indexOf("Время:")+12, 19);
    QString cur_dt = QDateTime::currentDateTime().toString("dd.MM.yyyy hh:mm:ss");
    qint64 sec1 = QDateTime::fromString(dev_time, "dd.MM.yyyy hh:mm:ss").toSecsSinceEpoch();
    qint64 sec2 = QDateTime::fromString(cur_dt, "dd.MM.yyyy hh:mm:ss").toSecsSinceEpoch();
    qint64 dd = abs(sec2 - sec1);
    if (dd > 60){
        QString com = QString("run УстВремя: \"%1\"").arg(cur_dt);
        port.write(com.toUtf8());
    }
//
    QString anw; // Ответ
    QSqlQuery* query = new QSqlQuery(db);
    QString statament = QString("SELECT max(hisid) FROM UNASSMHISTORY WHERE OBJECTID=%1").arg(azsNum);
    query->exec(statament);
    query->next();
    curid = query->value(0).toInt();
    if (curid <= 0) {
      int end;
      anw = port.write("get История");
      parseArray(anw.toUtf8(),curid,end);
    }
    anw = port.write("get Состояние.История");
/*    json = conv.dataToJson(anw);
    jsonDoc = QJsonDocument::fromJson(json.toUtf8());
    obj = jsonDoc.object();
    endid = obj["start"].toInt();
    len = obj["length"].toInt();*/


    endid=getJSONField(anw.toUtf8(),"Текущий").toInt();
    if (endid >= curid) {

      QProgressDialog *progr_dialog = new QProgressDialog("Считывание истории", "&Отмена", 0, endid-curid-1);
      progr_dialog->setWindowTitle("Пожалуйста подождите");
      progr_dialog->setMinimumDuration(0);

      for (i=curid; i<endid; i++) {
        QString request = QString("get История[%1]").arg(QString::number(i));
        QString anw2 = port.write(request.toUtf8());
        QString rq = "";

        if (anw2.indexOf("error") > 0) {
          rq = QString("INSERT INTO UNASSMHISTORY (HISID, OBJECTID, OBJTIME, OBJTYPE, DATA) VALUES (%1, %2, '%3', %4, '%5')")
                      .arg(i)
                      .arg(azsNum)
                      .arg("00.00.00 00:00:00")
                      .arg("-1")
                      .arg("");
        } else {
          QString sdt,smaindt,pdt;
          int sid,stp;
          JsonConvertor conv;
          QString json = conv.dataToJson(anw2.toUtf8());
          QJsonDocument jsonDoc = QJsonDocument::fromJson(json.toUtf8());
          QJsonObject obj = jsonDoc.object();
          sid=obj["ID"].toInt();
          sdt=obj["Время"].toString();
          pdt = QDateTime::fromString(sdt, "dd.MM.yyyy hh:mm:ss").toString("yyyy-MM-dd hh:mm:ss");
          stp=obj["Тип"].toInt();
// вот это может работать НЕ ПРАВИЛЬНО
          smaindt=obj["Data"].toString();
          rq = QString("INSERT INTO UNASSMHISTORY (HISID, OBJECTID, OBJTIME, OBJTYPE, DATA) VALUES (%1, %2, '%3', %4, '%5')")
                  .arg(i)
                  .arg(azsNum)
                  .arg(pdt)
                  .arg(stp)
                  .arg(smaindt);
        }
        query->exec(rq);
        if (query->lastError().isValid()) {
            qDebug() << query->lastError().databaseText();
        }
        QString lbl = QString("Считано %1 из %2").arg(QString::number(i)).arg(QString::number(endid-curid-1));
        progr_dialog->setLabelText(lbl);
        progr_dialog->setValue(i);
        QCoreApplication::processEvents();
        if (progr_dialog->wasCanceled()) {
            break;
        }
    }
    delete progr_dialog;
    delete query;
// запись на устройство кол-во РЕАЛЬНО считанных данных
// работает даже если нажали "ОТМЕНУ"
    if (i>0) port.write(QString("run ЧтИстория:{Считано:%1}").arg(i-1).toUtf8());
  }
  port.EndComm();
  updateHistory();
}

void MainWindow::updateObject() {
    model_object->setTable("OBJECT_TABLES");
    model_object->setEditStrategy(QSqlTableModel::OnManualSubmit);
    model_object->select();
    ui->tableObject->setModel(model_object);
    ui->tableObject->setColumnHidden(2, true);
    ui->tableObject->setColumnHidden(3, true);
    ui->tableObject->setColumnHidden(4, true);
    ui->tableObject->setColumnHidden(5, true);
    setTableFormat(ui->tableObject);
}

void MainWindow::addObject() {
    add_object->ShowObjectData(-1, db);
}

void MainWindow::deleteObject() {
    int idx = ui->tableObject->currentIndex().row();
    if (idx < 0) {
        return;
    }
    int objectid = model_object->record(idx).value("objectid").toInt();
    add_object->DeleteObject(objectid, db);
}

void MainWindow::editObject() {
    int idx = ui->tableObject->currentIndex().row();
    if (idx < 0) {
        return;
    }
    int objectid = model_object->record(idx).value("objectid").toInt();
    add_object->ShowObjectData(objectid, db);
}

void MainWindow::changedObject(QModelIndex idx) {
    int objectid = model_object->record(idx.row()).value("objectid").toInt();
    azsNum = objectid;
    accessCheck();
    QString sett_js = model_object->record(idx.row()).value("CONNECTIONPROPERTY").toByteArray();
    port_status->setText(model_object->record(idx.row()).value("OBJECTNAME").toString());
    changedObjectSettings(sett_js);
}

void MainWindow::changedObjectSettings(QString objSett) {
    QJsonDocument doc = QJsonDocument::fromJson(objSett.toUtf8());
    QJsonObject json_obj = doc.object();
    QString adr = json_obj["addres"].toString();
    QString br = QString::number(json_obj["baudRate"].toInt());
    QString db = QString::number(json_obj["dataBits"].toInt());
    QString na = json_obj["name"].toString();
    QString pa = QString::number(json_obj["pairy"].toInt());
    QString sb = QString::number(json_obj["stopBits"].toInt());

    port_settings->setSettings(adr, br, db, na, pa, sb);
}

void MainWindow::setObjectSettings() {
    int idx = ui->tableObject->currentIndex().row();
    if (idx < 0) {
        return;
    }
    int objectid = model_object->record(idx).value("objectid").toInt();
    QString sett_js = add_object->getSettings(objectid, db);
    if (!sett_js.isEmpty()) {
        changedObjectSettings(sett_js);
    }
    port_settings->exec();
}

void MainWindow::writeObjectSettings() {
    QJsonObject sett_json {
        {"addres", port_settings->SettingsPort.addres},
        {"baudRate", port_settings->SettingsPort.baudRate},
        {"byteOnPackage", port_settings->SettingsPort.byteOnPackage},
        {"dataBits", port_settings->SettingsPort.dataBits},
        {"flowControl", port_settings->SettingsPort.flowControl},
        {"name", port_settings->SettingsPort.name},
        {"pairy", port_settings->SettingsPort.parity},
        {"stopBits", port_settings->SettingsPort.stopBits}
    };
    QJsonDocument doc(sett_json);
    QString strJson(doc.toJson(QJsonDocument::Compact));
    add_object->setSettings(azsNum, strJson, db);
}

void MainWindow::startConfigurate() {
    int errcount=0;
    int allcount=0;
    QString conf_answ;
    QSqlQuery query(db);  // Запрос к БД
    int i; // Для перебора
    float pr;
    Port port(port_settings);
    QSet<int> disps_id_set;    // Список сторон
    if (!port.StartComm()) return;
    query.exec(QString("SELECT ADMINPASSWORD FROM OBJECT_TABLES WHERE OBJECTID=%1").arg(azsNum));
    query.next();
    QString ps = query.value("ADMINPASSWORD").toString();
    if (port.changeAccess(1, ps)) {
// Записываем топлива
      query.exec("SELECT FUELDID,PRICE,VIEWNAME FROM FUELS ORDER BY FUELDID");
      i=0;
      while(query.next()) {
          QString data = QString("set Установки.ВидыТоплива[%1]:").arg(QString::number(i));
          pr=query.value("PRICE").toFloat();
          QString json_data = QString("{Топливо:%1 Цена:%2 Наименование:\"%3\"}")
                  .arg(query.value("FUELDID").toString())
                  .arg(QString::number(pr))
                  .arg(query.value("VIEWNAME").toString());
          data += json_data;
// В data - строка для обмена
          allcount++;
          conf_answ = port.write(data);
          if (conf_answ.indexOf("OK") < 0) {
              qDebug() << "error on configurate fuel #" << i;
              errcount++;
          }
          i++;
          if (i == 8) break;
      }
      while (i < 8) {
          QString data = QString("set Установки.Резервуары[%1]:{Резервуар:0 Топливо:0 Адрес:0}").arg(QString::number(i));
          port.write(data);
          i++;
      }
// Записываем резервуары
      query.exec(QString("SELECT ID,FUELID,SENDADDR FROM tanks WHERE OBJECTID=%1 ORDER BY ID").arg(azsNum));
      i=0;
      while(query.next()) {
          QString data = QString("set Установки.Резервуары[%1]:").arg(QString::number(i));
          QString json_data = QString("{Резервуар:%1 Топливо:%2 Адрес:%3}")
                  .arg(query.value(0).toString())
                  .arg(query.value(1).toString())
                  .arg(query.value(2).toString());
          data += json_data;
// В data - строка для обмена
          allcount++;
          conf_answ = port.write(data);
          if (conf_answ.indexOf("OK") < 0) {
              qDebug() << "error on configurate tank #" << i;
              errcount++;
          }
          i++;
          if (i == 8) {
              break;
          }
      }
      while (i < 8) {
          QString data = QString("set Установки.Резервуары[%1]:{Резервуар:0 Топливо:0 Адрес:0}").arg(QString::number(i));
          port.write(data);
          i++;
      }
// Записываем колонки
      query.exec(QString("SELECT DISPSID FROM points WHERE OBJECTID=%1").arg(azsNum));
      while(query.next()) {
          disps_id_set.insert(query.value("DISPSID").toInt());
      }
      QList<int> disps_id_list = disps_id_set.toList();
      int p = 0;
      for (int i=0; i<disps_id_list.size(); i++) {
          p++;
          query.exec((QString("SELECT WORKFLAG, SENDADDR, TANKID FROM points WHERE dispsid=%1 AND OBJECTID=%2").arg(disps_id_list[i]).arg(azsNum)));
  //
          QString data = QString("set Установки.Колонки[%1]:{Рукава[]:[").arg(QString::number(i));
          QString mid_data="";
          int j = 0;
          while(query.next()) {
              j++;
              mid_data += QString("{Работа:%1 Адрес:%2 Резервуар:%3},")
                      .arg(query.value("WORKFLAG").toString())
                      .arg(query.value("SENDADDR").toString())
                      .arg(query.value("TANKID").toString());
              if (j == 5) {
                  break;
              }
          }
          while (j < 5) {
              mid_data += "{Работа:0 Адрес:0 Резервуар:0},";
              j++;
          }
          mid_data = mid_data.mid(0, mid_data.length()-1);
          data += mid_data;
          data.append("]}");

          allcount++;
          conf_answ = port.write(data);
          if (conf_answ.indexOf("OK") < 0) {
              qDebug() << "error on configurate point #" << i;
              errcount++;
          }
          if (p == 12) {
              break;
          }
      }

      while (p < 12) {
          QString data = QString("set Установки.Колонки[%1]:{Рукава[]:[").arg(QString::number(p));
          for (int i=0; i<5; i++) {
              data+= "{Работа:0 Адрес:0 Резервуар:0},";
          }
          data = data.mid(0, data.length()-1);
          data.append("]}");
          port.write(data);
          p++;
      }

      port.changeAccess(0,"");
      mbx->setWindowTitle("Конфигурация");
      if (errcount==0) {
        mbx->setText("Конфигурация прошла успешно");
      } else {
        mbx->setText("Конфигурация прошла с ошибками");
      }
      mbx->exec();
    } else {
      mbx->setWindowTitle("Конфигурация");
      mbx->setText("Не удалось перевести устройство в режим администратора");
      mbx->exec();
    }
    port.EndComm();
}

QString convertData(QString data) {
    QString _Data = data.split("-")[2];
    _Data.append(".");
    _Data.append(data.split("-")[1]);
    _Data.append(".");
    _Data.append(data.split("-")[0]);
    return _Data;
}

////////////////////////////////
// Всё что касается пользователей

// КАЖЕТСЯ, эта функция вызывается для обновления данных пользователей на устройстве
void MainWindow::getUserIndex() {
    Port port(port_settings);
    QVector<int> hd_indexes;  // Индексы в устройстве до синхронизации
//    QVector<int> del_indexes; // Индексы на удаление
//    QVector<int> sync_indexes;// Обновленные индексы
    QSqlQuery query(db);
    JsonConvertor conv;
    QString userNewLimits;
    QString user_answ;   // Ответ от устройства
    QString userNewData;
    if (!port.StartComm()) return;
//QByteArray query = "get UserIndex";
    QString answ = port.write("get UserIndex");
    int arr_start, arr_len;
    parseArray(answ.toUtf8(), arr_start, arr_len);

// Перебираем все элементы массива индексов
    for (int i = arr_start; i < arr_start + arr_len; i++) {
      QString middle_answ = port.write("get UserIndex[" + QByteArray::number(i) + "]");
      QString json = conv.dataToJson(middle_answ.toUtf8());
      QJsonObject obj = QJsonDocument::fromJson(json.toUtf8()).object();
      hd_indexes.append(obj["UserID"].toInt());
    }

// Перебираем все записи базы данных
    QString statament = QString("SELECT USERID, PARENTID, VIEWNAME, FLAGS, CARDID, SLDATE, REFSLIM FROM users"); //.arg(QString::number(userId));
    query.exec(statament);
    while (query.next()) {
      int base_uid=query.value("USERID").toInt();
      userNewData = QString("set UserArr[%1]:{ UserID:%2 ParentID:%3 ViewName:\"%4\" Flags:%5 CardID:%6 SLData:\"%7\" RefrLim:%8 ")
              .arg(query.value("USERID").toString())  //QString::number(userId))
              .arg(query.value("USERID").toString())
              .arg(query.value("PARENTID").toString())
              .arg(query.value("VIEWNAME").toString())
              .arg(query.value("FLAGS").toString())
              .arg(query.value("CARDID").toString())
              .arg(convertData(query.value("SLDATE").toString()))
              .arg(query.value("REFSLIM").toString());
      if (query.value("REFSLIM").toInt()) {
// Обновляем лимиты
        int limcnt=0;
        userNewLimits = QString("Limits[]:["); //.arg(QString::number(userId));
        QSqlQuery limits_query(db);
        QString limits_statament = QString("SELECT FUELID, TYPED, DAYSD, VALUEL FROM limits"); // WHERE userid=%1").arg(QString::number(userId));
        limits_query.exec(limits_statament);
        QString userLimits = "";
        while(limits_query.next()) {
            limcnt++;
            userLimits += QString("{fID:%1 Tp:%2 Dy:%3 Vl:%5},")
                    .arg(limits_query.value("FUELID").toString())
                    .arg(limits_query.value("TYPED").toString())
                    .arg(limits_query.value("DAYSD").toString())
                    .arg(limits_query.value("VALUEL").toString());
        }
        while (limcnt<5) {
          limcnt++;
          userLimits += QString("{fID:0 Tp:0 Dy:0 Vl:0},");
        }
        userLimits = userLimits.mid(0, userLimits.length()-1);
        userNewLimits.append(userLimits);
        userNewLimits.append("]");
      } else {
// Обновляем только данные
        userNewLimits="";
      }
      userNewData.append(userNewLimits);
      userNewData.append(" }");
// Записываем данные пользователя в базу, при этом удаляем данную запись из списка индексов
      user_answ = port.write(userNewData.toUtf8());
// НУЖНО ПРОАНАЛИЗИРОВАТЬ ОТВЕТ
      int idpos=hd_indexes.indexOf(base_uid);
      if (idpos>=0) {
        hd_indexes.remove(idpos);  // Выкидываем этот индекс
      }
    }
// Пользователи обновлены. Удаляем оставшихся пользователей
    int delindcount=hd_indexes.size();
    for (int di=0;di<delindcount;di++) {
      QString userNewData = QString("set UserArr[%1]:{UserID:0 ParentID:0 ViewName:\"\" Flags:0 CardID:0}").arg(QString::number(hd_indexes[di]));
      user_answ = port.write(userNewData.toUtf8());
    }
    port.EndComm();
}

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
    card_dialog->show();
}

void MainWindow::changeUserCard() {
    int state = 0;
    timer_end = 0;
    QString answ;
    QDateTime dt = QDateTime::currentDateTime();
    if (!card_reader->StartComm()) return;
    while (state < 1) {
        //таймер на 5 секунд
        if (QDateTime::currentDateTime().toSecsSinceEpoch() - dt.toSecsSinceEpoch() > 5) break;
        if (stopCard) break;
        answ = card_reader->writeData("get RFInfo");
        state = getValueFromState(answ, "State:").toInt();
    }

    qDebug() << answ;

    if (state != 2) {
        qDebug() << "State not 2";
    } else {
      QString ID = getValueFromState(answ, "ID:");
      QString card_id = dt.toString("dhhmmss");
    //запись в БД
      QSqlQuery* change_card_id_query = new QSqlQuery(db);
      QString statament = QString("UPDATE users SET cardid=%1 WHERE userid=%2").arg(card_id).arg(selected_user_id);
      change_card_id_query->exec(statament);

      if (change_card_id_query->lastError().isValid()) {
          qDebug() << change_card_id_query->lastError().databaseText();
      }
      delete change_card_id_query;

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
      } else {
        answ = card_reader->writeData("run RFRead:{ Key:\"B\" Addr:1}");
        answ = card_reader->writeData("get RFInfo");
        QString _final = getValueFromState(answ, "Data:");
        if (QString::compare(cart_data, _final)) {
          qDebug() << "Error. Date not equal.";
        }
        qDebug() << "Success. Card changed.";
      }
    }
    card_reader->EndComm();
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
    return finally;
}

void MainWindow::parseArray(QByteArray dataArr, int &start, int &len) {
    JsonConvertor conv;
    QString json = conv.dataToJson(dataArr);
    QJsonDocument jsonDoc = QJsonDocument::fromJson(json.toUtf8());
    QJsonObject obj = jsonDoc.object();
    start = obj["start"].toInt();
    len = obj["length"].toInt();
}

QJsonValue MainWindow::getJSONField(QByteArray data,QString fieldname) {
  JsonConvertor conv;
  QString json = conv.dataToJson(data);
  QJsonDocument jsonDoc = QJsonDocument::fromJson(json.toUtf8());
  QJsonObject obj = jsonDoc.object();
  return obj[fieldname];
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
