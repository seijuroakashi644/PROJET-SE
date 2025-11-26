#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "process.h"
#include "fcfs.h"
#include "sjf.h"
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
    void showStatistics() ;
    void showFinalComparison();  // Affiche les stats finales
    void displayStatsTable();    // Affiche le tableau
    void displayChartWait();     // Graphique temps d'attente
    void displayChartDeadlines(); // Graphique deadlines
    void displayRecommendations(); // Recommandations

    QVector<Process> cloneProcesses(const QVector<Process> &original);

    void on_btnPause_clicked();

    void on_pushButton_2_clicked();

    void on_btnStop_clicked();

private:
    Ui::MainWindow *ui;
    AlgorithmBase *current_scheduler;
    int numberOfProcesses;
    bool is_auto_mode;
    int current_algo_index;
    QVector<Statistics> allStats;
    void connectSignals();
    void runAlgorithm(int index);
    void startNextAlgorithm();
    void clearVisualization();
    QVector<Process> processes;

};
#endif // MAINWINDOW_H
