
#include "NeoPixcelCtrl.h"


/** --------------------------------------------------
 * @brief カラー定義
 --------------------------------------------------*/
const uint8_t _rainbowColors[][3] = 
{
  {  0,   0,   0},  // Black
  {255,   0,   0},  // Red
  {255, 100,   0},  // Orange
  {255, 255,   0},  // Yellow
  {  0, 128,   0},  // Green
  {  0,   0, 255},  // Blue
  { 50,  50, 160},  // Indigo blue
  {128,   0, 128},  // Pink
  {255, 255, 255},  // White
  {100,   0, 200},  // Purple
};


/** --------------------------------------------------
 * @brief  LED初期化処理
 * @note   
 * @param  _neoLed: CFastLED　クラス継承
 * @param  *_crgb: CRGBクラス継承
 * @param  lednum: LED個数
 * @retval None
 --------------------------------------------------*/
void NeoPixcelCtrl::begin(CFastLED _neoLed, CRGB *_crgb, int lednum)
{
    // 継承
    c_neoPix = _neoLed;
    c_LedCount = lednum;
    c_cRGB = _crgb;
    c_status = led_Ready;

    // LED 全て消灯
    resetLed(true);
}



/** -------------------------------------------------
 * @brief  色を指定
 * @note   
 * @param  col: Enum定義　色を指定
 * @retval None
 *-------------------------------------------------*/
void NeoPixcelCtrl::setColor(ledColors col)
{
  switch (col) 
  {
    case cl_Red:
    case cl_Orange:
    case cl_Yellow:
    case cl_Green:
    case cl_Blue:
    case cl_Indigo:
    case cl_Pink:
    case cl_White:
    case cl_Purple:
      m_Red = _rainbowColors[col][0];
      m_Green = _rainbowColors[col][1];
      m_Blue = _rainbowColors[col][2];
      break;
    default:    // Black
      m_Red = _rainbowColors[0][0];
      m_Green = _rainbowColors[0][1];
      m_Blue = _rainbowColors[0][2];
      break;    
  }
}



/** --------------------------------------------------
 * @brief  全てのLEDを点灯
 * @note   
 * @param  brightless: 照度
 * @retval None
 --------------------------------------------------*/
void NeoPixcelCtrl::AllLight(uint8_t brightless)
{
    c_neoPix.setBrightness(brightless);

    for (int i = 0; i < c_LedCount; i++){
        c_cRGB[i] = CRGB(m_Red, m_Green, m_Blue);
    }

    c_neoPix.show();
    c_status = led_Finish;
}

/**  --------------------------------------------------
 * @brief  全てのLEDを全消灯
 * @note   
 * @retval None
     --------------------------------------------------*/
void NeoPixcelCtrl::clear()
{
    for (int i = 0; i < c_LedCount; i++){
        c_cRGB[i] = CRGB(0,0,0);
    }

    c_neoPix.show();
    c_status = led_Finish;
}


/** --------------------------------------------------
 * @brief  LED 点滅
 * @note   
 * @param  brightless: 照度
 * @param  delaytime: 点滅遅延時間
 * @retval 終了　→　True
 --------------------------------------------------*/
bool NeoPixcelCtrl::blink(uint8_t brightless, int delaytime)
{
    bool buf = false;

    if(c_status != led_Running)
    {

        c_status = led_Running;
        c_neoPix.setBrightness(brightless);
        c_loopCount = 0;
        cyckle = false;
        c_waitTime = millis() + delaytime;
    }

    if(c_loopCount == 0){
        setAllRGB(m_Red, m_Green, m_Blue);
        c_neoPix.show();
    }
    else if(c_loopCount == 1){
        setAllRGB(0,0,0);
        c_neoPix.show();
    }

    if(millis() >= c_waitTime)
    {
        c_loopCount ++;
        c_waitTime = millis() + delaytime;

        if(c_loopCount >= 2)
        {

            c_loopCount = 0;
            c_status = led_Finish;
            buf = true;
        }
    }
    return buf;
    
}


/** --------------------------------------------------
 * @brief  点灯　→　消灯　
 * @note   
 * @param  brightless: 照度
 * @param  delaytime: 遅延時間
 * @retval 終了　→　True
 --------------------------------------------------*/
bool NeoPixcelCtrl::fade(uint8_t brightless, int delaytime)
{
    bool buf = false;


    if(c_status != led_Running)
    {

        c_status = led_Running;
        c_neoPix.setBrightness(brightless);
        c_loopCount = 0;
        cyckle = false;
        c_waitTime = millis() + delaytime;
    }


    for (int k = c_loopCount; k < 256 ; k++)
    {
        if(!cyckle)
            setAllRGB((k / 255.0) * (float)m_Red, (k / 255.0) * (float)m_Green, (k / 255.0) * (float)m_Blue);
        else
            setAllRGB((float)m_Red * ((255.0 - k)/ 255.0), (float)m_Green *((255.0 - k)/ 255.0) ,(float)m_Blue *((255.0 - k)/ 255.0));
        break;
    }

    c_neoPix.show();

    if(millis() >= c_waitTime)
    {
        c_loopCount = c_loopCount + 1;

        if(c_loopCount >= 254 && !cyckle)
        {
            cyckle = true;
            c_loopCount = 0;
            c_waitTime = millis() + delaytime;
        }
        else if(c_loopCount >= 254 && cyckle)
        {

            cyckle = false;
            c_loopCount = 0;
            c_status = led_Finish;
            buf = true;
        }

    }

    return buf;
}



/** --------------------------------------------------
 * @brief  七色ワイプの往復
 * @note   
 * @param  brightless: 照度
 * @retval 終了　→　True
 --------------------------------------------------*/
bool NeoPixcelCtrl::cycron(uint8_t brightless, int delaytime)
{
    bool buf = false;

    if(c_status != led_Running)
    {
        c_status = led_Running;
        c_neoPix.setBrightness(brightless);
        c_loopCount = 0;
        c_waitTime = millis() + delaytime;
    }

    static uint8_t hue = 0;

    for(int i = c_loopCount; i < c_LedCount; i++) 
    {
        if(!cyckle) 
            c_cRGB[i] = CHSV(hue++, 255, 255);
        else
            c_cRGB[c_LedCount - i] = CHSV(hue++, 255, 255);

        c_neoPix.show();
        
        // fade の効果をつける
        for(int i = 0; i < c_LedCount; i++){
            c_cRGB[i].nscale8(250);
        }
        break;
    }

    
    if(millis() >= c_waitTime){
        c_waitTime = millis() + delaytime;
        c_loopCount ++;
    }

    if(c_loopCount >= c_LedCount - 1)
    {
        c_loopCount = 0;
        cyckle = !cyckle;
        c_status = led_Finish;
        buf = true;
    }

    return buf;
}

/** --------------------------------------------------
 * @brief  七色ワイプの片道
 * @note   
 * @param  brightless: 照度
 * @param  revese: 点灯方向
 * @retval 終了　→　True
 --------------------------------------------------*/
bool NeoPixcelCtrl::turnRainbow(uint8_t brightless, int delaytime, bool revese = false)
{
    bool buf = false;

    if(c_status != led_Running)
    {
        c_status = led_Running;
        c_neoPix.setBrightness(brightless);
        c_loopCount = 0;
        cyckle = false;
        c_waitTime = millis() + delaytime;
    }

    static uint8_t hue = 0;

    for(int i = c_loopCount; i < c_LedCount; i++)
    {
        if(revese) 
            c_cRGB[i] = CHSV(hue++, 255, 255);
        else
            c_cRGB[c_LedCount - i] = CHSV(hue++, 255, 255);

        c_neoPix.show();
        // black show...
        for(int i = 0; i < c_LedCount; i++){
            c_cRGB[i].nscale8(250);
        }
        break;
    }

    if(millis() >= c_waitTime){
        c_waitTime = millis() + delaytime;
        c_loopCount ++;
    }

    if(c_loopCount >= c_LedCount - 1)
    {
        c_loopCount = 0;
        c_status = led_Finish;
        buf = true;
    }

    return buf;
}


/** --------------------------------------------------
 * @brief  虹色点灯
 * @note   
 * @param  brightless: 
 * @retval 終了　→　True
 --------------------------------------------------*/
bool NeoPixcelCtrl::rainbow(uint8_t brightless)
{
    bool buf = false;

    if(c_status != led_Running)
    {
        c_status = led_Running;
        c_neoPix.setBrightness(brightless);
        c_loopCount = 0;
        cyckle = false;
    }

    for (int k = c_loopCount; k < 256; k++)
    {
        for (int i = 0; i < c_LedCount; i++)
        {
            c_cRGB[i] = CHSV(k, 255, 255);
        }
        break;
    }

    c_loopCount ++;

    c_neoPix.show();

    if(c_loopCount >= 254)
    {

        c_loopCount = 0;
        c_status = led_Finish;
        buf = true;
    }

    return buf;
}


/** --------------------------------------------------
 * @brief  虹色　シフト
 * @note   
 * @param  brightless: 照度
 * @param  delaytime: 遅延時間
 * @param  cycleCount: 継続回数
 * @retval 終了　→　True
 --------------------------------------------------*/
bool NeoPixcelCtrl::rainbowCycle(uint8_t brightless, int delaytime,int cycleCount)
{
    bool buf = false;

    if(c_status != led_Running)
    {
        c_status = led_Running;
        c_loopCount = 0;
        cyckle = true;
        c_waitTime = millis() + delaytime;
    }

    int colorIndex = 0;

    colorIndex = c_loopCount;
    
    if(cyckle)
    {
        for (int i = 0; i < c_LedCount; i++)
        {
            c_cRGB[i] = ColorFromPalette( RainbowColors_p, colorIndex, brightless, LINEARBLEND);
            colorIndex = colorIndex + 3;
        }

        c_neoPix.show();
        cyckle = false;
        c_loopCount ++;
    }


    if(millis() >= c_waitTime)
    {
        cyckle = true;
        c_waitTime = millis() + delaytime;

        if(c_loopCount >= cycleCount)
        {
            cyckle = false;
            c_loopCount = 0;
            c_waitTime = 0;

            c_status = led_Finish;

            resetLed(true);
            buf = true;
        }
    }

    return buf;
}

/** --------------------------------------------------
 * @brief  区切り　点灯
 * @note   
 * @param  brightless: 照度
 * @param  delaytime: シフト遅延時間
 * @param  cycleCount: 継続回数
 * @param  doFade: 　ぼんやり効果
 * @param  revese: 　点灯方向
 * @retval 終了　→　True
 --------------------------------------------------*/
bool NeoPixcelCtrl::stripOneColor(uint8_t brightless, int delaytime, int cycleCount, 
                                                    bool doFade , bool revese )
{
    bool buf = false;

    if(c_status != led_Running)
    {

        c_status = led_Running;
        c_loopCount = 0;
        cyckle = true;
        c_neoPix.setBrightness(brightless);
        c_waitTime = millis() + delaytime;
        SetColorPalette(m_Red, m_Green, m_Blue);
        currentBlending = (doFade) ? LINEARBLEND : NOBLEND ;
    }

    int colorIndex = 0;
    colorIndex = c_loopCount;
    
    if(cyckle)
    {
        for (int i = 0; i < c_LedCount; i++)
        {
            if(revese)
                c_cRGB[i] = ColorFromPalette( currentPalette, colorIndex, brightless, currentBlending);
            else
                c_cRGB[c_LedCount - i] = ColorFromPalette( currentPalette, colorIndex, brightless, currentBlending);

            colorIndex = colorIndex + 3;
        }

        c_neoPix.show();
        cyckle = false;
        c_loopCount ++;
    }

    if(millis() >= c_waitTime)
    {
        cyckle = true;
        c_waitTime = millis() + delaytime;

        if(c_loopCount >= cycleCount)
        {
            cyckle = false;
            c_loopCount = 0;
            c_waitTime = 0;
            c_status = led_Finish;
            resetLed(true);
            buf = true;
        }
    }
    return buf;

}


/** --------------------------------------------------
 * @brief  七色の区切り点灯
 * @note   
 * @param  brightless: 照度
 * @param  delaytime: 遅延時間
 * @param  cycleCount: 継続回数
 * @param  doFade: ぼんやり効果
 * @param  revese: 点灯方向
 * @retval 終了　→　True
 --------------------------------------------------*/
bool NeoPixcelCtrl::stripRainbow(uint8_t brightless,int delaytime,int cycleCount, bool doFade ,bool revese)
{
    bool buf = false;

    if(c_status != led_Running)
    {

        c_status = led_Running;
        c_loopCount = 0;
        cyckle = true;
        c_waitTime = millis() + delaytime;
        currentPalette = RainbowStripeColors_p; 
        currentBlending = (doFade) ? LINEARBLEND : NOBLEND ;
    }

    int colorIndex = 0;
    colorIndex = c_loopCount;
    
    if(cyckle)
    {
        for (int i = 0; i < c_LedCount; i++)
        {
            if(revese)
                c_cRGB[i] = ColorFromPalette( currentPalette, colorIndex, brightless, currentBlending);
            else
                c_cRGB[c_LedCount - i] = ColorFromPalette( currentPalette, colorIndex, brightless, currentBlending);

            colorIndex = colorIndex + 3;// + c_loopCount;
        }
        c_neoPix.show();
        cyckle = false;
        c_loopCount ++;
    }

    if(millis() >= c_waitTime)
    {
        cyckle = true;
        c_waitTime = millis() + delaytime;

        if(c_loopCount >= cycleCount)
        {
            cyckle = false;
            c_loopCount = 0;
            c_waitTime = 0;

            c_status = led_Finish;
            resetLed(true);
            buf = true;
        }   
    }
    return buf;
}

/** --------------------------------------------------
 * @brief  キラキラひかる
 * @note   
 * @param  delaytime: 遅延時間
 * @param  cycleCount: 継続回数
 * @retval 終了　→　True
 --------------------------------------------------*/
bool NeoPixcelCtrl::glitterColor(int delaytime, int cycleCount)
{
    bool buf = false;

    if(c_status != led_Running)
    {
        c_status = led_Running;
        c_neoPix.setBrightness(100);
        c_loopCount = 0;
        cyckle = true;
        c_waitTime = millis() + delaytime;
    }

    if(cyckle)
    {
        setAllRGB(m_Red, m_Green, m_Blue);
        addGlitter(80);
        c_neoPix.show();
        cyckle = false;
    }

    if(millis() >= c_waitTime)
    {
        cyckle = true;
        c_loopCount ++;
        c_waitTime = millis() + delaytime;

        //　終了
        if(c_loopCount >= cycleCount)
        {   
            c_loopCount = 0;
            c_status = led_Finish;
            cyckle = false;
            buf = true;
        }
    }
    return buf;
}

/** --------------------------------------------------
 * @brief  七色シフト
 * @note   
 * @param  brightless:　照度 
 * @param  delaytime: 　遅延時間
 * @param  cycleCount: 継続回数
 * @retval 終了　→　True
 --------------------------------------------------*/
bool NeoPixcelCtrl::bpm(uint8_t brightless, int delaytime,int cycleCount)
{
    bool buf = false;

    if(c_status != led_Running)
    {

        c_status = led_Running;
        c_neoPix.setBrightness(brightless);
        c_loopCount = 0;
        cyckle = true;
        c_waitTime = millis() + delaytime;
    }

    int colorIndex = 0;
    colorIndex = c_loopCount;

    if(cyckle)
    {
        for ( int i = 0; i < c_LedCount; i++)
        { 
            uint8_t BeatsPerMinute = 62;
            uint8_t beat = beatsin8( BeatsPerMinute, 64, 255);
            c_cRGB[i] = ColorFromPalette(PartyColors_p, colorIndex+(i*2), beat-colorIndex+(i*10));
        }

        c_neoPix.show();
        cyckle = false;
        c_loopCount ++;
    }

    if(millis() >= c_waitTime)
    {
        cyckle = true;
        c_waitTime = millis() + delaytime;

        if(c_loopCount >= cycleCount)
        {
            cyckle = false;
            c_loopCount = 0;
            c_waitTime = 0;
            c_status = led_Finish;
            buf = true;
            resetLed(true);
        }
    }
    return buf;
}


/** --------------------------------------------------
 * @brief  順次点灯
 * @note   
 * @param  brightless:　照度 
 * @param  delaytime: 遅延時間
 * @param  revese: 点灯方向
 * @retval 終了　→　True
 --------------------------------------------------*/
bool NeoPixcelCtrl::wipe(uint8_t brightless, int delaytime, bool revese)
{
    bool buf = false;
    if(c_status != led_Running)
    {

        c_status = led_Running;
        c_neoPix.setBrightness(brightless);
        c_loopCount = 0;
        cyckle = true;
        c_waitTime = millis() + delaytime;
    }

    if(cyckle)
    {
        if(revese)
            c_cRGB[c_loopCount] = CRGB(m_Red, m_Green, m_Blue);
        else
            c_cRGB[c_LedCount - c_loopCount] = CRGB(m_Red, m_Green, m_Blue);

        c_neoPix.show();
        c_loopCount ++;
        cyckle = false;
    }

    if(millis() >= c_waitTime){

        cyckle = true;
        c_waitTime = millis() + delaytime;

        if(c_loopCount >= c_LedCount)
        {

            c_loopCount = c_waitTime = 0;
            cyckle = false;
            c_status = led_Finish;
            buf = true;
        }
    }
    return buf;
}


/** --------------------------------------------------
 * @brief  衝突
 * @note   
 * @retval 終了　→　True
 --------------------------------------------------*/
bool NeoPixcelCtrl::collision(int delaytime)
{
    bool buf = false;
    static bool endFade = false;

    if(c_status != led_Running)
    {
        c_status = led_Running;
        c_neoPix.setBrightness(60);
        c_loopCount = 0;
        cyckle = true;
        endFade = false;
        c_waitTime = millis() + delaytime;
    }

    if(cyckle)
    {
        c_cRGB[c_loopCount] = CRGB(m_Red, m_Green, m_Blue);
        c_cRGB[c_LedCount - c_loopCount] = CRGB(m_Red, m_Green, m_Blue);
        c_neoPix.show();
        c_loopCount ++;
        cyckle = false;
    }

    if(c_LedCount / 2 + 1 <=  c_loopCount)
    {
        if(!endFade)
        {
            c_neoPix.setBrightness(255);
            c_neoPix.show();
            c_waitTime = millis() + 1500;
            cyckle = false; 
            endFade = true;
        }

        for(int i = 0; i < c_LedCount; i++)
            c_cRGB[i].nscale8(253);
        
        c_neoPix.show();

        if(millis() >= c_waitTime)
        {
            c_loopCount = 0;
            c_status = led_Finish;
            endFade =false;
            buf = true;
            resetLed(true);
        }
    }
    else
    {
        if(millis() >= c_waitTime && !endFade)
        {
            c_waitTime = millis() + delaytime;
            cyckle = true;
        }
    }

    return buf;
}


/**　--------------------------------------------------
 * @brief  積み重ね
 * @note   
 * @param  brightless:　照度 
 * @param  revese: 　　　点灯方向
 * @retval 終了　→　True
 --------------------------------------------------*/
bool NeoPixcelCtrl::charge(uint8_t brightless, int delaytime, bool revese)
{
    bool buf = false;

   if(c_status != led_Running)
   {
        c_status = led_Running;
        c_neoPix.setBrightness(brightless);
        c_loopCount = chage_count = 0;
        cyckle = true;
        c_waitTime = millis() + delaytime;
    }

    // LEDを一つ点灯する
    if(cyckle)
    {
        if(revese)
            c_cRGB[chage_count] = CRGB(m_Red, m_Green, m_Blue);
        else
            c_cRGB[c_LedCount - chage_count] = CRGB(m_Red, m_Green, m_Blue);

        cyckle = false;
        c_waitTime = millis() + delaytime;
    }

    // 表示
    c_neoPix.show();

    // 次回が来たら次のLEDへ移行
    if(millis() >= c_waitTime)
    {
        // 消灯
        if(revese)
            c_cRGB[chage_count] = CRGB(0, 0, 0);
        else
            c_cRGB[c_LedCount - chage_count] = CRGB(0, 0, 0);
        
        // 更新
        chage_count ++;
        cyckle = true;
    }
            
    // 積み重ねLEDの保持
    if( (c_LedCount - c_loopCount) == chage_count)
    {
        c_loopCount++;
        chage_count = 0;

        if(revese) 
            c_cRGB[c_LedCount - c_loopCount] = CRGB(m_Red, m_Green, m_Blue);
        else
            c_cRGB[c_loopCount] = CRGB(m_Red, m_Green, m_Blue);

    }

    if(c_loopCount >= c_LedCount)
    {
        c_loopCount = c_waitTime = 0;
        cyckle = false;
        c_status = led_Finish;
        buf = true;
    }

    delayMicroseconds(100);

    return buf;
}


/**　--------------------------------------------------
 * @brief  蕩々と　小さいほのうの様に
 * @note   
 * @param  brightless: 照度
 * @param  delaytime:　遅延時間 
 * @retval 終了　→　True
 --------------------------------------------------*/
bool NeoPixcelCtrl::sinelon(uint8_t brightless,int delaytime)
{
    bool buf = false;

    if(c_status != led_Running)
    {

        c_status = led_Running;
        c_neoPix.setBrightness(brightless);
        c_loopCount = chage_count = 0;
        c_waitTime = millis() + delaytime;
        cyckle = false;
    }

    for (int i = c_loopCount; i < c_LedCount; i++)
    {
        nscale8( c_cRGB, c_LedCount ,255 - 20);
        int pos = beatsin16( 13, 0, c_LedCount-1 );
        c_cRGB[pos] += CHSV( c_loopCount, 255, 192);
        break;
    }

    c_neoPix.show();


    if(millis() >= c_waitTime)
    {
        c_loopCount ++;

        if(c_loopCount >= c_LedCount)
        {

            c_status = led_Finish;
            buf = true;
            resetLed(true);
        }
    }

    return buf;
}

/**　--------------------------------------------------
 * @brief  ジャグリング
 * @note   
 * @param  brightless: 照度
 * @param  delaytime: 遅延時間
 * @param  cycleCount: 継続回数
 * @retval 終了　→　True
 　--------------------------------------------------*/
bool NeoPixcelCtrl::juggle(uint8_t brightless,int delaytime, int cycleCount)
{
    bool buf = false;

    if(c_status != led_Running){

        c_status = led_Running;
        c_neoPix.setBrightness(brightless);
        c_loopCount = chage_count = 0;
        c_waitTime = millis() + delaytime;
        cyckle = false;
    }

    nscale8( c_cRGB, c_LedCount ,255 - 20);

    byte dothue = 0;

    for( int i = 0; i < 8; i++) 
    {
        c_cRGB[beatsin16( i+7, 0, c_LedCount - 1 )] |= CHSV(dothue, 200, 255);
        dothue += 32;
    }

    c_neoPix.show();

    if(millis() >= c_waitTime)
    {
        c_loopCount ++;

        if(c_loopCount >= cycleCount)
        {
           
            c_status = led_Finish;
            buf = true;
            resetLed(true);
        }
    }

    return buf;
}

/** --------------------------------------------------
 * @brief  ランダムにパラパラとひかる
 * @note   
 * @param  brightless:　照度 
 * @param  delaytime: 　遅延時間
 * @param  cycleCount: 継続回数
 * @retval 終了　→　True
 --------------------------------------------------*/
bool NeoPixcelCtrl::confetti(uint8_t brightless,int delaytime,int cycleCount)
{
    bool buf = false;

    if(c_status != led_Running)
    {

        c_status = led_Running;
        c_neoPix.setBrightness(brightless);
        c_loopCount = chage_count = 0;
        c_waitTime = millis() + delaytime;
        cyckle = false;
    }
    
    nscale8( c_cRGB, c_LedCount ,255 - 20);

    int pos = random16(c_LedCount);
    c_cRGB[pos] += CHSV( c_loopCount + random8(64), 200, 255);

    c_neoPix.show();

    if(millis() >= c_waitTime)
    {
        c_loopCount ++;

        if(c_loopCount >= cycleCount)
        {
            
            c_status = led_Finish;
            buf = true;
            resetLed(true);
        }
    }

    return buf;
}


/**　--------------------------------------------------
 * @brief  カラーパレット
 * @note   
 * @param  red: 
 * @param  green: 
 * @param  blue: 
 * @retval None
 --------------------------------------------------*/
void NeoPixcelCtrl::SetColorPalette( uint8_t red, uint8_t green, uint8_t blue)
{
    // 'black out' all 16 palette entries...
    fill_solid( currentPalette, 16, CRGB::Black);
    // and set every fourth one to white.
    currentPalette[0] = CRGB(red,green,blue);
    currentPalette[4] = CRGB(red,green,blue);
    currentPalette[8] = CRGB(red,green,blue);
    currentPalette[12] = CRGB(red,green,blue);
    
}


/**
 * @brief  煌めき箇所をランダムにセット
 * @note   
 * @param  chanceOfGlitter: 
 * @retval None
 */
void NeoPixcelCtrl::addGlitter(fract8 chanceOfGlitter)
{
  if( random8() < chanceOfGlitter) {
    c_cRGB[ random16(c_LedCount) ] += CRGB::White;
  }
}



/**　--------------------------------------------------
 * @brief  RGB セット
 * @note   
 * @param  red: 
 * @param  green: 
 * @param  blue: 
 * @retval None
-------------------------------------------------- */
void NeoPixcelCtrl::setAllRGB(uint8_t red, uint8_t green, uint8_t blue)
{
    for (int i = 0; i < c_LedCount; i++){
        c_cRGB[i] = CRGB(red, green, blue);
    }
}



/**　--------------------------------------------------
 * @brief  LEDリセット
 * @note   
 * @param  ledClear: 消灯するかどうか
 * @retval None
 --------------------------------------------------*/
void NeoPixcelCtrl::resetLed(bool ledClear)
{
    c_status = led_Finish;
    c_loopCount = 0;
    cyckle = false;
    if(ledClear)
        clear();
}

