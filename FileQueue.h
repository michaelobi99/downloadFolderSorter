#pragma once
#include <mutex>
#include <queue>
#include <string>
#include <string_view>
#include <filesystem>
namespace fs = std::filesystem;
class FileQueue
{
public:
	FileQueue() = default;
	FileQueue(FileQueue const&) = delete;
	void PushFile(fs::path& filePath);
	fs::path MoveAndPopFile();
	bool isEmpty() const;
private:
	std::mutex mut;
	std::queue<fs::path> filequeue;
};

