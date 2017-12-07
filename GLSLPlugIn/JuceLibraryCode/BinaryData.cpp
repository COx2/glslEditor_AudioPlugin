/* ==================================== JUCER_BINARY_RESOURCE ====================================

   This is an auto-generated file: Any edits you make may be overwritten!

*/

namespace BinaryData
{

//================== base.obj ==================
static const unsigned char temp_binary_data_0[] =
"g board\n"
"\n"
"v -1.000000 -1.000000 0.000000\n"
"v 1.000000 1.000000 0.000000\n"
"v 1.000000 -1.000000 0.000000\n"
"v -1.000000 1.000000 0.000000\n"
"vn 0.000000 0.000000 -1.000000\n"
"f 1//1 2//1 3//1\n"
"f 1//1 4//1 2//1\n";

const char* base_obj = (const char*) temp_binary_data_0;


const char* getNamedResource (const char*, int&) throw();
const char* getNamedResource (const char* resourceNameUTF8, int& numBytes) throw()
{
    unsigned int hash = 0;
    if (resourceNameUTF8 != 0)
        while (*resourceNameUTF8 != 0)
            hash = 31 * hash + (unsigned int) *resourceNameUTF8++;

    switch (hash)
    {
        case 0x99691289:  numBytes = 194; return base_obj;
        default: break;
    }

    numBytes = 0;
    return 0;
}

const char* namedResourceList[] =
{
    "base_obj"
};

}
