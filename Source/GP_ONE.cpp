#include "GP_ONE.h"

Sprite   GP_ONE::spriteMemory[MAX_SPRITE_COUNT];
uint16_t GP_ONE::frameBuffer[FRAMEBUFFER_BUF_SIZE];


void GP_ONE::loadSprites(const Sprite *sprites, uint16_t spriteCount) {
    for(uint16_t i = 0; i < spriteCount; ++i) {
        GP_ONE::spriteMemory[i] = *(sprites + i);
    }
    
}


void GP_ONE::clearFrameBuffer(BackGroundColor bkgColor) {
    uint16_t color;

    if(bkgColor == BackGroundColor::BLACK) {
        color = 0x0;//тайл черного цветв
    }
    else {
        color = 0xFFFF;//тайл белого цвета
    }

    for(uint16_t i = 0; i < FRAMEBUFFER_BUF_SIZE; ++i) {
        frameBuffer[i] = color;
    }
}


void GP_ONE::drawSpriteInstances(const SpriteInstance *instances, uint16_t instanceCount) {
    for(uint16_t instIndx = 0; instIndx < instanceCount; ++instIndx) {

        SpriteInstance currInst = *(instances + instIndx);
        uint16_t sIndx = currInst.ind;

        uint16_t offsetIndxRow = currInst.y;
        uint16_t offsetIndxClmn = currInst.x;

        
        
        
    }
}


void GP_ONE::saveFrameBuffer(FrameBuffer &outFrameBuffer) {

}