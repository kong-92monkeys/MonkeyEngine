export module ntmonkeys.com.Frameworks.Layer;

import ntmonkeys.com.Lib.Unique;
import ntmonkeys.com.Graphics.CommandBuffer;
import <memory>;

namespace Frameworks
{
	export class Layer : public Lib::Unique
	{
	public:
		void draw(Graphics::CommandBuffer &commandBuffer);

	private:

	};
}