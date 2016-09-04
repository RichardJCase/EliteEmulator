#include "chip8.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void chip8::initialize(){
// initialize registers and memory once

        opcode = 0;//opcode length
        I = 0;//index register
        pc = 0x200;//program counter
        sp = 0;//stack pointer

        delay_timer = 0;//timer register 60hz
        sound_timer = 0;//sound register 60hz
        //buzzer sounds whenever timer hits 0



        //clears screen
        for(int i = 0; i < 2048; ++i){
		gfx[i] = 0;
        }
            //clear memory
        for(int i = 0; i < 4096; ++i){
            memory[i] = 0;
        }
        //clear stack
        for(int i = 0; i < 16; ++i){
                stack[i] = 0;
            key[i] = V[i] = 0;
        }
        //clear registers
        for(int i = 0; i < 16; ++i){
		key[i] = V[i] = 0;
        }
                            //chip fontset
       unsigned char chip8_fontset[80] =//chip fontset
        {
          0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
          0x20, 0x60, 0x20, 0x20, 0x70, // 1
          0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
          0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
          0x90, 0x90, 0xF0, 0x10, 0x10, // 4
          0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
          0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
          0xF0, 0x10, 0x20, 0x40, 0x40, // 7
          0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
          0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
          0xF0, 0x90, 0xF0, 0x90, 0x90, // A
          0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
          0xF0, 0x80, 0x80, 0x80, 0xF0, // C
          0xE0, 0x90, 0x90, 0x90, 0xE0, // D
          0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
          0xF0, 0x80, 0xF0, 0x80, 0x80  // F
        };
        //initialize fontset
        for(int i = 0; i < 80; ++i){
            memory[i] = chip8_fontset[i];
        }


}









void chip8::emulateCycle(){

        //Fetch Opcode
        opcode = memory[pc] << 8 | memory[pc + 1];//merges both bytes and stores them
        //bit-fields of opcode
        unsigned p = (opcode>>0) & 0xF;
        unsigned y = (opcode>>4) & 0xF;
        unsigned x = (opcode>>8) & 0xF;
        unsigned kk = (opcode>>0) & 0xFF;
        unsigned nnn = (opcode>>0) & 0xFFF;
        //create aliases for registers accessed by instructions
        unsigned VX = V[x], VY = V[y], VF = V[0xF];
        //Decode Opcode
        switch(opcode & 0xF000){
        //Execute Opcode
            //get more opcodes
            /*case 0xA000:// ANNN: sets I to the address NNN
                I = opcode & 0x0FFF;
                pc += 2;
            break;*/
                //instructions
//            00CN*    Scroll display N lines down

            case 0x00E0: //Clear display
                for(int i = 0; i < 2048; ++i){
                    gfx[i] = 0;
                    drawFlag = true;
                }
                pc += 2;//continues
            break;

            case 0x00EE://     Return from subroutine
//00FB*    Scroll display 4 pixels right

//00FC*    Scroll display 4 pixels left
//00FD*    Exit CHIP interpreter
//00FE*    Disable extended screen mode
//00FF*    Enable extended screen mode for full-screen graphics
            break;
            case 0x1000: //Jump to NNN
                pc = nnn;//program counter = address at NNN
            break;
            case 0x2000://     Call subroutine at NNN
                stack[sp] = pc;//store current address in stack
                ++sp; //add to stack pointer
                pc = opcode & 0x0FFF;
            break;
            case 0x3000://     3XKK:Skip next instruction if VX == KK
                if(VX == kk){
                    pc += 4;
                }else{
                pc += 2;
                }
            break;
           // 4XKK    // Skip next instruction if VX <> KK
            case 0x4000:
                if(VX != kk){
                    pc +=4;
                }
                else{
                    pc += 2;
                }
            break;
//5XY0     Skip next instruction if VX == VY
            case 0x5000:
                if(VX == VY){
                    pc += 4;
                }else{
                    pc += 2;
                }
            break;
            case 0x6000:
                VX = kk;
            break;
            case 0x7000:
                VX = VX + kk;
                pc += 2;
            break;
//8XY0     VX := VY, VF may change
//change up code
		case 0x8000:
			switch(opcode & 0x000F)
			{
				case 0x0000: // 0x8XY0: Sets VX to the value of VY
					V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x00F0) >> 4];
					pc += 2;
				break;

				case 0x0001: // 0x8XY1: Sets VX to "VX OR VY"
					V[(opcode & 0x0F00) >> 8] |= V[(opcode & 0x00F0) >> 4];
					pc += 2;
				break;

				case 0x0002: // 0x8XY2: Sets VX to "VX AND VY"
					V[(opcode & 0x0F00) >> 8] &= V[(opcode & 0x00F0) >> 4];
					pc += 2;
				break;

				case 0x0003: // 0x8XY3: Sets VX to "VX XOR VY"
					V[(opcode & 0x0F00) >> 8] ^= V[(opcode & 0x00F0) >> 4];
					pc += 2;
				break;

				case 0x0004: // 0x8XY4: Adds VY to VX. VF is set to 1 when there's a carry, and to 0 when there isn't
					if(V[(opcode & 0x00F0) >> 4] > (0xFF - V[(opcode & 0x0F00) >> 8]))
						V[0xF] = 1; //carry
					else
						V[0xF] = 0;
					V[(opcode & 0x0F00) >> 8] += V[(opcode & 0x00F0) >> 4];
					pc += 2;
				break;

				case 0x0005: // 0x8XY5: VY is subtracted from VX. VF is set to 0 when there's a borrow, and 1 when there isn't
					if(V[(opcode & 0x00F0) >> 4] > V[(opcode & 0x0F00) >> 8])
						V[0xF] = 0; // there is a borrow
					else
						V[0xF] = 1;
					V[(opcode & 0x0F00) >> 8] -= V[(opcode & 0x00F0) >> 4];
					pc += 2;
				break;

				case 0x0006: // 0x8XY6: Shifts VX right by one. VF is set to the value of the least significant bit of VX before the shift
					V[0xF] = V[(opcode & 0x0F00) >> 8] & 0x1;
					V[(opcode & 0x0F00) >> 8] >>= 1;
					pc += 2;
				break;

				case 0x0007: // 0x8XY7: Sets VX to VY minus VX. VF is set to 0 when there's a borrow, and 1 when there isn't
					if(V[(opcode & 0x0F00) >> 8] > V[(opcode & 0x00F0) >> 4])	// VY-VX
						V[0xF] = 0; // there is a borrow
					else
						V[0xF] = 1;
					V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x00F0) >> 4] - V[(opcode & 0x0F00) >> 8];
					pc += 2;
				break;

				case 0x000E: // 0x8XYE: Shifts VX left by one. VF is set to the value of the most significant bit of VX before the shift
					V[0xF] = V[(opcode & 0x0F00) >> 8] >> 7;
					V[(opcode & 0x0F00) >> 8] <<= 1;
					pc += 2;
				break;

				default:
					printf ("Unknown opcode [0x8000]: 0x%X\n", opcode);
			}
        break;
//end changing code

            case 0x9000:
                switch(opcode & 0x0FFF){




                case 0x0000:
                    if(VX != VY){
                        pc += 4;
                    }
                    else{
                        pc += 2;
                    }
                break;
                }
            break;
//ANNN     I := NNN
            case 0xA000:
                I = nnn;
                pc +=2;
            break;
//BNNN     Jump to NNN+V0
            case 0xB000:
                pc = nnn+V[0];
            break;
//CXKK     VX := pseudorandom_number and KK
            case 0xC000:
                V[(opcode & 0x0F00) >> 8] = (rand() % 0xFF) & (opcode & 0x00FF);
                pc += 2;
            break;

//DXYN*    Show N-byte sprite from M(I) at coords (VX,VY), VF :=
		case 0xD000: // DXYN: Draws a sprite at coordinate (VX, VY) that has a width of 8 pixels and a height of N pixels.
					 // Each row of 8 pixels is read as bit-coded starting from memory location I;
					 // I value doesn't change after the execution of this instruction.
					 // VF is set to 1 if any screen pixels are flipped from set to unset when the sprite is drawn,
					 // and to 0 if that doesn't happen
		{
			unsigned short x = V[(opcode & 0x0F00) >> 8];
			unsigned short y = V[(opcode & 0x00F0) >> 4];
			unsigned short height = opcode & 0x000F;
			unsigned short pixel;

			V[0xF] = 0;
			for (int yline = 0; yline < height; yline++)
			{
				pixel = memory[I + yline];
				for(int xline = 0; xline < 8; xline++)
				{
					if((pixel & (0x80 >> xline)) != 0)
					{
						if(gfx[(x + xline + ((y + yline) * 64))] == 1)
						{
							V[0xF] = 1;
						}
						gfx[x + xline + ((y + yline) * 64)] ^= 1;
					}
				}
			}

			drawFlag = true;
			pc += 2;
		}
		break;

//         collision. If N=0 and extended mode, show 16x16 spritse.
//EX9E     Skip next instruction if key VX pressed
            case 0xE000:
                switch(opcode & 0x000F){
                    case 0x000E:
                        if(key[V[(opcode & 0x0F00) >> 8]] != 0)
                            pc += 4;
                        else
                            pc += 2;
                    break;
                    case 0x0001:
                        if(key[V[(opcode & 0x0F00) >> 8]] == 0)
                            pc += 4;
                        else
                            pc += 2;
                    break;
                    default:
                        printf("unknown opcode");

                }
            break;
//EXA1     Skip next instruction if key VX not pressed

                case 0xF000:
                    switch(opcode & 0x00FF){

        //FX07     VX := delay_timer
                    case 0x0007:
                        VX = delay_timer;
                        pc += 2;
                    break;
                    //FX15     delay_timer := VX
                    case 0x0015:
                        delay_timer = VX;
                        pc += 2;
                    break;
                    //FX18     sound_timer := VX
                    case 0x0018:
                        sound_timer = VX;
                        pc += 2;
                    break;
        //FX1E     I := I + VX
                    case 0x001E:
                        I += VX;
                        pc += 2;
                    break;
        //FX29     Point I to 5-byte font sprite for hex character VX
                    case 0x0029:
                        I = V[(opcode & 0x0F00) >> 8] * 0x5;
                        pc += 2;
                    break;
                    //FX33     Store BCD representation of VX in M(I)..M(I+2)
                    case 0x0033:
                            memory[I]     = V[(opcode & 0x0F00) >> 8] / 100;
                            memory[I + 1] = (V[(opcode & 0x0F00) >> 8] / 10) % 10;
                            memory[I + 2] = (V[(opcode & 0x0F00) >> 8] % 100) % 10;
                            pc += 2;
                    break;
                    //FX55     Store V0..VX in memory starting at M(I)
                    case 0x0055:
                        for (int i = 0; i <= ((opcode & 0x0F00) >> 8); ++i)
                            memory[I + i] = V[i];

                            // On the original interpreter, when the operation is done, I = I + X + 1.
                            I += ((opcode & 0x0F00) >> 8) + 1;
                            pc += 2;
                    break;
                    //FX65     Read V0..VX from memory starting at M(I)
                    case 0x0065:
                        for (int i = 0; i <= VX; ++i){
                            V[i] = memory[I+i];
                        }
                        I += VX +1;
                        pc += 2;
                    break;
        //FX0A     wait for keypress, store hex value of key in VX
                    case 0x000A:
                            bool keyPress = false;

                            for(int i = 0; i < 16; ++i)
                            {
                                if(key[i] != 0)
                                {
                                    V[(opcode & 0x0F00) >> 8] = i;
                                    keyPress = true;
                                }
                            }

                            // If we didn't received a keypress, skip this cycle and try again.
                            if(!keyPress){
                                return;

                            pc += 2;
                        }
                        break;


        //FX30*    Point I to 10-byte font sprite for digit VX (0..9)

        //FX75*    Store V0..VX in RPL user flags (X <= 7)
        //FX85*    Read V0..VX from RPL user flags (X <= 7)
        }
        break;





            default:
                printf("Unknown opcode: 0x%X\n", opcode);
        }



        //update timers
        if(delay_timer > 0){
            --delay_timer;
        }
        if(sound_timer > 0){
            if(sound_timer == 1){
                printf("BEEP!\n");
                --sound_timer;
            }
        }
}

void chip8::debugRender()
{
    for(int y = 0; y < 32; ++y)
    {
        for(int x = 0; x < 64; ++x){
            if(gfx[(y*64) + x] = 0){
                printf("O");
            }else{
                printf(" ");
                }
        }
        printf("\n");
    }
    printf("\n");

}




bool chip8::loadApplication(const char * filename)
{
	initialize();
	printf("Loading: %s\n", filename);

	// Open file
	FILE * pFile = fopen(filename, "rb");
	if (pFile == NULL)
	{
		fputs ("File error", stderr);
		return false;
	}

	// Check file size
	fseek(pFile , 0 , SEEK_END);
	long lSize = ftell(pFile);
	rewind(pFile);
	printf("Filesize: %d\n", (int)lSize);

	// Allocate memory to contain the whole file
	char * buffer = (char*)malloc(sizeof(char) * lSize);
	if (buffer == NULL)
	{
		fputs ("Memory error", stderr);
		return false;
	}

	// Copy the file into the buffer
	size_t result = fread (buffer, 1, lSize, pFile);
	if (result != lSize)
	{
		fputs("Reading error",stderr);
		return false;
	}

	// Copy buffer to Chip8 memory
	if((4096-512) > lSize)
	{
		for(int i = 0; i < lSize; ++i)
			memory[i + 512] = buffer[i];
	}
	else
		printf("Error: ROM too big for memory");

	// Close file, free buffer
	fclose(pFile);
	free(buffer);

	return true;
}





//update timers


chip8::chip8()
{
    //ctor
}

chip8::~chip8()
{
    //dtor
}
