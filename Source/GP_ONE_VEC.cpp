#include "GP_ONE_VEC.h"
#include <bitset>

Sprite   GP_ONE_VEC::spriteMemory[MAX_SPRITE_COUNT];
uint16_t GP_ONE_VEC::frameBuffer[FRAMEBUFFER_BUF_SIZE];


void GP_ONE_VEC::loadSprites(const Sprite *sprites, uint16_t spriteCount) {
    for(uint16_t i = 0; i < spriteCount; ++i) {
        GP_ONE_VEC::spriteMemory[i] = *(sprites + i);
    }
    
}


void GP_ONE_VEC::clearFrameBuffer(BackGroundColor bkgColor) {
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


void GP_ONE_VEC::drawSpriteInstances(const SpriteInstance *instances, uint16_t instanceCount) {
    for(uint16_t instIndx = 0; instIndx < instanceCount; ++instIndx) {
        SpriteInstance currInst = *(instances + instIndx);

        uint16_t FBoffset1, FBoffset2, sprOffset1, sprOffset2;
        uint16_t offsetTile = (currInst.x >> 4);//целочисленное деление на 16, номер тайла в ктором находится верхний угол начиная с 0
        uint16_t offsetInsideTile = currInst.x & (16 - 1);//остаток от деления на 16, на сколько пикселей сдвигаем относительно самого тайла
        uint16_t yOffset = currInst.y << 5 /* y*FRAMEBUFFER_TILES_X */;
        uint16_t wholeOffset = yOffset + offsetTile;
        
        //если нет сдвига внутри тайла - отлично, рисуем ровненько в тайлы буфера
        if(!offsetInsideTile) {
            __m128i _FBColor, _sprColor, _a, _m;
            union{__m128i _color; uint16_t colorArr[8];};
            for(uint16_t y = 0; y < SPRITE_TILES_Y; y+=2) {
                uint16_t x = 0;
                 {
                    
                    FBoffset1 = (y << 5 ) + wholeOffset;
                    FBoffset2 = ((y+1) << 5 ) + wholeOffset;

                    sprOffset1 = (y << 2);
                    sprOffset2 = ((y+1) << 2);
                    _FBColor = _mm_unpacklo_epi64(_mm_loadu_si128((const __m128i*)(frameBuffer + FBoffset1)), 
                                                  _mm_loadu_si128((const __m128i*)(frameBuffer + FBoffset2)));
                    
                    _sprColor =_mm_unpacklo_epi64(_mm_loadu_si128((const __m128i*)(spriteMemory[currInst.ind].color + sprOffset1)), 
                                                  _mm_loadu_si128((const __m128i*)(spriteMemory[currInst.ind].color + sprOffset2)));

                    _a = _mm_unpacklo_epi64(_mm_loadu_si128((const __m128i*)(spriteMemory[currInst.ind].alpha + sprOffset1)), 
                                           _mm_loadu_si128((const __m128i*)(spriteMemory[currInst.ind].alpha + sprOffset2)));

                    _color = _mm_or_si128(_mm_and_si128(_a, _sprColor), _mm_andnot_si128(_a, _FBColor));
                    frameBuffer[FBoffset1] = colorArr[0];
                    frameBuffer[FBoffset1 + 1] = colorArr[1];
                    frameBuffer[FBoffset1 + 2] = colorArr[2];
                    frameBuffer[FBoffset1 + 3] = colorArr[3];

                    frameBuffer[FBoffset2] = colorArr[4];
                    frameBuffer[FBoffset2 + 1] = colorArr[5];
                    frameBuffer[FBoffset2 + 2] = colorArr[6];
                    frameBuffer[FBoffset2 + 3] = colorArr[7];

                } 
            }
        }
        //иначе сдвигаем спрайт вдоль тайлов на оффсет
        else {
            for(uint16_t y = 0; y < SPRITE_TILES_Y; ++y) {
                for(uint16_t x = 0; x < SPRITE_TILES_X; ++x) {
                    uint16_t FBIndx = (y << 5/* y*FRAMEBUFFER_TILES_X */) + x + wholeOffset;
                    uint16_t sprIndx = (y << 2/* y*SPRITE_TILES_X */) + x;

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

void GP_ONE_VEC::saveFrameBuffer(FrameBuffer &outFrameBuffer) {
    for(uint16_t i = 0; i < FRAMEBUFFER_BUF_SIZE; ++i)
        outFrameBuffer.color[i] = frameBuffer[i];
}

