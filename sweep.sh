#!/bin/bash

for applications in asteroid climate cosmology
do
	for algorithms in raycasting geometry
	do
		for pipelines in insitu posthoc
		do
			for numcameras in 1 2 4 8 16 32
			do
				for timesteps in 1000 5000
				do
					for temporal in 0.2 0.4 0.6 0.8 1.0
					do
						for spatial in 0.125 0.25 0.5 1.0
						do
							for workdist in time-shared space-shared
							do
								./ecx-estimator.exe --application $applications --algorithm $algorithms --pipeline $pipelines --num-cameras $numcameras --time-steps $timesteps --temporal-ratio $temporal --spatial-ratio $spatial --work-dist $workdist --verbose=0
							done
						done
					done
				done
			done
		done
	done
done