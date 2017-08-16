// Copyright Vladimir Prus 2002-2004.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt
// or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <boost/program_options.hpp>

using namespace boost;
namespace po = boost::program_options;

#include <iostream>
#include <algorithm>
#include <iterator>
#include <string>
#include <iomanip>

#include "asteroid_config.hpp"
#include "climate_config.hpp"
#include "cosmology_config.hpp"
#include "system_config.hpp"

#include "input_variables.hpp"

using namespace std;

po::options_description desc("Allowed options");
po::positional_options_description p;
po::variables_map vm;

// A helper function to simplify the main part.
template<class T>
ostream& operator<<(ostream& os, const vector<T>& v)
{
    copy(v.begin(), v.end(), ostream_iterator<T>(os, " "));
    return os;
}

void setupCommandLineOptions(int ac, char* av[])
{

        desc.add_options()
            ("help", "help message")
            ("application", po::value<std::string>(),
                  "Valid options: asteroid, climate, cosmology")
            ("algorithm", po::value<std::string>(),
                  "Valid options: geometry, raycasting")     
            ("pipeline", po::value<std::string>(),
                  "Valid options: insitu, posthoc")      
            ("num-cameras", po::value<int>(&numCamera),
                  "Number of camera positions")
            ("time-steps", po::value<int>(&numTimeSteps),
                  "Total number of time steps")
            ("temporal-ratio", po::value<float>(&temporalRatio),
                  "Fration of time steps rendered (0-1]")
            ("spatial-ratio", po::value<float>(&spatialRatio),
                  "Fration of data points visualized (0-1]")
            ("work-dist", po::value<std::string>(),
                  "Valid options: space-shared, time-shared")
            ("verbose", po::value<int>(&verboseLevel),
                  "Set verbosity level. 0=csv 1=detailed 2=debug")
           // ("input-file", po::value< vector<string> >(), "input file")
        ;


       // p.add("input-file", -1);


        po::store(po::command_line_parser(ac, av).
                  options(desc).positional(p).run(), vm);
        po::notify(vm);

}


int main(int ac, char* av[])
{

  try {

        setupCommandLineOptions(ac, av);

        // Print Help Message
        if (vm.count("help")) {
            cout << "Usage: ecx-estimator [options]\n";
            cout << desc;
            return 0;
        }

        // FACTS

        // 1a. Time taken to simulate a timestep differs for each application
        // 1b. Simulation power likely changes, but we have reference measurement for only one application. So we will use that here.

        // 2. Data size (for both raw data and image) differs for each application
        // 2a. This has no impact on power (unless data is too small ... but we will not be using small data sets here)
        // 2b. Significant impact on rendering time

        if (vm.count("application"))
        {

            application = vm["application"].as< string >();

            // Constants across all applications
            avgSimulationPower = SIMULATION_POWER;
            avgRenderingPower = RENDERING_POWER;
            idlePower = IDLE_POWER;

            // Values that change: raw data size, image data size, rendering time, simulation time
            if(application.compare(asteroid) == 0)
            {
                rawDataSize = ASTEROID_IMAGE_SIZE;
                imageSize = ASTEROID_RAW_DATA_SIZE;

                avgSimulationTimePerStep = ASTEROID_AVG_SIMULATION_TIME;
                avgRenderingTimeGeom = ASTEROID_AVG_RENDERING_TIME_GEOM;
                avgRenderingTimeRayCast = ASTEROID_AVG_RENDERING_TIME_RAYCAST;

            }

            else if(application.compare(climate) == 0)
            {

                rawDataSize = CLIMATE_IMAGE_SIZE;
                imageSize = CLIMATE_RAW_DATA_SIZE;

                avgSimulationTimePerStep = CLIMATE_AVG_SIMULATION_TIME;
                avgRenderingTimeGeom = CLIMATE_AVG_RENDERING_TIME_GEOM;
                avgRenderingTimeRayCast = CLIMATE_AVG_RENDERING_TIME_RAYCAST;

            }

            else if(application.compare(cosmology) == 0)
            {
                rawDataSize = COSMOLOGY_IMAGE_SIZE;
                imageSize = COSMOLOGY_RAW_DATA_SIZE;

                avgSimulationTimePerStep = COSMOLOGY_AVG_SIMULATION_TIME;
                avgRenderingTimeGeom = COSMOLOGY_AVG_RENDERING_TIME_GEOM;
                avgRenderingTimeRayCast = COSMOLOGY_AVG_RENDERING_TIME_RAYCAST;

            }
            else
            {
              cout << "Invalid application" << endl;
              exit(-1);
            }

            
        }

        if (vm.count("algorithm"))
        {
          algorithm = vm["algorithm"].as< string >();

          if(algorithm.compare(geometry) == 0)
          {
            avgRenderingTime = avgRenderingTimeGeom;
            avgRenderingTime = (0.14275 + 0.83945*spatialRatio) * avgRenderingTime; //Based on the energy/time curves presented in Anne's Tech Report
          }
          else if(algorithm.compare(raycasting) == 0)
          {
            avgRenderingTime = avgRenderingTimeRayCast;   
            // Performance improvement for Raycasting isn't as pronounced    
            avgRenderingTime = (0.85 + 0.15*spatialRatio) * avgRenderingTime; // Based on Fig 13 in the LDAV paper   
          }
          else
          {
            cout << "Invalid algorithm" << endl;
            exit(-1);
          }
          
        }

        if(spatialRatio < 0.25)
        {
          avgRenderingPower = avgRenderingPower - 7.5; // For very low problem sizes, the power decreases. See Fig 8b in LDAV 2017
        }

        if (vm.count("pipeline"))
        {
          pipeline = vm["pipeline"].as< string >();

          if(pipeline.compare(insitu) == 0)
          {
            writeSize = imageSize * numCamera * numTimeSteps * temporalRatio * spatialRatio;
          }
          else if(pipeline.compare(posthoc) == 0)
          {
            writeSize = (imageSize * numCamera + rawDataSize) * numTimeSteps * temporalRatio * spatialRatio;
          }
          else
          {
            cout << "Invalid pipeline" << endl;
            exit(-1);
          }
        }

        simulationTime = avgSimulationTimePerStep * numTimeSteps;
        renderingTime = avgRenderingTime * numCamera * numTimeSteps * temporalRatio;
        writeTime = writeSize/systemBandwidth;

       if (vm.count("work-dist"))
        {
          workDistribution = vm["work-dist"].as< string >();
          if(workDistribution.compare(timeshared) == 0)
          {
            totalTime = simulationTime + renderingTime + writeTime;
            totalEnergy = simulationTime * avgSimulationPower + renderingTime * avgRenderingPower + writeTime * idlePower;
          }
          else if(workDistribution.compare(spaceshared) == 0)
          {
            simulationTime = simulationTime * 2;
            renderingTime = renderingTime; //Based on Figure 15 in our LDAV paper

            if(simulationTime > renderingTime)
            {
              totalTime = simulationTime + writeTime;
              totalEnergy = simulationTime * avgSimulationPower + renderingTime * avgRenderingPower + (simulationTime - renderingTime) * idlePower/2 + writeTime * idlePower;
            }
            else
            {
              totalTime = renderingTime + writeTime;
              totalEnergy = simulationTime * avgSimulationPower + renderingTime * avgRenderingPower + (renderingTime - simulationTime) * idlePower/2 + writeTime * idlePower;
            }
          }
          else
          {
            cout << "Invalid work distribution scheme" << endl;
            exit(-1);
          }
        }

        totalTime = totalTime;
        avgPower = totalEnergy/totalTime;
        storage = writeSize;

      cout << fixed;
      cout.precision(2);

      if(verboseLevel == 1 || verboseLevel == 2)
      {
        cout << endl;
        cout << "************** SETTINGS ***********" <<endl;
        cout << "Application: " << application << endl;
        cout << "Rendering Technique: " << algorithm << endl;
        cout << "Pipeline: " << pipeline << endl;
        cout << "Simulation Timesteps: " << numTimeSteps << endl;
        cout << "Fraction of Timesteps Visualized: " << temporalRatio << endl;
        cout << "Number of cameras: " << numCamera << endl;
        cout << "Spatial sampling ratio: "<<spatialRatio <<endl;
        cout << "Job layout/mapping: " << workDistribution << endl;
        cout << "*********************************" << endl; 
        cout << endl;
        cout << "************** OUTPUT ***********" <<endl;
        cout << "Execution Time: " << totalTime/3600 << " hours" <<endl;
        cout << "Avg. Power: " << avgPower << " kW" << endl;
        cout << "Energy Consumed: " << totalEnergy/1000/3.6 << " kWh" << endl;
        cout << "Storage Requirement: " << storage/1024 << " GB" << endl;
        cout << "*********************************" << endl;
      }
      if(verboseLevel == 0)
      {
        cout << application << ",";
        cout << algorithm << ",";
        cout << pipeline << ",";
        cout << numTimeSteps << ",";
        cout << temporalRatio << ",";
        cout << numCamera << ",";
        cout << spatialRatio << ",";
        cout << workDistribution << ",";
        cout << totalTime/3600 << ",";
        cout << avgPower << ",";
        cout << totalEnergy/1000/3.6 << ",";
        cout << storage/1024 << endl;
      }


    }
    catch(std::exception& e)
    {
        cout << e.what() << endl;
        return 1;
    }
    return 0;
}