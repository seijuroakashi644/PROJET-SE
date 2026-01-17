#include "algorithmrecommender.h"
#include "fcfs.h"
#include "sjf.h"
#include "round_robin.h"
#include "round_robin_multiniveaux.h"
#include "edf.h"
#include <QEventLoop>
#include <QTimer>
#include <QDebug>

AlgorithmRecommender::AlgorithmRecommender(QObject *parent)
    : QObject(parent)
{
}

QVector<AlgorithmRecommender::Recommendation> AlgorithmRecommender::analyzeNewOrder(
    const QVector<Process>& currentQueue,
    const Process* runningProcess,
    const Process& newOrder,
    int currentClock)
{
    QVector<Recommendation> results;

    // Prépare la liste complète des processus
    QVector<Process> allProcesses;

    // Ajoute la queue (contient déjà le processus en cours en position [0])
    allProcesses.append(currentQueue);

    // ✅ IMPORTANT: Le premier processus (en cours) a peut-être déjà été partiellement exécuté
    // Son burst_left est déjà correct dans currentQueue[0]

    qDebug() << "=== Analyse avec" << allProcesses.size() << "processus (clock=" << currentClock << ") ===";
    for (const Process& p : allProcesses) {
        qDebug() << "Process" << p.get_pid()
                 << "arrival:" << p.get_arrival()
                 << "burst:" << p.get_burst()
                 << "burst_left:" << p.get_burst_left()
                 << "deadline:" << p.get_deadline();
    }

    // Simule chaque algorithme
    QStringList algoNames = {"FCFS", "SJF", "Round Robin", "RR Multi-niveaux", "EDF"};

    for (int i = 0; i < 5; i++) {
        QVector<Process> cloned = cloneProcesses(allProcesses);
        Statistics stats = simulateAlgorithm(i, cloned, currentClock);

        Recommendation rec;
        rec.algorithmName = algoNames[i];
        rec.algoIndex = i;
        rec.avgWait = stats.getAvgWait();
        rec.avgTurnaround = stats.getAvgTurnaround();
        rec.deadlinesRespected = stats.deadlinesRespected;
        rec.deadlinesMissed = stats.deadlinesMissed;
        rec.contextSwitches = stats.contextSwitches;
        rec.score = calculateScore(stats, allProcesses.size());

        results.append(rec);

        qDebug() << rec.algorithmName << "- Score:" << rec.score
                 << "Wait:" << rec.avgWait
                 << "Deadlines:" << rec.getDeadlineText();
    }

    // Trie par score décroissant
    std::sort(results.begin(), results.end(),
              [](const Recommendation &a, const Recommendation &b) {
                  return a.score > b.score;
              });

    return results;
}

AlgorithmRecommender::Recommendation AlgorithmRecommender::getBestRecommendation(
    const QVector<Recommendation>& results)
{
    if (results.isEmpty()) {
        Recommendation empty;
        empty.algorithmName = "Aucun";
        empty.score = 0;
        return empty;
    }

    return results.first();  // Déjà trié par score
}

Statistics AlgorithmRecommender::simulateAlgorithm(
    int algoIndex,
    QVector<Process> processes,
    int startClock)
{
    AlgorithmBase* algo = nullptr;

    // Crée l'algorithme
    switch(algoIndex) {
        case 0: algo = new fcfs(this); break;
        case 1: algo = new sjf(this); break;
        case 2: algo = new round_robin(this); break;
        case 3: algo = new round_robin_multiniveaux(this); break;
        case 4: algo = new EDF(this); break;
    }

    if (!algo) {
        qDebug() << "Erreur: algo nullptr";
        return Statistics();
    }

    // ✅ AJUSTE les arrival_time : tous les processus arrivent maintenant (ou sont déjà là)
    // ✅ Tous les processus arrivent immédiatement (arrival = 0)
    // Car on simule "que se passe-t-il si on traite tout maintenant"
    // ✅ Ajuste les processus pour la simulation
    for (int i = 0; i < processes.size(); i++) {
        // Tous arrivent immédiatement
        processes[i].set_arrival(0);

        // Ajuste les deadlines relativement
        if (processes[i].hasDeadline()) {
            int originalDeadline = processes[i].get_deadline();
            int adjustedDeadline = originalDeadline - startClock;
            if (adjustedDeadline < 0) adjustedDeadline = 0;
            processes[i].set_deadline(adjustedDeadline);

            qDebug() << "Process" << processes[i].get_pid()
                     << "deadline adjusted:" << originalDeadline << "->" << adjustedDeadline;
        }
    }

    // Configure la vitesse au maximum pour simulation rapide
    algo->setSpeed(5000);  // Pas trop vite pour éviter les bugs

    // Déconnecte tous les signaux pour simulation silencieuse
    algo->blockSignals(true);

    // Lance la simulation
    algo->start(processes);

    // Attend la fin (avec timeout de sécurité)
    QEventLoop loop;
    QTimer timeout;
    timeout.setSingleShot(true);
    timeout.setInterval(500);  // 10 secondes max

    connect(algo, &AlgorithmBase::algorithmFinished, &loop, &QEventLoop::quit);
    connect(&timeout, &QTimer::timeout, &loop, &QEventLoop::quit);

    timeout.start();
    loop.exec();

    // ✅ Vérifie si la simulation s'est bien terminée
    if (timeout.isActive()) {
        timeout.stop();
        qDebug() << algoIndex << "terminé normalement";
    } else {
        qDebug() << "⚠️ Timeout pour algo" << algoIndex;
    }

    // Récupère les stats
    // Récupère les stats
    Statistics stats = algo->getStatistics();

    qDebug() << "=== RÉSULTATS ALGO" << algoIndex << "===";
    qDebug() << "Processes completed:" << stats.numProcesses << "/" << processes.size();
    qDebug() << "Avg Wait:" << stats.getAvgWait();
    qDebug() << "Avg Turnaround:" << stats.getAvgTurnaround();
    qDebug() << "Deadlines respected:" << stats.deadlinesRespected;
    qDebug() << "Deadlines missed:" << stats.deadlinesMissed;
    qDebug() << "Context switches:" << stats.contextSwitches;
    qDebug() << "===========================";

    // Nettoie
    algo->deleteLater();

    return stats;
}

double AlgorithmRecommender::calculateScore(const Statistics& stats, int totalProcesses)
{
    double score = 100.0;

    // 1. Pénalité pour deadlines manquées (priorité maximale)
    int totalDeadlines = stats.deadlinesRespected + stats.deadlinesMissed;
    if (totalDeadlines > 0) {
        double missRate = (stats.deadlinesMissed * 100.0) / totalDeadlines;
        score -= (missRate * 0.5);  // -0.5 point par % de deadline manquée
    }

    // 2. Pénalité pour temps d'attente élevé
    // On considère qu'un temps d'attente > 100ms est mauvais
    if (stats.getAvgWait() > 100) {
        score -= (stats.getAvgWait() - 100) * 0.1;
    }

    // 3. Pénalité pour turnaround élevé
    if (stats.getAvgTurnaround() > 200) {
        score -= (stats.getAvgTurnaround() - 200) * 0.05;
    }

    // 4. Bonus pour peu de context switches (efficacité)
    if (stats.contextSwitches < totalProcesses) {
        score += 5;
    }

    // Limite entre 0 et 100
    if (score < 0) score = 0;
    if (score > 100) score = 100;

    return score;
}

QVector<Process> AlgorithmRecommender::cloneProcesses(const QVector<Process>& original)
{
    QVector<Process> clones;

    for (const Process &p : original) {
        Process clone(p.get_burst(), p.get_arrival(), p.get_pid(),
                     p.get_priority(), p.get_deadline());
        clones.append(clone);
    }

    return clones;
}
