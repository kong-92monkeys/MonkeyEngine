module;

#include "../Library/GLM.h"

export module ntmonkeys.com.Frameworks.Vertex;
export import :VertexAttribute;

namespace Frameworks
{
	export template <VertexAttribFlags $flag>
	struct Vertex;

	template <>
	struct Vertex<VertexAttribFlags::POS>
	{
	public:
		glm::vec3 pos{ };
	};

	template <>
	struct Vertex<VertexAttribFlags::TEXCOORD>
	{
	public:
		glm::vec2 texCoord{ };
	};

	template <>
	struct Vertex<VertexAttribFlags::NORMAL>
	{
	public:
		glm::vec3 normal{ };
	};

	template <>
	struct Vertex<VertexAttribFlags::COLOR>
	{
	public:
		glm::vec4 color{ };
	};

	template <>
	struct Vertex<VertexAttribFlags::POS_TEXCOORD>
	{
	public:
		glm::vec3 pos{ };
		glm::vec2 texCoord{ };
	};

	template <>
	struct Vertex<VertexAttribFlags::POS_NORMAL>
	{
	public:
		glm::vec3 pos{ };
		glm::vec3 normal{ };
	};

	template <>
	struct Vertex<VertexAttribFlags::POS_COLOR>
	{
	public:
		glm::vec3 pos{ };
		glm::vec4 color{ };
	};

	export
	{
		using Vertex_P = Vertex<VertexAttribFlags::POS>;
		using Vertex_T = Vertex<VertexAttribFlags::TEXCOORD>;
		using Vertex_N = Vertex<VertexAttribFlags::NORMAL>;
		using Vertex_C = Vertex<VertexAttribFlags::COLOR>;

		using Vertex_PT = Vertex<VertexAttribFlags::POS_TEXCOORD>;
		using Vertex_PN = Vertex<VertexAttribFlags::POS_NORMAL>;
		using Vertex_PC = Vertex<VertexAttribFlags::POS_COLOR>;
	}
}