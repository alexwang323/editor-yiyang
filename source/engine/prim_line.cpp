#include "ginkgopch.h"
#include "prim_line.h"
prim_line::prim_line(glm::vec3 dir, float rad)
{
    radius = rad;
    set_direction(dir);
    my_type = collider_enums::collider_primitive_type::line;
}

prim_line::~prim_line()
{

}

glm::vec3 prim_line::find_furthest(const glm::vec3&dir)
{
    if(signbit(glm::dot(dir, direction)))
    {
        return (-radius)*direction;
    }
    else
    {
        return radius*direction;
    }
}

void prim_line::set_radius(const float p_radius)
{
    radius = p_radius;
}

float prim_line::get_radius() const
{
    return radius;
}

void prim_line::set_direction(const glm::vec3 p_direction)
{
    if (glm::dot(p_direction, p_direction) == 0.0)
        set_direction(glm::vec3(0, 1, 0));
    direction = glm::normalize(p_direction);
}

glm::vec3 prim_line::get_direction() const
{
    return direction;
}

prim_half_line::prim_half_line(glm::vec3 dir, float rad):prim_line(dir,rad)
{
}

prim_half_line::~prim_half_line()
{
}

glm::vec3 prim_half_line::find_furthest(const glm::vec3& dir)
{
    if (signbit(glm::dot(dir, direction)))
    {
        return glm::vec3{ 0,0,0 };
    }
    else
    {
        return radius * direction;
    }
}

void prim_line::CreatePrimLineLuaBindings(std::shared_ptr<sol::state> lua) {
    lua->new_usertype<prim_line>("prim_line",
        sol::constructors<prim_line(glm::vec3 dir, float rad)>(),
        sol::base_classes, sol::bases<collider_primitive>(),
        "direction", &prim_line::direction,
        "radius", &prim_line::radius,
        "find_furthest", &prim_line::find_furthest, 
        "set_radius", &prim_line::set_radius,
        "set_direction", &prim_line::set_direction,
        "get_direction", &prim_line::get_direction
        );
}

void prim_half_line::CreatePrimHalfLineBindings(std::shared_ptr<sol::state> lua) {
    lua->new_usertype<prim_half_line>("prim_half_line",
        sol::constructors<prim_half_line(glm::vec3 dir, float rad)>(),
        sol::base_classes, sol::bases<prim_line>(),
        "direction", &prim_half_line::direction,
        "radius", &prim_half_line::radius,
        "find_furthest", &prim_half_line::find_furthest
        );
}
