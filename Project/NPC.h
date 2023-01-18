#ifndef NPC_H
#define NPC_H

#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp> // for to_string
#include "Shader.h"
#include "Mesh.h"
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/Importer.hpp>

class NPC{
public:
    std::vector<Mesh> meshes;
    std::string file_directory;
    std::vector<Tex> textures_loaded;
    Shader shader_NPC;

    NPC(std::string path_string, std::string filename):
        shader_NPC(path_string + "vertex_shader_NPC.txt", path_string + "fragment_shader_NPC.txt")
    {
        load_model(path_string + filename);
    }

    void draw(glm::mat4 view, glm::mat4 projection, Sun sun, glm::vec3 camera_pos){
        shader_NPC.use();
        shader_NPC.set_uniform("light_color", sun.light_color);
        shader_NPC.set_uniform("light_pos", sun.light_pos);
        shader_NPC.set_uniform("viewing_pos", camera_pos);
        shader_NPC.set_uniform("shadow_texture_uniform", 1);
        shader_NPC.set_uniform("view_light", sun.view_light);
        shader_NPC.set_uniform("projection_light", sun.projection_light);
        for (unsigned int i = 0; i < meshes.size(); i++) meshes[i].draw_mesh(glm::vec3(0.0f), view, projection, shader_NPC);
    }

private:

    unsigned int import_texture(std::string filename){
        unsigned int textureID;
        glGenTextures(1, &textureID);

        int width, height, components_nb;
        unsigned char *data = stbi_load(filename.c_str(), &width, &height, &components_nb, 0);
        if(data){
            GLenum format;
            if (components_nb == 1) format = GL_RED; // Depending on the number of components of loaded texture the format is diferent
            else if (components_nb == 3) format = GL_RGB;
            else if (components_nb == 4) format = GL_RGBA;
            glBindTexture(GL_TEXTURE_2D, textureID);
            glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            stbi_image_free(data);
        }
        else{
            std::cout << "Import texture " << filename << " failed" << std::endl;
            stbi_image_free(data);
        }

        return textureID;
    }

    void load_model(std::string path){ // Load the NPC model at path
        Assimp::Importer importer;
        const aiScene *scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GenNormals | aiProcess_OptimizeMeshes);
        // Assimp loads the model and deals with the different formats specifications
        // ReadFile function needs a directory path, then process: transform all the model's primitive into triangles,
        // and optimize by joining meshes into larger ones

        // Check whether there has been an error
        if(!scene || !scene->mRootNode || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE){
            std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
            return;
        }
        file_directory = path.substr(0, path.find_last_of('/')); // Find the directory of the model, to load textures afterwards

        process_node(scene->mRootNode, scene); // Process each node starting by the root one
    }

    void process_node(aiNode *node, const aiScene *scene){ // Recursive function called on all nodes to process its children

        // Process the meshes corresponding to the node
        for (unsigned int i = 0; i < node->mNumMeshes; i++){
            aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
            meshes.push_back(process_mesh(mesh, scene));
        }

        // Recursive call on all children of the current node
        for (unsigned int i = 0; i < node->mNumChildren; i++){
            process_node(node->mChildren[i], scene);
        }
    }

    std::vector<Tex> load_textures_from_material(aiMaterial *material, aiTextureType type, std::string type_name){
        std::vector<Tex> textures;
        for(unsigned int i = 0; i < material->GetTextureCount(type); i++){ //Number of textures in the material
            aiString string;
            material->GetTexture(type, i, &string); // Gives each texture's file in string
            bool already_loaded = false; // Only load textures that have not been loaded previously
            for(unsigned int j = 0; j < textures_loaded.size(); j++){
                if(textures_loaded[j].path.data() == string.C_Str()){
                    textures.push_back(textures_loaded[j]);
                    already_loaded = true;
                    break;
                }
            }
            if(!already_loaded){ // If the texture hasn't been already loaded, load it
                Tex texture;
                texture.ID = import_texture(this->file_directory + "/" + string.C_Str());
                texture.type = type_name;
                texture.path = string.C_Str();
                textures.push_back(texture);
                textures_loaded.push_back(texture);
            }
        }
        return textures;
    }

    Mesh process_mesh(aiMesh *mesh, const aiScene *scene){
        // Extract the vertices, indices, and textures from the mesh
        std::vector<float> vertices;
        std::vector<unsigned int> indices;
        std::vector<Tex> textures;

        // Change of coordinates to scale down, translate, and rotate the mesh
        glm::mat4 model(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 6.4f, -4.0f));
        model = glm::scale(model, glm::vec3(0.08f));
        model = glm::rotate(model, glm::radians(90.0f), glm::vec3(-1.0f, 0.0f, 0.0f));

        for (unsigned int i=0; i < mesh->mNumVertices; i++){
            // Apply change of coordinates on position
            glm::vec3 vertex = glm::vec3(0.0f);
            vertex.x = mesh->mVertices[i].x;
            vertex.y = mesh->mVertices[i].y;
            vertex.z = mesh->mVertices[i].z;
            vertex = glm::vec3(model*glm::vec4(vertex, 1.0f));

            // We also have to apply the rotation on the normal vectors: since we rotate around axis -x, y becomes -z and z becomes y
            glm::vec3 normal = glm::vec3(0.0f);
            normal.x = mesh->mNormals[i].x;
            normal.y = mesh->mNormals[i].z;
            normal.z = -mesh->mNormals[i].y;
            normal = glm::vec3(glm::vec4(normal, 1.0f));

            vertices.push_back(vertex.x); // Assimp's vertex position called mVertices
            vertices.push_back(vertex.y);
            vertices.push_back(vertex.z);

            vertices.push_back(normal.x); // Normals
            vertices.push_back(normal.y);
            vertices.push_back(normal.z);

            if(mesh->mTextureCoords[0]){ // Add texture if the mesh contains some
                vertices.push_back(mesh->mTextureCoords[0][i].x);
                vertices.push_back(mesh->mTextureCoords[0][i].y);
            }
            else{
                vertices.push_back(0.0f);
                vertices.push_back(0.0f);
            }
        }

        // Extract indices from the list of faces (which are all triangles because we specified aiProcess_Triangulate)
        for (unsigned int i = 0; i < mesh->mNumFaces; i++){
            aiFace face = mesh->mFaces[i];
            for (unsigned int j = 0; j < face.mNumIndices; j++) indices.push_back(face.mIndices[j]);
        }

        // Extract materials of the mesh and their textures (we only consider diffuse ones)
        if(mesh->mMaterialIndex >= 0){
            aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex]; // Retrieve the material
            std::vector<Tex> diffuseMaps = load_textures_from_material(material, aiTextureType_DIFFUSE, "texture_diffuse");
            textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
        }

        return Mesh(vertices, indices, textures);
    }

};

#endif