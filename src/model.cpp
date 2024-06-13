#include "model.h"
#include "stb_image.h"

Model::Model(const char *path)
{
    // tell stb_image.h to flip loaded texture's on the y-axis (before loading model).
    stbi_set_flip_vertically_on_load(true);

    load_model(path);
}

void Model::draw(Shader &shader)
{
    for (int i = 0; i < meshes.size(); i++)
    {
        meshes[i].draw(shader);
    }
}

void Model::load_model(std::string path)
{
    Assimp::Importer importer;
    const aiScene *scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        std::cout << "ERROR:ASSIMP::" << importer.GetErrorString() << std::endl;
        return;
    }
    directory = path.substr(0, path.find_last_of('/'));

    process_node(scene->mRootNode, scene);
}

void Model::process_node(aiNode *node, const aiScene *scene)
{
    for (int i = 0; i < node->mNumMeshes; i++)
    {
        aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
        meshes.push_back(process_mesh(mesh, scene));
    }

    for (int i = 0; i < node->mNumChildren; i++)
    {
        process_node(node->mChildren[i], scene);
    }
}

Mesh Model::process_mesh(aiMesh *mesh, const aiScene *scene)
{
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture> textures;

    for (int i = 0; i < mesh->mNumVertices; i++)
    {
        Vertex vertex;
        // position
        glm::vec3 position;
        position.x = mesh->mVertices[i].x;
        position.y = mesh->mVertices[i].y;
        position.z = mesh->mVertices[i].z;
        vertex.position = position;
        // normals
        glm::vec3 normal;
        normal.x = mesh->mNormals[i].x;
        normal.y = mesh->mNormals[i].y;
        normal.z = mesh->mNormals[i].z;
        vertex.normal = normal;
        // texture coordinates
        if (mesh->mTextureCoords[0])
        {
            glm::vec2 texture_coords;
            texture_coords.x = mesh->mTextureCoords[0][i].x;
            texture_coords.y = mesh->mTextureCoords[0][i].y;
            vertex.texture_coords = texture_coords;
        }
        else
        {
            vertex.texture_coords = glm::vec2(0.0f, 0.0f);
        }
        vertices.push_back(vertex);
    }
    // indices
    for (int i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];
        for (int j = 0; j < face.mNumIndices; j++)
        {
            indices.push_back(face.mIndices[j]);
        }
    }
    // material
    if (mesh->mMaterialIndex >= 0)
    {
        aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];
        std::vector<Texture> diffuse_maps = load_material_textures(material, aiTextureType_DIFFUSE, "texture_diffuse");
        textures.insert(textures.end(), diffuse_maps.begin(), diffuse_maps.end());
        std::vector<Texture> specular_maps = load_material_textures(material, aiTextureType_SPECULAR, "texture_specular");
        textures.insert(textures.end(), specular_maps.begin(), specular_maps.end());
    }
    return Mesh(vertices, indices, textures);
}

std::vector<Texture> Model::load_material_textures(aiMaterial *material, aiTextureType type, std::string type_name)
{
    std::vector<Texture> textures;
    for (int i = 0; i < material->GetTextureCount(type); i++)
    {
        aiString path;
        material->GetTexture(type, i, &path);
        bool skip = false;
        for (int j = 0; j < textures_loaded.size(); j++)
        {
            textures.push_back(textures_loaded[i]);
            skip = true;
            break;
        }
        if (!skip)
        {
            Texture texture;
            texture.id = load_texture(path.C_Str(), directory);
            texture.type = type_name;
            texture.path = path.C_Str();
            textures.push_back(texture);
            textures_loaded.push_back(texture);
        }
    }
    return textures;
}

unsigned int Model::load_texture(char const *path, std::string &directory)
{
    std::string filename = directory + '/' + path;

    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, n_components;
    unsigned char *data = stbi_load(filename.c_str(), &width, &height, &n_components, 0);
    if (data)
    {
        GLenum format;
        if (n_components == 1)
        {
            format = GL_RED;
        }
        else if (n_components == 3)
        {
            format = GL_RGB;
        }
        else if (n_components == 4)
        {
            format = GL_RGBA;
        }

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
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}
