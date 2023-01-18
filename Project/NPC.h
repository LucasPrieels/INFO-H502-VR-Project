#ifndef NPC_H
#define NPC_H

#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp> // for to_string
#include "Shader.h"
#include "Mesh.h"
#include "Bone.h"
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/Importer.hpp>
#include "AssimpGLMHelpers.h"
#include <map>
#include <vector>

struct Vertex {
    // position
    glm::vec3 Position;
    // normal
    glm::vec3 Normal;
    // texCoords
    glm::vec2 TexCoords;
    //bone indexes which will influence this vertex
    float m_BoneIDs[MAX_BONE_INFLUENCE];
};

struct BoneInfo{
    /*id is index in finalBoneMatrices*/
    int id;
    /*offset matrix transforms vertex from model space to bone space*/
    glm::mat4 offset;
};

class NPC{
public:
    std::vector<Tex> textures;
    std::vector<Mesh> meshes;
    Shader shader_NPC;
    std::map<std::string, BoneInfo> m_BoneInfoMap;
    int m_BoneCounter = 0;

    NPC(std::string path_string, std::string NPC_model_filename) :
        shader_NPC(path_string + "vertex_shader_NPC.txt", path_string + "fragment_shader_NPC.txt")
    {
        this->path_string = path_string + "vampire/";
        loadModel(path_string, NPC_model_filename);
    }

    // draws the model, and thus all its meshes
    void Draw(glm::mat4 view, glm::mat4 projection, std::vector<glm::mat4> transforms){
        shader_NPC.use();
        for (int i = 0; i < transforms.size(); ++i) shader_NPC.set_uniform("finalBonesMatrices[" + std::to_string(i) + "]", transforms[i]);
        for(unsigned int i = 0; i < meshes.size(); i++) meshes[i].draw_mesh(glm::vec3(0.0f, 6.5f, -8.0f), view, projection, shader_NPC);
    }


private:
    // loads a model with supported ASSIMP extensions from file and stores the resulting meshes in the meshes std::vector.
    void loadModel(std::string path, std::string NPC_model_filename){
        // read file via ASSIMP
        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFile(path+NPC_model_filename, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_CalcTangentSpace);
        // check for errors
        if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // if is Not Zero
        {
            std::cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << std::endl;
            return;
        }

        // process ASSIMP's root node recursively
        processNode(scene->mRootNode, scene);
    }

    // processes a node in a recursive fashion. Processes each individual mesh located at the node and repeats this process on its children nodes (if any).
    void processNode(aiNode *node, const aiScene *scene){
        // process each mesh located at the current node
        for(unsigned int i = 0; i < node->mNumMeshes; i++){
            // the node object only contains indices to index the actual objects in the scene.
            // the scene contains all the data, node is just to keep stuff organized (like relations between nodes).
            aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
            meshes.push_back(processMesh(mesh, scene));
        }
        // after we've processed all of the meshes (if any) we then recursively process each of the children nodes
        for(unsigned int i = 0; i < node->mNumChildren; i++){
            processNode(node->mChildren[i], scene);
        }
    }

    void SetVertexBoneDataToDefault(Vertex& vertex){
        for (int i = 0; i < MAX_BONE_INFLUENCE; i++){
            vertex.m_BoneIDs[i] = -1.0f;
        }
    }


    Mesh processMesh(aiMesh* mesh, const aiScene* scene){
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;
        std::vector<Tex> textures;

        for (unsigned int i = 0; i < mesh->mNumVertices; i++){
            Vertex vertex;
            SetVertexBoneDataToDefault(vertex);
            vertex.Position = AssimpGLMHelpers::GetGLMVec(mesh->mVertices[i]);
            vertex.Normal = AssimpGLMHelpers::GetGLMVec(mesh->mNormals[i]);

            if (mesh->mTextureCoords[0]){
                glm::vec2 vec;
                vec.x = mesh->mTextureCoords[0][i].x;
                vec.y = mesh->mTextureCoords[0][i].y;
                vertex.TexCoords = vec;
            }
            else vertex.TexCoords = glm::vec2(0.0f, 0.0f);

            vertices.push_back(vertex);
        }
        for (unsigned int i = 0; i < mesh->mNumFaces; i++)
        {
            aiFace face = mesh->mFaces[i];
            for (unsigned int j = 0; j < face.mNumIndices; j++)
                indices.push_back(face.mIndices[j]);
        }
        aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

        std::vector<Tex> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
        textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
        std::vector<Tex> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
        textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
        std::vector<Tex> normalMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal");
        textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
        std::vector<Tex> heightMaps = loadMaterialTextures(material, aiTextureType_AMBIENT, "texture_height");
        textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());

        ExtractBoneWeightForVertices(vertices,mesh,scene);

        std::vector<float> vertices_float;
        for (int i = 0; i < vertices.size(); i++){
            for (int j = 0; j < 3; j++) vertices_float.push_back(vertices[i].Position[j]);
            for (int j = 0; j < 3; j++) vertices_float.push_back(vertices[i].Normal[j]);
            for (int j = 0; j < 2; j++) vertices_float.push_back(vertices[i].TexCoords[j]);
            for (int j = 0; j < MAX_BONE_INFLUENCE; j++) vertices_float.push_back(vertices[i].m_BoneIDs[j]);
        }

        return Mesh(vertices_float, indices, textures);
    }

    void SetVertexBoneData(Vertex& vertex, int boneID){
        for (int i = 0; i < MAX_BONE_INFLUENCE; ++i){
            if (vertex.m_BoneIDs[i] < 0){
                vertex.m_BoneIDs[i] = (float)boneID;
                break;
            }
        }
    }


    void ExtractBoneWeightForVertices(std::vector<Vertex>& vertices, aiMesh* mesh, const aiScene* scene){
        auto& boneInfoMap = m_BoneInfoMap;
        int& boneCount = m_BoneCounter;

        for (int boneIndex = 0; boneIndex < mesh->mNumBones; ++boneIndex){
            int boneID = -1;
            std::string boneName = mesh->mBones[boneIndex]->mName.C_Str();
            BoneInfo newBoneInfo;
            newBoneInfo.id = boneCount;
            newBoneInfo.offset = AssimpGLMHelpers::ConvertMatrixToGLMFormat(mesh->mBones[boneIndex]->mOffsetMatrix);
            boneInfoMap[boneName] = newBoneInfo;
            boneID = boneCount;
            boneCount++;
            assert(boneID != -1);
            auto weights = mesh->mBones[boneIndex]->mWeights;
            int numWeights = mesh->mBones[boneIndex]->mNumWeights;

            for (int weightIndex = 0; weightIndex < numWeights; ++weightIndex){
                int vertexId = weights[weightIndex].mVertexId;
                assert(vertexId <= vertices.size());
                SetVertexBoneData(vertices[vertexId], boneID);
            }
        }
    }


    unsigned int TextureFromFile(const char* path, std::string directory)
    {
        std::string filename = std::string(path);
        filename = directory + '/' + filename;

        unsigned int textureID;
        glGenTextures(1, &textureID);

        int width, height, nrComponents;
        unsigned char* data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
        if (data)
        {
            GLenum format;
            if (nrComponents == 1)
                format = GL_RED;
            else if (nrComponents == 3)
                format = GL_RGB;
            else if (nrComponents == 4)
                format = GL_RGBA;

            glBindTexture(GL_TEXTURE_2D, textureID);
            glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            stbi_image_free(data);
        }
        else
        {
            std::cout << "Texture failed to load at path: " << filename << std::endl;
            stbi_image_free(data);
        }

        return textureID;
    }

    // checks all material textures of a given type and loads the textures if they're not loaded yet.
    // the required info is returned as a Texture struct.
    std::vector<Tex> loadMaterialTextures(aiMaterial *mat, aiTextureType type, std::string typeName)
    {
        std::vector<Tex> textures;
        for(unsigned int i = 0; i < mat->GetTextureCount(type); i++)
        {
            aiString str;
            mat->GetTexture(type, i, &str);
            // check if texture was loaded before and if so, continue to next iteration: skip loading a new texture
            bool skip = false;
            for(unsigned int j = 0; j < textures.size(); j++)
            {
                if(std::strcmp(textures[j].path.data(), str.C_Str()) == 0)
                {
                    textures.push_back(textures[j]);
                    skip = true; // a texture with the same filepath has already been loaded, continue to next one. (optimization)
                    break;
                }
            }
            if(!skip)
            {   // if texture hasn't been loaded already, load it
                Tex texture;
                texture.id = TextureFromFile(str.C_Str(), path_string);
                texture.type = typeName;
                texture.path = str.C_Str();
                textures.push_back(texture);
                textures.push_back(texture);  // store it as texture loaded for entire model, to ensure we won't unnecesery load duplicate textures.
            }
        }
        return textures;
    }

    std::string path_string;
};


#endif