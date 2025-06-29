
#define GLM_ENABLE_EXPERIMENTAL
#ifndef COLLIDER_H
#define COLLIDER_H
#include <vector>
#include "collider_primitive.h"
#include "transformcomponent.h"
#define GLM_ENABLE_EXPEREMENTAL
#include    <glm/gtx/orthonormalize.hpp>
#include    <glm/gtx/matrix_cross_product.hpp>
#include    <glm/gtx/euler_angles.hpp>
#include <glm/ext/matrix_transform.hpp>
#include "prim_circle.h"
#include "prim_sphere.h"
#include "prim_line.h"
#include "RefWrapper.h"
    enum surface_types: char
    {
        none,
        ground,
        player
    };
    // we need to create something to wrap around all of transform
    //since do we really need a collider for it to be affected by transforms UGH.
    struct exists_in_threespace
    {
        glm::mat4x4 transform;
        exists_in_threespace();
    };
    class collider :public TransformComponent
    {
        //needs position
        //needs scale
        //needs matrix.
        //possibly make this inherit from an object thats position related.
        void internal_update_inertia();
        void internal_update_inertia(collider_primitive&);
        void internal_update_inertia(collider_primitive&, collider_primitive&);
        void internal_update_inertia(collider_primitive&, collider_primitive&, collider_primitive&);
        void internal_update_inertia(collider_primitive&, collider_primitive&, collider_primitive&, collider_primitive&);
        //[TODO] organize this
        public:
            collider();
            collider(TransformComponent& in);
            surface_types my_surface_type = surface_types::none;
            ~collider();
            void update_inertia();
            glm::vec3 find_furthest(const glm::vec3& dir);
            std::vector<Ref<collider_primitive>> axes; // figure out memory layout.
            TransformComponent transformRef;
            //this needs to be sorted out later
            float restitution = 1.0f;
            float static_friction = 0.0f;
            float kinetic_friction = 0.0f;
            float mass = 1.0f;
            float mass_inv = 1.0f;
            glm::vec3 velocity = glm::vec3();
            glm::vec3 acceleration = glm::vec3();
            //these rotations are about the world axis.
            glm::vec3 angularVelocity = glm::vec3();
            //[TODO] IMPULSES;
            glm::mat3x3 invInertia=glm::mat3x3(1.0);
            // this is based off world space using the axis angle
            //matrix of inertia (in the form of a basis) invInertia
            bool is_colliding=false;
            bool is_affected_by_gravity=false;
            bool is_rotatable = true;
            static void CreateColliderBindings(std::shared_ptr<sol::state> lua);


            //GETTERS AND SETTERS
            void set_rotatable(const bool& rotatable);
            bool get_rotatable();
            void set_mass(const float& ms);
            float get_mass()const;
            void set_friction_s(float stat);
            void set_friction_d(float dyna);
            void set_frictions(float stat, float dynamic);
            float get_inv_mass()const;
            glm::vec3 get_velocity()const;
            void set_velocity(const glm::vec3& vel);
            void set_velocity_on_axis(const glm::vec3& vel , const glm::vec3& axis_restraint = glm::vec3(0.5f,0, 0.5f));
            glm::vec3 get_acceleration()const;
            void set_acceleration(const glm::vec3& accel);
            void set_principleAxisRotationVelocity(const glm::vec3 & p_principleAxisRotationVelocity);
            glm::vec3 get_principleAxisRotationVelocity() const;
            void apply_AngularVelocity(float dt);
            glm::vec3 get_global_position() ;
            void set_global_position(const glm::vec3& pos);
            glm::vec3 get_position() ;
            void set_position(const glm::vec3& pos);
            void set_restitution(const float& res);
            //void set_Transform(const TransformComponent& in);
            float get_restitution()const;
            //TECHNICALLY THIS IS THE INVERSE INERTIA TENSOR
            glm::mat3x3 get_my_basis();
            //void SetRotation(glm::vec3 _rotation) override;
            //glm::mat4 getMatrix()override;
            //[TODO] FIGURE OUT THE INERTIA TENSORS FOR GENERALIZED SHAPES
    private:
            void internal_line_inertia();
            void internal_circle_inertia();
            void internal_sphere_inertia();
            void internal_line_inertia(collider_primitive&);
            void internal_line_line_inertia(prim_line&, prim_line&);
            void internal_line_line_line_inertia(prim_line&, prim_line&,prim_line&);
            void internal_line_circle_inertia(prim_line&, prim_circle&);
            void internal_line_sphere_inertia(prim_line&, prim_sphere&);
            void internal_circle_sphere_inertia(prim_circle&,prim_sphere&);
            void internal_circle_inertia(collider_primitive&);
            void internal_sphere_inertia(collider_primitive&);

    };
    //maybe i should split it out into physics objects and colliders huh.
    class player_collider: public collider
    {
        public:
            glm::vec3 normal_collision = glm::vec3{ 0,1,0 };
            
            //[TODO] IMPULSES;
            // this is based off world space using the axis angle
            //matrix of inertia (in the form of a basis) invInertia
    };
#endif