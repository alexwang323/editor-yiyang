#ifndef PRIM_ELLIPSE_H
#define PRIM_ELLIPSE_H
#include "../collider_primitive.h"
//[TODO] add the vector library we are using
class prim_ellipse: public collider_primitive
{
    public:
        prim_ellipse();
        ~prim_ellipse();
        glm::vec3 direction;
        //hmmmmmmmm
        float radius_x;
        float radius_z;
        
        float get_radius_z() const;

        glm::vec3 find_furthest(const glm::vec3& dir) override;
        void set_radius_x(const float p_radius);
        float get_radius_x() const;
        void set_radius_z(const float p_radius);
};


#endif
