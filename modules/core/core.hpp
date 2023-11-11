#pragma once
#include<string>
#include<vector>
class core
{
private:
	std::vector<std::string> GetNestedPath(std::string Path, char PathSeparator);//参数:路径,路径分隔符.返回:每一级路径
	std::string ReplaceString(std::string str, std::vector<std::pair<std::string, std::string>>Replace);//传入被替换的文本,<被替换的关键文本,替换为什么文本>
	bool _zipAddFile(void* ZipPoint, std::string FilePath, std::string PathInZip);//内部函数,将文件压缩进压缩包的指定位置
	bool _zipAddFolder(void* ZipPoint, std::string PathInZip);//添加一个空文件夹到压缩包的指定位置

	/*
		@brief 内部函数,将文件夹压缩进压缩包的指定位置.此函数可以递归调用,遍历所有文件夹,调用_zipAddFolder逐步创建文件夹,直到找到纯文件,逐个调用_zipFile压缩进压缩包.
		@param ZipPoint zip指针
		@param FolderPath 被压缩文件夹路径,支持相对路径,绝对路径
		@param PathInZip 被压缩文件夹在压缩包中的路径,留空为压缩包根目录
	*/
	bool _zipFolder(void* ZipPoint, std::string FolderPath, std::string PathInZip="");
public:
	std::string OutputZipName;
	core(std::string ZipName);
	~core();
	bool release_resource();//释放资源
	bool unZipFile(std::string path);

	/*
		@brief zipFolder:将一个文件夹压缩为压缩包
		@param FolderPath 被压缩文件夹路径,支持相对路径,绝对路径
		@param SavePath 压缩包保存路径,支持相对路径,绝对路径
	*/
	bool zipFolder(std::string FolderPath, std::string SavePath);//顶层函数,压缩文件夹

	std::vector<std::string> SplitString(std::string str, int n);
	void GenerateDocument(std::string student_name, std::string student_class, std::string student_ID, std::string word_title, std::string MainText);
	std::string GenerateXML_MainText(std::vector<std::string> str);
	bool GenerateWord(std::string SavePath);
};