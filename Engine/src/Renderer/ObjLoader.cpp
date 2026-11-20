#include "Forge/Renderer/Mesh.h"
#include "Forge/Core/Log.h"
#include <glm/glm.hpp>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

namespace Forge {

    namespace {
        struct ObjIndex {
            int v  = -1;
            int vt = -1;
            int vn = -1;
        };

        int ResolveIndex(int idx, int count) {
            if (idx > 0) return idx - 1;
            if (idx < 0) return count + idx;
            return -1;
        }

        ObjIndex ParseFaceToken(const std::string& token, int vCount, int vtCount, int vnCount) {
            ObjIndex res;
            size_t s1 = token.find('/');
            if (s1 == std::string::npos) {
                res.v = ResolveIndex(std::stoi(token), vCount);
                return res;
            }

            if (s1 > 0) {
                res.v = ResolveIndex(std::stoi(token.substr(0, s1)), vCount);
            }

            size_t s2 = token.find('/', s1 + 1);
            if (s2 == std::string::npos) {
                if (s1 + 1 < token.size()) {
                    res.vt = ResolveIndex(std::stoi(token.substr(s1 + 1)), vtCount);
                }
            } else {
                if (s2 > s1 + 1) {
                    res.vt = ResolveIndex(std::stoi(token.substr(s1 + 1, s2 - s1 - 1)), vtCount);
                }
                if (s2 + 1 < token.size()) {
                    res.vn = ResolveIndex(std::stoi(token.substr(s2 + 1)), vnCount);
                }
            }
            return res;
        }
    }

    std::unique_ptr<VertexArray> Mesh::LoadOBJ(const std::string& filepath) {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            FE_ERROR("OBJ fayl ochilmadi: " + filepath);
            return nullptr;
        }

        std::vector<glm::vec3> positions;
        std::vector<glm::vec2> texcoords;
        std::vector<glm::vec3> normals;
        std::vector<Vertex> vertices;

        std::string line;
        while (std::getline(file, line)) {
            if (line.empty() || line[0] == '#') continue;

            std::istringstream iss(line);
            std::string tag;
            iss >> tag;

            if (tag == "v") {
                glm::vec3 p(0.0f);
                iss >> p.x >> p.y >> p.z;
                positions.push_back(p);
            } else if (tag == "vt") {
                glm::vec2 uv(0.0f);
                iss >> uv.x >> uv.y;
                texcoords.push_back(uv);
            } else if (tag == "vn") {
                glm::vec3 n(0.0f, 1.0f, 0.0f);
                iss >> n.x >> n.y >> n.z;
                normals.push_back(n);
            } else if (tag == "f") {
                std::vector<ObjIndex> face;
                std::string tok;
                while (iss >> tok) {
                    face.push_back(ParseFaceToken(
                        tok,
                        static_cast<int>(positions.size()),
                        static_cast<int>(texcoords.size()),
                        static_cast<int>(normals.size())
                    ));
                }

                // Ko'pburchaklarni (quad va boshqalarni) uchburchaklarga bo'lish
                for (size_t i = 1; i + 1 < face.size(); ++i) {
                    ObjIndex tri[3] = { face[0], face[i], face[i + 1] };

                    glm::vec3 p0 = (tri[0].v >= 0 && tri[0].v < (int)positions.size()) ? positions[tri[0].v] : glm::vec3(0.0f);
                    glm::vec3 p1 = (tri[1].v >= 0 && tri[1].v < (int)positions.size()) ? positions[tri[1].v] : glm::vec3(0.0f);
                    glm::vec3 p2 = (tri[2].v >= 0 && tri[2].v < (int)positions.size()) ? positions[tri[2].v] : glm::vec3(0.0f);

                    glm::vec3 flatNormal = glm::cross(p1 - p0, p2 - p0);
                    float len = glm::length(flatNormal);
                    flatNormal = (len > 1e-6f) ? (flatNormal / len) : glm::vec3(0.0f, 1.0f, 0.0f);

                    for (int k = 0; k < 3; ++k) {
                        glm::vec3 pos = (tri[k].v >= 0 && tri[k].v < (int)positions.size()) ? positions[tri[k].v] : glm::vec3(0.0f);
                        glm::vec2 uv  = (tri[k].vt >= 0 && tri[k].vt < (int)texcoords.size()) ? texcoords[tri[k].vt] : glm::vec2(0.0f);
                        glm::vec3 nrm = (tri[k].vn >= 0 && tri[k].vn < (int)normals.size()) ? normals[tri[k].vn] : flatNormal;

                        vertices.push_back({
                            pos.x, pos.y, pos.z,
                            1.0f, 1.0f, 1.0f,
                            nrm.x, nrm.y, nrm.z,
                            uv.x, uv.y
                        });
                    }
                }
            }
        }

        if (vertices.empty()) {
            FE_ERROR("OBJ faylda geometriya topilmadi: " + filepath);
            return nullptr;
        }

        FE_INFO("OBJ yuklandi: " + filepath + " (" + std::to_string(vertices.size()) + " vertex)");
        return std::make_unique<VertexArray>(vertices);
    }

}
