#include <iostream>
#include <QLabel>
#include <QPainter>
#include <time.h>
#include <QTimer>

#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    statMsg = new QLabel(ui->statusBar);
    statMsg->setFrameStyle(QFrame::StyledPanel);
    statScore = new QLabel(ui->statusBar);
    statScore->setFrameStyle(QFrame::StyledPanel);
    ui->statusBar->addPermanentWidget(statMsg,3);
    ui->statusBar->addPermanentWidget(statScore,1);
    inPlay = false;
    strMessage = "Ready to play!";
    gameScore = 0;

    srandom(time(NULL));
    for (int i = 0; i < 9; i++)
    {
        gameStat[i] = (random() % 2)+1;
        connect(cell(i),SIGNAL(clicked(bool)),this,SLOT(squareClicked()));
        cell(i)->setIconSize(QSize(250,250));
    }
    connect(ui->actionNew,SIGNAL(triggered(bool)),this,SLOT(newGame()));

    QPixmap pmX(250,250);
    pmX.fill(Qt::transparent);
    QPainter p(&pmX);
    QPen pn;
    pn.setWidth(25);
    pn.setColor(Qt::black);
    pn.setCapStyle(Qt::RoundCap);
    p.setPen(pn);
    p.drawLine(30,30,220,220);
    p.drawLine(30,220,220,30);
    p.end();
    icons[1] = QIcon(pmX);

    QPixmap pmO(250,250);
    pmO.fill(Qt::transparent);
    QPainter po(&pmO);
    QPen pno;
    pno.setWidth(25);
    pno.setColor(Qt::black);
    pno.setCapStyle(Qt::RoundCap);
    po.setPen(pno);
    po.drawEllipse(20,20,210,210);
    po.end();
    icons[2] = QIcon(pmO);
    renderDisplay();

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::squareClicked()
{
    int i = -1;
    if (sender() == (QObject *)ui->cell0) i = 0;
    if (sender() == (QObject *)ui->cell1) i = 1;
    if (sender() == (QObject *)ui->cell2) i = 2;
    if (sender() == (QObject *)ui->cell3) i = 3;
    if (sender() == (QObject *)ui->cell4) i = 4;
    if (sender() == (QObject *)ui->cell5) i = 5;
    if (sender() == (QObject *)ui->cell6) i = 6;
    if (sender() == (QObject *)ui->cell7) i = 7;
    if (sender() == (QObject *)ui->cell8) i = 8;

    if (i == -1 || gameStat[i] || !inPlay)
        return;
    gameStat[i] = 1;
    gettimeofday(&pEnd,NULL);
    QTimer::singleShot(0,this,SLOT(gameplayAction()));
    renderDisplay();
}

void MainWindow::gameplayAction()
{
    inPlay = false;
    double bt = pStart.tv_sec;
    double et = pEnd.tv_sec;
    double btu = pStart.tv_usec;
    double etu = pEnd.tv_usec;
    btu /= 1000000.0;
    etu /= 1000000.0;
    bt += btu;
    et += etu;
    et -= bt;
    gameScore += 500.0 / et;
    winState = checkWin();
    renderDisplay();
    if (winState)
        return;

    unsigned long long gstate = 0;
    for (int i = 0; i < 9; i++)
    {
        gstate <<= 2;
        gstate |= gameStat[i];
    }
    int cm = computerMove(gstate);
    if (cm >= 0)
    {
        gameStat[cm] = 2;
    } else {
        strMessage = "Cats Game!";
        renderDisplay();
        return;
    }
    winState = checkWin();
    renderDisplay();
    if (winState)
        return;
    inPlay = true;
    gettimeofday(&pStart,NULL);
}

int MainWindow::exploreRecurs(unsigned long long gst, int nm, int lvl)
{
    int rv = 0;
    if ((rv = checkComputerWin(gst)))
        return rv * lvl;
    if (lvl == 1000000)
        lvl = 9;
    nm = (nm == 1)?2:1;
    unsigned long long ogst = gst;
    for (int i = 8; i >= 0; i--)
    {
        if ((gst & 3) == 0)
            rv += exploreRecurs(ogst | (nm << ((8-i)*2)),nm,lvl-1);
        gst >>= 2;
    }
    return rv;
}

int MainWindow::computerMove(unsigned long long gst)
{
    QMap<int,int> possibleMoves;
    unsigned long long ogst = gst;
    for (int i = 8; i >= 0; i--)
    {
        if ((gst & 3) == 0)
            possibleMoves[i] = exploreRecurs(ogst | (2 << ((8-i)*2)),2);
        gst >>= 2;
    }
    int highest = -2000000000;
    int rv = -1;
    auto pm = possibleMoves.begin();
    QVector<int> bestMoves;
    while (pm != possibleMoves.end())
    {
        if (pm.value() == highest)
            bestMoves.append(pm.key());
        if (pm.value() > highest)
        {
            highest = pm.value();
            rv = pm.key();
            bestMoves.clear();
            bestMoves.append(pm.key());
        }
        pm++;
    }
    srandom(time(NULL));
    if (bestMoves.length() > 1)
    {
        rv = bestMoves.at(random() % bestMoves.length());
    }
    return rv;
}

int MainWindow::checkComputerWin(unsigned long long g)
{
    unsigned char gs[9];
    for (int i = 8; i >= 0; i--)
    {
        gs[i] = g & 3;
        g >>= 2;
    }
    if (gs[0] && gs[0] == gs[1] && gs[0] == gs[2])
        return (gs[0] == 2)?1:-1;
    if (gs[3] && gs[3] == gs[4] && gs[3] == gs[5])
        return (gs[3] == 2)?1:-1;
    if (gs[6] && gs[6] == gs[7] && gs[6] == gs[8])
        return (gs[6] == 2)?1:-1;
    if (gs[0] && gs[0] == gs[3] && gs[0] == gs[6])
        return (gs[0] == 2)?1:-1;
    if (gs[1] && gs[1] == gs[4] && gs[4] == gs[7])
        return (gs[1] == 2)?1:-1;
    if (gs[2] && gs[2] == gs[5] && gs[2] == gs[8])
        return (gs[2] == 2)?1:-1;
    if (gs[0] && gs[0] == gs[4] && gs[4] == gs[8])
        return (gs[0] == 2)?1:-1;
    if (gs[2] && gs[2] == gs[4] && gs[4] == gs[6])
        return (gs[6] == 2)?1:-1;
    return 0;
}

unsigned int MainWindow::checkWin()
{
    if (gameStat[0] && gameStat[0] == gameStat[1] && gameStat[0] == gameStat[2])
        return 0x000200 + gameStat[0];
    if (gameStat[3] && gameStat[3] == gameStat[4] && gameStat[3] == gameStat[5])
        return 0x030500 + gameStat[3];
    if (gameStat[6] && gameStat[6] == gameStat[7] && gameStat[6] == gameStat[8])
        return 0x060800 + gameStat[6];
    if (gameStat[0] && gameStat[0] == gameStat[3] && gameStat[0] == gameStat[6])
        return 0x000600 + gameStat[0];
    if (gameStat[1] && gameStat[1] == gameStat[4] && gameStat[4] == gameStat[7])
        return 0x010700 + gameStat[1];
    if (gameStat[2] && gameStat[2] == gameStat[5] && gameStat[2] == gameStat[8])
        return 0x020800 + gameStat[2];
    if (gameStat[0] && gameStat[0] == gameStat[4] && gameStat[4] == gameStat[8])
        return 0x000800 + gameStat[0];
    if (gameStat[2] && gameStat[2] == gameStat[4] && gameStat[4] == gameStat[6])
        return 0x020600 + gameStat[6];
    return 0;
}

void MainWindow::paintEvent(QPaintEvent *e)
{
    QMainWindow::paintEvent(e);
    if (winState == 0)
        return;

    int wnr = winState & 0xff;
    int c1 = (winState >> 8) & 0xff;
    int c2 = (winState >> 16) & 0xff;

    QPainter p(this);
    QPen pn;
    if (wnr == 1)
    {
        pn.setColor(Qt::green);
    } else {
        pn.setColor(Qt::red);
    }
    pn.setWidth(50);
    pn.setCapStyle(Qt::RoundCap);
    p.setPen(pn);
    QPoint p1 = cell(c1)->mapToParent(cell(c1)->rect().center());
    QPoint p2 = cell(c2)->mapToParent(cell(c2)->rect().center());
    p1.setY(p1.y() + ui->menuBar->height());
    p2.setY(p2.y() + ui->menuBar->height());
    p.drawLine(p1,p2);
    p.end();
}

void MainWindow::newGame()
{
    memset(gameStat,0,sizeof(gameStat));
    inPlay = true;
    gettimeofday(&pStart,NULL);
    winState = 0;
    gameScore = 0;
    renderDisplay();
}

void MainWindow::renderDisplay()
{
    statMsg->setText(strMessage);
    statScore->setText(QString("Score: %1").arg(gameScore));
    for (int i = 0; i < 9; i++)
        cell(i)->setIcon(icons[gameStat[i]]);
}

QPushButton *MainWindow::cell(int idx)
{
    switch(idx)
    {
        case 0: return ui->cell0; break;
        case 1: return ui->cell1; break;
        case 2: return ui->cell2; break;
        case 3: return ui->cell3; break;
        case 4: return ui->cell4; break;
        case 5: return ui->cell5; break;
        case 6: return ui->cell6; break;
        case 7: return ui->cell7; break;
        case 8: return ui->cell8; break;
    }
    return NULL;
}
