

#include "TextureUtils.h"
#include <osg/ShapeDrawable>
#include <osgDB/ReadFile>
#define _USE_MATH_DEFINES
#include <math.h>

void TexCoordGenerator::apply(osg::Geode& node)
{
	const osg::BoundingSphere& bsphere = node.getBound();
	float scale = 1;
	/*if (bsphere.radius() != 0)
	{
		scale = 5 / bsphere.radius();
	}*/

	for (unsigned i = 0; i < node.getNumDrawables(); i++)
	{
		osg::Geometry* geom = dynamic_cast<osg::Geometry*>(node.getDrawable(i));
		if (geom)
		{
			osg::Vec2Array* tc = generate_coords(geom->getVertexArray(), geom->getNormalArray(), scale);
			geom->setTexCoordArray(0, tc);
		}
	}
}

osg::Vec2Array* TexCoordGenerator::generate_coords(osg::Array* vx, osg::Array* nx, float scale)
{
	osg::Vec2Array* v2a = dynamic_cast<osg::Vec2Array*>(vx);
	osg::Vec3Array* v3a = dynamic_cast<osg::Vec3Array*>(vx);
	osg::Vec4Array* v4a = dynamic_cast<osg::Vec4Array*>(vx);
	osg::Vec2Array* n2a = dynamic_cast<osg::Vec2Array*>(nx);
	osg::Vec3Array* n3a = dynamic_cast<osg::Vec3Array*>(nx);
	osg::Vec4Array* n4a = dynamic_cast<osg::Vec4Array*>(nx);

	osg::ref_ptr<osg::Vec2Array> tc = new osg::Vec2Array();
	for (unsigned i = 0; i < vx->getNumElements(); i++)
	{
		osg::Vec3 P;
		if (v2a) P.set((*v2a)[i].x(), (*v2a)[i].y(), 0);
		if (v3a) P.set((*v3a)[i].x(), (*v3a)[i].y(), (*v3a)[i].z());
		if (v4a) P.set((*v4a)[i].x(), (*v4a)[i].y(), (*v4a)[i].z());

		osg::Vec3 N(0, 0, 1);
		if (n2a) N.set((*n2a)[i].x(), (*n2a)[i].y(), 0);
		if (n3a) N.set((*n3a)[i].x(), (*n3a)[i].y(), (*n3a)[i].z());
		if (n4a) N.set((*n4a)[i].x(), (*n4a)[i].y(), (*n4a)[i].z());

		int axis = 0;
		if (N.y() > N.x() && N.y() > N.z()) axis = 1;
		if (-N.y() > N.x() && -N.y() > N.z()) axis = 1;
		if (N.z() > N.x() && N.z() > N.y()) axis = 2;
		if (-N.z() > N.x() && -N.z() > N.y()) axis = 2;

		osg::Vec2 uv;
		switch (axis)
		{
		case 0:uv.set(P.y(), P.z()); break;
		case 1:uv.set(P.x(), P.z()); break;
		case 2:uv.set(P.x(), P.y()); break;
		default:
			break;
		}
		tc->push_back(uv*scale);
	}
	return tc.release();
}

void createTexture2DState(osg::ref_ptr<osg::Image> image, osg::ref_ptr<osg::StateSet> stateset)
{
	osg::ref_ptr<osg::Texture2D> texture = new osg::Texture2D();
	texture->setDataVariance(osg::Object::DYNAMIC);
	texture->setImage(image);

	texture->setFilter(osg::Texture::MIN_FILTER, osg::Texture::LINEAR_MIPMAP_LINEAR);
	texture->setFilter(osg::Texture::MAG_FILTER, osg::Texture::LINEAR);
	texture->setWrap(osg::Texture::WRAP_S, osg::Texture::REPEAT);
	texture->setWrap(osg::Texture::WRAP_T, osg::Texture::REPEAT);

	stateset->setTextureAttributeAndModes(0, texture, osg::StateAttribute::ON);
}


//地形
void createTerrain(osg::ref_ptr<osg::Geode>& geo)
{
	//texture
	osg::ref_ptr<osg::StateSet> stateset = geo->getOrCreateStateSet();
	osg::ref_ptr<osg::Image> image = osgDB::readRefImageFile("J:\\OSG\\data\\Images/lz.rgb");
	if (image)
	{
		osg::ref_ptr<osg::Texture2D> texture = new osg::Texture2D;
		texture->setImage(image);
		texture->setFilter(osg::Texture::MIN_FILTER, osg::Texture::LINEAR);
		stateset->setTextureAttributeAndModes(0, texture, osg::StateAttribute::ON);
	}
	//
	stateset->setMode(GL_LIGHTING, osg::StateAttribute::ON);
	//
	osg::ref_ptr<osg::HeightField> grid = new osg::HeightField;
	
		grid->allocate(38, 39);  //cell数量
		grid->setXInterval(0.021f); //cell宽
		grid->setYInterval(0.038f); //cell高

		for (unsigned int r = 0; r<39; ++r)
		{
			for (unsigned int c = 0; c<38; ++c)
			{
				//                grid->setHeight(c,r,vertex[r+c*39][2]);
				grid->setHeight(c, r, 1);
				if (c == r)
				{
					grid->setHeight(c, r, 3);
				}
			}
		}	
	grid->setBorderWidth(10);
	grid->setSkirtHeight(20);//裙边
	geo->addDrawable(new osg::ShapeDrawable(grid));
}

//计算两个向量夹角
float getRadianOf3D(osg::Vec3f line1, osg::Vec3f line2, osg::Vec3f direction)
{
	line1.normalize();
	line2.normalize();
	float temp = (line1 * line2);
	// 1和-1时为0和180°
	float f = fabs(fabs(temp) - 1);
	float epsilon = std::numeric_limits<float>::epsilon();
	if (f< epsilon)
	{
		return temp > 0 ? 0 : osg::PI;
	}
	float angle = (float)acos(temp);

	//// 两个向量的叉乘结果与屏幕方向是否一致来判断角度是否超过180°
	//osg::Vec3f axis = line1 ^ line2;
	//return axis * direction > 0 ? angle : 2 * osg::PI - angle;
	return angle;
}


void getRadianOf2D(osg::Vec3& pre, osg::Vec3& cur, osg::Vec3& next, float ret[])
{
	osg::Vec3 v0 = cur - pre;
	osg::Vec3 v1 = next - cur;
	float H = std::atan2(v1.y(), v1.x()) - std::atan2(v0.y(), v0.x());
	float V = std::atan2(v1.y(), v1.z()) - std::atan2(v0.y(), v0.z());
	ret[0] = H;
	ret[1] = V;
}

//计算长度
float calcCir(osg::Vec3& pre, osg::Vec3& cur, osg::Vec3& next)
{

	osg::Vec3 v0 = cur - pre;
	osg::Vec3 v1 = next - cur;
	float rad = getRadianOf3D(v0, v1, osg::Vec3(0, 1, 0));

	float x0 = cur.x();
	float y0 = cur.y();
	float z0 = cur.z();
	float x1 = next.x();
	float y1 = next.y();
	float z1 = next.z();

	float len = std::sqrtf(std::powf((x1 - x0), 2.0) + std::powf((y1 - y0), 2.0));
	float length = std::sqrtf(std::powf(len, 2.0) + std::powf((z1 - z0), 2.0)) / 2.0;
	float a = rad / 2.0;
	float radius = length / std::sinf(a);
	float c = 2 * M_PI*radius;
	float rate = osg::RadiansToDegrees(rad) / 360;
	c = c*rate;
	c = c < 0 ? 0 - c : c;
	return c;
}