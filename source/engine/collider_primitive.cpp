#include "ginkgopch.h"
#include "collider_primitive.h"

glm::vec3 collider_primitive::find_furthest(const glm::vec3&dir)
{
    return glm::vec3();
}
collider_primitive::collider_primitive()
{
    
}
collider_primitive::~collider_primitive()
{
    
}
void collider_primitive::CreateColliderPrimitiveLuaBind(std::shared_ptr<sol::state> lua) {
    lua->new_enum("collider_primitive_type",
        "none", collider_enums::collider_primitive_type::none,
        "line", collider_enums::collider_primitive_type::line,
        "circle", collider_enums::collider_primitive_type::circle,
        "sphere", collider_enums::collider_primitive_type::sphere,
        "ellipse", collider_enums::collider_primitive_type::ellipse,
        "triangle", collider_enums::collider_primitive_type::triangle,
        "cone", collider_enums::collider_primitive_type::cone,
        "tetrahedron", collider_enums::collider_primitive_type::tetrahedron
    );

    lua->new_usertype<collider_primitive>("collider_primitive",
        sol::constructors<collider_primitive()>(),
        "my_type", &collider_primitive::my_type,
        "find_furthest", &collider_primitive::find_furthest
    );
}