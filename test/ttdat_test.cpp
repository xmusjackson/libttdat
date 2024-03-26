#include "../ttdat.hpp"

using namespace std;

int main (int argc, char** argv) {
    pathInfo file;
    if (argc > 1) {
        file = getPathAndName(argv[1]);
    } else {
        return 1;
    }

    TTDat datFile(file.filePath, file.fileName);

    cout << "Info Offset: " << datFile.infoOffset << endl;
    cout << "Info Size: " << datFile.infoSize << endl;
    cout << "File Count: " << datFile.fileCount << endl;

    return 0;
}