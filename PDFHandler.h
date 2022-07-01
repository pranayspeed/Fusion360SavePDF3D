//#ifdef PDFSDK_EXPORTS
//#define PDFSDK_API __declspec(dllexport)
//#else
//#define PDFSDK_API __declspec(dllimport)
//#endif


#include <iostream>

/*
 * Now include all podofo header files, to have access
 * to all functions of podofo and so that you do not have
 * to care about the order of includes.
 *
 * You should always use podofo.h and not try to include
 * the required headers on your own.
 */
#include "podofo\podofo.h"

/*
 * All podofo classes are member of the PoDoFo namespace.
 */
using namespace PoDoFo;



class Pdf3DObject : public PdfElement
{
public:
	Pdf3DObject(const char* pszType, PdfDocument* pParent);

	void EmbeddFile(const char* pszFilename);

};


class Pdf3DViewObject : public PdfElement
{
public:
	Pdf3DViewObject(std::string name, PdfDocument* pParent,PdfArray* c2wMat=NULL,double co=0.0);

	void SetMatrix(std::vector<float> c2wMatrix);

};

struct CameraPDF
{
	PdfArray c2w;
	float co;
	std::string name;
};

class PDFHandler
{
public:
	PDFHandler();
	virtual~PDFHandler();

	void Export3DPDF(const char* pszOutFilename, const char* streamFile);
	void AddView(std::vector<float> c2w, float co, std::string name);
private:
	void Create3DPdf(PdfPage* pPage, PdfDocument* pDocument, const char* pszFileName);	
	

	std::vector<CameraPDF> m_cams;
};







