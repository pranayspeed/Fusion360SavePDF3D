// ColloadaIO.cpp : Defines the initialization routines for the DLL.
//

#pragma once
//#include "stdafx.h"
#include "PdfIO.h"

#include "oPRCFile.h"

#include <iostream>
#include <iomanip>
#include <fstream>
#include <cmath>
#include <string>

#include <iterator>
#include <algorithm>

#include <Windows.h>
PRCoptions grpopt;

#include "PDFHandler.h"


PRCExporter::PRCExporter(oPRCFile* file, TesseKetor::Model_tk* model) : m_file(file), TesseKetorTraverser(model)
{

	grpopt.no_break = true;
	grpopt.do_break = false;
	grpopt.tess = true;
	grpopt.closed = true;


	m_imD = NULL;
	//std::ifstream imageFile("D:\\Test.png", std::ios::binary);
	//imageFile.seekg(0, std::ios::end);

	//sizeImage = imageFile.tellg();
	//imageFile.seekg(0, std::ios::beg);
	//char* imgData = new char[sizeImage];
	//int i = 0;
	//while (imageFile.good())
	//{
	//	imageFile.read(&imgData[i], 1);
	//	i++;
	//}
	//imageFile.close();
	//m_imD = (uint8_t*)imgData;
}
PRCExporter::~PRCExporter()
{

}
void PRCExporter::ExportMatrix(std::vector<float> matrix)
{
	double mat[4][4];
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			mat[j][i] = matrix[(j * 4) + i];
		}
	}
}
void PRCExporter::ModelStart(TesseKetor::Model_tk* model)
{
	double t[4][4];
	std::vector<float> transform = model->GetTransform();
	int transfIndex = 0;
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			t[j][i] = transform[transfIndex++];
		}
	}
	m_file->begingroup(model->GetName().c_str(), &grpopt, (double *)t);
}
void PRCExporter::ModelEnd(TesseKetor::Model_tk* model)
{
	m_file->endgroup();
}
void PRCExporter::PartStart(TesseKetor::Part_tk* part)
{
	double t[4][4];
	std::vector<float> transform = part->GetTransform();
	int transfIndex = 0;
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			t[j][i] = transform[transfIndex++];
		}
	}
	m_file->begingroup(part->GetName().c_str(), &grpopt, (double *)t);
}
void PRCExporter::PartEnd(TesseKetor::Part_tk* part)
{
	m_file->endgroup();
}
void PRCExporter::BodyStart(TesseKetor::Body_tk* body)
{
	double t[4][4];
	std::vector<float> transform =body->GetTransform();
	int transfIndex = 0;
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			t[j][i] = transform[transfIndex++];
		}
	}
	//ambient(a), diffuse(d), emissive(e), specular(s), alpha(p), shininess(h),
	//	picture_data(pic), picture_format(picf), picture_width(picw), picture_height(pich), picture_size(pics),
	//	picture_replace(picreplace), picture_repeat(picrepeat)

	double* rgb = body->GetRGB();
	double* rgbAmb = body->GetAmbientRGB();
	double* rgbSpec = body->GetSpecularRGB();
	double shininess = body->GetShineness();
	double alpha = 1.0 -body->GetTransparency();
//	double transp = 1.0 -body->GetTransparency();
	if (alpha < 0.2)
	{
		alpha = 0.2;
	}
	PRCmaterial materialBase(
		RGBAColour(rgbAmb[0], rgbAmb[1], rgbAmb[2], 1),
		RGBAColour(rgb[0], rgb[1], rgb[2], 1),
		RGBAColour(rgbAmb[0], rgbAmb[1], rgbAmb[2], 1),
		RGBAColour(rgbSpec[0], rgbSpec[1], rgbSpec[2], 1),
		alpha, shininess);
		//,m_imD, KEPRCPicture_PNG, 2, 2, sizeImage, true, false);

	m_file->begingroup(body->GetName().c_str(), &grpopt, (double *)t);

	TesseKetor::Face_tk* face = body->GetFaces();
	int faceIndex = 0;

	std::vector<double> vertexList, normalList;
	std::vector<int> faceList;


	while (face)
	{
		vertexList.insert(vertexList.end(), face->GetVertices(), face->GetVertices() + (face->GetVertexCount() * 3));
		normalList.insert(normalList.end(), face->GetNormals(), face->GetNormals() + (face->GetNormalCount() * 3));
		faceList.push_back(faceIndex++);
		faceList.push_back(faceIndex++);
		faceList.push_back(faceIndex++);

		face = face->GetNext();

	}

	const size_t nP = vertexList.size()/3;
	double* P = new double[nP * 3];
	
	const size_t nI = faceList.size()/3;
	uint32_t* PI = new uint32_t[nI * 3];

	for (size_t i = 0; i < vertexList.size(); i++)
	{
		P[i] = vertexList[i];
	}
	for (size_t i = 0; i < faceList.size(); i++)
	{
		PI[i] = faceList[i];
	}
	
	m_file->addTriangles(nP, (const double(*)[3])P, nI, (const uint32_t(*)[3]) PI, materialBase, 0, NULL, NULL, 0, 0, 0, 0, NULL, NULL, 0, NULL, NULL, NULL);
	m_file->endgroup();

}

PdfIO::PdfIO(TesseKetor::Model_tk* model, std::string fileName) :m_model(model)
{
	m_fileName = fileName;
}
PdfIO::~PdfIO()
{
	
}

std::vector<float> PdfIO::Getc2WFromCamera(Camera_tk* cam)
{
	std::vector<float> c2W;
	c2W.insert(c2W.end(),cam->GetLeftVec(), cam->GetLeftVec() + 3);
	c2W.insert(c2W.end(), cam->GetUpVec(), cam->GetUpVec() + 3);
	c2W.insert(c2W.end(), cam->GetViewVec(), cam->GetViewVec() + 3);
	c2W.insert(c2W.end(), cam->GetCameraPosition(), cam->GetCameraPosition() + 3);
	return c2W;
}

void PdfIO::Export()
{
	std::string prcFileName = m_fileName;
	int pos = prcFileName.find(".pdf");
	prcFileName.replace(pos, 4, ".prc");

	m_file = new oPRCFile(prcFileName);

	PRCExporter* prcExporter = new PRCExporter(m_file, m_model);
	prcExporter->Export();
	m_file->finish();
	delete prcExporter;

	PDFHandler* pdfHandler = new PDFHandler();
	std::vector<Camera_tk*> modelCams= m_model->GetCameras();
	for (size_t i = 0; i < modelCams.size(); i++)
	{
		std::vector<float>& c2w= Getc2WFromCamera(modelCams[i]);		
		pdfHandler->AddView(c2w, modelCams[i]->GetDistance(), modelCams[i]->GetCameraName());
	}	
	
	pdfHandler->Export3DPDF(m_fileName.c_str(), prcFileName.c_str());
	delete pdfHandler;
	delete m_file;
	if (!DeleteFileA(prcFileName.c_str()))
	{
		fcloseall();
		DeleteFileA(prcFileName.c_str());
	}

	
	//WritePDF(m_fileName, prcFileName);
}
void PdfIO::WritePDF(std::string filepdf, std::string inPrc)
{
	std::ifstream input(inPrc, std::ios::binary);
	std::ofstream output(filepdf, std::ios::binary);

	if (input)
	{
	}
	else
	{
		std::cout << "filenot found:";
		return;
	}

	std::string name1 = "%PDF-1.6\n 1 0 obj\n<< /Title (3D Model)\n/Author (Model)\n/Version Number: 1.0.0>>\nendobj";

	std::string name2 = "\n2 0 obj\n<</Type /Catalog /Pages 3 0 R /OpenAction [4 0 R /Fit]/ViewerPreferences << /DisplayDocTitle true>>>>\nendobj";

	std::string name3 = "\n3 0 obj\n<</Type /Pages /Count 1 /Kids [ 4 0 R ]>>\nendobj";

	std::string name4 = "\n4 0 obj<</Type /Page/Annots [ 9 0 R ] /Contents 8 0 R /MediaBox [0 0 1000 600] /Parent 3 0 R /Resources<</ProcSet[/PDF/Text/ImageB/ImageC/ImageI]/XObject<</XOb4 5 0 R>>>>>>endobj";

	std::string name5 = "\n5 0 obj<</Type/XObject/BBox[10 10 990 590]/Length 40/Resources<</Pattern<</Sh11 6 0 R>>/ProcSet[/PDF/Text/ImageB/ImageC/ImageI]>>/Subtype/Form>>\nstream\n/Pattern cs /Sh11 scn\n10 10 990 590 re\n f\nendstream\nendobj";

	std::string name6 = "\n6 0 obj<</Type/Pattern/PatternType 2/Shading<</ColorSpace/DeviceRGB/Coords[990 590 990 0]/Extend[ true true]/Function 7 0 R/ShadingType 2>>>>\nendobj";

	std::string name7 = "\n7 0 obj<</C0[0.00 0.27 0.27]/C1[0.00 0.59 0.59]/Domain[0 1]/FunctionType 2/N 1>>\nendobj";

	std::string name8 = "\n8 0 obj<</Length 10>>\n stream\n/XOb4 Do\nQ\n endstream\n endobj";

	std::string name9 = "\n9 0 obj\n<</Type /Annot /Subtype /3D /3DD 10 0 R /P 4 0 R /3DV (Default) /3DA <</A/PO/AIS/L/DIS/I/NP false/TB true/Transparent true>>/Rect [10 10 990 590]>>\nendobj";

	std::string name10 = "\n\n10 0 obj\n<< /VA [ 11 0 R ] /Type /3D /Subtype /PRC /Filter [] /Length 26325 >>\nstream\n";

	output << name1;
	output << name2;
	output << name3;
	output << name4;
	output << name5;
	output << name6;
	output << name7;
	output << name8;
	output << name9;
	output << name10;

	std::copy(
		std::istreambuf_iterator<char>(input),
		std::istreambuf_iterator<char>(),
		std::ostreambuf_iterator<char>(output));

	std::string name11 = "\nendstream\nendobj";

	std::string name12 = "\n% Default view for PDF\n11 0 obj\n<< /Type /3DView /XN (Default) /IN (Default) /MS /M /C2W [-0.71 -0.01 0.70 -0.42 0.80 -0.42 -0.56 -0.60 -0.57 60.90 58.64 47.48] /P << /Subtype /O /OS 16.64 >> /CO 81.81 /BG << /Type /3DBG /Subtype /SC /CS /DeviceRGB /C [1 1 1] >> /LS << /Type /3DLightingScheme /Subtype /CAD >> >>\nendobj";

	std::string name13 = "\nxref \n 0 12\n0000000000 65535 f\n0000000010 00000 n\n0000000321 00000 n\n0000000439 00000 n\n0000000497 00000 n\n0000000677 00000 n\n0000000888 00000 n\n0000001038 00000 n\n0000001127 00000 n\n0000001185 00000 n\n0000001349 00000 n\n0000027798 00000 n\ntrailer\n<< /Size 12 /Root 2 0 R>>\n\nstartxref\n28123\n%%EOF";

	output << name11;
	output << name12;
	output << name13;
	input.close();
	output.close();
	
	DeleteFileA(inPrc.c_str());
	
}