CollisionEvent = Event:Create()  -- allows for inheritance

function CollisionEvent:Create()
	local object_a = nil
	local object_b = nil

	function self.Execute()
		for k,v in pairs(self.m_Listeners) do
			v.OnCollision(object_a, object_b)
		end
	end

	function self:EmitEvent(obj_a, obj_b)
		object_a = obj_a
		object_b = obj_b
		self:Execute()
	end

	-- creating its own type
	return CreateObject(CollisionEvent):new()
end