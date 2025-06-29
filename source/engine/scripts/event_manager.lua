function CreateObject(obj)
	function obj:new(params)
		local this = params or {}

		setmetatable(this, self)
		self.__index = self
		return this
	end
	return obj
end

EventListener = {}
function EventListener:Create()
	function self:Update(param)
	end

	return CreateObject(EventListener):new()
end

Event = {}

-- need to be used for inheritance
function Event:Create()
	self.m_Listeners = {}

	function self:AddListnersToEvent(listener)
		print(table)
		--table.insert(self.m_Listeners, listener)
	end

	function self:Remove(listener)
		for k,v in pairs(self.m_Listeners) do
			if v == listener then
				table.remove(self.m_Listeners, k)
			end
		end
	end

	function self:Execute()
	end
	
	return CreateObject(Event):new()
end

function Event:ResetListeners()
	for k,v in ipairs(self.m_Listeners) do
		self.m_Listeners[k] = nil
	end
end