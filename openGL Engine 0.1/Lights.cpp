#include "Lights.h"

namespace World {
	Lights::Lights()
	{
	}


    void Lights::update(float deltaTime) {
            // Update the lerp data
            lightLerp.currentLerpTime += deltaTime * lightLerp.speedMultiplier;

            if (curvePositions.size() < 2) return;  // Ensure we have enough points

            // Check if we need to move to the next point
            if (lightLerp.currentLerpTime >= lightLerp.moveDuration) {
                lightLerp.currentLerpTime = 0.0f; // Reset lerp time
                lightLerp.currentPoint = (lightLerp.currentPoint + 1) % curvePositions.size(); // Move to next point
            }

            // Get the next point index
            int nextPoint = (lightLerp.currentPoint + 1) % curvePositions.size();

            // Calculate interpolation factor (t)
            float t = lightLerp.currentLerpTime / lightLerp.moveDuration;

            // Update the GPU position data (which will be sent to the GPU)
            gpuData.position = glm::lerp(curvePositions[lightLerp.currentPoint],
                curvePositions[nextPoint], t);
        
	}

};
