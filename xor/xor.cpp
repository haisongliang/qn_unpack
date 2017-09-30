//用一个密钥(字符串)加密另一个字符串,密钥是循环使用,加密方式为异或.现在要从密文中破解密钥来实现破解.
//这里假设密钥是由小写字母构成的组合,其他情况也和这类似
#include<iostream>
#include<fstream>
#include<cmath>
#include<string>

using namespace std;

int my_strlen(const char * str) {

	const char *p = str;
	while (*p++);
	return((int)(p - str - 1));
}

int function(int c1, int c2) {
	return c1^c2;
}

void JiaMi(const char* src, char * des, const char *key) {
	ofstream out("JiaMi.txt");
	int lengthOfKey = my_strlen(key);
	for (int i = 0; i<my_strlen(src); i++) {
		*(des + i) = char(function(*(src + i), *(key + i%lengthOfKey)));
		out << (int)*(des + i) << "  ";

	}
	out.close();
	cout << endl;
}

void JiaMi2(const char* src, char * des, const char *key) {
	ofstream out("JiaMi.txt");

	for (int i = 0; i < my_strlen(src); i++) {
		out << (int)*(src + i) << "  ";
	}
	out.close();
	cout << endl;
}

void JieMi(const char* src, char * des, const char *key, int Length) {//将src和key或运算
	for (int i = 0; i<Length; i++) {
		*(des + i) = char(function(*(src + i), *(key + i%my_strlen(key))));
	}
	cout << endl;
}


float ratio(int* src, int length) {//求重合指数率的函数
	int TotalOfEquals = 0;
	int total = 0;
	for (int i = 0; i<length; i++) {
		for (int j = i + 1; j<length; j++) {
			total++;
			if (*(src + i) == *(src + j)) {
				TotalOfEquals++;
			}
		}
	}
	return float((float)TotalOfEquals / (float)total);
}

void JieMiWithoutKey(int* src, char * des, int Length) {//src是暗文,des存放目标明文,Length是暗文长度
	ofstream jiemi("jiemi.txt");

	//计算一下暗文的重合指数
	int finalNo = 0;//密码长度
	int TotalOfEquals = 0;
	int total = 0;
	for (int i = 0; i<Length; i++) {
		for (int j = i + 1; j<Length; j++) {
			total++;
			if (*(src + i) == *(src + j)) {
				TotalOfEquals++;
			}
		}
	}

	cout << "TotalOfEquals=" << TotalOfEquals << endl;
	cout << "total=" << total << endl;
	cout << "total总数为:" << Length*Length / 2 << endl;
	jiemi << "TotalOfEquals=" << TotalOfEquals << endl;
	jiemi << "total=" << total << endl;
	jiemi << "total总数为:" << Length*Length / 2 << endl;
	cout << "全文重合指数为: " << (float)TotalOfEquals / (float)total << endl;
	jiemi << "全文重合指数为: " << (float)TotalOfEquals / (float)total << endl;

	if (fabs(0.038 - (float)TotalOfEquals / (float)total)<fabs(0.069 - (float)TotalOfEquals / (float)total)) {
		cout << "-----理论上支持这是采用多表加密的方法加密的密文" << endl;//单表解密的接近0.069
		jiemi << "-----理论上支持这是采用多表加密的方法加密的密文" << endl;//单表解密的接近0.069
	}
	else {
		cout << "-----理论上支持这是采用单表加密的方法加密的密文" << endl;
		jiemi << "-----理论上支持这是采用单表加密的方法加密的密文" << endl;
	}

	double ratioArray[18];//存放重合指数率的数组
	double* pratioArray = ratioArray;
	for (int x = 3; x <= 20; x++) {//分别猜测不同密钥长度下的重合指数
		float tempfloat = 0;
		//cout<<"如果密码是"<<x<<"位时,---------------:  "<<endl;
		double averRatio = 0;
		for (int n = 0; n<x; n++) {//针对已假设的密码长度进行重合指数计算
			int index = 0;
			int tempArray[70000];
			int* ptempArray = tempArray;
			int tempTotal = 0;

			for (int m = n; m<Length;) {//这只是一个赋值的过程
				*(ptempArray++) = *(src + m);
				tempTotal++;
				m = m + x;
			}

			//cout<<"tempTotal="<<tempTotal<<endl;
			//在这里要一列数据进行重合指数率的计算

			tempfloat = ratio(tempArray, tempTotal);
			averRatio = averRatio + tempfloat;
		}
		*pratioArray++ = (float)averRatio / x;
	}

	//输出重合指数率
	for (int h = 3; h <= 20; h++) {
		cout << "如果密码是" << h << "位时,重合指数率是:  " << (float)ratioArray[h - 3] << endl;
		jiemi << "如果密码是" << h << "位时,重合指数率是:  " << (float)ratioArray[h - 3] << endl;
	}

	//分析结构
	cout << "=================可能密码位数=====================" << endl;
	jiemi << "=================可能密码位数====================" << endl;

	for (int ma = 0; ma<18; ma++) {
		if (ratioArray[ma]>0.06) {
			cout << ma + 3 << "-----" << ratioArray[ma] << endl;
			jiemi << ma + 3 << "-----" << ratioArray[ma] << endl;
		}
	}

	cout << "==============取这些数的公共约数 最可能是密码位数==============" << endl;
	jiemi << "=============取这些数的公共约数 最可能是密码位数==============" << endl;

	cout << "请输入判断的密码长度:" << endl;
	jiemi << "请输入判断的密码长度:" << endl;
	cin >> finalNo;
	cout << "=====================密码长度为" << finalNo << "时,暴力破解......." << endl;
	jiemi << "=====================密码长度为" << finalNo << "时,暴力破解......." << endl;
	char result[4][10];//用于存放预测的结果
					   //这里假设密码都是小写的英文字符 从97 到122

	for (int mc = 0; mc<finalNo; mc++) {//对密码的每个单位进行破解
		cout << "==================正在破解第================" << mc + 1 << "位..." << endl;
		jiemi << "=================正在破解第===============" << mc + 1 << "位..." << endl;

		for (int md = 0; md <= 255; md++) {//这里是从a到z逐个的试探 用function(int c1,int c2)
			//cout << "---------正在测试----------:" << char(md) << "..." << endl;
			//jiemi << "---------正在测试----------:" << char(md) << "..." << endl;
			int mg = 0;
			int mindex = 0;
			int mf = 0;

			for (int me = mc; me<Length;) {
				mf = function(md, (int)*(src + me));
				if (!(mf == 44 || mf == 46 || mf == 45 || mf == 32 || mf == 33 || mf == 34 || (48 <= mf&&mf <= 59) || (65 <= mf&&mf <= 90) || (97 <= mf&&mf <= 122))) {

					//cout << "临时结果:" << mf << "--" << char(mf) << "不通过" << endl;
					//jiemi << "临时结果:" << mf << "--" << char(mf) << "不通过" << endl;
					mg = 1;
					break;//返回试探下一个
				}
				me = me + finalNo;
			}

			if (mg == 0) {
				cout << "-----------------------------------------" << char(md) << "--pass!!!" << endl;
				jiemi << "-----------------------------------------" << char(md) << "--pass!!!" << endl;
				result[mc][mindex++] = char(md);

			}
		}

	}
	jiemi.close();
}
#include <set>

void main() {

	const unsigned char test_keys[] = { 0xEB, 0xC2,0xDC,0xC3,0xC4,0xB5,
		0xD2,0xCA,0xF7,0xD7,0xA3,0xC0,0xE8,0x8F,0xE0,0xCE };

	//ifstream a("a");
	//ifstream b("b");

	//unsigned char a2, b2;
	//for (int i = 0; i < 200; i++)
	//{
	//	a >> a2;
	//	b >> b2;
	//	//int index = i % 16;
	//	//printf("%c", a2^test_keys[index]);
	//	printf("%02X", a2^b2);
	//}
	FILE* pFile1 = fopen("a", "r");
	FILE* pFile2 = fopen("b", "r");

	unsigned char buf1[1024] = "";
	unsigned char buf2[1024] = "";
	unsigned char buf3[1024] = "";

	fread(&buf1, 200, 1, pFile1);
	fread(&buf2, 200, 1, pFile2);

	//ofstream ooo("c");
	unsigned char a1, b1;
	std::set<unsigned char> keys;
	for (int i=0;i<78;i++)
	{
		unsigned char key = buf1[i] ^ buf2[i];
		printf("%02X", key);
		keys.insert(key);
	}
	printf("\nkey: %d\n", keys.size());
	////cout<<function(2,5);
	//char*src = "There was once a farmer who had a fine olive orchard. He was very hardworking, and the farm always prospered under his care. But he knew that his three sons despised the farm work, and were eager to make wealth, trough adventure.When the farmer was old, and felt that his time had come to die, he called the three sons to him and said, My sons, there is a pot of gold hidden in the olive orchard. Dig for it, if you wish it.After the farmer was dead, the sons went to work to find the pot of gold; since they did not know where the hiding-place was, they agreed to begin in a line, at one end of the orchard, and to dig until one of them should find the money.";
	//cout << "------------------------明文-----------------------" << endl;
	//ofstream Minwen("MingWen.txt");
	//cout << "明文长度:" << my_strlen(src) << endl;

	//for (int iii = 0; iii<my_strlen(src); iii++) {
	//	cout << (int)*(src + iii) << " ";
	//	Minwen << (int)*(src + iii) << "  ";
	//}
	//Minwen.close();
	//char*des = new char[2000];
	//string bb;
	//cout << endl;
	//cout << "^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^请输入本次加密的key:" << endl;
	//cin >> bb;
	//const char*key = bb.c_str();
	//cout << "lengthofkey=" << my_strlen(key) << endl;
	//cout << endl;
	//JiaMi(src, des, key);
	//cout << endl;
	//char* des2 = new char[2000];
	//JieMi(des, des2, key, my_strlen(src));
	//cout << endl;
	//int JiMIdes[2000];
	//ifstream JiaMIin("JiaMi.txt");
	//int LengthOfMingWen = my_strlen(src);//明文的长度
	//int k = 0;

	//while (!JiaMIin.eof())
	//{
	//	JiaMIin >> JiMIdes[k++];
	//}

	//测试
	/*for(int kk=0;kk<LengthOfMingWen;kk++){
	cout<<JiMIdes[kk]<<" ";
	}*/
	char desOfMingWen[200000];
	FILE* pFile = fopen("735440965", "r");
	fseek(pFile, 0, SEEK_END); //定位到文件末 
	int nFileLen = ftell(pFile); //文件长度
	fseek(pFile, 0, SEEK_SET);
	char* buffer = new char[nFileLen];
	fread(buffer, nFileLen, 1, pFile);
	int* buffer2 = new int[nFileLen];
	for (int i=0;i<nFileLen;i++)
	{
		buffer2[i] = buffer[i];
	}
	JieMiWithoutKey(buffer2, desOfMingWen, nFileLen);
	delete[] buffer;
}