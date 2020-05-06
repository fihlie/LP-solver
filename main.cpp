#include <stdio.h>
#include <fstream>
#include <iostream>
#include <bits/stdc++.h>
#include <array>
#include <algorithm>
#include <string>

using namespace std;
unsigned char memory[2<<15];

unsigned short ax = 0, bx = 0, cx = 0, dx = 0, di = 0, si = 0, bp = 0;
unsigned short sp = (2<<15)-2;

unsigned short *pax = &ax;
unsigned short *pbx = &bx;
unsigned short *pcx = &cx;
unsigned short *pdx = &dx;
unsigned short *pdi = &di;
unsigned short *psi = &si;
unsigned short *pbp = &bp;
unsigned short *psp = &sp;

unsigned char *pah = (unsigned char *) ( ( (unsigned char *) &ax) + 1);
unsigned char *pal = (unsigned char *) &ax;
unsigned char *pbh = (unsigned char *) ( ( (unsigned char *) &bx) + 1);
unsigned char *pbl = (unsigned char *) &bx;
unsigned char *pch = (unsigned char *) ( ( (unsigned char *) &cx) + 1);
unsigned char *pcl = (unsigned char *) &cx;
unsigned char *pdh = (unsigned char *) ( ( (unsigned char *) &dx) + 1);
unsigned char *pdl = (unsigned char *) &dx;

bool ZF=false, CF=false, AF=false, SF=false, OF=false;


unsigned short byte_to_word(unsigned byteHigh, unsigned byteLow) {
    unsigned short word = ( ( byteHigh & 0xFF ) << 8 ) | ( byteLow & 0xFF );
    return word;
}


struct label {
    string name;
    int line;
};
struct instruct {
    string name;
    int line;
    string operand1;
    string operand2;
};
struct variable {
    string name;
    bool isWord;
    unsigned short mem;
    unsigned short wordValue;
    unsigned char byteValue;
};


//MOV methods by operand types
template <typename regtype>
void mov_reg_word (regtype *preg, unsigned short word) {
    *preg = word;
}
template <typename regtype>
void mov_reg_byte (regtype *preg, unsigned char byte) {
    *preg = byte;
}
template <typename regtype>
void mov_reg_reg (regtype *preg1, regtype *preg2) {
    *preg1 = *preg2;
}
template <typename regtype>
void mov_reg_mem_word (regtype *preg, unsigned short memLocation) {
    unsigned short word = byte_to_word(memory[memLocation+1], memory[memLocation]);
    *preg = word;
}
template <typename regtype>
void mov_reg_mem_byte (regtype *preg, unsigned short memLocation) {
    *preg = memory[memLocation];
}
void mov_mem_byte (unsigned short memLocation, unsigned char byte) {
    memory[memLocation] = byte;
}
void mov_mem_word (unsigned short memLocation, unsigned short word) {
    unsigned char wordLow = word & 0xff;
    unsigned char wordHigh = (word >> 8);
    memory[memLocation+1] = wordHigh;
    memory[memLocation] = wordLow;
}
template <typename regtype>
void mov_mem_byte_reg (regtype *preg, unsigned short memLocation) {
    memory[memLocation] = *preg;
}
template <typename regtype>
void mov_mem_word_reg (regtype *preg, unsigned short memLocation) {
    unsigned char pregLow = *preg & 0xff;
    unsigned char pregHigh = (*preg >> 8);
    memory[memLocation+1] = pregHigh;
    memory[memLocation] = pregLow;
}

//ADD methods by operand types
template <typename regtype>
void add_reg_byte(regtype *preg, unsigned char byte) {
    if(*preg<0x80&&byte<0x80){
        if(*preg+byte>=0x80){
            OF=true;
        } else
            OF=0;
    }
    unsigned short temp;
    temp=*preg + byte;
    if(temp>0xff){
        *preg=temp%0x100;
        CF=true;
    }
    else{
        *preg=temp;
        CF=false;
    }
    if(*preg>=0x80) {
        SF=1;
    }
    else{
        SF=0;
    }
    if(*preg==0){
        ZF=true;
    }
    else{
        ZF=false;
    }
}
template <typename regtype>
void add_reg_word(regtype *preg, unsigned short word) {
    SF=0;
    if(*preg<0x8000&&word<0x8000){
        if(*preg+word>=0x8000){
            OF=true;
        } else
            OF=0;
    }
    unsigned int temp=*preg + word;
    if(temp>0xffff){
        *preg=temp%0x10000;
        CF=true;
    }
    else{
        *preg=temp;
        CF=false;
    }
    if(*preg>=0x8000) {
        SF=1;
    }
    else{
        SF=0;
    }
    if(*preg==0){
        ZF=true;
    }
    else{
        ZF=false;
    }}
void add_mem_byte(unsigned short memLocation, unsigned char byte) {
    SF = 0;
    if (memory[memLocation] < 0x80 && byte < 0x80) {
        if (memory[memLocation] + byte >= 0x80) {
            OF = true;
        } else
            OF = 0;
    }
    unsigned short temp;
    temp=memory[memLocation] + byte;
    if (temp > 0xff) {
        memory[memLocation] = temp % 0x100;
        CF = true;
    } else {
        memory[memLocation] = temp;
        CF = false;
    }
    if (memory[memLocation] >= 0x80) {
        SF = 1;
    } else {
        SF = 0;
    }
    if (memory[memLocation] == 0) {
        ZF = true;
    } else {
        ZF = false;
    }
}

void add_mem_word(unsigned short memLocation, unsigned short word) {
    unsigned short memWord = byte_to_word(memory[memLocation+1], memory[memLocation]);
    if(memWord<0x8000&&word<0x8000){
        if(memWord+word>=0x8000){
            OF=true;
        } else
            OF=0;
    }
    unsigned int temp=memWord + word;
    if(temp>0xffff){
        memWord=temp%0x10000;
        CF=true;
    }
    else{
        memWord=temp;
        CF=false;
    }
    if(memWord>=0x8000) {
        SF=1;
    }
    else{
        SF=0;
    }
    if(memWord==0){
        ZF=true;
    }
    else{
        ZF=false;
    }
    unsigned char memWordLow = memWord & 0xff;
    unsigned char memWordHigh = (memWord >> 8);
    memory[memLocation+1] = memWordHigh;
    memory[memLocation] = memWordLow;
}template <typename regtype>
void add_mem_byte_register(unsigned short memLocation, regtype *preg) {
    SF = 0;
    if (memory[memLocation] < 0x80 && *preg < 0x80) {
        if (memory[memLocation] + *preg >= 0x80) {
            OF = true;
        } else
            OF = 0;
    }
    unsigned short temp;
    temp=memory[memLocation] + *preg;
    if (temp > 0xff) {
        memory[memLocation] = temp % 0x100;
        CF = true;
    } else {
        memory[memLocation] = temp;
        CF = false;
    }
    if (memory[memLocation] >= 0x80) {
        SF = 1;
    } else {
        SF = 0;
    }
    if (memory[memLocation] == 0) {
        ZF = true;
    } else {
        ZF = false;
    }
}
template <typename regtype>
void add_mem_word_register(unsigned short memLocation, regtype *preg) {
    unsigned short memWord = byte_to_word(memory[memLocation+1], memory[memLocation]);
    if(memWord<0x8000&&*preg<0x8000){
        if(memWord+*preg>=0x8000){
            OF=true;
        } else
            OF=0;
    }
    unsigned int temp=*preg + memWord;
    if(temp>0xffff){
        memWord=temp%0x10000;
        CF=true;
    }
    else{
        memWord=temp;
        CF=false;
    }
    if(memWord>=0x8000) {
        SF=1;
    }
    else{
        SF=0;
    }
    if(memWord==0){
        ZF=true;
    }
    else{
        ZF=false;
    }    unsigned char memWordLow = memWord & 0xff;
    unsigned char memWordHigh = (memWord >> 8);
    memory[memLocation+1] = memWordHigh;
    memory[memLocation] = memWordLow;
}
template <typename regtype>
void add_register_byte_mem(regtype *preg, unsigned short memLocation) {
    SF=0;
    if(*preg<0x80&&memory[memLocation]<0x80){
        if(*preg+memory[memLocation]>=0x80){
            OF=true;
        } else
            OF=0;
    }
    unsigned short temp;
    temp=*preg + memory[memLocation];
    if(temp>0xff){
        *preg=temp%0x100;
        CF=true;
    }
    else{
        *preg=temp;
        CF=false;
    }
    if (*preg >= 0x80) {
        SF = 1;
    } else {
        SF = 0;
    }
    if(*preg==0){
        ZF=true;
    }
    else{
        ZF=false;
    }
}
template <typename regtype>
void add_register_word_mem(regtype *preg, unsigned short memLocation) {
    unsigned short word= byte_to_word(memory[memLocation+1],memory[memLocation]);
    SF=0;
    if(*preg<0x8000&&word<0x8000){
        if(*preg+word>=0x8000){
            OF=true;
        } else
            OF=0;
    }
    unsigned int temp=*preg + word;
    if(temp>0xffff){
        *preg=temp%0x10000;
        CF=true;
    }
    else{
        *preg=temp;
        CF=false;
    }
    if (*preg >= 0x8000) {
        SF = 1;
    } else {
        SF = 0;
    }
    if(*preg==0){
        ZF=true;
    }
    else{
        ZF=false;
    }
}

template <typename regtype>
void add_bregister_bregister(regtype *preg1, regtype *preg2) {
    SF=0;
    if(*preg1<0x80&&*preg2<0x80){
        if(*preg1+*preg2>=0x80){
            OF=true;
        } else
            OF=0;
    }
    unsigned short temp;
    temp=*preg1 + *preg2;
    if(temp>0xff){
        *preg1=temp%0x100;
        CF=true;
    }
    else{
        *preg1=temp;
        CF=false;
    }
    if (*preg1 >= 0x80) {
        SF = 1;
    } else {
        SF = 0;
    }
    if(*preg1==0){
        ZF=true;
    }
    else{
        ZF=false;
    }
}
template <typename regtype>
void add_wregister_wregister(regtype *preg1, regtype *preg2) {
    SF=0;
    if(*preg1<0x8000&&*preg2<0x8000){
        if(*preg1+*preg2>=0x8000){
            OF=true;
        } else
            OF=0;
    }
    unsigned int temp=*preg1 + *preg2;
    if(temp>0xffff){
        *preg1=temp%0x10000;
        CF=true;
    }
    else{
        *preg1=temp;
        CF=false;
    }
    if (*preg1 >= 0x8000) {
        SF = 1;
    } else {
        SF = 0;
    }
    if(*preg1==0){
        ZF=true;
    }
    else{
        ZF=false;
    }
}

//AND methods by operand types
template <typename regtype>
void and_bregister_bregister(regtype *preg1, regtype *preg2) {
    OF=0;
    CF=0;
    *preg1 = *preg1 & *preg2;
    if(*preg1==0){
        ZF=1;
    }
    else{
        ZF=0;
    }
    if(*preg1>=0x80){
        SF=1;
    }
    else{
        SF=0;
    }
}
template <typename regtype>
void and_wregister_wregister(regtype *preg1, regtype *preg2) {
    OF=0;
    CF=0;
    *preg1 = *preg1 & *preg2;
    if(*preg1==0){
        ZF=1;
    }
    else{
        ZF=0;
    }
    if(*preg1>=0x8000){
        SF=1;
    }
    else{
        SF=0;
    }
}
template <typename regtype>
void and_register_byte_memory(regtype *preg, unsigned short memLocation) {
    OF=0;
    CF=0;
    *preg = *preg & memory[memLocation];
    if(*preg==0){
        ZF=1;
    }
    else{
        ZF=0;
    }
    if(*preg>=0x80){
        SF=1;
    }
    else{
        SF=0;
    }
}
template <typename regtype>
void and_register_word_memory(regtype *preg, unsigned short memLocation) {
    unsigned short memWord = byte_to_word(memory[memLocation+1], memory[memLocation]);
    OF=0;
    CF=0;
    *preg = *preg & memWord;
    if(*preg==0){
        ZF=1;
    }
    else{
        ZF=0;
    }
    if(*preg>=0x8000){
        SF=1;
    }
    else{
        SF=0;
    }
}
template <typename regtype>
void and_memory_byte_register(regtype *preg, unsigned short memLocation) {
    OF=0;
    CF=0;
    memory[memLocation] = *preg & memory[memLocation];
    if(memory[memLocation]==0){
        ZF=1;
    }
    else{
        ZF=0;
    }
    if(memory[memLocation]>=0x80){
        SF=1;
    }
    else{
        SF=0;
    }}
template <typename regtype>
void and_memory_word_register(regtype *preg, unsigned short memLocation) {
    OF=0;
    CF=0;
    unsigned short memWord = byte_to_word(memory[memLocation+1], memory[memLocation]);
    memWord = memWord & *preg;
    if(memWord==0){
        ZF=1;
    }
    else{
        ZF=0;
    }
    if(memWord>=0x8000){
        SF=1;
    }
    else{
        SF=0;
    }
    unsigned char memWordLow = memWord & 0xff;
    unsigned char memWordHigh = (memWord >> 8);
    memory[memLocation] = memWordLow;
    memory[memLocation+1] = memWordHigh;
}
template <typename regtype>
void and_register_byte(regtype *preg, unsigned char byte) {
    OF=0;
    CF=0;
    *preg = *preg & byte;
    if(*preg==0){
        ZF=1;
    }
    else{
        ZF=0;
    }
    if(*preg>=0x80){
        SF=1;
    }
    else{
        SF=0;
    }}
template <typename regtype>
void and_register_word(regtype *preg, unsigned short word) {
    OF=0;
    CF=0;
    *preg = *preg & word;
    if(*preg==0){
        ZF=1;
    }
    else{
        ZF=0;
    }
    if(*preg>=0x8000){
        SF=1;
    }
    else{
        SF=0;
    }}
void and_memory_byte(unsigned short memLocation, unsigned char byte) {
    OF=0;
    CF=0;
    memory[memLocation] = byte & memory[memLocation];
    if(memory[memLocation]==0){
        ZF=1;
    }
    else{
        ZF=0;
    }
    if(memory[memLocation]>=0x80){
        SF=1;
    }
    else{
        SF=0;
    }
}
void and_memory_word(unsigned short memLocation, unsigned short word) {
    OF=0;
    CF=0;
    unsigned short memWord = byte_to_word(memory[memLocation+1], memory[memLocation]);
    memWord = memWord & word;
    if(memWord==0){
        ZF=1;
    }
    else{
        ZF=0;
    }
    if(memWord>=0x8000){
        SF=1;
    }
    else{
        SF=0;
    }
    unsigned char memWordLow = memWord & 0xff;
    unsigned char memWordHigh = (memWord >> 8);
    memory[memLocation+1] = memWordHigh;
    memory[memLocation] = memWordLow;
}

//OR
template <typename regtype>
void or_bregister_bregister(regtype *preg1, regtype *preg2) {
    OF=0;
    CF=0;
    *preg1 = *preg1 | *preg2;
    if(*preg1==0){
        ZF=1;
    }
    else{
        ZF=0;
    }
    if(*preg1>=0x80){
        SF=1;
    }
    else{
        SF=0;
    }
}
template <typename regtype>
void or_wregister_wregister(regtype *preg1, regtype *preg2) {
    OF=0;
    CF=0;
    *preg1 = *preg1 | *preg2;
    if(*preg1==0){
        ZF=1;
    }
    else{
        ZF=0;
    }
    if(*preg1>=0x8000){
        SF=1;
    }
    else{
        SF=0;
    }
}
template <typename regtype>
void or_register_byte_memory(regtype *preg, unsigned short memLocation) {
    OF=0;
    CF=0;
    *preg = *preg | memory[memLocation];
    if(*preg==0){
        ZF=1;
    }
    else{
        ZF=0;
    }
    if(*preg>=0x80){
        SF=1;
    }
    else{
        SF=0;
    }
}
template <typename regtype>
void or_register_word_memory(regtype *preg, unsigned short memLocation) {
    unsigned short memWord = byte_to_word(memory[memLocation+1], memory[memLocation]);
    OF=0;
    CF=0;
    *preg = *preg | memWord;
    if(*preg==0){
        ZF=1;
    }
    else{
        ZF=0;
    }
    if(*preg>=0x8000){
        SF=1;
    }
    else{
        SF=0;
    }
}
template <typename regtype>
void or_memory_byte_register(regtype *preg, unsigned short memLocation) {
    OF=0;
    CF=0;
    memory[memLocation] = *preg | memory[memLocation];
    if(memory[memLocation]==0){
        ZF=1;
    }
    else{
        ZF=0;
    }
    if(memory[memLocation]>=0x80){
        SF=1;
    }
    else{
        SF=0;
    }}
template <typename regtype>
void or_memory_word_register(regtype *preg, unsigned short memLocation) {
    OF=0;
    CF=0;
    unsigned short memWord = byte_to_word(memory[memLocation+1], memory[memLocation]);
    memWord = memWord | *preg;
    if(memWord==0){
        ZF=1;
    }
    else{
        ZF=0;
    }
    if(memWord>=0x8000){
        SF=1;
    }
    else{
        SF=0;
    }
    unsigned char memWordLow = memWord & 0xff;
    unsigned char memWordHigh = (memWord >> 8);
    memory[memLocation] = memWordLow;
    memory[memLocation+1] = memWordHigh;
}
template <typename regtype>
void or_register_byte(regtype *preg, unsigned char byte) {
    OF=0;
    CF=0;
    *preg = *preg | byte;
    if(*preg==0){
        ZF=1;
    }
    else{
        ZF=0;
    }
    if(*preg>=0x80){
        SF=1;
    }
    else{
        SF=0;
    }}
template <typename regtype>
void or_register_word(regtype *preg, unsigned short word) {
    OF=0;
    CF=0;
    *preg = *preg | word;
    if(*preg==0){
        ZF=1;
    }
    else{
        ZF=0;
    }
    if(*preg>=0x8000){
        SF=1;
    }
    else{
        SF=0;
    }}
void or_memory_byte(unsigned short memLocation, unsigned char byte) {
    OF=0;
    CF=0;
    memory[memLocation] = byte | memory[memLocation];
    if(memory[memLocation]==0){
        ZF=1;
    }
    else{
        ZF=0;
    }
    if(memory[memLocation]>=0x80){
        SF=1;
    }
    else{
        SF=0;
    }
}
void or_memory_word(unsigned short memLocation, unsigned short word) {
    OF=0;
    CF=0;
    unsigned short memWord = byte_to_word(memory[memLocation+1], memory[memLocation]);
    memWord = memWord | word;
    if(memWord==0){
        ZF=1;
    }
    else{
        ZF=0;
    }
    if(memWord>=0x8000){
        SF=1;
    }
    else{
        SF=0;
    }
    unsigned char memWordLow = memWord & 0xff;
    unsigned char memWordHigh = (memWord >> 8);
    memory[memLocation+1] = memWordHigh;
    memory[memLocation] = memWordLow;
}

//XOR
template <typename regtype>
void xor_bregister_bregister(regtype *preg1, regtype *preg2) {
    OF=0;
    CF=0;
    *preg1 = *preg1 ^ *preg2;
    if(*preg1==0){
        ZF=1;
    }
    else{
        ZF=0;
    }
    if(*preg1>=0x80){
        SF=1;
    }
    else{
        SF=0;
    }
}
template <typename regtype>
void xor_wregister_wregister(regtype *preg1, regtype *preg2) {
    OF=0;
    CF=0;
    *preg1 = *preg1 ^ *preg2;
    if(*preg1==0){
        ZF=1;
    }
    else{
        ZF=0;
    }
    if(*preg1>=0x8000){
        SF=1;
    }
    else{
        SF=0;
    }
}
template <typename regtype>
void xor_register_byte_memory(regtype *preg, unsigned short memLocation) {
    OF=0;
    CF=0;
    *preg = *preg ^ memory[memLocation];
    if(*preg==0){
        ZF=1;
    }
    else{
        ZF=0;
    }
    if(*preg>=0x80){
        SF=1;
    }
    else{
        SF=0;
    }
}
template <typename regtype>
void xor_register_word_memory(regtype *preg, unsigned short memLocation) {
    unsigned short memWord = byte_to_word(memory[memLocation+1], memory[memLocation]);
    OF=0;
    CF=0;
    *preg = *preg ^ memWord;
    if(*preg==0){
        ZF=1;
    }
    else{
        ZF=0;
    }
    if(*preg>=0x8000){
        SF=1;
    }
    else{
        SF=0;
    }
}
template <typename regtype>
void xor_memory_byte_register(regtype *preg, unsigned short memLocation) {
    OF=0;
    CF=0;
    memory[memLocation] = *preg ^ memory[memLocation];
    if(memory[memLocation]==0){
        ZF=1;
    }
    else{
        ZF=0;
    }
    if(memory[memLocation]>=0x80){
        SF=1;
    }
    else{
        SF=0;
    }}
template <typename regtype>
void xor_memory_word_register(regtype *preg, unsigned short memLocation) {
    OF=0;
    CF=0;
    unsigned short memWord = byte_to_word(memory[memLocation+1], memory[memLocation]);
    memWord = memWord ^ *preg;
    if(memWord==0){
        ZF=1;
    }
    else{
        ZF=0;
    }
    if(memWord>=0x8000){
        SF=1;
    }
    else{
        SF=0;
    }
    unsigned char memWordLow = memWord & 0xff;
    unsigned char memWordHigh = (memWord >> 8);
    memory[memLocation] = memWordLow;
    memory[memLocation+1] = memWordHigh;
}
template <typename regtype>
void xor_register_byte(regtype *preg, unsigned char byte) {
    OF=0;
    CF=0;
    *preg = *preg ^ byte;
    if(*preg==0){
        ZF=1;
    }
    else{
        ZF=0;
    }
    if(*preg>=0x80){
        SF=1;
    }
    else{
        SF=0;
    }}
template <typename regtype>
void xor_register_word(regtype *preg, unsigned short word) {
    OF=0;
    CF=0;
    *preg = *preg ^ word;
    if(*preg==0){
        ZF=1;
    }
    else{
        ZF=0;
    }
    if(*preg>=0x8000){
        SF=1;
    }
    else{
        SF=0;
    }}
void xor_memory_byte(unsigned short memLocation, unsigned char byte) {
    OF=0;
    CF=0;
    memory[memLocation] = byte ^ memory[memLocation];
    if(memory[memLocation]==0){
        ZF=1;
    }
    else{
        ZF=0;
    }
    if(memory[memLocation]>=0x80){
        SF=1;
    }
    else{
        SF=0;
    }
}
void xor_memory_word(unsigned short memLocation, unsigned short word) {
    OF=0;
    CF=0;
    unsigned short memWord = byte_to_word(memory[memLocation+1], memory[memLocation]);
    memWord = memWord ^ word;
    if(memWord==0){
        ZF=1;
    }
    else{
        ZF=0;
    }
    if(memWord>=0x8000){
        SF=1;
    }
    else{
        SF=0;
    }
    unsigned char memWordLow = memWord & 0xff;
    unsigned char memWordHigh = (memWord >> 8);
    memory[memLocation+1] = memWordHigh;
    memory[memLocation] = memWordLow;
}
//SUB methods by operand types
template <typename regtype>
void sub_reg_byte(regtype *preg, unsigned char byte) {
    if(*preg>=0x80&&byte<0x80){
        *preg -= byte;
        if(*preg<0x80){
            SF=0;
            OF=1;
            if(*preg==0){
                ZF=1;
            }
            else{
                ZF=0;
            }
        }
        else{
            SF=1;
            OF=0;
            ZF=0;
        }
        CF=0;
    }
    else if(*preg<0x80&&byte>=0x80){
        *preg= 0xFF-(byte-*preg);
        if(*preg>=0x80){
            SF=1;
            OF=1;
            ZF=0;
        }
        else{
            SF=0;
            OF=0;
            if(*preg==0){
                ZF=1;
            }
            else{
                ZF=0;
            }
        }
        CF=1;
    }
    else{
        OF=0;
        if(*preg>=byte){
            *preg -= byte;
            CF=0;
        }
        else{
            *preg= 0xFF-(byte-*preg);
            CF=1;
        }
        if(*preg==0){
            ZF=1;
        }
        else{ZF=0;}
        if(*preg>=0x80){
            SF=1;
        }
        else{
            SF=0;
        }
    }
}
template <typename regtype>
void sub_reg_word(regtype *preg, unsigned short word) {
    if(*preg>=0x8000&&word<0x8000){
        *preg -= word;
        if(*preg<0x8000){
            SF=0;
            OF=1;
            if(*preg==0){
                ZF=1;
            }
            else{
                ZF=0;
            }
        }
        else{
            SF=1;
            OF=0;
            ZF=0;
        }
        CF=0;
    }
    else if(*preg<0x8000&&word>=0x8000){
        *preg= 0xffff-(word-*preg);
        if(*preg>=0x8000){
            SF=1;
            OF=1;
            ZF=0;
        }
        else{
            SF=0;
            OF=0;
            if(*preg==0){
                ZF=1;
            }
            else{
                ZF=0;
            }
        }
        CF=1;
    }
    else{
        OF=0;
        if(*preg>=word){
            *preg -= word;
            CF=0;
        }
        else{
            *preg= 0xffff-(word-*preg);
            CF=1;
        }
        if(*preg==0){
            ZF=1;
        }
        else{ZF=0;}
        if(*preg>=0x8000){
            SF=1;
        }
        else{
            SF=0;
        }
    }
}
void sub_mem_byte(unsigned short memLocation, unsigned char byte) {
    if(memory[memLocation]>=0x80&&byte<0x80){
        memory[memLocation] -= byte;
        if(memory[memLocation]<0x80){
            SF=0;
            OF=1;
            if(memory[memLocation]==0){
                ZF=1;
            }
            else{
                ZF=0;
            }
        }
        else{
            SF=1;
            OF=0;
            ZF=0;
        }
        CF=0;
    }
    else if(memory[memLocation]<0x80&&byte>=0x80){
        memory[memLocation]= 0xFF-(byte-memory[memLocation]);
        if(memory[memLocation]>=0x80){
            SF=1;
            OF=1;
            ZF=0;
        }
        else{
            SF=0;
            OF=0;
            if(memory[memLocation]==0){
                ZF=1;
            }
            else{
                ZF=0;
            }
        }
        CF=1;
    }
    else{
        OF=0;
        if(memory[memLocation]>=byte){
            memory[memLocation] -= byte;
            CF=0;
        }
        else{
            memory[memLocation]= 0xFF-(byte-memory[memLocation]);
            CF=1;
        }
        if(memory[memLocation]==0){
            ZF=1;
        }
        else{ZF=0;}
        if(memory[memLocation]>=0x80){
            SF=1;
        }
        else{
            SF=0;
        }
    }
}
void sub_mem_word(unsigned short memLocation, unsigned short word) {
    unsigned short memWord = byte_to_word(memory[memLocation+1], memory[memLocation]);
    if(memWord>=0x8000&&word<0x8000){
        memWord -= word;
        if(memWord<0x8000){
            SF=0;
            OF=1;
            if(memWord==0){
                ZF=1;
            }
            else{
                ZF=0;
            }
        }
        else{
            SF=1;
            OF=0;
            ZF=0;
        }
        CF=0;
    }
    else if(memWord<0x8000&&word>=0x8000){
        memWord= 0xffff-(word-memWord);
        if(memWord>=0x8000){
            SF=1;
            OF=1;
            ZF=0;
        }
        else{
            SF=0;
            OF=0;
            if(memWord==0){
                ZF=1;
            }
            else{
                ZF=0;
            }
        }
        CF=1;
    }
    else{
        OF=0;
        if(memWord>=word){
            memWord -= word;
            CF=0;
        }
        else{
            memWord= 0xffff-(word-memWord);
            CF=1;
        }
        if(memWord==0){
            ZF=1;
        }
        else{ZF=0;}
        if(memWord>=0x8000){
            SF=1;
        }
        else{
            SF=0;
        }
    }
    unsigned char memWordLow = memWord & 0xff;
    unsigned char memWordHigh = (memWord >> 8);
    memory[memLocation+1] = memWordHigh;
    memory[memLocation] = memWordLow;
}
template <typename regtype>
void sub_mem_byte_register(unsigned short memLocation, regtype *preg) {
    if(memory[memLocation]>=0x80&&*preg<0x80){
        memory[memLocation] -= *preg;
        if(memory[memLocation]<0x80){
            SF=0;
            OF=1;
            if(memory[memLocation]==0){
                ZF=1;
            }
            else{
                ZF=0;
            }
        }
        else{
            SF=1;
            OF=0;
            ZF=0;
        }
        CF=0;
    }
    else if(memory[memLocation]<0x80&&*preg>=0x80){
        memory[memLocation]= 0xFF-(*preg-memory[memLocation]);
        if(memory[memLocation]>=0x80){
            SF=1;
            OF=1;
            ZF=0;
        }
        else{
            SF=0;
            OF=0;
            if(memory[memLocation]==0){
                ZF=1;
            }
            else{
                ZF=0;
            }
        }
        CF=1;
    }
    else{
        OF=0;
        if(memory[memLocation]>=*preg){
            memory[memLocation] -= *preg;
            CF=0;
        }
        else{
            memory[memLocation]= 0xFF-(*preg-memory[memLocation]);
            CF=1;
        }
        if(memory[memLocation]==0){
            ZF=1;
        }
        else{ZF=0;}
        if(memory[memLocation]>=0x80){
            SF=1;
        }
        else{
            SF=0;
        }
    }
}
template <typename regtype>
void sub_mem_word_register(unsigned short memLocation, regtype *preg) {
    unsigned short memWord = byte_to_word(memory[memLocation+1], memory[memLocation]);
    if(memWord>=0x8000&&*preg<0x8000){
        memWord -= *preg;
        if(memWord<0x8000){
            SF=0;
            OF=1;
            if(memWord==0){
                ZF=1;
            }
            else{
                ZF=0;
            }
        }
        else{
            SF=1;
            OF=0;
            ZF=0;
        }
        CF=0;
    }
    else if(memWord<0x8000&&*preg>=0x8000){
        memWord= 0xffff-(*preg-memWord);
        if(memWord>=0x8000){
            SF=1;
            OF=1;
            ZF=0;
        }
        else{
            SF=0;
            OF=0;
            if(memWord==0){
                ZF=1;
            }
            else{
                ZF=0;
            }
        }
        CF=1;
    }
    else{
        OF=0;
        if(memWord>=*preg){
            memWord -= *preg;
            CF=0;
        }
        else{
            memWord= 0xffff-(*preg-memWord);
            CF=1;
        }
        if(memWord==0){
            ZF=1;
        }
        else{ZF=0;}
        if(memWord>=0x8000){
            SF=1;
        }
        else{
            SF=0;
        }
    }
    unsigned char memWordLow = memWord & 0xff;
    unsigned char memWordHigh = (memWord >> 8);
    memory[memLocation+1] = memWordHigh;
    memory[memLocation] = memWordLow;
}
template <typename regtype>
void sub_register_byte_mem(regtype *preg, unsigned short memLocation) {
    if(*preg>=0x80&&memory[memLocation]<0x80){
        *preg -= memory[memLocation];
        if(*preg<0x80){
            SF=0;
            OF=1;
            if(*preg==0){
                ZF=1;
            }
            else{
                ZF=0;
            }
        }
        else{
            SF=1;
            OF=0;
            ZF=0;
        }
        CF=0;
    }
    else if(*preg<0x80&&memory[memLocation]>=0x80){
        *preg= 0xFF-(memory[memLocation]-*preg);
        if(*preg>=0x80){
            SF=1;
            OF=1;
            ZF=0;
        }
        else{
            SF=0;
            OF=0;
            if(*preg==0){
                ZF=1;
            }
            else{
                ZF=0;
            }
        }
        CF=1;
    }
    else{
        OF=0;
        if(*preg>=memory[memLocation]){
            *preg -= memory[memLocation];
            CF=0;
        }
        else{
            *preg= 0xFF-(memory[memLocation]-*preg);
            CF=1;
        }
        if(*preg==0){
            ZF=1;
        }
        else{ZF=0;}
        if(*preg>=0x80){
            SF=1;
        }
        else{
            SF=0;
        }
    }
}
template <typename regtype>
void sub_register_word_mem(regtype *preg, unsigned short memLocation) {
    unsigned short word = byte_to_word(memory[memLocation+1], memory[memLocation]);
    if(*preg>=0x8000&&word<0x8000){
        *preg -= word;
        if(*preg<0x8000){
            SF=0;
            OF=1;
            if(*preg==0){
                ZF=1;
            }
            else{
                ZF=0;
            }
        }
        else{
            SF=1;
            OF=0;
            ZF=0;
        }
        CF=0;
    }
    else if(*preg<0x8000&&word>=0x8000){
        *preg= 0xffff-(word-*preg);
        if(*preg>=0x8000){
            SF=1;
            OF=1;
            ZF=0;
        }
        else{
            SF=0;
            OF=0;
            if(*preg==0){
                ZF=1;
            }
            else{
                ZF=0;
            }
        }
        CF=1;
    }
    else{
        OF=0;
        if(*preg>=word){
            *preg -= word;
            CF=0;
        }
        else{
            *preg= 0xffff-(word-*preg);
            CF=1;
        }
        if(*preg==0){
            ZF=1;
        }
        else{ZF=0;}
        if(*preg>=0x8000){
            SF=1;
        }
        else{
            SF=0;
        }
    }
}
template <typename regtype>
void sub_bregister_bregister(regtype *preg1, regtype *preg2) {
    if(*preg1>=0x80&&*preg2<0x80){
        *preg1 -= *preg2;
        if(*preg1<0x80){
            SF=0;
            OF=1;
            if(*preg1==0){
                ZF=1;
            }
            else{
                ZF=0;
            }
        }
        else{
            SF=1;
            OF=0;
            ZF=0;
        }
        CF=0;
    }
    else if(*preg1<0x80&&*preg2>=0x80){
        *preg1= 0xFF-(*preg2-*preg1);
        if(*preg1>=0x80){
            SF=1;
            OF=1;
            ZF=0;
        }
        else{
            SF=0;
            OF=0;
            if(*preg1==0){
                ZF=1;
            }
            else{
                ZF=0;
            }
        }
        CF=1;
    }
    else{
        OF=0;
        if(*preg1>=*preg2){
            *preg1 -= *preg2;
            CF=0;
        }
        else{
            *preg1= 0xFF-(*preg2-*preg1);
            CF=1;
        }
        if(*preg1==0){
            ZF=1;
        }
        else{ZF=0;}
        if(*preg1>=0x80){
            SF=1;
        }
        else{
            SF=0;
        }
    }
}
template <typename regtype>
void sub_wregister_wregister(regtype *preg1, regtype *preg2) {
    if(*preg1>=0x8000&&*preg2<0x8000){
        *preg1 -= *preg2;
        if(*preg1<0x8000){
            SF=0;
            OF=1;
            if(*preg1==0){
                ZF=1;
            }
            else{
                ZF=0;
            }
        }
        else{
            SF=1;
            OF=0;
            ZF=0;
        }
        CF=0;
    }
    else if(*preg1<0x8000&&*preg2>=0x8000){
        *preg1= 0xffff-(*preg2-*preg1);
        if(*preg1>=0x8000){
            SF=1;
            OF=1;
            ZF=0;
        }
        else{
            SF=0;
            OF=0;
            if(*preg1==0){
                ZF=1;
            }
            else{
                ZF=0;
            }
        }
        CF=1;
    }
    else{
        OF=0;
        if(*preg1>=*preg2){
            *preg1 -= *preg2;
            CF=0;
        }
        else{
            *preg1= 0xffff-(*preg2-*preg1);
            CF=1;
        }
        if(*preg1==0){
            ZF=1;
        }
        else{ZF=0;}
        if(*preg1>=0x8000){
            SF=1;
        }
        else{
            SF=0;
        }
    }
}


//INC methods by operand types
template <typename regtype>
void inc_breg(regtype *preg){
    unsigned short temp=*preg+1;
    *preg=temp%0x100;
    if(*preg==0x80){
        OF=1;
    }
    else{
        OF=0;
    }
    if(*preg>=0x80){
        SF=1;
    }
    else{
        SF=0;
    }
    if(*preg==0){
        ZF=1;
    }
    else{
        ZF=0;
    }
}
template <typename regtype>
void inc_wreg(regtype *preg){
    unsigned int temp=*preg + 1;
    *preg=temp%0x10000;
    if(*preg==0x8000){
        OF=1;
    }
    else{
        OF=0;
    }
    if(*preg>=0x8000){
        SF=1;
    }
    else{
        SF=0;
    }
    if(*preg==0){
        ZF=1;
    }
    else{
        ZF=0;
    }
}
void inc_mem_byte(unsigned short memLocation){
    unsigned short temp=memory[memLocation]+1;
    memory[memLocation]=temp%0x100;
    if(memory[memLocation]==0x80){
        OF=1;
    }
    else{
        OF=0;
    }
    if(memory[memLocation]>=0x80){
        SF=1;
    }
    else{
        SF=0;
    }
    if(memory[memLocation]==0){
        ZF=1;
    }
    else{
        ZF=0;
    }
}
void inc_mem_word(unsigned short memLocation){
    unsigned word=byte_to_word(memory[memLocation+1],memory[memLocation]);
    unsigned int temp=word + 1;
    word=temp%0x10000;
    if(word==0x8000){
        OF=1;
    }
    else{
        OF=0;
    }
    if(word>=0x8000){
        SF=1;
    }
    else{
        SF=0;
    }
    if(word==0){
        ZF=1;
    }
    else{
        ZF=0;
    }
    unsigned char memWordLow = word & 0xff;
    unsigned char memWordHigh = (word >> 8);
    memory[memLocation+1] = memWordHigh;
    memory[memLocation] = memWordLow;

}

//DEC methods by operand types
template <typename regtype>
void dec_breg(regtype *preg){
    if(*preg!=0)
        *preg--;
    else{
        *preg=0xfe;
    }
    if(*preg==(0x80-1)){
        OF=1;
    }
    else{
        OF=0;
    }
    if(*preg>=0x80){
        SF=1;
    }
    else{
        SF=0;
    }
    if(*preg==0){
        ZF=1;
    }
    else{
        ZF=0;
    }
}
template <typename regtype>
void dec_wreg(regtype *preg){
    if(*preg!=0)
        *preg--;
    else{
        *preg=0xfffe;
    }
    if(*preg==(0x8000-1)){
        OF=1;
    }
    else{
        OF=0;
    }
    if(*preg>=0x8000){
        SF=1;
    }
    else{
        SF=0;
    }
    if(*preg==0){
        ZF=1;
    }
    else{
        ZF=0;
    }
}
void dec_mem_byte(unsigned short memLocation){
    if(memory[memLocation]!=0){
        memory[memLocation]--;
    }
    else{
        memory[memLocation]=0xfe;
    }
    if(memory[memLocation]==(0x80-1)){
        OF=1;
    }
    else{
        OF=0;
    }
    if(memory[memLocation]>=0x80){
        SF=1;
    }
    else{
        SF=0;
    }
    if(memory[memLocation]==0){
        ZF=1;
    }
    else{
        ZF=0;
    }
}
void dec_mem_word(unsigned short memLocation){
    unsigned short word=byte_to_word(memory[memLocation+1],memory[memLocation]);
    if(word!=0)
        word--;
    else{
        word=0xfffe;
    }
    if(word==(0x8000-1)){
        OF=1;
    }
    else{
        OF=0;
    }
    if(word>=0x8000){
        SF=1;
    }
    else{
        SF=0;
    }
    if(word==0){
        ZF=1;
    }
    else{
        ZF=0;
    }
    unsigned char memWordLow = word & 0xff;
    unsigned char memWordHigh = (word >> 8);
    memory[memLocation+1] = memWordHigh;
    memory[memLocation] = memWordLow;

}
//MUL methods by operand types
template <typename regtype>
void mul_breg(regtype *preg){
    unsigned short temp = *pal* *preg;
    *pax=temp;
    if(*pal!=0){
        OF=1;
        CF=1;
    }
    else{
        CF=0;
        OF=0;
    }
}
template <typename regtype>
void mul_wreg(regtype *preg){
    unsigned int memWord=*pax * *preg;
    unsigned short memWordLow = memWord & 0xffff;
    unsigned short memWordHigh= (memWord>>16);
    *pdx=memWordHigh;
    *pax=memWordLow;
    if(*pdx!=0){
        OF=1;
        CF=1;
    }
    else{
        CF=0;
        OF=0;
    }
}
void mul_mem_byte(unsigned short memLocation){
    unsigned short temp = *pal* memory[memLocation];
    *pax=temp;
    if(*pal!=0){
        OF=1;
        CF=1;
    }
    else{
        CF=0;
        OF=0;
    }
}
void mul_mem_word(unsigned short memLocation){
    unsigned int memWord = byte_to_word(memory[memLocation+1], memory[memLocation]);
    unsigned int othermem=*pax * memWord;
    unsigned short memWordLow = othermem & 0xffff;
    unsigned short memWordHigh= (othermem>>16);
    *pdx=memWordHigh;
    *pax=memWordLow;
    if(*pdx!=0){
        OF=1;
        CF=1;
    }
    else{
        CF=0;
        OF=0;
    }
}
//DIV methods by operand types
template <typename regtype>
int div_breg(regtype *preg){
    if(*preg==0){
        return 0;
    }
    if(*pax>0xff){
        return 0;
    }
    unsigned short high = *pax%*preg;
    unsigned short low = *pax/ *preg;
    *pah=high;
    *pal=low;
    return 1;
}
template <typename regtype>
int div_wreg(regtype *preg){
    if(*preg==0){
        return 0;
    }
    unsigned int word = ( ( *pdx & 0xFFFF ) << 16 ) | ( *pax & 0xFFFF );
    if(word>0xffff){
        return 0;
    }
    unsigned short high = word%*preg;
    unsigned short low = word/ *preg;
    *pdx=high;
    *pax=low;
    return 1;
}
int div_mem_byte(unsigned short memLocation){
    if(memory[memLocation]==0){
        return 0;
    }
    if(*pax>0xff){
        return 0;
    }
    unsigned short high = *pax%memory[memLocation];
    unsigned short low = *pax/ memory[memLocation];
    *pah=high;
    *pal=low;
    return 1;
}
int div_mem_word(unsigned short memLocation){
    unsigned int memWord = byte_to_word(memory[memLocation+1], memory[memLocation]);
    if(memWord==0){
        return 0;
    }
    unsigned int word = ( ( *pdx & 0xFFFF ) << 16 ) | ( *pax & 0xFFFF );
    if(word>0xFFFF){
        return 0;
    }
    unsigned short high = word%memWord;
    unsigned short low = word/ memWord;
    *pdx=high;
    *pax=low;
    return 1;
}
//PUSH methods by operand types
template <typename regtype>
void push_reg(regtype *preg){
    unsigned short first=*preg%0xff;
    unsigned short second=*preg/0xff;
    memory[sp]=first;
    memory[sp+1]=second;
    sp-=2;
}
void push_mem(unsigned short memWord){
    memory[sp]=memory[memWord];
    memory[sp+1]=memory[memWord+1];
    sp-=2;
}
void push_var(unsigned short var){
    unsigned short first=var%0xff;
    unsigned short second=var/0xff;
    memory[sp]=first;
    memory[sp+1]=second;
    sp-=2;
}
//POP methods by operand types
template <typename regtype>
void pop_reg(regtype *preg){
    unsigned short first=memory[sp+2];
    unsigned short second=memory[sp+3];
    unsigned short mem=byte_to_word(second,first);
    *preg=mem;
    memory[sp+2]=0;
    memory[sp+3]=0;
    sp+=2;
}
void pop_mem(unsigned short memWord){
    memory[memWord]=memory[sp+2];
    memory[memWord+1]=memory[sp+3];
    memory[sp+2]=0;
    memory[sp+3]=0;
    sp+=2;
}
//NOT
template <typename regtype>
void not_reg(regtype *preg) {
    *preg = ~*preg;
}
void not_mem_byte(unsigned short memLocation) {
    memory[memLocation] = ~memory[memLocation];
}
void not_mem_word(unsigned short memLocation) {
    memory[memLocation] = ~memory[memLocation];
    memory[memLocation+1] = ~memory[memLocation+1];
}
//CMP methods by operand types
template <typename regtype>
void cmp_reg_byte(regtype *preg, unsigned char byte) {
    if(*preg>=0x80&&byte<0x80){
        unsigned short temp=*preg - byte;
        if(temp<0x80){
            SF=0;
            OF=1;
            if(temp==0){
                ZF=1;
            }
            else{
                ZF=0;
            }
        }
        else{
            SF=1;
            OF=0;
            ZF=0;
        }
        CF=0;
    }
    else if(*preg<0x80&&byte>=0x80){
        unsigned short temp= 0xFF-(byte-*preg);
        if(temp>=0x80){
            SF=1;
            OF=1;
            ZF=0;
        }
        else{
            SF=0;
            OF=0;
            if(temp==0){
                ZF=1;
            }
            else{
                ZF=0;
            }
        }
        CF=1;
    }
    else{
        OF=0;
        unsigned short temp;
        if(*preg>=byte){
            temp=*preg - byte;
            CF=0;
        }
        else{
            temp= 0xFF-(byte-*preg);
            CF=1;
        }
        if(temp==0){
            ZF=1;
        }
        else{ZF=0;}
        if(temp>=0x80){
            SF=1;
        }
        else{
            SF=0;
        }
    }
}
template <typename regtype>
void cmp_reg_word(regtype *preg, unsigned short word) {
    unsigned short temp;
    if(*preg>=0x8000&&word<0x8000){
        temp=*preg - word;
        if(temp<0x8000){
            SF=0;
            OF=1;
            if(temp==0){
                ZF=1;
            }
            else{
                ZF=0;
            }
        }
        else{
            SF=1;
            OF=0;
            ZF=0;
        }
        CF=0;
    }
    else if(*preg<0x8000&&word>=0x8000){
        temp= 0xffff-(word-*preg);
        if(temp>=0x8000){
            SF=1;
            OF=1;
            ZF=0;
        }
        else{
            SF=0;
            OF=0;
            if(temp==0){
                ZF=1;
            }
            else{
                ZF=0;
            }
        }
        CF=1;
    }
    else{
        OF=0;
        if(*preg>=word){
            temp=*preg - word;
            CF=0;
        }
        else{
            temp= 0xffff-(word-*preg);
            CF=1;
        }
        if(temp==0){
            ZF=1;
        }
        else{ZF=0;}
        if(temp>=0x8000){
            SF=1;
        }
        else{
            SF=0;
        }
    }
}
void cmp_mem_byte(unsigned short memLocation, unsigned char byte) {
    unsigned short temp;
    if(memory[memLocation]>=0x80&&byte<0x80){
        temp= memory[memLocation] - byte;
        if(temp<0x80){
            SF=0;
            OF=1;
            if(temp==0){
                ZF=1;
            }
            else{
                ZF=0;
            }
        }
        else{
            SF=1;
            OF=0;
            ZF=0;
        }
        CF=0;
    }
    else if(memory[memLocation]<0x80&&byte>=0x80){
        temp= 0xFF-(byte-memory[memLocation]);
        if(temp>=0x80){
            SF=1;
            OF=1;
            ZF=0;
        }
        else{
            SF=0;
            OF=0;
            if(temp==0){
                ZF=1;
            }
            else{
                ZF=0;
            }
        }
        CF=1;
    }
    else{
        OF=0;
        if(memory[memLocation]>=byte){
            temp=memory[memLocation] - byte;
            CF=0;
        }
        else{
            temp= 0xFF-(byte-memory[memLocation]);
            CF=1;
        }
        if(temp==0){
            ZF=1;
        }
        else{ZF=0;}
        if(temp>=0x80){
            SF=1;
        }
        else{
            SF=0;
        }
    }
}
void cmp_mem_word(unsigned short memLocation, unsigned short word) {
    unsigned short temp;
    unsigned short memWord = byte_to_word(memory[memLocation+1], memory[memLocation]);
    if(memWord>=0x8000&&word<0x8000){
        temp=memWord - word;
        if(temp<0x8000){
            SF=0;
            OF=1;
            if(temp==0){
                ZF=1;
            }
            else{
                ZF=0;
            }
        }
        else{
            SF=1;
            OF=0;
            ZF=0;
        }
        CF=0;
    }
    else if(memWord<0x8000&&word>=0x8000){
        temp= 0xffff-(word-memWord);
        if(temp>=0x8000){
            SF=1;
            OF=1;
            ZF=0;
        }
        else{
            SF=0;
            OF=0;
            if(temp==0){
                ZF=1;
            }
            else{
                ZF=0;
            }
        }
        CF=1;
    }
    else{
        OF=0;
        if(memWord>=word){
            temp= memWord - word;
            CF=0;
        }
        else{
            temp= 0xffff-(word-memWord);
            CF=1;
        }
        if(temp==0){
            ZF=1;
        }
        else{ZF=0;}
        if(temp>=0x8000){
            SF=1;
        }
        else{
            SF=0;
        }
    }
}
template <typename regtype>
void cmp_mem_byte_register(unsigned short memLocation, regtype *preg) {
    unsigned short temp;
    if(memory[memLocation]>=0x80&&*preg<0x80){
        temp=memory[memLocation] - *preg;
        if(temp<0x80){
            SF=0;
            OF=1;
            if(temp==0){
                ZF=1;
            }
            else{
                ZF=0;
            }
        }
        else{
            SF=1;
            OF=0;
            ZF=0;
        }
        CF=0;
    }
    else if(memory[memLocation]<0x80&&*preg>=0x80){
        temp= 0xFF-(*preg-memory[memLocation]);
        if(temp>=0x80){
            SF=1;
            OF=1;
            ZF=0;
        }
        else{
            SF=0;
            OF=0;
            if(temp==0){
                ZF=1;
            }
            else{
                ZF=0;
            }
        }
        CF=1;
    }
    else{
        OF=0;
        if(memory[memLocation]>=*preg){
            temp=memory[memLocation] - *preg;
            CF=0;
        }
        else{
            temp= 0xFF-(*preg-memory[memLocation]);
            CF=1;
        }
        if(temp==0){
            ZF=1;
        }
        else{ZF=0;}
        if(temp>=0x80){
            SF=1;
        }
        else{
            SF=0;
        }
    }
}
template <typename regtype>
void cmp_mem_word_register(unsigned short memLocation, regtype *preg) {
    unsigned short temp;
    unsigned short memWord = byte_to_word(memory[memLocation+1], memory[memLocation]);
    if(memWord>=0x8000&&*preg<0x8000){
        temp=memWord - *preg;
        if(temp<0x8000){
            SF=0;
            OF=1;
            if(temp==0){
                ZF=1;
            }
            else{
                ZF=0;
            }
        }
        else{
            SF=1;
            OF=0;
            ZF=0;
        }
        CF=0;
    }
    else if(memWord<0x8000&&*preg>=0x8000){
        temp= 0xffff-(*preg-memWord);
        if(temp>=0x8000){
            SF=1;
            OF=1;
            ZF=0;
        }
        else{
            SF=0;
            OF=0;
            if(temp==0){
                ZF=1;
            }
            else{
                ZF=0;
            }
        }
        CF=1;
    }
    else{
        OF=0;
        if(memWord>=*preg){
            temp=memWord - *preg;
            CF=0;
        }
        else{
            temp= 0xffff-(*preg-memWord);
            CF=1;
        }
        if(temp==0){
            ZF=1;
        }
        else{ZF=0;}
        if(temp>=0x8000){
            SF=1;
        }
        else{
            SF=0;
        }
    }
}
template <typename regtype>
void cmp_register_byte_mem(regtype *preg, unsigned short memLocation) {
    unsigned short temp;
    if(*preg>=0x80&&memory[memLocation]<0x80){
        temp=*preg - memory[memLocation];
        if(temp<0x80){
            SF=0;
            OF=1;
            if(temp==0){
                ZF=1;
            }
            else{
                ZF=0;
            }
        }
        else{
            SF=1;
            OF=0;
            ZF=0;
        }
        CF=0;
    }
    else if(*preg<0x80&&memory[memLocation]>=0x80){
        temp= 0xFF-(memory[memLocation]-*preg);
        if(temp>=0x80){
            SF=1;
            OF=1;
            ZF=0;
        }
        else{
            SF=0;
            OF=0;
            if(temp==0){
                ZF=1;
            }
            else{
                ZF=0;
            }
        }
        CF=1;
    }
    else{
        OF=0;
        if(*preg>=memory[memLocation]){
            temp=*preg - memory[memLocation];
            CF=0;
        }
        else{
            temp= 0xFF-(memory[memLocation]-*preg);
            CF=1;
        }
        if(temp==0){
            ZF=1;
        }
        else{ZF=0;}
        if(temp>=0x80){
            SF=1;
        }
        else{
            SF=0;
        }
    }
}
template <typename regtype>
void cmp_register_word_mem(regtype *preg, unsigned short memLocation) {
    unsigned short temp;
    unsigned short word = byte_to_word(memory[memLocation+1], memory[memLocation]);
    if(*preg>=0x8000&&word<0x8000){
        temp=*preg - word;
        if(temp<0x8000){
            SF=0;
            OF=1;
            if(temp==0){
                ZF=1;
            }
            else{
                ZF=0;
            }
        }
        else{
            SF=1;
            OF=0;
            ZF=0;
        }
        CF=0;
    }
    else if(*preg<0x8000&&word>=0x8000){
        temp= 0xffff-(word-*preg);
        if(temp>=0x8000){
            SF=1;
            OF=1;
            ZF=0;
        }
        else{
            SF=0;
            OF=0;
            if(temp==0){
                ZF=1;
            }
            else{
                ZF=0;
            }
        }
        CF=1;
    }
    else{
        OF=0;
        if(*preg>=word){
            temp=*preg - word;
            CF=0;
        }
        else{
            temp= 0xffff-(word-*preg);
            CF=1;
        }
        if(temp==0){
            ZF=1;
        }
        else{ZF=0;}
        if(temp>=0x8000){
            SF=1;
        }
        else{
            SF=0;
        }
    }
}
template <typename regtype>
void cmp_bregister_bregister(regtype *preg1, regtype *preg2) {
    unsigned short temp;
    if(*preg1>=0x80&&*preg2<0x80){
        temp=*preg1 - *preg2;
        if(temp<0x80){
            SF=0;
            OF=1;
            if(temp==0){
                ZF=1;
            }
            else{
                ZF=0;
            }
        }
        else{
            SF=1;
            OF=0;
            ZF=0;
        }
        CF=0;
    }
    else if(*preg1<0x80&&*preg2>=0x80){
        temp= 0xFF-(*preg2-*preg1);
        if(*preg1>=0x80){
            SF=1;
            OF=1;
            ZF=0;
        }
        else{
            SF=0;
            OF=0;
            if(temp==0){
                ZF=1;
            }
            else{
                ZF=0;
            }
        }
        CF=1;
    }
    else{
        OF=0;
        if(*preg1>=*preg2){
            temp=*preg1 - *preg2;
            CF=0;
        }
        else{
            temp= 0xFF-(*preg2-*preg1);
            CF=1;
        }
        if(temp==0){
            ZF=1;
        }
        else{ZF=0;}
        if(temp>=0x80){
            SF=1;
        }
        else{
            SF=0;
        }
    }
}
template <typename regtype>
void cmp_wregister_wregister(regtype *preg1, regtype *preg2) {
    unsigned short temp;
    if(*preg1>=0x8000&&*preg2<0x8000){
        temp=*preg1 - *preg2;
        if(temp<0x8000){
            SF=0;
            OF=1;
            if(temp==0){
                ZF=1;
            }
            else{
                ZF=0;
            }
        }
        else{
            SF=1;
            OF=0;
            ZF=0;
        }
        CF=0;
    }
    else if(*preg1<0x8000&&*preg2>=0x8000){
        temp= 0xffff-(*preg2-*preg1);
        if(temp>=0x8000){
            SF=1;
            OF=1;
            ZF=0;
        }
        else{
            SF=0;
            OF=0;
            if(temp==0){
                ZF=1;
            }
            else{
                ZF=0;
            }
        }
        CF=1;
    }
    else{
        OF=0;
        if(*preg1>=*preg2){
            temp= *preg1 -=*preg2;
            CF=0;
        }
        else{
            temp= 0xffff-(*preg2-*preg1);
            CF=1;
        }
        if(temp==0){
            ZF=1;
        }
        else{ZF=0;}
        if(temp>=0x8000){
            SF=1;
        }
        else{
            SF=0;
        }
    }
}

bool jz(){
    return ZF;
}
bool jnz(){
    return !ZF;
}
bool je(){
    return ZF;
}
bool jne(){
    return !ZF;
}
bool ja(){
    return !ZF&&!CF;
}
bool jae(){
    return !CF;
}
bool jb(){
    return CF;
}
bool jbe(){
    return ZF||CF;
}
bool jnae(){
    return CF;
}
bool jnb(){
    return !CF;
}
bool jnbe(){
    return !CF&&!ZF;
}
bool jc(){
    return CF;
}
bool jnc(){
    return !CF;
}
template<typename regtype>
void shr_byte_reg_var(regtype *preg, unsigned short var){
    if(var!=1){
        for(int i=0;i<var;i++){
            CF=*preg%2;
            *preg=*preg>>1;
        }
    }
    else{
        if(*preg>=0x80){
            OF=1;
        }
        else{
            OF=0;
        }
        CF=*preg%2;
        *preg=*preg>>1;
    }
    if(*preg==0){
        ZF=1;
    }
    else{
        ZF=0;
    }
    SF=0;

}

template<typename regtype>
void shr_word_reg_var(regtype *preg, unsigned short var){
    if(var!=1){
        for(int i=0;i<var;i++){
            CF=*preg%2;
            *preg=*preg>>1;
        }
    }
    else{
        if(*preg>=0x8000){
            OF=1;
        }
        else{
            OF=0;
        }
        CF=*preg%2;
        *preg=*preg>>1;
    }
    if(*preg==0){
        ZF=1;
    }
    else{
        ZF=0;
    }
    SF=0;

}
template<typename regtype>
void shr_word_reg_cl(regtype *preg){
    if(*pcl!=1){
        for(int i=0;i<*pcl;i++){
            CF=*preg%2;
            *preg=*preg>>1;
        }
    }
    else{
        if(*preg>=0x8000){
            OF=1;
        }
        else{
            OF=0;
        }
        CF=*preg%2;
        *preg=*preg>>1;
    }
    if(*preg==0){
        ZF=1;
    }
    else{
        ZF=0;
    }
    SF=0;

}

template<typename regtype>
void shr_byte_reg_cl(regtype *preg){
    if(*pcl!=1){
        for(int i=0;i<*pcl;i++){
            CF=*preg%2;
            *preg=*preg>>1;
        }
    }
    else{
        if(*preg>=0x80){
            OF=1;
        }
        else{
            OF=0;
        }
        CF=*preg%2;
        *preg=*preg>>1;
    }
    if(*preg==0){
        ZF=1;
    }
    else{
        ZF=0;
    }
    SF=0;
}
void shr_byte_mem_var(unsigned short memLocation, unsigned short var){
    if(var!=1){
        for(int i=0;i<var;i++){
            CF=memory[memLocation]%2;
            memory[memLocation]=memory[memLocation]>>1;
        }
    }
    else{
        if(memory[memLocation]>=0x80){
            OF=1;
        }
        else{
            OF=0;
        }
        CF=memory[memLocation]%2;
        memory[memLocation]=memory[memLocation]>>1;
    }
    if(memory[memLocation]==0){
        ZF=1;
    }
    else{
        ZF=0;
    }
    SF=0;

}

void shr_word_mem_var(unsigned short memLocation, unsigned short var){
    unsigned short memWord = byte_to_word(memory[memLocation+1],memory[memLocation]);
    if(var!=1){
        for(int i=0;i<var;i++){
            CF=memWord%2;
            memWord=memWord>>1;
        }
    }
    else{
        if(memWord>=0x8000){
            OF=1;
        }
        else{
            OF=0;
        }
        CF=memWord%2;
        memWord=memWord>>1;
    }
    if(memWord==0){
        ZF=1;
    }
    else{
        ZF=0;
    }
    SF=0;
    unsigned char memWordLow = memWord & 0xff;
    unsigned char memWordHigh = (memWord >> 8);
    memory[memLocation+1] = memWordHigh;
    memory[memLocation] = memWordLow;
}
void shr_word_mem_cl(unsigned short memLocation){
    unsigned short memWord = byte_to_word(memory[memLocation+1],memory[memLocation]);
    if(*pcl!=1){
        for(int i=0;i<*pcl;i++){
            CF=memWord%2;
            memWord=memWord>>1;
        }
    }
    else{
        if(memWord>=0x8000){
            OF=1;
        }
        else{
            OF=0;
        }
        CF=memWord%2;
        memWord=memWord>>1;
    }
    if(memWord==0){
        ZF=1;
    }
    else{
        ZF=0;
    }
    SF=0;
    unsigned char memWordLow = memWord & 0xff;
    unsigned char memWordHigh = (memWord >> 8);
    memory[memLocation+1] = memWordHigh;
    memory[memLocation] = memWordLow;
}

void shr_byte_mem_cl(unsigned short memLocation){
    if(*pcl!=1){
        for(int i=0;i<*pcl;i++){
            CF=memory[memLocation]%2;
            memory[memLocation]=memory[memLocation]>>1;
        }
    }
    else{
        if(memory[memLocation]>=0x80){
            OF=1;
        }
        else{
            OF=0;
        }
        CF=memory[memLocation]%2;
        memory[memLocation]=memory[memLocation]>>1;
    }
    if(memory[memLocation]==0){
        ZF=1;
    }
    else{
        ZF=0;
    }
    SF=0;
}

template<typename regtype>
void shl_byte_reg_var(regtype *preg, unsigned short var){
    unsigned short dust;
    if(var!=1){
        for(int i=0;i<var;i++){
            CF=*preg/0x80;
            dust=*preg;
            dust=dust<<1;
            *preg=dust % 0x100;
        }
    }
    else{
        unsigned char temp=*preg;
        CF=*preg/0x80;
        dust=*preg;
        dust=dust<<1;
        *preg=dust % 0x100;
        if((*preg/0x80-temp/0x80)!=0){
            OF=1;
        }
        else{OF=0;}
    }
    if(*preg==0){
        ZF=1;
    }
    else{
        ZF=0;
    }
    if(*preg/0x80==1){
        SF=1;
    }
    else{
        SF=0;
    }
}

template<typename regtype>
void shl_word_reg_var(regtype *preg, unsigned short var){
    if(var!=1){
        for(int i=0;i<var;i++){
            CF=*preg/0x8000;
            *preg=*preg<<1;
        }
    }
    else{
        unsigned short temp=*preg;
        CF=*preg/0x8000;
        *preg=*preg<<1;
        if((*preg/0x8000-temp/0x8000)!=0){
            OF=1;
        }
        else{OF=0;}
    }
    if(*preg==0){
        ZF=1;
    }
    else{
        ZF=0;
    }
    if(*preg/0x8000==1){
        SF=1;
    }
    else{
        SF=0;
    }

}
template<typename regtype>
void shl_word_reg_cl(regtype *preg){
    if(*pcl!=1){
        for(int i=0;i<*pcl;i++){
            CF=*preg/0x8000;
            *preg=*preg<<1;
        }
    }
    else{
        unsigned short temp=*preg;
        CF=*preg/0x8000;
        *preg=*preg<<1;
        if((*preg/0x8000-temp/0x8000)!=0){
            OF=1;
        }
        else{OF=0;}
    }
    if(*preg==0){
        ZF=1;
    }
    else{
        ZF=0;
    }
    if(*preg/0x8000==1){
        SF=1;
    }
    else{
        SF=0;
    }

}

template<typename regtype>
void shl_byte_reg_cl(regtype *preg){
    if(*pcl!=1){
        for(int i=0;i<*pcl;i++){
            CF=*preg/0x80;
            *preg=*preg<<1;
        }
    }
    else{
        unsigned char temp=*preg;
        CF=*preg/0x80;
        *preg=*preg<<1;
        if((*preg/0x80-temp/0x80)!=0){
            OF=1;
        }
        else{OF=0;}
    }
    if(*preg==0){
        ZF=1;
    }
    else{
        ZF=0;
    }
    if(*preg/0x80==1){
        SF=1;
    }
    else{
        SF=0;
    }
}
void shl_byte_mem_var(unsigned short memLocation, unsigned short var){
    if(var!=1){
        for(int i=0;i<var;i++){
            CF=memory[memLocation]/0x80;
            memory[memLocation]=memory[memLocation]<<1;
        }
    }
    else{
        unsigned char temp=memory[memLocation];
        CF=memory[memLocation]/0x80;
        memory[memLocation]=memory[memLocation]<<1;
        if((memory[memLocation]/0x80-temp/0x80)!=0){
            OF=1;
        }
        else{OF=0;}
    }
    if(memory[memLocation]==0){
        ZF=1;
    }
    else{
        ZF=0;
    }
    if(memory[memLocation]/0x80==1){
        SF=1;
    }
    else{
        SF=0;
    }

}

void shl_word_mem_var(unsigned short memLocation, unsigned short var){
    unsigned short memWord = byte_to_word(memory[memLocation+1],memory[memLocation]);
    if(var!=1){
        for(int i=0;i<var;i++){
            CF=memWord/0x8000;
            memWord=memWord<<1;
        }
    }
    else{
        unsigned short temp=memWord;
        CF=memWord/0x8000;
        memWord=memWord<<1;
        if((memWord/0x8000-temp/0x8000)!=0){
            OF=1;
        }
        else{OF=0;}
    }
    if(memWord==0){
        ZF=1;
    }
    else{
        ZF=0;
    }
    if(memWord/0x8000==1){
        SF=1;
    }
    else{
        SF=0;
    }
    unsigned char memWordLow = memWord & 0xff;
    unsigned char memWordHigh = (memWord >> 8);
    memory[memLocation+1] = memWordHigh;
    memory[memLocation] = memWordLow;
}
void shl_word_mem_cl(unsigned short memLocation){
    unsigned short memWord = byte_to_word(memory[memLocation+1],memory[memLocation]);
    if(*pcl!=1){
        for(int i=0;i<*pcl;i++){
            CF=memWord/0x8000;
            memWord=memWord<<1;
        }
    }
    else{
        unsigned short temp=memWord;
        CF=memWord/0x8000;
        memWord=memWord<<1;
        if((memWord/0x8000-temp/0x8000)!=0){
            OF=1;
        }
        else{OF=0;}
    }
    if(memWord==0){
        ZF=1;
    }
    else{
        ZF=0;
    }
    if(memWord/0x8000==1){
        SF=1;
    }
    else{
        SF=0;
    }     unsigned char memWordLow = memWord & 0xff;
    unsigned char memWordHigh = (memWord >> 8);
    memory[memLocation+1] = memWordHigh;
    memory[memLocation] = memWordLow;
}

void shl_byte_mem_cl(unsigned short memLocation){
    if(*pcl!=1){
        for(int i=0;i<*pcl;i++){
            CF=memory[memLocation]/0x80;
            memory[memLocation]=memory[memLocation]<<1;
        }
    }
    else{
        unsigned char temp=memory[memLocation];
        CF=memory[memLocation]/0x80;
        memory[memLocation]=memory[memLocation]<<1;
        if((memory[memLocation]/0x80-temp/0x80)!=0){
            OF=1;
        }
        else{OF=0;}
    }
    if(memory[memLocation]==0){
        ZF=1;
    }
    else{
        ZF=0;
    }
    if(memory[memLocation]/0x80==1){
        SF=1;
    }
    else{
        SF=0;
    }
}
template<typename regtype>
void rcr_byte_reg_var(regtype *preg, unsigned short var){
    if(var!=1){
        for(int i=0;i<var;i++){
            unsigned int tempCF=CF;
            CF=*preg%2;
            *preg=*preg>>1;
            *preg+=tempCF*0x80;
        }
    }
    else{
        if(*preg>=0x80){
            OF=1;
        }
        else{
            OF=0;
        }
        unsigned int tempCF=CF;
        CF=*preg%2;
        *preg=*preg>>1;
        *preg+=tempCF*0x80;
    }
}

template<typename regtype>
void rcr_word_reg_var(regtype *preg, unsigned short var){
    if(var!=1){
        for(int i=0;i<var;i++){
            unsigned int tempCF=CF;
            CF=*preg%2;
            *preg=*preg>>1;
            *preg+=tempCF*0x8000;
        }
    }
    else{
        if(*preg>=0x8000){
            OF=1;
        }
        else{
            OF=0;
        }
        unsigned int tempCF=CF;
        CF=*preg%2;
        *preg=*preg>>1;
        *preg+=tempCF*0x8000;
    }


}
template<typename regtype>
void rcr_word_reg_cl(regtype *preg){
    if(*pcl!=1){
        for(int i=0;i<*pcl;i++){
            unsigned int tempCF=CF;
            CF=*preg%2;
            *preg=*preg>>1;
            *preg+=tempCF*0x8000;
        }
    }
    else{
        if(*preg>=0x8000){
            OF=1;
        }
        else{
            OF=0;
        }
        unsigned int tempCF=CF;
        CF=*preg%2;
        *preg=*preg>>1;
        *preg+=tempCF*0x80;
    }
}

template<typename regtype>
void rcr_byte_reg_cl(regtype *preg){
    if(*pcl!=1){
        for(int i=0;i<*pcl;i++){
            unsigned int tempCF=CF;
            CF=*preg%2;
            *preg=*preg>>1;
            *preg+=tempCF*0x80;
        }
    }
    else{
        if(*preg>=0x80){
            OF=1;
        }
        else{
            OF=0;
        }
        unsigned int tempCF=CF;
        CF=*preg%2;
        *preg=*preg>>1;
        *preg+=tempCF*0x80;
    }
}
void rcr_byte_mem_var(unsigned short memLocation, unsigned short var){
    if(var!=1){
        for(int i=0;i<var;i++){
            unsigned int tempCF=CF;
            CF=memory[memLocation]%2;
            memory[memLocation]=memory[memLocation]>>1;
            memory[memLocation]+=tempCF*0x80;
        }
    }
    else{
        if(memory[memLocation]>=0x80){
            OF=1;
        }
        else{
            OF=0;
        }
        unsigned int tempCF=CF;
        CF=memory[memLocation]%2;
        memory[memLocation]=memory[memLocation]>>1;
        memory[memLocation]+=tempCF*0x80;
    }
}

void rcr_word_mem_var(unsigned short memLocation, unsigned short var){
    unsigned short memWord = byte_to_word(memory[memLocation+1],memory[memLocation]);
    if(var!=1){
        for(int i=0;i<var;i++){
            unsigned int tempCF=CF;
            CF=memWord%2;
            memWord=memWord>>1;
            memWord+=tempCF*0x8000;
        }
    }
    else{
        if(memWord>=0x8000){
            OF=1;
        }
        else{
            OF=0;
        }
        unsigned int tempCF=CF;
        CF=memWord%2;
        memWord=memWord>>1;
        memWord+=tempCF*0x8000;;
    }
    unsigned char memWordLow = memWord & 0xff;
    unsigned char memWordHigh = (memWord >> 8);
    memory[memLocation+1] = memWordHigh;
    memory[memLocation] = memWordLow;
}
void rcr_word_mem_cl(unsigned short memLocation){
    unsigned short memWord = byte_to_word(memory[memLocation+1],memory[memLocation]);
    if(*pcl!=1){
        for(int i=0;i<*pcl;i++){
            unsigned int tempCF=CF;
            CF=memWord%2;
            memWord=memWord>>1;
            memWord+=tempCF*0x8000;
        }
    }
    else{
        if(memWord>=0x8000){
            OF=1;
        }
        else{
            OF=0;
        }
        unsigned int tempCF=CF;
        CF=memWord%2;
        memWord=memWord>>1;
        memWord+=tempCF*0x8000;
    }
    unsigned char memWordLow = memWord & 0xff;
    unsigned char memWordHigh = (memWord >> 8);
    memory[memLocation+1] = memWordHigh;
    memory[memLocation] = memWordLow;
}

void rcr_byte_mem_cl(unsigned short memLocation){
    if(*pcl!=1){
        for(int i=0;i<*pcl;i++){
            unsigned int tempCF=CF;
            CF=memory[memLocation]%2;
            memory[memLocation]=memory[memLocation]>>1;
            memory[memLocation]+=tempCF*0x80;
        }
    }
    else{
        if(memory[memLocation]>=0x80){
            OF=1;
        }
        else{
            OF=0;
        }
        unsigned int tempCF=CF;
        CF=memory[memLocation]%2;
        memory[memLocation]=memory[memLocation]>>1;
        memory[memLocation]+=tempCF*0x80;
    }
}

template<typename regtype>
void rcl_byte_reg_var(regtype *preg, unsigned short var){
    if(var!=1){
        for(int i=0;i<var;i++){
            unsigned int tempCF=CF;
            CF=*preg/0x80;
            *preg=*preg<<1;
            *preg+=tempCF;
        }
    }
    else{
        unsigned int temp=*preg;
        unsigned int tempCF=CF;
        CF=*preg/0x80;
        *preg=*preg<<1;
        *preg+=tempCF;
        if((*preg/0x80-temp/0x80)!=0){
            OF=1;
        }
        else{OF=0;}
    }
}

template<typename regtype>
void rcl_word_reg_var(regtype *preg, unsigned short var){
    if(var!=1){
        for(int i=0;i<var;i++){
            unsigned int tempCF=CF;
            CF=*preg/0x8000;
            *preg=*preg<<1;
            *preg+=tempCF;
        }
    }
    else{
        unsigned int temp=*preg;
        unsigned int tempCF=CF;
        CF=*preg/0x8000;
        *preg=*preg<<1;
        *preg+=tempCF;
        if((*preg/0x8000-temp/0x8000)!=0){
            OF=1;
        }
        else{OF=0;}
    }
}
template<typename regtype>
void rcl_word_reg_cl(regtype *preg){
    if(*pcl!=1){
        for(int i=0;i<*pcl;i++){
            unsigned int tempCF=CF;
            CF=*preg/0x8000;
            *preg=*preg<<1;
            *preg+=tempCF;
        }
    }
    else{
        unsigned int temp=*preg;
        unsigned int tempCF=CF;
        CF=*preg/0x8000;
        *preg=*preg<<1;
        *preg+=tempCF;
        if((*preg/0x8000-temp/0x8000)!=0){
            OF=1;
        }
        else{OF=0;}
    }
}

template<typename regtype>
void rcl_byte_reg_cl(regtype *preg){
    if(*pcl!=1){
        for(int i=0;i<*pcl;i++){
            unsigned int tempCF=CF;
            CF=*preg/0x80;
            *preg=*preg<<1;
            *preg+=tempCF;
        }
    }
    else{
        unsigned int temp=*preg;
        unsigned int tempCF=CF;
        CF=*preg/0x80;
        *preg=*preg<<1;
        *preg+=tempCF;
        if((*preg/0x80-temp/0x80)!=0){
            OF=1;
        }
        else{OF=0;}
    }
}
void rcl_byte_mem_var(unsigned short memLocation, unsigned short var){
    if(var!=1){
        for(int i=0;i<var;i++){
            unsigned int tempCF=CF;
            CF=memory[memLocation]/0x80;
            memory[memLocation]=memory[memLocation]<<1;
            memory[memLocation]+=tempCF;
        }
    }
    else{
        unsigned int temp=memory[memLocation];
        unsigned int tempCF=CF;
        CF=memory[memLocation]/0x80;
        memory[memLocation]=memory[memLocation]<<1;
        memory[memLocation]+=tempCF;
        if((memory[memLocation]/0x80-temp/0x80)!=0){
            OF=1;
        }
        else{OF=0;}
    }
}

void rcl_word_mem_var(unsigned short memLocation, unsigned short var){
    unsigned short memWord = byte_to_word(memory[memLocation+1],memory[memLocation]);
    if(var!=1){
        for(int i=0;i<var;i++){
            unsigned int tempCF=CF;
            CF=memWord/0x8000;
            memWord=memWord<<1;
            memWord+=tempCF;
        }
    }
    else{
        unsigned int temp=memWord;
        unsigned int tempCF=CF;
        CF=memWord/0x8000;
        memWord=memWord<<1;
        memWord+=tempCF;
        if((memWord/0x8000-temp/0x8000)!=0){
            OF=1;
        }
        else{OF=0;}
    }
    unsigned char memWordLow = memWord & 0xff;
    unsigned char memWordHigh = (memWord >> 8);
    memory[memLocation+1] = memWordHigh;
    memory[memLocation] = memWordLow;
}
void rcl_word_mem_cl(unsigned short memLocation){
    unsigned short memWord = byte_to_word(memory[memLocation+1],memory[memLocation]);
    if(*pcl!=1){
        for(int i=0;i<*pcl;i++){
            unsigned int tempCF=CF;
            CF=memWord/0x8000;
            memWord=memWord<<1;
            memWord+=tempCF;
        }
    }
    else{
        unsigned int temp=memWord;
        unsigned int tempCF=CF;
        CF=memWord/0x8000;
        memWord=memWord<<1;
        memWord+=tempCF;
        if((memWord/0x8000-temp/0x8000)!=0){
            OF=1;
        }
        else{OF=0;}
    }
    if(memWord==0){
        ZF=1;
    }
    else{
        ZF=0;
    }
    if(memWord/0x8000==1){
        SF=1;
    }
    else{
        SF=0;
    }     unsigned char memWordLow = memWord & 0xff;
    unsigned char memWordHigh = (memWord >> 8);
    memory[memLocation+1] = memWordHigh;
    memory[memLocation] = memWordLow;
}

void rcl_byte_mem_cl(unsigned short memLocation){
    if(*pcl!=1){
        for(int i=0;i<*pcl;i++){
            unsigned int tempCF=CF;
            CF=memory[memLocation]/0x80;
            memory[memLocation]=memory[memLocation]<<1;
            memory[memLocation]+=tempCF;
        }
    }
    else{
        unsigned int temp=memory[memLocation];
        unsigned int tempCF=CF;
        CF=memory[memLocation]/0x80;
        memory[memLocation]= memory[memLocation]<<1;
        memory[memLocation]+=tempCF;
        if((memory[memLocation]/0x80-temp/0x80)!=0){
            OF=1;
        }
        else{OF=0;}
    }
}
void print(ofstream & out){
    out<<char(*pdl);
}
char input() {
    string str;
    getline(cin, str);
    char c = str[0];
    return c;
}


int main(int argc, char *argv[]) {
    const vector<string> givenInstructions = {"mov", "add", "sub", "inc", "dec", "mul", "div", "xor", "or", "and",
                                              "not", "rcl", "rcr", "shl", "shr", "push", "pop", "nop", "cmp", "jz",
                                              "jnz", "je", "jne", "ja", "jae", "jb", "jbe", "jnae", "jnb", "jnbe",
                                              "jnc", "jc", "int20h", "int21h"};//all insts
    const vector<string> byteRegs = {"al", "ah", "bl", "bh", "cl", "ch", "dl", "dh"};
    const vector<string> wordRegs = {"ax", "bx", "cx", "dx", "di", "sp", "si", "bp"};
    fstream file(argv[1]);
    int instCount = 0;
    int varBytes = 0;
    string line;
    vector<string> lines;
    vector<label> labels;
    vector<instruct> instructs;
    vector<variable> variables;
    vector<string> variableNames;
    vector<string> labelNames;
    vector<int> instLines;
    while (getline(file, line)) {
        lines.push_back(line);
    }//loads all lines into a vector
    bool afterInt20 = false;
    for (int j = 1; j < lines.size() - 1; j++) {//goes over all lines one by one
        stringstream buffer(lines[j]);
        string item, item2;
        do {
            getline(buffer, item, ' ');//gets the first word
        } while (item.length() == 0);

        for (int k = 0; k < item.length(); k++) {
            item[k] = tolower(item[k]);
        }
        if (item.length() > 0) {
            if (item[item.length() - 1] == ':') {//checks of if label
                struct label lab;
                lab.line = j+1;
                lab.name = item.substr(0, item.length() - 1);
                labelNames.push_back(lab.name);
                labels.push_back(lab);
                continue;
            }
            if (item.compare("int") == 0) {//checks of if int instruction
                while (getline(buffer, item2, ' ')) {
                    if (item2.length() > 0) {
                        for (int h = 0; h < item2.length(); h++) {
                            item2[h] = tolower(item2[h]);
                        }
                        if (item2.compare("20h") == 0) {//if 20h
                            item = item + item2;
                            break;
                        } else if (item2.compare("21h") == 0) {//if21h
                            item = item + item2;
                            break;
                        }
                    }
                }
            }
            if (item.compare("int20h") == 0) {//sets afterInt20 bool false
                instLines.push_back(j);
                struct instruct int20;
                int20.name="int20h";
                instructs.push_back(int20);
                instCount++;
                afterInt20 = true;
                continue;
            } else if (find(givenInstructions.begin(), givenInstructions.end(), item) !=
                       givenInstructions.end()) {//checks if inst in given insts
                struct instruct inst;
                inst.line = j;
                inst.name = item;
                inst.operand1 = "";
                inst.operand2 = "";
                string temp = "";
                string temp2 = "";
                while (getline(buffer, item2, ' ')) {
                    if (item2.length() > 0) {
                        temp += item2;
                    }
                }
                stringstream temps(temp);
                getline(temps, temp2, ',');
                inst.operand1 = temp2;
                getline(temps, temp2, ',');
                inst.operand2 = temp2;
                while (getline(buffer, item2, ' ')) {
                    if (item2.length() > 0) {
                        inst.operand2 += item2;
                    }
                }
                instCount++;
                instLines.push_back(j);
                instructs.push_back(inst);
                continue;
            } else if (!afterInt20) {//if any other line than instructions and labels in before int20h
                cout << "Error";
                return 1;
            }
            //goes over variables
            struct variable var;
            var.name = item;
            while (getline(buffer, item2, ' ')) {//gets second token (db/dw)
                if (item2.length() > 0) {
                    for (int h = 0; h < item2.length(); h++) {
                        item2[h] = tolower(item2[h]);
                    }
                    if (item2.compare("db") == 0) {
                        var.isWord = false;
                        varBytes++;
                        break;
                    } else if (item2.compare("dw") == 0) {
                        var.isWord = true;
                        varBytes += 2;
                        break;
                    } else {
                        cout << "Error";
                        return 0;
                    }
                }
            }
            while (getline(buffer, item2, ' ')) {//gets value
                if (item2.length() > 0) {
                    if (item2[0] == '\'') {
                        char temp = item2[1];
                        var.byteValue = temp;
                        break;
                    } else if (item2[0] == '0' || item2[item2.length() - 1] == 'h') {
                        if (item2[item2.length() - 1] == 'h') {
                            string hex = item2.substr(0, item2.length() - 1);
                            if(var.isWord) {
                                var.wordValue=stoi(hex,0,16);
                            }
                            else {
                                var.byteValue=stoi(hex,0,16);
                            }
                        } else {
                            if(var.isWord) {
                                var.wordValue=stoi(item2,0,16);
                            }
                            else {
                                var.byteValue=stoi(item2,0,16);
                            }
                        }
                    } else if (item2[item2.length() - 1] == 'd' || item2[0] != '0') {
                        string dec = item2[item2.length() - 1] == 'd' ? item2.substr(0, item2.length() - 1) : item2;
                        if(var.isWord) {
                            var.wordValue=stoi(dec,0,10);
                        }
                        else {
                            var.byteValue=stoi(dec,0,10);
                        }
                    }
                }
            }
            var.mem = instCount * 6 + varBytes;
            variables.push_back(var);
        }
    }


    for (int i = 0; i < variables.size(); i++) {
        variableNames.push_back(variables[i].name);
        if (variables[i].isWord) {
            unsigned char valueLow = variables[i].wordValue & 0xff;
            unsigned char valueHigh = (variables[i].wordValue >> 8);
            memory[variables[i].mem - 1] = valueHigh;
            memory[variables[i].mem - 2] = valueLow;
        } else {
            memory[variables[i].mem - 1] = variables[i].byteValue;
        }
    }

    for (int i = 0; i < instructs.size(); i++) {

        struct instruct ins = instructs[i];
        int asd = *pcx;
        int asc = *pcl;
        int ash = *pch;
        int ahp = *pah;
        int alp = *pal;
        int spp = *psp;
        if (ins.name.compare("mov") == 0) {
            //firsrt operand 8 bit reg
            if (find(byteRegs.begin(), byteRegs.end(), ins.operand1) != byteRegs.end()) {
                unsigned char *preg;
                unsigned char *preg2;
                unsigned char op2;
                unsigned short mop2;
                if (ins.operand1.compare("al") == 0) {
                    preg = pal;
                } else if (ins.operand1.compare("ah") == 0) {
                    preg = pah;
                } else if (ins.operand1.compare("bl") == 0) {
                    preg = pbl;
                } else if (ins.operand1.compare("bh") == 0) {
                    preg = pbh;
                } else if (ins.operand1.compare("cl") == 0) {
                    preg = pcl;
                } else if (ins.operand1.compare("ch") == 0) {
                    preg = pch;
                } else if (ins.operand1.compare("dl") == 0) {
                    preg = pdl;
                } else if (ins.operand1.compare("dh") == 0) {
                    preg = pdh;
                }
                //second op reg
                if (find(byteRegs.begin(), byteRegs.end(), ins.operand2) != byteRegs.end()) {
                    if (ins.operand2.compare("al") == 0) {
                        preg2 = pal;
                    } else if (ins.operand2.compare("ah") == 0) {
                        preg2 = pah;
                    } else if (ins.operand2.compare("bl") == 0) {
                        preg2 = pbl;
                    } else if (ins.operand2.compare("bh") == 0) {
                        preg2 = pbh;
                    } else if (ins.operand2.compare("cl") == 0) {
                        preg2 = pcl;
                    } else if (ins.operand2.compare("ch") == 0) {
                        preg2 = pch;
                    } else if (ins.operand2.compare("dl") == 0) {
                        preg2 = pbl;
                    } else if (ins.operand2.compare("dh") == 0) {
                        preg2 = pdh;
                    }
                    mov_reg_reg(preg, preg2);
                    continue;
                }
                    //second op hex
                else if (ins.operand2[0] == '0' || ins.operand2[ins.operand2.length() - 1] == 'h') {//second operand hex
                    if (ins.operand2[ins.operand2.length() - 1] == 'h') {
                        string hex = ins.operand2.substr(0, ins.operand2.length() - 1);
                        op2 = stoi(hex, 0, 16);
                    } else {
                        op2 = stoi(ins.operand2, 0, 16);
                    }
                    mov_reg_byte(preg, op2);
                    continue;
                }
                    //second op dec
                else if (ins.operand2[0] == '1' || ins.operand2[0] == '2' || ins.operand2[0] == '3' ||
                         ins.operand2[0] == '4' || ins.operand2[0] == '5' || ins.operand2[0] == '6' ||
                         ins.operand2[0] == '7' || ins.operand2[0] == '8' || ins.operand2[0] == '9' ||
                         ins.operand2[ins.operand2.length() - 1] == 'd') {
                    if (ins.operand2[ins.operand2.length() - 1] == 'd') {
                        string dec = ins.operand2.substr(0, ins.operand2.length() - 1);
                        op2 = stoi(dec, 0, 10);
                    } else {
                        op2 = stoi(ins.operand2, 0, 10);
                    }
                    mov_reg_byte(preg, op2);
                    continue;
                }

                    //second op char
                else if(ins.operand2[0] == '\'' && ins.operand2[ins.operand2.length()-1] == '\'') {
                    char op3 = ins.operand2[1];
                    mov_reg_byte(preg, op3);
                }

                    //second op mem
                else if (ins.operand2[0] == 'b' && ins.operand2[1] == '[') {
                    string memLoc = ins.operand2.substr(2, ins.operand2.length()-3);
                    //hex
                    if (memLoc[0] == '0' || memLoc[memLoc.length() - 1] == 'h') {
                        string hex;
                        if (memLoc[memLoc.length() - 1] == 'h') {
                            hex = memLoc.substr(0, memLoc.length() - 1);
                        } else {
                            hex = memLoc;
                        }
                        if (stoi(hex, 0, 16) >= (2 << 15)) {
                            cout << "Error";
                            return 1;
                        } else {
                            mop2 = stoi(hex, 0, 16);
                        }
                    }
                        //dec
                    else if (memLoc[0] == '1' || memLoc[0] == '2' || memLoc[0] == '3' || memLoc[0] == '4' ||
                             memLoc[0] == '5' || memLoc[0] == '6' ||
                             memLoc[0] == '7' || memLoc[0] == '8' || memLoc[0] == '9' ||
                             memLoc[memLoc.length() - 1] == 'd') {
                        string dec;
                        if (memLoc[memLoc.length() - 1] == 'd') {
                            dec = memLoc.substr(0, memLoc.length() - 1);
                        } else {
                            dec = memLoc;
                        }
                        if (stoi(dec, 0, 10) >= (2 << 15)) {
                            cout << "Error";
                            return 1;
                        } else {
                            mop2 = stoi(dec, 0, 10);
                        }
                    }
                        //reg
                    else if (find(wordRegs.begin(), wordRegs.end(), memLoc) != wordRegs.end()) {
                        if (memLoc.compare("ax") == 0) {
                            mop2 = *pax;
                        } else if (memLoc.compare("bx") == 0) {
                            mop2 = *pbx;
                        } else if (memLoc.compare("cx") == 0) {
                            mop2 = *pcx;
                        } else if (memLoc.compare("dx") == 0) {
                            mop2 = *pdx;
                        } else if (memLoc.compare("di") == 0) {
                            mop2 = *pdi;
                        } else if (memLoc.compare("si") == 0) {
                            mop2 = *psi;
                        } else if (memLoc.compare("sp") == 0) {
                            mop2 = *psp;
                        } else if (memLoc.compare("bp") == 0) {
                            mop2 = *pbp;
                        }
                    }
                    mov_reg_mem_byte(preg, mop2);
                    continue;
                }
                    //second op variable
                else if (find(variableNames.begin(), variableNames.end(), ins.operand2) != variableNames.end()) {
                    vector<string>::iterator it = find(variableNames.begin(), variableNames.end(), ins.operand2);
                    int index = distance(variableNames.begin(), it);
                    if (!variables[index].isWord) { mop2 = variables[index].mem - 1; }
                    else { mop2 = variables[index].mem - 2; }
                    mov_reg_mem_byte(preg, mop2);
                }
            }
                //firsrt operand 16 bit reg
            else if (find(wordRegs.begin(), wordRegs.end(), ins.operand1) != wordRegs.end()) {
                unsigned short *preg;
                unsigned short *preg2;
                unsigned short op2;
                if (ins.operand1.compare("ax") == 0) {
                    preg = pax;
                } else if (ins.operand1.compare("bx") == 0) {
                    preg = pbx;
                } else if (ins.operand1.compare("cx") == 0) {
                    preg = pcx;
                } else if (ins.operand1.compare("dx") == 0) {
                    preg = pdx;
                } else if (ins.operand1.compare("di") == 0) {
                    preg = pdi;
                } else if (ins.operand1.compare("si") == 0) {
                    preg = psi;
                } else if (ins.operand1.compare("sp") == 0) {
                    preg = psp;
                } else if (ins.operand1.compare("bp") == 0) {
                    preg = pbp;
                }
                //second operand hex
                if (ins.operand2[0] == '0' || ins.operand2[ins.operand2.length() - 1] == 'h') {
                    if (ins.operand2[ins.operand2.length() - 1] == 'h') {
                        string hex = ins.operand2.substr(0, ins.operand2.length() - 1);
                        op2 = stoi(hex, 0, 16);
                    } else {
                        op2 = stoi(ins.operand2, 0, 16);
                    }
                    mov_reg_word(preg, op2);
                    continue;
                }
                    //second op dec
                else if (ins.operand2[0] == '1' || ins.operand2[0] == '2' || ins.operand2[0] == '3' ||
                         ins.operand2[0] == '4' || ins.operand2[0] == '5' || ins.operand2[0] == '6' ||
                         ins.operand2[0] == '7' || ins.operand2[0] == '8' || ins.operand2[0] == '9' ||
                         ins.operand2[ins.operand2.length() - 1] == 'd') {
                    if (ins.operand2[ins.operand2.length() - 1] == 'd') {
                        string dec = ins.operand2.substr(0, ins.operand2.length() - 1);
                        op2 = stoi(dec, 0, 10);
                    } else {
                        op2 = stoi(ins.operand2, 0, 10);
                    }
                    mov_reg_word(preg, op2);
                    continue;
                }
                    //second op reg
                else if (find(wordRegs.begin(), wordRegs.end(), ins.operand2) != wordRegs.end()) {
                    if (ins.operand2.compare("ax") == 0) {
                        preg2 = pax;
                    } else if (ins.operand2.compare("bx") == 0) {
                        preg2 = pbx;
                    } else if (ins.operand2.compare("cx") == 0) {
                        preg2 = pcx;
                    } else if (ins.operand2.compare("dx") == 0) {
                        preg2 = pdx;
                    } else if (ins.operand2.compare("di") == 0) {
                        preg2 = pdi;
                    } else if (ins.operand2.compare("si") == 0) {
                        preg2 = psi;
                    } else if (ins.operand2.compare("sp") == 0) {
                        preg2 = psp;
                    } else if (ins.operand2.compare("bp") == 0) {
                        preg2 = pbp;
                    }
                    mov_reg_reg(preg, preg2);
                    continue;
                }
                    //second op mem
                else if (ins.operand2[0] == 'w' && ins.operand2[1] == '[') {
                    string memLoc = ins.operand2.substr(2, ins.operand2.length() - 1);
                    //hex
                    if (memLoc[0] == '0' || memLoc[memLoc.length() - 1] == 'h') {
                        string hex;
                        if (memLoc[memLoc.length() - 1] == 'h') {
                            hex = memLoc.substr(0, memLoc.length() - 1);
                        } else {
                            hex = memLoc;
                        }
                        if (stoi(hex, 0, 16) >= (2 << 15)) {
                            cout << "Error";
                            return 1;
                        } else {
                            op2 = stoi(hex, 0, 16);
                        }
                    }
                        //dec
                    else if (memLoc[0] == '1' || memLoc[0] == '2' || memLoc[0] == '3' || memLoc[0] == '4' ||
                             memLoc[0] == '5' || memLoc[0] == '6' ||
                             memLoc[0] == '7' || memLoc[0] == '8' || memLoc[0] == '9' ||
                             memLoc[memLoc.length() - 1] == 'd') {
                        string dec;
                        if (memLoc[memLoc.length() - 1] == 'd') {
                            dec = memLoc.substr(0, memLoc.length() - 1);
                        } else {
                            dec = memLoc;
                        }
                        if (stoi(dec, 0, 10) >= (2 << 15)) {
                            cout << "Error";
                            return 1;
                        } else {
                            op2 = stoi(dec, 0, 10);
                        }
                    }
                        //reg
                    else if (find(wordRegs.begin(), wordRegs.end(), memLoc) != wordRegs.end()) {
                        unsigned short op2;
                        if (memLoc.compare("ax") == 0) {
                            op2 = *pax;
                        } else if (memLoc.compare("bx") == 0) {
                            op2 = *pbx;
                        } else if (memLoc.compare("cx") == 0) {
                            op2 = *pcx;
                        } else if (memLoc.compare("dx") == 0) {
                            op2 = *pdx;
                        } else if (memLoc.compare("di") == 0) {
                            op2 = *pdi;
                        } else if (memLoc.compare("si") == 0) {
                            op2 = *psi;
                        } else if (memLoc.compare("sp") == 0) {
                            op2 = *psp;
                        } else if (memLoc.compare("bp") == 0) {
                            op2 = *pbp;
                        }
                    }
                    mov_reg_mem_word(preg, op2);
                    continue;
                }
                    //second op variable
                else if (find(variableNames.begin(), variableNames.end(), ins.operand2) != variableNames.end()) {
                    vector<string>::iterator it = find(variableNames.begin(), variableNames.end(), ins.operand2);
                    int index = distance(variableNames.begin(), it);
                    if (!variables[index].isWord) { op2 = variables[index].mem - 1; }
                    else { op2 = variables[index].mem - 2; }
                    mov_reg_mem_word(preg, op2);
                }
                    //second op offset
                else if(ins.operand2.substr(0,6).compare("offset") == 0) {
                    string varName = ins.operand2.substr(6);
                    vector<string>::iterator it = find(variableNames.begin(), variableNames.end(), varName);
                    int index = distance(variableNames.begin(), it);
                    if (!variables[index].isWord) { op2 = variables[index].mem - 1; }
                    else { op2 = variables[index].mem - 2; }
                    mov_reg_word(preg, op2);
                }
            }
                //first byte operand memory
            else if (ins.operand1[0] == 'b' && ins.operand1[1] == '[') {
                string memLoc = ins.operand1.substr(2, ins.operand1.length() - 1);
                unsigned short memByte;
                unsigned char *preg2;
                unsigned char op2;
                //hex
                if (memLoc[0] == '0' || memLoc[memLoc.length() - 1] == 'h') {
                    string hex;
                    if (memLoc[memLoc.length() - 1] == 'h') {
                        hex = memLoc.substr(0, memLoc.length() - 1);
                    } else {
                        hex = memLoc;
                    }
                    if (stoi(hex, 0, 16) >= (2 << 15)) {
                        cout << "Error";
                        return 1;
                    } else {
                        memByte = stoi(hex, 0, 16);
                    }
                }
                    //dec
                else if (memLoc[0] == '1' || memLoc[0] == '2' || memLoc[0] == '3' || memLoc[0] == '4' ||
                         memLoc[0] == '5' || memLoc[0] == '6' ||
                         memLoc[0] == '7' || memLoc[0] == '8' || memLoc[0] == '9' ||
                         memLoc[memLoc.length() - 1] == 'd') {
                    string dec;
                    if (memLoc[memLoc.length() - 1] == 'd') {
                        dec = memLoc.substr(0, memLoc.length() - 1);
                    } else {
                        dec = memLoc;
                    }
                    if (stoi(dec, 0, 10) >= (2 << 15)) {
                        cout << "Error";
                        return 1;
                    } else {
                        memByte = stoi(dec, 0, 10);
                    }
                }
                    //reg
                else if (find(wordRegs.begin(), wordRegs.end(), memLoc) != wordRegs.end()) {
                    if (memLoc.compare("ax") == 0) {
                        memByte = *pax;
                    } else if (memLoc.compare("bx") == 0) {
                        memByte = *pbx;
                    } else if (memLoc.compare("cx") == 0) {
                        memByte = *pcx;
                    } else if (memLoc.compare("dx") == 0) {
                        memByte = *pdx;
                    } else if (memLoc.compare("di") == 0) {
                        memByte = *pdi;
                    } else if (memLoc.compare("si") == 0) {
                        memByte = *psi;
                    } else if (memLoc.compare("sp") == 0) {
                        memByte = *psp;
                    } else if (memLoc.compare("bp") == 0) {
                        memByte = *pbp;
                    }
                }
                //second operand hex
                if (ins.operand2[0] == '0' || ins.operand2[ins.operand2.length() - 1] == 'h') {//second operand hex
                    if (ins.operand2[ins.operand2.length() - 1] == 'h') {
                        string hex = ins.operand2.substr(0, ins.operand2.length() - 1);
                        op2 = stoi(hex, 0, 16);
                    } else {
                        op2 = stoi(ins.operand2, 0, 16);
                    }
                    mov_mem_byte(memByte, op2);
                    continue;
                }
                    //second operand dec
                else if (ins.operand2[0] == '1' || ins.operand2[0] == '2' || ins.operand2[0] == '3' ||
                         ins.operand2[0] == '4' || ins.operand2[0] == '5' || ins.operand2[0] == '6' ||
                         ins.operand2[0] == '7' || ins.operand2[0] == '8' || ins.operand2[0] == '9' ||
                         ins.operand2[ins.operand2.length() - 1] == 'd') {
                    if (ins.operand2[ins.operand2.length() - 1] == 'd') {
                        string dec = ins.operand2.substr(0, ins.operand2.length() - 1);
                        op2 = stoi(dec, 0, 10);
                    } else {
                        op2 = stoi(ins.operand2, 0, 10);
                    }
                    mov_mem_byte(memByte, op2);
                    continue;
                }
                    //second operand reg
                else if (find(byteRegs.begin(), byteRegs.end(), ins.operand2) != byteRegs.end()) {
                    if (ins.operand2.compare("al") == 0) {
                        preg2 = pal;
                    } else if (ins.operand2.compare("ah") == 0) {
                        preg2 = pah;
                    } else if (ins.operand2.compare("bl") == 0) {
                        preg2 = pbl;
                    } else if (ins.operand2.compare("bh") == 0) {
                        preg2 = pbh;
                    } else if (ins.operand2.compare("cl") == 0) {
                        preg2 = pcl;
                    } else if (ins.operand2.compare("ch") == 0) {
                        preg2 = pch;
                    } else if (ins.operand2.compare("dl") == 0) {
                        preg2 = pbl;
                    } else if (ins.operand2.compare("dh") == 0) {
                        preg2 = pdh;
                    }
                    mov_mem_byte_reg(preg2, memByte);
                    continue;
                }
            }
                //first operand word memory
                //first operand word memory
            else if(ins.operand1[0] == 'w' && ins.operand1[1] == '[') {
                string memLoc = ins.operand1.substr(2, ins.operand1.length() - 3);
                unsigned short memByte;
                unsigned short *preg2;
                unsigned short op2;
                //hex
                if (memLoc[0] == '0' || memLoc[memLoc.length() - 1] == 'h') {
                    string hex;
                    if (memLoc[memLoc.length() - 1] == 'h') {
                        hex = memLoc.substr(0, memLoc.length() - 1);
                    } else {
                        hex = memLoc;
                    }
                    if (stoi(hex, 0, 16) >= (2 << 15)) {
                        cout << "Error";
                        return 1;
                    } else {
                        memByte = stoi(hex, 0, 16);
                    }
                }
                    //dec
                else if (memLoc[0] == '1' || memLoc[0] == '2' || memLoc[0] == '3' || memLoc[0] == '4' ||
                         memLoc[0] == '5' || memLoc[0] == '6' ||
                         memLoc[0] == '7' || memLoc[0] == '8' || memLoc[0] == '9' ||
                         memLoc[memLoc.length() - 1] == 'd') {
                    string dec;
                    if (memLoc[memLoc.length() - 1] == 'd') {
                        dec = memLoc.substr(0, memLoc.length() - 1);
                    } else {
                        dec = memLoc;
                    }
                    if (stoi(dec, 0, 10) >= (2 << 15)) {
                        cout << "Error";
                        return 1;
                    } else {
                        memByte = stoi(dec, 0, 10);
                    }
                }
                    //reg
                else if (find(wordRegs.begin(), wordRegs.end(), memLoc) != wordRegs.end()) {
                    if (memLoc.compare("ax") == 0) {
                        memByte = *pax;
                    } else if (memLoc.compare("bx") == 0) {
                        memByte = *pbx;
                    } else if (memLoc.compare("cx") == 0) {
                        memByte = *pcx;
                    } else if (memLoc.compare("dx") == 0) {
                        memByte = *pdx;
                    } else if (memLoc.compare("di") == 0) {
                        memByte = *pdi;
                    } else if (memLoc.compare("si") == 0) {
                        memByte = *psi;
                    } else if (memLoc.compare("sp") == 0) {
                        memByte = *psp;
                    } else if (memLoc.compare("bp") == 0) {
                        memByte = *pbp;
                    }
                }
                //second operand hex
                if (ins.operand2[0] == '0' || ins.operand2[ins.operand2.length() - 1] == 'h') {
                    if (ins.operand2[ins.operand2.length() - 1] == 'h') {
                        string hex = ins.operand2.substr(0, ins.operand2.length() - 1);
                        op2 = stoi(hex, 0, 16);
                    } else {
                        op2 = stoi(ins.operand2, 0, 16);
                    }
                    mov_mem_word(memByte, op2);
                    continue;
                }
                    //second operand dec
                else if (ins.operand2[0] == '1' || ins.operand2[0] == '2' || ins.operand2[0] == '3' ||
                         ins.operand2[0] == '4' || ins.operand2[0] == '5' || ins.operand2[0] == '6' ||
                         ins.operand2[0] == '7' || ins.operand2[0] == '8' || ins.operand2[0] == '9' ||
                         ins.operand2[ins.operand2.length() - 1] == 'd') {
                    if (ins.operand2[ins.operand2.length() - 1] == 'd') {
                        string dec = ins.operand2.substr(0, ins.operand2.length() - 1);
                        op2 = stoi(dec, 0, 10);
                    } else {
                        op2 = stoi(ins.operand2, 0, 10);
                    }
                    mov_mem_word(memByte, op2);
                    continue;
                }
                    //second operand reg
                else if (find(byteRegs.begin(), byteRegs.end(), ins.operand2) != byteRegs.end()) {
                    if (ins.operand2.compare("ax") == 0) {
                        preg2 = pax;
                    } else if (ins.operand2.compare("bx") == 0) {
                        preg2 = pbx;
                    } else if (ins.operand2.compare("cx") == 0) {
                        preg2 = pcx;
                    } else if (ins.operand2.compare("dx") == 0) {
                        preg2 = pdx;
                    } else if (ins.operand2.compare("di") == 0) {
                        preg2 = pdi;
                    } else if (ins.operand2.compare("si") == 0) {
                        preg2 = psi;
                    } else if (ins.operand2.compare("sp") == 0) {
                        preg2 = psp;
                    } else if (ins.operand2.compare("bp") == 0) {
                        preg2 = pbp;
                    }
                    mov_mem_word_reg(preg2, memByte);
                    continue;
                }
            }
        }
        else if(ins.name.compare("add") == 0) {
//firsrt operand 8 bit reg
            if (find(byteRegs.begin(), byteRegs.end(), ins.operand1) != byteRegs.end()) {
                unsigned char *preg;
                unsigned char *preg2;
                unsigned char op2;
                unsigned short mop2;
                if (ins.operand1.compare("al") == 0) {
                    preg = pal;
                } else if (ins.operand1.compare("ah") == 0) {
                    preg = pah;
                } else if (ins.operand1.compare("bl") == 0) {
                    preg = pbl;
                } else if (ins.operand1.compare("bh") == 0) {
                    preg = pbh;
                } else if (ins.operand1.compare("cl") == 0) {
                    preg = pcl;
                } else if (ins.operand1.compare("ch") == 0) {
                    preg = pch;
                } else if (ins.operand1.compare("dl") == 0) {
                    preg = pdl;
                } else if (ins.operand1.compare("dh") == 0) {
                    preg = pdh;
                }
                //second op reg
                if (find(byteRegs.begin(), byteRegs.end(), ins.operand2) != byteRegs.end()) {
                    if (ins.operand2.compare("al") == 0) {
                        preg2 = pal;
                    } else if (ins.operand2.compare("ah") == 0) {
                        preg2 = pah;
                    } else if (ins.operand2.compare("bl") == 0) {
                        preg2 = pbl;
                    } else if (ins.operand2.compare("bh") == 0) {
                        preg2 = pbh;
                    } else if (ins.operand2.compare("cl") == 0) {
                        preg2 = pcl;
                    } else if (ins.operand2.compare("ch") == 0) {
                        preg2 = pch;
                    } else if (ins.operand2.compare("dl") == 0) {
                        preg2 = pbl;
                    } else if (ins.operand2.compare("dh") == 0) {
                        preg2 = pdh;
                    }
                    add_bregister_bregister(preg, preg2);
                    continue;
                }
                    //second op hex
                else if (ins.operand2[0] == '0' || ins.operand2[ins.operand2.length() - 1] == 'h') {//second operand hex
                    if (ins.operand2[ins.operand2.length() - 1] == 'h') {
                        string hex = ins.operand2.substr(0, ins.operand2.length() - 1);
                        op2 = stoi(hex, 0, 16);
                    } else {
                        op2 = stoi(ins.operand2, 0, 16);
                    }
                    add_reg_byte(preg, op2);
                    continue;
                }
                    //second op dec
                else if (ins.operand2[0] == '1' || ins.operand2[0] == '2' || ins.operand2[0] == '3' ||
                         ins.operand2[0] == '4' || ins.operand2[0] == '5' || ins.operand2[0] == '6' ||
                         ins.operand2[0] == '7' || ins.operand2[0] == '8' || ins.operand2[0] == '9' ||
                         ins.operand2[ins.operand2.length() - 1] == 'd') {
                    if (ins.operand2[ins.operand2.length() - 1] == 'd') {
                        string dec = ins.operand2.substr(0, ins.operand2.length() - 1);
                        op2 = stoi(dec, 0, 10);
                    } else {
                        op2 = stoi(ins.operand2, 0, 10);
                    }
                    add_reg_byte(preg, op2);
                    continue;
                }
                else if(ins.operand2[0] == '\'' && ins.operand2[ins.operand2.length()-1] == '\'') {
                    char op3 = ins.operand2[1];
                    add_reg_byte(preg, op3);
                }
                    //second op mem
                else if (ins.operand2[0] == 'b' && ins.operand2[1] == '[') {
                    string memLoc = ins.operand2.substr(2, ins.operand2.length() - 1);
                    //hex
                    if (memLoc[0] == '0' || memLoc[memLoc.length() - 1] == 'h') {
                        string hex;
                        if (memLoc[memLoc.length() - 1] == 'h') {
                            hex = memLoc.substr(0, memLoc.length() - 1);
                        } else {
                            hex = memLoc;
                        }
                        if (stoi(hex, 0, 16) >= (2 << 15)) {
                            cout << "Error";
                            return 1;
                        } else {
                            mop2 = stoi(hex, 0, 16);
                        }
                    }
                        //dec
                    else if (memLoc[0] == '1' || memLoc[0] == '2' || memLoc[0] == '3' || memLoc[0] == '4' ||
                             memLoc[0] == '5' || memLoc[0] == '6' ||
                             memLoc[0] == '7' || memLoc[0] == '8' || memLoc[0] == '9' ||
                             memLoc[memLoc.length() - 1] == 'd') {
                        string dec;
                        if (memLoc[memLoc.length() - 1] == 'd') {
                            dec = memLoc.substr(0, memLoc.length() - 1);
                        } else {
                            dec = memLoc;
                        }
                        if (stoi(dec, 0, 10) >= (2 << 15)) {
                            cout << "Error";
                            return 1;
                        } else {
                            mop2 = stoi(dec, 0, 10);
                        }
                    }
                        //reg
                    else if (find(wordRegs.begin(), wordRegs.end(), memLoc) != wordRegs.end()) {
                        if (memLoc.compare("ax") == 0) {
                            mop2 = *pax;
                        } else if (memLoc.compare("bx") == 0) {
                            mop2 = *pbx;
                        } else if (memLoc.compare("cx") == 0) {
                            mop2 = *pcx;
                        } else if (memLoc.compare("dx") == 0) {
                            mop2 = *pdx;
                        } else if (memLoc.compare("di") == 0) {
                            mop2 = *pdi;
                        } else if (memLoc.compare("si") == 0) {
                            mop2 = *psi;
                        } else if (memLoc.compare("sp") == 0) {
                            mop2 = *psp;
                        } else if (memLoc.compare("bp") == 0) {
                            mop2 = *pbp;
                        }
                    }
                    add_register_byte_mem(preg, mop2);
                    continue;
                }
                    //second op variable
                else if (find(variableNames.begin(), variableNames.end(), ins.operand2) != variableNames.end()) {
                    vector<string>::iterator it = find(variableNames.begin(), variableNames.end(), ins.operand2);
                    int index = distance(variableNames.begin(), it);
                    if (!variables[index].isWord) { mop2 = variables[index].mem - 1; }
                    else { mop2 = variables[index].mem - 2; }
                    add_register_byte_mem(preg, mop2);
                }
            }
                //firsrt operand 16 bit reg
            else if (find(wordRegs.begin(), wordRegs.end(), ins.operand1) != wordRegs.end()) {
                unsigned short *preg;
                unsigned short *preg2;
                unsigned short op2;
                if (ins.operand1.compare("ax") == 0) {
                    preg = pax;
                } else if (ins.operand1.compare("bx") == 0) {
                    preg = pbx;
                } else if (ins.operand1.compare("cx") == 0) {
                    preg = pcx;
                } else if (ins.operand1.compare("dx") == 0) {
                    preg = pdx;
                } else if (ins.operand1.compare("di") == 0) {
                    preg = pdi;
                } else if (ins.operand1.compare("si") == 0) {
                    preg = psi;
                } else if (ins.operand1.compare("sp") == 0) {
                    preg = psp;
                } else if (ins.operand1.compare("bp") == 0) {
                    preg = pbp;
                }
                //second operand hex
                if (ins.operand2[0] == '0' || ins.operand2[ins.operand2.length() - 1] == 'h') {
                    if (ins.operand2[ins.operand2.length() - 1] == 'h') {
                        string hex = ins.operand2.substr(0, ins.operand2.length() - 1);
                        op2 = stoi(hex, 0, 16);
                    } else {
                        op2 = stoi(ins.operand2, 0, 16);
                    }
                    add_reg_word(preg, op2);
                    continue;
                }
                    //second op dec
                else if (ins.operand2[0] == '1' || ins.operand2[0] == '2' || ins.operand2[0] == '3' ||
                         ins.operand2[0] == '4' || ins.operand2[0] == '5' || ins.operand2[0] == '6' ||
                         ins.operand2[0] == '7' || ins.operand2[0] == '8' || ins.operand2[0] == '9' ||
                         ins.operand2[ins.operand2.length() - 1] == 'd') {
                    if (ins.operand2[ins.operand2.length() - 1] == 'd') {
                        string dec = ins.operand2.substr(0, ins.operand2.length() - 1);
                        op2 = stoi(dec, 0, 10);
                    } else {
                        op2 = stoi(ins.operand2, 0, 10);
                    }
                    add_reg_word(preg, op2);
                    continue;
                }
                    //second op reg
                else if (find(wordRegs.begin(), wordRegs.end(), ins.operand2) != wordRegs.end()) {
                    if (ins.operand2.compare("ax") == 0) {
                        preg2 = pax;
                    } else if (ins.operand2.compare("bx") == 0) {
                        preg2 = pbx;
                    } else if (ins.operand2.compare("cx") == 0) {
                        preg2 = pcx;
                    } else if (ins.operand2.compare("dx") == 0) {
                        preg2 = pdx;
                    } else if (ins.operand2.compare("di") == 0) {
                        preg2 = pdi;
                    } else if (ins.operand2.compare("si") == 0) {
                        preg2 = psi;
                    } else if (ins.operand2.compare("sp") == 0) {
                        preg2 = psp;
                    } else if (ins.operand2.compare("bp") == 0) {
                        preg2 = pbp;
                    }
                    add_wregister_wregister(preg, preg2);
                    continue;
                }
                    //second op mem
                else if (ins.operand2[0] == 'w' && ins.operand2[1] == '[') {
                    string memLoc = ins.operand2.substr(2, ins.operand2.length() - 1);
                    //hex
                    if (memLoc[0] == '0' || memLoc[memLoc.length() - 1] == 'h') {
                        string hex;
                        if (memLoc[memLoc.length() - 1] == 'h') {
                            hex = memLoc.substr(0, memLoc.length() - 1);
                        } else {
                            hex = memLoc;
                        }
                        if (stoi(hex, 0, 16) >= (2 << 15)) {
                            cout << "Error";
                            return 1;
                        } else {
                            op2 = stoi(hex, 0, 16);
                        }
                    }
                        //dec
                    else if (memLoc[0] == '1' || memLoc[0] == '2' || memLoc[0] == '3' || memLoc[0] == '4' ||
                             memLoc[0] == '5' || memLoc[0] == '6' ||
                             memLoc[0] == '7' || memLoc[0] == '8' || memLoc[0] == '9' ||
                             memLoc[memLoc.length() - 1] == 'd') {
                        string dec;
                        if (memLoc[memLoc.length() - 1] == 'd') {
                            dec = memLoc.substr(0, memLoc.length() - 1);
                        } else {
                            dec = memLoc;
                        }
                        if (stoi(dec, 0, 10) >= (2 << 15)) {
                            cout << "Error";
                            return 1;
                        } else {
                            op2 = stoi(dec, 0, 10);
                        }
                    }
                        //reg
                    else if (find(wordRegs.begin(), wordRegs.end(), memLoc) != wordRegs.end()) {
                        unsigned short op2;
                        if (memLoc.compare("ax") == 0) {
                            op2 = *pax;
                        } else if (memLoc.compare("bx") == 0) {
                            op2 = *pbx;
                        } else if (memLoc.compare("cx") == 0) {
                            op2 = *pcx;
                        } else if (memLoc.compare("dx") == 0) {
                            op2 = *pdx;
                        } else if (memLoc.compare("di") == 0) {
                            op2 = *pdi;
                        } else if (memLoc.compare("si") == 0) {
                            op2 = *psi;
                        } else if (memLoc.compare("sp") == 0) {
                            op2 = *psp;
                        } else if (memLoc.compare("bp") == 0) {
                            op2 = *pbp;
                        }
                    }
                    add_register_word_mem(preg, op2);
                    continue;
                }
                    //second op variable
                else if (find(variableNames.begin(), variableNames.end(), ins.operand2) != variableNames.end()) {
                    vector<string>::iterator it = find(variableNames.begin(), variableNames.end(), ins.operand2);
                    int index = distance(variableNames.begin(), it);
                    if (!variables[index].isWord) { op2 = variables[index].mem - 1; }
                    else { op2 = variables[index].mem - 2; }
                    add_register_word_mem(preg, op2);
                }
            }
                //first byte operand memory
            else if (ins.operand1[0] == 'b' && ins.operand1[1] == '[') {
                string memLoc = ins.operand1.substr(2, ins.operand1.length() - 1);
                unsigned short memByte;
                unsigned char *preg2;
                unsigned char op2;
                //hex
                if (memLoc[0] == '0' || memLoc[memLoc.length() - 1] == 'h') {
                    string hex;
                    if (memLoc[memLoc.length() - 1] == 'h') {
                        hex = memLoc.substr(0, memLoc.length() - 1);
                    } else {
                        hex = memLoc;
                    }
                    if (stoi(hex, 0, 16) >= (2 << 15)) {
                        cout << "Error";
                        return 1;
                    } else {
                        memByte = stoi(hex, 0, 16);
                    }
                }
                    //dec
                else if (memLoc[0] == '1' || memLoc[0] == '2' || memLoc[0] == '3' || memLoc[0] == '4' ||
                         memLoc[0] == '5' || memLoc[0] == '6' ||
                         memLoc[0] == '7' || memLoc[0] == '8' || memLoc[0] == '9' ||
                         memLoc[memLoc.length() - 1] == 'd') {
                    string dec;
                    if (memLoc[memLoc.length() - 1] == 'd') {
                        dec = memLoc.substr(0, memLoc.length() - 1);
                    } else {
                        dec = memLoc;
                    }
                    if (stoi(dec, 0, 10) >= (2 << 15)) {
                        cout << "Error";
                        return 1;
                    } else {
                        memByte = stoi(dec, 0, 10);
                    }
                }
                    //reg
                else if (find(wordRegs.begin(), wordRegs.end(), memLoc) != wordRegs.end()) {
                    if (memLoc.compare("ax") == 0) {
                        memByte = *pax;
                    } else if (memLoc.compare("bx") == 0) {
                        memByte = *pbx;
                    } else if (memLoc.compare("cx") == 0) {
                        memByte = *pcx;
                    } else if (memLoc.compare("dx") == 0) {
                        memByte = *pdx;
                    } else if (memLoc.compare("di") == 0) {
                        memByte = *pdi;
                    } else if (memLoc.compare("si") == 0) {
                        memByte = *psi;
                    } else if (memLoc.compare("sp") == 0) {
                        memByte = *psp;
                    } else if (memLoc.compare("bp") == 0) {
                        memByte = *pbp;
                    }
                }
                //second operand hex
                if (ins.operand2[0] == '0' || ins.operand2[ins.operand2.length() - 1] == 'h') {//second operand hex
                    if (ins.operand2[ins.operand2.length() - 1] == 'h') {
                        string hex = ins.operand2.substr(0, ins.operand2.length() - 1);
                        op2 = stoi(hex, 0, 16);
                    } else {
                        op2 = stoi(ins.operand2, 0, 16);
                    }
                    add_mem_byte(memByte, op2);
                    continue;
                }
                    //second operand dec
                else if (ins.operand2[0] == '1' || ins.operand2[0] == '2' || ins.operand2[0] == '3' ||
                         ins.operand2[0] == '4' || ins.operand2[0] == '5' || ins.operand2[0] == '6' ||
                         ins.operand2[0] == '7' || ins.operand2[0] == '8' || ins.operand2[0] == '9' ||
                         ins.operand2[ins.operand2.length() - 1] == 'd') {
                    if (ins.operand2[ins.operand2.length() - 1] == 'd') {
                        string dec = ins.operand2.substr(0, ins.operand2.length() - 1);
                        op2 = stoi(dec, 0, 10);
                    } else {
                        op2 = stoi(ins.operand2, 0, 10);
                    }
                    add_mem_byte(memByte, op2);
                    continue;
                }
                    //second operand reg
                else if (find(byteRegs.begin(), byteRegs.end(), ins.operand2) != byteRegs.end()) {
                    if (ins.operand2.compare("al") == 0) {
                        preg2 = pal;
                    } else if (ins.operand2.compare("ah") == 0) {
                        preg2 = pah;
                    } else if (ins.operand2.compare("bl") == 0) {
                        preg2 = pbl;
                    } else if (ins.operand2.compare("bh") == 0) {
                        preg2 = pbh;
                    } else if (ins.operand2.compare("cl") == 0) {
                        preg2 = pcl;
                    } else if (ins.operand2.compare("ch") == 0) {
                        preg2 = pch;
                    } else if (ins.operand2.compare("dl") == 0) {
                        preg2 = pbl;
                    } else if (ins.operand2.compare("dh") == 0) {
                        preg2 = pdh;
                    }
                    add_mem_byte_register(memByte, preg2);
                    continue;
                }
            }
                //first operand word memory
            else if(ins.operand1[0] == 'w' && ins.operand1[1] == '[') {
                string memLoc = ins.operand1.substr(2, ins.operand1.length() - 1);
                unsigned short memByte;
                unsigned char *preg2;
                unsigned short op2;
                //hex
                if (memLoc[0] == '0' || memLoc[memLoc.length() - 1] == 'h') {
                    string hex;
                    if (memLoc[memLoc.length() - 1] == 'h') {
                        hex = memLoc.substr(0, memLoc.length() - 1);
                    } else {
                        hex = memLoc;
                    }
                    if (stoi(hex, 0, 16) >= (2 << 15)) {
                        cout << "Error";
                        return 1;
                    } else {
                        memByte = stoi(hex, 0, 16);
                    }
                }
                    //dec
                else if (memLoc[0] == '1' || memLoc[0] == '2' || memLoc[0] == '3' || memLoc[0] == '4' ||
                         memLoc[0] == '5' || memLoc[0] == '6' ||
                         memLoc[0] == '7' || memLoc[0] == '8' || memLoc[0] == '9' ||
                         memLoc[memLoc.length() - 1] == 'd') {
                    string dec;
                    if (memLoc[memLoc.length() - 1] == 'd') {
                        dec = memLoc.substr(0, memLoc.length() - 1);
                    } else {
                        dec = memLoc;
                    }
                    if (stoi(dec, 0, 10) >= (2 << 15)) {
                        cout << "Error";
                        return 1;
                    } else {
                        memByte = stoi(dec, 0, 10);
                    }
                }
                    //reg
                else if (find(wordRegs.begin(), wordRegs.end(), memLoc) != wordRegs.end()) {
                    if (memLoc.compare("ax") == 0) {
                        memByte = *pax;
                    } else if (memLoc.compare("bx") == 0) {
                        memByte = *pbx;
                    } else if (memLoc.compare("cx") == 0) {
                        memByte = *pcx;
                    } else if (memLoc.compare("dx") == 0) {
                        memByte = *pdx;
                    } else if (memLoc.compare("di") == 0) {
                        memByte = *pdi;
                    } else if (memLoc.compare("si") == 0) {
                        memByte = *psi;
                    } else if (memLoc.compare("sp") == 0) {
                        memByte = *psp;
                    } else if (memLoc.compare("bp") == 0) {
                        memByte = *pbp;
                    }
                }
                //second operand hex
                if (ins.operand2[0] == '0' || ins.operand2[ins.operand2.length() - 1] == 'h') {
                    if (ins.operand2[ins.operand2.length() - 1] == 'h') {
                        string hex = ins.operand2.substr(0, ins.operand2.length() - 1);
                        op2 = stoi(hex, 0, 16);
                    } else {
                        op2 = stoi(ins.operand2, 0, 16);
                    }
                    add_mem_word(memByte, op2);
                    continue;
                }
                    //second operand dec
                else if (ins.operand2[0] == '1' || ins.operand2[0] == '2' || ins.operand2[0] == '3' ||
                         ins.operand2[0] == '4' || ins.operand2[0] == '5' || ins.operand2[0] == '6' ||
                         ins.operand2[0] == '7' || ins.operand2[0] == '8' || ins.operand2[0] == '9' ||
                         ins.operand2[ins.operand2.length() - 1] == 'd') {
                    if (ins.operand2[ins.operand2.length() - 1] == 'd') {
                        string dec = ins.operand2.substr(0, ins.operand2.length() - 1);
                        op2 = stoi(dec, 0, 10);
                    } else {
                        op2 = stoi(ins.operand2, 0, 10);
                    }
                    add_mem_word(memByte, op2);
                    continue;
                }
                    //second operand reg
                else if (find(byteRegs.begin(), byteRegs.end(), ins.operand2) != byteRegs.end()) {
                    if (ins.operand2.compare("al") == 0) {
                        preg2 = pal;
                    } else if (ins.operand2.compare("ah") == 0) {
                        preg2 = pah;
                    } else if (ins.operand2.compare("bl") == 0) {
                        preg2 = pbl;
                    } else if (ins.operand2.compare("bh") == 0) {
                        preg2 = pbh;
                    } else if (ins.operand2.compare("cl") == 0) {
                        preg2 = pcl;
                    } else if (ins.operand2.compare("ch") == 0) {
                        preg2 = pch;
                    } else if (ins.operand2.compare("dl") == 0) {
                        preg2 = pbl;
                    } else if (ins.operand2.compare("dh") == 0) {
                        preg2 = pdh;
                    }
                    add_mem_word_register(memByte, preg2);
                    continue;
                }
            }
        }
        else if(ins.name.compare("and") == 0) {
            //firsrt operand 8 bit reg
            if (find(byteRegs.begin(), byteRegs.end(), ins.operand1) != byteRegs.end()) {
                unsigned char *preg;
                unsigned char *preg2;
                unsigned char op2;
                unsigned short mop2;
                if (ins.operand1.compare("al") == 0) {
                    preg = pal;
                } else if (ins.operand1.compare("ah") == 0) {
                    preg = pah;
                } else if (ins.operand1.compare("bl") == 0) {
                    preg = pbl;
                } else if (ins.operand1.compare("bh") == 0) {
                    preg = pbh;
                } else if (ins.operand1.compare("cl") == 0) {
                    preg = pcl;
                } else if (ins.operand1.compare("ch") == 0) {
                    preg = pch;
                } else if (ins.operand1.compare("dl") == 0) {
                    preg = pdl;
                } else if (ins.operand1.compare("dh") == 0) {
                    preg = pdh;
                }
                //second op hex
                if (ins.operand2[0] == '0' || ins.operand2[ins.operand2.length() - 1] == 'h') {//second operand hex
                    if (ins.operand2[ins.operand2.length() - 1] == 'h') {
                        string hex = ins.operand2.substr(0, ins.operand2.length() - 1);
                        op2 = stoi(hex, 0, 16);
                    } else {
                        op2 = stoi(ins.operand2, 0, 16);
                    }
                    and_register_byte(preg, op2);
                    continue;
                }
                    //second op dec
                else if (ins.operand2[0] == '1' || ins.operand2[0] == '2' || ins.operand2[0] == '3' ||
                         ins.operand2[0] == '4' || ins.operand2[0] == '5' || ins.operand2[0] == '6' ||
                         ins.operand2[0] == '7' || ins.operand2[0] == '8' || ins.operand2[0] == '9' ||
                         ins.operand2[ins.operand2.length() - 1] == 'd') {
                    if (ins.operand2[ins.operand2.length() - 1] == 'd') {
                        string dec = ins.operand2.substr(0, ins.operand2.length() - 1);
                        op2 = stoi(dec, 0, 10);
                    } else {
                        op2 = stoi(ins.operand2, 0, 10);
                    }
                    and_register_byte(preg, op2);
                    continue;
                }
                    //second op reg
                else if (find(byteRegs.begin(), byteRegs.end(), ins.operand2) != byteRegs.end()) {
                    if (ins.operand2.compare("al") == 0) {
                        preg2 = pal;
                    } else if (ins.operand2.compare("ah") == 0) {
                        preg2 = pah;
                    } else if (ins.operand2.compare("bl") == 0) {
                        preg2 = pbl;
                    } else if (ins.operand2.compare("bh") == 0) {
                        preg2 = pbh;
                    } else if (ins.operand2.compare("cl") == 0) {
                        preg2 = pcl;
                    } else if (ins.operand2.compare("ch") == 0) {
                        preg2 = pch;
                    } else if (ins.operand2.compare("dl") == 0) {
                        preg2 = pbl;
                    } else if (ins.operand2.compare("dh") == 0) {
                        preg2 = pdh;
                    }
                    and_bregister_bregister(preg, preg2);
                    continue;
                }
                    //second op mem
                else if (ins.operand2[0] == 'b' && ins.operand2[1] == '[') {
                    string memLoc = ins.operand2.substr(2, ins.operand2.length() - 1);
                    //hex
                    if (memLoc[0] == '0' || memLoc[memLoc.length() - 1] == 'h') {
                        string hex;
                        if (memLoc[memLoc.length() - 1] == 'h') {
                            hex = memLoc.substr(0, memLoc.length() - 1);
                        } else {
                            hex = memLoc;
                        }
                        if (stoi(hex, 0, 16) >= (2 << 15)) {
                            cout << "Error";
                            return 1;
                        } else {
                            mop2 = stoi(hex, 0, 16);
                        }
                    }
                        //dec
                    else if (memLoc[0] == '1' || memLoc[0] == '2' || memLoc[0] == '3' || memLoc[0] == '4' ||
                             memLoc[0] == '5' || memLoc[0] == '6' ||
                             memLoc[0] == '7' || memLoc[0] == '8' || memLoc[0] == '9' ||
                             memLoc[memLoc.length() - 1] == 'd') {
                        string dec;
                        if (memLoc[memLoc.length() - 1] == 'd') {
                            dec = memLoc.substr(0, memLoc.length() - 1);
                        } else {
                            dec = memLoc;
                        }
                        if (stoi(dec, 0, 10) >= (2 << 15)) {
                            cout << "Error";
                            return 1;
                        } else {
                            mop2 = stoi(dec, 0, 10);
                        }
                    }
                        //reg
                    else if (find(wordRegs.begin(), wordRegs.end(), memLoc) != wordRegs.end()) {
                        if (memLoc.compare("ax") == 0) {
                            mop2 = *pax;
                        } else if (memLoc.compare("bx") == 0) {
                            mop2 = *pbx;
                        } else if (memLoc.compare("cx") == 0) {
                            mop2 = *pcx;
                        } else if (memLoc.compare("dx") == 0) {
                            mop2 = *pdx;
                        } else if (memLoc.compare("di") == 0) {
                            mop2 = *pdi;
                        } else if (memLoc.compare("si") == 0) {
                            mop2 = *psi;
                        } else if (memLoc.compare("sp") == 0) {
                            mop2 = *psp;
                        } else if (memLoc.compare("bp") == 0) {
                            mop2 = *pbp;
                        }
                    }
                    and_register_byte_memory(preg, mop2);
                    continue;
                }
                    //second op variable
                else if (find(variableNames.begin(), variableNames.end(), ins.operand2) != variableNames.end()) {
                    vector<string>::iterator it = find(variableNames.begin(), variableNames.end(), ins.operand2);
                    int index = distance(variableNames.begin(), it);
                    if (!variables[index].isWord) { mop2 = variables[index].mem - 1; }
                    else { mop2 = variables[index].mem - 2; }
                    and_register_byte_memory(preg, mop2);
                }
            }
                //firsrt operand 16 bit reg
            else if (find(wordRegs.begin(), wordRegs.end(), ins.operand1) != wordRegs.end()) {
                unsigned short *preg;
                unsigned short *preg2;
                unsigned short op2;
                if (ins.operand1.compare("ax") == 0) {
                    preg = pax;
                } else if (ins.operand1.compare("bx") == 0) {
                    preg = pbx;
                } else if (ins.operand1.compare("cx") == 0) {
                    preg = pcx;
                } else if (ins.operand1.compare("dx") == 0) {
                    preg = pdx;
                } else if (ins.operand1.compare("di") == 0) {
                    preg = pdi;
                } else if (ins.operand1.compare("si") == 0) {
                    preg = psi;
                } else if (ins.operand1.compare("sp") == 0) {
                    preg = psp;
                } else if (ins.operand1.compare("bp") == 0) {
                    preg = pbp;
                }
                //second operand hex
                if (ins.operand2[0] == '0' || ins.operand2[ins.operand2.length() - 1] == 'h') {
                    if (ins.operand2[ins.operand2.length() - 1] == 'h') {
                        string hex = ins.operand2.substr(0, ins.operand2.length() - 1);
                        op2 = stoi(hex, 0, 16);
                    } else {
                        op2 = stoi(ins.operand2, 0, 16);
                    }
                    and_register_word(preg, op2);
                    continue;
                }
                    //second op dec
                else if (ins.operand2[0] == '1' || ins.operand2[0] == '2' || ins.operand2[0] == '3' ||
                         ins.operand2[0] == '4' || ins.operand2[0] == '5' || ins.operand2[0] == '6' ||
                         ins.operand2[0] == '7' || ins.operand2[0] == '8' || ins.operand2[0] == '9' ||
                         ins.operand2[ins.operand2.length() - 1] == 'd') {
                    if (ins.operand2[ins.operand2.length() - 1] == 'd') {
                        string dec = ins.operand2.substr(0, ins.operand2.length() - 1);
                        op2 = stoi(dec, 0, 10);
                    } else {
                        op2 = stoi(ins.operand2, 0, 10);
                    }
                    and_register_word(preg, op2);
                    continue;
                }
                    //second op reg
                else if (find(wordRegs.begin(), wordRegs.end(), ins.operand2) != wordRegs.end()) {
                    if (ins.operand2.compare("ax") == 0) {
                        preg2 = pax;
                    } else if (ins.operand2.compare("bx") == 0) {
                        preg2 = pbx;
                    } else if (ins.operand2.compare("cx") == 0) {
                        preg2 = pcx;
                    } else if (ins.operand2.compare("dx") == 0) {
                        preg2 = pdx;
                    } else if (ins.operand2.compare("di") == 0) {
                        preg2 = pdi;
                    } else if (ins.operand2.compare("si") == 0) {
                        preg2 = psi;
                    } else if (ins.operand2.compare("sp") == 0) {
                        preg2 = psp;
                    } else if (ins.operand2.compare("bp") == 0) {
                        preg2 = pbp;
                    }
                    and_wregister_wregister(preg, preg2);
                    continue;
                }
                    //second op mem
                else if (ins.operand2[0] == 'w' && ins.operand2[1] == '[') {
                    string memLoc = ins.operand2.substr(2, ins.operand2.length() - 1);
                    //hex
                    if (memLoc[0] == '0' || memLoc[memLoc.length() - 1] == 'h') {
                        string hex;
                        if (memLoc[memLoc.length() - 1] == 'h') {
                            hex = memLoc.substr(0, memLoc.length() - 1);
                        } else {
                            hex = memLoc;
                        }
                        if (stoi(hex, 0, 16) >= (2 << 15)) {
                            cout << "Error";
                            return 1;
                        } else {
                            op2 = stoi(hex, 0, 16);
                        }
                    }
                        //dec
                    else if (memLoc[0] == '1' || memLoc[0] == '2' || memLoc[0] == '3' || memLoc[0] == '4' ||
                             memLoc[0] == '5' || memLoc[0] == '6' ||
                             memLoc[0] == '7' || memLoc[0] == '8' || memLoc[0] == '9' ||
                             memLoc[memLoc.length() - 1] == 'd') {
                        string dec;
                        if (memLoc[memLoc.length() - 1] == 'd') {
                            dec = memLoc.substr(0, memLoc.length() - 1);
                        } else {
                            dec = memLoc;
                        }
                        if (stoi(dec, 0, 10) >= (2 << 15)) {
                            cout << "Error";
                            return 1;
                        } else {
                            op2 = stoi(dec, 0, 10);
                        }
                    }
                        //reg
                    else if (find(wordRegs.begin(), wordRegs.end(), memLoc) != wordRegs.end()) {
                        unsigned short op2;
                        if (memLoc.compare("ax") == 0) {
                            op2 = *pax;
                        } else if (memLoc.compare("bx") == 0) {
                            op2 = *pbx;
                        } else if (memLoc.compare("cx") == 0) {
                            op2 = *pcx;
                        } else if (memLoc.compare("dx") == 0) {
                            op2 = *pdx;
                        } else if (memLoc.compare("di") == 0) {
                            op2 = *pdi;
                        } else if (memLoc.compare("si") == 0) {
                            op2 = *psi;
                        } else if (memLoc.compare("sp") == 0) {
                            op2 = *psp;
                        } else if (memLoc.compare("bp") == 0) {
                            op2 = *pbp;
                        }
                    }
                    and_register_word_memory(preg, op2);
                    continue;
                }
                    //second op variable
                else if (find(variableNames.begin(), variableNames.end(), ins.operand2) != variableNames.end()) {
                    vector<string>::iterator it = find(variableNames.begin(), variableNames.end(), ins.operand2);
                    int index = distance(variableNames.begin(), it);
                    if (!variables[index].isWord) { op2 = variables[index].mem - 1; }
                    else { op2 = variables[index].mem - 2; }
                    and_register_word_memory(preg, op2);
                }
            }
                //first byte operand memory
            else if (ins.operand1[0] == 'b' && ins.operand1[1] == '[') {
                string memLoc = ins.operand1.substr(2, ins.operand1.length() - 1);
                unsigned short memByte;
                unsigned char *preg2;
                unsigned char op2;
                //hex
                if (memLoc[0] == '0' || memLoc[memLoc.length() - 1] == 'h') {
                    string hex;
                    if (memLoc[memLoc.length() - 1] == 'h') {
                        hex = memLoc.substr(0, memLoc.length() - 1);
                    } else {
                        hex = memLoc;
                    }
                    if (stoi(hex, 0, 16) >= (2 << 15)) {
                        cout << "Error";
                        return 1;
                    } else {
                        memByte = stoi(hex, 0, 16);
                    }
                }
                    //dec
                else if (memLoc[0] == '1' || memLoc[0] == '2' || memLoc[0] == '3' || memLoc[0] == '4' ||
                         memLoc[0] == '5' || memLoc[0] == '6' ||
                         memLoc[0] == '7' || memLoc[0] == '8' || memLoc[0] == '9' ||
                         memLoc[memLoc.length() - 1] == 'd') {
                    string dec;
                    if (memLoc[memLoc.length() - 1] == 'd') {
                        dec = memLoc.substr(0, memLoc.length() - 1);
                    } else {
                        dec = memLoc;
                    }
                    if (stoi(dec, 0, 10) >= (2 << 15)) {
                        cout << "Error";
                        return 1;
                    } else {
                        memByte = stoi(dec, 0, 10);
                    }
                }
                    //reg
                else if (find(wordRegs.begin(), wordRegs.end(), memLoc) != wordRegs.end()) {
                    if (memLoc.compare("ax") == 0) {
                        memByte = *pax;
                    } else if (memLoc.compare("bx") == 0) {
                        memByte = *pbx;
                    } else if (memLoc.compare("cx") == 0) {
                        memByte = *pcx;
                    } else if (memLoc.compare("dx") == 0) {
                        memByte = *pdx;
                    } else if (memLoc.compare("di") == 0) {
                        memByte = *pdi;
                    } else if (memLoc.compare("si") == 0) {
                        memByte = *psi;
                    } else if (memLoc.compare("sp") == 0) {
                        memByte = *psp;
                    } else if (memLoc.compare("bp") == 0) {
                        memByte = *pbp;
                    }
                }
                //second operand hex
                if (ins.operand2[0] == '0' || ins.operand2[ins.operand2.length() - 1] == 'h') {//second operand hex
                    if (ins.operand2[ins.operand2.length() - 1] == 'h') {
                        string hex = ins.operand2.substr(0, ins.operand2.length() - 1);
                        op2 = stoi(hex, 0, 16);
                    } else {
                        op2 = stoi(ins.operand2, 0, 16);
                    }
                    and_memory_byte(memByte, op2);
                    continue;
                }
                    //second operand dec
                else if (ins.operand2[0] == '1' || ins.operand2[0] == '2' || ins.operand2[0] == '3' ||
                         ins.operand2[0] == '4' || ins.operand2[0] == '5' || ins.operand2[0] == '6' ||
                         ins.operand2[0] == '7' || ins.operand2[0] == '8' || ins.operand2[0] == '9' ||
                         ins.operand2[ins.operand2.length() - 1] == 'd') {
                    if (ins.operand2[ins.operand2.length() - 1] == 'd') {
                        string dec = ins.operand2.substr(0, ins.operand2.length() - 1);
                        op2 = stoi(dec, 0, 10);
                    } else {
                        op2 = stoi(ins.operand2, 0, 10);
                    }
                    and_memory_byte(memByte, op2);
                    continue;
                }
                    //second operand reg
                else if (find(byteRegs.begin(), byteRegs.end(), ins.operand2) != byteRegs.end()) {
                    if (ins.operand2.compare("al") == 0) {
                        preg2 = pal;
                    } else if (ins.operand2.compare("ah") == 0) {
                        preg2 = pah;
                    } else if (ins.operand2.compare("bl") == 0) {
                        preg2 = pbl;
                    } else if (ins.operand2.compare("bh") == 0) {
                        preg2 = pbh;
                    } else if (ins.operand2.compare("cl") == 0) {
                        preg2 = pcl;
                    } else if (ins.operand2.compare("ch") == 0) {
                        preg2 = pch;
                    } else if (ins.operand2.compare("dl") == 0) {
                        preg2 = pbl;
                    } else if (ins.operand2.compare("dh") == 0) {
                        preg2 = pdh;
                    }
                    and_memory_byte_register(preg2, memByte);
                    continue;
                }
            }
                //first operand word memory
            else if(ins.operand1[0] == 'w' && ins.operand1[1] == '[') {
                string memLoc = ins.operand1.substr(2, ins.operand1.length() - 1);
                unsigned short memByte;
                unsigned char *preg2;
                unsigned short op2;
                //hex
                if (memLoc[0] == '0' || memLoc[memLoc.length() - 1] == 'h') {
                    string hex;
                    if (memLoc[memLoc.length() - 1] == 'h') {
                        hex = memLoc.substr(0, memLoc.length() - 1);
                    } else {
                        hex = memLoc;
                    }
                    if (stoi(hex, 0, 16) >= (2 << 15)) {
                        cout << "Error";
                        return 1;
                    } else {
                        memByte = stoi(hex, 0, 16);
                    }
                }
                    //dec
                else if (memLoc[0] == '1' || memLoc[0] == '2' || memLoc[0] == '3' || memLoc[0] == '4' ||
                         memLoc[0] == '5' || memLoc[0] == '6' ||
                         memLoc[0] == '7' || memLoc[0] == '8' || memLoc[0] == '9' ||
                         memLoc[memLoc.length() - 1] == 'd') {
                    string dec;
                    if (memLoc[memLoc.length() - 1] == 'd') {
                        dec = memLoc.substr(0, memLoc.length() - 1);
                    } else {
                        dec = memLoc;
                    }
                    if (stoi(dec, 0, 10) >= (2 << 15)) {
                        cout << "Error";
                        return 1;
                    } else {
                        memByte = stoi(dec, 0, 10);
                    }
                }
                    //reg
                else if (find(wordRegs.begin(), wordRegs.end(), memLoc) != wordRegs.end()) {
                    if (memLoc.compare("ax") == 0) {
                        memByte = *pax;
                    } else if (memLoc.compare("bx") == 0) {
                        memByte = *pbx;
                    } else if (memLoc.compare("cx") == 0) {
                        memByte = *pcx;
                    } else if (memLoc.compare("dx") == 0) {
                        memByte = *pdx;
                    } else if (memLoc.compare("di") == 0) {
                        memByte = *pdi;
                    } else if (memLoc.compare("si") == 0) {
                        memByte = *psi;
                    } else if (memLoc.compare("sp") == 0) {
                        memByte = *psp;
                    } else if (memLoc.compare("bp") == 0) {
                        memByte = *pbp;
                    }
                }
                //second operand hex
                if (ins.operand2[0] == '0' || ins.operand2[ins.operand2.length() - 1] == 'h') {
                    if (ins.operand2[ins.operand2.length() - 1] == 'h') {
                        string hex = ins.operand2.substr(0, ins.operand2.length() - 1);
                        op2 = stoi(hex, 0, 16);
                    } else {
                        op2 = stoi(ins.operand2, 0, 16);
                    }
                    and_memory_word(memByte, op2);
                    continue;
                }
                    //second operand dec
                else if (ins.operand2[0] == '1' || ins.operand2[0] == '2' || ins.operand2[0] == '3' ||
                         ins.operand2[0] == '4' || ins.operand2[0] == '5' || ins.operand2[0] == '6' ||
                         ins.operand2[0] == '7' || ins.operand2[0] == '8' || ins.operand2[0] == '9' ||
                         ins.operand2[ins.operand2.length() - 1] == 'd') {
                    if (ins.operand2[ins.operand2.length() - 1] == 'd') {
                        string dec = ins.operand2.substr(0, ins.operand2.length() - 1);
                        op2 = stoi(dec, 0, 10);
                    } else {
                        op2 = stoi(ins.operand2, 0, 10);
                    }
                    and_memory_word(memByte, op2);
                    continue;
                }
                    //second operand reg
                else if (find(byteRegs.begin(), byteRegs.end(), ins.operand2) != byteRegs.end()) {
                    if (ins.operand2.compare("al") == 0) {
                        preg2 = pal;
                    } else if (ins.operand2.compare("ah") == 0) {
                        preg2 = pah;
                    } else if (ins.operand2.compare("bl") == 0) {
                        preg2 = pbl;
                    } else if (ins.operand2.compare("bh") == 0) {
                        preg2 = pbh;
                    } else if (ins.operand2.compare("cl") == 0) {
                        preg2 = pcl;
                    } else if (ins.operand2.compare("ch") == 0) {
                        preg2 = pch;
                    } else if (ins.operand2.compare("dl") == 0) {
                        preg2 = pbl;
                    } else if (ins.operand2.compare("dh") == 0) {
                        preg2 = pdh;
                    }
                    and_memory_word_register(preg2, memByte);
                    continue;
                }
            }
        }
        else if(ins.name.compare("or") == 0) {
            //firsrt operand 8 bit reg
            if (find(byteRegs.begin(), byteRegs.end(), ins.operand1) != byteRegs.end()) {
                unsigned char *preg;
                unsigned char *preg2;
                unsigned char op2;
                unsigned short mop2;
                if (ins.operand1.compare("al") == 0) {
                    preg = pal;
                } else if (ins.operand1.compare("ah") == 0) {
                    preg = pah;
                } else if (ins.operand1.compare("bl") == 0) {
                    preg = pbl;
                } else if (ins.operand1.compare("bh") == 0) {
                    preg = pbh;
                } else if (ins.operand1.compare("cl") == 0) {
                    preg = pcl;
                } else if (ins.operand1.compare("ch") == 0) {
                    preg = pch;
                } else if (ins.operand1.compare("dl") == 0) {
                    preg = pdl;
                } else if (ins.operand1.compare("dh") == 0) {
                    preg = pdh;
                }
                //second op hex
                if (ins.operand2[0] == '0' || ins.operand2[ins.operand2.length() - 1] == 'h') {//second operand hex
                    if (ins.operand2[ins.operand2.length() - 1] == 'h') {
                        string hex = ins.operand2.substr(0, ins.operand2.length() - 1);
                        op2 = stoi(hex, 0, 16);
                    } else {
                        op2 = stoi(ins.operand2, 0, 16);
                    }
                    or_register_byte(preg, op2);
                    continue;
                }
                    //second op dec
                else if (ins.operand2[0] == '1' || ins.operand2[0] == '2' || ins.operand2[0] == '3' ||
                         ins.operand2[0] == '4' || ins.operand2[0] == '5' || ins.operand2[0] == '6' ||
                         ins.operand2[0] == '7' || ins.operand2[0] == '8' || ins.operand2[0] == '9' ||
                         ins.operand2[ins.operand2.length() - 1] == 'd') {
                    if (ins.operand2[ins.operand2.length() - 1] == 'd') {
                        string dec = ins.operand2.substr(0, ins.operand2.length() - 1);
                        op2 = stoi(dec, 0, 10);
                    } else {
                        op2 = stoi(ins.operand2, 0, 10);
                    }
                    or_register_byte(preg, op2);
                    continue;
                }
                    //second op reg
                else if (find(byteRegs.begin(), byteRegs.end(), ins.operand2) != byteRegs.end()) {
                    if (ins.operand2.compare("al") == 0) {
                        preg2 = pal;
                    } else if (ins.operand2.compare("ah") == 0) {
                        preg2 = pah;
                    } else if (ins.operand2.compare("bl") == 0) {
                        preg2 = pbl;
                    } else if (ins.operand2.compare("bh") == 0) {
                        preg2 = pbh;
                    } else if (ins.operand2.compare("cl") == 0) {
                        preg2 = pcl;
                    } else if (ins.operand2.compare("ch") == 0) {
                        preg2 = pch;
                    } else if (ins.operand2.compare("dl") == 0) {
                        preg2 = pbl;
                    } else if (ins.operand2.compare("dh") == 0) {
                        preg2 = pdh;
                    }
                    or_bregister_bregister(preg, preg2);
                    continue;
                }
                    //second op mem
                else if (ins.operand2[0] == 'b' && ins.operand2[1] == '[') {
                    string memLoc = ins.operand2.substr(2, ins.operand2.length() - 1);
                    //hex
                    if (memLoc[0] == '0' || memLoc[memLoc.length() - 1] == 'h') {
                        string hex;
                        if (memLoc[memLoc.length() - 1] == 'h') {
                            hex = memLoc.substr(0, memLoc.length() - 1);
                        } else {
                            hex = memLoc;
                        }
                        if (stoi(hex, 0, 16) >= (2 << 15)) {
                            cout << "Error";
                            return 1;
                        } else {
                            mop2 = stoi(hex, 0, 16);
                        }
                    }
                        //dec
                    else if (memLoc[0] == '1' || memLoc[0] == '2' || memLoc[0] == '3' || memLoc[0] == '4' ||
                             memLoc[0] == '5' || memLoc[0] == '6' ||
                             memLoc[0] == '7' || memLoc[0] == '8' || memLoc[0] == '9' ||
                             memLoc[memLoc.length() - 1] == 'd') {
                        string dec;
                        if (memLoc[memLoc.length() - 1] == 'd') {
                            dec = memLoc.substr(0, memLoc.length() - 1);
                        } else {
                            dec = memLoc;
                        }
                        if (stoi(dec, 0, 10) >= (2 << 15)) {
                            cout << "Error";
                            return 1;
                        } else {
                            mop2 = stoi(dec, 0, 10);
                        }
                    }
                        //reg
                    else if (find(wordRegs.begin(), wordRegs.end(), memLoc) != wordRegs.end()) {
                        if (memLoc.compare("ax") == 0) {
                            mop2 = *pax;
                        } else if (memLoc.compare("bx") == 0) {
                            mop2 = *pbx;
                        } else if (memLoc.compare("cx") == 0) {
                            mop2 = *pcx;
                        } else if (memLoc.compare("dx") == 0) {
                            mop2 = *pdx;
                        } else if (memLoc.compare("di") == 0) {
                            mop2 = *pdi;
                        } else if (memLoc.compare("si") == 0) {
                            mop2 = *psi;
                        } else if (memLoc.compare("sp") == 0) {
                            mop2 = *psp;
                        } else if (memLoc.compare("bp") == 0) {
                            mop2 = *pbp;
                        }
                    }
                    or_register_byte_memory(preg, mop2);
                    continue;
                }
                    //second op variable
                else if (find(variableNames.begin(), variableNames.end(), ins.operand2) != variableNames.end()) {
                    vector<string>::iterator it = find(variableNames.begin(), variableNames.end(), ins.operand2);
                    int index = distance(variableNames.begin(), it);
                    if (!variables[index].isWord) { mop2 = variables[index].mem - 1; }
                    else { mop2 = variables[index].mem - 2; }
                    or_register_byte_memory(preg, mop2);
                }
            }
                //firsrt operand 16 bit reg
            else if (find(wordRegs.begin(), wordRegs.end(), ins.operand1) != wordRegs.end()) {
                unsigned short *preg;
                unsigned short *preg2;
                unsigned short op2;
                if (ins.operand1.compare("ax") == 0) {
                    preg = pax;
                } else if (ins.operand1.compare("bx") == 0) {
                    preg = pbx;
                } else if (ins.operand1.compare("cx") == 0) {
                    preg = pcx;
                } else if (ins.operand1.compare("dx") == 0) {
                    preg = pdx;
                } else if (ins.operand1.compare("di") == 0) {
                    preg = pdi;
                } else if (ins.operand1.compare("si") == 0) {
                    preg = psi;
                } else if (ins.operand1.compare("sp") == 0) {
                    preg = psp;
                } else if (ins.operand1.compare("bp") == 0) {
                    preg = pbp;
                }
                //second operand hex
                if (ins.operand2[0] == '0' || ins.operand2[ins.operand2.length() - 1] == 'h') {
                    if (ins.operand2[ins.operand2.length() - 1] == 'h') {
                        string hex = ins.operand2.substr(0, ins.operand2.length() - 1);
                        op2 = stoi(hex, 0, 16);
                    } else {
                        op2 = stoi(ins.operand2, 0, 16);
                    }
                    or_register_word(preg, op2);
                    continue;
                }
                    //second op dec
                else if (ins.operand2[0] == '1' || ins.operand2[0] == '2' || ins.operand2[0] == '3' ||
                         ins.operand2[0] == '4' || ins.operand2[0] == '5' || ins.operand2[0] == '6' ||
                         ins.operand2[0] == '7' || ins.operand2[0] == '8' || ins.operand2[0] == '9' ||
                         ins.operand2[ins.operand2.length() - 1] == 'd') {
                    if (ins.operand2[ins.operand2.length() - 1] == 'd') {
                        string dec = ins.operand2.substr(0, ins.operand2.length() - 1);
                        op2 = stoi(dec, 0, 10);
                    } else {
                        op2 = stoi(ins.operand2, 0, 10);
                    }
                    or_register_word(preg, op2);
                    continue;
                }
                    //second op reg
                else if (find(wordRegs.begin(), wordRegs.end(), ins.operand2) != wordRegs.end()) {
                    if (ins.operand2.compare("ax") == 0) {
                        preg2 = pax;
                    } else if (ins.operand2.compare("bx") == 0) {
                        preg2 = pbx;
                    } else if (ins.operand2.compare("cx") == 0) {
                        preg2 = pcx;
                    } else if (ins.operand2.compare("dx") == 0) {
                        preg2 = pdx;
                    } else if (ins.operand2.compare("di") == 0) {
                        preg2 = pdi;
                    } else if (ins.operand2.compare("si") == 0) {
                        preg2 = psi;
                    } else if (ins.operand2.compare("sp") == 0) {
                        preg2 = psp;
                    } else if (ins.operand2.compare("bp") == 0) {
                        preg2 = pbp;
                    }
                    or_wregister_wregister(preg, preg2);
                    continue;
                }
                    //second op mem
                else if (ins.operand2[0] == 'w' && ins.operand2[1] == '[') {
                    string memLoc = ins.operand2.substr(2, ins.operand2.length() - 1);
                    //hex
                    if (memLoc[0] == '0' || memLoc[memLoc.length() - 1] == 'h') {
                        string hex;
                        if (memLoc[memLoc.length() - 1] == 'h') {
                            hex = memLoc.substr(0, memLoc.length() - 1);
                        } else {
                            hex = memLoc;
                        }
                        if (stoi(hex, 0, 16) >= (2 << 15)) {
                            cout << "Error";
                            return 1;
                        } else {
                            op2 = stoi(hex, 0, 16);
                        }
                    }
                        //dec
                    else if (memLoc[0] == '1' || memLoc[0] == '2' || memLoc[0] == '3' || memLoc[0] == '4' ||
                             memLoc[0] == '5' || memLoc[0] == '6' ||
                             memLoc[0] == '7' || memLoc[0] == '8' || memLoc[0] == '9' ||
                             memLoc[memLoc.length() - 1] == 'd') {
                        string dec;
                        if (memLoc[memLoc.length() - 1] == 'd') {
                            dec = memLoc.substr(0, memLoc.length() - 1);
                        } else {
                            dec = memLoc;
                        }
                        if (stoi(dec, 0, 10) >= (2 << 15)) {
                            cout << "Error";
                            return 1;
                        } else {
                            op2 = stoi(dec, 0, 10);
                        }
                    }
                        //reg
                    else if (find(wordRegs.begin(), wordRegs.end(), memLoc) != wordRegs.end()) {
                        unsigned short op2;
                        if (memLoc.compare("ax") == 0) {
                            op2 = *pax;
                        } else if (memLoc.compare("bx") == 0) {
                            op2 = *pbx;
                        } else if (memLoc.compare("cx") == 0) {
                            op2 = *pcx;
                        } else if (memLoc.compare("dx") == 0) {
                            op2 = *pdx;
                        } else if (memLoc.compare("di") == 0) {
                            op2 = *pdi;
                        } else if (memLoc.compare("si") == 0) {
                            op2 = *psi;
                        } else if (memLoc.compare("sp") == 0) {
                            op2 = *psp;
                        } else if (memLoc.compare("bp") == 0) {
                            op2 = *pbp;
                        }
                    }
                    or_register_word_memory(preg, op2);
                    continue;
                }
                    //second op variable
                else if (find(variableNames.begin(), variableNames.end(), ins.operand2) != variableNames.end()) {
                    vector<string>::iterator it = find(variableNames.begin(), variableNames.end(), ins.operand2);
                    int index = distance(variableNames.begin(), it);
                    if (!variables[index].isWord) { op2 = variables[index].mem - 1; }
                    else { op2 = variables[index].mem - 2; }
                    or_register_word_memory(preg, op2);
                }
            }
                //first byte operand memory
            else if (ins.operand1[0] == 'b' && ins.operand1[1] == '[') {
                string memLoc = ins.operand1.substr(2, ins.operand1.length() - 1);
                unsigned short memByte;
                unsigned char *preg2;
                unsigned char op2;
                //hex
                if (memLoc[0] == '0' || memLoc[memLoc.length() - 1] == 'h') {
                    string hex;
                    if (memLoc[memLoc.length() - 1] == 'h') {
                        hex = memLoc.substr(0, memLoc.length() - 1);
                    } else {
                        hex = memLoc;
                    }
                    if (stoi(hex, 0, 16) >= (2 << 15)) {
                        cout << "Error";
                        return 1;
                    } else {
                        memByte = stoi(hex, 0, 16);
                    }
                }
                    //dec
                else if (memLoc[0] == '1' || memLoc[0] == '2' || memLoc[0] == '3' || memLoc[0] == '4' ||
                         memLoc[0] == '5' || memLoc[0] == '6' ||
                         memLoc[0] == '7' || memLoc[0] == '8' || memLoc[0] == '9' ||
                         memLoc[memLoc.length() - 1] == 'd') {
                    string dec;
                    if (memLoc[memLoc.length() - 1] == 'd') {
                        dec = memLoc.substr(0, memLoc.length() - 1);
                    } else {
                        dec = memLoc;
                    }
                    if (stoi(dec, 0, 10) >= (2 << 15)) {
                        cout << "Error";
                        return 1;
                    } else {
                        memByte = stoi(dec, 0, 10);
                    }
                }
                    //reg
                else if (find(wordRegs.begin(), wordRegs.end(), memLoc) != wordRegs.end()) {
                    if (memLoc.compare("ax") == 0) {
                        memByte = *pax;
                    } else if (memLoc.compare("bx") == 0) {
                        memByte = *pbx;
                    } else if (memLoc.compare("cx") == 0) {
                        memByte = *pcx;
                    } else if (memLoc.compare("dx") == 0) {
                        memByte = *pdx;
                    } else if (memLoc.compare("di") == 0) {
                        memByte = *pdi;
                    } else if (memLoc.compare("si") == 0) {
                        memByte = *psi;
                    } else if (memLoc.compare("sp") == 0) {
                        memByte = *psp;
                    } else if (memLoc.compare("bp") == 0) {
                        memByte = *pbp;
                    }
                }
                //second operand hex
                if (ins.operand2[0] == '0' || ins.operand2[ins.operand2.length() - 1] == 'h') {//second operand hex
                    if (ins.operand2[ins.operand2.length() - 1] == 'h') {
                        string hex = ins.operand2.substr(0, ins.operand2.length() - 1);
                        op2 = stoi(hex, 0, 16);
                    } else {
                        op2 = stoi(ins.operand2, 0, 16);
                    }
                    or_memory_byte(memByte, op2);
                    continue;
                }
                    //second operand dec
                else if (ins.operand2[0] == '1' || ins.operand2[0] == '2' || ins.operand2[0] == '3' ||
                         ins.operand2[0] == '4' || ins.operand2[0] == '5' || ins.operand2[0] == '6' ||
                         ins.operand2[0] == '7' || ins.operand2[0] == '8' || ins.operand2[0] == '9' ||
                         ins.operand2[ins.operand2.length() - 1] == 'd') {
                    if (ins.operand2[ins.operand2.length() - 1] == 'd') {
                        string dec = ins.operand2.substr(0, ins.operand2.length() - 1);
                        op2 = stoi(dec, 0, 10);
                    } else {
                        op2 = stoi(ins.operand2, 0, 10);
                    }
                    or_memory_byte(memByte, op2);
                    continue;
                }
                    //second operand reg
                else if (find(byteRegs.begin(), byteRegs.end(), ins.operand2) != byteRegs.end()) {
                    if (ins.operand2.compare("al") == 0) {
                        preg2 = pal;
                    } else if (ins.operand2.compare("ah") == 0) {
                        preg2 = pah;
                    } else if (ins.operand2.compare("bl") == 0) {
                        preg2 = pbl;
                    } else if (ins.operand2.compare("bh") == 0) {
                        preg2 = pbh;
                    } else if (ins.operand2.compare("cl") == 0) {
                        preg2 = pcl;
                    } else if (ins.operand2.compare("ch") == 0) {
                        preg2 = pch;
                    } else if (ins.operand2.compare("dl") == 0) {
                        preg2 = pbl;
                    } else if (ins.operand2.compare("dh") == 0) {
                        preg2 = pdh;
                    }
                    or_memory_byte_register(preg2, memByte);
                    continue;
                }
            }
                //first operand word memory
            else if(ins.operand1[0] == 'w' && ins.operand1[1] == '[') {
                string memLoc = ins.operand1.substr(2, ins.operand1.length() - 1);
                unsigned short memByte;
                unsigned char *preg2;
                unsigned short op2;
                //hex
                if (memLoc[0] == '0' || memLoc[memLoc.length() - 1] == 'h') {
                    string hex;
                    if (memLoc[memLoc.length() - 1] == 'h') {
                        hex = memLoc.substr(0, memLoc.length() - 1);
                    } else {
                        hex = memLoc;
                    }
                    if (stoi(hex, 0, 16) >= (2 << 15)) {
                        cout << "Error";
                        return 1;
                    } else {
                        memByte = stoi(hex, 0, 16);
                    }
                }
                    //dec
                else if (memLoc[0] == '1' || memLoc[0] == '2' || memLoc[0] == '3' || memLoc[0] == '4' ||
                         memLoc[0] == '5' || memLoc[0] == '6' ||
                         memLoc[0] == '7' || memLoc[0] == '8' || memLoc[0] == '9' ||
                         memLoc[memLoc.length() - 1] == 'd') {
                    string dec;
                    if (memLoc[memLoc.length() - 1] == 'd') {
                        dec = memLoc.substr(0, memLoc.length() - 1);
                    } else {
                        dec = memLoc;
                    }
                    if (stoi(dec, 0, 10) >= (2 << 15)) {
                        cout << "Error";
                        return 1;
                    } else {
                        memByte = stoi(dec, 0, 10);
                    }
                }
                    //reg
                else if (find(wordRegs.begin(), wordRegs.end(), memLoc) != wordRegs.end()) {
                    if (memLoc.compare("ax") == 0) {
                        memByte = *pax;
                    } else if (memLoc.compare("bx") == 0) {
                        memByte = *pbx;
                    } else if (memLoc.compare("cx") == 0) {
                        memByte = *pcx;
                    } else if (memLoc.compare("dx") == 0) {
                        memByte = *pdx;
                    } else if (memLoc.compare("di") == 0) {
                        memByte = *pdi;
                    } else if (memLoc.compare("si") == 0) {
                        memByte = *psi;
                    } else if (memLoc.compare("sp") == 0) {
                        memByte = *psp;
                    } else if (memLoc.compare("bp") == 0) {
                        memByte = *pbp;
                    }
                }
                //second operand hex
                if (ins.operand2[0] == '0' || ins.operand2[ins.operand2.length() - 1] == 'h') {
                    if (ins.operand2[ins.operand2.length() - 1] == 'h') {
                        string hex = ins.operand2.substr(0, ins.operand2.length() - 1);
                        op2 = stoi(hex, 0, 16);
                    } else {
                        op2 = stoi(ins.operand2, 0, 16);
                    }
                    or_memory_word(memByte, op2);
                    continue;
                }
                    //second operand dec
                else if (ins.operand2[0] == '1' || ins.operand2[0] == '2' || ins.operand2[0] == '3' ||
                         ins.operand2[0] == '4' || ins.operand2[0] == '5' || ins.operand2[0] == '6' ||
                         ins.operand2[0] == '7' || ins.operand2[0] == '8' || ins.operand2[0] == '9' ||
                         ins.operand2[ins.operand2.length() - 1] == 'd') {
                    if (ins.operand2[ins.operand2.length() - 1] == 'd') {
                        string dec = ins.operand2.substr(0, ins.operand2.length() - 1);
                        op2 = stoi(dec, 0, 10);
                    } else {
                        op2 = stoi(ins.operand2, 0, 10);
                    }
                    or_memory_word(memByte, op2);
                    continue;
                }
                    //second operand reg
                else if (find(byteRegs.begin(), byteRegs.end(), ins.operand2) != byteRegs.end()) {
                    if (ins.operand2.compare("al") == 0) {
                        preg2 = pal;
                    } else if (ins.operand2.compare("ah") == 0) {
                        preg2 = pah;
                    } else if (ins.operand2.compare("bl") == 0) {
                        preg2 = pbl;
                    } else if (ins.operand2.compare("bh") == 0) {
                        preg2 = pbh;
                    } else if (ins.operand2.compare("cl") == 0) {
                        preg2 = pcl;
                    } else if (ins.operand2.compare("ch") == 0) {
                        preg2 = pch;
                    } else if (ins.operand2.compare("dl") == 0) {
                        preg2 = pbl;
                    } else if (ins.operand2.compare("dh") == 0) {
                        preg2 = pdh;
                    }
                    or_memory_word_register(preg2, memByte);
                    continue;
                }
            }
        }
        else if(ins.name.compare("xor") == 0) {
            //firsrt operand 8 bit reg
            if (find(byteRegs.begin(), byteRegs.end(), ins.operand1) != byteRegs.end()) {
                unsigned char *preg;
                unsigned char *preg2;
                unsigned char op2;
                unsigned short mop2;
                if (ins.operand1.compare("al") == 0) {
                    preg = pal;
                } else if (ins.operand1.compare("ah") == 0) {
                    preg = pah;
                } else if (ins.operand1.compare("bl") == 0) {
                    preg = pbl;
                } else if (ins.operand1.compare("bh") == 0) {
                    preg = pbh;
                } else if (ins.operand1.compare("cl") == 0) {
                    preg = pcl;
                } else if (ins.operand1.compare("ch") == 0) {
                    preg = pch;
                } else if (ins.operand1.compare("dl") == 0) {
                    preg = pdl;
                } else if (ins.operand1.compare("dh") == 0) {
                    preg = pdh;
                }
                //second op hex
                if (ins.operand2[0] == '0' || ins.operand2[ins.operand2.length() - 1] == 'h') {//second operand hex
                    if (ins.operand2[ins.operand2.length() - 1] == 'h') {
                        string hex = ins.operand2.substr(0, ins.operand2.length() - 1);
                        op2 = stoi(hex, 0, 16);
                    } else {
                        op2 = stoi(ins.operand2, 0, 16);
                    }
                    xor_register_byte(preg, op2);
                    continue;
                }
                    //second op dec
                else if (ins.operand2[0] == '1' || ins.operand2[0] == '2' || ins.operand2[0] == '3' ||
                         ins.operand2[0] == '4' || ins.operand2[0] == '5' || ins.operand2[0] == '6' ||
                         ins.operand2[0] == '7' || ins.operand2[0] == '8' || ins.operand2[0] == '9' ||
                         ins.operand2[ins.operand2.length() - 1] == 'd') {
                    if (ins.operand2[ins.operand2.length() - 1] == 'd') {
                        string dec = ins.operand2.substr(0, ins.operand2.length() - 1);
                        op2 = stoi(dec, 0, 10);
                    } else {
                        op2 = stoi(ins.operand2, 0, 10);
                    }
                    xor_register_byte(preg, op2);
                    continue;
                }
                    //second op reg
                else if (find(byteRegs.begin(), byteRegs.end(), ins.operand2) != byteRegs.end()) {
                    if (ins.operand2.compare("al") == 0) {
                        preg2 = pal;
                    } else if (ins.operand2.compare("ah") == 0) {
                        preg2 = pah;
                    } else if (ins.operand2.compare("bl") == 0) {
                        preg2 = pbl;
                    } else if (ins.operand2.compare("bh") == 0) {
                        preg2 = pbh;
                    } else if (ins.operand2.compare("cl") == 0) {
                        preg2 = pcl;
                    } else if (ins.operand2.compare("ch") == 0) {
                        preg2 = pch;
                    } else if (ins.operand2.compare("dl") == 0) {
                        preg2 = pbl;
                    } else if (ins.operand2.compare("dh") == 0) {
                        preg2 = pdh;
                    }
                    xor_bregister_bregister(preg, preg2);
                    continue;
                }
                    //second op mem
                else if (ins.operand2[0] == 'b' && ins.operand2[1] == '[') {
                    string memLoc = ins.operand2.substr(2, ins.operand2.length() - 1);
                    //hex
                    if (memLoc[0] == '0' || memLoc[memLoc.length() - 1] == 'h') {
                        string hex;
                        if (memLoc[memLoc.length() - 1] == 'h') {
                            hex = memLoc.substr(0, memLoc.length() - 1);
                        } else {
                            hex = memLoc;
                        }
                        if (stoi(hex, 0, 16) >= (2 << 15)) {
                            cout << "Error";
                            return 1;
                        } else {
                            mop2 = stoi(hex, 0, 16);
                        }
                    }
                        //dec
                    else if (memLoc[0] == '1' || memLoc[0] == '2' || memLoc[0] == '3' || memLoc[0] == '4' ||
                             memLoc[0] == '5' || memLoc[0] == '6' ||
                             memLoc[0] == '7' || memLoc[0] == '8' || memLoc[0] == '9' ||
                             memLoc[memLoc.length() - 1] == 'd') {
                        string dec;
                        if (memLoc[memLoc.length() - 1] == 'd') {
                            dec = memLoc.substr(0, memLoc.length() - 1);
                        } else {
                            dec = memLoc;
                        }
                        if (stoi(dec, 0, 10) >= (2 << 15)) {
                            cout << "Error";
                            return 1;
                        } else {
                            mop2 = stoi(dec, 0, 10);
                        }
                    }
                        //reg
                    else if (find(wordRegs.begin(), wordRegs.end(), memLoc) != wordRegs.end()) {
                        if (memLoc.compare("ax") == 0) {
                            mop2 = *pax;
                        } else if (memLoc.compare("bx") == 0) {
                            mop2 = *pbx;
                        } else if (memLoc.compare("cx") == 0) {
                            mop2 = *pcx;
                        } else if (memLoc.compare("dx") == 0) {
                            mop2 = *pdx;
                        } else if (memLoc.compare("di") == 0) {
                            mop2 = *pdi;
                        } else if (memLoc.compare("si") == 0) {
                            mop2 = *psi;
                        } else if (memLoc.compare("sp") == 0) {
                            mop2 = *psp;
                        } else if (memLoc.compare("bp") == 0) {
                            mop2 = *pbp;
                        }
                    }
                    xor_register_byte_memory(preg, mop2);
                    continue;
                }
                    //second op variable
                else if (find(variableNames.begin(), variableNames.end(), ins.operand2) != variableNames.end()) {
                    vector<string>::iterator it = find(variableNames.begin(), variableNames.end(), ins.operand2);
                    int index = distance(variableNames.begin(), it);
                    if (!variables[index].isWord) { mop2 = variables[index].mem - 1; }
                    else { mop2 = variables[index].mem - 2; }
                    xor_register_byte_memory(preg, mop2);
                }
            }
                //firsrt operand 16 bit reg
            else if (find(wordRegs.begin(), wordRegs.end(), ins.operand1) != wordRegs.end()) {
                unsigned short *preg;
                unsigned short *preg2;
                unsigned short op2;
                if (ins.operand1.compare("ax") == 0) {
                    preg = pax;
                } else if (ins.operand1.compare("bx") == 0) {
                    preg = pbx;
                } else if (ins.operand1.compare("cx") == 0) {
                    preg = pcx;
                } else if (ins.operand1.compare("dx") == 0) {
                    preg = pdx;
                } else if (ins.operand1.compare("di") == 0) {
                    preg = pdi;
                } else if (ins.operand1.compare("si") == 0) {
                    preg = psi;
                } else if (ins.operand1.compare("sp") == 0) {
                    preg = psp;
                } else if (ins.operand1.compare("bp") == 0) {
                    preg = pbp;
                }
                //second operand hex
                if (ins.operand2[0] == '0' || ins.operand2[ins.operand2.length() - 1] == 'h') {
                    if (ins.operand2[ins.operand2.length() - 1] == 'h') {
                        string hex = ins.operand2.substr(0, ins.operand2.length() - 1);
                        op2 = stoi(hex, 0, 16);
                    } else {
                        op2 = stoi(ins.operand2, 0, 16);
                    }
                    xor_register_word(preg, op2);
                    continue;
                }
                    //second op dec
                else if (ins.operand2[0] == '1' || ins.operand2[0] == '2' || ins.operand2[0] == '3' ||
                         ins.operand2[0] == '4' || ins.operand2[0] == '5' || ins.operand2[0] == '6' ||
                         ins.operand2[0] == '7' || ins.operand2[0] == '8' || ins.operand2[0] == '9' ||
                         ins.operand2[ins.operand2.length() - 1] == 'd') {
                    if (ins.operand2[ins.operand2.length() - 1] == 'd') {
                        string dec = ins.operand2.substr(0, ins.operand2.length() - 1);
                        op2 = stoi(dec, 0, 10);
                    } else {
                        op2 = stoi(ins.operand2, 0, 10);
                    }
                    xor_register_word(preg, op2);
                    continue;
                }
                    //second op reg
                else if (find(wordRegs.begin(), wordRegs.end(), ins.operand2) != wordRegs.end()) {
                    if (ins.operand2.compare("ax") == 0) {
                        preg2 = pax;
                    } else if (ins.operand2.compare("bx") == 0) {
                        preg2 = pbx;
                    } else if (ins.operand2.compare("cx") == 0) {
                        preg2 = pcx;
                    } else if (ins.operand2.compare("dx") == 0) {
                        preg2 = pdx;
                    } else if (ins.operand2.compare("di") == 0) {
                        preg2 = pdi;
                    } else if (ins.operand2.compare("si") == 0) {
                        preg2 = psi;
                    } else if (ins.operand2.compare("sp") == 0) {
                        preg2 = psp;
                    } else if (ins.operand2.compare("bp") == 0) {
                        preg2 = pbp;
                    }
                    xor_wregister_wregister(preg, preg2);
                    continue;
                }
                    //second op mem
                else if (ins.operand2[0] == 'w' && ins.operand2[1] == '[') {
                    string memLoc = ins.operand2.substr(2, ins.operand2.length() - 1);
                    //hex
                    if (memLoc[0] == '0' || memLoc[memLoc.length() - 1] == 'h') {
                        string hex;
                        if (memLoc[memLoc.length() - 1] == 'h') {
                            hex = memLoc.substr(0, memLoc.length() - 1);
                        } else {
                            hex = memLoc;
                        }
                        if (stoi(hex, 0, 16) >= (2 << 15)) {
                            cout << "Error";
                            return 1;
                        } else {
                            op2 = stoi(hex, 0, 16);
                        }
                    }
                        //dec
                    else if (memLoc[0] == '1' || memLoc[0] == '2' || memLoc[0] == '3' || memLoc[0] == '4' ||
                             memLoc[0] == '5' || memLoc[0] == '6' ||
                             memLoc[0] == '7' || memLoc[0] == '8' || memLoc[0] == '9' ||
                             memLoc[memLoc.length() - 1] == 'd') {
                        string dec;
                        if (memLoc[memLoc.length() - 1] == 'd') {
                            dec = memLoc.substr(0, memLoc.length() - 1);
                        } else {
                            dec = memLoc;
                        }
                        if (stoi(dec, 0, 10) >= (2 << 15)) {
                            cout << "Error";
                            return 1;
                        } else {
                            op2 = stoi(dec, 0, 10);
                        }
                    }
                        //reg
                    else if (find(wordRegs.begin(), wordRegs.end(), memLoc) != wordRegs.end()) {
                        unsigned short op2;
                        if (memLoc.compare("ax") == 0) {
                            op2 = *pax;
                        } else if (memLoc.compare("bx") == 0) {
                            op2 = *pbx;
                        } else if (memLoc.compare("cx") == 0) {
                            op2 = *pcx;
                        } else if (memLoc.compare("dx") == 0) {
                            op2 = *pdx;
                        } else if (memLoc.compare("di") == 0) {
                            op2 = *pdi;
                        } else if (memLoc.compare("si") == 0) {
                            op2 = *psi;
                        } else if (memLoc.compare("sp") == 0) {
                            op2 = *psp;
                        } else if (memLoc.compare("bp") == 0) {
                            op2 = *pbp;
                        }
                    }
                    xor_register_word_memory(preg, op2);
                    continue;
                }
                    //second op variable
                else if (find(variableNames.begin(), variableNames.end(), ins.operand2) != variableNames.end()) {
                    vector<string>::iterator it = find(variableNames.begin(), variableNames.end(), ins.operand2);
                    int index = distance(variableNames.begin(), it);
                    if (!variables[index].isWord) { op2 = variables[index].mem - 1; }
                    else { op2 = variables[index].mem - 2; }
                    xor_register_word_memory(preg, op2);
                }
            }
                //first byte operand memory
            else if (ins.operand1[0] == 'b' && ins.operand1[1] == '[') {
                string memLoc = ins.operand1.substr(2, ins.operand1.length() - 1);
                unsigned short memByte;
                unsigned char *preg2;
                unsigned char op2;
                //hex
                if (memLoc[0] == '0' || memLoc[memLoc.length() - 1] == 'h') {
                    string hex;
                    if (memLoc[memLoc.length() - 1] == 'h') {
                        hex = memLoc.substr(0, memLoc.length() - 1);
                    } else {
                        hex = memLoc;
                    }
                    if (stoi(hex, 0, 16) >= (2 << 15)) {
                        cout << "Error";
                        return 1;
                    } else {
                        memByte = stoi(hex, 0, 16);
                    }
                }
                    //dec
                else if (memLoc[0] == '1' || memLoc[0] == '2' || memLoc[0] == '3' || memLoc[0] == '4' ||
                         memLoc[0] == '5' || memLoc[0] == '6' ||
                         memLoc[0] == '7' || memLoc[0] == '8' || memLoc[0] == '9' ||
                         memLoc[memLoc.length() - 1] == 'd') {
                    string dec;
                    if (memLoc[memLoc.length() - 1] == 'd') {
                        dec = memLoc.substr(0, memLoc.length() - 1);
                    } else {
                        dec = memLoc;
                    }
                    if (stoi(dec, 0, 10) >= (2 << 15)) {
                        cout << "Error";
                        return 1;
                    } else {
                        memByte = stoi(dec, 0, 10);
                    }
                }
                    //reg
                else if (find(wordRegs.begin(), wordRegs.end(), memLoc) != wordRegs.end()) {
                    if (memLoc.compare("ax") == 0) {
                        memByte = *pax;
                    } else if (memLoc.compare("bx") == 0) {
                        memByte = *pbx;
                    } else if (memLoc.compare("cx") == 0) {
                        memByte = *pcx;
                    } else if (memLoc.compare("dx") == 0) {
                        memByte = *pdx;
                    } else if (memLoc.compare("di") == 0) {
                        memByte = *pdi;
                    } else if (memLoc.compare("si") == 0) {
                        memByte = *psi;
                    } else if (memLoc.compare("sp") == 0) {
                        memByte = *psp;
                    } else if (memLoc.compare("bp") == 0) {
                        memByte = *pbp;
                    }
                }
                //second operand hex
                if (ins.operand2[0] == '0' || ins.operand2[ins.operand2.length() - 1] == 'h') {//second operand hex
                    if (ins.operand2[ins.operand2.length() - 1] == 'h') {
                        string hex = ins.operand2.substr(0, ins.operand2.length() - 1);
                        op2 = stoi(hex, 0, 16);
                    } else {
                        op2 = stoi(ins.operand2, 0, 16);
                    }
                    xor_memory_byte(memByte, op2);
                    continue;
                }
                    //second operand dec
                else if (ins.operand2[0] == '1' || ins.operand2[0] == '2' || ins.operand2[0] == '3' ||
                         ins.operand2[0] == '4' || ins.operand2[0] == '5' || ins.operand2[0] == '6' ||
                         ins.operand2[0] == '7' || ins.operand2[0] == '8' || ins.operand2[0] == '9' ||
                         ins.operand2[ins.operand2.length() - 1] == 'd') {
                    if (ins.operand2[ins.operand2.length() - 1] == 'd') {
                        string dec = ins.operand2.substr(0, ins.operand2.length() - 1);
                        op2 = stoi(dec, 0, 10);
                    } else {
                        op2 = stoi(ins.operand2, 0, 10);
                    }
                    xor_memory_byte(memByte, op2);
                    continue;
                }
                    //second operand reg
                else if (find(byteRegs.begin(), byteRegs.end(), ins.operand2) != byteRegs.end()) {
                    if (ins.operand2.compare("al") == 0) {
                        preg2 = pal;
                    } else if (ins.operand2.compare("ah") == 0) {
                        preg2 = pah;
                    } else if (ins.operand2.compare("bl") == 0) {
                        preg2 = pbl;
                    } else if (ins.operand2.compare("bh") == 0) {
                        preg2 = pbh;
                    } else if (ins.operand2.compare("cl") == 0) {
                        preg2 = pcl;
                    } else if (ins.operand2.compare("ch") == 0) {
                        preg2 = pch;
                    } else if (ins.operand2.compare("dl") == 0) {
                        preg2 = pbl;
                    } else if (ins.operand2.compare("dh") == 0) {
                        preg2 = pdh;
                    }
                    xor_memory_byte_register(preg2, memByte);
                    continue;
                }
            }
                //first operand word memory
            else if(ins.operand1[0] == 'w' && ins.operand1[1] == '[') {
                string memLoc = ins.operand1.substr(2, ins.operand1.length() - 1);
                unsigned short memByte;
                unsigned char *preg2;
                unsigned short op2;
                //hex
                if (memLoc[0] == '0' || memLoc[memLoc.length() - 1] == 'h') {
                    string hex;
                    if (memLoc[memLoc.length() - 1] == 'h') {
                        hex = memLoc.substr(0, memLoc.length() - 1);
                    } else {
                        hex = memLoc;
                    }
                    if (stoi(hex, 0, 16) >= (2 << 15)) {
                        cout << "Error";
                        return 1;
                    } else {
                        memByte = stoi(hex, 0, 16);
                    }
                }
                    //dec
                else if (memLoc[0] == '1' || memLoc[0] == '2' || memLoc[0] == '3' || memLoc[0] == '4' ||
                         memLoc[0] == '5' || memLoc[0] == '6' ||
                         memLoc[0] == '7' || memLoc[0] == '8' || memLoc[0] == '9' ||
                         memLoc[memLoc.length() - 1] == 'd') {
                    string dec;
                    if (memLoc[memLoc.length() - 1] == 'd') {
                        dec = memLoc.substr(0, memLoc.length() - 1);
                    } else {
                        dec = memLoc;
                    }
                    if (stoi(dec, 0, 10) >= (2 << 15)) {
                        cout << "Error";
                        return 1;
                    } else {
                        memByte = stoi(dec, 0, 10);
                    }
                }
                    //reg
                else if (find(wordRegs.begin(), wordRegs.end(), memLoc) != wordRegs.end()) {
                    if (memLoc.compare("ax") == 0) {
                        memByte = *pax;
                    } else if (memLoc.compare("bx") == 0) {
                        memByte = *pbx;
                    } else if (memLoc.compare("cx") == 0) {
                        memByte = *pcx;
                    } else if (memLoc.compare("dx") == 0) {
                        memByte = *pdx;
                    } else if (memLoc.compare("di") == 0) {
                        memByte = *pdi;
                    } else if (memLoc.compare("si") == 0) {
                        memByte = *psi;
                    } else if (memLoc.compare("sp") == 0) {
                        memByte = *psp;
                    } else if (memLoc.compare("bp") == 0) {
                        memByte = *pbp;
                    }
                }
                //second operand hex
                if (ins.operand2[0] == '0' || ins.operand2[ins.operand2.length() - 1] == 'h') {
                    if (ins.operand2[ins.operand2.length() - 1] == 'h') {
                        string hex = ins.operand2.substr(0, ins.operand2.length() - 1);
                        op2 = stoi(hex, 0, 16);
                    } else {
                        op2 = stoi(ins.operand2, 0, 16);
                    }
                    xor_memory_word(memByte, op2);
                    continue;
                }
                    //second operand dec
                else if (ins.operand2[0] == '1' || ins.operand2[0] == '2' || ins.operand2[0] == '3' ||
                         ins.operand2[0] == '4' || ins.operand2[0] == '5' || ins.operand2[0] == '6' ||
                         ins.operand2[0] == '7' || ins.operand2[0] == '8' || ins.operand2[0] == '9' ||
                         ins.operand2[ins.operand2.length() - 1] == 'd') {
                    if (ins.operand2[ins.operand2.length() - 1] == 'd') {
                        string dec = ins.operand2.substr(0, ins.operand2.length() - 1);
                        op2 = stoi(dec, 0, 10);
                    } else {
                        op2 = stoi(ins.operand2, 0, 10);
                    }
                    xor_memory_word(memByte, op2);
                    continue;
                }
                    //second operand reg
                else if (find(byteRegs.begin(), byteRegs.end(), ins.operand2) != byteRegs.end()) {
                    if (ins.operand2.compare("al") == 0) {
                        preg2 = pal;
                    } else if (ins.operand2.compare("ah") == 0) {
                        preg2 = pah;
                    } else if (ins.operand2.compare("bl") == 0) {
                        preg2 = pbl;
                    } else if (ins.operand2.compare("bh") == 0) {
                        preg2 = pbh;
                    } else if (ins.operand2.compare("cl") == 0) {
                        preg2 = pcl;
                    } else if (ins.operand2.compare("ch") == 0) {
                        preg2 = pch;
                    } else if (ins.operand2.compare("dl") == 0) {
                        preg2 = pbl;
                    } else if (ins.operand2.compare("dh") == 0) {
                        preg2 = pdh;
                    }
                    xor_memory_word_register(preg2, memByte);
                    continue;
                }
            }
        }
        else if(ins.name.compare("sub") == 0) {
            //firsrt operand 8 bit reg
            if (find(byteRegs.begin(), byteRegs.end(), ins.operand1) != byteRegs.end()) {
                unsigned char *preg;
                unsigned char *preg2;
                unsigned char op2;
                unsigned short mop2;
                if (ins.operand1.compare("al") == 0) {
                    preg = pal;
                } else if (ins.operand1.compare("ah") == 0) {
                    preg = pah;
                } else if (ins.operand1.compare("bl") == 0) {
                    preg = pbl;
                } else if (ins.operand1.compare("bh") == 0) {
                    preg = pbh;
                } else if (ins.operand1.compare("cl") == 0) {
                    preg = pcl;
                } else if (ins.operand1.compare("ch") == 0) {
                    preg = pch;
                } else if (ins.operand1.compare("dl") == 0) {
                    preg = pdl;
                } else if (ins.operand1.compare("dh") == 0) {
                    preg = pdh;
                }
                 if (find(byteRegs.begin(), byteRegs.end(), ins.operand2) != byteRegs.end()) {
                    if (ins.operand2.compare("al") == 0) {
                        preg2 = pal;
                    } else if (ins.operand2.compare("ah") == 0) {
                        preg2 = pah;
                    } else if (ins.operand2.compare("bl") == 0) {
                        preg2 = pbl;
                    } else if (ins.operand2.compare("bh") == 0) {
                        preg2 = pbh;
                    } else if (ins.operand2.compare("cl") == 0) {
                        preg2 = pcl;
                    } else if (ins.operand2.compare("ch") == 0) {
                        preg2 = pch;
                    } else if (ins.operand2.compare("dl") == 0) {
                        preg2 = pbl;
                    } else if (ins.operand2.compare("dh") == 0) {
                        preg2 = pdh;
                    }
                    sub_bregister_bregister(preg, preg2);
                    continue;
                }
                //second op hex
                else if (ins.operand2[0] == '0' || ins.operand2[ins.operand2.length() - 1] == 'h') {//second operand hex
                    if (ins.operand2[ins.operand2.length() - 1] == 'h') {
                        string hex = ins.operand2.substr(0, ins.operand2.length() - 1);
                        op2 = stoi(hex, 0, 16);
                    } else {
                        op2 = stoi(ins.operand2, 0, 16);
                    }
                    sub_reg_byte(preg, op2);
                    continue;
                }
                    //second op dec
                else if (ins.operand2[0] == '1' || ins.operand2[0] == '2' || ins.operand2[0] == '3' ||
                         ins.operand2[0] == '4' || ins.operand2[0] == '5' || ins.operand2[0] == '6' ||
                         ins.operand2[0] == '7' || ins.operand2[0] == '8' || ins.operand2[0] == '9' ||
                         ins.operand2[ins.operand2.length() - 1] == 'd') {
                    if (ins.operand2[ins.operand2.length() - 1] == 'd') {
                        string dec = ins.operand2.substr(0, ins.operand2.length() - 1);
                        op2 = stoi(dec, 0, 10);
                    } else {
                        op2 = stoi(ins.operand2, 0, 10);
                    }
                    sub_reg_byte(preg, op2);
                    continue;
                }
                    //second op reg

                    //second op mem
                else if (ins.operand2[0] == 'b' && ins.operand2[1] == '[') {
                    string memLoc = ins.operand2.substr(2, ins.operand2.length() - 1);
                    //hex
                    if (memLoc[0] == '0' || memLoc[memLoc.length() - 1] == 'h') {
                        string hex;
                        if (memLoc[memLoc.length() - 1] == 'h') {
                            hex = memLoc.substr(0, memLoc.length() - 1);
                        } else {
                            hex = memLoc;
                        }
                        if (stoi(hex, 0, 16) >= (2 << 15)) {
                            cout << "Error";
                            return 1;
                        } else {
                            mop2 = stoi(hex, 0, 16);
                        }
                    }
                        //dec
                    else if (memLoc[0] == '1' || memLoc[0] == '2' || memLoc[0] == '3' || memLoc[0] == '4' ||
                             memLoc[0] == '5' || memLoc[0] == '6' ||
                             memLoc[0] == '7' || memLoc[0] == '8' || memLoc[0] == '9' ||
                             memLoc[memLoc.length() - 1] == 'd') {
                        string dec;
                        if (memLoc[memLoc.length() - 1] == 'd') {
                            dec = memLoc.substr(0, memLoc.length() - 1);
                        } else {
                            dec = memLoc;
                        }
                        if (stoi(dec, 0, 10) >= (2 << 15)) {
                            cout << "Error";
                            return 1;
                        } else {
                            mop2 = stoi(dec, 0, 10);
                        }
                    }
                        //reg
                    else if (find(wordRegs.begin(), wordRegs.end(), memLoc) != wordRegs.end()) {
                        if (memLoc.compare("ax") == 0) {
                            mop2 = *pax;
                        } else if (memLoc.compare("bx") == 0) {
                            mop2 = *pbx;
                        } else if (memLoc.compare("cx") == 0) {
                            mop2 = *pcx;
                        } else if (memLoc.compare("dx") == 0) {
                            mop2 = *pdx;
                        } else if (memLoc.compare("di") == 0) {
                            mop2 = *pdi;
                        } else if (memLoc.compare("si") == 0) {
                            mop2 = *psi;
                        } else if (memLoc.compare("sp") == 0) {
                            mop2 = *psp;
                        } else if (memLoc.compare("bp") == 0) {
                            mop2 = *pbp;
                        }
                    }
                    sub_register_byte_mem(preg, mop2);
                    continue;
                }
                    //second op variable
                else if (find(variableNames.begin(), variableNames.end(), ins.operand2) != variableNames.end()) {
                    vector<string>::iterator it = find(variableNames.begin(), variableNames.end(), ins.operand2);
                    int index = distance(variableNames.begin(), it);
                    if (!variables[index].isWord) { mop2 = variables[index].mem - 1; }
                    else { mop2 = variables[index].mem - 2; }
                    sub_register_byte_mem(preg, mop2);
                }
            }
                //firsrt operand 16 bit reg
            else if (find(wordRegs.begin(), wordRegs.end(), ins.operand1) != wordRegs.end()) {
                unsigned short *preg;
                unsigned short *preg2;
                unsigned short op2;
                if (ins.operand1.compare("ax") == 0) {
                    preg = pax;
                } else if (ins.operand1.compare("bx") == 0) {
                    preg = pbx;
                } else if (ins.operand1.compare("cx") == 0) {
                    preg = pcx;
                } else if (ins.operand1.compare("dx") == 0) {
                    preg = pdx;
                } else if (ins.operand1.compare("di") == 0) {
                    preg = pdi;
                } else if (ins.operand1.compare("si") == 0) {
                    preg = psi;
                } else if (ins.operand1.compare("sp") == 0) {
                    preg = psp;
                } else if (ins.operand1.compare("bp") == 0) {
                    preg = pbp;
                }
                //second operand hex
                if (ins.operand2[0] == '0' || ins.operand2[ins.operand2.length() - 1] == 'h') {
                    if (ins.operand2[ins.operand2.length() - 1] == 'h') {
                        string hex = ins.operand2.substr(0, ins.operand2.length() - 1);
                        op2 = stoi(hex, 0, 16);
                    } else {
                        op2 = stoi(ins.operand2, 0, 16);
                    }
                    sub_reg_word(preg, op2);
                    continue;
                }
                    //second op dec
                else if (ins.operand2[0] == '1' || ins.operand2[0] == '2' || ins.operand2[0] == '3' ||
                         ins.operand2[0] == '4' || ins.operand2[0] == '5' || ins.operand2[0] == '6' ||
                         ins.operand2[0] == '7' || ins.operand2[0] == '8' || ins.operand2[0] == '9' ||
                         ins.operand2[ins.operand2.length() - 1] == 'd') {
                    if (ins.operand2[ins.operand2.length() - 1] == 'd') {
                        string dec = ins.operand2.substr(0, ins.operand2.length() - 1);
                        op2 = stoi(dec, 0, 10);
                    } else {
                        op2 = stoi(ins.operand2, 0, 10);
                    }
                    sub_reg_word(preg, op2);
                    continue;
                }
                    //second op reg
                else if (find(wordRegs.begin(), wordRegs.end(), ins.operand2) != wordRegs.end()) {
                    if (ins.operand2.compare("ax") == 0) {
                        preg2 = pax;
                    } else if (ins.operand2.compare("bx") == 0) {
                        preg2 = pbx;
                    } else if (ins.operand2.compare("cx") == 0) {
                        preg2 = pcx;
                    } else if (ins.operand2.compare("dx") == 0) {
                        preg2 = pdx;
                    } else if (ins.operand2.compare("di") == 0) {
                        preg2 = pdi;
                    } else if (ins.operand2.compare("si") == 0) {
                        preg2 = psi;
                    } else if (ins.operand2.compare("sp") == 0) {
                        preg2 = psp;
                    } else if (ins.operand2.compare("bp") == 0) {
                        preg2 = pbp;
                    }
                    sub_wregister_wregister(preg, preg2);
                    continue;
                }
                    //second op mem
                else if (ins.operand2[0] == 'w' && ins.operand2[1] == '[') {
                    string memLoc = ins.operand2.substr(2, ins.operand2.length() - 1);
                    //hex
                    if (memLoc[0] == '0' || memLoc[memLoc.length() - 1] == 'h') {
                        string hex;
                        if (memLoc[memLoc.length() - 1] == 'h') {
                            hex = memLoc.substr(0, memLoc.length() - 1);
                        } else {
                            hex = memLoc;
                        }
                        if (stoi(hex, 0, 16) >= (2 << 15)) {
                            cout << "Error";
                            return 1;
                        } else {
                            op2 = stoi(hex, 0, 16);
                        }
                    }
                        //dec
                    else if (memLoc[0] == '1' || memLoc[0] == '2' || memLoc[0] == '3' || memLoc[0] == '4' ||
                             memLoc[0] == '5' || memLoc[0] == '6' ||
                             memLoc[0] == '7' || memLoc[0] == '8' || memLoc[0] == '9' ||
                             memLoc[memLoc.length() - 1] == 'd') {
                        string dec;
                        if (memLoc[memLoc.length() - 1] == 'd') {
                            dec = memLoc.substr(0, memLoc.length() - 1);
                        } else {
                            dec = memLoc;
                        }
                        if (stoi(dec, 0, 10) >= (2 << 15)) {
                            cout << "Error";
                            return 1;
                        } else {
                            op2 = stoi(dec, 0, 10);
                        }
                    }
                        //reg
                    else if (find(wordRegs.begin(), wordRegs.end(), memLoc) != wordRegs.end()) {
                        unsigned short op2;
                        if (memLoc.compare("ax") == 0) {
                            op2 = *pax;
                        } else if (memLoc.compare("bx") == 0) {
                            op2 = *pbx;
                        } else if (memLoc.compare("cx") == 0) {
                            op2 = *pcx;
                        } else if (memLoc.compare("dx") == 0) {
                            op2 = *pdx;
                        } else if (memLoc.compare("di") == 0) {
                            op2 = *pdi;
                        } else if (memLoc.compare("si") == 0) {
                            op2 = *psi;
                        } else if (memLoc.compare("sp") == 0) {
                            op2 = *psp;
                        } else if (memLoc.compare("bp") == 0) {
                            op2 = *pbp;
                        }
                    }
                    sub_register_word_mem(preg, op2);
                    continue;
                }
                    //second op variable
                else if (find(variableNames.begin(), variableNames.end(), ins.operand2) != variableNames.end()) {
                    vector<string>::iterator it = find(variableNames.begin(), variableNames.end(), ins.operand2);
                    int index = distance(variableNames.begin(), it);
                    if (!variables[index].isWord) { op2 = variables[index].mem - 1; }
                    else { op2 = variables[index].mem - 2; }
                    sub_register_word_mem(preg, op2);
                }
            }
                //first byte operand memory
            else if (ins.operand1[0] == 'b' && ins.operand1[1] == '[') {
                string memLoc = ins.operand1.substr(2, ins.operand1.length() - 1);
                unsigned short memByte;
                unsigned char *preg2;
                unsigned char op2;
                //hex
                if (memLoc[0] == '0' || memLoc[memLoc.length() - 1] == 'h') {
                    string hex;
                    if (memLoc[memLoc.length() - 1] == 'h') {
                        hex = memLoc.substr(0, memLoc.length() - 1);
                    } else {
                        hex = memLoc;
                    }
                    if (stoi(hex, 0, 16) >= (2 << 15)) {
                        cout << "Error";
                        return 1;
                    } else {
                        memByte = stoi(hex, 0, 16);
                    }
                }
                    //dec
                else if (memLoc[0] == '1' || memLoc[0] == '2' || memLoc[0] == '3' || memLoc[0] == '4' ||
                         memLoc[0] == '5' || memLoc[0] == '6' ||
                         memLoc[0] == '7' || memLoc[0] == '8' || memLoc[0] == '9' ||
                         memLoc[memLoc.length() - 1] == 'd') {
                    string dec;
                    if (memLoc[memLoc.length() - 1] == 'd') {
                        dec = memLoc.substr(0, memLoc.length() - 1);
                    } else {
                        dec = memLoc;
                    }
                    if (stoi(dec, 0, 10) >= (2 << 15)) {
                        cout << "Error";
                        return 1;
                    } else {
                        memByte = stoi(dec, 0, 10);
                    }
                }
                    //reg
                else if (find(wordRegs.begin(), wordRegs.end(), memLoc) != wordRegs.end()) {
                    if (memLoc.compare("ax") == 0) {
                        memByte = *pax;
                    } else if (memLoc.compare("bx") == 0) {
                        memByte = *pbx;
                    } else if (memLoc.compare("cx") == 0) {
                        memByte = *pcx;
                    } else if (memLoc.compare("dx") == 0) {
                        memByte = *pdx;
                    } else if (memLoc.compare("di") == 0) {
                        memByte = *pdi;
                    } else if (memLoc.compare("si") == 0) {
                        memByte = *psi;
                    } else if (memLoc.compare("sp") == 0) {
                        memByte = *psp;
                    } else if (memLoc.compare("bp") == 0) {
                        memByte = *pbp;
                    }
                }
                //second operand hex
                if (ins.operand2[0] == '0' || ins.operand2[ins.operand2.length() - 1] == 'h') {//second operand hex
                    if (ins.operand2[ins.operand2.length() - 1] == 'h') {
                        string hex = ins.operand2.substr(0, ins.operand2.length() - 1);
                        op2 = stoi(hex, 0, 16);
                    } else {
                        op2 = stoi(ins.operand2, 0, 16);
                    }
                    sub_mem_byte(memByte, op2);
                    continue;
                }
                    //second operand dec
                else if (ins.operand2[0] == '1' || ins.operand2[0] == '2' || ins.operand2[0] == '3' ||
                         ins.operand2[0] == '4' || ins.operand2[0] == '5' || ins.operand2[0] == '6' ||
                         ins.operand2[0] == '7' || ins.operand2[0] == '8' || ins.operand2[0] == '9' ||
                         ins.operand2[ins.operand2.length() - 1] == 'd') {
                    if (ins.operand2[ins.operand2.length() - 1] == 'd') {
                        string dec = ins.operand2.substr(0, ins.operand2.length() - 1);
                        op2 = stoi(dec, 0, 10);
                    } else {
                        op2 = stoi(ins.operand2, 0, 10);
                    }
                    sub_mem_byte(memByte, op2);
                    continue;
                }
                    //second operand reg
                else if (find(byteRegs.begin(), byteRegs.end(), ins.operand2) != byteRegs.end()) {
                    if (ins.operand2.compare("al") == 0) {
                        preg2 = pal;
                    } else if (ins.operand2.compare("ah") == 0) {
                        preg2 = pah;
                    } else if (ins.operand2.compare("bl") == 0) {
                        preg2 = pbl;
                    } else if (ins.operand2.compare("bh") == 0) {
                        preg2 = pbh;
                    } else if (ins.operand2.compare("cl") == 0) {
                        preg2 = pcl;
                    } else if (ins.operand2.compare("ch") == 0) {
                        preg2 = pch;
                    } else if (ins.operand2.compare("dl") == 0) {
                        preg2 = pbl;
                    } else if (ins.operand2.compare("dh") == 0) {
                        preg2 = pdh;
                    }
                    sub_mem_byte_register(memByte,preg2);
                    continue;
                }
            }
                //first operand word memory
            else if(ins.operand1[0] == 'w' && ins.operand1[1] == '[') {
                string memLoc = ins.operand1.substr(2, ins.operand1.length() - 1);
                unsigned short memByte;
                unsigned char *preg2;
                unsigned short op2;
                //hex
                if (memLoc[0] == '0' || memLoc[memLoc.length() - 1] == 'h') {
                    string hex;
                    if (memLoc[memLoc.length() - 1] == 'h') {
                        hex = memLoc.substr(0, memLoc.length() - 1);
                    } else {
                        hex = memLoc;
                    }
                    if (stoi(hex, 0, 16) >= (2 << 15)) {
                        cout << "Error";
                        return 1;
                    } else {
                        memByte = stoi(hex, 0, 16);
                    }
                }
                    //dec
                else if (memLoc[0] == '1' || memLoc[0] == '2' || memLoc[0] == '3' || memLoc[0] == '4' ||
                         memLoc[0] == '5' || memLoc[0] == '6' ||
                         memLoc[0] == '7' || memLoc[0] == '8' || memLoc[0] == '9' ||
                         memLoc[memLoc.length() - 1] == 'd') {
                    string dec;
                    if (memLoc[memLoc.length() - 1] == 'd') {
                        dec = memLoc.substr(0, memLoc.length() - 1);
                    } else {
                        dec = memLoc;
                    }
                    if (stoi(dec, 0, 10) >= (2 << 15)) {
                        cout << "Error";
                        return 1;
                    } else {
                        memByte = stoi(dec, 0, 10);
                    }
                }
                    //reg
                else if (find(wordRegs.begin(), wordRegs.end(), memLoc) != wordRegs.end()) {
                    if (memLoc.compare("ax") == 0) {
                        memByte = *pax;
                    } else if (memLoc.compare("bx") == 0) {
                        memByte = *pbx;
                    } else if (memLoc.compare("cx") == 0) {
                        memByte = *pcx;
                    } else if (memLoc.compare("dx") == 0) {
                        memByte = *pdx;
                    } else if (memLoc.compare("di") == 0) {
                        memByte = *pdi;
                    } else if (memLoc.compare("si") == 0) {
                        memByte = *psi;
                    } else if (memLoc.compare("sp") == 0) {
                        memByte = *psp;
                    } else if (memLoc.compare("bp") == 0) {
                        memByte = *pbp;
                    }
                }
                //second operand hex
                if (ins.operand2[0] == '0' || ins.operand2[ins.operand2.length() - 1] == 'h') {
                    if (ins.operand2[ins.operand2.length() - 1] == 'h') {
                        string hex = ins.operand2.substr(0, ins.operand2.length() - 1);
                        op2 = stoi(hex, 0, 16);
                    } else {
                        op2 = stoi(ins.operand2, 0, 16);
                    }
                    sub_mem_word(memByte, op2);
                    continue;
                }
                    //second operand dec
                else if (ins.operand2[0] == '1' || ins.operand2[0] == '2' || ins.operand2[0] == '3' ||
                         ins.operand2[0] == '4' || ins.operand2[0] == '5' || ins.operand2[0] == '6' ||
                         ins.operand2[0] == '7' || ins.operand2[0] == '8' || ins.operand2[0] == '9' ||
                         ins.operand2[ins.operand2.length() - 1] == 'd') {
                    if (ins.operand2[ins.operand2.length() - 1] == 'd') {
                        string dec = ins.operand2.substr(0, ins.operand2.length() - 1);
                        op2 = stoi(dec, 0, 10);
                    } else {
                        op2 = stoi(ins.operand2, 0, 10);
                    }
                    sub_mem_word(memByte, op2);
                    continue;
                }
                    //second operand reg
                else if (find(byteRegs.begin(), byteRegs.end(), ins.operand2) != byteRegs.end()) {
                    if (ins.operand2.compare("al") == 0) {
                        preg2 = pal;
                    } else if (ins.operand2.compare("ah") == 0) {
                        preg2 = pah;
                    } else if (ins.operand2.compare("bl") == 0) {
                        preg2 = pbl;
                    } else if (ins.operand2.compare("bh") == 0) {
                        preg2 = pbh;
                    } else if (ins.operand2.compare("cl") == 0) {
                        preg2 = pcl;
                    } else if (ins.operand2.compare("ch") == 0) {
                        preg2 = pch;
                    } else if (ins.operand2.compare("dl") == 0) {
                        preg2 = pbl;
                    } else if (ins.operand2.compare("dh") == 0) {
                        preg2 = pdh;
                    }
                    sub_mem_word_register(memByte, preg2);
                    continue;
                }
            }
            else if(find(variableNames.begin(),variableNames.end(),ins.operand1)!=variableNames.end()) {
                unsigned short varLocation;
                unsigned char op2;
                unsigned char *preg2;
                unsigned short *preg3;
                vector<string>::iterator it = find(variableNames.begin(), variableNames.end(), ins.operand1);
                int index = distance(variableNames.begin(), it);
                if (!variables[index].isWord) { varLocation= variables[index].mem - 1; }
                else { varLocation = variables[index].mem - 2; }
                //second operand hex
                if(!variables[index].isWord) {
                    if (ins.operand2[0] == '0' || ins.operand2[ins.operand2.length() - 1] == 'h') {//second operand hex
                        if (ins.operand2[ins.operand2.length() - 1] == 'h') {
                            string hex = ins.operand2.substr(0, ins.operand2.length() - 1);
                            op2 = stoi(hex, 0, 16);
                        } else {
                            op2 = stoi(ins.operand2, 0, 16);
                        }
                        sub_mem_byte(varLocation, op2);
                        continue;
                    }
                        //second operand dec
                    else if (ins.operand2[0] == '1' || ins.operand2[0] == '2' || ins.operand2[0] == '3' ||
                             ins.operand2[0] == '4' || ins.operand2[0] == '5' || ins.operand2[0] == '6' ||
                             ins.operand2[0] == '7' || ins.operand2[0] == '8' || ins.operand2[0] == '9' ||
                             ins.operand2[ins.operand2.length() - 1] == 'd') {
                        if (ins.operand2[ins.operand2.length() - 1] == 'd') {
                            string dec = ins.operand2.substr(0, ins.operand2.length() - 1);
                            op2 = stoi(dec, 0, 10);
                        } else {
                            op2 = stoi(ins.operand2, 0, 10);
                        }
                        sub_mem_byte(varLocation, op2);
                        continue;
                    }
                        //second operand reg
                    else if (find(byteRegs.begin(), byteRegs.end(), ins.operand2) != byteRegs.end()) {
                        if (ins.operand2.compare("al") == 0) {
                            preg2 = pal;
                        } else if (ins.operand2.compare("ah") == 0) {
                            preg2 = pah;
                        } else if (ins.operand2.compare("bl") == 0) {
                            preg2 = pbl;
                        } else if (ins.operand2.compare("bh") == 0) {
                            preg2 = pbh;
                        } else if (ins.operand2.compare("cl") == 0) {
                            preg2 = pcl;
                        } else if (ins.operand2.compare("ch") == 0) {
                            preg2 = pch;
                        } else if (ins.operand2.compare("dl") == 0) {
                            preg2 = pbl;
                        } else if (ins.operand2.compare("dh") == 0) {
                            preg2 = pdh;
                        }
                        sub_mem_byte_register(varLocation,preg2);
                        continue;
                    }
                }
                else {
                    if (ins.operand2[0] == '0' || ins.operand2[ins.operand2.length() - 1] == 'h') {//second operand hex
                        if (ins.operand2[ins.operand2.length() - 1] == 'h') {
                            string hex = ins.operand2.substr(0, ins.operand2.length() - 1);
                            op2 = stoi(hex, 0, 16);
                        } else {
                            op2 = stoi(ins.operand2, 0, 16);
                        }
                        sub_mem_word(varLocation, op2);
                        continue;
                    }
                        //second operand dec
                    else if (ins.operand2[0] == '1' || ins.operand2[0] == '2' || ins.operand2[0] == '3' ||
                             ins.operand2[0] == '4' || ins.operand2[0] == '5' || ins.operand2[0] == '6' ||
                             ins.operand2[0] == '7' || ins.operand2[0] == '8' || ins.operand2[0] == '9' ||
                             ins.operand2[ins.operand2.length() - 1] == 'd') {
                        if (ins.operand2[ins.operand2.length() - 1] == 'd') {
                            string dec = ins.operand2.substr(0, ins.operand2.length() - 1);
                            op2 = stoi(dec, 0, 10);
                        } else {
                            op2 = stoi(ins.operand2, 0, 10);
                        }
                        sub_mem_word(varLocation, op2);
                        continue;
                    }
                        //second operand reg
                    else if (find(byteRegs.begin(), byteRegs.end(), ins.operand2) != byteRegs.end()) {
                        if (ins.operand2.compare("ax") == 0) {
                            preg3 = pax;
                        } else if (ins.operand2.compare("bx") == 0) {
                            preg3 = pbx;
                        } else if (ins.operand2.compare("cx") == 0) {
                            preg3 = pcx;
                        } else if (ins.operand2.compare("dx") == 0) {
                            preg3 = pdx;
                        } else if (ins.operand2.compare("di") == 0) {
                            preg3 = pdi;
                        } else if (ins.operand2.compare("si") == 0) {
                            preg3 = psi;
                        } else if (ins.operand2.compare("sp") == 0) {
                            preg3 = psp;
                        } else if (ins.operand2.compare("bp") == 0) {
                            preg3 = pbp;
                        }
                        sub_mem_word_register(varLocation,preg3);
                        continue;
                    }
                }
            }
        }
        else if(ins.name.compare("cmp") == 0) {
            //firsrt operand 8 bit reg
            if (find(byteRegs.begin(), byteRegs.end(), ins.operand1) != byteRegs.end()) {
                unsigned char *preg;
                unsigned char *preg2;
                unsigned char op2;
                unsigned short mop2;
                if (ins.operand1.compare("al") == 0) {
                    preg = pal;
                } else if (ins.operand1.compare("ah") == 0) {
                    preg = pah;
                } else if (ins.operand1.compare("bl") == 0) {
                    preg = pbl;
                } else if (ins.operand1.compare("bh") == 0) {
                    preg = pbh;
                } else if (ins.operand1.compare("cl") == 0) {
                    preg = pcl;
                } else if (ins.operand1.compare("ch") == 0) {
                    preg = pch;
                } else if (ins.operand1.compare("dl") == 0) {
                    preg = pdl;
                } else if (ins.operand1.compare("dh") == 0) {
                    preg = pdh;
                }
                //second op hex
                if (ins.operand2[0] == '0' || ins.operand2[ins.operand2.length() - 1] == 'h') {//second operand hex
                    if (ins.operand2[ins.operand2.length() - 1] == 'h') {
                        string hex = ins.operand2.substr(0, ins.operand2.length() - 1);
                        op2 = stoi(hex, 0, 16);
                    } else {
                        op2 = stoi(ins.operand2, 0, 16);
                    }
                    cmp_reg_byte(preg, op2);
                    continue;
                }
                    //second op dec
                else if (ins.operand2[0] == '1' || ins.operand2[0] == '2' || ins.operand2[0] == '3' ||
                         ins.operand2[0] == '4' || ins.operand2[0] == '5' || ins.operand2[0] == '6' ||
                         ins.operand2[0] == '7' || ins.operand2[0] == '8' || ins.operand2[0] == '9' ||
                         ins.operand2[ins.operand2.length() - 1] == 'd') {
                    if (ins.operand2[ins.operand2.length() - 1] == 'd') {
                        string dec = ins.operand2.substr(0, ins.operand2.length() - 1);
                        op2 = stoi(dec, 0, 10);
                    } else {
                        op2 = stoi(ins.operand2, 0, 10);
                    }
                    cmp_reg_byte(preg, op2);
                    continue;
                }
                    //second op reg
                else if (find(byteRegs.begin(), byteRegs.end(), ins.operand2) != byteRegs.end()) {
                    if (ins.operand2.compare("al") == 0) {
                        preg2 = pal;
                    } else if (ins.operand2.compare("ah") == 0) {
                        preg2 = pah;
                    } else if (ins.operand2.compare("bl") == 0) {
                        preg2 = pbl;
                    } else if (ins.operand2.compare("bh") == 0) {
                        preg2 = pbh;
                    } else if (ins.operand2.compare("cl") == 0) {
                        preg2 = pcl;
                    } else if (ins.operand2.compare("ch") == 0) {
                        preg2 = pch;
                    } else if (ins.operand2.compare("dl") == 0) {
                        preg2 = pbl;
                    } else if (ins.operand2.compare("dh") == 0) {
                        preg2 = pdh;
                    }
                    cmp_bregister_bregister(preg, preg2);
                    continue;
                }
                    //second op mem
                else if (ins.operand2[0] == 'b' && ins.operand2[1] == '[') {
                    string memLoc = ins.operand2.substr(2, ins.operand2.length() - 1);
                    //hex
                    if (memLoc[0] == '0' || memLoc[memLoc.length() - 1] == 'h') {
                        string hex;
                        if (memLoc[memLoc.length() - 1] == 'h') {
                            hex = memLoc.substr(0, memLoc.length() - 1);
                        } else {
                            hex = memLoc;
                        }
                        if (stoi(hex, 0, 16) >= (2 << 15)) {
                            cout << "Error";
                            return 1;
                        } else {
                            mop2 = stoi(hex, 0, 16);
                        }
                    }
                        //dec
                    else if (memLoc[0] == '1' || memLoc[0] == '2' || memLoc[0] == '3' || memLoc[0] == '4' ||
                             memLoc[0] == '5' || memLoc[0] == '6' ||
                             memLoc[0] == '7' || memLoc[0] == '8' || memLoc[0] == '9' ||
                             memLoc[memLoc.length() - 1] == 'd') {
                        string dec;
                        if (memLoc[memLoc.length() - 1] == 'd') {
                            dec = memLoc.substr(0, memLoc.length() - 1);
                        } else {
                            dec = memLoc;
                        }
                        if (stoi(dec, 0, 10) >= (2 << 15)) {
                            cout << "Error";
                            return 1;
                        } else {
                            mop2 = stoi(dec, 0, 10);
                        }
                    }
                        //reg
                    else if (find(wordRegs.begin(), wordRegs.end(), memLoc) != wordRegs.end()) {
                        if (memLoc.compare("ax") == 0) {
                            mop2 = *pax;
                        } else if (memLoc.compare("bx") == 0) {
                            mop2 = *pbx;
                        } else if (memLoc.compare("cx") == 0) {
                            mop2 = *pcx;
                        } else if (memLoc.compare("dx") == 0) {
                            mop2 = *pdx;
                        } else if (memLoc.compare("di") == 0) {
                            mop2 = *pdi;
                        } else if (memLoc.compare("si") == 0) {
                            mop2 = *psi;
                        } else if (memLoc.compare("sp") == 0) {
                            mop2 = *psp;
                        } else if (memLoc.compare("bp") == 0) {
                            mop2 = *pbp;
                        }
                    }
                    cmp_register_byte_mem(preg, mop2);
                    continue;
                }
                    //second op variable
                else if (find(variableNames.begin(), variableNames.end(), ins.operand2) != variableNames.end()) {
                    vector<string>::iterator it = find(variableNames.begin(), variableNames.end(), ins.operand2);
                    int index = distance(variableNames.begin(), it);
                    if (!variables[index].isWord) { mop2 = variables[index].mem - 1; }
                    else { mop2 = variables[index].mem - 2; }
                    cmp_register_byte_mem(preg, mop2);
                }
            }
                //firsrt operand 16 bit reg
            else if (find(wordRegs.begin(), wordRegs.end(), ins.operand1) != wordRegs.end()) {
                unsigned short *preg;
                unsigned short *preg2;
                unsigned short op2;
                if (ins.operand1.compare("ax") == 0) {
                    preg = pax;
                } else if (ins.operand1.compare("bx") == 0) {
                    preg = pbx;
                } else if (ins.operand1.compare("cx") == 0) {
                    preg = pcx;
                } else if (ins.operand1.compare("dx") == 0) {
                    preg = pdx;
                } else if (ins.operand1.compare("di") == 0) {
                    preg = pdi;
                } else if (ins.operand1.compare("si") == 0) {
                    preg = psi;
                } else if (ins.operand1.compare("sp") == 0) {
                    preg = psp;
                } else if (ins.operand1.compare("bp") == 0) {
                    preg = pbp;
                }
                //second operand hex
                if (ins.operand2[0] == '0' || ins.operand2[ins.operand2.length() - 1] == 'h') {
                    if (ins.operand2[ins.operand2.length() - 1] == 'h') {
                        string hex = ins.operand2.substr(0, ins.operand2.length() - 1);
                        op2 = stoi(hex, 0, 16);
                    } else {
                        op2 = stoi(ins.operand2, 0, 16);
                    }
                    cmp_reg_word(preg, op2);
                    continue;
                }
                    //second op dec
                else if (ins.operand2[0] == '1' || ins.operand2[0] == '2' || ins.operand2[0] == '3' ||
                         ins.operand2[0] == '4' || ins.operand2[0] == '5' || ins.operand2[0] == '6' ||
                         ins.operand2[0] == '7' || ins.operand2[0] == '8' || ins.operand2[0] == '9' ||
                         ins.operand2[ins.operand2.length() - 1] == 'd') {
                    if (ins.operand2[ins.operand2.length() - 1] == 'd') {
                        string dec = ins.operand2.substr(0, ins.operand2.length() - 1);
                        op2 = stoi(dec, 0, 10);
                    } else {
                        op2 = stoi(ins.operand2, 0, 10);
                    }
                    cmp_reg_word(preg, op2);
                    continue;
                }
                    //second op reg
                else if (find(wordRegs.begin(), wordRegs.end(), ins.operand2) != wordRegs.end()) {
                    if (ins.operand2.compare("ax") == 0) {
                        preg2 = pax;
                    } else if (ins.operand2.compare("bx") == 0) {
                        preg2 = pbx;
                    } else if (ins.operand2.compare("cx") == 0) {
                        preg2 = pcx;
                    } else if (ins.operand2.compare("dx") == 0) {
                        preg2 = pdx;
                    } else if (ins.operand2.compare("di") == 0) {
                        preg2 = pdi;
                    } else if (ins.operand2.compare("si") == 0) {
                        preg2 = psi;
                    } else if (ins.operand2.compare("sp") == 0) {
                        preg2 = psp;
                    } else if (ins.operand2.compare("bp") == 0) {
                        preg2 = pbp;
                    }
                    cmp_wregister_wregister(preg, preg2);
                    continue;
                }
                    //second op mem
                else if (ins.operand2[0] == 'w' && ins.operand2[1] == '[') {
                    string memLoc = ins.operand2.substr(2, ins.operand2.length() - 1);
                    //hex
                    if (memLoc[0] == '0' || memLoc[memLoc.length() - 1] == 'h') {
                        string hex;
                        if (memLoc[memLoc.length() - 1] == 'h') {
                            hex = memLoc.substr(0, memLoc.length() - 1);
                        } else {
                            hex = memLoc;
                        }
                        if (stoi(hex, 0, 16) >= (2 << 15)) {
                            cout << "Error";
                            return 1;
                        } else {
                            op2 = stoi(hex, 0, 16);
                        }
                    }
                        //dec
                    else if (memLoc[0] == '1' || memLoc[0] == '2' || memLoc[0] == '3' || memLoc[0] == '4' ||
                             memLoc[0] == '5' || memLoc[0] == '6' ||
                             memLoc[0] == '7' || memLoc[0] == '8' || memLoc[0] == '9' ||
                             memLoc[memLoc.length() - 1] == 'd') {
                        string dec;
                        if (memLoc[memLoc.length() - 1] == 'd') {
                            dec = memLoc.substr(0, memLoc.length() - 1);
                        } else {
                            dec = memLoc;
                        }
                        if (stoi(dec, 0, 10) >= (2 << 15)) {
                            cout << "Error";
                            return 1;
                        } else {
                            op2 = stoi(dec, 0, 10);
                        }
                    }
                        //reg
                    else if (find(wordRegs.begin(), wordRegs.end(), memLoc) != wordRegs.end()) {
                        unsigned short op2;
                        if (memLoc.compare("ax") == 0) {
                            op2 = *pax;
                        } else if (memLoc.compare("bx") == 0) {
                            op2 = *pbx;
                        } else if (memLoc.compare("cx") == 0) {
                            op2 = *pcx;
                        } else if (memLoc.compare("dx") == 0) {
                            op2 = *pdx;
                        } else if (memLoc.compare("di") == 0) {
                            op2 = *pdi;
                        } else if (memLoc.compare("si") == 0) {
                            op2 = *psi;
                        } else if (memLoc.compare("sp") == 0) {
                            op2 = *psp;
                        } else if (memLoc.compare("bp") == 0) {
                            op2 = *pbp;
                        }
                    }
                    cmp_register_word_mem(preg, op2);
                    continue;
                }
                    //second op variable
                else if (find(variableNames.begin(), variableNames.end(), ins.operand2) != variableNames.end()) {
                    vector<string>::iterator it = find(variableNames.begin(), variableNames.end(), ins.operand2);
                    int index = distance(variableNames.begin(), it);
                    if (!variables[index].isWord) { op2 = variables[index].mem - 1; }
                    else { op2 = variables[index].mem - 2; }
                    cmp_register_word_mem(preg, op2);
                }
            }
                //first byte operand memory
            else if (ins.operand1[0] == 'b' && ins.operand1[1] == '[') {
                string memLoc = ins.operand1.substr(2, ins.operand1.length() - 1);
                unsigned short memByte;
                unsigned char *preg2;
                unsigned char op2;
                //hex
                if (memLoc[0] == '0' || memLoc[memLoc.length() - 1] == 'h') {
                    string hex;
                    if (memLoc[memLoc.length() - 1] == 'h') {
                        hex = memLoc.substr(0, memLoc.length() - 1);
                    } else {
                        hex = memLoc;
                    }
                    if (stoi(hex, 0, 16) >= (2 << 15)) {
                        cout << "Error";
                        return 1;
                    } else {
                        memByte = stoi(hex, 0, 16);
                    }
                }
                    //dec
                else if (memLoc[0] == '1' || memLoc[0] == '2' || memLoc[0] == '3' || memLoc[0] == '4' ||
                         memLoc[0] == '5' || memLoc[0] == '6' ||
                         memLoc[0] == '7' || memLoc[0] == '8' || memLoc[0] == '9' ||
                         memLoc[memLoc.length() - 1] == 'd') {
                    string dec;
                    if (memLoc[memLoc.length() - 1] == 'd') {
                        dec = memLoc.substr(0, memLoc.length() - 1);
                    } else {
                        dec = memLoc;
                    }
                    if (stoi(dec, 0, 10) >= (2 << 15)) {
                        cout << "Error";
                        return 1;
                    } else {
                        memByte = stoi(dec, 0, 10);
                    }
                }
                    //reg
                else if (find(wordRegs.begin(), wordRegs.end(), memLoc) != wordRegs.end()) {
                    if (memLoc.compare("ax") == 0) {
                        memByte = *pax;
                    } else if (memLoc.compare("bx") == 0) {
                        memByte = *pbx;
                    } else if (memLoc.compare("cx") == 0) {
                        memByte = *pcx;
                    } else if (memLoc.compare("dx") == 0) {
                        memByte = *pdx;
                    } else if (memLoc.compare("di") == 0) {
                        memByte = *pdi;
                    } else if (memLoc.compare("si") == 0) {
                        memByte = *psi;
                    } else if (memLoc.compare("sp") == 0) {
                        memByte = *psp;
                    } else if (memLoc.compare("bp") == 0) {
                        memByte = *pbp;
                    }
                }
                //second operand hex
                if (ins.operand2[0] == '0' || ins.operand2[ins.operand2.length() - 1] == 'h') {//second operand hex
                    if (ins.operand2[ins.operand2.length() - 1] == 'h') {
                        string hex = ins.operand2.substr(0, ins.operand2.length() - 1);
                        op2 = stoi(hex, 0, 16);
                    } else {
                        op2 = stoi(ins.operand2, 0, 16);
                    }
                    cmp_mem_byte(memByte, op2);
                    continue;
                }
                    //second operand dec
                else if (ins.operand2[0] == '1' || ins.operand2[0] == '2' || ins.operand2[0] == '3' ||
                         ins.operand2[0] == '4' || ins.operand2[0] == '5' || ins.operand2[0] == '6' ||
                         ins.operand2[0] == '7' || ins.operand2[0] == '8' || ins.operand2[0] == '9' ||
                         ins.operand2[ins.operand2.length() - 1] == 'd') {
                    if (ins.operand2[ins.operand2.length() - 1] == 'd') {
                        string dec = ins.operand2.substr(0, ins.operand2.length() - 1);
                        op2 = stoi(dec, 0, 10);
                    } else {
                        op2 = stoi(ins.operand2, 0, 10);
                    }
                    cmp_mem_byte(memByte, op2);
                    continue;
                }
                    //second operand reg
                else if (find(byteRegs.begin(), byteRegs.end(), ins.operand2) != byteRegs.end()) {
                    if (ins.operand2.compare("al") == 0) {
                        preg2 = pal;
                    } else if (ins.operand2.compare("ah") == 0) {
                        preg2 = pah;
                    } else if (ins.operand2.compare("bl") == 0) {
                        preg2 = pbl;
                    } else if (ins.operand2.compare("bh") == 0) {
                        preg2 = pbh;
                    } else if (ins.operand2.compare("cl") == 0) {
                        preg2 = pcl;
                    } else if (ins.operand2.compare("ch") == 0) {
                        preg2 = pch;
                    } else if (ins.operand2.compare("dl") == 0) {
                        preg2 = pbl;
                    } else if (ins.operand2.compare("dh") == 0) {
                        preg2 = pdh;
                    }
                    cmp_mem_byte_register(memByte,preg2);
                    continue;
                }
            }
                //first operand word memory
            else if(ins.operand1[0] == 'w' && ins.operand1[1] == '[') {
                string memLoc = ins.operand1.substr(2, ins.operand1.length() - 1);
                unsigned short memByte;
                unsigned char *preg2;
                unsigned short op2;
                //hex
                if (memLoc[0] == '0' || memLoc[memLoc.length() - 1] == 'h') {
                    string hex;
                    if (memLoc[memLoc.length() - 1] == 'h') {
                        hex = memLoc.substr(0, memLoc.length() - 1);
                    } else {
                        hex = memLoc;
                    }
                    if (stoi(hex, 0, 16) >= (2 << 15)) {
                        cout << "Error";
                        return 1;
                    } else {
                        memByte = stoi(hex, 0, 16);
                    }
                }
                    //dec
                else if (memLoc[0] == '1' || memLoc[0] == '2' || memLoc[0] == '3' || memLoc[0] == '4' ||
                         memLoc[0] == '5' || memLoc[0] == '6' ||
                         memLoc[0] == '7' || memLoc[0] == '8' || memLoc[0] == '9' ||
                         memLoc[memLoc.length() - 1] == 'd') {
                    string dec;
                    if (memLoc[memLoc.length() - 1] == 'd') {
                        dec = memLoc.substr(0, memLoc.length() - 1);
                    } else {
                        dec = memLoc;
                    }
                    if (stoi(dec, 0, 10) >= (2 << 15)) {
                        cout << "Error";
                        return 1;
                    } else {
                        memByte = stoi(dec, 0, 10);
                    }
                }
                    //reg
                else if (find(wordRegs.begin(), wordRegs.end(), memLoc) != wordRegs.end()) {
                    if (memLoc.compare("ax") == 0) {
                        memByte = *pax;
                    } else if (memLoc.compare("bx") == 0) {
                        memByte = *pbx;
                    } else if (memLoc.compare("cx") == 0) {
                        memByte = *pcx;
                    } else if (memLoc.compare("dx") == 0) {
                        memByte = *pdx;
                    } else if (memLoc.compare("di") == 0) {
                        memByte = *pdi;
                    } else if (memLoc.compare("si") == 0) {
                        memByte = *psi;
                    } else if (memLoc.compare("sp") == 0) {
                        memByte = *psp;
                    } else if (memLoc.compare("bp") == 0) {
                        memByte = *pbp;
                    }
                }
                //second operand hex
                if (ins.operand2[0] == '0' || ins.operand2[ins.operand2.length() - 1] == 'h') {
                    if (ins.operand2[ins.operand2.length() - 1] == 'h') {
                        string hex = ins.operand2.substr(0, ins.operand2.length() - 1);
                        op2 = stoi(hex, 0, 16);
                    } else {
                        op2 = stoi(ins.operand2, 0, 16);
                    }
                    cmp_mem_word(memByte, op2);
                    continue;
                }
                    //second operand dec
                else if (ins.operand2[0] == '1' || ins.operand2[0] == '2' || ins.operand2[0] == '3' ||
                         ins.operand2[0] == '4' || ins.operand2[0] == '5' || ins.operand2[0] == '6' ||
                         ins.operand2[0] == '7' || ins.operand2[0] == '8' || ins.operand2[0] == '9' ||
                         ins.operand2[ins.operand2.length() - 1] == 'd') {
                    if (ins.operand2[ins.operand2.length() - 1] == 'd') {
                        string dec = ins.operand2.substr(0, ins.operand2.length() - 1);
                        op2 = stoi(dec, 0, 10);
                    } else {
                        op2 = stoi(ins.operand2, 0, 10);
                    }
                    cmp_mem_word(memByte, op2);
                    continue;
                }
                    //second operand reg
                else if (find(byteRegs.begin(), byteRegs.end(), ins.operand2) != byteRegs.end()) {
                    if (ins.operand2.compare("al") == 0) {
                        preg2 = pal;
                    } else if (ins.operand2.compare("ah") == 0) {
                        preg2 = pah;
                    } else if (ins.operand2.compare("bl") == 0) {
                        preg2 = pbl;
                    } else if (ins.operand2.compare("bh") == 0) {
                        preg2 = pbh;
                    } else if (ins.operand2.compare("cl") == 0) {
                        preg2 = pcl;
                    } else if (ins.operand2.compare("ch") == 0) {
                        preg2 = pch;
                    } else if (ins.operand2.compare("dl") == 0) {
                        preg2 = pbl;
                    } else if (ins.operand2.compare("dh") == 0) {
                        preg2 = pdh;
                    }
                    cmp_mem_word_register(memByte, preg2);
                    continue;
                }
            }
        }
        else if(ins.name.compare("inc") == 0) {
            //firsrt operand 8 bit reg
            if (find(byteRegs.begin(), byteRegs.end(), ins.operand1) != byteRegs.end()) {
                unsigned char *preg;
                unsigned char *preg2;
                unsigned char op2;
                unsigned short mop2;
                if (ins.operand1.compare("al") == 0) {
                    preg = pal;
                } else if (ins.operand1.compare("ah") == 0) {
                    preg = pah;
                } else if (ins.operand1.compare("bl") == 0) {
                    preg = pbl;
                } else if (ins.operand1.compare("bh") == 0) {
                    preg = pbh;
                } else if (ins.operand1.compare("cl") == 0) {
                    preg = pcl;
                } else if (ins.operand1.compare("ch") == 0) {
                    preg = pch;
                } else if (ins.operand1.compare("dl") == 0) {
                    preg = pdl;
                } else if (ins.operand1.compare("dh") == 0) {
                    preg = pdh;
                }
                inc_breg(preg);
                continue;
            }
                //firsrt operand 16 bit reg
            else if (find(wordRegs.begin(), wordRegs.end(), ins.operand1) != wordRegs.end()) {
                unsigned short *preg;
                unsigned short *preg2;
                unsigned short op2;
                if (ins.operand1.compare("ax") == 0) {
                    preg = pax;
                } else if (ins.operand1.compare("bx") == 0) {
                    preg = pbx;
                } else if (ins.operand1.compare("cx") == 0) {
                    preg = pcx;
                } else if (ins.operand1.compare("dx") == 0) {
                    preg = pdx;
                } else if (ins.operand1.compare("di") == 0) {
                    preg = pdi;
                } else if (ins.operand1.compare("si") == 0) {
                    preg = psi;
                } else if (ins.operand1.compare("sp") == 0) {
                    preg = psp;
                } else if (ins.operand1.compare("bp") == 0) {
                    preg = pbp;
                }
                inc_wreg(preg);
                continue;
            }
                //first byte operand memory
            else if (ins.operand1[0] == 'b' && ins.operand1[1] == '[') {
                string memLoc = ins.operand1.substr(2, ins.operand1.length() - 1);
                unsigned short memByte;
                unsigned char *preg2;
                unsigned char op2;
                //hex
                if (memLoc[0] == '0' || memLoc[memLoc.length() - 1] == 'h') {
                    string hex;
                    if (memLoc[memLoc.length() - 1] == 'h') {
                        hex = memLoc.substr(0, memLoc.length() - 1);
                    } else {
                        hex = memLoc;
                    }
                    if (stoi(hex, 0, 16) >= (2 << 15)) {
                        cout << "Error";
                        return 1;
                    } else {
                        memByte = stoi(hex, 0, 16);
                    }
                }
                    //dec
                else if (memLoc[0] == '1' || memLoc[0] == '2' || memLoc[0] == '3' || memLoc[0] == '4' ||
                         memLoc[0] == '5' || memLoc[0] == '6' ||
                         memLoc[0] == '7' || memLoc[0] == '8' || memLoc[0] == '9' ||
                         memLoc[memLoc.length() - 1] == 'd') {
                    string dec;
                    if (memLoc[memLoc.length() - 1] == 'd') {
                        dec = memLoc.substr(0, memLoc.length() - 1);
                    } else {
                        dec = memLoc;
                    }
                    if (stoi(dec, 0, 10) >= (2 << 15)) {
                        cout << "Error";
                        return 1;
                    } else {
                        memByte = stoi(dec, 0, 10);
                    }
                }
                    //reg
                else if (find(wordRegs.begin(), wordRegs.end(), memLoc) != wordRegs.end()) {
                    if (memLoc.compare("ax") == 0) {
                        memByte = *pax;
                    } else if (memLoc.compare("bx") == 0) {
                        memByte = *pbx;
                    } else if (memLoc.compare("cx") == 0) {
                        memByte = *pcx;
                    } else if (memLoc.compare("dx") == 0) {
                        memByte = *pdx;
                    } else if (memLoc.compare("di") == 0) {
                        memByte = *pdi;
                    } else if (memLoc.compare("si") == 0) {
                        memByte = *psi;
                    } else if (memLoc.compare("sp") == 0) {
                        memByte = *psp;
                    } else if (memLoc.compare("bp") == 0) {
                        memByte = *pbp;
                    }
                }
                inc_mem_byte(memByte);
                continue;
            }
                //first operand word memory
            else if(ins.operand1[0] == 'w' && ins.operand1[1] == '[') {
                string memLoc = ins.operand1.substr(2, ins.operand1.length() - 1);
                unsigned short memByte;
                unsigned char *preg2;
                unsigned short op2;
                //hex
                if (memLoc[0] == '0' || memLoc[memLoc.length() - 1] == 'h') {
                    string hex;
                    if (memLoc[memLoc.length() - 1] == 'h') {
                        hex = memLoc.substr(0, memLoc.length() - 1);
                    } else {
                        hex = memLoc;
                    }
                    if (stoi(hex, 0, 16) >= (2 << 15)) {
                        cout << "Error";
                        return 1;
                    } else {
                        memByte = stoi(hex, 0, 16);
                    }
                }
                    //dec
                else if (memLoc[0] == '1' || memLoc[0] == '2' || memLoc[0] == '3' || memLoc[0] == '4' ||
                         memLoc[0] == '5' || memLoc[0] == '6' ||
                         memLoc[0] == '7' || memLoc[0] == '8' || memLoc[0] == '9' ||
                         memLoc[memLoc.length() - 1] == 'd') {
                    string dec;
                    if (memLoc[memLoc.length() - 1] == 'd') {
                        dec = memLoc.substr(0, memLoc.length() - 1);
                    } else {
                        dec = memLoc;
                    }
                    if (stoi(dec, 0, 10) >= (2 << 15)) {
                        cout << "Error";
                        return 1;
                    } else {
                        memByte = stoi(dec, 0, 10);
                    }
                }
                    //reg
                else if (find(wordRegs.begin(), wordRegs.end(), memLoc) != wordRegs.end()) {
                    if (memLoc.compare("ax") == 0) {
                        memByte = *pax;
                    } else if (memLoc.compare("bx") == 0) {
                        memByte = *pbx;
                    } else if (memLoc.compare("cx") == 0) {
                        memByte = *pcx;
                    } else if (memLoc.compare("dx") == 0) {
                        memByte = *pdx;
                    } else if (memLoc.compare("di") == 0) {
                        memByte = *pdi;
                    } else if (memLoc.compare("si") == 0) {
                        memByte = *psi;
                    } else if (memLoc.compare("sp") == 0) {
                        memByte = *psp;
                    } else if (memLoc.compare("bp") == 0) {
                        memByte = *pbp;
                    }
                }
                inc_mem_word(memByte);
                continue;
            }
        }
        else if(ins.name.compare("dec") == 0) {
            //firsrt operand 8 bit reg
            if (find(byteRegs.begin(), byteRegs.end(), ins.operand1) != byteRegs.end()) {
                unsigned char *preg;
                unsigned char *preg2;
                unsigned char op2;
                unsigned short mop2;
                if (ins.operand1.compare("al") == 0) {
                    preg = pal;
                } else if (ins.operand1.compare("ah") == 0) {
                    preg = pah;
                } else if (ins.operand1.compare("bl") == 0) {
                    preg = pbl;
                } else if (ins.operand1.compare("bh") == 0) {
                    preg = pbh;
                } else if (ins.operand1.compare("cl") == 0) {
                    preg = pcl;
                } else if (ins.operand1.compare("ch") == 0) {
                    preg = pch;
                } else if (ins.operand1.compare("dl") == 0) {
                    preg = pdl;
                } else if (ins.operand1.compare("dh") == 0) {
                    preg = pdh;
                }
                dec_breg(preg);
                continue;
            }
                //firsrt operand 16 bit reg
            else if (find(wordRegs.begin(), wordRegs.end(), ins.operand1) != wordRegs.end()) {
                unsigned short *preg;
                unsigned short *preg2;
                unsigned short op2;
                if (ins.operand1.compare("ax") == 0) {
                    preg = pax;
                } else if (ins.operand1.compare("bx") == 0) {
                    preg = pbx;
                } else if (ins.operand1.compare("cx") == 0) {
                    preg = pcx;
                } else if (ins.operand1.compare("dx") == 0) {
                    preg = pdx;
                } else if (ins.operand1.compare("di") == 0) {
                    preg = pdi;
                } else if (ins.operand1.compare("si") == 0) {
                    preg = psi;
                } else if (ins.operand1.compare("sp") == 0) {
                    preg = psp;
                } else if (ins.operand1.compare("bp") == 0) {
                    preg = pbp;
                }
                dec_wreg(preg);
                continue;
            }
                //first byte operand memory
            else if (ins.operand1[0] == 'b' && ins.operand1[1] == '[') {
                string memLoc = ins.operand1.substr(2, ins.operand1.length() - 1);
                unsigned short memByte;
                unsigned char *preg2;
                unsigned char op2;
                //hex
                if (memLoc[0] == '0' || memLoc[memLoc.length() - 1] == 'h') {
                    string hex;
                    if (memLoc[memLoc.length() - 1] == 'h') {
                        hex = memLoc.substr(0, memLoc.length() - 1);
                    } else {
                        hex = memLoc;
                    }
                    if (stoi(hex, 0, 16) >= (2 << 15)) {
                        cout << "Error";
                        return 1;
                    } else {
                        memByte = stoi(hex, 0, 16);
                    }
                }
                    //dec
                else if (memLoc[0] == '1' || memLoc[0] == '2' || memLoc[0] == '3' || memLoc[0] == '4' ||
                         memLoc[0] == '5' || memLoc[0] == '6' ||
                         memLoc[0] == '7' || memLoc[0] == '8' || memLoc[0] == '9' ||
                         memLoc[memLoc.length() - 1] == 'd') {
                    string dec;
                    if (memLoc[memLoc.length() - 1] == 'd') {
                        dec = memLoc.substr(0, memLoc.length() - 1);
                    } else {
                        dec = memLoc;
                    }
                    if (stoi(dec, 0, 10) >= (2 << 15)) {
                        cout << "Error";
                        return 1;
                    } else {
                        memByte = stoi(dec, 0, 10);
                    }
                }
                    //reg
                else if (find(wordRegs.begin(), wordRegs.end(), memLoc) != wordRegs.end()) {
                    if (memLoc.compare("ax") == 0) {
                        memByte = *pax;
                    } else if (memLoc.compare("bx") == 0) {
                        memByte = *pbx;
                    } else if (memLoc.compare("cx") == 0) {
                        memByte = *pcx;
                    } else if (memLoc.compare("dx") == 0) {
                        memByte = *pdx;
                    } else if (memLoc.compare("di") == 0) {
                        memByte = *pdi;
                    } else if (memLoc.compare("si") == 0) {
                        memByte = *psi;
                    } else if (memLoc.compare("sp") == 0) {
                        memByte = *psp;
                    } else if (memLoc.compare("bp") == 0) {
                        memByte = *pbp;
                    }
                }
                dec_mem_byte(memByte);
                continue;
            }
                //first operand word memory
            else if(ins.operand1[0] == 'w' && ins.operand1[1] == '[') {
                string memLoc = ins.operand1.substr(2, ins.operand1.length() - 1);
                unsigned short memByte;
                unsigned char *preg2;
                unsigned short op2;
                //hex
                if (memLoc[0] == '0' || memLoc[memLoc.length() - 1] == 'h') {
                    string hex;
                    if (memLoc[memLoc.length() - 1] == 'h') {
                        hex = memLoc.substr(0, memLoc.length() - 1);
                    } else {
                        hex = memLoc;
                    }
                    if (stoi(hex, 0, 16) >= (2 << 15)) {
                        cout << "Error";
                        return 1;
                    } else {
                        memByte = stoi(hex, 0, 16);
                    }
                }
                    //dec
                else if (memLoc[0] == '1' || memLoc[0] == '2' || memLoc[0] == '3' || memLoc[0] == '4' ||
                         memLoc[0] == '5' || memLoc[0] == '6' ||
                         memLoc[0] == '7' || memLoc[0] == '8' || memLoc[0] == '9' ||
                         memLoc[memLoc.length() - 1] == 'd') {
                    string dec;
                    if (memLoc[memLoc.length() - 1] == 'd') {
                        dec = memLoc.substr(0, memLoc.length() - 1);
                    } else {
                        dec = memLoc;
                    }
                    if (stoi(dec, 0, 10) >= (2 << 15)) {
                        cout << "Error";
                        return 1;
                    } else {
                        memByte = stoi(dec, 0, 10);
                    }
                }
                    //reg
                else if (find(wordRegs.begin(), wordRegs.end(), memLoc) != wordRegs.end()) {
                    if (memLoc.compare("ax") == 0) {
                        memByte = *pax;
                    } else if (memLoc.compare("bx") == 0) {
                        memByte = *pbx;
                    } else if (memLoc.compare("cx") == 0) {
                        memByte = *pcx;
                    } else if (memLoc.compare("dx") == 0) {
                        memByte = *pdx;
                    } else if (memLoc.compare("di") == 0) {
                        memByte = *pdi;
                    } else if (memLoc.compare("si") == 0) {
                        memByte = *psi;
                    } else if (memLoc.compare("sp") == 0) {
                        memByte = *psp;
                    } else if (memLoc.compare("bp") == 0) {
                        memByte = *pbp;
                    }
                }
                dec_mem_word(memByte);
                continue;
            }
        }
        else if(ins.name.compare("mul") == 0) {
            //firsrt operand 8 bit reg
            if (find(byteRegs.begin(), byteRegs.end(), ins.operand1) != byteRegs.end()) {
                unsigned char *preg;
                unsigned char *preg2;
                unsigned char op2;
                unsigned short mop2;
                if (ins.operand1.compare("al") == 0) {
                    preg = pal;
                } else if (ins.operand1.compare("ah") == 0) {
                    preg = pah;
                } else if (ins.operand1.compare("bl") == 0) {
                    preg = pbl;
                } else if (ins.operand1.compare("bh") == 0) {
                    preg = pbh;
                } else if (ins.operand1.compare("cl") == 0) {
                    preg = pcl;
                } else if (ins.operand1.compare("ch") == 0) {
                    preg = pch;
                } else if (ins.operand1.compare("dl") == 0) {
                    preg = pdl;
                } else if (ins.operand1.compare("dh") == 0) {
                    preg = pdh;
                }
                mul_breg(preg);
                continue;
            }
                //firsrt operand 16 bit reg
            else if (find(wordRegs.begin(), wordRegs.end(), ins.operand1) != wordRegs.end()) {
                unsigned short *preg;
                unsigned short *preg2;
                unsigned short op2;
                if (ins.operand1.compare("ax") == 0) {
                    preg = pax;
                } else if (ins.operand1.compare("bx") == 0) {
                    preg = pbx;
                } else if (ins.operand1.compare("cx") == 0) {
                    preg = pcx;
                } else if (ins.operand1.compare("dx") == 0) {
                    preg = pdx;
                } else if (ins.operand1.compare("di") == 0) {
                    preg = pdi;
                } else if (ins.operand1.compare("si") == 0) {
                    preg = psi;
                } else if (ins.operand1.compare("sp") == 0) {
                    preg = psp;
                } else if (ins.operand1.compare("bp") == 0) {
                    preg = pbp;
                }
                mul_wreg(preg);
                continue;
            }
                //first byte operand memory
            else if (ins.operand1[0] == 'b' && ins.operand1[1] == '[') {
                string memLoc = ins.operand1.substr(2, ins.operand1.length() - 1);
                unsigned short memByte;
                unsigned char *preg2;
                unsigned char op2;
                //hex
                if (memLoc[0] == '0' || memLoc[memLoc.length() - 1] == 'h') {
                    string hex;
                    if (memLoc[memLoc.length() - 1] == 'h') {
                        hex = memLoc.substr(0, memLoc.length() - 1);
                    } else {
                        hex = memLoc;
                    }
                    if (stoi(hex, 0, 16) >= (2 << 15)) {
                        cout << "Error";
                        return 1;
                    } else {
                        memByte = stoi(hex, 0, 16);
                    }
                }
                    //dec
                else if (memLoc[0] == '1' || memLoc[0] == '2' || memLoc[0] == '3' || memLoc[0] == '4' ||
                         memLoc[0] == '5' || memLoc[0] == '6' ||
                         memLoc[0] == '7' || memLoc[0] == '8' || memLoc[0] == '9' ||
                         memLoc[memLoc.length() - 1] == 'd') {
                    string dec;
                    if (memLoc[memLoc.length() - 1] == 'd') {
                        dec = memLoc.substr(0, memLoc.length() - 1);
                    } else {
                        dec = memLoc;
                    }
                    if (stoi(dec, 0, 10) >= (2 << 15)) {
                        cout << "Error";
                        return 1;
                    } else {
                        memByte = stoi(dec, 0, 10);
                    }
                }
                    //reg
                else if (find(wordRegs.begin(), wordRegs.end(), memLoc) != wordRegs.end()) {
                    if (memLoc.compare("ax") == 0) {
                        memByte = *pax;
                    } else if (memLoc.compare("bx") == 0) {
                        memByte = *pbx;
                    } else if (memLoc.compare("cx") == 0) {
                        memByte = *pcx;
                    } else if (memLoc.compare("dx") == 0) {
                        memByte = *pdx;
                    } else if (memLoc.compare("di") == 0) {
                        memByte = *pdi;
                    } else if (memLoc.compare("si") == 0) {
                        memByte = *psi;
                    } else if (memLoc.compare("sp") == 0) {
                        memByte = *psp;
                    } else if (memLoc.compare("bp") == 0) {
                        memByte = *pbp;
                    }
                }
                mul_mem_byte(memByte);
                continue;
            }
                //first operand word memory
            else if(ins.operand1[0] == 'w' && ins.operand1[1] == '[') {
                string memLoc = ins.operand1.substr(2, ins.operand1.length() - 1);
                unsigned short memByte;
                unsigned char *preg2;
                unsigned short op2;
                //hex
                if (memLoc[0] == '0' || memLoc[memLoc.length() - 1] == 'h') {
                    string hex;
                    if (memLoc[memLoc.length() - 1] == 'h') {
                        hex = memLoc.substr(0, memLoc.length() - 1);
                    } else {
                        hex = memLoc;
                    }
                    if (stoi(hex, 0, 16) >= (2 << 15)) {
                        cout << "Error";
                        return 1;
                    } else {
                        memByte = stoi(hex, 0, 16);
                    }
                }
                    //dec
                else if (memLoc[0] == '1' || memLoc[0] == '2' || memLoc[0] == '3' || memLoc[0] == '4' ||
                         memLoc[0] == '5' || memLoc[0] == '6' ||
                         memLoc[0] == '7' || memLoc[0] == '8' || memLoc[0] == '9' ||
                         memLoc[memLoc.length() - 1] == 'd') {
                    string dec;
                    if (memLoc[memLoc.length() - 1] == 'd') {
                        dec = memLoc.substr(0, memLoc.length() - 1);
                    } else {
                        dec = memLoc;
                    }
                    if (stoi(dec, 0, 10) >= (2 << 15)) {
                        cout << "Error";
                        return 1;
                    } else {
                        memByte = stoi(dec, 0, 10);
                    }
                }
                    //reg
                else if (find(wordRegs.begin(), wordRegs.end(), memLoc) != wordRegs.end()) {
                    if (memLoc.compare("ax") == 0) {
                        memByte = *pax;
                    } else if (memLoc.compare("bx") == 0) {
                        memByte = *pbx;
                    } else if (memLoc.compare("cx") == 0) {
                        memByte = *pcx;
                    } else if (memLoc.compare("dx") == 0) {
                        memByte = *pdx;
                    } else if (memLoc.compare("di") == 0) {
                        memByte = *pdi;
                    } else if (memLoc.compare("si") == 0) {
                        memByte = *psi;
                    } else if (memLoc.compare("sp") == 0) {
                        memByte = *psp;
                    } else if (memLoc.compare("bp") == 0) {
                        memByte = *pbp;
                    }
                }
                mul_mem_word(memByte);
                continue;
            }
        }
        else if(ins.name.compare("div") == 0) {
            //firsrt operand 8 bit reg
            if (find(byteRegs.begin(), byteRegs.end(), ins.operand1) != byteRegs.end()) {
                unsigned char *preg;
                unsigned char *preg2;
                unsigned char op2;
                unsigned short mop2;
                if (ins.operand1.compare("al") == 0) {
                    preg = pal;
                } else if (ins.operand1.compare("ah") == 0) {
                    preg = pah;
                } else if (ins.operand1.compare("bl") == 0) {
                    preg = pbl;
                } else if (ins.operand1.compare("bh") == 0) {
                    preg = pbh;
                } else if (ins.operand1.compare("cl") == 0) {
                    preg = pcl;
                } else if (ins.operand1.compare("ch") == 0) {
                    preg = pch;
                } else if (ins.operand1.compare("dl") == 0) {
                    preg = pdl;
                } else if (ins.operand1.compare("dh") == 0) {
                    preg = pdh;
                }
                if(div_breg(preg)==0){
                    cout<<"Error";
                    return 1;
                }
                continue;
            }
                //firsrt operand 16 bit reg
            else if (find(wordRegs.begin(), wordRegs.end(), ins.operand1) != wordRegs.end()) {
                unsigned short *preg;
                unsigned short *preg2;
                unsigned short op2;
                if (ins.operand1.compare("ax") == 0) {
                    preg = pax;
                } else if (ins.operand1.compare("bx") == 0) {
                    preg = pbx;
                } else if (ins.operand1.compare("cx") == 0) {
                    preg = pcx;
                } else if (ins.operand1.compare("dx") == 0) {
                    preg = pdx;
                } else if (ins.operand1.compare("di") == 0) {
                    preg = pdi;
                } else if (ins.operand1.compare("si") == 0) {
                    preg = psi;
                } else if (ins.operand1.compare("sp") == 0) {
                    preg = psp;
                } else if (ins.operand1.compare("bp") == 0) {
                    preg = pbp;
                }
                if(div_wreg(preg)==0){
                    cout<<"Error";
                    return 1;
                }
                continue;
            }
                //first byte operand memory
            else if (ins.operand1[0] == 'b' && ins.operand1[1] == '[') {
                string memLoc = ins.operand1.substr(2, ins.operand1.length() - 1);
                unsigned short memByte;
                unsigned char *preg2;
                unsigned char op2;
                //hex
                if (memLoc[0] == '0' || memLoc[memLoc.length() - 1] == 'h') {
                    string hex;
                    if (memLoc[memLoc.length() - 1] == 'h') {
                        hex = memLoc.substr(0, memLoc.length() - 1);
                    } else {
                        hex = memLoc;
                    }
                    if (stoi(hex, 0, 16) >= (2 << 15)) {
                        cout << "Error";
                        return 1;
                    } else {
                        memByte = stoi(hex, 0, 16);
                    }
                }
                    //dec
                else if (memLoc[0] == '1' || memLoc[0] == '2' || memLoc[0] == '3' || memLoc[0] == '4' ||
                         memLoc[0] == '5' || memLoc[0] == '6' ||
                         memLoc[0] == '7' || memLoc[0] == '8' || memLoc[0] == '9' ||
                         memLoc[memLoc.length() - 1] == 'd') {
                    string dec;
                    if (memLoc[memLoc.length() - 1] == 'd') {
                        dec = memLoc.substr(0, memLoc.length() - 1);
                    } else {
                        dec = memLoc;
                    }
                    if (stoi(dec, 0, 10) >= (2 << 15)) {
                        cout << "Error";
                        return 1;
                    } else {
                        memByte = stoi(dec, 0, 10);
                    }
                }
                    //reg
                else if (find(wordRegs.begin(), wordRegs.end(), memLoc) != wordRegs.end()) {
                    if (memLoc.compare("ax") == 0) {
                        memByte = *pax;
                    } else if (memLoc.compare("bx") == 0) {
                        memByte = *pbx;
                    } else if (memLoc.compare("cx") == 0) {
                        memByte = *pcx;
                    } else if (memLoc.compare("dx") == 0) {
                        memByte = *pdx;
                    } else if (memLoc.compare("di") == 0) {
                        memByte = *pdi;
                    } else if (memLoc.compare("si") == 0) {
                        memByte = *psi;
                    } else if (memLoc.compare("sp") == 0) {
                        memByte = *psp;
                    } else if (memLoc.compare("bp") == 0) {
                        memByte = *pbp;
                    }
                }
                if(div_mem_byte(memByte)==0){
                    cout<<"Error";
                    return 1;
                };
                continue;
            }
                //first operand word memory
            else if(ins.operand1[0] == 'w' && ins.operand1[1] == '[') {
                string memLoc = ins.operand1.substr(2, ins.operand1.length() - 1);
                unsigned short memByte;
                unsigned char *preg2;
                unsigned short op2;
                //hex
                if (memLoc[0] == '0' || memLoc[memLoc.length() - 1] == 'h') {
                    string hex;
                    if (memLoc[memLoc.length() - 1] == 'h') {
                        hex = memLoc.substr(0, memLoc.length() - 1);
                    } else {
                        hex = memLoc;
                    }
                    if (stoi(hex, 0, 16) >= (2 << 15)) {
                        cout << "Error";
                        return 1;
                    } else {
                        memByte = stoi(hex, 0, 16);
                    }
                }
                    //dec
                else if (memLoc[0] == '1' || memLoc[0] == '2' || memLoc[0] == '3' || memLoc[0] == '4' ||
                         memLoc[0] == '5' || memLoc[0] == '6' ||
                         memLoc[0] == '7' || memLoc[0] == '8' || memLoc[0] == '9' ||
                         memLoc[memLoc.length() - 1] == 'd') {
                    string dec;
                    if (memLoc[memLoc.length() - 1] == 'd') {
                        dec = memLoc.substr(0, memLoc.length() - 1);
                    } else {
                        dec = memLoc;
                    }
                    if (stoi(dec, 0, 10) >= (2 << 15)) {
                        cout << "Error";
                        return 1;
                    } else {
                        memByte = stoi(dec, 0, 10);
                    }
                }
                    //reg
                else if (find(wordRegs.begin(), wordRegs.end(), memLoc) != wordRegs.end()) {
                    if (memLoc.compare("ax") == 0) {
                        memByte = *pax;
                    } else if (memLoc.compare("bx") == 0) {
                        memByte = *pbx;
                    } else if (memLoc.compare("cx") == 0) {
                        memByte = *pcx;
                    } else if (memLoc.compare("dx") == 0) {
                        memByte = *pdx;
                    } else if (memLoc.compare("di") == 0) {
                        memByte = *pdi;
                    } else if (memLoc.compare("si") == 0) {
                        memByte = *psi;
                    } else if (memLoc.compare("sp") == 0) {
                        memByte = *psp;
                    } else if (memLoc.compare("bp") == 0) {
                        memByte = *pbp;
                    }
                }
                if(div_mem_word(memByte)==0) {
                    cout<<"Error";
                    return 1;
                }
                continue;
            }
        }
        else if(ins.name.compare("shr") == 0) {
//firsrt operand 8 bit reg
            if (find(byteRegs.begin(), byteRegs.end(), ins.operand1) != byteRegs.end()) {
                unsigned char *preg;
                unsigned char *preg2;
                unsigned short op2;
                unsigned short mop2;
                if (ins.operand1.compare("al") == 0) {
                    preg = pal;
                } else if (ins.operand1.compare("ah") == 0) {
                    preg = pah;
                } else if (ins.operand1.compare("bl") == 0) {
                    preg = pbl;
                } else if (ins.operand1.compare("bh") == 0) {
                    preg = pbh;
                } else if (ins.operand1.compare("cl") == 0) {
                    preg = pcl;
                } else if (ins.operand1.compare("ch") == 0) {
                    preg = pch;
                } else if (ins.operand1.compare("dl") == 0) {
                    preg = pdl;
                } else if (ins.operand1.compare("dh") == 0) {
                    preg = pdh;
                }
                //second op hex
                if (ins.operand2[0] == '0' || ins.operand2[ins.operand2.length() - 1] == 'h') {//second operand hex
                    if (ins.operand2[ins.operand2.length() - 1] == 'h') {
                        string hex = ins.operand2.substr(0, ins.operand2.length() - 1);
                        op2 = stoi(hex, 0, 16);
                    } else {
                        op2 = stoi(ins.operand2, 0, 16);
                    }
                    shr_byte_reg_var(preg, op2);
                    continue;
                }
                    //second op dec
                else if (ins.operand2[0] == '1' || ins.operand2[0] == '2' || ins.operand2[0] == '3' ||
                         ins.operand2[0] == '4' || ins.operand2[0] == '5' || ins.operand2[0] == '6' ||
                         ins.operand2[0] == '7' || ins.operand2[0] == '8' || ins.operand2[0] == '9' ||
                         ins.operand2[ins.operand2.length() - 1] == 'd') {
                    if (ins.operand2[ins.operand2.length() - 1] == 'd') {
                        string dec = ins.operand2.substr(0, ins.operand2.length() - 1);
                        op2 = stoi(dec, 0, 10);
                    } else {
                        op2 = stoi(ins.operand2, 0, 10);
                    }
                    shr_byte_reg_var(preg, op2);
                    continue;
                }
                    //second op reg
                else if (find(byteRegs.begin(), byteRegs.end(), ins.operand2) != byteRegs.end()) {
                    if (ins.operand2.compare("al") == 0) {
                        preg2 = pal;
                    } else if (ins.operand2.compare("ah") == 0) {
                        preg2 = pah;
                    } else if (ins.operand2.compare("bl") == 0) {
                        preg2 = pbl;
                    } else if (ins.operand2.compare("bh") == 0) {
                        preg2 = pbh;
                    } else if (ins.operand2.compare("cl") == 0) {
                        preg2 = pcl;
                    } else if (ins.operand2.compare("ch") == 0) {
                        preg2 = pch;
                    } else if (ins.operand2.compare("dl") == 0) {
                        preg2 = pbl;
                    } else if (ins.operand2.compare("dh") == 0) {
                        preg2 = pdh;
                    }
                    shr_byte_reg_cl(preg);
                    continue;
                }
            }
                //firsrt operand 16 bit reg
            else if (find(wordRegs.begin(), wordRegs.end(), ins.operand1) != wordRegs.end()) {
                unsigned short *preg;
                unsigned short *preg2;
                unsigned short op2;
                if (ins.operand1.compare("ax") == 0) {
                    preg = pax;
                } else if (ins.operand1.compare("bx") == 0) {
                    preg = pbx;
                } else if (ins.operand1.compare("cx") == 0) {
                    preg = pcx;
                } else if (ins.operand1.compare("dx") == 0) {
                    preg = pdx;
                } else if (ins.operand1.compare("di") == 0) {
                    preg = pdi;
                } else if (ins.operand1.compare("si") == 0) {
                    preg = psi;
                } else if (ins.operand1.compare("sp") == 0) {
                    preg = psp;
                } else if (ins.operand1.compare("bp") == 0) {
                    preg = pbp;
                }
                //second operand hex
                if (ins.operand2[0] == '0' || ins.operand2[ins.operand2.length() - 1] == 'h') {
                    if (ins.operand2[ins.operand2.length() - 1] == 'h') {
                        string hex = ins.operand2.substr(0, ins.operand2.length() - 1);
                        op2 = stoi(hex, 0, 16);
                    } else {
                        op2 = stoi(ins.operand2, 0, 16);
                    }
                    shr_word_reg_var(preg, op2);
                    continue;
                }
                    //second op dec
                else if (ins.operand2[0] == '1' || ins.operand2[0] == '2' || ins.operand2[0] == '3' ||
                         ins.operand2[0] == '4' || ins.operand2[0] == '5' || ins.operand2[0] == '6' ||
                         ins.operand2[0] == '7' || ins.operand2[0] == '8' || ins.operand2[0] == '9' ||
                         ins.operand2[ins.operand2.length() - 1] == 'd') {
                    if (ins.operand2[ins.operand2.length() - 1] == 'd') {
                        string dec = ins.operand2.substr(0, ins.operand2.length() - 1);
                        op2 = stoi(dec, 0, 10);
                    } else {
                        op2 = stoi(ins.operand2, 0, 10);
                    }
                    shr_word_reg_var(preg, op2);
                    continue;
                }
                    //second op reg
                else if (find(wordRegs.begin(), wordRegs.end(), ins.operand2) != wordRegs.end()) {
                    if (ins.operand2.compare("ax") == 0) {
                        preg2 = pax;
                    } else if (ins.operand2.compare("bx") == 0) {
                        preg2 = pbx;
                    } else if (ins.operand2.compare("cx") == 0) {
                        preg2 = pcx;
                    } else if (ins.operand2.compare("dx") == 0) {
                        preg2 = pdx;
                    } else if (ins.operand2.compare("di") == 0) {
                        preg2 = pdi;
                    } else if (ins.operand2.compare("si") == 0) {
                        preg2 = psi;
                    } else if (ins.operand2.compare("sp") == 0) {
                        preg2 = psp;
                    } else if (ins.operand2.compare("bp") == 0) {
                        preg2 = pbp;
                    }
                    shr_word_reg_cl(preg);
                    continue;
                }

            }
                //first byte operand memory
            else if (ins.operand1[0] == 'b' && ins.operand1[1] == '[') {
                string memLoc = ins.operand1.substr(2, ins.operand1.length() - 1);
                unsigned short memByte;
                unsigned char *preg2;
                unsigned short op2;
                //hex
                if (memLoc[0] == '0' || memLoc[memLoc.length() - 1] == 'h') {
                    string hex;
                    if (memLoc[memLoc.length() - 1] == 'h') {
                        hex = memLoc.substr(0, memLoc.length() - 1);
                    } else {
                        hex = memLoc;
                    }
                    if (stoi(hex, 0, 16) >= (2 << 15)) {
                        cout << "Error";
                        return 1;
                    } else {
                        memByte = stoi(hex, 0, 16);
                    }
                }
                    //dec
                else if (memLoc[0] == '1' || memLoc[0] == '2' || memLoc[0] == '3' || memLoc[0] == '4' ||
                         memLoc[0] == '5' || memLoc[0] == '6' ||
                         memLoc[0] == '7' || memLoc[0] == '8' || memLoc[0] == '9' ||
                         memLoc[memLoc.length() - 1] == 'd') {
                    string dec;
                    if (memLoc[memLoc.length() - 1] == 'd') {
                        dec = memLoc.substr(0, memLoc.length() - 1);
                    } else {
                        dec = memLoc;
                    }
                    if (stoi(dec, 0, 10) >= (2 << 15)) {
                        cout << "Error";
                        return 1;
                    } else {
                        memByte = stoi(dec, 0, 10);
                    }
                }
                    //reg
                else if (find(wordRegs.begin(), wordRegs.end(), memLoc) != wordRegs.end()) {
                    if (memLoc.compare("ax") == 0) {
                        memByte = *pax;
                    } else if (memLoc.compare("bx") == 0) {
                        memByte = *pbx;
                    } else if (memLoc.compare("cx") == 0) {
                        memByte = *pcx;
                    } else if (memLoc.compare("dx") == 0) {
                        memByte = *pdx;
                    } else if (memLoc.compare("di") == 0) {
                        memByte = *pdi;
                    } else if (memLoc.compare("si") == 0) {
                        memByte = *psi;
                    } else if (memLoc.compare("sp") == 0) {
                        memByte = *psp;
                    } else if (memLoc.compare("bp") == 0) {
                        memByte = *pbp;
                    }
                }
                //second operand hex
                if (ins.operand2[0] == '0' || ins.operand2[ins.operand2.length() - 1] == 'h') {//second operand hex
                    if (ins.operand2[ins.operand2.length() - 1] == 'h') {
                        string hex = ins.operand2.substr(0, ins.operand2.length() - 1);
                        op2 = stoi(hex, 0, 16);
                    } else {
                        op2 = stoi(ins.operand2, 0, 16);
                    }
                    shr_byte_mem_var(memByte, op2);
                    continue;
                }
                    //second operand dec
                else if (ins.operand2[0] == '1' || ins.operand2[0] == '2' || ins.operand2[0] == '3' ||
                         ins.operand2[0] == '4' || ins.operand2[0] == '5' || ins.operand2[0] == '6' ||
                         ins.operand2[0] == '7' || ins.operand2[0] == '8' || ins.operand2[0] == '9' ||
                         ins.operand2[ins.operand2.length() - 1] == 'd') {
                    if (ins.operand2[ins.operand2.length() - 1] == 'd') {
                        string dec = ins.operand2.substr(0, ins.operand2.length() - 1);
                        op2 = stoi(dec, 0, 10);
                    } else {
                        op2 = stoi(ins.operand2, 0, 10);
                    }
                    shr_byte_mem_var(memByte, op2);
                    continue;
                }
                    //second operand reg
                else if (find(byteRegs.begin(), byteRegs.end(), ins.operand2) != byteRegs.end()) {
                    if (ins.operand2.compare("al") == 0) {
                        preg2 = pal;
                    } else if (ins.operand2.compare("ah") == 0) {
                        preg2 = pah;
                    } else if (ins.operand2.compare("bl") == 0) {
                        preg2 = pbl;
                    } else if (ins.operand2.compare("bh") == 0) {
                        preg2 = pbh;
                    } else if (ins.operand2.compare("cl") == 0) {
                        preg2 = pcl;
                    } else if (ins.operand2.compare("ch") == 0) {
                        preg2 = pch;
                    } else if (ins.operand2.compare("dl") == 0) {
                        preg2 = pbl;
                    } else if (ins.operand2.compare("dh") == 0) {
                        preg2 = pdh;
                    }
                    shr_byte_mem_cl(memByte);
                    continue;
                }
            }
                //first operand word memory
            else if(ins.operand1[0] == 'w' && ins.operand1[1] == '[') {
                string memLoc = ins.operand1.substr(2, ins.operand1.length() - 1);
                unsigned short memByte;
                unsigned char *preg2;
                unsigned short op2;
                //hex
                if (memLoc[0] == '0' || memLoc[memLoc.length() - 1] == 'h') {
                    string hex;
                    if (memLoc[memLoc.length() - 1] == 'h') {
                        hex = memLoc.substr(0, memLoc.length() - 1);
                    } else {
                        hex = memLoc;
                    }
                    if (stoi(hex, 0, 16) >= (2 << 15)) {
                        cout << "Error";
                        return 1;
                    } else {
                        memByte = stoi(hex, 0, 16);
                    }
                }
                    //dec
                else if (memLoc[0] == '1' || memLoc[0] == '2' || memLoc[0] == '3' || memLoc[0] == '4' ||
                         memLoc[0] == '5' || memLoc[0] == '6' ||
                         memLoc[0] == '7' || memLoc[0] == '8' || memLoc[0] == '9' ||
                         memLoc[memLoc.length() - 1] == 'd') {
                    string dec;
                    if (memLoc[memLoc.length() - 1] == 'd') {
                        dec = memLoc.substr(0, memLoc.length() - 1);
                    } else {
                        dec = memLoc;
                    }
                    if (stoi(dec, 0, 10) >= (2 << 15)) {
                        cout << "Error";
                        return 1;
                    } else {
                        memByte = stoi(dec, 0, 10);
                    }
                }
                    //reg
                else if (find(wordRegs.begin(), wordRegs.end(), memLoc) != wordRegs.end()) {
                    if (memLoc.compare("ax") == 0) {
                        memByte = *pax;
                    } else if (memLoc.compare("bx") == 0) {
                        memByte = *pbx;
                    } else if (memLoc.compare("cx") == 0) {
                        memByte = *pcx;
                    } else if (memLoc.compare("dx") == 0) {
                        memByte = *pdx;
                    } else if (memLoc.compare("di") == 0) {
                        memByte = *pdi;
                    } else if (memLoc.compare("si") == 0) {
                        memByte = *psi;
                    } else if (memLoc.compare("sp") == 0) {
                        memByte = *psp;
                    } else if (memLoc.compare("bp") == 0) {
                        memByte = *pbp;
                    }
                }
                //second operand hex
                if (ins.operand2[0] == '0' || ins.operand2[ins.operand2.length() - 1] == 'h') {
                    if (ins.operand2[ins.operand2.length() - 1] == 'h') {
                        string hex = ins.operand2.substr(0, ins.operand2.length() - 1);
                        op2 = stoi(hex, 0, 16);
                    } else {
                        op2 = stoi(ins.operand2, 0, 16);
                    }
                    shr_word_mem_var(memByte, op2);
                    continue;
                }
                    //second operand dec
                else if (ins.operand2[0] == '1' || ins.operand2[0] == '2' || ins.operand2[0] == '3' ||
                         ins.operand2[0] == '4' || ins.operand2[0] == '5' || ins.operand2[0] == '6' ||
                         ins.operand2[0] == '7' || ins.operand2[0] == '8' || ins.operand2[0] == '9' ||
                         ins.operand2[ins.operand2.length() - 1] == 'd') {
                    if (ins.operand2[ins.operand2.length() - 1] == 'd') {
                        string dec = ins.operand2.substr(0, ins.operand2.length() - 1);
                        op2 = stoi(dec, 0, 10);
                    } else {
                        op2 = stoi(ins.operand2, 0, 10);
                    }
                    shr_word_mem_var(memByte, op2);
                    continue;
                }
                    //second operand reg
                else if (find(byteRegs.begin(), byteRegs.end(), ins.operand2) != byteRegs.end()) {
                    if (ins.operand2.compare("al") == 0) {
                        preg2 = pal;
                    } else if (ins.operand2.compare("ah") == 0) {
                        preg2 = pah;
                    } else if (ins.operand2.compare("bl") == 0) {
                        preg2 = pbl;
                    } else if (ins.operand2.compare("bh") == 0) {
                        preg2 = pbh;
                    } else if (ins.operand2.compare("cl") == 0) {
                        preg2 = pcl;
                    } else if (ins.operand2.compare("ch") == 0) {
                        preg2 = pch;
                    } else if (ins.operand2.compare("dl") == 0) {
                        preg2 = pbl;
                    } else if (ins.operand2.compare("dh") == 0) {
                        preg2 = pdh;
                    }
                    shr_word_mem_cl(memByte);
                    continue;
                }
            }
        }
        else if(ins.name.compare("shl") == 0) {
//firsrt operand 8 bit reg
            if (find(byteRegs.begin(), byteRegs.end(), ins.operand1) != byteRegs.end()) {
                unsigned char *preg;
                unsigned char *preg2;
                unsigned short op2;
                unsigned short mop2;
                if (ins.operand1.compare("al") == 0) {
                    preg = pal;
                } else if (ins.operand1.compare("ah") == 0) {
                    preg = pah;
                } else if (ins.operand1.compare("bl") == 0) {
                    preg = pbl;
                } else if (ins.operand1.compare("bh") == 0) {
                    preg = pbh;
                } else if (ins.operand1.compare("cl") == 0) {
                    preg = pcl;
                } else if (ins.operand1.compare("ch") == 0) {
                    preg = pch;
                } else if (ins.operand1.compare("dl") == 0) {
                    preg = pdl;
                } else if (ins.operand1.compare("dh") == 0) {
                    preg = pdh;
                }
                //second op hex
                if (ins.operand2[0] == '0' || ins.operand2[ins.operand2.length() - 1] == 'h') {//second operand hex
                    if (ins.operand2[ins.operand2.length() - 1] == 'h') {
                        string hex = ins.operand2.substr(0, ins.operand2.length() - 1);
                        op2 = stoi(hex, 0, 16);
                    } else {
                        op2 = stoi(ins.operand2, 0, 16);
                    }
                    shl_byte_reg_var(preg, op2);
                    continue;
                }
                    //second op dec
                else if (ins.operand2[0] == '1' || ins.operand2[0] == '2' || ins.operand2[0] == '3' ||
                         ins.operand2[0] == '4' || ins.operand2[0] == '5' || ins.operand2[0] == '6' ||
                         ins.operand2[0] == '7' || ins.operand2[0] == '8' || ins.operand2[0] == '9' ||
                         ins.operand2[ins.operand2.length() - 1] == 'd') {
                    if (ins.operand2[ins.operand2.length() - 1] == 'd') {
                        string dec = ins.operand2.substr(0, ins.operand2.length() - 1);
                        op2 = stoi(dec, 0, 10);
                    } else {
                        op2 = stoi(ins.operand2, 0, 10);
                    }
                    shl_byte_reg_var(preg, op2);
                    continue;
                }
                    //second op reg
                else if (find(byteRegs.begin(), byteRegs.end(), ins.operand2) != byteRegs.end()) {
                    if (ins.operand2.compare("al") == 0) {
                        preg2 = pal;
                    } else if (ins.operand2.compare("ah") == 0) {
                        preg2 = pah;
                    } else if (ins.operand2.compare("bl") == 0) {
                        preg2 = pbl;
                    } else if (ins.operand2.compare("bh") == 0) {
                        preg2 = pbh;
                    } else if (ins.operand2.compare("cl") == 0) {
                        preg2 = pcl;
                    } else if (ins.operand2.compare("ch") == 0) {
                        preg2 = pch;
                    } else if (ins.operand2.compare("dl") == 0) {
                        preg2 = pbl;
                    } else if (ins.operand2.compare("dh") == 0) {
                        preg2 = pdh;
                    }
                    shl_byte_reg_cl(preg);
                    continue;
                }
            }
                //firsrt operand 16 bit reg
            else if (find(wordRegs.begin(), wordRegs.end(), ins.operand1) != wordRegs.end()) {
                unsigned short *preg;
                unsigned short *preg2;
                unsigned short op2;
                if (ins.operand1.compare("ax") == 0) {
                    preg = pax;
                } else if (ins.operand1.compare("bx") == 0) {
                    preg = pbx;
                } else if (ins.operand1.compare("cx") == 0) {
                    preg = pcx;
                } else if (ins.operand1.compare("dx") == 0) {
                    preg = pdx;
                } else if (ins.operand1.compare("di") == 0) {
                    preg = pdi;
                } else if (ins.operand1.compare("si") == 0) {
                    preg = psi;
                } else if (ins.operand1.compare("sp") == 0) {
                    preg = psp;
                } else if (ins.operand1.compare("bp") == 0) {
                    preg = pbp;
                }
                //second operand hex
                if (ins.operand2[0] == '0' || ins.operand2[ins.operand2.length() - 1] == 'h') {
                    if (ins.operand2[ins.operand2.length() - 1] == 'h') {
                        string hex = ins.operand2.substr(0, ins.operand2.length() - 1);
                        op2 = stoi(hex, 0, 16);
                    } else {
                        op2 = stoi(ins.operand2, 0, 16);
                    }
                    shl_word_reg_var(preg, op2);
                    continue;
                }
                    //second op dec
                else if (ins.operand2[0] == '1' || ins.operand2[0] == '2' || ins.operand2[0] == '3' ||
                         ins.operand2[0] == '4' || ins.operand2[0] == '5' || ins.operand2[0] == '6' ||
                         ins.operand2[0] == '7' || ins.operand2[0] == '8' || ins.operand2[0] == '9' ||
                         ins.operand2[ins.operand2.length() - 1] == 'd') {
                    if (ins.operand2[ins.operand2.length() - 1] == 'd') {
                        string dec = ins.operand2.substr(0, ins.operand2.length() - 1);
                        op2 = stoi(dec, 0, 10);
                    } else {
                        op2 = stoi(ins.operand2, 0, 10);
                    }
                    shl_word_reg_var(preg, op2);
                    continue;
                }
                    //second op reg
                else if (find(wordRegs.begin(), wordRegs.end(), ins.operand2) != wordRegs.end()) {
                    if (ins.operand2.compare("ax") == 0) {
                        preg2 = pax;
                    } else if (ins.operand2.compare("bx") == 0) {
                        preg2 = pbx;
                    } else if (ins.operand2.compare("cx") == 0) {
                        preg2 = pcx;
                    } else if (ins.operand2.compare("dx") == 0) {
                        preg2 = pdx;
                    } else if (ins.operand2.compare("di") == 0) {
                        preg2 = pdi;
                    } else if (ins.operand2.compare("si") == 0) {
                        preg2 = psi;
                    } else if (ins.operand2.compare("sp") == 0) {
                        preg2 = psp;
                    } else if (ins.operand2.compare("bp") == 0) {
                        preg2 = pbp;
                    }
                    shl_word_reg_cl(preg);
                    continue;
                }

            }
                //first byte operand memory
            else if (ins.operand1[0] == 'b' && ins.operand1[1] == '[') {
                string memLoc = ins.operand1.substr(2, ins.operand1.length() - 1);
                unsigned short memByte;
                unsigned char *preg2;
                unsigned short op2;
                //hex
                if (memLoc[0] == '0' || memLoc[memLoc.length() - 1] == 'h') {
                    string hex;
                    if (memLoc[memLoc.length() - 1] == 'h') {
                        hex = memLoc.substr(0, memLoc.length() - 1);
                    } else {
                        hex = memLoc;
                    }
                    if (stoi(hex, 0, 16) >= (2 << 15)) {
                        cout << "Error";
                        return 1;
                    } else {
                        memByte = stoi(hex, 0, 16);
                    }
                }
                    //dec
                else if (memLoc[0] == '1' || memLoc[0] == '2' || memLoc[0] == '3' || memLoc[0] == '4' ||
                         memLoc[0] == '5' || memLoc[0] == '6' ||
                         memLoc[0] == '7' || memLoc[0] == '8' || memLoc[0] == '9' ||
                         memLoc[memLoc.length() - 1] == 'd') {
                    string dec;
                    if (memLoc[memLoc.length() - 1] == 'd') {
                        dec = memLoc.substr(0, memLoc.length() - 1);
                    } else {
                        dec = memLoc;
                    }
                    if (stoi(dec, 0, 10) >= (2 << 15)) {
                        cout << "Error";
                        return 1;
                    } else {
                        memByte = stoi(dec, 0, 10);
                    }
                }
                    //reg
                else if (find(wordRegs.begin(), wordRegs.end(), memLoc) != wordRegs.end()) {
                    if (memLoc.compare("ax") == 0) {
                        memByte = *pax;
                    } else if (memLoc.compare("bx") == 0) {
                        memByte = *pbx;
                    } else if (memLoc.compare("cx") == 0) {
                        memByte = *pcx;
                    } else if (memLoc.compare("dx") == 0) {
                        memByte = *pdx;
                    } else if (memLoc.compare("di") == 0) {
                        memByte = *pdi;
                    } else if (memLoc.compare("si") == 0) {
                        memByte = *psi;
                    } else if (memLoc.compare("sp") == 0) {
                        memByte = *psp;
                    } else if (memLoc.compare("bp") == 0) {
                        memByte = *pbp;
                    }
                }
                //second operand hex
                if (ins.operand2[0] == '0' || ins.operand2[ins.operand2.length() - 1] == 'h') {//second operand hex
                    if (ins.operand2[ins.operand2.length() - 1] == 'h') {
                        string hex = ins.operand2.substr(0, ins.operand2.length() - 1);
                        op2 = stoi(hex, 0, 16);
                    } else {
                        op2 = stoi(ins.operand2, 0, 16);
                    }
                    shl_byte_mem_var(memByte, op2);
                    continue;
                }
                    //second operand dec
                else if (ins.operand2[0] == '1' || ins.operand2[0] == '2' || ins.operand2[0] == '3' ||
                         ins.operand2[0] == '4' || ins.operand2[0] == '5' || ins.operand2[0] == '6' ||
                         ins.operand2[0] == '7' || ins.operand2[0] == '8' || ins.operand2[0] == '9' ||
                         ins.operand2[ins.operand2.length() - 1] == 'd') {
                    if (ins.operand2[ins.operand2.length() - 1] == 'd') {
                        string dec = ins.operand2.substr(0, ins.operand2.length() - 1);
                        op2 = stoi(dec, 0, 10);
                    } else {
                        op2 = stoi(ins.operand2, 0, 10);
                    }
                    shl_byte_mem_var(memByte, op2);
                    continue;
                }
                    //second operand reg
                else if (find(byteRegs.begin(), byteRegs.end(), ins.operand2) != byteRegs.end()) {
                    if (ins.operand2.compare("al") == 0) {
                        preg2 = pal;
                    } else if (ins.operand2.compare("ah") == 0) {
                        preg2 = pah;
                    } else if (ins.operand2.compare("bl") == 0) {
                        preg2 = pbl;
                    } else if (ins.operand2.compare("bh") == 0) {
                        preg2 = pbh;
                    } else if (ins.operand2.compare("cl") == 0) {
                        preg2 = pcl;
                    } else if (ins.operand2.compare("ch") == 0) {
                        preg2 = pch;
                    } else if (ins.operand2.compare("dl") == 0) {
                        preg2 = pbl;
                    } else if (ins.operand2.compare("dh") == 0) {
                        preg2 = pdh;
                    }
                    shl_byte_mem_cl(memByte);
                    continue;
                }
            }
                //first operand word memory
            else if(ins.operand1[0] == 'w' && ins.operand1[1] == '[') {
                string memLoc = ins.operand1.substr(2, ins.operand1.length() - 1);
                unsigned short memByte;
                unsigned char *preg2;
                unsigned short op2;
                //hex
                if (memLoc[0] == '0' || memLoc[memLoc.length() - 1] == 'h') {
                    string hex;
                    if (memLoc[memLoc.length() - 1] == 'h') {
                        hex = memLoc.substr(0, memLoc.length() - 1);
                    } else {
                        hex = memLoc;
                    }
                    if (stoi(hex, 0, 16) >= (2 << 15)) {
                        cout << "Error";
                        return 1;
                    } else {
                        memByte = stoi(hex, 0, 16);
                    }
                }
                    //dec
                else if (memLoc[0] == '1' || memLoc[0] == '2' || memLoc[0] == '3' || memLoc[0] == '4' ||
                         memLoc[0] == '5' || memLoc[0] == '6' ||
                         memLoc[0] == '7' || memLoc[0] == '8' || memLoc[0] == '9' ||
                         memLoc[memLoc.length() - 1] == 'd') {
                    string dec;
                    if (memLoc[memLoc.length() - 1] == 'd') {
                        dec = memLoc.substr(0, memLoc.length() - 1);
                    } else {
                        dec = memLoc;
                    }
                    if (stoi(dec, 0, 10) >= (2 << 15)) {
                        cout << "Error";
                        return 1;
                    } else {
                        memByte = stoi(dec, 0, 10);
                    }
                }
                    //reg
                else if (find(wordRegs.begin(), wordRegs.end(), memLoc) != wordRegs.end()) {
                    if (memLoc.compare("ax") == 0) {
                        memByte = *pax;
                    } else if (memLoc.compare("bx") == 0) {
                        memByte = *pbx;
                    } else if (memLoc.compare("cx") == 0) {
                        memByte = *pcx;
                    } else if (memLoc.compare("dx") == 0) {
                        memByte = *pdx;
                    } else if (memLoc.compare("di") == 0) {
                        memByte = *pdi;
                    } else if (memLoc.compare("si") == 0) {
                        memByte = *psi;
                    } else if (memLoc.compare("sp") == 0) {
                        memByte = *psp;
                    } else if (memLoc.compare("bp") == 0) {
                        memByte = *pbp;
                    }
                }
                //second operand hex
                if (ins.operand2[0] == '0' || ins.operand2[ins.operand2.length() - 1] == 'h') {
                    if (ins.operand2[ins.operand2.length() - 1] == 'h') {
                        string hex = ins.operand2.substr(0, ins.operand2.length() - 1);
                        op2 = stoi(hex, 0, 16);
                    } else {
                        op2 = stoi(ins.operand2, 0, 16);
                    }
                    shl_word_mem_var(memByte, op2);
                    continue;
                }
                    //second operand dec
                else if (ins.operand2[0] == '1' || ins.operand2[0] == '2' || ins.operand2[0] == '3' ||
                         ins.operand2[0] == '4' || ins.operand2[0] == '5' || ins.operand2[0] == '6' ||
                         ins.operand2[0] == '7' || ins.operand2[0] == '8' || ins.operand2[0] == '9' ||
                         ins.operand2[ins.operand2.length() - 1] == 'd') {
                    if (ins.operand2[ins.operand2.length() - 1] == 'd') {
                        string dec = ins.operand2.substr(0, ins.operand2.length() - 1);
                        op2 = stoi(dec, 0, 10);
                    } else {
                        op2 = stoi(ins.operand2, 0, 10);
                    }
                    shl_word_mem_var(memByte, op2);
                    continue;
                }
                    //second operand reg
                else if (find(byteRegs.begin(), byteRegs.end(), ins.operand2) != byteRegs.end()) {
                    if (ins.operand2.compare("al") == 0) {
                        preg2 = pal;
                    } else if (ins.operand2.compare("ah") == 0) {
                        preg2 = pah;
                    } else if (ins.operand2.compare("bl") == 0) {
                        preg2 = pbl;
                    } else if (ins.operand2.compare("bh") == 0) {
                        preg2 = pbh;
                    } else if (ins.operand2.compare("cl") == 0) {
                        preg2 = pcl;
                    } else if (ins.operand2.compare("ch") == 0) {
                        preg2 = pch;
                    } else if (ins.operand2.compare("dl") == 0) {
                        preg2 = pbl;
                    } else if (ins.operand2.compare("dh") == 0) {
                        preg2 = pdh;
                    }
                    shl_word_mem_cl(memByte);
                    continue;
                }
            }
        }
        else if(ins.name.compare("rcr") == 0) {
//firsrt operand 8 bit reg
            if (find(byteRegs.begin(), byteRegs.end(), ins.operand1) != byteRegs.end()) {
                unsigned char *preg;
                unsigned char *preg2;
                unsigned short op2;
                unsigned short mop2;
                if (ins.operand1.compare("al") == 0) {
                    preg = pal;
                } else if (ins.operand1.compare("ah") == 0) {
                    preg = pah;
                } else if (ins.operand1.compare("bl") == 0) {
                    preg = pbl;
                } else if (ins.operand1.compare("bh") == 0) {
                    preg = pbh;
                } else if (ins.operand1.compare("cl") == 0) {
                    preg = pcl;
                } else if (ins.operand1.compare("ch") == 0) {
                    preg = pch;
                } else if (ins.operand1.compare("dl") == 0) {
                    preg = pdl;
                } else if (ins.operand1.compare("dh") == 0) {
                    preg = pdh;
                }
                //second op hex
                if (ins.operand2[0] == '0' || ins.operand2[ins.operand2.length() - 1] == 'h') {//second operand hex
                    if (ins.operand2[ins.operand2.length() - 1] == 'h') {
                        string hex = ins.operand2.substr(0, ins.operand2.length() - 1);
                        op2 = stoi(hex, 0, 16);
                    } else {
                        op2 = stoi(ins.operand2, 0, 16);
                    }
                    rcr_byte_reg_var(preg, op2);
                    continue;
                }
                    //second op dec
                else if (ins.operand2[0] == '1' || ins.operand2[0] == '2' || ins.operand2[0] == '3' ||
                         ins.operand2[0] == '4' || ins.operand2[0] == '5' || ins.operand2[0] == '6' ||
                         ins.operand2[0] == '7' || ins.operand2[0] == '8' || ins.operand2[0] == '9' ||
                         ins.operand2[ins.operand2.length() - 1] == 'd') {
                    if (ins.operand2[ins.operand2.length() - 1] == 'd') {
                        string dec = ins.operand2.substr(0, ins.operand2.length() - 1);
                        op2 = stoi(dec, 0, 10);
                    } else {
                        op2 = stoi(ins.operand2, 0, 10);
                    }
                    rcr_byte_reg_var(preg, op2);
                    continue;
                }
                    //second op reg
                else if (find(byteRegs.begin(), byteRegs.end(), ins.operand2) != byteRegs.end()) {
                    if (ins.operand2.compare("al") == 0) {
                        preg2 = pal;
                    } else if (ins.operand2.compare("ah") == 0) {
                        preg2 = pah;
                    } else if (ins.operand2.compare("bl") == 0) {
                        preg2 = pbl;
                    } else if (ins.operand2.compare("bh") == 0) {
                        preg2 = pbh;
                    } else if (ins.operand2.compare("cl") == 0) {
                        preg2 = pcl;
                    } else if (ins.operand2.compare("ch") == 0) {
                        preg2 = pch;
                    } else if (ins.operand2.compare("dl") == 0) {
                        preg2 = pbl;
                    } else if (ins.operand2.compare("dh") == 0) {
                        preg2 = pdh;
                    }
                    rcr_byte_reg_cl(preg);
                    continue;
                }
            }
                //firsrt operand 16 bit reg
            else if (find(wordRegs.begin(), wordRegs.end(), ins.operand1) != wordRegs.end()) {
                unsigned short *preg;
                unsigned short *preg2;
                unsigned short op2;
                if (ins.operand1.compare("ax") == 0) {
                    preg = pax;
                } else if (ins.operand1.compare("bx") == 0) {
                    preg = pbx;
                } else if (ins.operand1.compare("cx") == 0) {
                    preg = pcx;
                } else if (ins.operand1.compare("dx") == 0) {
                    preg = pdx;
                } else if (ins.operand1.compare("di") == 0) {
                    preg = pdi;
                } else if (ins.operand1.compare("si") == 0) {
                    preg = psi;
                } else if (ins.operand1.compare("sp") == 0) {
                    preg = psp;
                } else if (ins.operand1.compare("bp") == 0) {
                    preg = pbp;
                }
                //second operand hex
                if (ins.operand2[0] == '0' || ins.operand2[ins.operand2.length() - 1] == 'h') {
                    if (ins.operand2[ins.operand2.length() - 1] == 'h') {
                        string hex = ins.operand2.substr(0, ins.operand2.length() - 1);
                        op2 = stoi(hex, 0, 16);
                    } else {
                        op2 = stoi(ins.operand2, 0, 16);
                    }
                    rcr_word_reg_var(preg, op2);
                    continue;
                }
                    //second op dec
                else if (ins.operand2[0] == '1' || ins.operand2[0] == '2' || ins.operand2[0] == '3' ||
                         ins.operand2[0] == '4' || ins.operand2[0] == '5' || ins.operand2[0] == '6' ||
                         ins.operand2[0] == '7' || ins.operand2[0] == '8' || ins.operand2[0] == '9' ||
                         ins.operand2[ins.operand2.length() - 1] == 'd') {
                    if (ins.operand2[ins.operand2.length() - 1] == 'd') {
                        string dec = ins.operand2.substr(0, ins.operand2.length() - 1);
                        op2 = stoi(dec, 0, 10);
                    } else {
                        op2 = stoi(ins.operand2, 0, 10);
                    }
                    rcr_word_reg_var(preg, op2);
                    continue;
                }
                    //second op reg
                else if (find(wordRegs.begin(), wordRegs.end(), ins.operand2) != wordRegs.end()) {
                    if (ins.operand2.compare("ax") == 0) {
                        preg2 = pax;
                    } else if (ins.operand2.compare("bx") == 0) {
                        preg2 = pbx;
                    } else if (ins.operand2.compare("cx") == 0) {
                        preg2 = pcx;
                    } else if (ins.operand2.compare("dx") == 0) {
                        preg2 = pdx;
                    } else if (ins.operand2.compare("di") == 0) {
                        preg2 = pdi;
                    } else if (ins.operand2.compare("si") == 0) {
                        preg2 = psi;
                    } else if (ins.operand2.compare("sp") == 0) {
                        preg2 = psp;
                    } else if (ins.operand2.compare("bp") == 0) {
                        preg2 = pbp;
                    }
                    rcr_word_reg_cl(preg);
                    continue;
                }

            }
                //first byte operand memory
            else if (ins.operand1[0] == 'b' && ins.operand1[1] == '[') {
                string memLoc = ins.operand1.substr(2, ins.operand1.length() - 1);
                unsigned short memByte;
                unsigned char *preg2;
                unsigned short op2;
                //hex
                if (memLoc[0] == '0' || memLoc[memLoc.length() - 1] == 'h') {
                    string hex;
                    if (memLoc[memLoc.length() - 1] == 'h') {
                        hex = memLoc.substr(0, memLoc.length() - 1);
                    } else {
                        hex = memLoc;
                    }
                    if (stoi(hex, 0, 16) >= (2 << 15)) {
                        cout << "Error";
                        return 1;
                    } else {
                        memByte = stoi(hex, 0, 16);
                    }
                }
                    //dec
                else if (memLoc[0] == '1' || memLoc[0] == '2' || memLoc[0] == '3' || memLoc[0] == '4' ||
                         memLoc[0] == '5' || memLoc[0] == '6' ||
                         memLoc[0] == '7' || memLoc[0] == '8' || memLoc[0] == '9' ||
                         memLoc[memLoc.length() - 1] == 'd') {
                    string dec;
                    if (memLoc[memLoc.length() - 1] == 'd') {
                        dec = memLoc.substr(0, memLoc.length() - 1);
                    } else {
                        dec = memLoc;
                    }
                    if (stoi(dec, 0, 10) >= (2 << 15)) {
                        cout << "Error";
                        return 1;
                    } else {
                        memByte = stoi(dec, 0, 10);
                    }
                }
                    //reg
                else if (find(wordRegs.begin(), wordRegs.end(), memLoc) != wordRegs.end()) {
                    if (memLoc.compare("ax") == 0) {
                        memByte = *pax;
                    } else if (memLoc.compare("bx") == 0) {
                        memByte = *pbx;
                    } else if (memLoc.compare("cx") == 0) {
                        memByte = *pcx;
                    } else if (memLoc.compare("dx") == 0) {
                        memByte = *pdx;
                    } else if (memLoc.compare("di") == 0) {
                        memByte = *pdi;
                    } else if (memLoc.compare("si") == 0) {
                        memByte = *psi;
                    } else if (memLoc.compare("sp") == 0) {
                        memByte = *psp;
                    } else if (memLoc.compare("bp") == 0) {
                        memByte = *pbp;
                    }
                }
                //second operand hex
                if (ins.operand2[0] == '0' || ins.operand2[ins.operand2.length() - 1] == 'h') {//second operand hex
                    if (ins.operand2[ins.operand2.length() - 1] == 'h') {
                        string hex = ins.operand2.substr(0, ins.operand2.length() - 1);
                        op2 = stoi(hex, 0, 16);
                    } else {
                        op2 = stoi(ins.operand2, 0, 16);
                    }
                    rcr_byte_mem_var(memByte, op2);
                    continue;
                }
                    //second operand dec
                else if (ins.operand2[0] == '1' || ins.operand2[0] == '2' || ins.operand2[0] == '3' ||
                         ins.operand2[0] == '4' || ins.operand2[0] == '5' || ins.operand2[0] == '6' ||
                         ins.operand2[0] == '7' || ins.operand2[0] == '8' || ins.operand2[0] == '9' ||
                         ins.operand2[ins.operand2.length() - 1] == 'd') {
                    if (ins.operand2[ins.operand2.length() - 1] == 'd') {
                        string dec = ins.operand2.substr(0, ins.operand2.length() - 1);
                        op2 = stoi(dec, 0, 10);
                    } else {
                        op2 = stoi(ins.operand2, 0, 10);
                    }
                    rcr_byte_mem_var(memByte, op2);
                    continue;
                }
                    //second operand reg
                else if (find(byteRegs.begin(), byteRegs.end(), ins.operand2) != byteRegs.end()) {
                    if (ins.operand2.compare("al") == 0) {
                        preg2 = pal;
                    } else if (ins.operand2.compare("ah") == 0) {
                        preg2 = pah;
                    } else if (ins.operand2.compare("bl") == 0) {
                        preg2 = pbl;
                    } else if (ins.operand2.compare("bh") == 0) {
                        preg2 = pbh;
                    } else if (ins.operand2.compare("cl") == 0) {
                        preg2 = pcl;
                    } else if (ins.operand2.compare("ch") == 0) {
                        preg2 = pch;
                    } else if (ins.operand2.compare("dl") == 0) {
                        preg2 = pbl;
                    } else if (ins.operand2.compare("dh") == 0) {
                        preg2 = pdh;
                    }
                    rcr_byte_mem_cl(memByte);
                    continue;
                }
            }
                //first operand word memory
            else if(ins.operand1[0] == 'w' && ins.operand1[1] == '[') {
                string memLoc = ins.operand1.substr(2, ins.operand1.length() - 1);
                unsigned short memByte;
                unsigned char *preg2;
                unsigned short op2;
                //hex
                if (memLoc[0] == '0' || memLoc[memLoc.length() - 1] == 'h') {
                    string hex;
                    if (memLoc[memLoc.length() - 1] == 'h') {
                        hex = memLoc.substr(0, memLoc.length() - 1);
                    } else {
                        hex = memLoc;
                    }
                    if (stoi(hex, 0, 16) >= (2 << 15)) {
                        cout << "Error";
                        return 1;
                    } else {
                        memByte = stoi(hex, 0, 16);
                    }
                }
                    //dec
                else if (memLoc[0] == '1' || memLoc[0] == '2' || memLoc[0] == '3' || memLoc[0] == '4' ||
                         memLoc[0] == '5' || memLoc[0] == '6' ||
                         memLoc[0] == '7' || memLoc[0] == '8' || memLoc[0] == '9' ||
                         memLoc[memLoc.length() - 1] == 'd') {
                    string dec;
                    if (memLoc[memLoc.length() - 1] == 'd') {
                        dec = memLoc.substr(0, memLoc.length() - 1);
                    } else {
                        dec = memLoc;
                    }
                    if (stoi(dec, 0, 10) >= (2 << 15)) {
                        cout << "Error";
                        return 1;
                    } else {
                        memByte = stoi(dec, 0, 10);
                    }
                }
                    //reg
                else if (find(wordRegs.begin(), wordRegs.end(), memLoc) != wordRegs.end()) {
                    if (memLoc.compare("ax") == 0) {
                        memByte = *pax;
                    } else if (memLoc.compare("bx") == 0) {
                        memByte = *pbx;
                    } else if (memLoc.compare("cx") == 0) {
                        memByte = *pcx;
                    } else if (memLoc.compare("dx") == 0) {
                        memByte = *pdx;
                    } else if (memLoc.compare("di") == 0) {
                        memByte = *pdi;
                    } else if (memLoc.compare("si") == 0) {
                        memByte = *psi;
                    } else if (memLoc.compare("sp") == 0) {
                        memByte = *psp;
                    } else if (memLoc.compare("bp") == 0) {
                        memByte = *pbp;
                    }
                }
                //second operand hex
                if (ins.operand2[0] == '0' || ins.operand2[ins.operand2.length() - 1] == 'h') {
                    if (ins.operand2[ins.operand2.length() - 1] == 'h') {
                        string hex = ins.operand2.substr(0, ins.operand2.length() - 1);
                        op2 = stoi(hex, 0, 16);
                    } else {
                        op2 = stoi(ins.operand2, 0, 16);
                    }
                    rcr_word_mem_var(memByte, op2);
                    continue;
                }
                    //second operand dec
                else if (ins.operand2[0] == '1' || ins.operand2[0] == '2' || ins.operand2[0] == '3' ||
                         ins.operand2[0] == '4' || ins.operand2[0] == '5' || ins.operand2[0] == '6' ||
                         ins.operand2[0] == '7' || ins.operand2[0] == '8' || ins.operand2[0] == '9' ||
                         ins.operand2[ins.operand2.length() - 1] == 'd') {
                    if (ins.operand2[ins.operand2.length() - 1] == 'd') {
                        string dec = ins.operand2.substr(0, ins.operand2.length() - 1);
                        op2 = stoi(dec, 0, 10);
                    } else {
                        op2 = stoi(ins.operand2, 0, 10);
                    }
                    rcr_word_mem_var(memByte, op2);
                    continue;
                }
                    //second operand reg
                else if (find(byteRegs.begin(), byteRegs.end(), ins.operand2) != byteRegs.end()) {
                    if (ins.operand2.compare("al") == 0) {
                        preg2 = pal;
                    } else if (ins.operand2.compare("ah") == 0) {
                        preg2 = pah;
                    } else if (ins.operand2.compare("bl") == 0) {
                        preg2 = pbl;
                    } else if (ins.operand2.compare("bh") == 0) {
                        preg2 = pbh;
                    } else if (ins.operand2.compare("cl") == 0) {
                        preg2 = pcl;
                    } else if (ins.operand2.compare("ch") == 0) {
                        preg2 = pch;
                    } else if (ins.operand2.compare("dl") == 0) {
                        preg2 = pbl;
                    } else if (ins.operand2.compare("dh") == 0) {
                        preg2 = pdh;
                    }
                    rcr_word_mem_cl(memByte);
                    continue;
                }
            }
        }
        else if(ins.name.compare("rcl") == 0) {
//firsrt operand 8 bit reg
            if (find(byteRegs.begin(), byteRegs.end(), ins.operand1) != byteRegs.end()) {
                unsigned char *preg;
                unsigned char *preg2;
                unsigned short op2;
                unsigned short mop2;
                if (ins.operand1.compare("al") == 0) {
                    preg = pal;
                } else if (ins.operand1.compare("ah") == 0) {
                    preg = pah;
                } else if (ins.operand1.compare("bl") == 0) {
                    preg = pbl;
                } else if (ins.operand1.compare("bh") == 0) {
                    preg = pbh;
                } else if (ins.operand1.compare("cl") == 0) {
                    preg = pcl;
                } else if (ins.operand1.compare("ch") == 0) {
                    preg = pch;
                } else if (ins.operand1.compare("dl") == 0) {
                    preg = pdl;
                } else if (ins.operand1.compare("dh") == 0) {
                    preg = pdh;
                }
                //second op hex
                if (ins.operand2[0] == '0' || ins.operand2[ins.operand2.length() - 1] == 'h') {//second operand hex
                    if (ins.operand2[ins.operand2.length() - 1] == 'h') {
                        string hex = ins.operand2.substr(0, ins.operand2.length() - 1);
                        op2 = stoi(hex, 0, 16);
                    } else {
                        op2 = stoi(ins.operand2, 0, 16);
                    }
                    rcl_byte_reg_var(preg, op2);
                    continue;
                }
                    //second op dec
                else if (ins.operand2[0] == '1' || ins.operand2[0] == '2' || ins.operand2[0] == '3' ||
                         ins.operand2[0] == '4' || ins.operand2[0] == '5' || ins.operand2[0] == '6' ||
                         ins.operand2[0] == '7' || ins.operand2[0] == '8' || ins.operand2[0] == '9' ||
                         ins.operand2[ins.operand2.length() - 1] == 'd') {
                    if (ins.operand2[ins.operand2.length() - 1] == 'd') {
                        string dec = ins.operand2.substr(0, ins.operand2.length() - 1);
                        op2 = stoi(dec, 0, 10);
                    } else {
                        op2 = stoi(ins.operand2, 0, 10);
                    }
                    rcl_byte_reg_var(preg, op2);
                    continue;
                }
                    //second op reg
                else if (find(byteRegs.begin(), byteRegs.end(), ins.operand2) != byteRegs.end()) {
                    if (ins.operand2.compare("al") == 0) {
                        preg2 = pal;
                    } else if (ins.operand2.compare("ah") == 0) {
                        preg2 = pah;
                    } else if (ins.operand2.compare("bl") == 0) {
                        preg2 = pbl;
                    } else if (ins.operand2.compare("bh") == 0) {
                        preg2 = pbh;
                    } else if (ins.operand2.compare("cl") == 0) {
                        preg2 = pcl;
                    } else if (ins.operand2.compare("ch") == 0) {
                        preg2 = pch;
                    } else if (ins.operand2.compare("dl") == 0) {
                        preg2 = pbl;
                    } else if (ins.operand2.compare("dh") == 0) {
                        preg2 = pdh;
                    }
                    rcl_byte_reg_cl(preg);
                    continue;
                }
            }
                //firsrt operand 16 bit reg
            else if (find(wordRegs.begin(), wordRegs.end(), ins.operand1) != wordRegs.end()) {
                unsigned short *preg;
                unsigned short *preg2;
                unsigned short op2;
                if (ins.operand1.compare("ax") == 0) {
                    preg = pax;
                } else if (ins.operand1.compare("bx") == 0) {
                    preg = pbx;
                } else if (ins.operand1.compare("cx") == 0) {
                    preg = pcx;
                } else if (ins.operand1.compare("dx") == 0) {
                    preg = pdx;
                } else if (ins.operand1.compare("di") == 0) {
                    preg = pdi;
                } else if (ins.operand1.compare("si") == 0) {
                    preg = psi;
                } else if (ins.operand1.compare("sp") == 0) {
                    preg = psp;
                } else if (ins.operand1.compare("bp") == 0) {
                    preg = pbp;
                }
                //second operand hex
                if (ins.operand2[0] == '0' || ins.operand2[ins.operand2.length() - 1] == 'h') {
                    if (ins.operand2[ins.operand2.length() - 1] == 'h') {
                        string hex = ins.operand2.substr(0, ins.operand2.length() - 1);
                        op2 = stoi(hex, 0, 16);
                    } else {
                        op2 = stoi(ins.operand2, 0, 16);
                    }
                    rcl_word_reg_var(preg, op2);
                    continue;
                }
                    //second op dec
                else if (ins.operand2[0] == '1' || ins.operand2[0] == '2' || ins.operand2[0] == '3' ||
                         ins.operand2[0] == '4' || ins.operand2[0] == '5' || ins.operand2[0] == '6' ||
                         ins.operand2[0] == '7' || ins.operand2[0] == '8' || ins.operand2[0] == '9' ||
                         ins.operand2[ins.operand2.length() - 1] == 'd') {
                    if (ins.operand2[ins.operand2.length() - 1] == 'd') {
                        string dec = ins.operand2.substr(0, ins.operand2.length() - 1);
                        op2 = stoi(dec, 0, 10);
                    } else {
                        op2 = stoi(ins.operand2, 0, 10);
                    }
                    rcl_word_reg_var(preg, op2);
                    continue;
                }
                    //second op reg
                else if (find(wordRegs.begin(), wordRegs.end(), ins.operand2) != wordRegs.end()) {
                    if (ins.operand2.compare("ax") == 0) {
                        preg2 = pax;
                    } else if (ins.operand2.compare("bx") == 0) {
                        preg2 = pbx;
                    } else if (ins.operand2.compare("cx") == 0) {
                        preg2 = pcx;
                    } else if (ins.operand2.compare("dx") == 0) {
                        preg2 = pdx;
                    } else if (ins.operand2.compare("di") == 0) {
                        preg2 = pdi;
                    } else if (ins.operand2.compare("si") == 0) {
                        preg2 = psi;
                    } else if (ins.operand2.compare("sp") == 0) {
                        preg2 = psp;
                    } else if (ins.operand2.compare("bp") == 0) {
                        preg2 = pbp;
                    }
                    rcl_word_reg_cl(preg);
                    continue;
                }

            }
                //first byte operand memory
            else if (ins.operand1[0] == 'b' && ins.operand1[1] == '[') {
                string memLoc = ins.operand1.substr(2, ins.operand1.length() - 1);
                unsigned short memByte;
                unsigned char *preg2;
                unsigned short op2;
                //hex
                if (memLoc[0] == '0' || memLoc[memLoc.length() - 1] == 'h') {
                    string hex;
                    if (memLoc[memLoc.length() - 1] == 'h') {
                        hex = memLoc.substr(0, memLoc.length() - 1);
                    } else {
                        hex = memLoc;
                    }
                    if (stoi(hex, 0, 16) >= (2 << 15)) {
                        cout << "Error";
                        return 1;
                    } else {
                        memByte = stoi(hex, 0, 16);
                    }
                }
                    //dec
                else if (memLoc[0] == '1' || memLoc[0] == '2' || memLoc[0] == '3' || memLoc[0] == '4' ||
                         memLoc[0] == '5' || memLoc[0] == '6' ||
                         memLoc[0] == '7' || memLoc[0] == '8' || memLoc[0] == '9' ||
                         memLoc[memLoc.length() - 1] == 'd') {
                    string dec;
                    if (memLoc[memLoc.length() - 1] == 'd') {
                        dec = memLoc.substr(0, memLoc.length() - 1);
                    } else {
                        dec = memLoc;
                    }
                    if (stoi(dec, 0, 10) >= (2 << 15)) {
                        cout << "Error";
                        return 1;
                    } else {
                        memByte = stoi(dec, 0, 10);
                    }
                }
                    //reg
                else if (find(wordRegs.begin(), wordRegs.end(), memLoc) != wordRegs.end()) {
                    if (memLoc.compare("ax") == 0) {
                        memByte = *pax;
                    } else if (memLoc.compare("bx") == 0) {
                        memByte = *pbx;
                    } else if (memLoc.compare("cx") == 0) {
                        memByte = *pcx;
                    } else if (memLoc.compare("dx") == 0) {
                        memByte = *pdx;
                    } else if (memLoc.compare("di") == 0) {
                        memByte = *pdi;
                    } else if (memLoc.compare("si") == 0) {
                        memByte = *psi;
                    } else if (memLoc.compare("sp") == 0) {
                        memByte = *psp;
                    } else if (memLoc.compare("bp") == 0) {
                        memByte = *pbp;
                    }
                }
                //second operand hex
                if (ins.operand2[0] == '0' || ins.operand2[ins.operand2.length() - 1] == 'h') {//second operand hex
                    if (ins.operand2[ins.operand2.length() - 1] == 'h') {
                        string hex = ins.operand2.substr(0, ins.operand2.length() - 1);
                        op2 = stoi(hex, 0, 16);
                    } else {
                        op2 = stoi(ins.operand2, 0, 16);
                    }
                    rcl_byte_mem_var(memByte, op2);
                    continue;
                }
                    //second operand dec
                else if (ins.operand2[0] == '1' || ins.operand2[0] == '2' || ins.operand2[0] == '3' ||
                         ins.operand2[0] == '4' || ins.operand2[0] == '5' || ins.operand2[0] == '6' ||
                         ins.operand2[0] == '7' || ins.operand2[0] == '8' || ins.operand2[0] == '9' ||
                         ins.operand2[ins.operand2.length() - 1] == 'd') {
                    if (ins.operand2[ins.operand2.length() - 1] == 'd') {
                        string dec = ins.operand2.substr(0, ins.operand2.length() - 1);
                        op2 = stoi(dec, 0, 10);
                    } else {
                        op2 = stoi(ins.operand2, 0, 10);
                    }
                    rcl_byte_mem_var(memByte, op2);
                    continue;
                }
                    //second operand reg
                else if (find(byteRegs.begin(), byteRegs.end(), ins.operand2) != byteRegs.end()) {
                    if (ins.operand2.compare("al") == 0) {
                        preg2 = pal;
                    } else if (ins.operand2.compare("ah") == 0) {
                        preg2 = pah;
                    } else if (ins.operand2.compare("bl") == 0) {
                        preg2 = pbl;
                    } else if (ins.operand2.compare("bh") == 0) {
                        preg2 = pbh;
                    } else if (ins.operand2.compare("cl") == 0) {
                        preg2 = pcl;
                    } else if (ins.operand2.compare("ch") == 0) {
                        preg2 = pch;
                    } else if (ins.operand2.compare("dl") == 0) {
                        preg2 = pbl;
                    } else if (ins.operand2.compare("dh") == 0) {
                        preg2 = pdh;
                    }
                    rcl_byte_mem_cl(memByte);
                    continue;
                }
            }
                //first operand word memory
            else if(ins.operand1[0] == 'w' && ins.operand1[1] == '[') {
                string memLoc = ins.operand1.substr(2, ins.operand1.length() - 1);
                unsigned short memByte;
                unsigned char *preg2;
                unsigned short op2;
                //hex
                if (memLoc[0] == '0' || memLoc[memLoc.length() - 1] == 'h') {
                    string hex;
                    if (memLoc[memLoc.length() - 1] == 'h') {
                        hex = memLoc.substr(0, memLoc.length() - 1);
                    } else {
                        hex = memLoc;
                    }
                    if (stoi(hex, 0, 16) >= (2 << 15)) {
                        cout << "Error";
                        return 1;
                    } else {
                        memByte = stoi(hex, 0, 16);
                    }
                }
                    //dec
                else if (memLoc[0] == '1' || memLoc[0] == '2' || memLoc[0] == '3' || memLoc[0] == '4' ||
                         memLoc[0] == '5' || memLoc[0] == '6' ||
                         memLoc[0] == '7' || memLoc[0] == '8' || memLoc[0] == '9' ||
                         memLoc[memLoc.length() - 1] == 'd') {
                    string dec;
                    if (memLoc[memLoc.length() - 1] == 'd') {
                        dec = memLoc.substr(0, memLoc.length() - 1);
                    } else {
                        dec = memLoc;
                    }
                    if (stoi(dec, 0, 10) >= (2 << 15)) {
                        cout << "Error";
                        return 1;
                    } else {
                        memByte = stoi(dec, 0, 10);
                    }
                }
                    //reg
                else if (find(wordRegs.begin(), wordRegs.end(), memLoc) != wordRegs.end()) {
                    if (memLoc.compare("ax") == 0) {
                        memByte = *pax;
                    } else if (memLoc.compare("bx") == 0) {
                        memByte = *pbx;
                    } else if (memLoc.compare("cx") == 0) {
                        memByte = *pcx;
                    } else if (memLoc.compare("dx") == 0) {
                        memByte = *pdx;
                    } else if (memLoc.compare("di") == 0) {
                        memByte = *pdi;
                    } else if (memLoc.compare("si") == 0) {
                        memByte = *psi;
                    } else if (memLoc.compare("sp") == 0) {
                        memByte = *psp;
                    } else if (memLoc.compare("bp") == 0) {
                        memByte = *pbp;
                    }
                }
                //second operand hex
                if (ins.operand2[0] == '0' || ins.operand2[ins.operand2.length() - 1] == 'h') {
                    if (ins.operand2[ins.operand2.length() - 1] == 'h') {
                        string hex = ins.operand2.substr(0, ins.operand2.length() - 1);
                        op2 = stoi(hex, 0, 16);
                    } else {
                        op2 = stoi(ins.operand2, 0, 16);
                    }
                    rcl_word_mem_var(memByte, op2);
                    continue;
                }
                    //second operand dec
                else if (ins.operand2[0] == '1' || ins.operand2[0] == '2' || ins.operand2[0] == '3' ||
                         ins.operand2[0] == '4' || ins.operand2[0] == '5' || ins.operand2[0] == '6' ||
                         ins.operand2[0] == '7' || ins.operand2[0] == '8' || ins.operand2[0] == '9' ||
                         ins.operand2[ins.operand2.length() - 1] == 'd') {
                    if (ins.operand2[ins.operand2.length() - 1] == 'd') {
                        string dec = ins.operand2.substr(0, ins.operand2.length() - 1);
                        op2 = stoi(dec, 0, 10);
                    } else {
                        op2 = stoi(ins.operand2, 0, 10);
                    }
                    rcl_word_mem_var(memByte, op2);
                    continue;
                }
                    //second operand reg
                else if (find(byteRegs.begin(), byteRegs.end(), ins.operand2) != byteRegs.end()) {
                    if (ins.operand2.compare("al") == 0) {
                        preg2 = pal;
                    } else if (ins.operand2.compare("ah") == 0) {
                        preg2 = pah;
                    } else if (ins.operand2.compare("bl") == 0) {
                        preg2 = pbl;
                    } else if (ins.operand2.compare("bh") == 0) {
                        preg2 = pbh;
                    } else if (ins.operand2.compare("cl") == 0) {
                        preg2 = pcl;
                    } else if (ins.operand2.compare("ch") == 0) {
                        preg2 = pch;
                    } else if (ins.operand2.compare("dl") == 0) {
                        preg2 = pbl;
                    } else if (ins.operand2.compare("dh") == 0) {
                        preg2 = pdh;
                    }
                    rcl_word_mem_cl(memByte);
                    continue;
                }
            }
        }
        else if(ins.name.compare("push") == 0) {
            //firsrt operand 16 bit reg
            if (find(byteRegs.begin(), byteRegs.end(), ins.operand1) != byteRegs.end()) {
                cout<<"Error";
                return 1;
            }
            if (find(wordRegs.begin(), wordRegs.end(), ins.operand1) != wordRegs.end()) {
                unsigned short *preg;
                unsigned short *preg2;
                unsigned short op2;
                if (ins.operand1.compare("ax") == 0) {
                    preg = pax;
                } else if (ins.operand1.compare("bx") == 0) {
                    preg = pbx;
                } else if (ins.operand1.compare("cx") == 0) {
                    preg = pcx;
                } else if (ins.operand1.compare("dx") == 0) {
                    preg = pdx;
                } else if (ins.operand1.compare("di") == 0) {
                    preg = pdi;
                } else if (ins.operand1.compare("si") == 0) {
                    preg = psi;
                } else if (ins.operand1.compare("sp") == 0) {
                    preg = psp;
                } else if (ins.operand1.compare("bp") == 0) {
                    preg = pbp;
                }
                push_reg(preg);
                continue;
            }

                //first operand word memory
            else if(ins.operand1[0] == 'w' && ins.operand1[1] == '[') {
                string memLoc = ins.operand1.substr(2, ins.operand1.length() - 1);
                unsigned short memByte;
                unsigned char *preg2;
                unsigned short op2;
                //hex
                if (memLoc[0] == '0' || memLoc[memLoc.length() - 1] == 'h') {
                    string hex;
                    if (memLoc[memLoc.length() - 1] == 'h') {
                        hex = memLoc.substr(0, memLoc.length() - 1);
                    } else {
                        hex = memLoc;
                    }
                    if (stoi(hex, 0, 16) >= (2 << 15)) {
                        cout << "Error";
                        return 1;
                    } else {
                        memByte = stoi(hex, 0, 16);
                    }
                }
                    //dec
                else if (memLoc[0] == '1' || memLoc[0] == '2' || memLoc[0] == '3' || memLoc[0] == '4' ||
                         memLoc[0] == '5' || memLoc[0] == '6' ||
                         memLoc[0] == '7' || memLoc[0] == '8' || memLoc[0] == '9' ||
                         memLoc[memLoc.length() - 1] == 'd') {
                    string dec;
                    if (memLoc[memLoc.length() - 1] == 'd') {
                        dec = memLoc.substr(0, memLoc.length() - 1);
                    } else {
                        dec = memLoc;
                    }
                    if (stoi(dec, 0, 10) >= (2 << 15)) {
                        cout << "Error";
                        return 1;
                    } else {
                        memByte = stoi(dec, 0, 10);
                    }
                }
                    //reg
                else if (find(wordRegs.begin(), wordRegs.end(), memLoc) != wordRegs.end()) {
                    if (memLoc.compare("ax") == 0) {
                        memByte = *pax;
                    } else if (memLoc.compare("bx") == 0) {
                        memByte = *pbx;
                    } else if (memLoc.compare("cx") == 0) {
                        memByte = *pcx;
                    } else if (memLoc.compare("dx") == 0) {
                        memByte = *pdx;
                    } else if (memLoc.compare("di") == 0) {
                        memByte = *pdi;
                    } else if (memLoc.compare("si") == 0) {
                        memByte = *psi;
                    } else if (memLoc.compare("sp") == 0) {
                        memByte = *psp;
                    } else if (memLoc.compare("bp") == 0) {
                        memByte = *pbp;
                    }
                }
                push_mem(memByte);
                continue;
            }

            else if (ins.operand1[0] == '0' || ins.operand2[ins.operand1.length() - 1] == 'h') {//second operand hex
                unsigned short op;
                if (ins.operand1[ins.operand1.length() - 1] == 'h') {
                    string hex = ins.operand1.substr(0, ins.operand1.length() - 1);
                    op = stoi(hex, 0, 16);
                } else {
                    op = stoi(ins.operand1, 0, 16);
                }
                push_var(op);
                continue;
            }
                //second op dec
            else if (ins.operand1[0] == '1' || ins.operand1[0] == '2' || ins.operand1[0] == '3' ||
                     ins.operand1[0] == '4' || ins.operand1[0] == '5' || ins.operand1[0] == '6' ||
                     ins.operand1[0] == '7' || ins.operand1[0] == '8' || ins.operand1[0] == '9' ||
                     ins.operand1[ins.operand1.length() - 1] == 'd') {
                unsigned short op;
                if (ins.operand1[ins.operand1.length() - 1] == 'd') {
                    string dec = ins.operand1.substr(0, ins.operand1.length() - 1);
                    op = stoi(dec, 0, 10);
                } else {
                    op = stoi(ins.operand1, 0, 10);
                }
                push_var(op);
                continue;
            }
            else if (find(variableNames.begin(), variableNames.end(), ins.operand1) != variableNames.end()) {
                unsigned short op;
                vector<string>::iterator it = find(variableNames.begin(), variableNames.end(), ins.operand1);
                int index = distance(variableNames.begin(), it);
                if (!variables[index].isWord) { op = variables[index].mem - 1; }
                else { op = variables[index].mem - 2; }
                push_mem(op);
            }
        }
        else if(ins.name.compare("pop") == 0) {
            //firsrt operand 16 bit reg
            if (find(wordRegs.begin(), wordRegs.end(), ins.operand1) != wordRegs.end()) {
                unsigned short *preg;
                unsigned short *preg2;
                unsigned short op2;
                if (ins.operand1.compare("ax") == 0) {
                    preg = pax;
                } else if (ins.operand1.compare("bx") == 0) {
                    preg = pbx;
                } else if (ins.operand1.compare("cx") == 0) {
                    preg = pcx;
                } else if (ins.operand1.compare("dx") == 0) {
                    preg = pdx;
                } else if (ins.operand1.compare("di") == 0) {
                    preg = pdi;
                } else if (ins.operand1.compare("si") == 0) {
                    preg = psi;
                } else if (ins.operand1.compare("sp") == 0) {
                    preg = psp;
                } else if (ins.operand1.compare("bp") == 0) {
                    preg = pbp;
                }
                pop_reg(preg);
                continue;
            }

                //first operand word memory
            else if(ins.operand1[0] == 'w' && ins.operand1[1] == '[') {
                string memLoc = ins.operand1.substr(2, ins.operand1.length() - 1);
                unsigned short memByte;
                unsigned char *preg2;
                unsigned short op2;
                //hex
                if (memLoc[0] == '0' || memLoc[memLoc.length() - 1] == 'h') {
                    string hex;
                    if (memLoc[memLoc.length() - 1] == 'h') {
                        hex = memLoc.substr(0, memLoc.length() - 1);
                    } else {
                        hex = memLoc;
                    }
                    if (stoi(hex, 0, 16) >= (2 << 15)) {
                        cout << "Error";
                        return 1;
                    } else {
                        memByte = stoi(hex, 0, 16);
                    }
                }
                    //dec
                else if (memLoc[0] == '1' || memLoc[0] == '2' || memLoc[0] == '3' || memLoc[0] == '4' ||
                         memLoc[0] == '5' || memLoc[0] == '6' ||
                         memLoc[0] == '7' || memLoc[0] == '8' || memLoc[0] == '9' ||
                         memLoc[memLoc.length() - 1] == 'd') {
                    string dec;
                    if (memLoc[memLoc.length() - 1] == 'd') {
                        dec = memLoc.substr(0, memLoc.length() - 1);
                    } else {
                        dec = memLoc;
                    }
                    if (stoi(dec, 0, 10) >= (2 << 15)) {
                        cout << "Error";
                        return 1;
                    } else {
                        memByte = stoi(dec, 0, 10);
                    }
                }
                    //reg
                else if (find(wordRegs.begin(), wordRegs.end(), memLoc) != wordRegs.end()) {
                    if (memLoc.compare("ax") == 0) {
                        memByte = *pax;
                    } else if (memLoc.compare("bx") == 0) {
                        memByte = *pbx;
                    } else if (memLoc.compare("cx") == 0) {
                        memByte = *pcx;
                    } else if (memLoc.compare("dx") == 0) {
                        memByte = *pdx;
                    } else if (memLoc.compare("di") == 0) {
                        memByte = *pdi;
                    } else if (memLoc.compare("si") == 0) {
                        memByte = *psi;
                    } else if (memLoc.compare("sp") == 0) {
                        memByte = *psp;
                    } else if (memLoc.compare("bp") == 0) {
                        memByte = *pbp;
                    }
                }
                pop_mem(memByte);
                continue;
            }

            else if (find(variableNames.begin(), variableNames.end(), ins.operand1) != variableNames.end()) {
                unsigned short op;
                vector<string>::iterator it = find(variableNames.begin(), variableNames.end(), ins.operand1);
                int index = distance(variableNames.begin(), it);
                if (!variables[index].isWord) { op = variables[index].mem - 1; }
                else { op = variables[index].mem - 2; }
                pop_mem(op);
            }
        }
        else if(ins.name.compare("not") == 0) {
            //firsrt operand 8 bit reg
            if (find(byteRegs.begin(), byteRegs.end(), ins.operand1) != byteRegs.end()) {
                unsigned char *preg;
                unsigned char *preg2;
                unsigned char op2;
                unsigned short mop2;
                if (ins.operand1.compare("al") == 0) {
                    preg = pal;
                } else if (ins.operand1.compare("ah") == 0) {
                    preg = pah;
                } else if (ins.operand1.compare("bl") == 0) {
                    preg = pbl;
                } else if (ins.operand1.compare("bh") == 0) {
                    preg = pbh;
                } else if (ins.operand1.compare("cl") == 0) {
                    preg = pcl;
                } else if (ins.operand1.compare("ch") == 0) {
                    preg = pch;
                } else if (ins.operand1.compare("dl") == 0) {
                    preg = pdl;
                } else if (ins.operand1.compare("dh") == 0) {
                    preg = pdh;
                }
                not_reg(preg);
                continue;
            }
                //firsrt operand 16 bit reg
            else if (find(wordRegs.begin(), wordRegs.end(), ins.operand1) != wordRegs.end()) {
                unsigned short *preg;
                unsigned short *preg2;
                unsigned short op2;
                if (ins.operand1.compare("ax") == 0) {
                    preg = pax;
                } else if (ins.operand1.compare("bx") == 0) {
                    preg = pbx;
                } else if (ins.operand1.compare("cx") == 0) {
                    preg = pcx;
                } else if (ins.operand1.compare("dx") == 0) {
                    preg = pdx;
                } else if (ins.operand1.compare("di") == 0) {
                    preg = pdi;
                } else if (ins.operand1.compare("si") == 0) {
                    preg = psi;
                } else if (ins.operand1.compare("sp") == 0) {
                    preg = psp;
                } else if (ins.operand1.compare("bp") == 0) {
                    preg = pbp;
                }
                not_reg(preg);
                continue;
            }
                //first byte operand memory
            else if (ins.operand1[0] == 'b' && ins.operand1[1] == '[') {
                string memLoc = ins.operand1.substr(2, ins.operand1.length() - 1);
                unsigned short memByte;
                unsigned char *preg2;
                unsigned char op2;
                //hex
                if (memLoc[0] == '0' || memLoc[memLoc.length() - 1] == 'h') {
                    string hex;
                    if (memLoc[memLoc.length() - 1] == 'h') {
                        hex = memLoc.substr(0, memLoc.length() - 1);
                    } else {
                        hex = memLoc;
                    }
                    if (stoi(hex, 0, 16) >= (2 << 15)) {
                        cout << "Error";
                        return 1;
                    } else {
                        memByte = stoi(hex, 0, 16);
                    }
                }
                    //dec
                else if (memLoc[0] == '1' || memLoc[0] == '2' || memLoc[0] == '3' || memLoc[0] == '4' ||
                         memLoc[0] == '5' || memLoc[0] == '6' ||
                         memLoc[0] == '7' || memLoc[0] == '8' || memLoc[0] == '9' ||
                         memLoc[memLoc.length() - 1] == 'd') {
                    string dec;
                    if (memLoc[memLoc.length() - 1] == 'd') {
                        dec = memLoc.substr(0, memLoc.length() - 1);
                    } else {
                        dec = memLoc;
                    }
                    if (stoi(dec, 0, 10) >= (2 << 15)) {
                        cout << "Error";
                        return 1;
                    } else {
                        memByte = stoi(dec, 0, 10);
                    }
                }
                    //reg
                else if (find(wordRegs.begin(), wordRegs.end(), memLoc) != wordRegs.end()) {
                    if (memLoc.compare("ax") == 0) {
                        memByte = *pax;
                    } else if (memLoc.compare("bx") == 0) {
                        memByte = *pbx;
                    } else if (memLoc.compare("cx") == 0) {
                        memByte = *pcx;
                    } else if (memLoc.compare("dx") == 0) {
                        memByte = *pdx;
                    } else if (memLoc.compare("di") == 0) {
                        memByte = *pdi;
                    } else if (memLoc.compare("si") == 0) {
                        memByte = *psi;
                    } else if (memLoc.compare("sp") == 0) {
                        memByte = *psp;
                    } else if (memLoc.compare("bp") == 0) {
                        memByte = *pbp;
                    }
                }
                not_mem_byte(memByte);
                continue;
            }
                //first operand word memory
            else if(ins.operand1[0] == 'w' && ins.operand1[1] == '[') {
                string memLoc = ins.operand1.substr(2, ins.operand1.length() - 1);
                unsigned short memByte;
                unsigned char *preg2;
                unsigned short op2;
                //hex
                if (memLoc[0] == '0' || memLoc[memLoc.length() - 1] == 'h') {
                    string hex;
                    if (memLoc[memLoc.length() - 1] == 'h') {
                        hex = memLoc.substr(0, memLoc.length() - 1);
                    } else {
                        hex = memLoc;
                    }
                    if (stoi(hex, 0, 16) >= (2 << 15)) {
                        cout << "Error";
                        return 1;
                    } else {
                        memByte = stoi(hex, 0, 16);
                    }
                }
                    //dec
                else if (memLoc[0] == '1' || memLoc[0] == '2' || memLoc[0] == '3' || memLoc[0] == '4' ||
                         memLoc[0] == '5' || memLoc[0] == '6' ||
                         memLoc[0] == '7' || memLoc[0] == '8' || memLoc[0] == '9' ||
                         memLoc[memLoc.length() - 1] == 'd') {
                    string dec;
                    if (memLoc[memLoc.length() - 1] == 'd') {
                        dec = memLoc.substr(0, memLoc.length() - 1);
                    } else {
                        dec = memLoc;
                    }
                    if (stoi(dec, 0, 10) >= (2 << 15)) {
                        cout << "Error";
                        return 1;
                    } else {
                        memByte = stoi(dec, 0, 10);
                    }
                }
                    //reg
                else if (find(wordRegs.begin(), wordRegs.end(), memLoc) != wordRegs.end()) {
                    if (memLoc.compare("ax") == 0) {
                        memByte = *pax;
                    } else if (memLoc.compare("bx") == 0) {
                        memByte = *pbx;
                    } else if (memLoc.compare("cx") == 0) {
                        memByte = *pcx;
                    } else if (memLoc.compare("dx") == 0) {
                        memByte = *pdx;
                    } else if (memLoc.compare("di") == 0) {
                        memByte = *pdi;
                    } else if (memLoc.compare("si") == 0) {
                        memByte = *psi;
                    } else if (memLoc.compare("sp") == 0) {
                        memByte = *psp;
                    } else if (memLoc.compare("bp") == 0) {
                        memByte = *pbp;
                    }
                }
                not_mem_word(memByte);
                continue;
            }
        }
        else if(ins.name[0] == 'j') {

            if(ins.name.compare("jz")==0) {
                vector<string>::iterator it = find(labelNames.begin(), labelNames.end(), ins.operand2);
                int index = distance(labelNames.begin(), it);
                int lineNo = labels[index].line;
                vector<int>::iterator it2 = find(instLines.begin(), instLines.end(), lineNo);
                int instIndex = distance(instLines.begin(), it2);
                if(jz()){
                    i = instIndex-1;
                }
            }
            if(ins.name.compare("jnz")==0) {
                vector<string>::iterator it = find(labelNames.begin(), labelNames.end(), ins.operand2);
                int index = distance(labelNames.begin(), it);
                int lineNo = labels[index].line;
                vector<int>::iterator it2 = find(instLines.begin(), instLines.end(), lineNo);
                int instIndex = distance(instLines.begin(), it2);
                if (jnz()) {
                    i = instIndex - 1;
                }
            }
            if(ins.name.compare("je")==0) {
                vector<string>::iterator it = find(labelNames.begin(), labelNames.end(), ins.operand2);
                int index = distance(labelNames.begin(), it);
                int lineNo = labels[index].line;
                vector<int>::iterator it2 = find(instLines.begin(), instLines.end(), lineNo);
                int instIndex = distance(instLines.begin(), it2);
                if (je()) {
                    i = instIndex - 1;
                }
            }
            if(ins.name.compare("jne")==0) {
                vector<string>::iterator it = find(labelNames.begin(), labelNames.end(), ins.operand2);
                int index = distance(labelNames.begin(), it);
                int lineNo = labels[index].line;
                vector<int>::iterator it2 = find(instLines.begin(), instLines.end(), lineNo);
                int instIndex = distance(instLines.begin(), it2);
                if (jne()) {
                    i = instIndex - 1;
                }
            }
            if(ins.name.compare("ja")==0) {
                vector<string>::iterator it = find(labelNames.begin(), labelNames.end(), ins.operand2);
                int index = distance(labelNames.begin(), it);
                int lineNo = labels[index].line;
                vector<int>::iterator it2 = find(instLines.begin(), instLines.end(), lineNo);
                int instIndex = distance(instLines.begin(), it2);
                if (ja()) {
                    i = instIndex - 1;
                }
            }
            if(ins.name.compare("jae")==0) {
                vector<string>::iterator it = find(labelNames.begin(), labelNames.end(), ins.operand2);
                int index = distance(labelNames.begin(), it);
                int lineNo = labels[index].line;
                vector<int>::iterator it2 = find(instLines.begin(), instLines.end(), lineNo);
                int instIndex = distance(instLines.begin(), it2);
                if (jae()) {
                    i = instIndex - 1;
                }
            }
            if(ins.name.compare("jb")==0) {
                vector<string>::iterator it = find(labelNames.begin(), labelNames.end(), ins.operand2);
                int index = distance(labelNames.begin(), it);
                int lineNo = labels[index].line;
                vector<int>::iterator it2 = find(instLines.begin(), instLines.end(), lineNo);
                int instIndex = distance(instLines.begin(), it2);
                if (jb()) {
                    i = instIndex - 1;
                }
            }
            if(ins.name.compare("jbe")==0) {
                vector<string>::iterator it = find(labelNames.begin(), labelNames.end(), ins.operand2);
                int index = distance(labelNames.begin(), it);
                int lineNo = labels[index].line;
                vector<int>::iterator it2 = find(instLines.begin(), instLines.end(), lineNo);
                int instIndex = distance(instLines.begin(), it2);
                if (jbe()) {
                    i = instIndex - 1;
                }
            }
            if(ins.name.compare("jnae")==0) {
                vector<string>::iterator it = find(labelNames.begin(), labelNames.end(), ins.operand2);
                int index = distance(labelNames.begin(), it);
                int lineNo = labels[index].line;
                vector<int>::iterator it2 = find(instLines.begin(), instLines.end(), lineNo);
                int instIndex = distance(instLines.begin(), it2);
                if (jnae()) {
                    i = instIndex - 1;
                }
            }
            if(ins.name.compare("jnb")==0) {
                vector<string>::iterator it = find(labelNames.begin(), labelNames.end(), ins.operand2);
                int index = distance(labelNames.begin(), it);
                int lineNo = labels[index].line;
                vector<int>::iterator it2 = find(instLines.begin(), instLines.end(), lineNo);
                int instIndex = distance(instLines.begin(), it2);
                if (jnb()) {
                    i = instIndex - 1;
                }
            }
            if(ins.name.compare("jnbe")==0) {
                vector<string>::iterator it = find(labelNames.begin(), labelNames.end(), ins.operand2);
                int index = distance(labelNames.begin(), it);
                int lineNo = labels[index].line;
                vector<int>::iterator it2 = find(instLines.begin(), instLines.end(), lineNo);
                int instIndex = distance(instLines.begin(), it2);
                if (jnbe()) {
                    i = instIndex - 1;
                }
            }
            if(ins.name.compare("jnc")==0) {
                vector<string>::iterator it = find(labelNames.begin(), labelNames.end(), ins.operand2);
                int index = distance(labelNames.begin(), it);
                int lineNo = labels[index].line;
                vector<int>::iterator it2 = find(instLines.begin(), instLines.end(), lineNo);
                int instIndex = distance(instLines.begin(), it2);
                if (jnc()) {
                    i = instIndex - 1;
                }
            }
            if(ins.name.compare("jc")==0) {
                vector<string>::iterator it = find(labelNames.begin(), labelNames.end(), ins.operand2);
                int index = distance(labelNames.begin(), it);
                int lineNo = labels[index].line;
                vector<int>::iterator it2 = find(instLines.begin(), instLines.end(), lineNo);
                int instIndex = distance(instLines.begin(), it2);
                if (jc()) {
                    i = instIndex - 1;
                }
            }
            if(ins.name.compare("jmp")==0) {
                vector<string>::iterator it = find(labelNames.begin(), labelNames.end(), ins.operand2);
                int index = distance(labelNames.begin(), it);
                int lineNo = labels[index].line;
                vector<int>::iterator it2 = find(instLines.begin(), instLines.end(), lineNo);
                int instIndex = distance(instLines.begin(), it2);
                i = instIndex - 1;
            }
        }
        else if(ins.name.compare("int21h")==0) {
            if(*pah==1) {
                string str = "a";
                getline(cin, str);
                char c = str[0];
                *pal = c;
            }
            else if(*pah==2) {
                *pal = *pdl;
                cout << *pdl;
            }
        }
        else if(ins.name.compare("int20h")==0) {
            return 0;
            break;
        }
    }
}
