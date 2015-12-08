#include <Rendering/FrameBufferObject.h>

class DepthPeelingBuffers
{
public:
	std::vector<FrameBufferObject* > m_fbos;

	DepthPeelingBuffers(int width = 800, int height = 600, int depthBuffers = 1, int colorAttachments = 0)
	{
		m_fbos.resize(depthBuffers, 0);

		for ( int i = 0; i < depthBuffers; i++)
		{
			m_fbos[i] = new FrameBufferObject(width, height);

			if(colorAttachments != 0)
			{
				m_fbos[i]->addColorAttachments(colorAttachments);
			}
		}
	}	
};