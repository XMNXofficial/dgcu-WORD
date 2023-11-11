#include"core.hpp"
#include<vector>
#include<fstream>
#include<sstream>
#include<locale>
#include<codecvt>
#include<iostream>
#include<filesystem>
#include<Windows.h>
#include<minizip/zip.h>
#include<minizip/unzip.h>
#include"../../res.h"
#include"iconv.h"
core::core(std::string ZipName)
{
	OutputZipName = ZipName;
}

core::~core()
{

}
std::vector<std::string> core::GetNestedPath(std::string Path, char PathSeparator)
{
	std::vector<std::string>Paths;
	std::istringstream iss(Path);//使用FileName构造string输入流
	std::string temp_result;//暂存每次的结果
	while (std::getline(iss, temp_result, PathSeparator))
	{
		Paths.push_back(temp_result);
	}
	return Paths;
}

std::string core::ReplaceString(std::string str, std::vector<std::pair<std::string, std::string>> Replace)
{
	for (auto& i : Replace)//批量替换
	{
		size_t pos = 0;//每次循环重置读取位置
		while ((pos = str.find(i.first, pos)) != std::string::npos)
			str.replace(pos, i.first.length(), i.second);
	}
	return str;
}

bool core::release_resource()
{

	HRSRC hResInfo = FindResource(NULL, MAKEINTRESOURCE(IDR_ZIP1), "ZIP");//寻找资源文件
	if (!hResInfo)
	{
		return false;
	}
	HGLOBAL hResData = LoadResource(NULL, hResInfo);//内存加载资源文件
	if (!hResData)
	{
		return false;
	}
	LPVOID resPoint = LockResource(hResData);//获取加载资源文件的内存指针
	DWORD size = SizeofResource(NULL, hResInfo);

	std::ofstream file(OutputZipName, std::ios::out | std::ios::binary);
	if (file.is_open())
	{
		file.write(static_cast<const char*>(resPoint), size);
		file.close();
	}
	else
	{
		FreeResource(hResData);
		return false;
	}
	FreeResource(hResData);
	return true;
}

bool core::unZipFile(std::string path)
{
	unzFile zip = unzOpen(path.c_str());//打开压缩包
	if (zip)//打开成功
	{
		if (unzGoToFirstFile(zip) == UNZ_OK)//尝试获取第一个文件
		{
			std::string UnzipFilePath = ReplaceString(GetNestedPath(path, '/').back(), { {".zip",""} });//解压到指定的目录,此处默认以被解压的压缩包的文件名作为文件夹名,不带拓展名 (这里放到循环外面,提高性能)
			do
			{
				//这里用do-while的原因,先到达第一个文件位置,然后打开文件进行处理,然后处理完再开始下一个
				char FileName[1024] = { 0 };
				unz_file_info FileInfo;
				if (unzGetCurrentFileInfo(
					zip, &FileInfo,
					FileName, sizeof(FileName),//文件名接收
					nullptr, NULL, nullptr, NULL
				) == UNZ_OK)//尝试获取当前位置文件信息
				{
					if (unzOpenCurrentFile(zip) == UNZ_OK)//尝试打开当前位置文件
					{
						//读取文件到缓冲区
						int FileBufferSize = 1024 * 1024;//1024B:1kb,1024KB:1MB 这里设置缓冲区1MB,按需调整
						char* FileBuffer = new char[FileBufferSize];
						int FileSize = unzReadCurrentFile(zip, FileBuffer, FileBufferSize);//读取当前位置文件,写入内存缓冲区
						//处理文件路径
						std::vector<std::string>FileChildPaths = GetNestedPath(FileName, '/');//存储每一级目录名
						FileChildPaths.insert(FileChildPaths.begin(), UnzipFilePath);//指定第一级解压目录,将其插入到数组首部
						FileChildPaths.pop_back();//最后一个是文件名,不是目录名,需要去除
						std::string TempPath = "./";//临时路径,用于多级路径的逐级拼接
						for (auto& i : FileChildPaths)
						{
							TempPath += i + "/";
							CreateDirectory(TempPath.c_str(), NULL);
						}
						std::ofstream OutputFile(UnzipFilePath + "/" + FileName, std::ios::out | std::ios::binary);//创建输出
						if (OutputFile.is_open())
						{
							OutputFile.write(static_cast<const char*>(FileBuffer), FileSize);
							OutputFile.close();//释放
						}
						delete[] FileBuffer;
						unzCloseCurrentFile(zip);
					}
					else
					{
						unzClose(zip);
						return false;
					}
				}
				else
				{
					unzClose(zip);
					return false;
				}
			} while (unzGoToNextFile(zip) == UNZ_OK);
		}
		else
		{
			unzClose(zip);
			return false;
		}
		unzClose(zip);
		return true;
	}
	return false;
}

std::vector<std::string> core::SplitString(std::string str, int n)//str:输入的字符串;n:每行n个字
{
	//因为string下中文字符与英文字符所占字节不同,因此统一转换为wstring进行处理
	str = ReplaceString(str, { {"\n  ","\n"} });//先统一去除空格
	str = ReplaceString(str, { {"\n\n","\n"} });//将连续两次换行的,转为一次换行
	std::vector<std::string> result;
	std::vector<std::string> paragraph = GetNestedPath(str, '\n');//获取每一大段
	for (auto& i1 : paragraph)
	{
		std::wstring w_str;
		std::wstring_convert<std::codecvt_utf8<wchar_t>> convert;//定义转换器

		w_str = convert.from_bytes(i1);//string-->wstring
		for (size_t i2 = 0; i2 < w_str.length(); i2 += n)
		{
			std::wstring temp_wstr = w_str.substr(i2, n);
			std::string temp_str = convert.to_bytes(temp_wstr);
			result.push_back((i2 == 0) ? ("XMNX_两个空格_XMNX" + temp_str) : (temp_str));//如果是一段的首行,则添加两个空格
		}
	}
	return result;
}

std::string core::GenerateXML_MainText(std::vector<std::string> str)
{
	std::string result;
	std::string XML = R"(
<w:tr w:rsidR="00360C2C" w14:textId="77777777">
    <w:trPr>
        <w:trHeight w:hRule="exact" w:val="567" />
    </w:trPr>
    <w:tc>
        <w:tcPr>
            <w:tcW w:w="7924" w:type="dxa" />
            <w:gridSpan w:val="3" />
            <w:vAlign w:val="center" />
        </w:tcPr>
        <w:p w14:paraId="45FA1213" w14:textId="77777777" w:rsidR="00360C2C"
            w:rsidRDefault="00000000">
            <w:r>
                <w:rPr>
                    <w:rFonts w:hint="eastAsia" />
                </w:rPr>
                <w:t>XMNX_正文_XMNX</w:t>
            </w:r>
        </w:p>
    </w:tc>
</w:tr>
)";
	for (auto& i : str)
	{
		std::string temp_XML = XML;
		size_t found = i.find("XMNX_两个空格_XMNX");
		if (found != std::string::npos)
		{
			//找到了
			temp_XML = ReplaceString(temp_XML, { {
					R"(
        <w:p w14:paraId="45FA1213" w14:textId="77777777" w:rsidR="00360C2C"
            w:rsidRDefault="00000000">
)",
					R"(
        <w:p w14:paraId="45FA1212" w14:textId="77777777" w:rsidR="00360C2C"
            w:rsidRDefault="00000000">
		<w:pPr>
			<w:ind w:firstLineChars="200" w:firstLine="420" />
		</w:pPr>
)"
				} });

		}
		result += ReplaceString(temp_XML, { {"XMNX_正文_XMNX",i} }) + "\n";
	}
	result = ReplaceString(result, { {"XMNX_两个空格_XMNX",""} });
	return result;
}

void core::GenerateDocument(std::string student_name, std::string student_class, std::string student_ID, std::string word_title, std::string MainText)
{
	std::string UnzipFilePath = ReplaceString(GetNestedPath(OutputZipName, '/').back(), { {".zip",""} });//获取解压缩第一级目录名
	std::string DocumentPath = UnzipFilePath + "/" + "word/document.xml";
	std::ifstream document_in(DocumentPath, std::ios::in);//打开document.xml
	std::istreambuf_iterator<char> begin(document_in), end;
	std::string document_str(begin, end);
	document_in.close();
	document_str = ReplaceString(document_str, { {"XMNX_段落_XMNX",MainText}, {"XMNX_学号_XMNX",student_ID}, {"XMNX_姓名_XMNX",student_name}, {"XMNX_班级_XMNX",student_class}, {"XMNX_标题_XMNX",word_title} });
	std::ofstream OutputFile(DocumentPath, std::ios::out | std::ios::binary);//创建输出
	if (OutputFile.is_open())
	{
		OutputFile.write(static_cast<const char*>(document_str.c_str()), document_str.size());
		OutputFile.close();//释放
	}

}

bool core::GenerateWord(std::string SavePath)
{
	std::string UnzipFilePath = ReplaceString(GetNestedPath(OutputZipName, '/').back(), { {".zip",""} });//获取解压缩第一级目录名
	char* temp_in = new char[1024];
	wchar_t* temp_out = new wchar_t[1024];
	memset(temp_in, 0, 1024);
	memset(temp_out, 0, 1024);
	char* temp_in_backup = temp_in;
	wchar_t* temp_out_backup = temp_out;
	strcpy(temp_in, SavePath.c_str());
	size_t in_length = strlen(temp_in) + 1;
	size_t out_buffer_length = 1024;
	iconv_t temp_iconv = iconv_open("wchar_t", "UTF-8");
	int temp_code = iconv(temp_iconv, &temp_in_backup, &in_length, (char**)&temp_out_backup, &out_buffer_length);
	iconv_close(temp_iconv);


	int wide_string_length = wcslen(temp_out);

	int ansi_length = WideCharToMultiByte(CP_ACP, 0, temp_out, wide_string_length, NULL, 0, NULL, NULL);
	char* ansi_string = new char[ansi_length + 1];

	WideCharToMultiByte(CP_ACP, 0, temp_out, wide_string_length, ansi_string, ansi_length, NULL, NULL);
	ansi_string[ansi_length] = '\0'; // 手动添加字符串终止符

	zipFolder(UnzipFilePath, ansi_string);
	delete[] ansi_string; // 释放内存

	return true;
}

bool core::_zipAddFile(void* ZipPoint, std::string FilePath, std::string PathInZip)
{
	std::printf("\033[0m\033[1;32m _zipAddFile:\nFilePath:   %s\nPathInZip:   %s\n\n \033[0m", FilePath.c_str(), PathInZip.c_str());
	zip_fileinfo ZipFileInfo;
	if (zipOpenNewFileInZip
	(
		ZipPoint, PathInZip.c_str(), &ZipFileInfo,
		nullptr, NULL, nullptr, NULL, nullptr,
		Z_DEFLATED, 9 //默认压缩方法,默认压缩等级
	) == ZIP_OK)
	{

		std::ifstream InputFile(FilePath, std::ios::binary | std::ios::in);
		if (InputFile.is_open())
		{
			InputFile.seekg(0l, std::ios::end);
			std::streamsize FileSize = InputFile.tellg();
			InputFile.seekg(0l, std::ios::beg);
			char* buffer = new char[FileSize];
			memset(buffer, 0, FileSize);
			InputFile.read(buffer, FileSize);
			zipWriteInFileInZip(ZipPoint, buffer, FileSize);
			InputFile.close();
			zipCloseFileInZip(ZipPoint);//无论是否成功,都需要关闭
			delete[] buffer;
			return true;
		}
		else
		{
			zipCloseFileInZip(ZipPoint);//无论是否成功,都需要关闭
			return false;
		}
	}
	else
	{
		zipCloseFileInZip(ZipPoint);//无论是否成功,都需要关闭
		return false;
	}

}
bool core::_zipAddFolder(void* ZipPoint, std::string PathInZip)
{
	std::printf("_zipAddFolder:\nPathInZip:   %s\n\n", PathInZip.c_str());
	//zip_fileinfo ZipFileInfo;
	//if (zipOpenNewFileInZip
	//(
	//	ZipPoint, PathInZip.c_str(), &ZipFileInfo,
	//	nullptr, NULL, nullptr, NULL, nullptr,
	//	Z_DEFLATED, Z_DEFAULT_COMPRESSION //默认压缩方法,默认压缩等级
	//) == ZIP_OK)
	//{
	//	zipCloseFileInZip(ZipPoint);//无论是否成功,都需要关闭
	//	return true;
	//}
	//else
	//{
	//	zipCloseFileInZip(ZipPoint);//无论是否成功,都需要关闭
	//	return false;
	//}
	return true;
}
bool core::_zipFolder(void* ZipPoint, std::string FolderPath, std::string PathInZip)
{
	std::filesystem::path path(FolderPath);
	if (std::filesystem::is_directory(path))
	{
		for (auto& i : std::filesystem::directory_iterator(path))
		{
			std::filesystem::path entryPath = i.path();
			std::string entryPathInZip;
			if (!PathInZip.empty())
			{
				entryPathInZip = PathInZip + "/" + entryPath.filename().string();
			}
			else
			{
				entryPathInZip = entryPath.filename().string();
			}

			if (std::filesystem::is_directory(entryPath))
			{
				if (!_zipAddFolder(ZipPoint, entryPathInZip))
				{
					return false;
				}
				if (!_zipFolder(ZipPoint, entryPath.string(), entryPathInZip))
				{
					return false;
				}
			}
			else
			{
				if (!_zipAddFile(ZipPoint, entryPath.string(), entryPathInZip))
				{
					return false;
				}
			}
		}
	}
	return true;
}

bool core::zipFolder(std::string FolderPath, std::string SavePath)//顶层封装
{
	zipFile zip = zipOpen(SavePath.c_str(), APPEND_STATUS_CREATE);
	if (zip != NULL)//注意,这里zip是句柄,不是判断是否OK,minizip只有操作才需要判断是否ok
	{
		_zipFolder(zip, FolderPath);
		zipClose(zip, nullptr);
		return true;
	}
	else
	{
		zipClose(zip, nullptr);
		return false;
	}

}