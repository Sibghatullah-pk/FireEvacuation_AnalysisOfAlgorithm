#pragma once
#include "Grid.h"
#include "Person.h"
#include "Fire.h"
#include <vector>
#include <string>

struct SimulationConfig
{
    int width = 50;
    int height = 25;
    int peopleCount = 8;
    int fireCount = 2;
    double obstacleDensity = 0.12;
    int fireSpreadChance = 60;
    bool saveOutput = true;
};

class SimulationManager
{
private:
    SimulationConfig config;
    Grid grid;
    std::vector<Person> people;
    Fire fire;
    int stepCount;
    bool autoMode;
    int rescued;
    int burned;
    int simulationSpeed; // milliseconds per step
    bool showStatistics;
    bool showHeatMap;

    std::string buildRunSummary() const;
    std::string buildGridSnapshot() const;
    std::string buildStatusBreakdown() const;
    std::string buildTimestamp() const;
    void exportRunArtifacts() const;

public:
    SimulationManager(const SimulationConfig &simulationConfig);

    void initializeSimulation();
    void step();
    void updateStats();
    void displayStats();
    void displayDetailedStats();
    void displayLegend();
    void displayHeatMap() const;
    bool isSimulationComplete();
    void run(bool initialAutoMode = true, int speed = 800);
    void displayAlgorithmStats();
    void handleSpeedChange(char key, int &speed);
};