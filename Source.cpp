#include <iostream>
#include <chrono>
#include <filesystem>
#include <vector>
#include <iostream>
#include <algorithm>
#include <fstream>
#include <thread>
#include <string>
#include <string_view>
#include <condition_variable>
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

void help() {

}
int main() {
	std::cout << "************Download Folder Sorter v1.0************\n";
	std::cout << R"(Type "help" for more information)"<<"\n";
	auto [help, number] = std::tuple(std::string{ "" }, 0);
#if defined _MSC_VER
	std::cout << "\npress...\n1>To make this app run in the nackground\n2>to ignore this option\n";
	std::cin >> number;
	while (number != 1 && number != 2) {
		std::cout << "ERROR: Invalid command\n";
		std::cout << "\npress...\n1>To make this app run in the background\n2>to ignore this option\n";
		std::cin >> number;
	}
	number == 1 ? hide(0) : hide(1);
#endif
	std::this_thread::sleep_for(3s);
	hide(1);
	std::this_thread::sleep_for(3s);

}