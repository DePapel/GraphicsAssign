#pragma once

#include <glm/glm.hpp>

#include <string>
#include <vector>
#include <array>

#include "animations.h"

class CS300Parser
{
  public:
    void LoadDataFromFile(const char * filename);

    float     fovy      = 60.0f;
    float     width     = 16.0f;
    float     height    = 9.0f;
    float     nearPlane = 1.0f;
    float     farPlane  = 500.0f;
    glm::vec3 camPos;
    glm::vec3 camTarget;
    glm::vec3 camUp;

    struct Transform
    {
        std::string name;

        std::string mesh;
        std::string normalMap = "data/textures/default_normal.png";

        glm::vec3 pos;
        glm::vec3 rot;
        glm::vec3 sca;
        float     ns        = 10.0f;
        float     ior       = 1.33f;
        bool      reflector = false;

        std::vector<Animations::Anim> anims;
        glm::vec3 startPos;
    };

    std::vector<Transform> objects;

    struct Light_info
    {
        glm::vec3 pos;
        glm::vec3 dir;
        glm::vec3 col;
        glm::vec3 att;
        float     amb     = 0.0f;
        float     inner   = 0.0f;
        float     outer   = 30.0f;
        float     falloff = 1.0f;
        float     bias    = 0.0f;
        unsigned  pcf     = 0;

        std::string type = "POINT";

        std::vector<Animations::Anim> anims;
    };
    std::vector<Light_info> lights;

    std::array<std::string, 6> environmentMap;

  private:
    static float     ReadFloat(std::ifstream & f);
    static int       ReadInt(std::ifstream & f);
    static glm::vec3 ReadVec3(std::ifstream & f);
};