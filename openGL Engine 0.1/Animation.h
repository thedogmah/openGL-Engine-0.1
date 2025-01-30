#pragma once
#include <unordered_map>
#include <vector>
#include <string>
#include <assimp/scene.h>  // For aiVector3D, aiQuaternion, aiMatrix4x4, etc.
#include "glm/glm.hpp"
#include "globals.h"

struct PositionKey {

	double time; //Time (in ticks) of the keyframe
	aiVector3D value; //Position at this keyframe
};


struct RotationKey {

	double time;		// Time in ticks of the keyframe
	aiQuaternion value; //Rotation (quaternion) at this keyframe
};

struct ScalingKey {

	double time; //Time in ticks
	aiVector3D value; //scale at this keyframe
};

struct BoneAnimation {


	std::string boneName; //the name of the bone affected by given animation
	std::vector<PositionKey> positions; //position keyframes
	std::vector<RotationKey> rotations; //rotation keyframe
	std::vector<ScalingKey> scales;
};


struct AnimationData {

	double duration; //total duration of the animation (in ticks)
	double ticksPerSecond; //speed
	std::vector<BoneAnimation> boneAnimations; //animation data per bone
};




class AnimationController {

public:
	//AnimationController();

	//checker functions
	bool isNodeNameValid(const aiNode* node);

	void updateBoneTransformations(float currentTime,  aiNode* node, const aiMatrix4x4& parentTransform);
	const BoneAnimation* findBoneAnimation(const std::string& boneName);
	bool isValidForAnimation(const aiNode* rootNode);//error checking in ::render function for when aniacontroller is incorrectly called
	//helper  functions to interpolate between keyframes
	aiVector3D interpolatePosition(float animationTime, const BoneAnimation& boneAnim);
	aiQuaternion interpolateRotation(float animationTime, const BoneAnimation& boneAnim);
	aiVector3D interpolateScaling(float animationTime, const BoneAnimation& boneAnim);

	//

	
	std::unordered_map<std::string, aiMatrix4x4> boneTransforms;

private:
	//template <typename KeyType>
	//unsigned int findKeyframeIndex(float animationTime, const std::vector<KeyType>& keys);
	//store final transofrms for each bone.

	aiMatrix4x4 globalInverseTransform;

	// Templated function to find keyframe index
	template<typename KeyType>
	unsigned int findKeyframeIndex(float animationTime, const std::vector<KeyType>& keys) {
		if (keys.size() == 1) {
			return 0;  // Only one keyframe
		}

		unsigned int low = 0;
		unsigned int high = keys.size() - 1;

		while (low < high) {
			unsigned int mid = low + (high - low) / 2;
			if (keys[mid].time < animationTime) {
				low = mid + 1; // Move to the right half
			}
			else {
				high = mid; // Move to the left half (includes mid)
			}
		}

		return low > 0 ? low - 1 : 0; // Return the index of the keyframe before animationTime
	}
};





/*notes or docs
the Skin is essentially the VAO which is positions, texcoords, normal, jointIDs, weights

*/