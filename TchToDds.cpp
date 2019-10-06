#ifdef _WIN32
#define NOMINMAX
#include <Windows.h>
#endif
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <vector>
#include <algorithm>

#ifndef _WIN32
typedef unsigned int DWORD;
typedef unsigned short WORD;
#define strcmpi strcasecmp
#endif

#ifndef _MSC_VER
#ifndef __forceinline
#define __forceinline inline
#endif
#endif

struct DDS_PIXELFORMAT
{
	DWORD dwSize;
	DWORD dwFlags;
	DWORD dwFourCC;
	DWORD dwRGBBitCount;
	DWORD dwRBitMask;
	DWORD dwGBitMask;
	DWORD dwBBitMask;
	DWORD dwABitMask;
};

typedef struct
{
	DWORD           dwSize;
	DWORD           dwFlags;
	DWORD           dwHeight;
	DWORD           dwWidth;
	DWORD           dwPitchOrLinearSize;
	DWORD           dwDepth;
	DWORD           dwMipMapCount;
	DWORD           dwReserved1[11];
	DDS_PIXELFORMAT ddspf;
	DWORD           dwCaps;
	DWORD           dwCaps2;
	DWORD           dwCaps3;
	DWORD           dwCaps4;
	DWORD           dwReserved2;
} DDS_HEADER;

//These constants are taken from http://www.mindcontrol.org/~hplus/graphics/dds-info/

#define DDS_MAGIC 0x20534444

//  DDS_header.dwFlags
#define DDSD_CAPS                   0x00000001 
#define DDSD_HEIGHT                 0x00000002 
#define DDSD_WIDTH                  0x00000004 
#define DDSD_PITCH                  0x00000008 
#define DDSD_PIXELFORMAT            0x00001000 
#define DDSD_MIPMAPCOUNT            0x00020000 
#define DDSD_LINEARSIZE             0x00080000 
#define DDSD_DEPTH                  0x00800000 

//  DDS_header.sPixelFormat.dwFlags
#define DDPF_ALPHAPIXELS            0x00000001 
#define DDPF_FOURCC                 0x00000004 
#define DDPF_INDEXED                0x00000020 
#define DDPF_RGB                    0x00000040 

//  DDS_header.sCaps.dwCaps1
#define DDSCAPS_COMPLEX             0x00000008 
#define DDSCAPS_TEXTURE             0x00001000 
#define DDSCAPS_MIPMAP              0x00400000 

//  DDS_header.sCaps.dwCaps2
#define DDSCAPS2_CUBEMAP            0x00000200 
#define DDSCAPS2_CUBEMAP_POSITIVEX  0x00000400 
#define DDSCAPS2_CUBEMAP_NEGATIVEX  0x00000800 
#define DDSCAPS2_CUBEMAP_POSITIVEY  0x00001000 
#define DDSCAPS2_CUBEMAP_NEGATIVEY  0x00002000 
#define DDSCAPS2_CUBEMAP_POSITIVEZ  0x00004000 
#define DDSCAPS2_CUBEMAP_NEGATIVEZ  0x00008000 
#define DDSCAPS2_VOLUME             0x00200000 

#define D3DFMT_DXT1     '1TXD'    //  DXT1 compression texture format 
#define D3DFMT_DXT2     '2TXD'    //  DXT2 compression texture format 
#define D3DFMT_DXT3     '3TXD'    //  DXT3 compression texture format 
#define D3DFMT_DXT4     '4TXD'    //  DXT4 compression texture format 
#define D3DFMT_DXT5     '5TXD'    //  DXT5 compression texture format 
#define D3DFMT_BC4      0x00344342   //  D3DFMT_BC5 texture format 
#define D3DFMT_BC4U     'U4CB'   //  D3DFMT_BC5U texture format 
#define D3DFMT_BC5      0x00354342   //  D3DFMT_BC5 texture format 
#define D3DFMT_BC5U     'U5CB'   //  D3DFMT_BC5U texture format 

__forceinline  DWORD GetDivSize(DWORD dwFourCC)
{
	switch (dwFourCC)
	{
	case D3DFMT_DXT1:
	case D3DFMT_DXT3:
	case D3DFMT_DXT5:
		return 4;
	default:
		return -1;
	}
}

__forceinline DWORD GetBlockBytes(DWORD dwFourCC)
{
	switch (dwFourCC)
	{
	case D3DFMT_DXT1:
		return 8;
	case D3DFMT_DXT3:
	case D3DFMT_DXT5:
		return 16;
	default:
		return -1;
	}
}

__forceinline DWORD CalculateLinearSize(DWORD dwFourCC, DWORD dwHeight, DWORD dwWidth)
{
	DWORD dwDivSize = GetDivSize(dwFourCC);
	DWORD dwBlockBytes = GetBlockBytes(dwFourCC);
	return std::max(dwDivSize, dwWidth) / dwDivSize * std::max(dwDivSize, dwHeight) / dwDivSize * dwBlockBytes;
}

int main(int argc, char** argv)
{
	char *pszInput = nullptr, *pszOutput = nullptr, *pszDonor = nullptr;
	int iUserMipMapCount = 0;
	for (int i = 1; i < argc; i++)
	{
		if (strcmpi(argv[i], "-tch") == 0) pszInput = argv[++i];
		else if (strcmpi(argv[i], "-o") == 0) pszOutput = argv[++i];
		else if (strcmpi(argv[i], "-dds") == 0) pszOutput = argv[++i];
		else if (strcmpi(argv[i], "-d") == 0) pszDonor = argv[++i];
		else if (strcmpi(argv[i], "-mipmapcount") == 0) iUserMipMapCount = atoi(argv[++i]);
		else if (strcmpi(argv[i], "-h") == 0)
		{
			printf("Hello! It is shitty .tch -> .dds convertor, version #1\n");
			printf("It supports only DXT1, DXT3, DXT5, BC5, BC4, because it's shitty\n");
			printf("The program do not check file existence, because it's shitty\n");
			printf("If you want convert nor DXT1 nor DXT3 nor DXT5 nor BC5 nor BC4, you must use donor-dds\n");
			printf("Maybe, I will write later plugin for some kind of graphics editor\n");
			printf("Command line options:\n");
			printf(" -tch <tch-file-as-input>\n");
			printf(" -o <dds-file-as-input>\n");
			printf(" -dds <dds-file-as-input> /*equal to -o <dds-file-as-input>*/\n");
			printf(" -h /* shows help */\n");
			printf(" -mipmapcount <number> /* set mip map count to number */\n");
			printf(" -d <dds-file-as-donor> /* uses this dds file for dds header*/\n");
		}
	}

	if (!pszInput)
	{
		printf("NO INPUT\n");
		return EXIT_FAILURE;
	}

	const bool noUserDefinedOutput = pszOutput == nullptr;
	if (noUserDefinedOutput)
	{
		pszOutput = new char[strlen(pszInput) + 4 + 1];
		strcpy(pszOutput, pszInput);
		strcat(pszOutput, ".dds");
		printf("No user defined output\n");
		printf("We shall use '%s' as output\n", pszOutput);
	}

	FILE* fTch = fopen(pszInput, "rb");

	int iVersion, iMagic1, iMagic2;
	fread(&iVersion, sizeof(int), 1, fTch);
	fread(&iMagic1, sizeof(int), 1, fTch);
	fread(&iMagic2, sizeof(int), 1, fTch);

	char aMeta[92];
	fread(aMeta, sizeof(char), 92, fTch);
	std::vector<char> aContent;

	while (!feof(fTch))
	{
		char c;
		fread(&c, sizeof(char), 1, fTch);
		aContent.push_back(c);
	}

	fclose(fTch);

	DWORD dwMagic = 0x20534444;
	DDS_HEADER header;
	memset(&header, 0, sizeof(DDS_HEADER));
	header.dwSize = 124;
	header.ddspf.dwSize = sizeof(DDS_PIXELFORMAT);
	memcpy(&header.ddspf.dwFourCC, aMeta + 4, sizeof(DWORD));
	WORD wHeight, wWidth;
	memcpy(&wHeight, aMeta + 0x58 - 0xc, sizeof(WORD));
	memcpy(&wWidth, aMeta + 0x5a - 0xc, sizeof(WORD));
	header.dwHeight = wHeight;
	header.dwWidth = wWidth;

	switch (header.ddspf.dwFourCC)
	{
	case D3DFMT_DXT1:
	case D3DFMT_DXT3:
	case D3DFMT_DXT5:
		header.dwFlags |= DDSD_CAPS | DDSD_LINEARSIZE | DDSD_HEIGHT | DDSD_WIDTH | DDSD_MIPMAPCOUNT | DDSD_PIXELFORMAT;
		header.ddspf.dwFlags |= DDPF_FOURCC;
		header.dwCaps |= DDSCAPS_TEXTURE | DDSCAPS_COMPLEX | DDSCAPS_MIPMAP;
		header.dwPitchOrLinearSize = CalculateLinearSize(header.ddspf.dwFourCC, header.dwHeight, header.dwWidth);
		break;
	case D3DFMT_BC5:
		header.ddspf.dwFourCC = D3DFMT_BC5U;
		header.dwFlags |= DDSD_CAPS | DDSD_LINEARSIZE | DDSD_HEIGHT | DDSD_WIDTH | DDSD_MIPMAPCOUNT | DDSD_PIXELFORMAT;
		header.ddspf.dwFlags |= DDPF_FOURCC;
		header.dwCaps |= DDSCAPS_TEXTURE | DDSCAPS_COMPLEX | DDSCAPS_MIPMAP;
		header.dwPitchOrLinearSize = header.dwHeight * header.dwWidth;
		break;
	case D3DFMT_BC4:
		header.ddspf.dwFourCC = D3DFMT_BC4U;
		header.dwFlags |= DDSD_CAPS | DDSD_LINEARSIZE | DDSD_HEIGHT | DDSD_WIDTH | DDSD_MIPMAPCOUNT | DDSD_PIXELFORMAT;
		header.ddspf.dwFlags |= DDPF_FOURCC;
		header.dwCaps |= DDSCAPS_TEXTURE | DDSCAPS_COMPLEX | DDSCAPS_MIPMAP;
		header.dwPitchOrLinearSize = (header.dwHeight * header.dwWidth) >> 1;
		break;
	}

	header.dwMipMapCount = (DWORD)iUserMipMapCount; 
  
	if (pszDonor != nullptr)
	{
		FILE* fRealDds = fopen(pszDonor, "rb");
		fread(&dwMagic, sizeof(DWORD), 1, fRealDds);
		fread(&header, sizeof(DDS_HEADER), 1, fRealDds);
		fclose(fRealDds);
	}

	FILE* fDds = fopen(pszOutput, "wb");
	fwrite(&dwMagic, 4, 1, fDds);
	fwrite(&header, sizeof(DDS_HEADER), 1, fDds);
	fwrite(aContent.data(), 1, aContent.size(), fDds);
	fclose(fDds);

	if (noUserDefinedOutput) delete[]pszOutput;

	return EXIT_SUCCESS;
}
