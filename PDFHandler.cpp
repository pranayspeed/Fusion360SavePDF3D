/***************************************************************************
 *   Copyright (C) 2006 by Dominik Seichter                                *
 *   domseichter@web.de                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/


/*
 * Include the standard headers for cout to write
 * some output to the console.
 */
#include <iostream>

/*
 * Now include all podofo header files, to have access
 * to all functions of podofo and so that you do not have
 * to care about the order of includes.
 *
 * You should always use podofo.h and not try to include
 * the required headers on your own.
 */
/*
 * All podofo classes are member of the PoDoFo namespace.
 */

#include "PDFHandler.h"

#include "podofo\base\PdfArray.h"

using namespace PoDoFo;


pdf_long ReadFileSize(std::string sFilename)
{
	FILE * f = fopen(sFilename.c_str(), "rb");
	fseek(f, 0L, SEEK_END);
	size_t bytes = ftell(f);
	fclose(f);
	return bytes;
}
const pdf_utf16be* ReadFile(std::string sFilename)
{
	FILE * f = fopen(sFilename.c_str(), "rb");
	fseek(f, 0L, SEEK_END);
	size_t bytes = ftell(f);
	fseek(f, 0L, SEEK_SET);
	pdf_utf16be* buf = new pdf_utf16be[bytes];
	size_t bytesRead = fread(buf, 1, bytes, f);
	if (bytesRead != bytes)
	{
		//std::cerr << "Failed to read file " << sFilename << std::endl;
		PODOFO_RAISE_ERROR(ePdfError_TestFailed);
	}
	fclose(f);
	return (const pdf_utf16be*)buf;
}

Pdf3DObject::Pdf3DObject(const char* pszType, PdfDocument* pParent) : PdfElement("3D", pParent)
{

}

void Pdf3DObject::EmbeddFile(const char* pszFilename)
{
	PdfObject* pStream = GetObject();
	PdfFileInputStream* stream=new PdfFileInputStream(pszFilename);
	pStream->GetStream()->Set(stream);
	std::string fileName(pszFilename);
	if (fileName.find(".u3d")!=-1)
	{
		pStream->GetDictionary().AddKey(PdfName::KeySubtype, PdfName("U3D"));
	}
	else
	{
		pStream->GetDictionary().AddKey(PdfName::KeySubtype, PdfName("PRC"));

	}
	// Add additional information about the embedded file to the stream
	PdfDictionary params;
	params.AddKey("Size", static_cast<pdf_int64>(stream->GetFileLength()));
	// TODO: CreationDate and ModDate
	pStream->GetDictionary().AddKey("Params", params);


}



Pdf3DViewObject::Pdf3DViewObject(std::string name, PdfDocument* pParent, PdfArray* c2wMat, double co) :PdfElement("3DVIEW", pParent)
{
	PdfDictionary& dict3D = GetObject()->GetDictionary();
	/*dict3D.AddKey("TYPE", PdfName("3DVIEW"));*/
	dict3D.AddKey("XN", PdfString(name));
	dict3D.AddKey("IN", PdfString("Default"));
	dict3D.AddKey("MS", PdfName("M"));
	//Matrix
	if (c2wMat)
	{
		PdfArray arrMat(*c2wMat);
		dict3D.AddKey(PdfName("C2W"), (const PdfObject&)arrMat);
		dict3D.AddKey(PdfName("CO"), PdfVariant(co));
	}
	else
	{
		PdfArray arrMat;
		arrMat.push_back(PdfVariant(1.0));
		arrMat.push_back(PdfVariant(0.0));
		arrMat.push_back(PdfVariant(-1.0));

		arrMat.push_back(PdfVariant(0.0));
		arrMat.push_back(PdfVariant(1.0));
		arrMat.push_back(PdfVariant(0.0));

		arrMat.push_back(PdfVariant(-0.5));
		arrMat.push_back(PdfVariant(0.0));
		arrMat.push_back(PdfVariant(-0.5));

		arrMat.push_back(PdfVariant(0.0));
		arrMat.push_back(PdfVariant(0.0));
		arrMat.push_back(PdfVariant(0.0));

		dict3D.AddKey(PdfName("C2W"), (const PdfObject&)arrMat);
		dict3D.AddKey(PdfName("CO"), PdfVariant(co));
	}
	// / BG << / Type / 3DBG / Subtype / SC / CS / DeviceRGB / C[1 1 1] >> / LS << / Type / 3DLightingScheme / Subtype / CAD >> ;
	PdfDictionary lightSource;
	lightSource.AddKey(PdfName("Type"), PdfName("3DLightingScheme"));
	lightSource.AddKey(PdfName("Subtype"), PdfName("CAD"));
	dict3D.AddKey(PdfName("LS"), lightSource);
	PdfDictionary background;
	background.AddKey(PdfName("Type"), PdfName("3DBG"));
	background.AddKey(PdfName("Subtype"), PdfName("SC"));
	background.AddKey(PdfName("CS"), PdfName("DeviceRGB"));

}

void Pdf3DViewObject::SetMatrix(std::vector<float> c2wMatrix)
{

}

PDFHandler::PDFHandler()
{

}


PDFHandler::~PDFHandler()
{

}

void PDFHandler::AddView(std::vector<float> c2w, float co, std::string name)
{
	CameraPDF cam;
	for (size_t i = 0; i < 12; i++)
	{
		cam.c2w.push_back(c2w[i]);
	}	
	cam.co = co;
	cam.name = name;
	m_cams.push_back(cam);
}

void PDFHandler::Create3DPdf(PdfPage* pPage, PdfDocument* pDocument, const char* pszFileName)
{
	PdfArray viewRefArray;
	if (m_cams.size())
	{
		for (size_t i = 0; i < m_cams.size(); i++)
		{			
			Pdf3DViewObject view3d(m_cams[i].name, pDocument, &m_cams[i].c2w, m_cams[i].co);
			viewRefArray.push_back(view3d.GetObject()->Reference());
		}
	}
	else
	{
		Pdf3DViewObject view3d("3DVIEW", pDocument);
		viewRefArray.push_back(view3d.GetObject()->Reference());
	}
	

	PdfRect rect(0.0, 0.0, 1000.0, 600.0);
	
	PdfDictionary ef;
	ef.AddKey("A", PdfName("PO"));
	ef.AddKey("AIS", PdfName("L"));
	ef.AddKey("DIS", PdfName("I"));
	ef.AddKey("NP", PdfVariant(false));
	ef.AddKey("TB", PdfVariant(true));
	ef.AddKey("Transparent", PdfVariant(true));	

	PdfAnnotation* pAnnotation = pPage->CreateAnnotation(ePdfAnnotation_3D, rect);
	pAnnotation->GetObject()->GetDictionary().AddKey("3DA", ef);
	pAnnotation->GetObject()->GetDictionary().AddKey("3DV", PdfString("Default"));
	

	Pdf3DObject pdf3D("3D", pDocument);
	//pdf3D.GetObject()->GetDictionary().AddKey("Subtype", PdfName("3D"));
	pdf3D.GetObject()->GetDictionary().AddKey("VA", viewRefArray);

	pdf3D.EmbeddFile(pszFileName);

	pAnnotation->GetObject()->GetDictionary().AddKey("3DD", pdf3D.GetObject()->Reference());
	

}




void PDFHandler::Export3DPDF(const char* pszOutFilename, const char* streamFile)
{
	/*
	 * PdfStreamedDocument is the class that can actually write a PDF file.
	 * PdfStreamedDocument is much faster than PdfDocument, but it is only
	 * suitable for creating/drawing PDF files and cannot modify existing
	 * PDF documents.
	 *
	 * The document is written directly to pszFilename while being created.
	 */
	PdfStreamedDocument document(pszOutFilename);


	/*
	 * PdfPainter is the class which is able to draw text and graphics
	 * directly on a PdfPage object.
	 */
	PdfPainter painter;

	/*
	 * This pointer will hold the page object later.
	 * PdfSimpleWriter can write several PdfPage's to a PDF file.
	 */
	PdfPage* pPage;

	/*
	 * A PdfFont object is required to draw text on a PdfPage using a PdfPainter.
	 * PoDoFo will find the font using fontconfig on your system and embedd truetype
	 * fonts automatically in the PDF file.
	 */
	PdfFont* pFont;

	try {
		/*
		 * The PdfDocument object can be used to create new PdfPage objects.
		 * The PdfPage object is owned by the PdfDocument will also be deleted automatically
		 * by the PdfDocument object.
		 *
		 * You have to pass only one argument, i.e. the page size of the page to create.
		 * There are predefined enums for some common page sizes.
		 */
		pPage = document.CreatePage(PdfPage::CreateStandardPageSize(ePdfPageSize_Legal, true));

		/*
		 * If the page cannot be created because of an error (e.g. ePdfError_OutOfMemory )
		 * a NULL pointer is returned.
		 * We check for a NULL pointer here and throw an exception using the RAISE_ERROR macro.
		 * The raise error macro initializes a PdfError object with a given error code and
		 * the location in the file in which the error ocurred and throws it as an exception.
		 */
		if (!pPage)
		{
			PODOFO_RAISE_ERROR(ePdfError_InvalidHandle);
		}

		/*
		 * Set the page as drawing target for the PdfPainter.
		 * Before the painter can draw, a page has to be set first.
		 */
		painter.SetPage(pPage);

		Create3DPdf(pPage, &document, streamFile);

		/*
		 * Tell PoDoFo that the page has been drawn completely.
		 * This required to optimize drawing operations inside in PoDoFo
		 * and has to be done whenever you are done with drawing a page.
		 */
		painter.FinishPage();

		/*
		 * The last step is to close the document.
		 */
		document.Close();
	}
	catch (const PdfError & e) {
		/*
		 * All PoDoFo methods may throw exceptions
		 * make sure that painter.FinishPage() is called
		 * or who will get an assert in its destructor
		 */
		try {
			painter.FinishPage();
		}
		catch (...) {
			/*
			 * Ignore errors this time
			 */
		}

		throw e;
	}
}

