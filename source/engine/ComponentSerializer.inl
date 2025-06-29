#include "ComponentSerializer.h"

// Helper template to make the static_assert dependent on the type
template<class>
inline constexpr bool always_false = false;

template <typename TComponent, typename TSerializer>
inline void ComponentSerializer::Serialize(TSerializer& serializer, TComponent& component)
{
	SerializeComponent(serializer, component);
}

/*
* Will return the component of the corresponding type
*/
template<typename TComponent, typename TTable>
inline auto ComponentSerializer::Deserialize(const TTable& table)
{
	if constexpr (std::is_same_v<TComponent, TransformComponent>) {
		return DeserializeTransform(table);
	}
	else if constexpr (std::is_same_v<TComponent, RenderComponent>) {
		return DeserializeRenderComp(table);
	}
	//TODO: check for other types of component
	else {
		static_assert(always_false<TComponent>, "Component type is invalid!");
	}
}
