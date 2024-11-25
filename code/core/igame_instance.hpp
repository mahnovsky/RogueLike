#pragma once

class IRender;

namespace core
{
	class IGameInstance
	{
	public:
		virtual void initialize() = 0;

		virtual void draw(IRender* render) = 0;

		virtual void update(float delta) = 0;
	};
}