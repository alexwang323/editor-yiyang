#include "ginkgopch.h"
#include "collider.h"
#include <iostream>
void collider::internal_update_inertia()
{
    //check which situation is which.
    //inertia 
    //this is for 
    //need to get radius
    //collision detection by line sphere blah blah
    // update lines?
    switch (axes.size())
    {
    case 1:
        internal_update_inertia(axes[0].get());
        break;
    case 2:
        internal_update_inertia(axes[0].get(), axes[1].get());
        break;
    case 3:
        internal_update_inertia(axes[0].get(), axes[1].get(), axes[2].get());
        break;
    case 4:
        internal_update_inertia(axes[0].get(), axes[1].get(), axes[2].get(), axes[3].get());
        break;
    default:
        //get size.
        invInertia = glm::mat3x3(5.0 / 2.0 * mass_inv, 0, 0, 0, 5.0 / 2.0 * mass_inv, 0, 0, 0, 5.0 / 2.0 * mass_inv);
        break;
    }
    /*line line
    * look into this
    * but for a cube its
    //assume around y.
    auto lsq= tmp0->radius *2* tmp0->radius *2 ;
    auto lsq1= tmp1->radius *2* tmp1->radius *2 ;
    //lsq+=lsq1;
    invInertia=Basis(12.0*mass_inv/lsq,0,0,0,12.0*mass_inv/(lsq+lsq1),0,0,0,12.0*mass_inv/lsq1);
    */
    /* circle with line
    auto lsq= tmp0->radius;
    auto lsq1= tmp1->radius*2.0;
    lsq*=lsq;
    lsq1*=lsq1;
    lsq1 += 3*lsq;
    invInertia=Basis(12.0*mass_inv/lsq1,0,0,0,2.0*mass_inv/(lsq),0,0,0,12.0*mass_inv/lsq1);
    */
    /*
    * capsule
    auto lsq= tmp0->radius;
    auto lsq1= tmp1->radius*2.0;
    auto base = (4.0*lsq/3.0)+lsq1;
    auto masssph = mass*4.0/3.0*lsq/base;
    auto masscyl = mass*lsq1/base;
    auto h_sq = lsq1*lsq1;
    auto r_sq = lsq*lsq;
    auto h_r = lsq*lsq1;


    auto x_comp = masscyl*(h_sq/12.0 + r_sq/4.0)+masssph*(h_sq/2.0+0.4*r_sq+0.375*h_r);
    auto y_comp = masscyl*(r_sq/2.0)+masssph*(0.4*r_sq);
    auto z_comp = masscyl*(h_sq/12.0 + r_sq/4.0)+masssph*(h_sq/2.0+0.4*r_sq+0.375*h_r);
    invInertia=Basis(1.0/x_comp,0,0,0,1.0/y_comp,0,0,0,1.0/z_comp);
    */
    /*
    RHOMBUS
        auto lsq= tmp0->radius *4.0* tmp0->radius;
    auto lsq1= tmp1->radius *4.0* tmp1->radius;
    auto lsq2= tmp2->radius *4.0* tmp2->radius;
    auto l0 = lsq1+lsq2;
    auto l1 = lsq+lsq2;
    auto l2 = lsq+lsq1;
    invInertia=Basis(12.0*mass_inv/l0,0,0,0,12.0*mass_inv/(l1),0,0,0,12.0*mass_inv/l2);
    */
    //invInertia = glm::mat3x3(5.0 / 2.0 * mass_inv, 0, 0, 0, 5.0 / 2.0 * mass_inv, 0, 0, 0, 5.0 / 2.0 * mass_inv);
}

void collider::internal_update_inertia(collider_primitive& a)
{
    switch (a.my_type)
    {
    case collider_enums::collider_primitive_type::none:
        invInertia = glm::mat3x3(5.0 / 2.0 * mass_inv, 0, 0, 0, 5.0 / 2.0 * mass_inv, 0, 0, 0, 5.0 / 2.0 * mass_inv);
        break;
    case collider_enums::collider_primitive_type::line:
        internal_line_inertia(a);
        break;
    case collider_enums::collider_primitive_type::circle:
        //YOU KNOW THIS ONE.
        internal_circle_inertia(a);
        break;
    case collider_enums::collider_primitive_type::sphere:
        internal_sphere_inertia(a);
        break;
    default:
        invInertia = glm::mat3x3(5.0 / 2.0 * mass_inv, 0, 0, 0, 5.0 / 2.0 * mass_inv, 0, 0, 0, 5.0 / 2.0 * mass_inv);
        break;
    }
}

void collider::internal_update_inertia(collider_primitive& a, collider_primitive& b)
{
    switch (a.my_type)
    {
    case collider_enums::collider_primitive_type::none:
        internal_update_inertia(b);
        break;
    case collider_enums::collider_primitive_type::line:
        switch (b.my_type)
        {
        case collider_enums::collider_primitive_type::none:
            internal_line_inertia(a);
            break;
        case collider_enums::collider_primitive_type::line:
            internal_line_line_inertia(dynamic_cast<prim_line&>(a), dynamic_cast<prim_line&>(b));
            break;
        case collider_enums::collider_primitive_type::circle:
            //YOU KNOW THIS ONE.
            internal_line_circle_inertia(dynamic_cast<prim_line&>(a), dynamic_cast<prim_circle&>(b));
            break;
        case collider_enums::collider_primitive_type::sphere:
            internal_line_sphere_inertia(dynamic_cast<prim_line&>(a), dynamic_cast<prim_sphere&>(b));
            break;
        default:
            invInertia = glm::mat3x3(5.0 / 2.0 * mass_inv, 0, 0, 0, 5.0 / 2.0 * mass_inv, 0, 0, 0, 5.0 / 2.0 * mass_inv);
            break;
        }
        break;
    case collider_enums::collider_primitive_type::circle:
        switch (b.my_type)
        {
        case collider_enums::collider_primitive_type::none:
            internal_circle_inertia(a);
            break;
        case collider_enums::collider_primitive_type::line:
            internal_line_circle_inertia(dynamic_cast<prim_line&>(b), dynamic_cast<prim_circle&>(a));
            break;
        case collider_enums::collider_primitive_type::circle:
            //this seems wrong
            internal_line_circle_inertia(dynamic_cast<prim_line&>(a), dynamic_cast<prim_circle&>(b));
            break;
        case collider_enums::collider_primitive_type::sphere:
            //use cylinder but modified.
            //internal_line_sphere_inertia((prim_line*)a, (prim_sphere*)b);
            break;
        default:
            invInertia = glm::mat3x3(5.0 / 2.0 * mass_inv, 0, 0, 0, 5.0 / 2.0 * mass_inv, 0, 0, 0, 5.0 / 2.0 * mass_inv);
            break;
        }
        break;
    case collider_enums::collider_primitive_type::sphere:
        switch (b.my_type)
        {
        case collider_enums::collider_primitive_type::none:
            internal_sphere_inertia(a);
            break;
        case collider_enums::collider_primitive_type::line:
            //pill
            internal_line_sphere_inertia(dynamic_cast<prim_line&>(b), dynamic_cast<prim_sphere&>(a));
            break;
        case collider_enums::collider_primitive_type::circle:
            //this seems wrong
            //change to cylinder
            break;
        case collider_enums::collider_primitive_type::sphere:
            //this is wrong
            ///illegal combination
            break;
        default:
            invInertia = glm::mat3x3(5.0 / 2.0 * mass_inv, 0, 0, 0, 5.0 / 2.0 * mass_inv, 0, 0, 0, 5.0 / 2.0 * mass_inv);
            break;
        }
        break;
    default:
        invInertia = glm::mat3x3(5.0 / 2.0 * mass_inv, 0, 0, 0, 5.0 / 2.0 * mass_inv, 0, 0, 0, 5.0 / 2.0 * mass_inv);
        break;
    }
}

void collider::internal_update_inertia(collider_primitive& a, collider_primitive& b, collider_primitive& c)
{
    switch (a.my_type)
    {
    case collider_enums::collider_primitive_type::none:
        internal_update_inertia(b, c);
        break;
    case collider_enums::collider_primitive_type::line:
        switch (b.my_type)
        {
        case collider_enums::collider_primitive_type::none:
            internal_update_inertia(a, c);
            break;
        case collider_enums::collider_primitive_type::line:
            //do line line line here
            //internal_line_line_inertia((prim_line*)a, (prim_line*)b);
        {
            switch (c.my_type)
            {
            case collider_enums::collider_primitive_type::line:
                internal_line_line_line_inertia(dynamic_cast<prim_line&>(a), dynamic_cast<prim_line&>(b), dynamic_cast<prim_line&>(c));
                break;
            default:
                break;
            };
        }
        break;
        case collider_enums::collider_primitive_type::circle:
            //YOU KNOW THIS ONE.
            //circle line 
            internal_line_circle_inertia(dynamic_cast<prim_line&>(a), dynamic_cast<prim_circle&>(b));
            break;
        case collider_enums::collider_primitive_type::sphere:
            internal_line_sphere_inertia(dynamic_cast<prim_line&>(a), dynamic_cast<prim_sphere&>(b));
            break;
        default:
            invInertia = glm::mat3x3(5.0 / 2.0 * mass_inv, 0, 0, 0, 5.0 / 2.0 * mass_inv, 0, 0, 0, 5.0 / 2.0 * mass_inv);
            break;
        }
        break;
    case collider_enums::collider_primitive_type::circle:
        switch (b.my_type)
        {
        case collider_enums::collider_primitive_type::none:
            internal_circle_inertia(a);
            break;
        case collider_enums::collider_primitive_type::line:
            internal_line_circle_inertia(dynamic_cast<prim_line&>(b), dynamic_cast<prim_circle&>(a));
            break;
        case collider_enums::collider_primitive_type::circle:
            //this seems wrong
            internal_line_circle_inertia(dynamic_cast<prim_line&>(a), dynamic_cast<prim_circle&>(b));
            break;
        case collider_enums::collider_primitive_type::sphere:
            internal_line_sphere_inertia(dynamic_cast<prim_line&>(a), dynamic_cast<prim_sphere&>(b));
            break;
        case collider_enums::collider_primitive_type::ellipse:
            //[TODO]
            break;
        case collider_enums::collider_primitive_type::triangle:
            //[TODO]
            break;
        case collider_enums::collider_primitive_type::cone:
            //[TODO]
            break;
        case collider_enums::collider_primitive_type::tetrahedron:
            break;
        default:
            invInertia = glm::mat3x3(5.0 / 2.0 * mass_inv, 0, 0, 0, 5.0 / 2.0 * mass_inv, 0, 0, 0, 5.0 / 2.0 * mass_inv);
            break;
        }
        break;
    case collider_enums::collider_primitive_type::sphere:
        switch (b.my_type)
        {
        case collider_enums::collider_primitive_type::none:
            internal_sphere_inertia(a);
            break;
        case collider_enums::collider_primitive_type::line:
            //pill
            break;
        case collider_enums::collider_primitive_type::circle:
            //this seems wrong
            //change to cylinder
            break;
        case collider_enums::collider_primitive_type::sphere:
            //this is wrong
            break;
        case collider_enums::collider_primitive_type::ellipse:
            //[TODO]
            break;
        case collider_enums::collider_primitive_type::triangle:
            //[TODO]
            break;
        case collider_enums::collider_primitive_type::cone:
            //[TODO]
            break;
        case collider_enums::collider_primitive_type::tetrahedron:
            break;
        default:
            invInertia = glm::mat3x3(5.0 / 2.0 * mass_inv, 0, 0, 0, 5.0 / 2.0 * mass_inv, 0, 0, 0, 5.0 / 2.0 * mass_inv);
            break;
        }
        break;
    default:
        invInertia = glm::mat3x3(5.0 / 2.0 * mass_inv, 0, 0, 0, 5.0 / 2.0 * mass_inv, 0, 0, 0, 5.0 / 2.0 * mass_inv);
        break;
    }
    //invInertia = glm::mat3x3(5.0 / 2.0 * mass_inv, 0, 0, 0, 5.0 / 2.0 * mass_inv, 0, 0, 0, 5.0 / 2.0 * mass_inv);
}

void collider::internal_update_inertia(collider_primitive& a, collider_primitive& b, collider_primitive& c, collider_primitive& d)
{
    invInertia = glm::mat3x3(5.0 / 2.0 * mass_inv, 0, 0, 0, 5.0 / 2.0 * mass_inv, 0, 0, 0, 5.0 / 2.0 * mass_inv);
}

collider::collider()
{

}

collider::collider(TransformComponent& in) :TransformComponent(in)
{
    std::cout << "collider with transform " << in.GetPosition().x <<
        ", " << in.GetPosition().y << ", " << in.GetPosition().z << std::endl;
    std::cout << "address of collider: " << &transformRef << std::endl;
}


collider::~collider()
{
}

void collider::update_inertia()
{
    set_mass(mass);
    //auto tmp = this->GetScale();
    //auto tmpscalar = std::max(std::max(tmp.x, tmp.y), tmp.z);
    //auto lsq = tmpscalar* tmpscalar;
    //invInertia = glm::mat3x3(5.0 / 2.0 * mass_inv / lsq, 0, 0, 0, 5.0 / 2.0 * mass_inv / lsq, 0, 0, 0, 5.0 / 2.0 * mass_inv / lsq);
    //internal_sphere_inertia();
    internal_update_inertia();
}

glm::vec3 collider::find_furthest(const glm::vec3& dir)
{
    /*
    auto tfm = get_global_transform();
    Vector3 tempdir = xform(tfm.inverse(),dir);
    Vector3 tmp = Vector3();
    for(auto each:axes)
    {
        tmp+=each->find_furthest(tempdir);
    }
    return xform(tfm,tmp)+get_global_position();
    */
    auto mt = getMatrix();
    glm::vec4 tmpdir = glm::inverse(mt) * glm::vec4(dir, 0.0f);

    glm::vec3 dir_search = glm::normalize(glm::vec3(tmpdir.x, tmpdir.y, tmpdir.z));
    glm::vec3 tmp{ 0.0f,0.0f,0.0f };
    for (auto each : axes)
    {
        tmp += each.get().find_furthest(dir_search);
    }
    return glm::vec3(mt * glm::vec4(tmp, 1.0f));
}
void collider::set_rotatable(const bool& rotatable)
{
    is_rotatable = rotatable;
}
bool collider::get_rotatable()
{
    return is_rotatable;
}
void collider::set_mass(const float& ms)
{
    mass = ms;
    if (mass != 0)
        mass_inv = 1.0f / mass;
    else
        mass_inv = INFINITY;
    //recalculate the momentum
}
float collider::get_mass() const
{
    return mass;
}
void collider::set_friction_s(float stat)
{
    static_friction = stat;
};
void collider::set_friction_d(float dyna)
{
    kinetic_friction = dyna;
};
void collider::set_frictions(float stat, float dynamic)
{
    set_friction_s(stat); set_friction_d(dynamic);
};
float collider::get_inv_mass() const
{
    if (!is_affected_by_gravity)
    {
        return 0.0f;
    }
    return mass_inv;
}
glm::vec3 collider::get_velocity() const
{
    return velocity;
}
void collider::set_velocity(const glm::vec3& vel)
{
    velocity = vel;
}
void collider::set_velocity_on_axis(const glm::vec3& vel, const glm::vec3& axis_restraint)
{

    //auto axtest = glm::normalize(vel);
    //std::cout << "hi";
    velocity = vel - glm::dot(vel, axis_restraint) * axis_restraint;
}
glm::vec3 collider::get_acceleration() const
{
    return acceleration;
}
void collider::set_acceleration(const glm::vec3& accel)
{
    acceleration = accel;
}
void collider::set_principleAxisRotationVelocity(const glm::vec3& p_principleAxisRotationVelocity)
{
    angularVelocity = p_principleAxisRotationVelocity;
}
glm::vec3 collider::get_principleAxisRotationVelocity() const
{
    if (is_affected_by_gravity && is_rotatable)
    {
        return angularVelocity;
    }
    else
    {
        return glm::vec3(0, 0, 0);
    }

}
void collider::apply_AngularVelocity(float dt)
{
    auto tmp = dt * glm::matrixCross4(angularVelocity);
    glm::mat4 scaleMat = glm::scale(glm::mat4(1.0), scale);
    //std::cout << angularVelocity.x << "," << angularVelocity.y << "," << angularVelocity.z << "," << std::endl;
    rotation_internal = rotation_internal + tmp * rotation_internal;
    //;

    rotation_internal = glm::orthonormalize(glm::mat3(rotation_internal));
    rotation = glm::eulerAngles(glm::toQuat(rotation_internal));
    //std::cout << glm::determinant(rotation_internal) << "," << std::endl;
    glm::mat4 translateMat = glm::translate(glm::mat4(1.0f), position);

    matrix = translateMat * rotation_internal * scaleMat;
    needToUpdate = false;
    //;

//account for rotas
}
//[TODO] UPDATE THIS
glm::vec3 collider::get_global_position()
{

    return glm::vec3(getMatrix() * glm::vec4(0, 0, 0, 1.0f));
}
void collider::set_global_position(const glm::vec3& pos)
{
    SetPosition(pos);
    //  transform = glm::translate(glm::mat4x4(glm::mat3x3(transform)), pos);

}
//[TODO] UPDATE THIS
glm::vec3 collider::get_position()
{
    return get_global_position();
}

void collider::set_position(const glm::vec3& pos)
{
    set_global_position(pos);
}

void collider::set_restitution(const float& res)
{
    restitution = res;
}

//void collider::set_Transform(const TransformComponent& in)
//{
//
//}

float collider::get_restitution() const
{
    return restitution;
}

glm::mat3x3 collider::get_my_basis()
{
    //glm::orthonormalize
    auto tmp = glm::orthonormalize(glm::mat3x3(getMatrix()));
    //scale unaccounted for 
    //invInertia = glm::mat3x3(5.0 / 2.0 * mass_inv , 0, 0, 0, 5.0 / 2.0 * mass_inv, 0, 0, 0, 5.0 / 2.0 * mass_inv );
    // A B A^T
    // A B_INV A^T
    return tmp * invInertia * glm::transpose(tmp);
}

void collider::internal_line_inertia()
{
    prim_line& tmp0 = dynamic_cast<prim_line&>(axes[0].get());
    auto lsq = tmp0.radius * 2.0;
    lsq *= lsq;
    //create cordinate system based off line?
    invInertia = glm::mat3x3(12.0 * mass_inv / lsq, 0, 0,
        0, 0, 0,
        0, 0, 12.0 * mass_inv / lsq);
}

void collider::internal_circle_inertia()
{
    prim_circle& axe = dynamic_cast<prim_circle&>(axes[0].get());
    auto lsq = axe.radius * axe.radius;
    invInertia = glm::mat3x3(4.0 * mass_inv / lsq, 0, 0, 0, 2.0 * mass_inv / (lsq), 0, 0, 0, 4.0 * mass_inv / lsq);
}

void collider::internal_sphere_inertia()
{
    prim_sphere& axe = dynamic_cast<prim_sphere&>(axes[0].get());
    //account for scale
    auto lsq = axe.radius * axe.radius;
    invInertia = glm::mat3x3(5.0 / 2.0 * mass_inv / lsq, 0, 0, 0, 5.0 / 2.0 * mass_inv / lsq, 0, 0, 0, 5.0 / 2.0 * mass_inv / lsq);
}

void collider::internal_line_inertia(collider_primitive& a)
{
    prim_line& tmp0 = dynamic_cast<prim_line&>(a);
    auto lsq = tmp0.radius * 2.0;
    lsq *= lsq;
    invInertia = glm::mat3x3(12.0 * mass_inv / lsq, 0, 0,
        0, 0, 0,
        0, 0, 12.0 * mass_inv / lsq);
}

void collider::internal_line_line_inertia(prim_line& a, prim_line& b)
{
    auto lsq = a.radius * 2.0 * a.radius * 2.0;
    auto lsq1 = b.radius * 2.0 * b.radius * 2.0;
    //lsq+=lsq1;
    invInertia = glm::mat3x3(12.0 * mass_inv / lsq, 0, 0, 0, 12.0 * mass_inv / (lsq + lsq1), 0, 0, 0, 12.0 * mass_inv / lsq1);
}

void collider::internal_line_line_line_inertia(prim_line& tmp0, prim_line& tmp1, prim_line& tmp2)
{
    /*
   RHOMBUS
   */
    auto lsq = tmp0.radius * 4.0 * tmp0.radius;
    auto lsq1 = tmp1.radius * 4.0 * tmp1.radius;
    auto lsq2 = tmp2.radius * 4.0 * tmp2.radius;
    auto l0 = lsq1 + lsq2;
    auto l1 = lsq + lsq2;
    auto l2 = lsq + lsq1;
    invInertia = glm::mat3x3(12.0 * mass_inv / l0, 0, 0, 0, 12.0 * mass_inv / (l1), 0, 0, 0, 12.0 * mass_inv / l2);

}

void collider::internal_line_circle_inertia(prim_line& a, prim_circle& b)
{
    //need to account for nonstandard.
    auto lsq = b.radius;
    auto lsq1 = a.radius * 2.0;
    lsq *= lsq;
    lsq1 *= lsq1;
    lsq1 += 3 * lsq;
    invInertia = glm::mat3x3(12.0 * mass_inv / lsq1, 0, 0, 0, 2.0 * mass_inv / (lsq), 0, 0, 0, 12.0 * mass_inv / lsq1);
}

void collider::internal_line_sphere_inertia(prim_line& tmp1, prim_sphere& tmp0)
{
    //need to account for nonstandard.
    auto lsq = tmp0.radius;
    auto lsq1 = tmp1.radius * 2.0;
    auto base = (4.0 * lsq / 3.0) + lsq1;
    auto masssph = mass * 4.0 / 3.0 * lsq / base;
    auto masscyl = mass * lsq1 / base;
    auto h_sq = lsq1 * lsq1;
    auto r_sq = lsq * lsq;
    auto h_r = lsq * lsq1;


    auto x_comp = masscyl * (h_sq / 12.0 + r_sq / 4.0) + masssph * (h_sq / 2.0 + 0.4 * r_sq + 0.375 * h_r);
    auto y_comp = masscyl * (r_sq / 2.0) + masssph * (0.4 * r_sq);
    auto z_comp = masscyl * (h_sq / 12.0 + r_sq / 4.0) + masssph * (h_sq / 2.0 + 0.4 * r_sq + 0.375 * h_r);
    invInertia = glm::mat3x3(1.0 / x_comp, 0, 0, 0, 1.0 / y_comp, 0, 0, 0, 1.0 / z_comp);
}

void collider::internal_circle_sphere_inertia(prim_circle& a, prim_sphere& b)
{
    //account for direction.
    auto lsq = a.radius + b.radius;
    auto lsq1 = b.radius * 2.0;
    lsq *= lsq;
    lsq1 *= lsq1;
    lsq1 += 3 * lsq;
    invInertia = glm::mat3x3(12.0 * mass_inv / lsq1, 0, 0, 0, 2.0 * mass_inv / (lsq), 0, 0, 0, 12.0 * mass_inv / lsq1);
}

void collider::internal_circle_inertia(collider_primitive& a)
{
    prim_circle& axe = dynamic_cast<prim_circle&>(a);
    //colliders assume uniform scale.
    auto tmp = this->GetScale();
    auto tmpscalar = std::max(std::max(tmp.x, tmp.y), tmp.z);
    //auto lsq = ;
    auto lsq = tmpscalar * tmpscalar * axe.radius * axe.radius;
    invInertia = glm::mat3x3(4.0 * mass_inv / lsq, 0, 0, 0, 2.0 * mass_inv / (lsq), 0, 0, 0, 4.0 * mass_inv / lsq);
}

void collider::internal_sphere_inertia(collider_primitive& a)
{
    prim_sphere& axe = dynamic_cast<prim_sphere&>(a);
    auto sca = this->GetScale();
    auto dominantScale = std::max(std::max(sca.x, sca.y), sca.z);
    auto lsq = dominantScale * dominantScale * axe.radius * axe.radius;
    invInertia = glm::mat3x3(5.0 / 2.0 * mass_inv / lsq, 0, 0, 0, 5.0 / 2.0 * mass_inv / lsq, 0, 0, 0, 5.0 / 2.0 * mass_inv / lsq);
}


exists_in_threespace::exists_in_threespace() :transform(1.0f)
{
}

void collider::CreateColliderBindings(std::shared_ptr<sol::state> lua) {

    lua->new_enum("surface_types",
        "none", surface_types::none,
        "ground", surface_types::ground,
        "player", surface_types::player);

    Ref<collider_primitive>::CreateRefWrapperLuaBindings(lua);

    // bind std::vector<Ref<collider_primitive>> to lua
    lua->new_usertype<std::vector<Ref<collider_primitive>>>("collider_primitives_vec",
        "emplace_back", [](std::vector<Ref<collider_primitive>>& v, collider_primitive& value) {
            v.emplace_back(Ref<collider_primitive>(value));
            std::cout << "ref collider primitive vector emplace_back called" << std::endl;
        },
        "push_back", [](std::vector<Ref<collider_primitive>>& v, collider_primitive& value) {
            v.push_back(Ref<collider_primitive>(value)); }
    );

    lua->new_usertype<collider>("collider",
        sol::constructors<collider(),
        collider(TransformComponent & in)>(),
        sol::base_classes, sol::bases<TransformComponent>(),
        "my_surface_type", &collider::my_surface_type,
        "update_inertia", &collider::update_inertia,
        "find_furthest", &collider::find_furthest,
        "axes", &collider::axes,
        "transformRef", &collider::transformRef,
        "restitution", &collider::restitution,
        "static_friction", &collider::static_friction,
        "kinetic_friction", &collider::kinetic_friction,
        "mass", &collider::mass,
        "mass_inv", &collider::mass_inv,
        "isColliding", &collider::is_colliding,
        "velocity", &collider::velocity,
        "acceleration", &collider::acceleration,
        "angularVelocity", &collider::angularVelocity,
        "is_affected_by_gravity", &collider::is_affected_by_gravity,
        "is_rotatable", &collider::is_rotatable,
        "set_restitution", &collider::set_restitution,
        "set_mass", &collider::set_mass
    );
}