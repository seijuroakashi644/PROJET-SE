#include "process.h"
#include <QDebug>
Process::Process()
{
    this->pid = 0;
    this->arrival_time = 0;
    this->burst_time = 0;
    this->burst_left = 0;
    this->priority = 0;
    this->initial_wait = 0;
    this->total_wait = 0;
    this->executed = false;
    this->deadline = -1;
}
Process::Process(int burst , int arrival , int pid , int priority , int deadline)
{
    this->pid = pid ;
    this->arrival_time = arrival;
    this->burst_time = burst;
    this->burst_left = burst;
    this->priority = priority;
    this->initial_wait = 0;
    this->total_wait = 0;
    this->executed = false ;
    this->deadline = deadline;


}
void Process::set_pid(int num)
{
    pid = num;
}

bool Process::get_executed()
{
    return executed;
}

int Process::get_priority() const
{
    return priority;
}

int Process::get_burst() const
{
    return burst_time;
}

int Process::get_pid() const
{
    return pid;
}

void Process::set_arrival(int time)
{
    arrival_time = time;
}

int Process::get_arrival() const
{
    return arrival_time;
}

int Process::get_burst_left() const
{
    return burst_left;
}

// Décrémente le temps d'exécution restant de 1 ms
void Process::dec_burst(int amount =1)
{
    burst_left = burst_left - amount;
    if(burst_left < 0) { burst_left = 0; }
}

int Process::get_initial_wait() const
{
    return initial_wait;
}

int Process::get_total_wait() const
{
    return total_wait;
}

void Process::set_premp_time(int time)
{
    premp_time = time;
}


void Process::add_wait(int time)
{
    total_wait = total_wait + time;
}

// Marque le processus comme ayant accédé au CPU pour la première fois
void Process::set_executed(int time)
{
    executed = true;
    initial_wait = time;
    total_wait = time;
}
void Process::set_deadline(int deadline)
{
    this->deadline = deadline;
}
int Process::get_deadline() const { return deadline; }
 bool Process::hasDeadline() const { return deadline > 0; }
int Process::last_pid = 0;
