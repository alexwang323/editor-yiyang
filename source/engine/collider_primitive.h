#ifndef COLLIDER_PRIMITIVE_H
#define COLLIDER_PRIMITIVE_H
#include <glm/glm.hpp>
#include <sol/sol.hpp>
    namespace collider_enums
    {
        enum collider_primitive_type: char
        {
            none,
            line,
            circle,
            sphere,
            ellipse,
            triangle,
            cone,
            tetrahedron,
            
        };
    }
    //[TODO]inherit from base object type that exists in the engine
    // [TODO] PICK YOUR VECTOR3 NOTATION.
    // [TODO] parents need to pick up a message that this is being changed?
    class collider_primitive
    {
    public:
        collider_enums::collider_primitive_type my_type = collider_enums::collider_primitive_type::none;
        //literally thats all it has lol
        virtual glm::vec3 find_furthest(const glm::vec3& dir);
        collider_primitive();
        ~collider_primitive();
        //but this is for the editor only so is that even neccesary?
        static void CreateColliderPrimitiveLuaBind(std::shared_ptr<sol::state> lua);
        bool operator==(const collider_primitive& other) const {
            return this == &other;
        } // no actual use, just for the colider_primitive vector
    };
#endif