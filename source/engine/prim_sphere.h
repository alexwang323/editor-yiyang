#ifndef PRIM_SPHERE_H
#define PRIM_SPHERE_H
#include "collider_primitive.h"
#include <sol/sol.hpp>
//[TODO] add the vector library we are using
class prim_sphere: public collider_primitive
{
    public:
        prim_sphere(float rradius =0.01f);
        ~prim_sphere();
        float radius;
        glm::vec3 find_furthest(const glm::vec3& dir) override;
        void set_radius(const float p_radius);
        float get_radius() const;
        static void CreatePrimSphereLuaBindings(std::shared_ptr<sol::state> lua);
};


#endif
