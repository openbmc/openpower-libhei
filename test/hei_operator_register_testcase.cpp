#include <hei_includes.hpp>
#include <register/hei_hardware_register.hpp>
#include <register/hei_operator_register.hpp>
#include <hei_chip.hpp>
#include <register/hei_register.hpp>
#include <iostream>
#include <iomanip>

#include "stdio.h"

using namespace std;
using namespace libhei;


int main()
{

    // Initialization

    Chip my_chip;
    Register * my_register = nullptr;

    cout << endl << "Constants:";
    cout << endl << "0x000000000000ffff" <<
            endl << "0x000000000000f0ff" << endl;

    uint8_t a[2];
    printf("\nCreating bsa.\n");
    BitString bsa { 16, a };
    printf("Setting bsa (oxffff).\n");
    bsa.setFieldRight( 0, 16, 0xffff );
    uint8_t b[2];
    printf("Creating bsb.\n");
    BitString bsb { 16, b };
    printf("Setting bsb (0xf0ff).\n");
    bsb.setFieldRight( 0, 16, 0xf0ff );

    printf("Creating leftside from bsa.\n");
    ConstantRegister leftside (bsa);
    printf("Creating rightside from bsb.\n");
    ConstantRegister rightside (bsb);

    // Basic function tests:
    my_register = new AndRegister(leftside,rightside);
    cout << "And (constants):   ";
    cout << "0x" << setfill('0')<<setw(16)<<hex<<my_register->
                    getBitString(my_chip)-> getFieldRight(0,16) << endl;

    my_register = new NotRegister(*my_register);
    cout << "Not:               ";
    cout << "0x" << setfill('0')<<setw(16)<<hex<<my_register->
                    getBitString(my_chip)->getFieldRight(0,16) << endl;

    cout << "LeftShift:         ";
    my_register = new LeftShiftRegister(*my_register, 4);
    cout << "0x" << setfill('0')<<setw(16)<<hex<<my_register->
                    getBitString(my_chip)->getFieldRight(0,16) << endl;

    cout << "RightShift:        ";
    my_register = new RightShiftRegister(*my_register,4);
    cout << "0x" << setfill('0')<<setw(16)<<hex<<my_register->
                    getBitString(my_chip)->getFieldRight(0,16) << endl;

    cout << "Or:                ";
    my_register = new OrRegister(leftside,rightside);
    cout << "0x" << setfill('0')<<setw(16)<<hex<<my_register->
                    getBitString(my_chip)->getFieldRight(0,16) << endl;

    // = operator
    cout << endl << "Operator \"=\" (assignment)" << endl << endl;

    cout << "    ConstantRegister:   ";
    ConstantRegister constreg = rightside;
    cout << "0x" << setfill('0')<<setw(16)<<hex<<
                    constreg.getBitString(my_chip)->getFieldRight(0,16) << endl; 
    cout << "    AndRegister:        ";
    AndRegister and_register1(leftside,rightside);
    AndRegister and_register2 = and_register1;
    cout << "0x" << setfill('0') << setw(16) << hex << and_register2.
                    getBitString(my_chip) -> getFieldRight(0,16) << endl;

    cout << "    NotRegster:         ";
    NotRegister notreg = rightside;
    cout << "0x" << setfill('0')<<setw(16)<<hex<<
                    notreg.getBitString(my_chip)->getFieldRight(0,16) << endl;

    cout << "    LeftShift Register: ";
    LeftShiftRegister lsreg1(leftside,4);
    LeftShiftRegister lsreg2 = lsreg1;
    cout << "0x" << setfill('0')<<setw(16)<<hex<<
                    lsreg2.getBitString(my_chip)->getFieldRight(0,16) << endl;

    cout << "    RightShift Register:";
    RightShiftRegister rsreg1(leftside,4);
    RightShiftRegister rsreg2 = rsreg1;
    cout << "0x" << setfill('0')<<setw(16)<<hex<<
                    rsreg2.getBitString(my_chip)->getFieldRight(0,16) << endl;

    cout << "    OrRegister:         ";
    OrRegister or_register1(leftside,rightside);
    OrRegister or_register2 = or_register1;
    cout << "0x" << setfill('0') << setw(16) << hex << or_register2.
                    getBitString(my_chip) -> getFieldRight(0,16) << endl;

    // == operator
    cout << endl << "Operator \"==\" (equality)" << endl << endl;
    ConstantRegister cr1(leftside);
    ConstantRegister cr2(rightside);
    if (cr1 == cr2) cout << "Equal" << endl;
    else cout << "Not Equal" << endl;

    AndRegister and1(leftside,rightside);
    AndRegister and2(rightside, leftside);
    if (and1 == and2) cout << "Equal" << endl;
    else cout << "Not Equal" << endl;

    NotRegister nr1(leftside);
    NotRegister nr2(rightside);
    if (nr1 == nr2) cout << "Equal" << endl;
    else cout << "Not Equal" << endl;

    LeftShiftRegister lsr1(leftside,4);
    LeftShiftRegister lsr2(leftside,5);
    if (lsr1 == lsr2) cout << "Equal" << endl;
    else cout << "Not Equal" << endl;

    RightShiftRegister rsr1(leftside,4);
    RightShiftRegister rsr2(leftside,5);
    if (lsr1 == lsr2) cout << "Equal" << endl;
    else cout << "Not Equal" << endl;

    OrRegister or1(leftside,rightside);
    OrRegister or2(rightside,leftside);
    if (or1 == or2) cout << "Equal" << endl;
    else cout << "Not Equal" << endl;

    // < operator
    AndRegister and1a(leftside,rightside);
    AndRegister and2a(rightside, leftside);
    if (and1a < and2a) cout << "Equal" << endl;
    else cout << "Not Equal" << endl;

    NotRegister nr1a(leftside);
    NotRegister nr2a(rightside);
    if (nr1a < nr2a) cout << "Equal" << endl;
    else cout << "Not Equal" << endl;

    LeftShiftRegister lsr1a(leftside,4);
    LeftShiftRegister lsr2a(leftside,5);
    if (lsr1a < lsr2a) cout << "Equal" << endl;
    else cout << "Not Equal" << endl;

    RightShiftRegister rsr1a(leftside,4);
    RightShiftRegister rsr2a(leftside,5);
    if (lsr1a < lsr2a) cout << "Equal" << endl;
    else cout << "Not Equal" << endl;

    OrRegister or1a(leftside,rightside);
    OrRegister or2a(rightside,leftside);
    if (or1a < or2a) cout << "Equal" << endl;
    else cout << "Not Equal" << endl;

    // >= operator
    AndRegister and1b(leftside,rightside);
    AndRegister and2b(rightside, leftside);
    if (and1b >= and2b) cout << "gt|equal" << endl;
    else cout << "Not Equal" << endl;

    NotRegister nr1b(leftside);
    NotRegister nr2b(rightside);
    if (nr1b >= nr2b) cout << "gt|equal" << endl;
    else cout << "Not Equal" << endl;

    LeftShiftRegister lsr1b(leftside,4);
    LeftShiftRegister lsr2b(leftside,5);
    if (lsr1b >= lsr2b) cout << "gt|equal" << endl;
    else cout << "Not Equal" << endl;

    RightShiftRegister rsr1b(leftside,4);
    RightShiftRegister rsr2b(leftside,5);
    if (lsr1b >= lsr2b) cout << "gt|equal" << endl;
    else cout << "Not Equal" << endl;

    OrRegister or1b(leftside,rightside);
    OrRegister or2b(rightside,leftside);
    if (or1b >= or2b) cout << "gt|equal" << endl;
    else cout << "Not Equal" << endl;

    return(0);
}
