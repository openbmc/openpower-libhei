#include "chip_data_stream.hpp"
#include <iostream>

using namespace std;

int main ()
{

    //Create a buffer with well determined data in well determined locations
    uint8_t PRF_data[32]={0x01,0x80,0x80,0x80,0x80,0,0x80,0,0x80,0,0,0,
                          0x80,0,0,0,0x80,0,0,0,0,0,0,0,0x80,0,0,0,0,0,0,0};

    //Instantiate a cds object
    ChipDataStream cds(PRF_data,32);

    //Test Get functions that accept an offset parameter.
    cout << endl << endl << endl << endl << endl;
    cout << "Testing \"Offset\" functions:" << endl;
    cout << "bool: ";
    cout << cds.GetBool(0) << endl;
    cout << "char: ";
    cout << cds.GetChar(1) << endl;
    cout << "uint8: ";
    cout << +cds.GetUInt8(2) << endl;
    cout << "int8: ";
    printf("%d\n",cds.GetInt8(3));
    cout << "uint16: ";
    cout << cds.GetUInt16(4) << endl;
    cout << "int16: ";
    cout << cds.GetInt16(6) << endl;
    cout << "uint32: ";
    cout << cds.GetUInt32(8) << endl;
    cout << "int32: ";
    cout << cds.GetInt32(12) << endl;
    cout << "uint64: ";
    cout << cds.GetUInt64(16) << endl;
    cout << "int64: ";
    cout << cds.GetInt64(24) << endl;
    cout << cds.GetInt64(25) << endl;

    //Test "GetNext" functions
    //These need offset to be set before executing a GetNext command
    cout << endl;
    cds.SetOffset(0);
    cout << "Testing \"Next\" functions:" << endl;
    cout << "Offset set to 0" << endl;
    cout << "bool: ";
    cout << cds.GetNextBool() << endl;
    cout << "char: ";
    cout << cds.GetNextChar() << endl;
    cout << "uint8: ";
    cout << +cds.GetNextUInt8() << endl;
    cout << "int8: ";
    printf("%d\n",cds.GetNextInt8());
    cout << "uint16: ";
    cout << cds.GetNextUInt16() << endl;
    cout << "int16: ";
    cout << cds.GetNextInt16() << endl;
    cout << "uint32: ";
    cout << cds.GetNextUInt32() << endl;
    cout << "int32: ";
    cout << cds.GetNextInt32() << endl;
    cout << "uint64: ";
    cout << cds.GetNextUInt64() << endl;
    cout << "int64: ";
    cout << cds.GetNextInt64() << endl;

    //Test ability to recursively read using ">>" stream operator.
    cout << endl;
    cout << "Testing \"<<\" operator:" << endl;

    bool q = 0;
    char r = 0;
    cds.SetOffset(0);
    cds >> q >> r;
    cout << "bool: " << q << endl  << "char: " << r << endl;

    uint8_t s = 0;
    int8_t t = 0;
    cds.SetOffset(2);
    cds >> s >> t;
    printf("uint8_t: %d\nint8_t: %d\n",s,t);

    uint16_t u = 0;
    int16_t v = 0;
    cds.SetOffset(4);
    cds >> u >> v;
    cout << "uint16_t: " << u << endl  << "int16_t: " << v << endl;

    uint32_t w = 0;
    int32_t x = 0;
    cds.SetOffset(8);
    cds >> w >> x;
    cout << "uint32_t: " << w << endl << "int32_t: " << x << endl;

    uint64_t y = 0;
    int64_t z = 0;
    cds.SetOffset(16);
    cds >> y >> z;
    cout << "uint64_t: " << y << endl << "int64_t: " << z << endl;
}
