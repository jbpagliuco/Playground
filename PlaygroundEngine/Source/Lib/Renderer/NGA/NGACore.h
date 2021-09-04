#pragma once

#include "NGA/NGADefs.h"

namespace playground
{
	struct NGAInitParams
	{
	};

	void NGAInitialize(const NGAInitParams &params);
	void NGAShutdown();
}