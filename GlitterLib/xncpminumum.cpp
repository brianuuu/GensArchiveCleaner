#include "xncpminumum.h"

namespace Glitter
{

XNCPMinumum::XNCPMinumum()
{

}

XNCPMinumum::XNCPMinumum(std::string filename)
{
    BinaryReader reader(filename, Endianness::LITTLE);

    name = filename;
    if (reader.valid())
    {
        reader.readHeader();
        read(&reader);
        reader.close();
    }
}

XNCPMinumum::~XNCPMinumum()
{

}

std::string XNCPMinumum::getName() const
{
    return name;
}

std::vector<std::string> XNCPMinumum::getTextures() const
{
    return textures;
}

void XNCPMinumum::read(BinaryReader *reader)
{
    if (!reader->valid())
        return;

    reader->setRootNodeAddress(0);
    reader->gotoAddress(0);

    reader->readInt32(); // skip FAPC
    size_t resource0Size = reader->readAddress();
    reader->gotoAddress(reader->getCurrentAddress() + resource0Size + 0x24);

    size_t startAddress = reader->getCurrentAddress();
    reader->readInt32(); // skip NXTL
    reader->readInt32(); // skip size
    reader->readInt32(); // skip offset
    reader->readInt32(); // skip 0
    size_t textureCount = reader->readInt32();
    reader->readInt32(); // skip 0x18
    size_t listAddress = reader->getCurrentAddress();

    for (size_t i = 0; i < textureCount; i++)
    {
        reader->gotoAddress(listAddress + i * 0x8);
        size_t offset = reader->readAddress();
        reader->gotoAddress(startAddress + offset);
        textures.push_back(reader->readString());
    }
}

}
