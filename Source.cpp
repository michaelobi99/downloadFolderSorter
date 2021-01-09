//.........................................................................................................................
//optional compilation..
//this part of the code works only on windows
#if defined _MSC_VER
#include <Windows.h>
#include <WinUser.h>
void hide(bool hide) {
	HWND stealth;
	AllocConsole();
	stealth = FindWindowA("ConsoleWindowClass", nullptr);
	ShowWindow(stealth, hide);
}
#endif
//end optional compilation
//.........................................................................................................................

#include <iostream>
#include <chrono>
#include <filesystem>
#include <vector>
#include <algorithm>
#include <thread>
#include <string>
#include <string_view>
#include <condition_variable>
#include <future>
#include <map>
#include <limits>
#include <fstream>
#include <regex>

namespace fs = std::filesystem;
using namespace std::literals::chrono_literals;
std::condition_variable condition;
std::mutex mut;



void mainProcess();
void monitorKeyboard();
std::vector<fs::path> listDir();
void moveFilesToFolder(std::vector<fs::path> const&);
fs::path getDestination(fs::path const&);
std::string getExactFolder(fs::path const&);
bool pathContains(std::vector<std::string> const&, std::string const&);

int main() {
	std::cout << "***********************************************************************************\n";
	std::cout << "*                     Download Folder Sorter v1.0                                 *\n";
	std::cout << "*    Press Esc key on your keyboard to stop this app running in the backgroung    *\n";
	std::cout << "*      In 5 seconds this application will start execution in the background       *\n";
	std::cout << "***********************************************************************************\n";
	std::this_thread::sleep_for(5s);
	std::cin.exceptions(std::fstream::badbit | std::fstream::failbit);
	std::thread workerThread(mainProcess);
	while (true) {
		std::thread keyboardMonitoringThread(monitorKeyboard);
		while (true) {
			std::cout << "\nPress...1> to go back to running in backgroud: ";
			try {
				auto num{ 0 };
				std::cin >> num;
				if (num == 1)
					break;
				else
					continue;
			}
			catch (std::exception const&) {
				std::cin.clear();
				std::cin.ignore(1000, '\n');
				std::cout << "ERROR: Invalid command\n";
			}
		}
		keyboardMonitoringThread.join();
	}
	workerThread.join();
}
void monitorKeyboard() {
	hide(0);
	while (true) {
		char key;
		for (key = 8; key <= 222; ++key) {
			if (GetAsyncKeyState(key) == -32767) {
				if (key == VK_ESCAPE)
					break;
			}
		}
		if (key == VK_ESCAPE)
			break;
	}
	hide(1);
}
void mainProcess() {
	while (true) {
		std::future<std::vector<fs::path>> listOfPaths = std::async(listDir);
		moveFilesToFolder(listOfPaths.get());
	}
}
std::vector<fs::path> listDir() {
	std::vector<std::string> fileExtensionsToLookFor{ ".pdf", ".mkv", ".mp4", ".srt", ".mp3", ".gif", ".png", ".jpg" };
	auto filter = [&fileExtensionsToLookFor](fs::path const& file) {
		for (auto const& extension : fileExtensionsToLookFor) {
			if (file.extension().string() == extension)
				return true;
		}
		return false;
	};
	fs::path folder = R"(C:\Users\HP\Downloads)";
	std::vector<fs::path> listOfPaths;
	listOfPaths.reserve(100);
	while (true) {
		try {
			if (fs::exists(folder) && fs::is_directory(folder)) {
				for (auto const& file : fs::directory_iterator(folder, fs::directory_options::skip_permission_denied)) {
					try {
						if (fs::is_regular_file(file) && filter(file)) {
							listOfPaths.emplace_back(file.path().parent_path() / file.path().filename());
							if (listOfPaths.capacity() == listOfPaths.size())
								listOfPaths.reserve(50);
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
		else
			continue;
	}
	return listOfPaths;
}
void moveFilesToFolder(std::vector<fs::path> const& listOfPaths) {
	std::error_code err;
	
	for (const auto& path : listOfPaths) {
		auto source = path;
		auto destination = fs::path{ getDestination(path) };
		std::cout << destination.string()<<"\n";
	}
	std::this_thread::sleep_for(300s);
}
fs::path getDestination(fs::path const& path) {
	fs::path destination;
	std::map<int, std::string> folderDestinations{
		{0, getExactFolder(path)},
		{1, R"(C:\Users\HP\Videos)"},
		{2, R"(C:\Users\HP\Music\music)"},
		{3, R"(C:\Users\HP\Pictures)"}
	};
	std::vector<std::vector<std::string>> fileExtensionsToLookFor{ {".pdf"}, {".mkv", ".mp4", ".srt"}, {".mp3"}, {".gif", ".png", ".jpg"} };
	auto fileExtension = path.extension().string();
	auto counter{ 0 };
	for (auto const& row : fileExtensionsToLookFor) {
		for (auto const& col : row) {
			if (fileExtension == col) {
				return fs::path{ folderDestinations[counter] }.append(path.filename().string());
			}
		}
		++counter;
	}
}
std::string getExactFolder(fs::path const& path) {
	auto fileName = path.filename().string();
	if (pathContains({ "c++", "cplusplus", "Cplusplus", "cpp" }, fileName))
		return R"(C:\Users\HP\Desktop\my books\programming books\Cplusplus)";
	else if (pathContains({ "Python", "python" }, fileName))
		return R"(C:\Users\HP\Desktop\my books\programming books\Python)";
	else if (pathContains({ "Rust", "rust" }, fileName))
		return R"(C:\Users\HP\Desktop\my books\programming books\Rustlang)";
	else
		return R"(C:\Users\HP\Desktop\my books\programming books)";
}
bool pathContains(std::vector<std::string> const& listOfStr, std::string const& searchString) {
	for (auto const& token : listOfStr) {
		if (auto iter = std::search(std::cbegin(searchString), std::cend(searchString), std::cbegin(token), std::cend(token)); iter != std::cend(searchString))
			return true;
	}
	return false;
}