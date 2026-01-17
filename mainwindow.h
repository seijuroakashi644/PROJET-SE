#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "process.h"
#include "fcfs.h"
#include "sjf.h"
#include "circularqueuewidget.h"
#include "algorithmrecommender.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_btnPlay_clicked();
    void on_label_linkActivated(const QString &link);

    QVector<Process> generateProcesses(int numProcesses);
    void onProcessArrived(Process p);
    void onAlgorithmFinished();
    void onProcessCompleted(Process p , int numCompleted);
    void onProcessExecuting(Process p);
    void onClockUpdated(int clock) ;
    void showStatistics();
    void showFinalComparison();
    void displayStatsTable();
    void displayChartWait();
    void displayChartDeadlines();
    void displayRecommendations();

    QVector<Process> cloneProcesses(const QVector<Process> &original);

    void on_btnPause_clicked();
    void on_pushButton_2_clicked();
    void on_btnStop_clicked();
    void on_BtnAuto_clicked();
    void on_BtnManual_clicked();

    void on_bntAddOrder_clicked();
    void on_btnDisplayAddOrder_clicked();

    // NOUVEAUX SLOTS POUR MODE MANUEL
    void onManualModeToggled(bool enabled);
    void onManualClockTick();
    void addManualOrder();

    void showRecommendationsDialog(const QVector<AlgorithmRecommender::Recommendation>& recs);
    void switchToAlgorithm(int algoIndex);

private:
    Ui::MainWindow *ui;
    AlgorithmBase *current_scheduler;
    int numberOfProcesses;
    bool is_auto_mode;
    int current_algo_index;
    QVector<Statistics> allStats;
    QVector<Process> processes;
    CircularQueueWidget *circularQueue;

    bool is_manual_mode;
    QTimer *manual_timer;  //
    QVector<Process> manual_orders;
    AlgorithmRecommender *recommender;

    void connectSignals();
    void runAlgorithm(int index);
    void startNextAlgorithm();
    void clearVisualization();
    bool isRoundRobinActive();

    // NOUVELLES MÉTHODES
    void startManualMode();
    void stopManualMode();
    Process* getCurrentRunningProcess();
};

#endif // MAINWINDOW_H
