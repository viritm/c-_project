
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtWidgets>



QT_BEGIN_NAMESPACE
namespace Ui { class TodoList; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow

{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

signals:

private slots:
    /**
     * Слот, обрабатывающий событие нажатия на кнопку добавления задачи.
     */
    void on_addTask_clicked();
    /**
     * Слот, завершающий задачу и перемещающий ее в список выполненных задач.
     */
    void completeTask();
    /**
     * Слот, обрабатывающий событие клика по элементу в списке невыполненных задач.
     */
    void on_unfinishedTasksList_itemClicked(QListWidgetItem *item);
    /**
     * Слот, обрабатывающий событие нажатия клавиши Enter в поле ввода новой задачи.
     */
    void on_newTaskLineEdit_returnPressed();
    /**
     * Слот, обрабатывающий событие нажатия на кнопку очистки.
     */
    void on_clearButton_clicked();

private:
    Ui::TodoList *ui;
    /**
     * Метод, обновляющий вкладку задач путем загрузки задач из базы данных.
     */
    void updateTasksTab();

    /**
     * Метод, загружающий задачи из базы данных и заполняющий списки.
     */
    void loadTasksFromDb();
    /**
     * Метод, очищающий все задачи из списка и базы данных.
     */
    void clearAll();

};

#endif // MAINWINDOW_H
