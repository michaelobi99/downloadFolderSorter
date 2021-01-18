#include <Windows.h>
#include <WinUser.h>
#include <iostream>
#include <chrono>
#include <filesystem>
#include <vector>
#include <algorithm>
#include <thread>
#include <string>
#include <map>

namespace fs = std::filesystem;
using namespace std::literals::chrono_literals;

void mainProcess();
void monitorKeyboard();
std::vector<fs::path> listDir();
void moveFilesToFolder(std::vector<fs::path> const&);
fs::path getDestination(fs::path const&);
std::string getExactFolder(fs::path const&);
void deleteCRdownloadFiles();
bool isOldFile(fs::path const&);
void hide(bool);

template <typename Type>
bool pathContains(std::vector<Type> const& listOfStr, Type const& searchString) {
	for (auto const& token : listOfStr) {
		if (auto iter = std::search(std::cbegin(searchString), std::cend(searchString), std::cbegin(token), std::cend(token)); iter != std::cend(searchString))
			return true;
	}
	return false;
}

int main() {
	std::cout << "***********************************************************************************\n";
	std::cout << "*                     Download Folder Sorter v1.0                                 *\n";
	std::cout << "*    Press Esc key on your keyboard to stop this app running in the backgroung    *\n";
	std::cout << "*                   Press f1 to run in the in the background                      *\n";
	std::cout << "***********************************************************************************\n";
	std::this_thread::sleep_for(3s);
	std::thread workerThread(mainProcess);
	std::thread keyboardMonitoringThread(monitorKeyboard);
	std::thread deleteFilesThatFailedToDownload(deleteCRdownloadFiles);
	keyboardMonitoringThread.join();
	deleteFilesThatFailedToDownload.join();
	workerThread.join();
	return 0;
}
void monitorKeyboard() {
	auto showScreen{ 0 };
	hide(showScreen);
	while (true) {
		char key;
		for (key = 8; key <= 222; ++key) {
			if (GetAsyncKeyState(key) == -32767) {
				if (key == VK_F12) {
					if (showScreen == 1)
						showScreen = 0;
					else
						showScreen = 1;
					hide(showScreen);
				}
			}
		}
	}
}

void hide(bool num) {
	HWND stealth;
	AllocConsole();
	stealth = FindWindowA("ConsoleWindowClass", nullptr);
	ShowWindow(stealth, num);
}

void mainProcess() {
	while (true) {
		std::vector<fs::path> listOfPaths = listDir();
		moveFilesToFolder(listOfPaths);
	}
}
std::vector<fs::path> listDir() {
	std::vector<std::string> fileExtensionsToLookFor{ ".pdf", ".mkv", ".mp4", ".srt", ".mp3", ".gif", ".png", ".jpg" };
	auto toUppercase = [](std::string str) {
		std::transform(std::begin(str), std::end(str), std::begin(str), ::toupper);
		return str;
	};
	auto filter = [&fileExtensionsToLookFor, &toUppercase](fs::path const& file) {
		for (auto const& extension : fileExtensionsToLookFor) {
			if (file.extension().string() == extension || file.extension().string() == toUppercase(extension))
				return true;
		}
		return false;
	};
	fs::path folder = R"(C:\Users\HP\Downloads)";
	std::vector<fs::path> listOfPaths;
	listOfPaths.reserve(10);
	while (true) {
		try {
			if (fs::exists(folder) && fs::is_directory(folder)) {
				for (auto const& file : fs::directory_iterator(folder, fs::directory_options::skip_permission_denied)) {
					try {
						if (fs::is_regular_file(file) && filter(file)) {
							listOfPaths.emplace_back(file.path().parent_path() / file.path().filename());
							if (listOfPaths.capacity() == listOfPaths.size())
								listOfPaths.reserve(10);
						}
					}
					catch (fs::filesystem_error const&) {}
					catch (std::exception const&) {}
				}
			}
		}
		catch (fs::filesystem_error const&) {}
		catch (std::exception const&) {}
		if (std::size(listOfPaths) > 0)
			break;
	}
	return listOfPaths;
}
void moveFilesToFolder(std::vector<fs::path> const& listOfPaths) {
	std::error_code err;
	for (const auto& path : listOfPaths) {
		auto source = path;
		auto destination = fs::path{ getDestination(path) };
		std::this_thread::sleep_for(2s);
		fs::rename(source, destination, err);
		if (err) {}
		
	}
}
fs::path getDestination(fs::path const& path) {
	fs::path destination;
	std::map<int, std::string> folderDestinations{
		{0, getExactFolder(path)},
		{1, R"(C:\Users\HP\Videos)"},
		{2, R"(C:\Users\HP\Music\music)"},
		{3, R"(C:\Users\HP\Pictures)"}
	};
	auto toUppercase = [](std::string str) {
		std::transform(std::begin(str), std::end(str), std::begin(str), ::toupper);
		return str;
	};
	std::vector<std::vector<std::string>> fileExtensionsToLookFor{ {".pdf"}, {".mkv", ".mp4", ".srt"}, {".mp3"}, {".gif", ".png", ".jpg"} };
	auto fileExtension = path.extension().string();
	auto counter{ 0 };
	for (auto const& row : fileExtensionsToLookFor) {
		for (auto const& col : row) {
			if (fileExtension == col || fileExtension == toUppercase(col)) {
				try {
					return fs::path{ folderDestinations[counter] }.append(path.filename().string());
				}
				catch (std::exception const&) {
					return fs::path{ folderDestinations[counter] }.append(path.filename().wstring());
				}
			}
		}
		++counter;
	}
}
std::string getExactFolder(fs::path const& path) {
	try {
		auto fileName = path.filename().string();
		if (pathContains({ "c++", "C++", "cplusplus", "Cplusplus", "cpp", "Cpp" }, fileName))
			return R"(C:\Users\HP\Desktop\my books\programming books\Cplusplus)";
		else if (pathContains({ "Python", "python" }, fileName))
			return R"(C:\Users\HP\Desktop\my books\programming books\Python)";
		else if (pathContains({ "Rust", "rust" }, fileName))
			return R"(C:\Users\HP\Desktop\my books\programming books\Rustlang)";
		else
			return R"(C:\Users\HP\Desktop\my books)";
	}
	catch (std::exception const&) {
		std::wstring fileName = path.filename().wstring();
		if (pathContains({ L"c++", L"C++", L"cplusplus", L"Cplusplus", L"cpp", L"Cpp" }, fileName))
			return R"(C:\Users\HP\Desktop\my books\programming books\Cplusplus)";
		else if (pathContains({ L"Python", L"python" }, fileName))
			return R"(C:\Users\HP\Desktop\my books\programming books\Python)";
		else if (pathContains({ L"Rust", L"rust" }, fileName))
			return R"(C:\Users\HP\Desktop\my books\programming books\Rustlang)";
		else
			return R"(C:\Users\HP\Desktop\my books)";
	}
	
}

void deleteCRdownloadFiles() {
	using namespace std::literals::string_literals;
	auto extensionToLookFor = R"(.crdownload)"s;
	auto filter = [&extensionToLookFor](fs::path const& file) {
		auto upper = extensionToLookFor;
		std::transform(std::begin(upper), std::end(upper), std::begin(upper), ::toupper);
		if (file.extension().string() == extensionToLookFor || file.extension().string() == upper)
			return true;
		return false;
	};
	fs::path folder = R"(C:\Users\HP\Downloads)";
	std::vector<fs::path> listOfPaths;
	listOfPaths.reserve(3);
	while (true) {
		try {
			if (fs::exists(folder) && fs::is_directory(folder)) {
				for (auto const& file : fs::directory_iterator(folder, fs::directory_options::skip_permission_denied)) {
					try {
						if (filter(file) && isOldFile(file)) {
							listOfPaths.emplace_back(file.path().parent_path() / file.path().filename());
							if (listOfPaths.capacity() == listOfPaths.size())
								listOfPaths.reserve(3);
						}
					}
					catch (fs::filesystem_error const&) {}
					catch (std::exception const&) {}
				}
			}
		}
		catch (fs::filesystem_error const&) {}
		catch (std::exception const&) {}
		if (std::size(listOfPaths) > 0) {
			std::error_code err;
			for (auto const& path : listOfPaths) {
				auto success = fs::remove(path, err);
			}
			listOfPaths.clear();
			listOfPaths.reserve(3);
		}
	}
}

bool isOldFile(fs::path const& file) {
	std::error_code err;
	auto lwt = fs::last_write_time(file, err);
	auto diff = std::filesystem::file_time_type::clock::now() - lwt;
	auto timeInSeconds = std::chrono::duration_cast<std::chrono::seconds>(diff).count();
	return timeInSeconds < 7200 ? false : true;
}