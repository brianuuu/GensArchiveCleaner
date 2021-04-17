#include "PTAnim.h"

namespace Glitter
{

PTAnim::PTAnim()
{

}

PTAnim::PTAnim(std::string filename)
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

void PTAnim::read(BinaryReader* reader)
{
    size_t headerAddress = reader->getCurrentAddress();

    reader->gotoAddress(headerAddress + 0x10);
    size_t materialAddress = reader->readAddress();
    reader->gotoAddress(materialAddress);
    material = reader->readString();

    reader->gotoAddress(headerAddress + 0x18);
    size_t textureListAddress = reader->readAddress();
    size_t textureListSize = reader->readInt32();

    // find the actual size since textureListSize includes 00 padding
    reader->gotoAddress(textureListAddress + textureListSize - 1);
    while (reader->readChar() == 0)
    {
        textureListSize--;
        reader->gotoAddress(textureListAddress + textureListSize - 1);
    }
    textureListSize++;

    reader->gotoAddress(textureListAddress);
    while(reader->getCurrentAddress() < textureListAddress + textureListSize)
    {
        textures.push_back(reader->readString());
    }
}

}
