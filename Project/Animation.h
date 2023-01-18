#ifndef ANIMATION_H
#define ANIMATION_H

#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp> // for to_string
#include <array>
#include <vector>
#include <map>
#include <assimp/scene.h>
#include "Bone.h"
#include "NPC.h"
#define MAX_BONE_INFLUENCE 4

struct AssimpNodeData{
    glm::mat4 transformation;
    std::string name;
    int childrenCount;
    std::vector<AssimpNodeData> children;
};

class Animation{
public:
    Animation(std::string animationPath, NPC* model){
        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFile(animationPath, aiProcess_Triangulate);
        assert(scene && scene->mRootNode);
        auto animation = scene->mAnimations[0];
        m_Duration = animation->mDuration;
        m_TicksPerSecond = animation->mTicksPerSecond;
        aiMatrix4x4 globalTransformation = scene->mRootNode->mTransformation;
        globalTransformation = globalTransformation.Inverse();
        readHeirarchyData(m_RootNode, scene->mRootNode);
        ReadMissingBones(animation, *model);

        m_CurrentTime = 0.0;

        m_FinalBoneMatrices.reserve(100);

        for (int i = 0; i < 100; i++)
            m_FinalBoneMatrices.push_back(glm::mat4(1.0f));
    }

    void UpdateAnimation(float dt){
        m_CurrentTime += this->GetTicksPerSecond() * dt;
        m_CurrentTime = fmod(m_CurrentTime, this->GetDuration());
        CalculateBoneTransform(&this->GetRootNode(), glm::mat4(1.0f));
    }

    Bone* FindBone(const std::string& name){
        auto iter = std::find_if(m_Bones.begin(), m_Bones.end(),
                                 [&](const Bone& Bone)
                                 {
                                     return Bone.GetBoneName() == name;
                                 }
        );
        if (iter == m_Bones.end()) return nullptr;
        else return &(*iter);
    }


    inline float GetTicksPerSecond() { return m_TicksPerSecond; }
    inline float GetDuration() { return m_Duration;}
    inline const AssimpNodeData& GetRootNode() { return m_RootNode; }
    inline const std::map<std::string,BoneInfo>& GetBoneIDMap()
    {
        return m_BoneInfoMap;
    }

    void CalculateBoneTransform(const AssimpNodeData* node, glm::mat4 parentTransform){
        std::string nodeName = node->name;
        glm::mat4 nodeTransform = node->transformation;

        Bone* Bone = this->FindBone(nodeName);

        if (Bone){
            Bone->Update(m_CurrentTime);
            nodeTransform = Bone->GetLocalTransform();
        }
        glm::mat4 globalTransformation = parentTransform * nodeTransform;

        auto boneInfoMap = this->GetBoneIDMap();
        if (boneInfoMap.find(nodeName) != boneInfoMap.end()){
            int index = boneInfoMap[nodeName].id;
            glm::mat4 offset = boneInfoMap[nodeName].offset;
            m_FinalBoneMatrices[index] = globalTransformation * offset;
        }

        for (int i = 0; i < node->childrenCount; i++) CalculateBoneTransform(&node->children[i], globalTransformation);
    }

    std::vector<glm::mat4> GetFinalBoneMatrices(){
        return m_FinalBoneMatrices;
    }

private:
    void ReadMissingBones(const aiAnimation* animation, NPC& model)
    {
        int size = animation->mNumChannels;

        auto& boneInfoMap = model.m_BoneInfoMap;//getting m_BoneInfoMap from NPC class
        int& boneCount = model.m_BoneCounter; //getting the m_BoneCounter from NPC class

        //reading channels(bones engaged in an animation and their keyframes)
        for (int i = 0; i < size; i++)
        {
            auto channel = animation->mChannels[i];
            std::string boneName = channel->mNodeName.data;

            if (boneInfoMap.find(boneName) == boneInfoMap.end())
            {
                boneInfoMap[boneName].id = boneCount;
                boneCount++;
            }
            m_Bones.push_back(Bone(channel->mNodeName.data,
                                   boneInfoMap[channel->mNodeName.data].id, channel));
        }

        m_BoneInfoMap = boneInfoMap;
    }

    void readHeirarchyData(AssimpNodeData& dest, const aiNode* src)
    {
        assert(src);

        dest.name = src->mName.data;
        dest.transformation = AssimpGLMHelpers::ConvertMatrixToGLMFormat(src->mTransformation);
        dest.childrenCount = src->mNumChildren;

        for (int i = 0; i < src->mNumChildren; i++)
        {
            AssimpNodeData newData;
            readHeirarchyData(newData, src->mChildren[i]);
            dest.children.push_back(newData);
        }
    }
    float m_Duration;
    int m_TicksPerSecond;
    std::vector<Bone> m_Bones;
    AssimpNodeData m_RootNode;
    std::map<std::string, BoneInfo> m_BoneInfoMap;
    std::vector<glm::mat4> m_FinalBoneMatrices;
    float m_CurrentTime;
};

#endif