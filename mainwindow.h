#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QLabel>
#include <sys/time.h>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void squareClicked();
    void newGame();
    void gameplayAction();

private:
// My functions
    void renderDisplay();
    QPushButton *cell(int);
    unsigned int checkWin();
    int computerMove(unsigned long long);
    int exploreRecurs(unsigned long long,int,int lvl=1000001);
    int checkComputerWin(unsigned long long);

// UI Overrides
    void paintEvent(QPaintEvent *event);

//Data
    QString strMessage;
    uint32_t gameScore;
    unsigned char gameStat[9];
    QIcon icons[3];
    Ui::MainWindow *ui;
    QLabel *statMsg;
    QLabel *statScore;
    bool inPlay;
    struct timeval pStart,pEnd;
    unsigned int winState;

};

#endif // MAINWINDOW_H
