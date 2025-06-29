#include "prim_triangle.h"


prim_triangle::prim_triangle()
{
    radius = 0.01;
    direction=Vector3(0,1,0);
    my_type = collider_enums::collider_primitive_type::circle;
}

prim_triangle::~prim_triangle()
{

}

Vector3 prim_triangle::find_furthest(const Vector3 &dir)
{
    return (dir - (dir.dot(direction)*direction)).normalized()*radius;
}



void prim_triangle::set_radius(const float p_radius)
{
    
    radius = std::max<float>(p_radius,0.01);
}

float prim_triangle::get_radius() const
{
    return radius;
}

void prim_triangle::set_direction(const Vector3 p_direction)
{
    if(p_direction.dot(p_direction)==0.0)
        set_direction(Vector3(0,1,0));
    direction=p_direction.normalized();
}

Vector3 prim_triangle::get_direction() const
{
    return direction;
}