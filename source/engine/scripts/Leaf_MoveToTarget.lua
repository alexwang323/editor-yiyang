local behaviorAgent = GOF:findGameObject("json object")
local behaviorAgentPos = behaviorAgent.render:GetTransform().position
local rotate_angle = vec3.new(0.0, 0.02,0.0)
local move_speed = vec3.new(0.005,0.0, 0.0)
local targetPoint
main = {
	[1] = {
		on_enter = function()
			-- on_enter function for L_moveToTarget
			--print("on_enter function for L_moveToTarget called")
			targetPoint = vec3.new(1.5, 1.0, -2.0)
		end
	},
	[2] = {
		on_update = function()
			-- on_update function for L_movetoTarget
			--print("on_udpate function for L_moveToTarget called")
			behaviorAgent.render:GetTransform():Rotate(rotate_angle)
			--print("Position of json object: "..behaviorAgent.render.transform.position.x.." , "..behaviorAgent.render.transform.position.y.." , "..behaviorAgent.render.transform.position.z)
			if behaviorAgentPos.x < targetPoint.x then
				behaviorAgent.render:GetTransform():ModPosition(move_speed)
				return NodeStatus.RUNNING
			elseif behaviorAgentPos.x > (targetPoint.x + 0.1) then
				return NodeStatus.FAILURE
			else 
				return NodeStatus.SUCCESS
			end
		end
	}
}