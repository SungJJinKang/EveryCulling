#pragma once

namespace culling
{
	struct BufferViewer
	{
		void* pixels;
	};

	class DebugBuffer
	{
	private:

	public:
		virtual void DrawBuffer(BufferViewer bufferViewer);
	};
}

