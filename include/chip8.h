#ifndef CHIP8_H
#define CHIP8_H


class chip8
{
    public:
        chip8();
        ~chip8();
        unsigned char gfx[64 * 32];//pixels plus total pixels on the screen
        unsigned char key[16];
        bool drawFlag;
        void emulateCycle();
         void initialize();
        void debugRender();
        bool loadApplication(const char * filename);
    protected:
    private:
        unsigned short opcode;//opcode length
        unsigned char memory[4096];//4k of memory
        //unsigned char Regs[16];//registers
        unsigned short I;//index register
        unsigned short pc;//program counter
        unsigned char V[16];//general purpose registers
        unsigned char delay_timer;//timer register 60hz
        unsigned char sound_timer;//sound register 60hz
        //buzzer sounds whenever timer hits 0
        unsigned short stack[16];//used to remember current location before jumping
        unsigned short sp;//stack pointer

};

#endif // CHIP8_H
