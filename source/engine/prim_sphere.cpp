#include "ginkgopch.h"
#include "prim_sphere.h"
prim_sphere::prim_sphere(float rradius): radius(rradius)
{
    //set_radius(0.01);
    my_type = collider_enums::collider_primitive_type::sphere;
}
prim_sphere::~prim_sphere()
{
}
void prim_sphere::set_radius(const float p_radius)
{
    radius = p_radius;
}

float prim_sphere::get_radius() const
{
    return radius;
}

glm::vec3 prim_sphere::find_furthest(const glm::vec3&dir)
{
    return dir*radius;
}

void prim_sphere::CreatePrimSphereLuaBindings(std::shared_ptr<sol::state> lua) {
    lua->new_usertype<prim_sphere>("prim_sphere",
        sol::constructors<prim_sphere(float rradius)>(),
        sol::base_classes, sol::bases<collider_primitive>(),
        "radius", &prim_sphere::radius,
        "find_furthest", &prim_sphere::find_furthest,
        "set_radius", &prim_sphere::set_radius,
        "get_radius", &prim_sphere::get_radius
     );
}