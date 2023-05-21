#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QDebug>
#include <QSqlError>
#include <QMessageBox>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::TodoList)
{
    ui->setupUi(this);
    setWindowTitle("Список задач");
    QIcon icon(":/todo-list.ico");
    this->setWindowIcon(icon);

    // Создаем объект БД
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");

    // Указываем имя файла БД
    db.setDatabaseName("todo_list.db");

    // Открываем БД
    if (!db.open())
    {
        qDebug() << "Ошибка открытия базы данных";
        return;
    }

    // Создаем таблицу задач, если её еще нет
    QSqlQuery query;
    if (!query.exec("CREATE TABLE IF NOT EXISTS tasks ("
                    "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                    "name TEXT,"
                    "isFinished BOOLEAN)"))
    {
        qDebug() << "Ошибка создания таблицы: " << query.lastError().text();
    }
    // Загружаем задачи из БД
    loadTasksFromDb();
    ui->tabWidget->setCurrentIndex(0);

    // Подключаем обработчик нажатия на кнопку добавления задачи
    connect(ui->clearList, SIGNAL(clicked()), this, SLOT(on_clearButton_clicked()));
    connect(ui->addTask, SIGNAL(clicked()), this, SLOT(on_addTaskButton_clicked()));
    connect(ui->unfinishedTasksList, SIGNAL(itemActivated(QListWidgetItem*)), this, SLOT(on_unfinishedTasksList_itemClicked(QListWidgetItem*)));
    connect(ui->taskNameEdit, SIGNAL(returnPressed()), this, SLOT(on_newTaskLineEdit_returnPressed()));

}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_clearButton_clicked()
{
    QMessageBox msgBox;
    msgBox.setWindowTitle("Очищение списка");
    msgBox.setText("Вы уверены, что хотите очистить список?");
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    msgBox.setDefaultButton(QMessageBox::No);

    int result = msgBox.exec();
    if (result == QMessageBox::Yes) {
        clearAll();
    }
    else if (result == QMessageBox::No) {
        msgBox.close();
    }
}

void MainWindow::clearAll()
{
    // удаляем все задачи из списка
    ui->unfinishedTasksList->clear();
    ui->finishedTasksList->clear();

    // очищаем БД
    QSqlQuery query;
    query.exec("DELETE FROM tasks");
}

void MainWindow::updateTasksTab()
{
    // Очищаем списки задач
    ui->unfinishedTasksList->clear();
    ui->finishedTasksList->clear();

    // Получаем все задачи из БД
    QSqlQuery query;
    query.prepare("SELECT id, name, isFinished FROM tasks ORDER BY id DESC");
    if (!query.exec()) {
        QMessageBox::critical(this, "Ошибка", "Не удалось получить задачи из БД: " + query.lastError().text());
        return;
    }

    // Добавляем задачи в соответствующие списки
    while (query.next()) {
        int id = query.value(0).toInt();
        QString name = query.value(1).toString();
        bool isFinished = query.value(2).toBool();

        QListWidgetItem* item = new QListWidgetItem(name);

        if (isFinished) {
            ui->finishedTasksList->addItem(item);
        } else {
            ui->unfinishedTasksList->addItem(item);
        }
    }
}

void MainWindow::loadTasksFromDb()
{
    QSqlQuery query("SELECT * FROM tasks");

    while (query.next()) {
        QString taskName = query.value("name").toString();
        bool isFinished = query.value("isFinished").toBool();
        int taskId = query.value("id").toInt();

        QListWidgetItem* taskItem = new QListWidgetItem(taskName);
        taskItem->setData(Qt::UserRole, taskId);
        taskItem->setFlags(taskItem->flags() | Qt::ItemIsEditable);

        if (isFinished) {
            ui->finishedTasksList->insertItem(0,taskItem);
        } else {
            ui->unfinishedTasksList->insertItem(0,taskItem);
        }
    }
}

void MainWindow::on_addTask_clicked()
{
    QString taskName = ui->taskNameEdit->text();

    if (taskName.isEmpty()) {
        QMessageBox::warning(this, "Предупреждение", "Введите название задачи.");
        return;
    }

    QSqlQuery query;
    query.prepare("INSERT INTO tasks (name) VALUES (:name)");
    query.bindValue(":name", taskName);
    if (!query.exec()) {
        QMessageBox::critical(this, "Ошибка", "Не удалось добавить задачу: " + query.lastError().text());
        return;
    }

    ui->taskNameEdit->clear();
    updateTasksTab();
}

void MainWindow::on_newTaskLineEdit_returnPressed()
{
    on_addTask_clicked();
}

void MainWindow::completeTask()
{
    // получаем выбранную задачу из списка невыполненных задач
    QListWidgetItem* taskItem = ui->unfinishedTasksList->currentItem();
    if (!taskItem) {
        return; // если ничего не выбрано, выходим из функции
    }

    // перемещаем задачу в список выполненных задач
    ui->unfinishedTasksList->takeItem(ui->unfinishedTasksList->row(taskItem));
    ui->finishedTasksList->addItem(taskItem);

    // изменяем значение поля isFinished в базе данных
    int taskId = taskItem->data(Qt::UserRole).toInt();
    QSqlQuery query;
    query.prepare("UPDATE tasks SET isFinished = :isFinished WHERE id = :id");
    query.bindValue(":id", taskId);
    query.bindValue(":isFinished", true);
    query.exec();

}

void MainWindow::on_unfinishedTasksList_itemClicked(QListWidgetItem *item)
{

    int taskId = item->data(Qt::UserRole).toInt();

    QMessageBox msgBox;
    msgBox.setWindowTitle("Завершение задачи");
    msgBox.setText("Вы уверены, что хотите завершить выбранную задачу?");
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    msgBox.setDefaultButton(QMessageBox::No);

    msgBox.button(QMessageBox::Yes)->setText("Да");
    msgBox.button(QMessageBox::No)->setText("Нет");

    int result = msgBox.exec();
    if (result == QMessageBox::Yes) {
        completeTask();
    }
    else if (result == QMessageBox::No) {
        msgBox.close();
    }
}
