#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "process.h"
#include <QRandomGenerator>
#include <QtMath>
#include "fcfs.h"
#include "sjf.h"
#include "round_robin.h"
#include "round_robin_multiniveaux.h"
#include "edf.h"
#include <algorithm>
#include <QDebug>
#include <QPropertyAnimation>
#include <QGraphicsOpacityEffect>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow),
    current_scheduler(nullptr),
    is_auto_mode(false),
    current_algo_index(0)
{
    ui->setupUi(this);
    numberOfProcesses  = 10;
    ui->stackedWidget->setCurrentIndex(0);
}

MainWindow::~MainWindow()
{
    delete ui;
}
void MainWindow::on_btnPlay_clicked()
{
    is_auto_mode = true ;
    current_algo_index = 0;
    allStats.clear();
    processes = generateProcesses(numberOfProcesses);
    startNextAlgorithm();
}
void MainWindow::startNextAlgorithm()
{

    if (current_algo_index >= 5) {
            showStatistics();
            return;
     }

        runAlgorithm(current_algo_index);
}
void MainWindow::runAlgorithm(int index)
{
    if (current_scheduler != nullptr) {
           disconnect(current_scheduler, nullptr, this, nullptr);
           delete current_scheduler;
       }

       // Crée le scheduler
       switch(index) {
           case 0:
               current_scheduler = new fcfs(this);
               ui->algoName->setText("🔵 FCFS");
               break;
           case 1:
               current_scheduler = new sjf(this);
               ui->algoName->setText("🟢 SJF");
               break;
           case 2:
               current_scheduler = new round_robin(this);
               ui->algoName->setText("🟡 Round Robin");
               break;
           case 3:
                current_scheduler = new round_robin_multiniveaux(this);
                ui->algoName->setText("🟡 Round Robin multi-niveaux");
                break;
           case 4:  // ← NOUVEAU
               current_scheduler = new EDF(this);
               ui->algoName->setText("🔴 EDF (Earliest Deadline First)");
               break;
       }

       connectSignals();
       QVector<Process> cloned_processes = cloneProcesses(processes);
       current_scheduler->start(cloned_processes);
}
QVector<Process> MainWindow::cloneProcesses(const QVector<Process> &original) {
    QVector<Process> clones;  // Nouveau vecteur vide

    // Parcourt tous les processus originaux
    for (const Process &p : original) {
        // Crée un NOUVEAU processus avec les MÊMES valeurs
        Process clone(p.get_burst(),     // burst_time original
                     p.get_arrival(),    // arrival_time original
                     p.get_pid(),        // pid original
                     p.get_priority(),
                     p.get_deadline());  // priority original

        // Ajoute le clone au vecteur
        clones.append(clone);
    }

    return clones;  // Retourne la copie
}
void MainWindow::connectSignals()
{

    connect(current_scheduler, &AlgorithmBase::processArrived,
            this, &MainWindow::onProcessArrived);
    connect(current_scheduler, &AlgorithmBase::algorithmFinished,
            this, &MainWindow::onAlgorithmFinished);
    connect(current_scheduler , &AlgorithmBase::processCompleted ,
            this , &MainWindow::onProcessCompleted);
    connect(current_scheduler , &AlgorithmBase::processExecuting ,
            this , &MainWindow::onProcessExecuting);
}
void MainWindow::onProcessArrived(Process p) {
    int nb_col = 4;
    int row , col = 0;
    int i = p.get_pid() -1;
    // QString style = "border : 1px solid rgb(235,235,235)";
    QLabel * pid = new QLabel(QString("PID : %1").arg(p.get_pid()));
    QLabel * burst_left_ui = new QLabel(QString("Temps restant : %1 ms").arg(p.get_burst_left()));
    QLabel * priority_ui = new QLabel(QString("Priorité : %1 ").arg(p.get_priority()));
    QLabel * deadline_ui = new QLabel(QString("Echéance : %1 ms").arg(p.get_deadline()));
    /*pid->setStyleSheet(style);
    burst_left_ui->setStyleSheet(style);
    priority_ui->setStyleSheet(style); */
    QWidget *process_ui = new QWidget();
    process_ui->setObjectName(QString("process_ui_%1").arg(p.get_pid()));
    QVBoxLayout *process_layout = new QVBoxLayout(process_ui);
    process_ui->setFixedSize(181,140);
    process_layout->addWidget(pid);
    process_layout->addWidget(burst_left_ui);
    process_layout->addWidget(priority_ui);
    process_layout->addWidget(deadline_ui);


    pid->setAlignment(Qt::AlignCenter);
    burst_left_ui->setAlignment(Qt::AlignCenter);
    priority_ui->setAlignment(Qt::AlignCenter);
    //QString style2  = QString("QWidget#process_ui_%1{background-color : white; border : 1.5px solid; border-radius : 5px;");
     QString style2  ;
    switch (p.get_priority()) {
        case 0 :
            qDebug() << "ici";
            style2  = QString("QWidget#process_ui_%1{background-color : white; border : 1.5px solid; border-radius : 5px;border-color : %2} ").arg(p.get_pid()).arg("#DC3545");
            //style2.append(QString("border-color : %2}").arg(p.get_pid()).arg("#DC3545"));
        break;
    case 1 :
         style2  = QString("QWidget#process_ui_%1{background-color : white; border : 1.5px solid; border-radius : 5px;border-color : %2} ").arg(p.get_pid()).arg("#FF9F40");
    break;
    case 2:
         style2  = QString("QWidget#process_ui_%1{background-color : white; border : 1.5px solid; border-radius : 5px;border-color : %2} ").arg(p.get_pid()).arg("#FFC107");
    break;
    case 3:
         style2  = QString("QWidget#process_ui_%1{background-color : white; border : 1.5px solid; border-radius : 5px;border-color : %2} ").arg(p.get_pid()).arg("#28A745");
    break;
    case 4 :
         style2  = QString("QWidget#process_ui_%1{background-color : white; border : 1.5px solid; border-radius : 5px;border-color : %2} ").arg(p.get_pid()).arg("#DEE2E6");
    break;


    }
    process_ui->setStyleSheet(style2);
    row = i / nb_col;
    col = i % nb_col;
   if(row % 2 == 0) col = nb_col - 1 - col;
    ui->gl1->addWidget(process_ui, row , col);
}
void MainWindow::onProcessExecuting(Process p)
{
    QString style = "font-size : 18px; border : none";
    QWidget *q = this->findChild<QWidget*>(QString("running_process"));
    q->setStyleSheet("background-color : white; border : 2px solid gray;border-radius : 10px");
    QLabel *l = this->findChild<QLabel*>(QString("rp_id"));
    l->setText(QString("ID : %1").arg(p.get_pid()));
    QLabel *l2 = this->findChild<QLabel*>(QString("rp_priority"));
    l2->setText(QString("Priorité : %1").arg(p.get_priority()));
    QLabel *l3 = this->findChild<QLabel*>(QString("rp_burst_left"));
    l3->setText(QString("Temps restant : %1 ms").arg(p.get_burst_left()));
    QLabel *l4 = this->findChild<QLabel*>(QString("rp_waiting_time"));
    l4->setText(QString("Temps attentu : %1 ms").arg(p.get_total_wait()));

    l->setStyleSheet(style);
    l2->setStyleSheet(style);
    l3->setStyleSheet(style);
    l4->setStyleSheet(style);

    /*
    l->setAlignment(Qt::AlignCenter);
    l2->setAlignment(Qt::AlignCenter);
    l3->setAlignment(Qt::AlignCenter);
    l4->setAlignment(Qt::AlignCenter);
*/



}
void MainWindow::onProcessCompleted(Process p , int numCompleted)

{

    QWidget *w = this->findChild<QWidget*>(QString("process_ui_%1").arg(p.get_pid()));

    if(w) w->deleteLater();

    qDebug() << current_scheduler->getNumCompleted();

    QLabel *l = this->findChild<QLabel*>(QString("lblNumCompleted"));

    if(l)

    {

        l->setText(QString("Taches terminées : %1 / %2").arg(numCompleted).arg(numberOfProcesses));

        l->setStyleSheet("font-size : 18px");

    }

}

void MainWindow::onAlgorithmFinished() {
    Statistics current_stats = current_scheduler->getStatistics();

    if(is_auto_mode)
    {
        allStats.append(current_stats);
        QTimer::singleShot(2000 , this , [this]()
        {
            current_algo_index++;
            startNextAlgorithm();
        });
    }
    else
    {
        ui->stackedWidget->setCurrentIndex(1);
    }
    qDebug() << "FCFS terminé !";
    Statistics stats = current_scheduler->getStatistics();
    qDebug() << "total turnaround:" << stats.getTotalTurnaround();
    qDebug() << "Avg turnaround:" << stats.getAvgTurnaround();
    qDebug() << "time simulation :" << current_scheduler->getclock();
}
void MainWindow::showStatistics() {
     qDebug()<< "ici";
    // Effet de fondu sur la page actuelle
    QGraphicsOpacityEffect *fadeOut = new QGraphicsOpacityEffect(this);
    ui->stackedWidget->currentWidget()->setGraphicsEffect(fadeOut);

    QPropertyAnimation *animOut = new QPropertyAnimation(fadeOut, "opacity");
    animOut->setDuration(300);  // 300ms
    animOut->setStartValue(1.0);  // Opaque
    animOut->setEndValue(0.0);    // Transparent

    // Quand l'animation est terminée, change de page et fade in
    connect(animOut, &QPropertyAnimation::finished, this, [this]() {
        ui->stackedWidget->setCurrentIndex(1);  // Change de page

        // Fade in de la nouvelle page
        QGraphicsOpacityEffect *fadeIn = new QGraphicsOpacityEffect(this);
        ui->stackedWidget->currentWidget()->setGraphicsEffect(fadeIn);

        QPropertyAnimation *animIn = new QPropertyAnimation(fadeIn, "opacity");
        animIn->setDuration(700);
        animIn->setStartValue(0.0);  // Transparent
        animIn->setEndValue(1.0);    // Opaque
        animIn->start(QAbstractAnimation::DeleteWhenStopped);
    });

    animOut->start(QAbstractAnimation::DeleteWhenStopped);
}
QVector<Process> MainWindow::generateProcesses(int numProcesses)
{
    QVector<Process> processes;
    QRandomGenerator *rng = QRandomGenerator::global();

    int numZero = numProcesses * 0.2;
    int numRand = numProcesses - numZero;

    // Processus avec arrival = 0
    for (int i = 0; i < numZero; i++) {
        int burst = rng->bounded(30, 561);
        int priority = rng->bounded(0, 5);
        // ✅ Génère une deadline
              // Deadline = burst + marge (50% à 150% du burst)
              int margin = burst * (rng->bounded(50, 151) / 100.0);
              int deadline = 0 + burst + margin;
        processes.append(Process(burst, 0, i + 1, priority , deadline));
    }

    // Processus avec arrival aléatoire
    for (int i = 0; i < numRand; i++) {
        int burst = rng->bounded(30, 561);
        int priority = rng->bounded(0, 5);
        double r = rng->generateDouble();
        int arrival = -(qLn(r) / 0.001);
        if (arrival > 3000) { i--; continue; }
        // ✅ Génère une deadline
          // Deadline = burst + marge (50% à 150% du burst)
          int margin = burst * (rng->bounded(50, 151) / 100.0);
          int deadline = 0 + burst + margin;
        Process p = Process(burst , arrival , numZero +i +1 , priority , deadline);

        processes.append(p);
    }

    // Trier par arrival_time
    std::sort(processes.begin(), processes.end(),
              [](const Process &a, const Process &b) {
                  return a.get_arrival() < b.get_arrival();
              });

    // Réassigner les PIDs
    for (int i = 0; i < processes.size(); i++) {
        processes[i].set_pid(i + 1);
    }
    int sum  =0 ;
    for(int i = 0 ; i< processes.size()  ; i++)
    {
        sum += processes[i].get_burst();
    }
    qDebug() << "supposed simulation time " << sum << "ms";
    return processes;
}



void MainWindow::on_label_linkActivated(const QString &link)
{

}


void MainWindow::on_btnPause_clicked()
{
    current_scheduler->pause();
}


void MainWindow::on_pushButton_2_clicked()
{

    current_scheduler->reset();
}


void MainWindow::on_btnStop_clicked()
{
    if (current_scheduler == nullptr) {
            qDebug() << "Aucune simulation en cours";
            return;
        }

        // 1. Arrête la simulation
        current_scheduler->pause();

        qDebug() << "Simulation arrêtée";

        // 2. Affiche un message
        emit current_scheduler->logMessage("⏹️ Simulation arrêtée par l'utilisateur");

        // 3. Optionnel : Nettoie l'affichage
        clearVisualization();

        // 4. Optionnel : Affiche les stats partielles
        Statistics stats = current_scheduler->getStatistics();
        QString msg = QString("📊 Stats partielles :\n"
                             "Processus complétés : %1 / %2\n"
                             "Avg Turnaround : %3 ms\n"
                             "Avg Wait : %4 ms")
            .arg(stats.numProcesses)
            .arg(numberOfProcesses)
            .arg(stats.getAvgTurnaround(), 0, 'f', 2)
            .arg(stats.getAvgWait(), 0, 'f', 2);

        qDebug() << msg;
}
void MainWindow::clearVisualization()
{
    // Supprime tous les widgets de la queue
    QLayoutItem *item;
    while ((item = ui->gl1->takeAt(0)) != nullptr) {
        if (item->widget()) {
            delete item->widget();
        }
        delete item;
    }

    // Supprime le widget en cours d'exécution
  /*  while ((item = executingLayout->takeAt(0)) != nullptr) {
        if (item->widget()) {
            delete item->widget();
        }
        delete item;
    }
*/
    qDebug() << "Affichage nettoyé";
}

