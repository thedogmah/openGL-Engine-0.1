#include "Animation.h"
#include <iostream>
#include "globals.h"
const unsigned int MAX_CHILDREN = 5;  // Choose a sensible limit based on your expected data

aiVector3D operator*(float scalar, const aiVector3D& vec) {
    return aiVector3D(vec.x * scalar, vec.y * scalar, vec.z * scalar);
}

bool AnimationController::isNodeNameValid(const aiNode* node) {
    return node && node->mName.data && node->mName.length > 0;
}
void AnimationController::updateBoneTransformations(float animationTime,  aiNode* node, const aiMatrix4x4& parentTransform)
{
    if (!node || !node->mName.data || node->mName.length == 0) {
        std::cerr << "Error: Invalid or null node encountered." << std::endl;
        return; // Early exit
    }

    std::string nodeName(node->mName.data);


    //Find the bone animation data corresponding to this node ^
    const BoneAnimation* boneAnim = findBoneAnimation(nodeName);

    aiMatrix4x4 nodeTransform = node->mTransformation;

    //Apply the transformation from the animation (if present)
    if (boneAnim)
    {
        aiVector3D translation = interpolatePosition(animationTime, *boneAnim);
        aiQuaternion rotation = interpolateRotation(animationTime, *boneAnim);
        aiVector3D scale = interpolateScaling(animationTime, *boneAnim);


        //Convert transormtion components to a matrix.
        aiMatrix4x4 translationMatrix;
        aiMatrix4x4::Translation(translation, translationMatrix);

        aiMatrix4x4 rotationMatrix = aiMatrix4x4(rotation.GetMatrix());

        aiMatrix4x4 scalingMatrix;
        aiMatrix4x4::Scaling(scale, scalingMatrix);

        nodeTransform = translationMatrix * rotationMatrix * scalingMatrix;

    }

    aiMatrix4x4 finalTransform = parentTransform * nodeTransform;

    boneTransforms[nodeName] = finalTransform;
    // Child processing
    if (node->mNumChildren > 0 && node->mNumChildren < 4) {  // Verify children count
        for (unsigned int i = 0; i < node->mNumChildren; ++i) {
            if (node->mChildren[i]) {  // Null-check each child node
                updateBoneTransformations(animationTime, node->mChildren[i], finalTransform);
            }
            else {
                std::cout << "Warning: 2koy node is null." << std::endl;
            }
        }
    }
    else {
        std::cerr << "Warning: Unexpected number of children (" << node->mNumChildren
            << "). Possible data corruption." << std::endl;
    }
}


std::vector<AnimationData> animationsNew;
const BoneAnimation* AnimationController::findBoneAnimation(const std::string& boneName) {
    for (const auto& animation : animationsNew) {
        for (const auto& boneAnim : animation.boneAnimations) {
            if (boneAnim.boneName == boneName) {
                return &boneAnim;
            }
        }
    }
    return nullptr;
}
aiVector3D AnimationController::interpolatePosition(float animationTime, const BoneAnimation& boneAnim)
{
    if (boneAnim.positions.size() == 1) {
        return boneAnim.positions[0].value;
    }

    unsigned int frameIndex = findKeyframeIndex(animationTime, boneAnim.positions);
    unsigned int nextFrameIndex = frameIndex + 1;

    if (nextFrameIndex >= boneAnim.positions.size()) {
        throw std::out_of_range("nextFrameIndex out of bounds for positions");
    }

    double deltaTime = boneAnim.positions[nextFrameIndex].time - boneAnim.positions[frameIndex].time;
    if (deltaTime <= 0.0) {
        throw std::runtime_error("Delta time between position keyframes is zero or negative");
    }

    // Perform interpolation (not implemented in your current code)
    double factor = (animationTime - boneAnim.positions[frameIndex].time) / deltaTime;
    const aiVector3D& start = boneAnim.positions[frameIndex].value;
    const aiVector3D& end = boneAnim.positions[nextFrameIndex].value;

    return start + factor * (end - start);
}aiQuaternion AnimationController::interpolateRotation(float animationTime, const BoneAnimation& boneAnim)
{
    if (boneAnim.rotations.size() == 1) {
        return boneAnim.rotations[0].value;
    }

    unsigned int frameIndex = findKeyframeIndex(animationTime, boneAnim.rotations);
    unsigned int nextFrameIndex = frameIndex + 1;

    if (nextFrameIndex >= boneAnim.rotations.size()) {
        throw std::out_of_range("nextFrameIndex out of bounds for rotations");
    }

    double deltaTime = boneAnim.rotations[nextFrameIndex].time - boneAnim.rotations[frameIndex].time;
    if (deltaTime <= 0.0) {
        throw std::runtime_error("Delta time between rotation keyframes is zero or negative");
    }

    double factor = (animationTime - boneAnim.rotations[frameIndex].time) / deltaTime;

    const aiQuaternion& start = boneAnim.rotations[frameIndex].value;
    const aiQuaternion& end = boneAnim.rotations[nextFrameIndex].value;

    aiQuaternion interpolated;
    aiQuaternion::Interpolate(interpolated, start, end, factor);
    interpolated.Normalize();
    return interpolated;
}
aiVector3D AnimationController::interpolateScaling(float animationTime, const BoneAnimation& boneAnim)
{
    if (boneAnim.scales.size() == 1) {
        return boneAnim.scales[0].value;
    }

    unsigned int frameIndex = findKeyframeIndex(animationTime, boneAnim.scales);
    unsigned int nextFrameIndex = frameIndex + 1;

    if (nextFrameIndex >= boneAnim.scales.size()) {
        throw std::out_of_range("nextFrameIndex out of bounds for scales");
    }

    double deltaTime = boneAnim.scales[nextFrameIndex].time - boneAnim.scales[frameIndex].time;
    if (deltaTime <= 0.0) {
        throw std::runtime_error("Delta time between scale keyframes is zero or negative");
    }

    double factor = (animationTime - boneAnim.scales[frameIndex].time) / deltaTime;

    const aiVector3D& start = boneAnim.scales[frameIndex].value;
    const aiVector3D& end = boneAnim.scales[nextFrameIndex].value;

    return start + factor * (end - start);
}



