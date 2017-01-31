/* ==================================== JUCER_BINARY_RESOURCE ====================================

   This is an auto-generated file: Any edits you make may be overwritten!

*/

namespace BinaryData
{

//================== base.obj ==================
static const unsigned char temp_binary_data_0[] =
"# cube.obj\r\n"
"#\r\n"
" \r\n"
"g cube\r\n"
" \r\n"
"v  -1.0  -1.0  0.0\r\n"
"v  -1.0  -1.0  1.0\r\n"
"v  -1.0  1.0  0.0\r\n"
"v  -1.0  1.0  1.0\r\n"
"v  1.0  -1.0  0.0\r\n"
"v  1.0  -1.0  1.0\r\n"
"v  1.0  1.0  0.0\r\n"
"v  1.0  1.0  1.0\r\n"
"\r\n"
"vn  0.0  0.0  1.0\r\n"
"vn  0.0  0.0 -1.0\r\n"
"vn  0.0  1.0  0.0\r\n"
"vn  0.0 -1.0  0.0\r\n"
"vn  1.0  0.0  0.0\r\n"
"vn -1.0  0.0  0.0\r\n"
" \r\n"
"f  1//2  7//2  5//2\r\n"
"f  1//2  3//2  7//2 \r\n"
"f  1//6  4//6  3//6 \r\n"
"f  1//6  2//6  4//6 \r\n"
"f  3//3  8//3  7//3 \r\n"
"f  3//3  4//3  8//3 \r\n"
"f  5//5  7//5  8//5 \r\n"
"f  5//5  8//5  6//5 \r\n"
"f  1//4  5//4  6//4 \r\n"
"f  1//4  6//4  2//4 \r\n"
"f  2//1  6//1  8//1 \r\n"
"f  2//1  8//1  4//1 ";

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
        case 0x99691289:  numBytes = 561; return base_obj;
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
