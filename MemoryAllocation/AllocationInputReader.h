#ifndef ALLOCATIONINPUTREADER_H
#define ALLOCATIONINPUTREADER_H

#include <string>
#include <vector>

class AllocationInputReader {
public:
    static std::string selectImplementation();
    static std::string readMode();
    static int readInteger(const std::string& prompt, int minValue = 0);
    static std::string readStrategy();

    struct FileCommand {
        bool isCreate;
        int value;
        int lineNumber;
        std::string rawLine;
    };

    static bool parseCommandFile(
        const std::string& path,
        int& outTotalMemory,
        std::vector<FileCommand>& outCommands,
        std::vector<std::string>& outLog
    );
};

#endif // ALLOCATIONINPUTREADER_H
