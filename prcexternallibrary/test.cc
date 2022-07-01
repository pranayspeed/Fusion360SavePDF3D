/************
*
*   This file is part of a tool for producing 3D content in the PRC format.
*   Copyright (C) 2008  Orest Shardt <shardtor (at) gmail dot com>
*
*   This program is free software: you can redistribute it and/or modify
*   it under the terms of the GNU Lesser General Public License as published by
*   the Free Software Foundation, either version 3 of the License, or
*   (at your option) any later version.
*
*   This program is distributed in the hope that it will be useful,
*   but WITHOUT ANY WARRANTY; without even the implied warranty of
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*   GNU Lesser General Public License for more details.
*
*   You should have received a copy of the GNU Lesser General Public License
*   along with this program.  If not, see <http://www.gnu.org/licenses/>.
*
*************/

#include <iostream>
#include <iomanip>
#include <fstream>
#include <cmath>
#include <string>
#include "oPRCFile.h"

using namespace std;

extern const double pi;
void writePDF();
int main()
{
  // List of pictures used; keep track of memory allocated to free it in the end
  // shared pointers or garbage collector may be an alternative
  oPRCFile file("D:\\testasy.prc");

  if(1){
    PRCoptions grpopt;
    grpopt.no_break = true;
    grpopt.do_break = false;
    grpopt.tess = true;
    grpopt.closed = true;
	
    if(1){

		FILE* filein = NULL;
		std::ifstream imageFile("D:\\Test1.png", std::ios::binary);
		imageFile.seekg(0, std::ios::end);

		uint32_t sizeImage = imageFile.tellg();
		imageFile.seekg(0, std::ios::beg);
		char* imgData = new char[sizeImage];
		int i = 0;
		while (imageFile.good())
		{
			imageFile.read(&imgData[i], 1);
			i++;
		}
		const uint8_t* imD = (uint8_t*)imgData;
// triangle points
		const size_t nP = 3;
		double P[nP][3] = { { 0, 0, 0 }, { 1, 0, 0 }, { 1, 1, 0 } };
		const size_t nI = 2;
		uint32_t PI[nI][3] = { { 0, 1, 2 }, { 2, 0, 1 } };
		

		double t[4][4];
    t[0][0]=1; t[0][1]=0; t[0][2]=0; t[0][3]=0;
    t[1][0]=0; t[1][1]=1; t[1][2]=0; t[1][3]=0;
    t[2][0]=0; t[2][1]=0; t[2][2]=1; t[2][3]=6;
    t[3][0]=0; t[3][1]=0; t[3][2]=0; t[3][3]=1;
   
    const uint32_t nT = 4;
    const double T[nT][2] = { {0.1,0.1}, {0.9,0.1}, {0.9,0.9}, {0.1,0.9} };
    uint32_t TI[nI][3] = {{0,1,3},{1,2,3}};

    PRCmaterial materialBase(
      RGBAColour(0.1,0.1,0.1,1),
      RGBAColour(1,1,1,1),
      RGBAColour(0.1,0.1,0.1,1),
      RGBAColour(0.1,0.1,0.1,1),
      1.0,0.1,
	  imD, KEPRCPicture_PNG, 2, 2, sizeImage, true, false);
    file.begingroup("triangles_rgb_texture",&grpopt,(double *)t);
    file.addTriangles(nP, P, nI, PI, materialBase, 0, NULL, NULL, nT, T, TI, 0, NULL, NULL, 0, NULL, NULL, NULL);
	
    file.endgroup();
	
    }

  }

  file.finish();


  writePDF();

  return 0;
}
#include <fstream>
#include <iterator>
#include <algorithm>
#include <iostream>
#include <string>

void writePDF()
{
	std::ifstream input("D:\\testasy.prc", std::ios::binary);
	std::ofstream output("D:\\testasy13.pdf", std::ios::binary);

	if (input)
	{
	}
	else
	{
		std::cout << "filenot found:";
		return ;
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

}