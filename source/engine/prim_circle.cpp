#include "ginkgopch.h"
#include "prim_circle.h"


prim_circle::prim_circle()
{
    radius = 0.5f;
    direction=glm::vec3(0,1,0);
    my_type = collider_enums::collider_primitive_type::circle;
}

prim_circle::~prim_circle()
{

}

glm::vec3 prim_circle::find_furthest(const glm::vec3 &dir)
{
    return (dir - (glm::dot(dir,direction)*direction))*radius;
}



void prim_circle::set_radius(const float p_radius)
{
    
    radius = std::max<float>(p_radius,0.01f);
}

float prim_circle::get_radius() const
{
    return radius;
}

void prim_circle::set_direction(const glm::vec3 p_direction)
{
    if(glm::dot(p_direction,p_direction)==0.0)
        set_direction(glm::vec3(0,1,0));
    direction=glm::normalize(p_direction);
}

glm::vec3 prim_circle::get_direction() const
{
    return direction;
}

void prim_circle::CreatePrimCircleLuaBindings(std::shared_ptr<sol::state> lua) {
    lua->new_usertype<prim_circle>("prim_circle",
        sol::constructors<prim_circle()>(),
        sol::base_classes, sol::bases<collider_primitive>(),
        "direction", &prim_circle::direction,
        "radius", &prim_circle::radius,
        "find_furthest", &prim_circle::find_furthest,
        "set_radius", &prim_circle::set_radius,
        "get_radius", &prim_circle::get_radius,
        "set_direction", &prim_circle::set_direction,
        "get_direction", &prim_circle::get_direction);
}