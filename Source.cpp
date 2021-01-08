#include <iostream>
#include <chrono>
#include <filesystem>
#include <vector>
#include <algorithm>
#include <thread>
#include <string>
#include <string_view>
#include <condition_variable>
#include <queue>
#include <map>
#include <limits>
#include <fstream>
#include "FileQueue.h"
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

namespace fs = std::filesystem;
using namespace std::literals::chrono_literals;

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
void checkForNewFilesInDownloadFolder(FileQueue& queue, std::condition_variable& queueFilled) {
	fs::path folder = R"(C:\Users\HP\Downloads)";
	while (true) {
		try {
			if (fs::exists(folder) && fs::is_directory(folder)) {
				for (auto const& file : fs::directory_iterator(folder, fs::directory_options::skip_permission_denied)) {
					try {
						if (fs::is_regular_file(file)) {
							auto fullPath = file.path().parent_path() / file.path().filename();
							queue.PushFile(fullPath);
						}
					}
					catch (fs::filesystem_error const& error) {}
					catch (std::exception const& error) {}
				}

			}
		}
		catch (fs::filesystem_error const& error) {}
		catch (std::exception const& error) {}
	}
	
}

void moveFilesToFolder(FileQueue& queue, std::condition_variable& queueFilled) {
	extern std::string getExactFolder();
	while (true) {
		std::vector<std::string> fileExtensionsToLookFor{ ".pdf", ".mkv", ".mp4", ".mp3", ".gif", ".png", ".jpg" };
		std::map<std::string, std::string> folderDestinations{
			{"video", R"(C:\Users\HP\Videos)"},
			{"music", R"(C:\Users\HP\Music\music)"},
			{"picture", R"(C:\Users\HP\Pictures)"},
			{"book", getExactFolder()}
		};
	}
}
std::string getExactFolder() {
	return "";
}

int main() {
	auto queue = FileQueue();
	std::condition_variable queueFilled;
	std::cout << "***********************************************************************************\n";
	std::cout << "*                     Download Folder Sorter v1.0                                 *\n";
	std::cout << "*    Press Esc key on your keyboard to stop this app running in the backgroung    *\n";
	std::cout << "*      In 5 seconds this application will start execution in the background       *\n";
	std::cout << "***********************************************************************************\n";
	std::this_thread::sleep_for(5s);
	//std::thread downloadFolderMonitorThread(checkForNewFilesInDownloadFolder, std::ref(queue), std::ref(queueFilled));
	//std::thread fileMoverThread(moveFilesToFolder, std::ref(queue), std::ref(queueFilled));
	//downloadFolderMonitorThread.detach();
	//fileMoverThread.detach();
	std::cin.exceptions(std::fstream::badbit | std::fstream::failbit);
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
			catch (std::exception const& e) {
				std::cin.clear();
				std::cin.ignore(1000, '\n');
				std::cout << "ERROR: Invalid command\n";
			}
		}
		keyboardMonitoringThread.join();
	}
}