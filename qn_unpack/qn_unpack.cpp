// qn_unpack.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <stdlib.h>
#include <string.h>
#include <string>
#include <windows.h>
#include "bzlib.h"
#pragma comment(lib, "libbz2.lib")

typedef size_t uint32;
typedef size_t uint;
typedef unsigned char uint8;
typedef int int32;

const uint8* GetPackKey()
{
	//异或加密的KEY
	static const uint8 ls_PackKey[16] =
	{ 0xEB, 0xC2,0xDC,0xC3,0xC4,0xB5,0xD2,0xCA,
		0xF7,0xD7,0xA4,0xB9,0xF0,0xBB,0xD7,0xC0 };

	return ls_PackKey;
}

void XorBuf(char* buf)
{
	const uint8* key = GetPackKey();

	for (int i = 0; i < 1024; i += 16)
		for (int j = 0; j < 16; j++)
			buf[i + j] = buf[i + j] ^ key[j];
}

inline static void BufferXor(char * buffer, uint32 uPosition, size_t uBytes)
{
	const uint8* szPackKey = GetPackKey();

	for (unsigned i = 0; i < uBytes; i++)
		buffer[i] = buffer[i] ^ szPackKey[(uPosition + i) & 0xf];
}

// 为了适应将字符串转化为唯一的数字ID，调整字符串
// 如果字符大写，就转化为小写，将'\\'转化为'/'
static char* StringAdjust(char* p)
{
	for (int i = 0; p[i]; i++)
	{
		if (p[i] >= 'A' && p[i] <= 'Z')
			p[i] += 'a' - 'A';
		else if (p[i] == '/')
			p[i] = '\\';
	}
	return p;
}

#define mix(a,b,c) \
			{ \
				a -= b; a -= c; a ^= (c>>13); \
				b -= c; b -= a; b ^= (a<<8); \
				c -= a; c -= b; c ^= (b>>13); \
				a -= b; a -= c; a ^= (c>>12);  \
				b -= c; b -= a; b ^= (a<<16); \
				c -= a; c -= b; c ^= (b>>5); \
				a -= b; a -= c; a ^= (c>>3);  \
				b -= c; b -= a; b ^= (a<<10); \
				c -= a; c -= b; c ^= (b>>15); \
			}

inline uint32 hash(
	register uint8 *k,        /* the key */
	register uint32  length,   /* the length of the key */
	register uint32  initval    /* the previous hash, or an arbitrary value */
)
{
	register uint32 a, b, c, len;

	/* Set up the internal state */
	len = length;
	a = b = 0x9e3779b9;  /* the golden ratio; an arbitrary value */
	c = initval;           /* the previous hash value */

						   /*---------------------------------------- handle most of the key */
	while (len >= 12)
	{
		a += (k[0] + ((uint32)k[1] << 8) + ((uint32)k[2] << 16) + ((uint32)k[3] << 24));
		b += (k[4] + ((uint32)k[5] << 8) + ((uint32)k[6] << 16) + ((uint32)k[7] << 24));
		c += (k[8] + ((uint32)k[9] << 8) + ((uint32)k[10] << 16) + ((uint32)k[11] << 24));
		mix(a, b, c);
		k += 12;
		len -= 12;
	}

	/*------------------------------------- handle the last 11 bytes */
	c += length;
	switch (len)             /* all the case statements fall through */
	{
	case 11:
		c += ((uint32)k[10] << 24);
	case 10:
		c += ((uint32)k[9] << 16);
	case 9:
		c += ((uint32)k[8] << 8);
		/* the first byte of c is reserved for the length */
	case 8:
		b += ((uint32)k[7] << 24);
	case 7:
		b += ((uint32)k[6] << 16);
	case 6:
		b += ((uint32)k[5] << 8);
	case 5:
		b += k[4];
	case 4:
		a += ((uint32)k[3] << 24);
	case 3:
		a += ((uint32)k[2] << 16);
	case 2:
		a += ((uint32)k[1] << 8);
	case 1:
		a += k[0];
		/* case 0: nothing left to add */
	}
	mix(a, b, c);
	/*-------------------------------------------- report the result */
	return c;
}

uint32 String2ID(const char* str)
{
	if (str == NULL)
		return 0;
	return hash((uint8*)str, (uint32)strlen(str), 0);
}

unsigned int FileName2Id(const char *filename)
{
	char buffer[256];
	buffer[255] = 0;
	strncpy(buffer, filename, 255);
	return (unsigned int)String2ID(StringAdjust(buffer));
}

#pragma  pack (push,1) 
//文件索引
struct PackFileIndex
{
	//ID
	uint32		uid;
	//文件偏移
	uint		offset;
	//文件尺寸
	uint		size;

	uint		space;
	//校验和
	uint32		check_code;

	PackFileIndex()
		: uid(0)
		, offset(0)
		, size(0)
		, space(0)
		, check_code(0)
	{
	}
};

//文件头
struct PackageHead
{
	//ID
	uint		id;
	//文件是否使用异或加密
	uint		bXor;
	//当前包中文件的数量
	int			number;
	//文件索引的偏移
	uint		offset;
	//包版本
	uint32		ver;

	PackageHead()
		: id('XWDF')
		, bXor(0)
		, number(0)
		, offset(0)
		, ver(0)
	{
	}
};
#pragma pack(pop) 
int main()
{
#pragma region regionName
	//std::string szFileName = "C:/script/test1/test2_c.ams";
	//// 从源文件路径中去掉包含包名的路径，使用相对路径计算uid
	//std::string temp = "script.pkg";
	//temp = temp.substr(0, temp.size() - 4) + "/";

	////int32  pos  = szFileName.find(temp) + temp.size();

	//std::string fullfolder = "/" + temp;
	//int32 pos = szFileName.rfind(fullfolder);
	//if (pos == std::string::npos)
	//{
	//	if (szFileName.substr(0, temp.size()) == temp)
	//	{
	//		pos = temp.size();
	//	}
	//	else
	//	{
	//		//cerr << (string("路径") + szFileName + "中不存在" + fullfolder);
	//	}
	//}
	//else
	//{
	//	pos += fullfolder.size();
	//}

	//szFileName = szFileName.substr(pos, szFileName.size());

	//uint32 id = FileName2Id(szFileName.c_str());
	//printf("%d\n", id);
#pragma endregion regionName
	FILE* pFile = fopen("D:\\tile.wdf", "rb");
	PackageHead fileHead;
	fread(&fileHead, sizeof(PackageHead), 1, pFile);

	printf("filehead:\nID:%d num:%d offset:%d xor:%d ver:%d\n", fileHead.id, fileHead.number,
		fileHead.offset, fileHead.bXor, fileHead.ver);
	PackFileIndex fileIndex;
	char fileBuffer[10000000];
	for (size_t i = 0; i < fileHead.number; i++)
	{
		//移动文件指针到i个索引文件并读取
		fseek(pFile, fileHead.offset + i * sizeof(fileIndex), SEEK_SET);
		fread(&fileIndex, sizeof(fileIndex), 1, pFile);

		//printf("%d\n", fileIndex.uid, fileIndex.offset, fileIndex.size, fileIndex.space,
		//	fileIndex.check_code);

		//根据文件索引读取文件
		memset(fileBuffer, 0, sizeof(fileBuffer));
		fseek(pFile, fileIndex.offset, SEEK_SET);
		fread(fileBuffer, 1, fileIndex.size, pFile);

		//准备解压缓冲区
		static char decompressBuffer[10000000] = "";
		memset(decompressBuffer, 0, sizeof(decompressBuffer));

		//使用bzip2解压文件
		unsigned int len = sizeof(decompressBuffer);
		int decompressOK = BZ2_bzBuffToBuffDecompress(decompressBuffer, &len, fileBuffer,
			fileIndex.size, 0, 0);
		if (decompressOK == 0)
		{
			//异或解密
			BufferXor(decompressBuffer, 0, len);

			//检测文件后缀

			char fileExt[10] = "";
			if (strncmp(decompressBuffer + 1, "Lua", 3) == 0)
			{
				sprintf(fileExt, "%s", "Lua");
			}
			else if (strncmp(decompressBuffer, "TEX", 3) == 0)
			{
				sprintf(fileExt, "%s", "TEX");
			}
			else if (strncmp(decompressBuffer+2, "rtf", 3) == 0)
			{
				sprintf(fileExt, "%s", "RTF");
			}
			else if (strncmp(decompressBuffer, "ARP", 3) == 0)
			{
				sprintf(fileExt, "%s", "ARP");
			}
			else if (strncmp(decompressBuffer, "RS", 2) == 0)
			{
				sprintf(fileExt, "%s", "RS");
			}
			else if (strncmp(decompressBuffer, "ANF", 3) == 0)
			{
				sprintf(fileExt, "%s", "ANF");
			}
			else if (strncmp(decompressBuffer, "ARA", 3) == 0)
			{
				sprintf(fileExt, "%s", "ARA");
			}
			else if (strncmp(decompressBuffer, "ARE", 3) == 0)
			{
				sprintf(fileExt, "%s", "ARE");
			}
			else if (strncmp(decompressBuffer, "MSH", 3) == 0)
			{
				sprintf(fileExt, "%s", "MSH");
			}
			else if (strncmp(decompressBuffer+1, "BPS", 3) == 0)
			{
				sprintf(fileExt, "%s", "BPS");
			}
			else if (strncmp(decompressBuffer, "EXIF", 4) == 0)
			{
				sprintf(fileExt, "%s", "EXIF");
			}
			else if (strncmp(decompressBuffer, "REGN", 4) == 0)
			{
				sprintf(fileExt, "%s", "REGN");
			}
			else if (strncmp(decompressBuffer, "AMAP", 4) == 0)
			{
				sprintf(fileExt, "%s", "AMAP");
			}
			else if (strncmp(decompressBuffer, "<x>", 3) == 0)
			{
				sprintf(fileExt, "%s", "x");
			}
			else if (strncmp(decompressBuffer, "ACF", 3) == 0)
			{
				sprintf(fileExt, "%s", "ACF");
			}
			else
			{
				printf("unknown\n");
			}

			//确定文件名
			char fileName[255] = "";
			sprintf(fileName, "D:\\qn\\tile\\%d.%s", fileIndex.uid, fileExt);

			//写入到文件
			FILE* unPackFile = fopen(fileName, "wb+");
			if (unPackFile)
			{
				fwrite(decompressBuffer, 1, len, unPackFile);
				fclose(unPackFile);
			}
		}
		else 
		{
			printf("提取文件失败！文件ID %d。\n", fileIndex.uid);
			system("pause");
			//XorBuf(fileBuffer);
			//fwrite(fileBuffer, 1, fileIndex.size, unPackFile);
		}
	}
	fclose(pFile);
	return 0;
}

