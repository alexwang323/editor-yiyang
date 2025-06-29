#ifndef PRIM_LINE_H
#define PRIM_LINE_H
#include "collider_primitive.h"
#include <sol/sol.hpp>
//[TODO] add the vector library we are using
class prim_line: public collider_primitive
{
    public:
        prim_line(glm::vec3 dir = glm::vec3(1,0,0), float rad = 1.0f);
        ~prim_line();
        glm::vec3 direction;
        float radius;
        glm::vec3 find_furthest(const glm::vec3& dir) override;
        void set_radius(const float p_radius);
        float get_radius() const;
        void set_direction(const glm::vec3 p_direction);
        glm::vec3 get_direction() const;
        static void CreatePrimLineLuaBindings(std::shared_ptr<sol::state> lua);
};
class prim_half_line : public prim_line
{
public:
    prim_half_line(glm::vec3 dir = glm::vec3(1, 0, 0), float rad = 1.0f);
    ~prim_half_line();
    glm::vec3 direction;
    float radius;
    glm::vec3 find_furthest(const glm::vec3& dir) override;
    static void CreatePrimHalfLineBindings(std::shared_ptr<sol::state> lua);
};


#endif
