#include "chip_data_stream.hpp"

using namespace std;

int main ()
{

    //Create a buffer with well determined data in well determined locations
    //uint8_t streamData[32]={0x01,0x80,0x80,0x80,0x80,0,0x80,0,0x80,0,0,0,
    //size_t streamData[32]={0x01,0x80,0x80,0x80,0x81,0,0x82,0,0x80,0,0,0,
    //uint8_t streamData[32]={0x01,0x80,0x80,0x80,0x80,0,0x80,0,0x80,0,0,0,
    uint8_t streamData[32]={0x01,0x80,0x80,0x80,0x00,0x80,0x00,0x80,0,0,0,0x80,
                          0,0,0,0x80,0,0,0,0,0,0,0,0x80,0,0,0,0,0,0,0,0x80};

    //Instantiate a cds object
    ChipDataStream cds(streamData,32);

    //Test ability to recursively read using ">>" stream operator.
    cout << "Testing \">>\" operator:" << endl;

    bool q = 0;
    char r = 0;
    cds.setOffset(0);
    cds >> q >> r;
    cout << "bool: " << q << endl  << "char: " << r << endl;

    uint8_t s = 0;
    int8_t t = 0;
    cds.setOffset(2);
    cds >> s >> t;
    printf("uint8_t: %d\nint8_t: %d\n",s,t);

    uint16_t u = 0;
    int16_t v = 0;
    cds.setOffset(4);
    cds >> u >> v;
    cout << "uint16_t: " << u << endl  << "int16_t: " << v << endl;

    uint32_t w = 0;
    int32_t x = 0;
    cds.setOffset(8);
    cds >> w >> x;
    cout << "uint32_t: " << w << endl << "int32_t: " << x << endl;

    uint64_t y = 0;
    int64_t z = 0;
    cds.setOffset(16);
    cds >> y >> z;
    cout << "uint64_t: " << y << endl << "int64_t: " << z << endl;
    cout << endl;

    //Test long recursive reads using ">>" stream operator.
    cout << "Testing long \">>\" operator reads:" << endl;
    cds.setOffset(0);
    cds >>q>>r>>s>>t>>u>>v>>w>>x>>y>>z;
    cout <<q<<endl<<r<<endl;
    printf("%d\n%d\n",s,t);
    cout <<u<<endl<<v<<endl;
    cout <<w<<endl<<x<<endl<<y<<endl<<z<<endl<<endl;
}
