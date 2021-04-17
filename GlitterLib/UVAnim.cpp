#include "UVAnim.h"

namespace Glitter
{

UVAnim::UVAnim()
{

}

UVAnim::UVAnim(std::string filename)
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

void UVAnim::read(BinaryReader* reader)
{
    size_t headerAddress = reader->getCurrentAddress();

    reader->gotoAddress(headerAddress + 0x10);
    size_t materialAddress = reader->readAddress();
    reader->gotoAddress(materialAddress);
    material = reader->readString();
}

}
