#ifndef MODEL_H
#define MODEL_H

#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp> // for to_string
#include "Shader.h"
#include "Drawable.h"
#include  "Mesh.h"
#include  "Mirror.h"
#include <stb_image.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

class Model{

public:

        Model(std::string path_to_current_folder){
            this->path = path_to_current_folder;
            loadModel(path_to_current_folder);
                
        }

       

private:
    std::string path;
    std::vector<Mesh> meshes;
    std::string file_directory;
    std::vector<Text> textures_loaded;


    void loadModel(std::string path){
        Assimp::Importer importer;
        const aiScene *scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenNormals | aiProcess_OptimizeMeshes);
        //Assimp load the model and deal with the different formats specifications
        //ReadFile function needs a directory path, then process: transform all the model's primitive into triangles, 
        //flips the text coords arounf y axis (textures are often reversed) and optimize by joining meshes into one larger

        if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) 
            {
                //check if scene is null or data incomplete
                std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
                return;
            }
            file_directory = path.substr(0, path.find_last_of('/')); //find directory path

            processNode(scene->mRootNode, scene); //pass the first node = root node to processNode funtion
        }  
        
    void processNode(aiNode *node, const aiScene *scene){ //recursive function until all nodes are processed
        //process all the node's meshes
        for (unsigned int i = 0; i < node->mNumMeshes; i++){ //check each of the mesh indices and retrieve the corresponding mesh by indexing the scene mMeshes array
            aiMesh *mesh = scene->mMeshes[node->mMeshes[i]]; //retrieve all the meshes of each node 
            meshes.push_back(processMesh(mesh, scene)); //processMesh return a Mesh object 
        }

        for (unsigned int i = 0; i < node->mNumChildren; i++){
            processNode(node->mChildren[i], scene); //recursive call to process each of its children
        }
    }
    std::vector<Text> loadMaterialTextures(aiMaterial *material, aiTextureType type, std::string type_name){
        std::vector<Text> textures;
        for(unsigned int i = 0; i < material->GetTextureCount(type); i++){ //give the amount of texture in the material
            aiString string;
            material->GetTexture(type, i, &string); //gives each texture's file in aiString
            bool skip = false;
            for(unsigned int j =0; j < textures_loaded.size(); j++){
                if(std::strcmp(textures_loaded[j].path.data(), string.C_Str()) == 0){
                    textures.push_back(textures_loaded[j]);
                    skip = true; 
                    break;
                }
            }
            if(!skip){ //if texture hasn't been already loaded, load it
                Text texture;
                texture.Texture_ID = TextureFromFile(string.C_Str(), this->file_directory); //stb_image library function that loads a texture and return its id
                texture.type = type_name;
                texture.path = string.C_Str();
                textures.push_back(texture);
                textures_loaded.push_back(texture);
            }
        }
        return textures;

    }

    Mesh processMesh(aiMesh *mesh, const aiScene *scene){
        std::vector<unsigned int> indices;
        std::vector<Text> textures;
        std::vector<float> vertices;
        
        for(unsigned int i=0; i < mesh->mNumVertices; i++){ //3 parts: retrieve vertices, indices, material
            
            vertices.push_back(mesh->mVertices[i].x); //Assimp's vertex position called mVertices
            vertices.push_back(mesh->mVertices[i].y);
            vertices.push_back(mesh->mVertices[i].z);

            vertices.push_back(mesh->mNormals[i].x); //Normals
            vertices.push_back(mesh->mNormals[i].y);
            vertices.push_back(mesh->mNormals[i].z);
            
            if(mesh->mTextureCoords[0]){ //check  if mesh contains texture 
                vertices.push_back(mesh->mTextureCoords[0][i].x);
                vertices.push_back(mesh->mTextureCoords[0][i].y);
            }
            else 
            vertices.push_back(0.0f);
            vertices.push_back(0.0f);
        }

        for (unsigned int i = 0; i < mesh->mNumFaces; i++){ //as have asked aiProcess_Triangulate => know that each primitive is a triangle
        // each mesh has array of faces, each face contains indices to draw the primitive
            aiFace face = mesh->mFaces[i];
            for (unsigned int j = 0; j < face.mNumIndices; j++){
                indices.push_back(face.mIndices[j]);
            }
        }

        if(mesh->mMaterialIndex >= 0){
            aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex]; //retrieve material from the mMaterials array
            std::vector<Text> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");  //load and initialize the array of texture locations stored in the material
            textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
            std::vector<Text> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
            textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
        }

        return Mesh(path, vertices, indices, textures);
    }



    unsigned int TextureFromFile(const char *path, const std::string &directory){
        std::string filename = std::string(path);
        filename = directory + '/' + filename;

        unsigned int textureID; 
        glGenTextures(1, &textureID);

        int width, height, nrComponents; 
        unsigned char *data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
        if(data){
            GLenum format;
            if(nrComponents==1) format = GL_RED;
            else if(nrComponents==3) format = GL_RGB;
            else if(nrComponents==4) format = GL_RGBA;
            glBindTexture(GL_TEXTURE_2D, textureID);
            glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

            stbi_image_free(data);
        }
        else 
            {
                std::cout << "Texture failed to load at path: " << path << std::endl;
                stbi_image_free(data);
            }

    return textureID;
    }

    void draw(Shader &shader){
        for(unsigned int i = 0; i < meshes.size(); i++){
            meshes[i].drawMesh(shader);
        }
    }



};

#endif