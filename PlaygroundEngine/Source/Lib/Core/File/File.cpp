#include "File.h"

#include <filesystem>
#include <sstream>

#include "Debug/Assert.h"
#include "Util/String.h"

static std::string NAOpenFileDialog(const std::string &defaultPath);

namespace playground
{
	File::File(const std::string &filename, int mode)
	{
		mBinary = mode & std::ios::binary;

		Open(filename, mode);

		// If the file is readable, make sure we can open it.
		if ((mode & std::ios::in) != 0) {
			CORE_ASSERT(mFile.is_open(), "Unable to open file '%s'", filename.c_str());
		}
	}

	File::operator bool()const
	{
		return !mFile.eof();
	}

	File::~File()
	{
		mFile.close();
	}

	std::string File::FileExt()const
	{
		return GetFileExt(mFilename);
	}

	int64_t File::ReadBytes(char *buf, size_t size)
	{
		mFile.read(buf, size);

		return mFile.gcount();
	}

	bool File::ReadLine(std::string &buf)
	{
		return !!std::getline(mFile, buf);
	}

	std::string File::ReadTextFile()
	{
		mFile.seekg(std::ios_base::beg);

		std::stringstream buffer;
		buffer << mFile.rdbuf();

		return buffer.str();
	}

	bool File::WriteBytes(const char *buf, size_t n)
	{
		return !!mFile.write(buf, n);
	}

	void File::Open(const std::string &filename, int mode)
	{
		if (IsFilePath(filename)) {
			mFilename = filename;
		} else {
			FindDataFileRecursively(mFilename, filename);
		}

		mFile.open(mFilename, mode);
	}

	size_t File::GetFileSize()
	{
		std::ifstream temp;
		std::ios::openmode openmode = std::ios::ate;
		if (mBinary) {
			openmode |= std::ios::binary;
		}

		temp.open(mFilename, openmode);
		const size_t size = temp.tellg();

		temp.close();

		return size;
	}


	std::string GetFileExt(const std::string &filename)
	{
		const size_t lastDot = filename.find_last_of(".");
		if (lastDot == filename.npos) {
			return "";
		}

		return filename.substr(lastDot + 1);
	}

	std::string DropFileExt(const std::string &filename)
	{
		std::string ext = GetFileExt(filename);
		return filename.substr(0, filename.length() - ext.length() - 1);
	}


	bool IsFilePath(const std::string &filename)
	{
		return filename.find('\\') != filename.npos;
	}

	bool FileExists(const std::string &filename)
	{
		if (IsFilePath(filename)) {
			return std::filesystem::exists(filename);
		} else {
			std::string temp;
			return FindDataFileRecursively(temp, filename);
		}
	}
	
	bool FindFileRecursively(std::string &out, const std::string &directory, const std::string &filename)
	{
		const std::string suffix = "/" + filename;

		using namespace std::filesystem;
		for (auto &entry : recursive_directory_iterator(directory, directory_options::follow_directory_symlink)) {
			const std::string f = entry.path().generic_string();
			if (StrEndsWith(f, suffix)) {
				out = f;

#if CORE_PLATFORM(PC)
				std::replace(out.begin(), out.end(), '/', '\\');
#endif
				return true;
			}
		}

		return false;
	}

	bool FindDataFileRecursively(std::string& out, const std::string& filename)
	{
		// Search the game files
		if (FindFileRecursively(out, "Data", filename)) {
			return true;
		}

		// Search the engine files
		return FindFileRecursively(out, "../PlaygroundEngine/Data", filename);
	}

	bool GetFullFilepath(std::string &out, const std::string &filename)
	{
		if (IsFilePath(filename)) {
			if (FileExists(filename)) {
				out = filename;
				return true;
			}

			return false;

		} else {
			return FindDataFileRecursively(out, filename);
		}
	}

	bool IsAbsoluteFilePath(const std::string &path)
	{
#if CORE_PLATFORM(PC)
		return path.length() > 1 && path[1] == ':';
#endif
	}



#if defined(_CORE_TOOLS) && CORE_PLATFORM(PC)
	std::string OpenFileDialog(const std::string &defaultPath)
	{
		return NAOpenFileDialog(defaultPath);
	}
#endif // defined(CORE_TOOLS) && CORE_PLATFORM(PC)

}

#if defined(_CORE_TOOLS) && CORE_PLATFORM(PC)
#include "OS/OSWin32.h"
#include <shobjidl.h>
static std::string NAOpenFileDialog(const std::string &defaultPath)
{
	std::string filename;

	HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED |
		COINIT_DISABLE_OLE1DDE);
	if (SUCCEEDED(hr)) {
		IFileOpenDialog *pFileOpen;

		// Create the FileOpenDialog object.
		hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_ALL,
			IID_IFileOpenDialog, reinterpret_cast<void**>(&pFileOpen));

		if (SUCCEEDED(hr)) {
			wchar_t wDefaultPath[2048];
			MultiByteToWideChar(CP_ACP, 0, defaultPath.c_str(), MAX_FILEPATH_LENGTH, wDefaultPath, MAX_FILEPATH_LENGTH);

			IShellItem *pSI;
			hr = SHCreateItemFromParsingName(wDefaultPath, NULL, IID_PPV_ARGS(&pSI));

			if (SUCCEEDED(hr)) {
				pFileOpen->SetDefaultFolder(pSI);

				// Show the Open dialog box.
				hr = pFileOpen->Show(NULL);

				// Get the file name from the dialog box.
				if (SUCCEEDED(hr)) {
					IShellItem *pItem;
					hr = pFileOpen->GetResult(&pItem);
					if (SUCCEEDED(hr)) {
						PWSTR pszFilePath;
						hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);

						// Display the file name to the user.
						if (SUCCEEDED(hr)) {
							char buffer[2048];
							WideCharToMultiByte(CP_ACP, 0, pszFilePath, MAX_FILEPATH_LENGTH, buffer, 2048, 0, 0);
							CoTaskMemFree(pszFilePath);

							filename = buffer;
						}
						pItem->Release();
					}
				}
				pFileOpen->Release();
			}
			CoUninitialize();
		}
	}

	return filename;
}
#endif