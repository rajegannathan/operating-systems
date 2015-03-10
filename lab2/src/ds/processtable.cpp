#include "processtable.h" 
#include <iostream>
#include <iomanip>
#include <stdio.h>

using namespace std;

ProcessTable& ProcessTable::getInstance(){
  static ProcessTable instance;
  return instance;
}

ProcessTable::ProcessTable(){ 
  this->table = new map<int, Process*>();
  lastFinishingTime = 0;
  cpuUtil = 0;
  ioUtil = 0;
  averageTurnAround = 0;
  averageWaitTime = 0;
  totalThroughput = 0;
}

void ProcessTable::push(Process* newProcess){
  table->insert(pair<int, Process*>(newProcess->getPID(), newProcess));
}

Process* ProcessTable::getProcess(int pid){
  return table->at(pid);
}

void ProcessTable::printPerProocessSummary(){
  int sumTurnAroundTime = 0, sumWaitTime = 0;
  for(map<int, Process*>::iterator ci = table->begin(); ci != table->end(); ci++){
    Process* p = ci->second;
    cout << setfill('0') << setw(4) << ci->first << ": ";
    cout << setfill(' ') << setw(4) << p->getArrivalTime();
    cout << " " << setfill(' ') << setw(4) << p->getTotalCPUTime();
    cout << " " << setfill(' ') << setw(4) << p->getCpuBurst();
    cout << " " << setfill(' ') << setw(4) << p->getIOBurst();
    cout << " " << p->getStaticPriority();
    cout << " | ";
    cout << setfill(' ') << setw(5) << p->getLastTransitionTime() << " ";
    cout << setfill(' ') << setw(5) << p->getTurnaroundTime() << " ";
    cout << setfill(' ') << setw(5) << p->getIOTime() << " ";
    cout << setfill(' ') << setw(5) << p->getCpuWaitingTime();
    cout << endl;

    if(p->getLastTransitionTime() > lastFinishingTime){
      lastFinishingTime = p->getLastTransitionTime();
    }
    cpuUtil += p->getTotalCPUTime();
    ioUtil += p->getIOTime();
    sumTurnAroundTime += p->getTurnaroundTime();
    sumWaitTime += p->getCpuWaitingTime();
  }
  averageTurnAround = sumTurnAroundTime/table->size();
  averageWaitTime = sumWaitTime/table->size();
  totalThroughput = (double)table->size()/(double)lastFinishingTime/100;
}

void ProcessTable::printSummary(){
  printf("SUM: %d %.2lf %.2lf %.2lf %.2lf %.3lf\n",
        lastFinishingTime,
        ((double)cpuUtil/lastFinishingTime)*100,
        ((double)ioUtil/lastFinishingTime)*100,
        averageTurnAround,
        averageWaitTime,
        totalThroughput);
}
