#ifndef ANIMATOR_H
#define ANIMATOR_H

#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp> // for to_string
#include <assimp/scene.h>
#include "Animation.h"
#include "Bone.h"

class Animator{
    public:

	std::vector<glm::mat4> m_FinalBoneMatrices;
	Animation* m_CurrentAnimation;
	float m_CurrentTime;
	float m_DeltaTime;

    Animator(Animation* animation){
        m_CurrentTime = 0.0;
        m_CurrentAnimation = animation;

        m_FinalBoneMatrices.reserve(100);

        for(int i = 0; i < 100; i++){
            m_FinalBoneMatrices.push_back(glm::mat4(1.0f));
        }
    }
    void Update(float delta_time){
        m_DeltaTime = delta_time;
        if(m_CurrentAnimation){
            m_CurrentTime += m_CurrentAnimation->m_tick_per_second * delta_time;
            m_CurrentTime = fmod(m_CurrentTime, m_CurrentAnimation->m_duration_animation);
            boneTransform(&m_CurrentAnimation->m_root_node, glm::mat4(1.0f));

        }
    }

    void boneTransform(const AssimpNodeData* node, glm::mat4 parentTransform){
        std::string nodeName = node->name;
        glm::mat4 nodeTransform = node->transformation;

        Bone* bone = m_CurrentAnimation->findBone(nodeName);
        if(bone){
            bone->Update(m_CurrentTime);
            nodeTransform = bone->GetLocalTransform();

            }
        glm::mat4 globalTransfo = parentTransform * nodeTransform;

        auto boneMap = m_CurrentAnimation->m_boneMap;
        if (boneMap.find(nodeName) != boneMap.end()){
            int index = boneMap[nodeName].id;
            glm::mat4 offset = boneMap[nodeName].offset;
            m_FinalBoneMatrices[index] = globalTransfo *offset;

        }

        for (int i = 0; i< node->childrenCount; i++){
            boneTransform(&node->children[i], globalTransfo);

        }
    }

    void play(Animation* animation){
        m_CurrentAnimation = animation;
        m_CurrentTime = 0.0f;
    }

    

};

#endif;