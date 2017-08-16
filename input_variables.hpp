 //Input paramters
  int numCamera = 1;
  int numTimeSteps = 1000;
  float spatialRatio = 1.0;
  float temporalRatio = 1.0;
  int verboseLevel = 0;

  std::string asteroid("asteroid");
  std::string climate("climate");
  std::string cosmology("cosmology");

  std::string geometry("geometry");
  std::string raycasting("raycasting");

  std::string insitu("insitu");
  std::string posthoc("posthoc");

  std::string timeshared("time-shared");
  std::string spaceshared("space-shared");

  std::string application;
  std::string algorithm;
  std::string pipeline;
  std::string workDistribution;

  //Output paramters
  float avgPower;
  float totalEnergy;
  float storage;
  float totalTime;

  //system parameters
  int numNodes = NUM_NODES;
  float systemBandwidth = SYSTEM_BANDWIDTH;
  float idlePower = IDLE_POWER;

  //Intermediate Parameters
  float avgSimulationTimePerStep;
  float avgRenderingTimeGeom;
  float avgRenderingTimeRayCast;
  float avgRenderingTime;

  float simulationTime;
  float renderingTime;
  float writeTime;

  float avgSimulationPower;
  float avgRenderingPowerGeom;
  float avgRenderingPowerRayCast;
  float avgRenderingPower;

  float imageSize;
  float rawDataSize;
  float writeSize;
