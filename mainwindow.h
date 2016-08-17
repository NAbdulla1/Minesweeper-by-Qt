#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    QWidget *centralWidget;

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    QPushButton *buttons[8][8];
    QGridLayout *gameGrid;
    QVBoxLayout *vbox;
    void createLayout();

private slots:
    void buttonPressed(QPushButton*);
    void on_actionQuit_triggered();

    void on_actionInstructions_triggered();

    void on_actionAbout_triggered();

    void on_actionAbout_Dev_triggered();

private:
    Ui::MainWindow *ui;
    int totalMines = 16;
    char minePositions[8][8];
    bool visited[8][8];
    bool isFirst;
    void setMines();
    void calc();
    int countVis();
    void update(int, int);
    void setView(int, int);
    void setView();
    void bfs(QPair<int, int>, int);
    void failMessage();
    void successMessage();
};

#endif // MAINWINDOW_H
