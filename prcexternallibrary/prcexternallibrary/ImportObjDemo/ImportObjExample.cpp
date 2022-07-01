#include "ImportObjExample.h"
#include <vector>
#include <math.h>
#include "Wavefront2GLInstanceGraphicsShape.h"
#include "b3ResourcePath.h"
#include "b3FileUtils.h"
#include <string.h>

#include "tiny_obj_loader.h"

class ImportObjSetup
{

public:
	ImportObjSetup(const char* filename = NULL);
    virtual ~ImportObjSetup();
	char m_fileName[1024];
	tinyobj::Bbox bbox;
	virtual void initPhysics();
};

ImportObjSetup::ImportObjSetup(const char* fileName)
{
	memcpy(m_fileName, fileName, strlen(fileName) + 1);
}

ImportObjSetup::~ImportObjSetup()
{
    
}

void ImportObjSetup::initPhysics()
{
	char relativeFileName[1024];
	if (b3ResourcePath::findResourcePath(m_fileName, relativeFileName, 1024))
	{
		char pathPrefix[1024];

		b3FileUtils::extractPath(relativeFileName, pathPrefix, 1024);


		std::vector<tinyobj::shape_t> shapes;

		std::string err = tinyobj::LoadObj(bbox, shapes, relativeFileName, pathPrefix);
	}
}

CommonExampleInterface*    ImportObjCreateFunc(struct CommonExampleOptions& options, const char* fileName)
 {
	 ImportObjSetup(fileName);
 }
