#ifndef MODEL_H
#define MODEL_H

#include <vector>

#include <assimp/Importer.hpp>
#include <assimp/mesh.h>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include "mesh.h"
#include "shader.h"

class Model
{
public:
    Model(const char *path);
    void draw(Shader &shader);

private:
    // model data
    std::vector<Mesh> meshes;
    std::vector<Texture> textures_loaded;
    std::string directory;

    void load_model(std::string path);
    void process_node(aiNode *node, const aiScene *scene);
    Mesh process_mesh(aiMesh *mesh, const aiScene *scene);
    std::vector<Texture> load_material_textures(aiMaterial *material, aiTextureType type, std::string type_name);
    unsigned int load_texture(char const *path, std::string &directory);
};

#endif