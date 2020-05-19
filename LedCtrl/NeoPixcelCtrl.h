/*_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
*
*   概　要： NeoPixcel (WS2812B)を使用したLED点灯クラス バージョン_2 0 0
*　　　　　 ライブラリ ⇛ FastLEDを使用
*　　
*　 詳　細： effect 16個まで搭載
*　　　　　 処理自体はすべて毎回mainLoopに戻すようにしているため
*         ぐに戻ってくるようにしている（おそくても6msで戻るはず)
*         
*
 _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/*/

#ifndef NEOPIXCEL_H
    #define NEOPIXCEL_H

#define DEBUG_LED (0)

#include <FastLED.h>

enum LedStatus 
{
    led_Ready,
    led_Running,
    led_Finish
};

enum ledColors  
{ 
    cl_Black = 0, 
    cl_Red,
    cl_Orange, 
    cl_Yellow, 
    cl_Green,
    cl_Blue,
    cl_Indigo,
    cl_Pink, 
    cl_White,
    cl_Purple
};

class NeoPixcelCtrl
{
    private:
        // enum instance
        LedStatus c_status;

        ledColors col;

        CRGBPalette16 currentPalette;
        TBlendType    currentBlending;

        // gloaval instance
        uint64_t c_waitTime = 0;
        int chage_count = 0;
        int c_LedCount = 0;
        int c_loopCount = 0;
        bool cyckle = false;

        uint8_t m_Red = 250;
        uint8_t m_Green = 250;
        uint8_t m_Blue = 250;

        // private Functions
        void setAllRGB(uint8_t red, uint8_t green, uint8_t blue);
        void setHeatColor(uint8_t temperature , int heatNum);
        void SetColorPalette( uint8_t red, uint8_t green, uint8_t blue);
        void addGlitter( fract8 chanceOfGlitter);

    public:
    
        // class intstance
        CFastLED c_neoPix;
        CRGB *c_cRGB;

        void begin(CFastLED _neoLed, CRGB *_crgb, int lednum);

        void setColor(ledColors col);

        /* -----------------------------------------------
         * 概　要　：　RGBでLEDの色をセット
         * ------------------------------------------------*/
        void setRGB(uint8_t red, uint8_t green, uint8_t blue){
            m_Red = red;
            m_Green = green;
            m_Blue = blue;
        }

        void getRGB(uint8_t &red, uint8_t &green, uint8_t &blue) {
            red = m_Red;
            green = m_Green;
            blue = m_Blue;
        }

        LedStatus getStatus() { 
            return c_status; 
        }

        uint8_t getBrightless() {
            return c_neoPix.getBrightness(); 
        }


        /* -----------------------------------------------
         *  LED エフェクト
         * ------------------------------------------------*/

        void AllLight(uint8_t brightless);
        void clear();

        bool blink(uint8_t brightless, int delaytime);

        bool cycron(uint8_t brightless, int delaytime);

        bool fade(uint8_t brightless, int delaytime);

        bool turnRainbow(uint8_t brightless, int delaytime ,bool revese);

        bool rainbow(uint8_t brightless);

        bool rainbowCycle(uint8_t brightless, int delaytime,int cycleCount);

        bool stripOneColor(uint8_t brightless, int delaytime,int cycleCount, bool doFade,bool revese);

        bool stripRainbow(uint8_t brightless,int delaytime,int cycleCount, bool doFade,bool revese);

        bool glitterColor(int delaytime, int cycleCount);

        bool bpm(uint8_t brightless,int delaytime, int cycleCount);

        bool wipe(uint8_t brightless, int delaytime, bool revese);

        bool collision(int delaytime);

        bool charge(uint8_t brightless, int delaytime, bool revese);

        bool sinelon(uint8_t brightless,int delaytime);

        bool juggle(uint8_t brightless,int delaytime, int cycleCount);

        bool confetti(uint8_t brightless,int delaytime,int cycleCount);

        void resetLed(bool ledClear);

};

#endif 