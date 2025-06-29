#ifndef PRIM_CIRCLE_H
#define PRIM_CIRCLE_H
//[TODO] update this thing so it doesnt do relative positioning lol.
#include "collider_primitive.h"
#include <algorithm>
#include <sol/sol.hpp>
//[TODO] add the vector library we are using
class prim_circle: public collider_primitive
{
    public:
    prim_circle();
    ~prim_circle();
    glm::vec3 direction;
    float radius;
    glm::vec3 find_furthest(const glm::vec3& dir) override;
    void set_radius(const float p_radius);
    float get_radius() const;
    void set_direction(const glm::vec3 p_direction);
    glm::vec3 get_direction() const;
    static void CreatePrimCircleLuaBindings(std::shared_ptr<sol::state> lua);
};


#endif