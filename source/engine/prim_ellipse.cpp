#include "prim_ellipse.h"
prim_ellipse::prim_ellipse()
{
    set_radius_x(0.01);
    set_radius_z(0.01);
    my_type = collider_enums::collider_primitive_type::ellipse;
}
prim_ellipse::~prim_ellipse()
{
}
void prim_ellipse::set_radius_x(const float p_radius)
{
    radius_x = p_radius;
}

float prim_ellipse::get_radius_x() const
{
    return radius_x;
}
void prim_ellipse::set_radius_z(const float p_radius)
{
    radius_z = p_radius;
}

float prim_ellipse::get_radius_z() const
{
    return radius_z;
}

glm::vec3 prim_ellipse::find_furthest(const glm::vec3&dir)
{
    //[TODO] UPDATE THIS
    //this needs to store its own coordinate system todo
    return dir;
}