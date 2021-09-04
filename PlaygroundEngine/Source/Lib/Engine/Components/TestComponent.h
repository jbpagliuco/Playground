#pragma once

#include "GameComponent.h"

#include "Core/Debug/Log.h"
#include "Core/Util/Serialize.h"

namespace playground
{
	class TestComponent : public GameComponent
	{
	public:
		inline void Deserialize(DeserializationParameterMap &params)
		{
			mTestInt = params["int"].AsInt(100);
		}

		inline void Update() override { LogInfo("TEST", "%d", mTestInt); }

	private:
		int mTestInt;
	};
}