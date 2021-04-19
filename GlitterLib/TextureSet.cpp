#include "TextureSet.h"

namespace Glitter
{

TextureSet::TextureSet()
{

}

TextureSet::TextureSet(std::string filename)
{
    BinaryReader reader(filename, Endianness::BIG);

    name = filename;
    if (reader.valid())
    {
        reader.readHeader();
        read(&reader);
        reader.close();
    }
}

void TextureSet::read(BinaryReader* reader)
{
    uint32_t textureCount = reader->readInt32();
    size_t tableAddress = reader->readAddress();

    for (uint32_t i = 0; i < textureCount; i++)
    {
        reader->gotoAddress(tableAddress + i * 0x4);
        size_t textureAddress = reader->readAddress();
        reader->gotoAddress(textureAddress);
        dotTextures.push_back(reader->readString());
    }
}

}
