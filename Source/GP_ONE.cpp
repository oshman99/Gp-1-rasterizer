#include "GP_ONE.h"
#include <bitset>

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

        uint16_t offsetTile = (currInst.x >> 4);//целочисленное деление на 16, номер тайла в ктором находится верхний угол начиная с 0
        uint16_t offsetInsideTile = currInst.x & (16 - 1);//остаток от деления на 16, на сколько пикселей сдвигаем относительно самого тайла
        uint16_t yOffset = currInst.y << 5 /* y*FRAMEBUFFER_TILES_X */;

        uint16_t wholeOffset = yOffset + offsetTile;
        
        //если нет сдвига внутри тайла - отлично, рисуем ровненько в тайлы буфера
        if(!offsetInsideTile) {
            for(uint16_t j = 0; j < SPRITE_TILES_Y; ++j) {
                for(uint16_t i = 0; i < SPRITE_TILES_X; ++i) {

                    uint16_t FBIndx = (j << 5/* j*FRAMEBUFFER_TILES_X */) + i + wholeOffset;
                    uint16_t sprIndx = (j << 2/* j*SPRITE_TILES_X */) + i;

                    uint16_t FBColor = frameBuffer[FBIndx];
                    uint16_t sprColor = spriteMemory[currInst.ind].color[sprIndx];
                    uint16_t a = spriteMemory[currInst.ind].alpha[sprIndx];
                    frameBuffer[FBIndx] = (a & sprColor) | (~a & FBColor);
                } 
            }
        }
        //иначе сдвигаем спрайт вдоль тайлов на оффсет
        else {
            for(uint16_t j = 0; j < SPRITE_TILES_Y; ++j) {
                for(uint16_t i = 0; i < SPRITE_TILES_X; ++i) {
                    uint16_t FBIndx = (j << 5/* j*FRAMEBUFFER_TILES_X */) + i + wholeOffset;
                    uint16_t sprIndx = (j << 2/* j*SPRITE_TILES_X */) + i;

                    //надо 2 раза смешивать альфу, для текущего и следующего
                    uint16_t FBColor = frameBuffer[FBIndx];
                    uint16_t sprColor = spriteMemory[currInst.ind].color[sprIndx] >> offsetInsideTile;
                    uint16_t a = spriteMemory[currInst.ind].alpha[sprIndx] >> offsetInsideTile;
                    uint16_t trueColor = (a & sprColor) | (~a & FBColor);
                    frameBuffer[FBIndx] = trueColor;

                    //второй раз
                    FBColor = frameBuffer[FBIndx + 1];
                    sprColor = spriteMemory[currInst.ind].color[sprIndx] << (16 - offsetInsideTile);
                    a = spriteMemory[currInst.ind].alpha[sprIndx] << (16 - offsetInsideTile);
                    trueColor = (a & sprColor) | (~a & FBColor);
                    frameBuffer[FBIndx + 1] = trueColor;

                } 
            }
        }
    }
}

void GP_ONE::saveFrameBuffer(FrameBuffer &outFrameBuffer) {
    for(uint16_t i = 0; i < FRAMEBUFFER_BUF_SIZE; ++i)
        outFrameBuffer.color[i] = frameBuffer[i];
}


