#include <iostream>
#include <vector>
#include <algorithm>
#include <random>
#include <iomanip>

struct Process {
    int id;
    int arrivalTime;
    int executionTime;
    int priority;
    int startTime = -1;
    int endTime = -1;
    int waitingTime = 0;
};

void generateProcesses(std::vector<Process>& processes, int count) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> arrivalDist(0, 10);
    std::uniform_int_distribution<> executionDist(1, 10);
    std::uniform_int_distribution<> priorityDist(1, 5);

    for (int i = 0; i < count; ++i) {
        processes.push_back({
            .id = i + 1,
            .arrivalTime = arrivalDist(gen),
            .executionTime = executionDist(gen),
            .priority = priorityDist(gen)
        });
    }
}

void simulateProcesses(std::vector<Process>& processes, int timeQuantum) {
    int currentTime = 0;

    std::sort(processes.begin(), processes.end(), [](const Process& a, const Process& b) {
        return a.arrivalTime < b.arrivalTime;
    });

    std::vector<Process*> readyQueue;
    int completedProcesses = 0;

    while (completedProcesses < processes.size()) {
        for (auto& process : processes) {
            if (process.arrivalTime <= currentTime && process.executionTime > 0 &&
                std::find(readyQueue.begin(), readyQueue.end(), &process) == readyQueue.end()) {
                readyQueue.push_back(&process);
            }
        }

        if (!readyQueue.empty()) {
            auto currentProcess = readyQueue.front();
            readyQueue.erase(readyQueue.begin());

            if (currentProcess->startTime == -1) {
                currentProcess->startTime = currentTime;
                currentProcess->waitingTime = currentTime - currentProcess->arrivalTime;
            }

            int timeSlice = std::min(timeQuantum, currentProcess->executionTime);
            currentProcess->executionTime -= timeSlice;
            currentTime += timeSlice;

            if (currentProcess->executionTime == 0) {
                currentProcess->endTime = currentTime;
                ++completedProcesses;
            } else {
                readyQueue.push_back(currentProcess);
            }
        } else {
            ++currentTime;
        }
    }
}

void simulateWithPriority(std::vector<Process>& processes) {
    std::sort(processes.begin(), processes.end(), [](const Process& a, const Process& b) {
        return (a.priority > b.priority) || (a.priority == b.priority && a.arrivalTime < b.arrivalTime);
    });

    int currentTime = 0;

    for (auto& process : processes) {
        if (currentTime < process.arrivalTime) {
            currentTime = process.arrivalTime;
        }
        process.startTime = currentTime;
        currentTime += process.executionTime;
        process.endTime = currentTime;
        process.waitingTime = process.startTime - process.arrivalTime;
    }
}

void printProcesses(const std::vector<Process>& processes) {
    std::cout << std::setw(5) << "ID"
              << std::setw(10) << "Arrival"
              << std::setw(10) << "Exec"
              << std::setw(10) << "Priority"
              << std::setw(10) << "Start"
              << std::setw(10) << "End"
              << std::setw(10) << "Wait" << "\n";

    for (const auto& process : processes) {
        std::cout << std::setw(5) << process.id
                  << std::setw(10) << process.arrivalTime
                  << std::setw(10) << process.executionTime
                  << std::setw(10) << process.priority
                  << std::setw(10) << process.startTime
                  << std::setw(10) << process.endTime
                  << std::setw(10) << process.waitingTime << "\n";
    }
}

int main() {
    int processCount = 5;
    int timeQuantum = 3;

    std::vector<Process> processes;
    generateProcesses(processes, processCount);

    std::cout << "Generated Processes:\n";
    printProcesses(processes);

    std::cout << "\nSimulating with Round-Robin (Quantum = " << timeQuantum << "):\n";
    simulateProcesses(processes, timeQuantum);
    printProcesses(processes);

    std::cout << "\nSimulating with Priority Scheduling:\n";
    simulateWithPriority(processes);
    printProcesses(processes);

    return 0;
}

