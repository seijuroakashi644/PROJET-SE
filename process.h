#ifndef PROCESS_H
#define PROCESS_H


class Process
{
private:
            int arrival_time;
            int burst_time;
            int burst_left;
            int initial_wait;
            int total_wait;
            int priority;
            int pid;
            bool executed;
            int premp_time;
            int deadline;
public:

    Process(int burst , int arrival , int pid , int priority, int deadline);
            void set_arrival(int time);
            void set_pid(int num);
            int get_arrival() const;
            int get_burst() const;
            int get_priority() const;
            int get_burst_left() const;
            void dec_burst(int amount);
            int get_initial_wait() const;
            int get_total_wait() const;
            void set_premp_time(int time);
            void add_wait(int time);
            bool get_executed();
            void set_executed(int time);
            int get_pid() const;
            int get_deadline() const;
            bool hasDeadline() const ;

};

#endif // PROCESS_H
