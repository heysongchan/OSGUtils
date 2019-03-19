#include <osg/NodeVisitor>
#include <osg/Geode>
#include <osg/Geometry>
#include <osg/Texture2D>

class TexCoordGenerator :public osg::NodeVisitor
{
public:
	TexCoordGenerator() :NodeVisitor(NodeVisitor::TRAVERSE_ALL_CHILDREN)
	{

	};
	virtual ~TexCoordGenerator(){};

	void apply(osg::Geode& node);
protected:
	osg::Vec2Array* generate_coords(osg::Array* vx, osg::Array* nx, float scale);
};

void createTexture2DState(osg::ref_ptr<osg::Image> image, osg::ref_ptr<osg::StateSet> stateset);