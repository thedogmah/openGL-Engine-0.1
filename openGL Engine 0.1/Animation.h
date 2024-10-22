#pragma once


#include <vector>
#include <string>
#include <assimp/scene.h>  // For aiVector3D, aiQuaternion, aiMatrix4x4, etc.

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

std::vector<AnimationData> animations;
