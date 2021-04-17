#ifndef XNCPMINUMUM_H
#define XNCPMINUMUM_H

#include <string>
#include <vector>
#include "BinaryReader.h"

namespace Glitter
{

class XNCPMinumum
{
private:
    std::string name;
    std::vector<std::string> textures;

public:
    XNCPMinumum();
    XNCPMinumum(std::string filename);
    ~XNCPMinumum();

    std::string getName() const;
    std::vector<std::string> getTextures() const;

    void read(BinaryReader* reader);
};

}

#endif // XNCPMINUMUM_H
