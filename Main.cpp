// Main.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <osgViewer/Viewer>
#include <osgDB/ReadFile>
#include <osgGA/TrackballManipulator>
#include <osg/ShapeDrawable>
#include <osg/TexGen>
#include <osg/Plane>
#include <osgUtil/SmoothingVisitor>
#include <osg/Geode>
#include "TextureUtils.h"
#include <memory>

void setTexture2D(osg::ref_ptr<osg::StateSet>& state, osg::ref_ptr<osg::Image>& image);

osg::ref_ptr<osg::Geode> createModel()
{	
	osg::ref_ptr<osg::Geode> geo = new osg::Geode();
	osg::ref_ptr<osg::Geometry> geom = new osg::Geometry();
	osg::ref_ptr<osg::Vec3Array> v = new osg::Vec3Array();

	float ii[] = { 0, 0.1, 0.3, 0.6, 1, 1.5, 2.1, 2.8, 3.4, 3.9, 4.3, 4.6, 4.8, 4.9, 4.8, 4.6, 4.3, 3.9, 3.4, 2.8, 2.1, 1.5, 1, 0.3, 0.1, 0 };
	unsigned size = sizeof(ii)/sizeof(unsigned);
	float f = 0;
	char str[100] = { 0 };
	for (unsigned i = 0; i < 40; i++)
	{
		unsigned index = i % size;
		v->push_back(osg::Vec3(0+ii[index], i, ii[index]));
		v->push_back(osg::Vec3(10+ii[index], i, ii[index]));
		f = f + 0.1;
		sprintf(str, "%f   %d", ii[index], i);
		std::cout << str << std::endl;
	}
	
	
	geom->setVertexArray(v);
	//
	/*osg::ref_ptr<osg::Vec2Array> vt = new osg::Vec2Array();
	vt->push_back(osg::Vec2(0, 0));
	vt->push_back(osg::Vec2(1, 0));
	vt->push_back(osg::Vec2(1, 1));
	vt->push_back(osg::Vec2(0, 1));
	geom->setTexCoordArray(0, vt);*/
	//
	geom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::QUAD_STRIP, 0, v->size()));
	osgUtil::SmoothingVisitor::smooth(*geom);
	osg::ref_ptr<osg::Image> image = osgDB::readImageFile("zz.JPG");
	osg::ref_ptr<osg::StateSet> state=geom->getOrCreateStateSet();
	setTexture2D(state, image);
	geo->addDrawable(geom);
	return geo;
}

void setTexture2D(osg::ref_ptr<osg::StateSet>& state, osg::ref_ptr<osg::Image>& image)
{
	osg::TexGen* gen = new osg::TexGen();
	gen->setMode(osg::TexGen::OBJECT_LINEAR);
	gen->setPlane(osg::TexGen::S, osg::Plane(1, 0, 0, 0));
	state->setTextureAttribute(0, gen, osg::StateAttribute::ON);
	state->setTextureMode(0, GL_TEXTURE_GEN_S, osg::StateAttribute::ON);
	state->setTextureMode(0, GL_TEXTURE_GEN_T, osg::StateAttribute::ON);	 
	//
	osg::ref_ptr<osg::Texture2D> texture = new osg::Texture2D();
	texture->setDataVariance(osg::Object::DYNAMIC);
	texture->setImage(image);

	texture->setFilter(osg::Texture::MIN_FILTER, osg::Texture::LINEAR_MIPMAP_LINEAR);
	texture->setFilter(osg::Texture::MAG_FILTER, osg::Texture::LINEAR);
	texture->setWrap(osg::Texture::WRAP_S, osg::Texture::REPEAT);
	texture->setWrap(osg::Texture::WRAP_T, osg::Texture::REPEAT);
	state->setTextureAttributeAndModes(0, texture, osg::StateAttribute::ON);
}



int _tmain(int argc, _TCHAR* argv[]) 
{
	

	
	osg::ref_ptr<osgViewer::Viewer> v = new osgViewer::Viewer(); 
	osg::ref_ptr<osg::Group> root = new osg::Group();
	osg::ref_ptr<osg::Geode> geo = createModel();
	root->addChild(geo);
//	osg::ref_ptr<osg::Node> shape0 = osgDB::readRefNodeFile("J:\\OSG\\data\\cow.osg");
	
	
	

	v->setSceneData(root);
	v->setCameraManipulator(new osgGA::TrackballManipulator());
	v->setUpViewInWindow(20, 40, 600, 400);
	v->realize();
	v->run();
	return 0;
}

