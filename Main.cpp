// Main.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <osgViewer/Viewer>
#include <osgDB/ReadFile>
#include <osgGA/TrackballManipulator>
#include <osg/ShapeDrawable>
#include "TextureUtils.h"


int _tmain(int argc, _TCHAR* argv[]) 
{
	
	osgViewer::Viewer* v=new osgViewer::Viewer();
	osg::Group* root = new osg::Group();
//	osg::ref_ptr<osg::Node> shape0 = osgDB::readRefNodeFile("J:\\OSG\\data\\cow.osg");
	osg::Geode* geo = new osg::Geode();
	
	geo->addDrawable(new osg::ShapeDrawable(new osg::Box()));

	TexCoordGenerator tcg;
	geo->accept(tcg);
	osg::ref_ptr<osg::Image> image=osgDB::readImageFile("zz.JPG");
	createTexture2DState(image, geo->getOrCreateStateSet());

	root->addChild(geo);

	v->setSceneData(root);
	v->setCameraManipulator(new osgGA::TrackballManipulator());
	v->setUpViewInWindow(20, 20, 600, 400);
	v->run();
	return 0;
}

