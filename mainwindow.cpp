#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QLCDNumber>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <algorithm>
#include <queue>
#include <utility>
#include <QMessageBox>
#include <QGridLayout>
#include <QPalette>
#include <QColor>
#include <QLabel>

#define isValid(a, b) ((a >= 0 && a < b) ? 1 : 0)
int dr[]  =  {0, -1, -1, -1,  0,  1, 1, 1};
int dc[]  =  {1,  1,  0, -1, -1, -1, 0, 1};

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    createLayout();
    memset(minePositions, '0', sizeof(minePositions));
    memset(visited, 0, sizeof(minePositions));
    setMines();
    calc();
    isFirst = true;
}

MainWindow::~MainWindow()
{
    delete ui;
}

void buttonColor(int c, QPushButton *b)
{
    QColor col;
    switch(c){
    case -1:
        col.setRgb(0,0,255);
        break;
    case 0:
        col.setRgb(0,255,0);
        break;
    case 1:
        col.setRgb(255, 0, 0, 105);
        break;
    case 2:
        col.setRgb(255, 0, 0, 105);
        break;
    case 3:
        col.setRgb(255, 0, 0, 155);
        break;
    case 4:
        col.setRgb(255, 0, 0, 155);
        break;
    case 5:
        col.setRgb(255, 0, 0, 205);
        break;
    case 6:
        col.setRgb(255, 0, 0, 205);
        break;
    case 7:
        col.setRgb(255, 0, 0, 255);
        break;
    case 8:
        col.setRgb(255, 0, 0, 255);
        break;
    }

    QPalette pal = b->palette();
    pal.setColor(QPalette::Button, col);
    pal.setColor(QPalette::ButtonText, col);
    b->setAutoFillBackground(true);
    b->setPalette(pal);
    b->update();
}

void MainWindow::createLayout()
{
    centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

//    vbox = new QVBoxLayout(centralWidget);

//    vbox2 = new QVBoxLayout();
//    welcome = new QLabel("Welcome to MineSweeper");
//    cont = new QPushButton("Lets Start");
//    vbox2->addWidget(welcome);
//    vbox2->addWidget(cont);
//    connect(cont,SIGNAL(clicked()),this,SLOT(contPressed()));
//    vbox->addLayout(vbox2);
//    centralWidget->setLayout(vbox);

    gameGrid = new QGridLayout(centralWidget);
    gameGrid->setSpacing(1);
    for(int i = 0; i < 8; i++){
        for(int j = 0; j < 8; j++){
            buttons[i][j] = new QPushButton("");
            buttons[i][j]->setFixedHeight(30);
            buttons[i][j]->setFixedWidth(30);
            connect(buttons[i][j], &QPushButton::clicked, this, [this, i, j]{buttonPressed(buttons[i][j]);});
            gameGrid->addWidget(buttons[i][j], i, j, 1, 1);
        }
    }
    centralWidget->setLayout(gameGrid);
}

//void MainWindow::contPressed()
//{
//    vbox2->removeWidget(welcome);
//    vbox2->removeWidget(cont);
//   // vbox->remo
//    vbox->addLayout(gameGrid);
//}

QPair<int,int> gridPosition(QWidget * widget) {
    auto gp = qMakePair(-1,-1);
    if (! widget->parentWidget()) return gp;
    auto layout = dynamic_cast<QGridLayout*>(widget->parentWidget()->layout());
    if (! layout) return gp;
    int index = layout->indexOf(widget);
    Q_ASSERT(index >= 0);
    int rs,cs;
    layout->getItemPosition(index, &gp.first, &gp.second, &rs, &cs);
    return gp;
}

void MainWindow::buttonPressed(QPushButton* b)
{
    QPair<int, int>pos = gridPosition(b);
    //b->setText(QString("%1,%2").arg(pos.first).arg(pos.second));
    if(isFirst){
        isFirst = false;
        if(minePositions[pos.first][pos.second] == '*'){//clicked in a mine field
            buttonColor(-1, b);
            failMessage();
        }
        else{
            srand(time(NULL));
            bfs(pos, 16+abs(rand())%25);
        }
    }
    else{
        if(minePositions[pos.first][pos.second] == '*'){//clicked in a mine field
            buttonColor(-1, b);
            failMessage();
        }
        else{
            bool f = true;
            for(int i = 0; i < 8; i++){
                if(visited[pos.first+dr[i]][pos.second+dc[i]]){
                    f = false;
                    break;
                }
            }
            if(f){
                srand(time(NULL));
                int h = 4+abs(rand())%5;
                if(countVis()+h < 64-totalMines-8)
                    bfs(pos, h);
                else{
                    visited[pos.first][pos.second] = true;
                    setView(pos.first, pos.second);
                    if(countVis() >= 64-totalMines){
                        successMessage();
                    }
                }
            }
            else{
                visited[pos.first][pos.second] = true;
                setView(pos.first, pos.second);
                if(countVis() >= 64-totalMines){
                    successMessage();
                }
            }
        }
    }
}

void MainWindow::setMines()
{
    srand(time(NULL));
    for(int i = 0; i < totalMines;){
        int x = abs(rand())%8;
        int y = abs(rand())%8;
        if(minePositions[x][y] != '*'){
            minePositions[x][y] = '*';
            i++;
        }
    }
}

void MainWindow::calc()
{
    int i, j;
    for(i = 0; i < 8; i++){
       for(j = 0; j < 8; j++){
           if(minePositions[i][j] == '*'){//we found a mine
                update(i, j);
           }
       }
   }
}

void MainWindow::update(int r, int c)
{
    int i;
    for(i = 0; i < 8; i++){
        int nr = r + dr[i];
        int nc = c + dc[i];
        if(isValid(nr, 8) && isValid(nc, 8) && minePositions[nr][nc] != '*'){
            int curr = minePositions[nr][nc] - '0';
            curr++;
            minePositions[nr][nc] = curr + '0';
        }
    }
}

void MainWindow::setView(int x, int y)
{
    QLayoutItem *l;
    QWidget *w;
    QPushButton *b;

    l = gameGrid->itemAtPosition(x, y);
    w = l->widget();
    b = dynamic_cast<QPushButton*>(w);
    b->setText(QString(minePositions[x][y]));
    buttonColor(minePositions[x][y]-'0', b);

    /*for(int i = 0; i < 8; i++){
        for(int j = 0; j < 8; j++){
            if(visited[i][j]){
                l = ui->gameGrid->itemAtPosition(i, j);
                w = l->widget();
                b = dynamic_cast<QPushButton*>(w);
                b->setText(QString(minePositions[i][j]));
            }
        }
    }*/
}

void MainWindow::setView()
{
    QLayoutItem *l;
    QWidget *w;
    QPushButton *b;
    for(int i = 0; i < 8; i++){
        for(int j = 0; j < 8; j++){
            l = gameGrid->itemAtPosition(i, j);
            w = l->widget();
            b = dynamic_cast<QPushButton*>(w);
            b->setText(QString(minePositions[i][j]));
        }
    }
}

int MainWindow::countVis()
{
    int r = 0;
    for(int i = 0; i < 8; i++){
        for(int j = 0; j < 8; j++)
            if(visited[i][j])
                r++;
    }
    return r;
}

void MainWindow::bfs(QPair<int,int> pa, int n)
{
    std::queue<QPair<int, int> > Q;
    Q.push(pa);
    visited[pa.first][pa.second] = true;
    setView(pa.first, pa.second);
    int c = 0;
    while(!Q.empty() && c <= n){
        QPair<int, int> par = Q.front();
        Q.pop();
        for(int i = 0; i < 8; i++){
            int nr = par.first + dr[i];
            int nc = par.second + dc[i];
            if(isValid(nr, 8) && isValid(nc, 8) && c <= n && minePositions[nr][nc] != '*'){
                visited[nr][nc] = true;
                c++;
                Q.push(qMakePair(nr, nc));
                setView(nr, nc);
            }
        }
    }
}

void MainWindow::failMessage()
{
    setView();
    show();
    QMessageBox ms;
    ms.setText(QString("You Lose!\nTo Play again, restart the game."));
    ms.exec();
    exit(1);
}

void MainWindow::successMessage()
{
    show();
    QMessageBox ms;
    ms.setText(QString("You Win!!!!\nTo Play again, restart the game."));
    ms.exec();
    exit(1);
}

void MainWindow::on_actionQuit_triggered()
{
    exit(1);
}

void MainWindow::on_actionInstructions_triggered()
{
    QMessageBox instruction;
    instruction.setWindowTitle("Instructions to Play");
    instruction.setText("There are 16 hidden mines in this game.\nTo Play this game:\n\
                        1. press in any cell in the grid.\n\
                        2. if the cell contains any mine you lose.\n\
                        3. if the cell is empty it will show some cell.\n\
                        4. in each gell the number indicates the number of\n\t\t mines' influence in it.\n\
                        5. a mine's influence is in it's 8 adjacent cells.\n\
                        6. your goal is to unhide all cells that does not contain mines.");
    instruction.exec();
}

void MainWindow::on_actionAbout_triggered()
{
    QMessageBox about;
    about.setWindowTitle("About Game");
    about.setText("This is a single-player puzzle game.\n\
                  The objective of the game is to clear\n\
                  a rectangular board containing hidden\n\
                  \"mines\" without detonating any of\n\
                  them, with help from clues about the number\n\
                  of neighboring mines in each field.");
    about.exec();
}

void MainWindow::on_actionAbout_Dev_triggered()
{
    QMessageBox about;
    about.setWindowTitle("About Developer");
    about.setText("Md. Abdulla Al Mamun\nID: 1306001\nSession: 2013-2014\nDepartment of Computer Science and Engineering\nBegum Rokeya University, Rangpur.");
    about.exec();
}
