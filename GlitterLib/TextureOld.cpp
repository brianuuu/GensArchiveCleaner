#include "TextureOld.h"

namespace Glitter
{

TextureOld::TextureOld()
{

}

TextureOld::TextureOld(std::string filename)
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

void TextureOld::read(BinaryReader* reader)
{
    size_t textureAddress = reader->readAddress();
    reader->gotoAddress(textureAddress);
    texture = reader->readString();
}

}
