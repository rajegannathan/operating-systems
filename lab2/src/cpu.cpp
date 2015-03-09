#include "cpu.h"

using namespace std;

// Destructor
CPU::~CPU(){
  inFile->close();
  randFile->close();
  delete inFile;
  delete randFile;
}

Scheduler* CPU::getScheduler(char* schedulerSpec){
  Scheduler* scheduler;
  quantum;
  char ch = schedulerSpec[0];
  switch(ch){
    case 'F':
      scheduler = new FCFS();
      break;
    case 'L':
      scheduler = new LCFS();
      break;
    case 'S':
      scheduler = new SJF();
      break;
    case 'P':
      schedulerSpec++;
      quantum = atoi(schedulerSpec);
      scheduler = new PriorityScheduler(quantum);
      break;
    case 'R':
      schedulerSpec++;
      quantum = atoi(schedulerSpec);
      scheduler = new RoundRobin(quantum);
      break;
    default:
      return NULL;
  }
}

// Constructor 
CPU::CPU(char* inputFileName, char* randFileName, char* schedulerSpec){
  good = true;
  inFile = new ifstream(inputFileName);
  randFile = new ifstream(randFileName);

  if(!inFile->good()){
    good = false;
    error += "Invalid input file\n";
  }

  if(!randFile->good()){
    good = false;
    error += "Invalid rand file\n";
  }
  randGen = new RandomNumberGenerator(*(randFile));

  curScheduler = getScheduler(schedulerSpec);
}

void CPU::populateEventQueue(){
  int arrivalTime, totalCPU, cpuBurst, ioBurst;
  while(*(inFile) >> arrivalTime >> totalCPU >> cpuBurst >> ioBurst){
    int priority =  randGen->getStaticPriority();
    Process* newProcess = new Process(arrivalTime, totalCPU, cpuBurst, ioBurst, priority);
    Event* eve = new Event(arrivalTime, newProcess->getPID(), newProcess->getCurrentState(), READY);
    ProcessTable::getInstance().push(newProcess);
    eventQueue.push(eve);
  }
}

/*
 * All public methods below this
 */

bool CPU::isGood(){
  return good;
}

string CPU::getError(){
  return error;
}

void CPU::start(){
  populateEventQueue();


  unsigned int time = 0;

  while(!eventQueue.empty()){
    time = eventQueue.top()->getTimestamp();
    while(!eventQueue.empty() && eventQueue.top()->getTimestamp() == time){
      Event* eve = eventQueue.top();
      eventQueue.pop();

      Event::Transition eventTransition =  eve->getTransition();
      Process* p = ProcessTable::getInstance().getProcess(eve->getPID());

      cout << time << " " << p->getPID()<< " " << time - p->getLastTransitionTime() << ": ";
      cout << eve->getTransitionLogString();

      ProcessState endState;
      int cpuBurst = 0, ioBurst = 0;
      switch(eventTransition){

        case Event::T_CREATE:
          curScheduler->addProcess(p);
          break;

        case Event::T_RUN:
          endState = BLOCKED;
          cpuBurst = p->getRemainingCpuBurst();
          if(cpuBurst <= 0){
            cpuBurst = randGen->getBurstCycle(p->getCpuBurst());
            if(cpuBurst >= p->getRemainingTime()){
              cpuBurst = p->getRemainingTime();
              endState = TERMINATED;
            }
          }
          if(cpuBurst > quantum){
            endState= READY;
          }

          cout << " cb=" << cpuBurst << " rem="<<p->getRemainingTime() << " prio="<<p->getDynamicPriority();
          eventQueue.push(new Event(time+cpuBurst, p->getPID(), RUNNING, endState));
          p->reduceRemainingTime(cpuBurst);
          p->reduceDynamicPriority();
          break;

        case Event::T_BLOCK:
          p->resetDynamicPriority();
          ioBurst = randGen->getBurstCycle(p->getIOBurst());
          cout << " ib="<<ioBurst<< " rem="<<p->getRemainingTime();
          eventQueue.push(new Event(time+ioBurst, p->getPID(), BLOCKED, READY));
          break;

        case Event::T_UNBLOCK:
          curScheduler->addProcess(p);
          break;

        case Event::T_PREEMPT:
          endState = READY;
          eventQueue.push(new Event(time+cpuBurst, p->getPID(), RUNNING, endState));
          curScheduler->addProcess(p);
          break;

        case Event::T_TERMINATE:
          break;

        default:
          cout << "in default transition.  Some error "<< eventTransition << endl;
      }
      cout << endl;
      p->setLastTransitionTime(time);
      delete eve;
    }
    Process* runP = curScheduler->get_next_process();
    if(runP != NULL){
      eventQueue.push(new Event(time, runP->getPID(), READY, RUNNING));
    }
  }
}
