 
#pragma once

#include "TesseKetor.h"


class oPRCFile;
using namespace TesseKetor;

class PRCExporter : public TesseKetorTraverser
{
private:
	TesseKetor::Model_tk* m_model;
	oPRCFile* m_file;
	unsigned char* m_imD;
	unsigned int sizeImage;
	
public:
	PRCExporter(oPRCFile* file, TesseKetor::Model_tk* model);
	~PRCExporter();

protected:

	void ExportMatrix(std::vector<float> matrix);
	virtual void ModelStart(TesseKetor::Model_tk* model);
	virtual void ModelEnd(TesseKetor::Model_tk* model);
	virtual void PartStart(TesseKetor::Part_tk* part);
	virtual void PartEnd(TesseKetor::Part_tk* part);
	virtual void BodyStart(TesseKetor::Body_tk* body);
};

class PdfIO
{

private:
	TesseKetor::Model_tk* m_model;
	oPRCFile* m_file;
	std::string m_fileName;
	std::vector<float> Getc2WFromCamera(Camera_tk* cam);

public :
	PdfIO(TesseKetor::Model_tk* model, std::string fileName);
	virtual ~PdfIO();

	void Export();
	void WritePDF(std::string filepdf, std::string inPrc);
};
