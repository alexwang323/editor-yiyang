local soundEffectPlayCount = 0
local vPosition = SoundVec3.new(0.0,0.0,0.0)

main = {
	[1] = {
		on_enter = function()
			-- on_enter function for L_playSound
			--print("on_enter function for L_playSound is called")
			if soundEffectPlayCount < 1 then 
				audioEngine:PlaySound("json object", false, vPosition, audioEngine:VolumetoDB(0.2));
				soundEffectPlayCount = soundEffectPlayCount + 1
			end
		end
	},
	[2] = {
		on_update = function()
			-- on_update function for L_playSound
			--print("on_update function for L_playSound is called for "..sendbehaviorAgent.name)
			--print("x position: "..sendbehaviorAgent.render.transform.position.x)
			return NodeStatus.SUCCESS
		end
	}
}