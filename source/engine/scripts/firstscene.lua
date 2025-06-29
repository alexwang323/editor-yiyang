-- run other scripts and then return to main script here
dofile("scripts/event_manager.lua")
dofile("scripts/collision_event.lua")
dofile("scripts/TriggerSystem.lua")

-- game states 
gGameStateStack = StateStack()
dofile("scripts/GameState.lua")
dofile("scripts/TitleState.lua")

-- event system
dofile("scripts/collision_event.lua")

gCollisionEvent = CollisionEvent:Create()
gTriggerSystem = TriggerSystem:Create()
gCollisionEvent:AddListnersToEvent(gTriggerSystem)

main = {
	[1] = {
		-- scripting system call
		update = function()
		    --if InputSystem:IsKeyDown(68) then
				--print("game state onUpdate called")
				--scene2D:SetRenderComponentPosition(0,vec3.new(0.0, 0.0, 5.0 * dt))
				--scene2D:SetRenderComponentRotation(0,vec3.new(0.5 * cos(dt), 0.5 * sin(dt), 0.0))
			--end
		    --if demoCube.render.transform.position.x < 1.5 then
				--demoCube.render.transform:ModPosition(vec3.new(0.001,0.0,0.0))
				--print("Position of Cube in transformComponent: "..demoCube.transform.position.x.." , "..demoCube.transform.position.y.." , "..demoCube.transform.position.z)
				--print("Position of Cube in renderComponent: "..demoCube.render.transform.position.x.." , "..demoCube.render.transform.position.y.." , "..demoCube.render.transform.position.z)
				--print("Position of Cube in local transformComponent: "..demoCubeTr.position.x.." , "..demoCubeTr.position.y.." , "..demoCubeTr.position.z)
			--end
			--print("We are updating in lua!")
			-- manipulate the game objects here
			--firstArchetype.GetTransform().ModPosition(vec3.new(0.01,0.01,0.01))
			--print("Position of Cube: "..demoCube.render.transform.position.x.." , "..demoCube.render.transform.position.y.." , "..demoCube.render.transform.position.z)
			gGameStateStack:update(0.016)
			-- capture all the collisions here in the collision recording
			for i,v in ipairs(scene.physicsDir.collision_recording) do
				print("num_collision: "..scene.physicsDir.num_collision)
				local obj_a = v.a
				local obj_b = v.b
				if obj_a and obj_b then
					gCollisionEvent:EmitEvent(obj_a, obj_b)
				end
			end
			--gCollisionEvent:EmitEvent(collider_a, collider_b)
		end
	},
	[2] = {
		-- scripting system call
		setup = function()
			print("setup function in script is called")
			audioEngine:PlaySound("Background", true, vPosition, audioEngine:VolumetoDB(0.1));
			--scene:addToRenderComponents(demoCubeRender)

			--[[
			-- sphere collider (after the stress test)
			local sphereCollider = collider.new(sphereColliderTr)
			local tmpCircle = prim_circle.new()
			tmpCircle:set_radius(1.0)
			sphereCollider.axes:emplace_back(tmpCircle)
			sphereCollider:SetScale(vec3.new(1.0))
			sphereCollider.is_affected_by_gravity = true;
			if sphereCollider.is_affected_by_gravity == true then
				print("sphereCollider.is_affected_by_gravity is true ")
			end
			sphereCollider:set_restitution(0.5)
			sphereCollider:set_mass(1.0)
			sphereCollider:update_inertia()
			scene.physicsDir.axes:push_back(sphereCollider)
			--sphereRC:SetTransform(sphereCollider.transform)
			
			scene:addToRenderComponents(sphereRC) --]]

			--[[
			--build the plane matrix
			local planeMat = Material.new(
				vec3.new(0, 0, 0),
				vec3.new(radians(-90.0), 0, radians(-45.0)),
				vec3.new(10.0, 10.0, 10.0)
			)
			local planeCollider = collider.new(
				TransformComponent.new(
					vec3.new(0.0, 0.0, 0.0),
					vec3.new(radians(-90.0), 0.0, radians(-45.0)),
					vec3.new(10.0, 10.0, 10.0)
				)
			)
			-- load texture function already bound
			planeMat:LoadTexture(MATERIAL_BIT.ALBEDO_BIT, "plane")

			local planeRC = RenderComponent.new()
			planeRC:Init(MESH_TYPE.MESH_TYPE_PLANE)
			planeRC:SetMaterial(planeMat)
			planeRC:SetTransform(planeCollider)

			tmp_primLine = prim_line.new()
			tmp_primLine:set_direction(vec3.new(1,0,0))
			planeCollider.axes:push_back(tmp_primLine)
			tmp_primLine = prim_line.new()
			tmp_primLine:set_direction(vec3.new(0,1,0))
			planeCollider.transformRef = planeRC.transform
			planeCollider.axes:push_back(tmp_primLine)
			planeCollider.is_affected_by_gravity = false
			planeCollider:set_restitution(1.0)
			planeCollider:set_frictions(1.0, 1.0)
			planeCollider:set_mass(10000.0)
			planeCollider:update_inertia()
			physicsDir.axes:push_back(planeCollider)
			scene:addToRenderComponents(planeRC)  --]] 
		end  
	},
	[3] = {
		render = function()
			-- later gonna move scene:addToRenderComponents here
			print("render State function is called")
			gStateStack: render()
		end
	}
}