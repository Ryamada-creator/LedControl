/*_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
*
*   概　要： NeoPixcel (WS2812B)を使用したLED点灯クラス バージョン_2 0 0
*　　　　　 ライブラリ ⇛ FastLEDを使用
*　　
*　 詳　細： effect 27個まで搭載
*　　　　　 処理自体はすべて毎回mainLoopに戻すようにしているため
*         ぐに戻ってくるようにしている（おそくても6msで戻るはず)
*         
*
 _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/*/

#ifndef NEOPIXCEL_H
    #define NEOPIXCEL_H

#define DEBUG_LED (0)

#include <Arduino.h>
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
        LedStatus m_Status;

        ledColors col;

        CRGBPalette16   currentPalette;
        CRGBPalette16   firePalette;
        TBlendType      currentBlending;

        // gloaval instance
        uint64_t        m_waitTime = 0;
        int             m_chageCount = 0;
        int             m_LedMax = 0;
        int             m_LedFocus = 0; 
        bool            m_LightEnable = false;

        // 色　保持変数
        uint8_t         m_Red = 100;
        uint8_t         m_Green = 50;
        uint8_t         m_Blue = 250;

        uint8_t         m_flashCount = 0;
        bool            m_doStrobe  = false;

        bool            m_Cycron = false;

        uint8_t         m_ShiftStep = 0;

        byte            *heat;

        // private Functions
        void setAllRGB(uint8_t red, uint8_t green, uint8_t blue);
        void setHeatColor(uint8_t temperature , int heatNum);
        void SetColorPalette( uint8_t red, uint8_t green, uint8_t blue);
        void addGlitter( fract8 chanceOfGlitter);

        bool passedTime(uint64_t _time){ return ( millis() >= _time); }

        bool countUpLedFocus(int dtTime);

        void startUp(uint8_t brightless = 0,int delaytime = 0); 

        void setFireColor(uint8_t cooling, uint8_t spaking, uint8_t fireColor, bool revese = true);

    public:
    
        // class intstance
        CFastLED m_neoPix;
        CRGB *m_cRGB;

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
            return m_Status; 
        }

        uint8_t getBrightless() {
            return m_neoPix.getBrightness(); 
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

        bool rainbow(uint8_t brightless, int delaytime);

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

        bool fire(uint8_t brightless, uint8_t cooling, uint8_t spaking, 
                            int delaytime, int cycleCount = 400, bool revese = false);

        bool blueFire(uint8_t brightless, uint8_t cooling, uint8_t spaking, 
                            int delaytime, int cycleCount = 400, bool revese = false);

        bool scanner(uint8_t brightless,int delaytime);

        bool Shift(uint8_t brightless, int delaytime, bool revese);

        bool blackShift(uint8_t brightless, int delaytime, bool revese);

        bool round_Shift(uint8_t brightless, int delaytime);

        bool round_blackShift(uint8_t brightless, int delaytime);

        bool dual_Shift(uint8_t brightless, int delaytime);

        bool dual_blackShift(uint8_t brightless, int delaytime);

        bool strobe(uint8_t brightless, uint8_t flashCount, int flashDelayTime , int waittime);

        bool interior(uint8_t brightless, int waittime);

        void resetLed(bool ledClear = false);

};

#endif 