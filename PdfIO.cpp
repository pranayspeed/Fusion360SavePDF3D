// ColloadaIO.cpp : Defines the initialization routines for the DLL.
//

#pragma once
#include "stdafx.h"
#include "PdfIO.h"

#include "oPRCFile.h"

#include <iostream>
#include <iomanip>
#include <fstream>
#include <cmath>
#include <string>

#include <iterator>
#include <algorithm>


void TesseKetorTraverser::ModelStart(TesseKetor::Model_tk* model){}
void TesseKetorTraverser::ModelEnd(TesseKetor::Model_tk* model){}
void TesseKetorTraverser::PartStart(TesseKetor::Part_tk* model){}
void TesseKetorTraverser::PartEnd(TesseKetor::Part_tk* model){}
void TesseKetorTraverser::BodyStart(TesseKetor::Body_tk* model){}

TesseKetorTraverser::TesseKetorTraverser(TesseKetor::Model_tk* model) :m_model(model){}
TesseKetorTraverser::~TesseKetorTraverser(){}

void TesseKetorTraverser::Export(TesseKetor::Model_tk* model = NULL)
{
	if (!model)
		model = m_model;
	TesseKetor::Model_tk* tmp = model;

	while (tmp != NULL)
	{
		ModelStart(tmp);
		Part_tk* parts = tmp->GetPart();
		while (parts != NULL)
		{
			TraversePart(parts);
			parts = parts->GetNext();
		}
		TesseKetor::Model_tk* tmpChild = tmp->GetChildModel();
		if (tmpChild)
			Export(tmpChild);
		tmp = tmp->GetNext();
		ModelEnd(tmp);
	}
}


void TesseKetorTraverser::TraversePart(TesseKetor::Part_tk* part)
{
	PartStart(part);
	TesseKetor::Body_tk* body = part->GetBodies();
	while (body != NULL)
	{
		BodyStart(body);
		body = body->GetNext();
	}
	PartEnd(part);
}



PDFExporter::PDFExporter(oPRCFile* file, TesseKetor::Model_tk* model) : m_file(file), TesseKetorTraverser(model)
{

	grpopt.no_break = true;
	grpopt.do_break = false;
	grpopt.tess = true;
	grpopt.closed = true;


	m_imD = NULL;
	FILE* filein = NULL;
	std::ifstream imageFile("D:\\Test.png", std::ios::binary);
	imageFile.seekg(0, std::ios::end);

	sizeImage = imageFile.tellg();
	imageFile.seekg(0, std::ios::beg);
	char* imgData = new char[sizeImage];
	int i = 0;
	while (imageFile.good())
	{
		imageFile.read(&imgData[i], 1);
		i++;
	}
	imageFile.close();
	m_imD = (uint8_t*)imgData;
}
PDFExporter::~PDFExporter()
{

}
void PDFExporter::ExportMatrix(std::vector<float> matrix)
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
void PDFExporter::ModelStart(TesseKetor::Model_tk* model)
{
	double t[4][4];
	t[0][0] = 1; t[0][1] = 0; t[0][2] = 0; t[0][3] = 0;
	t[1][0] = 0; t[1][1] = 1; t[1][2] = 0; t[1][3] = 0;
	t[2][0] = 0; t[2][1] = 0; t[2][2] = 1; t[2][3] = 0;
	t[3][0] = 0; t[3][1] = 0; t[3][2] = 0; t[3][3] = 1;
	m_file->begingroup(model->GetName().c_str(), &grpopt, (double *)t);
}
void PDFExporter::ModelEnd(TesseKetor::Model_tk* model)
{
	m_file->endgroup();
}
void PDFExporter::PartStart(TesseKetor::Part_tk* part)
{
	double t[4][4];
	t[0][0] = 1; t[0][1] = 0; t[0][2] = 0; t[0][3] = 0;
	t[1][0] = 0; t[1][1] = 1; t[1][2] = 0; t[1][3] = 0;
	t[2][0] = 0; t[2][1] = 0; t[2][2] = 1; t[2][3] = 0;
	t[3][0] = 0; t[3][1] = 0; t[3][2] = 0; t[3][3] = 1;
	m_file->begingroup(part->GetName().c_str(), &grpopt, (double *)t);
}
void PDFExporter::PartEnd(TesseKetor::Part_tk* part)
{
	m_file->endgroup();
}
void PDFExporter::BodyStart(TesseKetor::Body_tk* body)
{
	const size_t nP = 3;
	double P[nP][3] = { { 0, 0, 0 }, { 1, 0, 0 }, { 1, 1, 0 } };
	const size_t nI = 2;
	uint32_t PI[nI][3] = { { 0, 1, 2 }, { 2, 0, 1 } };


	double t[4][4];
	t[0][0] = 1; t[0][1] = 0; t[0][2] = 0; t[0][3] = 0;
	t[1][0] = 0; t[1][1] = 1; t[1][2] = 0; t[1][3] = 0;
	t[2][0] = 0; t[2][1] = 0; t[2][2] = 1; t[2][3] = 6;
	t[3][0] = 0; t[3][1] = 0; t[3][2] = 0; t[3][3] = 1;

	//const uint32_t nT = 4;
	//const double T[nT][2] = { { 0.1, 0.1 }, { 0.9, 0.1 }, { 0.9, 0.9 }, { 0.1, 0.9 } };
	//uint32_t TI[nI][3] = { { 0, 1, 3 }, { 1, 2, 3 } };

	double* rgb = body->GetRGB();
	PRCmaterial materialBase(
		RGBAColour(rgb[0], rgb[1], rgb[2], 1),
		RGBAColour(1, 1, 1, 1),
		RGBAColour(0.1, 0.1, 0.1, 1),
		RGBAColour(0.1, 0.1, 0.1, 1),
		1.0, 0.1,
		m_imD, KEPRCPicture_PNG, 2, 2, sizeImage, true, false);
	TesseKetor::Face_tk* face = body->GetFaces();
	std::vector<double> vertexList, normalList;
	std::vector<double> faceList;
	int faceIndex = 0;
	while (face)
	{
		std::vector<double> tmpVList;
		vertexList.insert(vertexList.end(), face->GetVertices(), face->GetVertices() + (face->GetVertexCount() * 3));
		normalList.insert(normalList.end(), face->GetNormals(), face->GetNormals() + (face->GetNormalCount() * 3));
		faceList.push_back(faceIndex++);
		faceList.push_back(faceIndex++);
		faceList.push_back(faceIndex++);
		face = face->GetNext();
	}

	const size_t nP = vertexList.size() / 3;
	double P[nP][3];

	memcpy(P, &vertexList[0], nP);
	const size_t nI = faceList.size();
	uint32_t PI[nI][3];

	memcpy(PI, &faceList[0], nI);

	m_file->begingroup(body->GetName().c_str(), &grpopt, (double *)t);
	//m_file->addTriangles(nP, P, nI, PI, materialBase, 0, NULL, NULL, nT, T, TI, 0, NULL, NULL, 0, NULL, NULL, NULL);
	m_file->addTriangles(nP, P, nI, PI, materialBase, 0, NULL, NULL, 0, 0, 0, 0, NULL, NULL, 0, NULL, NULL, NULL);
	m_file->endgroup();

}

PdfIO::PdfIO(TesseKetor::Model_tk* model, std::string fileName) :m_model(model)
{
	m_fileName = fileName;
	if (m_fileName.size() == 0)
		m_fileName = "D:\\test.pdf";
	m_file = new oPRCFile(m_fileName);

}
PdfIO::~PdfIO()
{
	delete m_file;
}

void PdfIO::Export()
{
	//PDFExporter* pdfExporter = new PDFExporter(m_file, m_model);
	//pdfExporter->Export();
	//delete pdfExporter;

	WritePDF(m_fileName);
}
void PdfIO::WritePDF(std::string fileName)
{
	std::ifstream input(fileName, std::ios::binary);
	std::ofstream output("D:\\testasy13.pdf", std::ios::binary);

	if (input)
	{
	}
	else
	{
		std::cout << "filenot found:";
		return;
	}

	std::string name1 = "%PDF-1.6\n 1 0 obj\n<< /Title (3D Model)\n/Author (TesseKetor)\n/Version Number: 1.0.0>>\nendobj";

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
}


