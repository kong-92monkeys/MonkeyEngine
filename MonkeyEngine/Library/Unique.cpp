#include "Unique.h"

namespace Lib
{
	UniqueId::UniqueId() noexcept : __uid{ __idAllocator.allocate() }
	{}
}