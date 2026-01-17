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
#include <QtCharts>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow),
    current_scheduler(nullptr),
    is_auto_mode(true),
    current_algo_index(0),
    circularQueue(nullptr)
{
    ui->setupUi(this);
    numberOfProcesses  = 10;
    ui->stackedWidget->setCurrentIndex(0);
    // Bouton checkable avec du CSS
    ui->mode->setCheckable(true);
    ui->mode->setStyleSheet(
        "QPushButton { background-color: gray; }"
        "QPushButton:checked { background-color: green; }"
    );
    ui->overlayFrame->setVisible(false);
    ui->btnDisplayAddOrder->setVisible(false);
    if(ui->mode->isChecked())
    {
        is_auto_mode = false ;

    }

    // ✅ AJOUTE ÇA
    is_manual_mode = false;
    manual_timer = new QTimer(this);
    manual_timer->setInterval(1000);  // 1 seconde = 1 unité de temps
    connect(manual_timer, &QTimer::timeout, this, [this]() {
        if (current_scheduler && is_manual_mode) {
            // Fait avancer l'horloge du scheduler de 1 unité
            int currentClock = current_scheduler->getclock();
            current_scheduler->setManualClock(currentClock + 1);

            // Met à jour l'affichage
            ui->lblClock->setText(QString("⏰ Temps: %1 min").arg(currentClock + 1));
        }
    });

    recommender = new AlgorithmRecommender(this);

    // Connecte le bouton mode
    connect(ui->mode, &QPushButton::toggled, this, &MainWindow::onManualModeToggled);



}
MainWindow::~MainWindow()
{
    delete ui;
}
void MainWindow::on_btnPlay_clicked()
{

    if(is_auto_mode)
    {
        current_algo_index = 0;
        allStats.clear();
        processes = generateProcesses(numberOfProcesses);
        startNextAlgorithm();
    }
    else {

    }

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
qDebug() <<  "dans runA";
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
    connect(current_scheduler , &AlgorithmBase::clockUpdated ,
            this , &MainWindow::onClockUpdated);


    connect(ui->mode, &QPushButton::toggled, this, [this](bool checked) {
        if(checked) {
            ui->mode->setText("ON");
            ui->btnDisplayAddOrder->setVisible(true);
        } else {
            ui->mode->setText("OFF");
            ui->btnDisplayAddOrder->setVisible(false);
            // Ton action quand désactivé
        }



    });
}
void MainWindow::onClockUpdated(int clock)
{
    QString tmp = QString::number(clock);
    ui->lblClock->setText(QString("Horloge : %1 min").arg(clock));
}
void MainWindow::onProcessArrived(Process p) {

    int nb_col = 4;
    int row , col = 0;
    int i = p.get_pid() -1;
    // QString style = "border : 1px solid rgb(235,235,235)";
    QLabel * pid = new QLabel(QString("Commande N°: %1").arg(p.get_pid()));
    QLabel * burst_left_ui = new QLabel(QString("Temps restant : %1 min").arg(p.get_burst_left()));
    QLabel * priority_ui = new QLabel(QString("Priorité : %1 ").arg(p.get_priority()));
    QLabel * deadline_ui = new QLabel(QString("Echéance : %1 min").arg(p.get_deadline()));
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
    l->setText(QString("Commande N° : %1").arg(p.get_pid()));

    QLabel *l2 = this->findChild<QLabel*>(QString("rp_priority"));
    l2->setText(QString("Priorité : %1").arg(p.get_priority()));

    QLabel *l3 = this->findChild<QLabel*>(QString("rp_burst_left"));
    l3->setText(QString("Temps restant : %1 min").arg(p.get_burst_left()));

    QLabel *l4 = this->findChild<QLabel*>(QString("rp_waiting_time"));
    // ✅ CORRECTION : Calcule le wait time réel
    int currentClock = current_scheduler->getclock();
    int actualWaitTime = (currentClock - p.get_arrival()) - (p.get_burst() - p.get_burst_left());
    l4->setText(QString("Temps attendu : %1 min").arg(actualWaitTime));

    l->setStyleSheet(style);
    l2->setStyleSheet(style);
    l3->setStyleSheet(style);
    l4->setStyleSheet(style);
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


        allStats.append(current_stats);
        QTimer::singleShot(2000 , this , [this]()
        {
            if(is_auto_mode)
            {
                current_algo_index++;
                startNextAlgorithm();
            }

        });


    Statistics stats = current_scheduler->getStatistics();
    qDebug() << " " << stats.algorithmName << "terminé ";

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
        ui->stackedWidget->setCurrentIndex(1);
        ui->lblStatsTitle->setText("🏆 Comparaison des Algorithmes d'Ordonnancement");

            // Affiche les 3 éléments
            displayStatsTable();
            displayChartWait();
            displayChartDeadlines();
            displayRecommendations();// Change de page

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
QVector<Process> MainWindow::generateProcesses(int numProcesses) {
    QVector<Process> processes;
    QRandomGenerator *rng = QRandomGenerator::global();

    int numZero = numProcesses * 0.2;
    int numRand = numProcesses - numZero;

    int totalBurst = 0;

    // Génère les processus
    for (int i = 0; i < numZero; i++) {
        int burst = rng->bounded(30, 561);
        int priority = rng->bounded(0, 5);
        totalBurst += burst;
        processes.append(Process(burst, 0, i + 1, priority, -1));
    }

    for (int i = 0; i < numRand; i++) {
        int burst = rng->bounded(30, 561);
        int priority = rng->bounded(0, 5);
        double r = rng->generateDouble();
        int arrival = -(std::log(r) / 0.001);
        if (arrival > 8000) { i--; continue; }

        totalBurst += burst;
        processes.append(Process(burst, arrival, numZero + i + 1, priority, -1));
    }

    // Tri
    std::sort(processes.begin(), processes.end(),
              [](const Process &a, const Process &b) {
                  return a.get_arrival() < b.get_arrival();
              });

    // ✅ Calcul des deadlines réalistes
    int avgBurst = totalBurst / numProcesses;
    int maxArrival = processes.isEmpty() ? 0 : processes.last().get_arrival();

    for (int i = 0; i < processes.size(); i++) {
        processes[i].set_pid(i + 1);

        // Deadline = arrival + burst + marge (2x à 5x le burst moyen)
        int margin = avgBurst * rng->bounded(3, 7);
        int deadline = processes[i].get_arrival() + processes[i].get_burst() + margin;
        qDebug() << "deadline :  " << deadline ;

        // S'assure que la deadline n'est pas trop lointaine
        int maxDeadline = maxArrival + totalBurst;
        if (deadline > maxDeadline) {
            deadline = maxDeadline;
        }

        processes[i].set_deadline(deadline);
    }

    // Debug
    qDebug() << "=== Processus générés ===";
    qDebug() << "Total burst:" << totalBurst << "ms";
    qDebug() << "Avg burst:" << avgBurst << "ms";
    for (const Process &p : processes) {
        int minNeeded = p.get_arrival() + p.get_burst();
        qDebug() << QString("P%1: arrival=%2, burst=%3, deadline=%4 (%5)")
            .arg(p.get_pid())
            .arg(p.get_arrival())
            .arg(p.get_burst())
            .arg(p.get_deadline())
            .arg(p.get_deadline() >= minNeeded ? "OK" : "IMPOSSIBLE");
    }

    return processes;
}

void MainWindow::showFinalComparison()
{
    qDebug() << "=== Affichage des statistiques finales ===";

    // Change vers la page des stats
    ui->stackedWidget->setCurrentIndex(1);

    // Affiche le titre
    ui->lblStatsTitle->setText("🏆 Comparaison des Algorithmes d'Ordonnancement");

    // Affiche les 3 éléments
    displayStatsTable();
    displayChartWait();
    displayChartDeadlines();
    displayRecommendations();
}

// ═══════════════════════════════════
// 1️⃣ TABLEAU COMPARATIF
// ═══════════════════════════════════

void MainWindow::displayStatsTable()
{
    QString table = "<h3>📊 Tableau Comparatif</h3>";
    qDebug() << "dans display table";
    // En-tête du tableau HTML
    table += "<table border='1' cellpadding='8' cellspacing='0' style='border-collapse: collapse; width: 100%;'>";
    table += "<tr style='background-color: #34495e; color: white;'>";
    table += "<th>Algorithme</th>";
    table += "<th>Avg Wait (min)</th>";
    table += "<th>Avg Turnaround (min)</th>";
    table += "<th>Context Switches</th>";
    table += "<th>Deadlines</th>";
    table += "</tr>";

    // Lignes de données
    for (const Statistics &s : allStats) {
        qDebug() << s.algorithmName;
        table += "<tr>";
        table += QString("<td><b>%1</b></td>").arg(s.algorithmName);
        table += QString("<td>%1</td>").arg(s.getAvgWait(), 0, 'f', 2);
        table += QString("<td>%1</td>").arg(s.getAvgTurnaround(), 0, 'f', 2);
        table += QString("<td>%1</td>").arg(s.contextSwitches);

        // Deadlines (si applicable)
        int total = s.deadlinesRespected + s.deadlinesMissed;
        if (total > 0) {
            QString deadlineText = QString("%1/%2 (%3%)")
                .arg(s.deadlinesRespected)
                .arg(total)
                .arg(s.getDeadlineRespectRate(), 0, 'f', 0);
            table += QString("<td>%1</td>").arg(deadlineText);
        } else {
            table += "<td>N/A</td>";
        }

        table += "</tr>";
    }

    table += "</table>";

    ui->txtStatsTable->setHtml(table);
}


// ═══════════════════════════════════
// 2️⃣ GRAPHIQUE - TEMPS D'ATTENTE
// ═══════════════════════════════════

void MainWindow::displayChartWait()
{
    using namespace QtCharts;

    // Crée le bar set
    QBarSet *set = new QBarSet("Temps d'attente moyen");

    QStringList categories;

    // Ajoute les données
    for (const Statistics &s : allStats) {
        *set << s.getAvgWait();

        // Nom court pour l'axe X
        QString shortName = s.algorithmName;
        if (shortName.contains("(")) {
            shortName = shortName.left(shortName.indexOf("(")).trimmed();
        }
        categories << shortName;
    }

    // Couleur des barres
    set->setColor(QColor("#3498db"));

    // Crée la série
    QBarSeries *series = new QBarSeries();
    series->append(set);

    // Crée le graphique
    QChart *chart = new QChart();
    chart->addSeries(series);
    chart->setTitle("Temps d'attente moyen (min)");
    chart->setAnimationOptions(QChart::SeriesAnimations);

    // Axe X (noms des algos)
    QBarCategoryAxis *axisX = new QBarCategoryAxis();
    axisX->append(categories);
    chart->addAxis(axisX, Qt::AlignBottom);
    series->attachAxis(axisX);

    // Axe Y (temps)
    QValueAxis *axisY = new QValueAxis();
    axisY->setTitleText("Temps (min)");
    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisY);

    // Légende
    chart->legend()->setVisible(false);

    // Affiche dans le widget
    QChartView *chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);

    // Nettoie l'ancien contenu
    QLayout *oldLayout = ui->widgetChartWait->layout();
    if (oldLayout) {
        QLayoutItem *item;
        while ((item = oldLayout->takeAt(0)) != nullptr) {
            delete item->widget();
            delete item;
        }
        delete oldLayout;
    }

    // Ajoute le nouveau graphique
    QVBoxLayout *layout = new QVBoxLayout(ui->widgetChartWait);
    layout->addWidget(chartView);
    ui->widgetChartWait->setLayout(layout);
}

void MainWindow::displayChartDeadlines()
{
    using namespace QtCharts;

    // Crée le bar set
    QBarSet *set = new QBarSet("Taux de respect (%)");

    QStringList categories;
    bool hasDeadlines = false;

    // Ajoute les données
    for (const Statistics &s : allStats) {
        int total = s.deadlinesRespected + s.deadlinesMissed;
        if (total > 0) {
            *set << s.getDeadlineRespectRate();
            hasDeadlines = true;
        } else {
            *set << 0;
        }

        QString shortName = s.algorithmName;
        if (shortName.contains("(")) {
            shortName = shortName.left(shortName.indexOf("(")).trimmed();
        }
        categories << shortName;
    }

    // Si aucun algo n'a de deadlines, affiche un message
    if (!hasDeadlines) {
        QLabel *label = new QLabel("Aucune deadline définie", ui->widgetChartDeadlines);
        label->setAlignment(Qt::AlignCenter);

        QLayout *oldLayout = ui->widgetChartDeadlines->layout();
        if (oldLayout) delete oldLayout;

        QVBoxLayout *layout = new QVBoxLayout(ui->widgetChartDeadlines);
        layout->addWidget(label);
        return;
    }

    // Couleur des barres
    set->setColor(QColor("#2ecc71"));

    // Crée la série
    QBarSeries *series = new QBarSeries();
    series->append(set);

    // Crée le graphique
    QChart *chart = new QChart();
    chart->addSeries(series);
    chart->setTitle("Taux de respect des deadlines (%)");
    chart->setAnimationOptions(QChart::SeriesAnimations);

    // Axe X
    QBarCategoryAxis *axisX = new QBarCategoryAxis();
    axisX->append(categories);
    chart->addAxis(axisX, Qt::AlignBottom);
    series->attachAxis(axisX);

    // Axe Y (0 à 100%)
    QValueAxis *axisY = new QValueAxis();
    axisY->setTitleText("Taux (%)");
    axisY->setRange(0, 100);
    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisY);

    // Légende
    chart->legend()->setVisible(false);

    // Affiche
    QChartView *chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);

    QLayout *oldLayout = ui->widgetChartDeadlines->layout();
    if (oldLayout) {
        QLayoutItem *item;
        while ((item = oldLayout->takeAt(0)) != nullptr) {
            delete item->widget();
            delete item;
        }
        delete oldLayout;
    }

    QVBoxLayout *layout = new QVBoxLayout(ui->widgetChartDeadlines);
    layout->addWidget(chartView);
    ui->widgetChartDeadlines->setLayout(layout);
}
void MainWindow::displayRecommendations()
{
    // Trouve les meilleurs pour chaque critère
    Statistics bestWait = allStats[0];
    Statistics bestTurnaround = allStats[0];
    Statistics bestSwitches = allStats[0];
    Statistics bestDeadlines = allStats[0];

    for (const Statistics &s : allStats) {
        if (s.getAvgWait() < bestWait.getAvgWait())
            bestWait = s;

        if (s.getAvgTurnaround() < bestTurnaround.getAvgTurnaround())
            bestTurnaround = s;

        if (s.contextSwitches < bestSwitches.contextSwitches)
            bestSwitches = s;

        if (s.getDeadlineRespectRate() > bestDeadlines.getDeadlineRespectRate())
            bestDeadlines = s;
    }

    // Crée le texte de recommandation
    QString reco = "<h3>🎯 Recommandations</h3>";
    reco += "<p style='line-height: 1.8;'>";

    reco += QString("✅ <b>Meilleur pour temps d'attente</b> : %1 (<b>%2 min</b>)<br>")
        .arg(bestWait.algorithmName)
        .arg(bestWait.getAvgWait(), 0, 'f', 2);

    reco += QString("✅ <b>Meilleur pour temps de rotation</b> : %1 (<b>%2 min</b>)<br>")
        .arg(bestTurnaround.algorithmName)
        .arg(bestTurnaround.getAvgTurnaround(), 0, 'f', 2);

    reco += QString("✅ <b>Meilleur pour efficacité</b> (moins de switches) : %1 (<b>%2 switches</b>)<br>")
        .arg(bestSwitches.algorithmName)
        .arg(bestSwitches.contextSwitches);

    int totalDeadlines = bestDeadlines.deadlinesRespected + bestDeadlines.deadlinesMissed;
    if (totalDeadlines > 0) {
        reco += QString("✅ <b>Meilleur pour deadlines</b> : %1 (<b>%2%</b>)<br>")
            .arg(bestDeadlines.algorithmName)
            .arg(bestDeadlines.getDeadlineRespectRate(), 0, 'f', 0);
    }

    reco += "</p>";

    // Conseil personnalisé
   /* reco += "<hr>";
    reco += "<h4>💡 Conseil pour votre atelier :</h4>";
    reco += "<ul>";
    reco += QString("<li>Si vous avez des <b>commandes urgentes</b> → %1</li>").arg(bestDeadlines.algorithmName);
    reco += QString("<li>Si vous voulez <b>minimiser l'attente</b> → %1</li>").arg(bestWait.algorithmName);
    reco += QString("<li>Si vous voulez un système <b>simple et équitable</b> → FCFS</li>");
    reco += "</ul>";*/

    ui->lblRecommandations->setText(reco);
}

void MainWindow::on_label_linkActivated(const QString &link)
{

}


void MainWindow::on_btnPause_clicked()
{
    if(current_scheduler->get_isrunning())
    {current_scheduler->pause();}
    else{
        current_scheduler->play();
    }



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


void MainWindow::on_BtnAuto_clicked()
{
    ui->stackedWidget->setCurrentIndex(1);
}


void MainWindow::on_BtnManual_clicked()
{

}





void MainWindow::on_btnDisplayAddOrder_clicked()
{
    QGraphicsDropShadowEffect* shadow = new QGraphicsDropShadowEffect();
    shadow->setBlurRadius(20);        // Flou de l'ombre
    shadow->setXOffset(0);            // Décalage horizontal
    shadow->setYOffset(5);            // Décalage vertical
    shadow->setColor(QColor(0, 0, 0, 160)); // Couleur et transparence

    ui->overlayFrame->setGraphicsEffect(shadow);
    ui->overlayFrame->raise();
    ui->overlayFrame->show();

}


/*void MainWindow::on_bntAddOrder_clicked()
{

    QString tmp = ui->newOrderBurst->text();
    int np_burst = tmp.toInt();
    tmp = ui->newOrderPriority->text();
    int np_priority = tmp.toInt();
    tmp = ui->newOrderDeadline->text();
    int np_deadline = tmp.toInt();
    int last_pid = Process::last_pid;
    processes.append(Process(np_burst, 5 , last_pid + 1 , np_priority, np_deadline));

}

*/

// nouvelles méthodes
void MainWindow::onManualModeToggled(bool enabled)
{
    if (enabled) {
        is_auto_mode = false;
        ui->mode->setText("MANUEL ON");
        ui->btnDisplayAddOrder->setVisible(true);

        startManualMode();
    } else {
        is_auto_mode = true;
        ui->mode->setText("MANUEL OFF");
        ui->btnDisplayAddOrder->setVisible(false);
        stopManualMode();
    }
}

void MainWindow::startManualMode()
{
    is_manual_mode = true;
    manual_orders.clear();

    // Lance FCFS par défaut
    current_algo_index = 0;

    if (current_scheduler != nullptr) {
        disconnect(current_scheduler, nullptr, this, nullptr);
        delete current_scheduler;
    }

    current_scheduler = new fcfs(this);
    connectSignals();

    // ✅ Initialise l'horloge à 0
    QVector<Process> empty;
    current_scheduler->start(empty);
    current_scheduler->pause();  // Le scheduler ne tourne pas tout seul

    // ✅ Lance le timer manuel (1 sec = 1 unité)
    manual_timer->start();

    ui->algoName->setText("🔵 Mode Manuel - FCFS");
    ui->lblClock->setText("⏰ Temps: 0 min");

    emit current_scheduler->logMessage("⏰ Mode manuel activé - Horloge démarrée");

    qDebug() << "Mode manuel activé";
}

void MainWindow::stopManualMode()
{
    is_manual_mode = false;
    manual_timer->stop();  // ✅ Arrête le timer manuel

    if (current_scheduler) {
        current_scheduler->pause();
    }

    emit current_scheduler->logMessage("⏹️ Mode manuel désactivé");

    qDebug() << "Mode manuel désactivé";
}


void MainWindow::on_bntAddOrder_clicked()
{
    // Récupère les valeurs
    QString tmp = ui->newOrderBurst->text();
    int np_burst = tmp.toInt();

    tmp = ui->newOrderPriority->text();
    int np_priority = tmp.toInt();

    tmp = ui->newOrderDeadline->text();
    int np_deadline = tmp.toInt();

    // Validation
    if (np_burst <= 0) {
        emit current_scheduler->logMessage("❌ Erreur: Temps de travail invalide");
        return;
    }

    // ✅ L'horloge c'est celle du scheduler
    int arrival = current_scheduler->getclock();

    // La deadline est relative
    int absolute_deadline = arrival + np_deadline;

    // Crée la nouvelle commande
    int new_pid = manual_orders.size() + 1;
    Process newOrder(np_burst, arrival, new_pid, np_priority, absolute_deadline);

    manual_orders.append(newOrder);

    emit current_scheduler->logMessage(QString("📦 Nouvelle commande #%1 ajoutée à t=%2min (deadline: %3min)")
        .arg(new_pid)
        .arg(arrival)
        .arg(absolute_deadline));

    ui->overlayFrame->hide();
    ui->newOrderBurst->clear();
    ui->newOrderPriority->clear();
    ui->newOrderDeadline->clear();

    if (manual_orders.size() == 1) {
        QVector<Process> firstOrder;
        firstOrder.append(newOrder);
        current_scheduler->start(firstOrder);
        emit current_scheduler->logMessage("▶️ Première commande - Démarrage du FCFS");
    } else {
        addManualOrder();
    }
}

void MainWindow::addManualOrder()
{
    // Pause la simulation
    bool wasRunning = current_scheduler->get_isrunning();
    if (wasRunning) {
        current_scheduler->pause();
    }

    emit current_scheduler->logMessage("⏸️ Pause pour analyse...");

    // ✅ Récupère l'horloge du scheduler (pas manual_clock)
    int currentClock = current_scheduler->getclock();

    // Récupère l'état actuel
    QVector<Process> currentQueue = current_scheduler->getQueue();
    Process* runningProcess = getCurrentRunningProcess();
    Process newOrder = manual_orders.last();

    qDebug() << "=== État avant analyse ===";
    qDebug() << "Current clock:" << currentClock;
    qDebug() << "Queue size:" << currentQueue.size();
    qDebug() << "New order: PID=" << newOrder.get_pid()
             << "Arrival=" << newOrder.get_arrival()
             << "Burst=" << newOrder.get_burst()
             << "Deadline=" << newOrder.get_deadline();

    // ✅ AJOUTE LA NOUVELLE COMMANDE À LA QUEUE MAINTENANT
    currentQueue.append(newOrder);

    qDebug() << "Queue size after adding:" << currentQueue.size();

    // Lance l'analyse
    emit current_scheduler->logMessage("🧠 Analyse des algorithmes en cours...");

    QVector<AlgorithmRecommender::Recommendation> recommendations =
        recommender->analyzeNewOrder(currentQueue, runningProcess, newOrder, currentClock);

    // Affiche les recommandations
    showRecommendationsDialog(recommendations);
}

void MainWindow::showRecommendationsDialog(const QVector<AlgorithmRecommender::Recommendation>& recs)
{
    if (recs.isEmpty()) {
        qDebug() << "Aucune recommandation";
        return;
    }

    // Crée un dialog personnalisé
    QDialog *dialog = new QDialog(this);
    dialog->setWindowTitle("💡 Recommandations d'Algorithme");
    dialog->setMinimumWidth(600);

    QVBoxLayout *mainLayout = new QVBoxLayout(dialog);

    // Titre
    QLabel *title = new QLabel("<h2>🎯 Meilleur algorithme pour votre atelier</h2>");
    title->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(title);

    mainLayout->addSpacing(10);

    // Affiche les 3 meilleurs
    for (int i = 0; i < qMin(3, recs.size()); i++) {
        const auto& rec = recs[i];

        QFrame *frame = new QFrame();
        frame->setFrameShape(QFrame::StyledPanel);

        QString bgColor = (i == 0) ? "#d4edda" : "#fff3cd";  // Vert pour le meilleur, jaune pour les autres
        frame->setStyleSheet(QString("QFrame { background-color: %1; border-radius: 8px; padding: 10px; }").arg(bgColor));

        QVBoxLayout *frameLayout = new QVBoxLayout(frame);

        // Nom de l'algo + badge
        QString badge = (i == 0) ? "🏆 RECOMMANDÉ" : QString("#%1").arg(i + 1);
        QLabel *algoName = new QLabel(QString("<b>%1 - %2</b>").arg(badge).arg(rec.algorithmName));
        algoName->setStyleSheet("font-size: 16px;");
        frameLayout->addWidget(algoName);

        // Score
        QLabel *score = new QLabel(QString("Score: %1/100").arg(rec.score, 0, 'f', 1));
        score->setStyleSheet(QString("color: %1; font-weight: bold;").arg(rec.getScoreColor()));
        frameLayout->addWidget(score);

        // Métriques
        QString metrics = QString(
            "⏱️ Temps d'attente moyen: <b>%1 min</b><br>"
            "🔄 Context switches: <b>%2</b><br>"
            "📅 Deadlines: <b>%3</b>"
        ).arg(rec.avgWait, 0, 'f', 1)
         .arg(rec.contextSwitches)
         .arg(rec.getDeadlineText());

        QLabel *metricsLabel = new QLabel(metrics);
        frameLayout->addWidget(metricsLabel);

        // Bouton "Choisir cet algorithme"
        if (i == 0) {
            QPushButton *chooseBtn = new QPushButton("✅ Utiliser cet algorithme");
            chooseBtn->setStyleSheet("background-color: #28a745; color: white; padding: 8px; font-weight: bold;");
            connect(chooseBtn, &QPushButton::clicked, dialog, [this, rec, dialog]() {
                switchToAlgorithm(rec.algoIndex);
                dialog->accept();
            });
            frameLayout->addWidget(chooseBtn);
        }

        mainLayout->addWidget(frame);
    }

    // Bouton "Garder l'algorithme actuel"
    QPushButton *keepBtn = new QPushButton("Garder " + current_scheduler->getAlgorithmName());
    keepBtn->setStyleSheet("background-color: #6c757d; color: white; padding: 8px;");
    connect(keepBtn, &QPushButton::clicked, dialog, [this, dialog]() {
        emit current_scheduler->logMessage("ℹ️ Algorithme inchangé");
        current_scheduler->play();
        dialog->accept();
    });
    mainLayout->addWidget(keepBtn);

    dialog->setLayout(mainLayout);
    dialog->exec();
}

void MainWindow::switchToAlgorithm(int algoIndex)
{
    emit current_scheduler->logMessage(QString("🔄 Changement d'algorithme vers %1...")
        .arg(algoIndex == 0 ? "FCFS" :
             algoIndex == 1 ? "SJF" :
             algoIndex == 2 ? "Round Robin" :
             algoIndex == 3 ? "RR Multi-niveaux" : "EDF"));

    // Sauvegarde l'état actuel (la queue contient déjà tout)
    QVector<Process> allProcesses = current_scheduler->getQueue();

    // ✅ AJOUTE LA NOUVELLE COMMANDE
    Process newOrder = manual_orders.last();
    allProcesses.append(newOrder);

    // Change d'algorithme
    current_algo_index = algoIndex;
    runAlgorithm(algoIndex);

    // Charge l'état complet (avec la nouvelle commande)
    current_scheduler->start(allProcesses);

    emit current_scheduler->logMessage("✅ Algorithme changé, traitement repris");
}
Process* MainWindow::getCurrentRunningProcess()
{
    if (!current_scheduler) return nullptr;

    QVector<Process> queue = current_scheduler->getQueue();
    if (queue.isEmpty()) return nullptr;

    // Le processus en cours est le premier de la queue
    return new Process(queue.first());
}
