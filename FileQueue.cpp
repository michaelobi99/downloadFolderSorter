#include "FileQueue.h"
void FileQueue::PushFile(fs::path& filePath) {
	std::lock_guard<std::mutex> guard(mut);
	filequeue.push(filePath);
}
fs::path FileQueue::MoveAndPopFile() {
	std::lock_guard<std::mutex> guard(mut);
	auto tempVariable = filequeue.front();
	filequeue.pop();
	return tempVariable;
}
bool FileQueue::isEmpty() const {
	return filequeue.empty();
}