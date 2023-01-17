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
#include "Model.h"
struct AssimpNodeData{
    glm::mat4 transformation;
    std::string name;
    int childrenCount;
    std::vector<AssimpNodeData> children;
};
struct BoneInfo{
    int id; //id of the bone matrix
    glm::mat4 offset; //transform vertices from model space to bone space
};

class Animation{
    public: 

    float m_duration_animation;
    int m_tick_per_second;
    std::vector<Bone> m_bones;
    AssimpNodeData m_root_node;
    std::map<std::string, BoneInfo> m_boneMap;
    Animation() = default;

    Animation(const std::string& animationPath, Model* model){
        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFile(animationPath, aiProcess_Triangulate | aiProcess_GenNormals | aiProcess_OptimizeMeshes);
        assert(scene && scene->mRootNode);
        auto animation = scene->mAnimations[0];

        m_duration_animation = animation->mDuration;
        m_tick_per_second = animation->mTicksPerSecond;
        aiMatrix4x4 globalTransfo = scene->mRootNode->mTransformation;
        globalTransfo = globalTransfo.Inverse();
        read_data(m_root_node, scene->mRootNode);

    }
    Bone* findBone(const std::string& name){
        for (int i = 0; i < m_bones.size(); i++){
            if (m_bones[i].GetBoneName() == name){
                return &m_bones[i];
            }
            else return nullptr;
            
        }
    }


	// inline const std::map<std::string,BoneInfo>& GetBoneIDMap() 
	// { 
	// 	return m_boneMap;
	// }
    private:
        void read_data(AssimpNodeData& nodeData, const aiNode* src){
        
        nodeData.name = src->mName.data;
        nodeData.transformation = AssimpGLMHelpers::ConvertMatrixToGLMFormat(src->mTransformation);
        nodeData.childrenCount = src->mNumChildren;

        for(int i = 0; i < src->mNumChildren; i++){
            AssimpNodeData newAssimpNodeData;
            read_data(newAssimpNodeData, src->mChildren[i]);
            nodeData..children.pushbak(newAssimpNodeData);
        }
    }


};

#endif