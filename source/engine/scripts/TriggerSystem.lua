TriggerSystem = EventListener:Create()

function EventListener:Create()
	return CreateObject(TriggerSystem):new()
end

function EventListener:OnCollision(object_a, object_b)
	-- have a condition to determine which one is the player, 
	--which one is the triggered game entity, and determine which game entity it is
	self:OnInGameJoyStickTriggered(object_a, object_b)
	self:OnInGameUpButtonTriggered(object_a, object_b)
	self:OnInGameDownButtonTriggered(object_a, object_b)
	self:OnInGameLeftButtonTriggered(object_a, object_b)
	self:OnInGameRightButtonTriggered(object_a, object_b)
end

function EventListener:OnInGameJoyStickTriggered(trigger, player)
	-- trigger and player are supposed to be colliders
	-- try printing the surface types first
	--print(trigger.my_surface_type .. "has been triggered by " .. player.my_surface_type)
	print("OnInGameJoyStickTriggered called!")
end

function EventListener:OnInGameUpButtonTriggered(object_a, object_b)
end

function EventListener:OnInGameDownButtonTriggered(object_a, object_b)
end

function EventListener:OnInGameLeftButtonTriggered(object_a, object_b)
end

function EventListener:OnInGameRightButtonTriggered(object_a, object_b)
end