GameState = {}
GameState.__index = GameState

function GameState:Create(stack)
    -- 'this' is the variable for the state
	-- 'this' is a table
	local this = 
	{
		m_Stack = stack
		-- will have extra stuff here
	}
	local state = State("game state")
	state:set_variable_table(this)
	state:set_on_enter( function() this:onEnter() end )
	state:set_on_exit( function() this:onExit() end )
	state:set_on_update( function(dt) this:onUpdate(dt) end )
	state:set_on_render( function() this:onRender() end )
	state:set_handle_inputs( function() this:HandleInputs() end )

	setmetatable(this, self)
	
	this:Initialize()

	return state
end

function GameState:Initialize()
    --- place to create the events and trigger system
	if not gCollisionEvent then
		gCollisionEvent = CollisionEvent:Create()
	end

	if not gTriggerSystem then
		gTriggerSystem = TriggerSystem:Create()
	end

	gCollisionEvent:AddListnersToEvent(gTriggerSystem)

	-- add the player render and physics component, as game objects
	local demoCubeTr = TransformComponent.new(
		vec3.new(-2.0, 1.0, -2.0),
		vec3.new(0.0),
		vec3.new(0.5)
	)
	local demoMaterial = Material.new(
		vec3.new(0.5, 1.0, 0.5),
		vec3.new(0.5),
		5.0
	)
	local demoCubeRender = RenderComponent.new()
	-- just pass subdivisions as 16 here
	demoCubeRender:Init(MESH_TYPE.MESH_TYPE_CUBE, 16)
	demoCubeRender:SetMaterial(demoMaterial)
	demoCubeRender:SetTransform(demoCubeTr)
	-- need to fix the game object id later 
	--should it be able to call GameObject here or let GOF do it
	GOF.nextID = GOF.nextID + 1
	local demoCube = GameObject.new(GOF.nextID, "demoCube")
	demoCube:SetParent("test parent")
	GOF:addGameObject(demoCube)
	demoCube:AddTransform(demoCubeTr)
	demoCube:AddRender(demoCubeRender)
	--scene:addToRenderComponents(demoCubeRender) -- don't do this in global scope!
	print(demoCubeRender:GetTransform().position.x)
	print(demoCubeRender.meshType)
	--print(demoCubeRender.id)
	print(demoCube.render.meshType)

	local jsonObj = GOF:makeGameObjectArchetype()

	local vPosition = SoundVec3.new(0.0,0.0,0.0)

	-- add cone to the scene
	local coneMat = Material.new(
		vec3.new(1.0, 0.5, 0.5),
		vec3.new(0.3),
		0.8
	)
	local coneTr = TransformComponent.new(
		vec3.new(-4.0, 0.0, 0.0),
		vec3.new(0.0),
		vec3.new(1.5)
	)
	coneRC = RenderComponent.new()
	coneRC:Init(MESH_TYPE.MESH_TYPE_CONE, 256)
	coneRC:SetMaterial(coneMat)
	coneRC:SetTransform(coneTr)
	scene:addToRenderComponents(coneRC)

	local sphereColliderTr = TransformComponent.new(
		vec3.new(0.0, 3.0, 0.0),
		vec3.new(0.0, 0.0, 15.0),
		vec3.new(1.0)
	)

	local sphereRC = RenderComponent:new()
	sphereRC:Init(MESH_TYPE.MESH_TYPE_SPHERE, 64)
	sphereRC:SetMaterial(demoMaterial)
	sphereRC:SetTransform(sphereColliderTr)

	-- scene2d
	local quadTr = TransformComponent.new(
		vec3.new(0.0,5.0,-10.0),
		vec3.new(0.0),
		vec3.new(5.0)
	)
	local quadMat = Material.new(
		vec3.new(1.0, 1.0, 1.0),
		vec3.new(0.0),
		0.1
	)
	quadMat:LoadExistingTexture(MATERIAL_BIT.ALBEDO_BIT, scene2D:GetOutputFramebuffer())
	quadRC = RenderComponent.new()
	quadRC:Init(MESH_TYPE.MESH_TYPE_QUAD,0) -- to match parameters list on c++ side
	quadRC:SetMaterial(quadMat)
	quadRC:SetTransform(quadTr)
	scene:addToRenderComponents(quadRC)

end

function GameState:onEnter()	
	print("game state onEnter called")
	-- temporary 2d components
	local tempMat = Material.new(
		vec3.new(1.0, 0.0, 1.0),
		vec3.new(0.5),
		1.0
	)
	local tempTr = TransformComponent.new(
		vec3.new(0.0),
		vec3.new(radians(45.0), 0.0 ,0.0),
		vec3.new(0.2)
	)
	tempRC = RenderComponent.new()
	tempRC:Init(MESH_TYPE.MESH_TYPE_QUAD, 0)
	tempRC:SetMaterial(tempMat)
	tempRC:SetTransform(tempTr)
	scene2D:addToRenderComponents(tempRC)
	--scene2D:SetRenderComponentPosition(0,vec3.new(0.0, 0.0, 5.0))	
end

function GameState:onExit()
    -- destroy all entities
end

function GameState:onUpdate(dt)
	-- update all the components
	if InputSystem:IsKeyDown(68) then
		print("game state onUpdate called")
		--scene2D:SetRenderComponentPosition(0,vec3.new(0.0, 0.0, 5.0*dt))
		scene2D:SetRenderComponentRotation(0,vec3.new(0.5 * cos(dt), 0.5 * sin(dt), 0.0))
	end
end

function GameState:HandleInputs()
	--if InputSystem:IsKeyDown(87) then
		--self.m_Stack:pop()
		--return
	--end
end

local game_state = GameState:Create(gGameStateStack)
gGameStateStack:change_state(game_state)