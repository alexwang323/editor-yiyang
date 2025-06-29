#ifndef PRIM_TRIANGLE_H
#define PRIM_TRIANGLE_H
#include "collider_primitive.h"
//[TODO] add the vector library we are using
class prim_triangle: public collider_primitive
{
    public:
    prim_triangle();
    ~prim_triangle();
    Vector3 direction;
    //this shall be defined as such triangle is facing direction, with three vertices 
    float radius;
    Vector3 find_furthest(const Vector3& dir) override;
    void set_radius(const float p_radius);
    float get_radius() const;
    void set_direction(const Vector3 p_direction);
    Vector3 get_direction() const;
};


#endif