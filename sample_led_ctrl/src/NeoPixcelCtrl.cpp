
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
    m_neoPix = _neoLed;
    m_LedMax = lednum;
    m_cRGB = _crgb;
    m_Status = led_Ready;

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
    m_neoPix.setBrightness(brightless);

    for (int i = 0; i < m_LedMax; i++){
        m_cRGB[i] = CRGB(m_Red, m_Green, m_Blue);
    }

    m_neoPix.show();
    m_Status = LedStatus::led_Finish;
}

/**  --------------------------------------------------
 * @brief  全てのLEDを全消灯
 * @note   
 * @retval None
     --------------------------------------------------*/
void NeoPixcelCtrl::clear()
{
    for (int i = 0; i < m_LedMax; i++){
        m_cRGB[i] = CRGB(0,0,0);
    }

    m_neoPix.show();
    m_Status = LedStatus::led_Finish;
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

    if(m_Status != LedStatus::led_Running)
    {
        this->startUp(brightless, delaytime);
    }

    // 点灯
    if(m_LedFocus == 0) {
        setAllRGB(m_Red, m_Green, m_Blue);
    }
    // 消灯
    else if(m_LedFocus == 1){
        setAllRGB(0,0,0);
    }

    m_neoPix.show();

    this->countUpLedFocus(delaytime);

    if(m_LedFocus >= 2)
    {
        resetLed();
        buf = true;
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


    if(m_Status != LedStatus::led_Running)
    {
        this->startUp(brightless, delaytime);
    }


    for (int k = m_LedFocus; k < 256 ; k++)
    {
        if(m_LightEnable)
            setAllRGB((k / 255.0) * (float)m_Red, (k / 255.0) * (float)m_Green, (k / 255.0) * (float)m_Blue);
        else
            setAllRGB((float)m_Red * ((255.0 - k)/ 255.0), (float)m_Green *((255.0 - k)/ 255.0) ,(float)m_Blue *((255.0 - k)/ 255.0));
        break;
    }

    m_neoPix.show();

    this->countUpLedFocus(delaytime);


    if(m_LedFocus >= 254 && m_LightEnable)
    {
        // 消灯へ移行
        m_LightEnable = false;
        m_LedFocus = 0;
        m_waitTime = millis() + delaytime;
    }


    if(m_LedFocus >= 254 && !m_LightEnable)
    {
        resetLed(true);
        buf = true;
    }

    return buf;
}



/** --------------------------------------------------
 * @brief  七色ワイプの往復
 * @note   
 * @param  brightless: 照度
 * @retval 終了　→　True
 * TODO:　片道しか動かない
 --------------------------------------------------*/
bool NeoPixcelCtrl::cycron(uint8_t brightless, int delaytime)
{
    bool buf = false;
    static uint8_t hue = 0;

    if(m_Status != LedStatus::led_Running)
    {
        this->startUp(brightless, delaytime);
        m_Cycron = true;
    }

    if(hue >= 255)
        hue = 0;

    if(m_Cycron) 
        m_cRGB[m_LedFocus] = CHSV(hue++, 255, 255);
    else
        m_cRGB[m_LedMax - m_LedFocus] = CHSV(hue++, 255, 255);

    m_neoPix.show();
        
    // fade の効果をつける
    for(int i = 0; i < m_LedMax; i++){
        m_cRGB[i].nscale8(250);
    }
    
    this->countUpLedFocus(delaytime);

    if(m_LedFocus >= m_LedMax)
    {
        m_LedFocus = 0;
        m_Cycron = !m_Cycron;

        // 終了
        if(m_Cycron)
        {
            buf = true;
        }
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

    if(m_Status != LedStatus::led_Running)
    {
        this->startUp(brightless, delaytime);
    }

    static uint8_t hue = 0;
    
    if(hue >= 255)
        hue = 0;

    if(revese) 
        m_cRGB[m_LedFocus] = CHSV(hue++, 255, 255);
    else
        m_cRGB[m_LedMax - m_LedFocus] = CHSV(hue++, 255, 255);

    m_neoPix.show();

    for(int i = 0; i < m_LedMax; i++){
        m_cRGB[i].nscale8(250);
    }
    
    this->countUpLedFocus(delaytime);

    if(m_LedFocus >= m_LedMax)
    {
        resetLed();
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
bool NeoPixcelCtrl::rainbow(uint8_t brightless, int delaytime)
{
    bool buf = false;

    if(m_Status != LedStatus::led_Running)
    {
        this->startUp(brightless);
    }

    for (int i = 0; i < m_LedMax; i++)
    {
        m_cRGB[i] = CHSV(m_LedFocus, 255, 255);
    }

    this->countUpLedFocus(delaytime);

    m_neoPix.show();

    if(m_LedFocus >= 255)
    {
        resetLed(true);
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

    if(m_Status != LedStatus::led_Running)
    {
        this->startUp(brightless, delaytime);
    }

    int colorIndex = 0;

    colorIndex = m_LedFocus;
    
    if(m_LightEnable)
    {
        for (int i = 0; i < m_LedMax; i++)
        {
            m_cRGB[i] = ColorFromPalette( RainbowColors_p, colorIndex, brightless, LINEARBLEND);
            colorIndex = colorIndex + 3;
        }

        m_neoPix.show();
        m_waitTime = millis() + delaytime;
    }

    m_LightEnable = this->countUpLedFocus(delaytime);

    if(m_LedFocus >= cycleCount)
    {
        resetLed(true);
        buf = true;
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

    if(m_Status != LedStatus::led_Running)
    {
        this->startUp(brightless, delaytime);
        SetColorPalette(m_Red, m_Green, m_Blue);
        currentBlending = (doFade) ? LINEARBLEND : NOBLEND ;
    }

    int colorIndex = 0;
    colorIndex = m_LedFocus;
    
    if(m_LightEnable)
    {
        for (int i = 0; i < m_LedMax; i++)
        {
            if(revese)
                m_cRGB[i] = ColorFromPalette( currentPalette, colorIndex, brightless, currentBlending);
            else
                m_cRGB[m_LedMax - i] = ColorFromPalette( currentPalette, colorIndex, brightless, currentBlending);

            colorIndex = colorIndex + 3;
        }

        m_neoPix.show();
        m_waitTime = millis() + delaytime;
    }

    m_LightEnable = this->countUpLedFocus(delaytime);

    if(m_LedFocus >= cycleCount)
    {
        resetLed(true);
        buf = true;
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

    if(m_Status != LedStatus::led_Running)
    {
        this->startUp(brightless, delaytime);
        currentPalette = RainbowStripeColors_p; 
        currentBlending = (doFade) ? LINEARBLEND : NOBLEND ;
    }

    int colorIndex = 0;
    colorIndex = m_LedFocus;
    
    if(m_LightEnable)
    {
        for (int i = 0; i < m_LedMax; i++)
        {
            if(revese)
                m_cRGB[i] = ColorFromPalette( currentPalette, colorIndex, brightless, currentBlending);
            else
                m_cRGB[m_LedMax - i] = ColorFromPalette( currentPalette, colorIndex, brightless, currentBlending);

            colorIndex = colorIndex + 3;// + m_LedFocus;
        }

        m_neoPix.show();
        m_waitTime = millis() + delaytime;
    }

    m_LightEnable = this->countUpLedFocus(delaytime);

    if(m_LedFocus >= cycleCount)
    {
        resetLed(true);
        buf = true;
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

    if(m_Status != LedStatus::led_Running)
    {
        this->startUp(100, delaytime);
    }

    if(m_LightEnable)
    {
        setAllRGB(m_Red, m_Green, m_Blue);
        addGlitter(80);
        m_neoPix.show();
        m_waitTime = millis() + delaytime;
    }

    m_LightEnable = this->countUpLedFocus(delaytime);

    //　終了
    if(m_LedFocus >= cycleCount)
    {   
        this->resetLed(true);
        buf = true;
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

    if(m_Status != LedStatus::led_Running)
    {
        this->startUp(brightless, delaytime);
    }

    int colorIndex = 0;
    colorIndex = m_LedFocus;

    if(m_LightEnable)
    {
        for ( int i = 0; i < m_LedMax; i++)
        { 
            uint8_t BeatsPerMinute = 62;
            uint8_t beat = beatsin8( BeatsPerMinute, 64, 255);
            m_cRGB[i] = ColorFromPalette(PartyColors_p, colorIndex+(i*2), beat-colorIndex+(i*10));
        }

        m_neoPix.show();
        m_waitTime = millis() + delaytime;
    }

    m_LightEnable = this->countUpLedFocus(delaytime);

    if(m_LedFocus >= cycleCount)
    {
        resetLed(true);
        buf = true;
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

    if(m_Status != LedStatus::led_Running)
    {
        this->startUp(brightless, delaytime);
    }

    if(m_LightEnable)
    {
        if(revese)
            m_cRGB[m_LedFocus] = CRGB(m_Red, m_Green, m_Blue);
        else
            m_cRGB[m_LedMax - m_LedFocus] = CRGB(m_Red, m_Green, m_Blue);

        m_neoPix.show();
        m_waitTime = millis() + delaytime;
    }

    m_LightEnable = this->countUpLedFocus(delaytime);

    if(m_LedFocus >= m_LedMax)
    {
        this->resetLed(true);
        buf = true;
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

    if(m_Status != LedStatus::led_Running)
    {
        this->startUp(60, delaytime);
        endFade = false;
    }

    if(m_LightEnable)
    {
        m_cRGB[m_LedFocus] = CRGB(m_Red, m_Green, m_Blue);
        m_cRGB[m_LedMax - m_LedFocus] = CRGB(m_Red, m_Green, m_Blue);
        m_neoPix.show();
        m_LedFocus ++;
        m_LightEnable = false;
    }

    // 衝突
    if(m_LedMax / 2 + 1 <=  m_LedFocus)
    {
        if(!endFade)
        {
            // 衝突後は明るくする
            m_neoPix.setBrightness(255);
            m_neoPix.show();
            m_waitTime = millis() + 1500;
            m_LightEnable = false; 
            endFade = true;
        }

        // Fade 効果
        for(int i = 0; i < m_LedMax; i++)
            m_cRGB[i].nscale8(253);
        
        m_neoPix.show();

        // LED 終了
        if(this->passedTime(m_waitTime))
        {
            resetLed(true);
            endFade =false;
            buf = true;
        }
    }
    else
    {
        if(this->passedTime(m_waitTime) && !endFade)
        {
            m_waitTime = millis() + delaytime;
            m_LightEnable = true;
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

    if(m_Status != LedStatus::led_Running)
    {
        this->startUp(brightless, delaytime);
    }

    // LEDを一つ点灯する
    if(m_LightEnable)
    {
        if(revese)
            m_cRGB[m_chageCount] = CRGB(m_Red, m_Green, m_Blue);
        else
            m_cRGB[m_LedMax - m_chageCount] = CRGB(m_Red, m_Green, m_Blue);

        m_LightEnable = false;
        m_waitTime = millis() + delaytime;
    }

    // 表示
    m_neoPix.show();

    // 時間が来たら次のLEDへ移行
    if(this->passedTime(m_waitTime))
    {
        // 消灯
        if(revese)
            m_cRGB[m_chageCount] = CRGB(0, 0, 0);
        else
            m_cRGB[m_LedMax - m_chageCount] = CRGB(0, 0, 0);
        
        // 更新
        m_chageCount ++;
        m_LightEnable = true;
    }
            
    // 積み重ねLEDの保持
    if( (m_LedMax - m_LedFocus) == m_chageCount)
    {
        m_LedFocus++;
        m_chageCount = 0;

        if(revese) 
            m_cRGB[m_LedMax - m_LedFocus] = CRGB(m_Red, m_Green, m_Blue);
        else
            m_cRGB[m_LedFocus] = CRGB(m_Red, m_Green, m_Blue);
    }

    if(m_LedFocus >= m_LedMax)
    {
        this->resetLed(true);
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

    if(m_Status != LedStatus::led_Running)
    {
        this->startUp(brightless, delaytime);
    }

    nscale8( m_cRGB, m_LedMax ,255 - 20);
    int pos = beatsin16( 13, 0, m_LedMax - 1 );
    m_cRGB[pos] += CHSV( m_LedFocus, 255, 192);

    m_neoPix.show();

    this->countUpLedFocus(delaytime);

    if(m_LedFocus >= m_LedMax)
    {
        resetLed(true);
        buf = true;
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

    if(m_Status != LedStatus::led_Running)
    {
        this->startUp(brightless, delaytime);
    }

    nscale8( m_cRGB, m_LedMax ,255 - 20);

    byte dothue = 0;

    for( int i = 0; i < 8; i++) 
    {
        m_cRGB[beatsin16( i+7, 0, m_LedMax - 1 )] |= CHSV(dothue, 200, 255);
        dothue += 32;
    }

    m_neoPix.show();

    // 遅延時間が経過したか確認
    this->countUpLedFocus(delaytime);

    if(m_LedFocus >= cycleCount)
    {
        // LED　終了処理
        resetLed(true);
        buf = true;
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

    if(m_Status != LedStatus::led_Running)
    {
        this->startUp(brightless, delaytime);
    }
    
    nscale8( m_cRGB, m_LedMax ,255 - 20);

    int pos = random16(m_LedMax);

    m_cRGB[pos] += CHSV( m_LedFocus + random8(64), 200, 255);

    m_neoPix.show();

    this->countUpLedFocus(delaytime);

    if(m_LedFocus >= cycleCount)
    {
        // LED 終了処理
        resetLed(true);
        buf = true;
    }

    return buf;
}



/** --------------------------------------------------
 * @brief  炎をイメージしたLEDエフェクト
 * @note   
 * @param  cooling: 炎の高さ　数値が高いほど　高さは低い
 * @param  spaking: 煌めき具合
 * @param  delaytime: 遅延時間
 * @param  revese: 点灯芳香
 * @retval 終了したら True
 --------------------------------------------------*/
bool NeoPixcelCtrl::fire(uint8_t brightless, uint8_t cooling, 
                                uint8_t spaking, int delaytime, int cycleCount, bool revese)
{

    bool buf = false;

    if(m_Status != LedStatus::led_Running)
    {
        this->startUp(brightless, delaytime);
    }

    if(m_LightEnable)
        setFireColor(cooling, spaking, 0, revese);

    m_LightEnable = this->countUpLedFocus(delaytime);

    m_neoPix.show();

    if(m_LedFocus >= cycleCount)
    {
        // LED 終了処理
        resetLed();
        buf = true;
    }

    return buf;
}


/** --------------------------------------------------
 * @brief  青い炎をイメージしたLEDエフェクト
 * @note   
 * @param  cooling: 炎の高さ　数値が高いほど　高さは低い
 * @param  spaking: 煌めき具合
 * @param  delaytime: 遅延時間
 * @param  revese: 点灯芳香
 * @retval 終了したら True
 --------------------------------------------------*/
bool NeoPixcelCtrl::blueFire(uint8_t brightless, uint8_t cooling,
                                uint8_t spaking, int delaytime, int cycleCount, bool revese)
{

    bool buf = false;

    if(m_Status != LedStatus::led_Running)
    {
        this->startUp(brightless, delaytime);
    }

    if(m_LightEnable)
        setFireColor(cooling, spaking, 1, revese);

    m_LightEnable = this->countUpLedFocus(delaytime);

    m_neoPix.show();

    if(m_LedFocus >= cycleCount)
    {
        // LED 終了処理
        resetLed();
        buf = true;
    }

    return buf;
}

/**　--------------------------------------------------
 * @brief  LEDの柔らかな移動
 * @note   
 * @param  brightless: 照度
 * @param  delaytime:　遅延時間 
 * @retval 終了　→　True
 --------------------------------------------------*/
bool NeoPixcelCtrl::scanner(uint8_t brightless,int delaytime)
{
    bool buf = false;

    if(m_Status != LedStatus::led_Running)
    {
        this->startUp(brightless, delaytime);
    }

    nscale8( m_cRGB, m_LedMax ,255 - 20);
    int pos = beatsin16( 13, 0, m_LedMax - 1 );
    m_cRGB[pos] += CRGB(m_Red, m_Green, m_Blue);

    m_neoPix.show();

    this->countUpLedFocus(delaytime);

    if(m_LedFocus >= m_LedMax)
    {
        resetLed(true);
        buf = true;
    }

    return buf;
}

/** --------------------------------------------------
 * @brief  シフト点灯
 * @note   
 * @param  brightless: 照度
 * @param  delaytime: 遅延時間
 * @param  revese: 方向
 * @retval 完了したらTrue
 --------------------------------------------------*/
 bool NeoPixcelCtrl::Shift(uint8_t brightless, int delaytime, bool revese)
 {
    bool buf = false;

    if(m_Status != LedStatus::led_Running)
    {
        this->startUp(brightless, delaytime);
    }

    if(m_LightEnable)
    {
        setAllRGB(0, 0, 0);

        if(revese)
        {
            m_cRGB[m_LedFocus] = CRGB(m_Red, m_Green, m_Blue);
            if(m_LedFocus != 0)
                m_cRGB[m_LedFocus - 1 ] = CRGB(m_Red, m_Green, m_Blue);
        }
        else
        {
            m_cRGB[m_LedMax - m_LedFocus] = CRGB(m_Red, m_Green, m_Blue);
            if(m_LedFocus != m_LedMax)
                m_cRGB[m_LedMax - (m_LedFocus + 1)] = CRGB(m_Red, m_Green, m_Blue);
        }

        m_neoPix.show();
        m_waitTime = millis() + delaytime;
    }

    m_LightEnable = this->countUpLedFocus(delaytime);

    if(m_LedFocus >= m_LedMax)
    {
        this->resetLed();
        buf = true;
    }

    return buf;
    
 }


/** --------------------------------------------------
 * @brief  シフト消灯
 * @note   
 * @param  brightless: 照度
 * @param  delaytime: 遅延時間
 * @param  revese: 方向
 * @retval 完了したらTrue
 --------------------------------------------------*/
 bool NeoPixcelCtrl::blackShift(uint8_t brightless, int delaytime, bool revese)
 {
    bool buf = false;

    if(m_Status != LedStatus::led_Running)
    {
        this->startUp(brightless, delaytime);
    }

    if(m_LightEnable)
    {
        setAllRGB(m_Red, m_Green, m_Blue);

        if(revese)
        {
            m_cRGB[m_LedFocus] = CRGB::Black;
            if(m_LedFocus != 0)
                m_cRGB[m_LedFocus - 1 ] = CRGB::Black;
        }
        else
        {
            m_cRGB[m_LedMax - m_LedFocus] = CRGB::Black;
            if(m_LedFocus != m_LedMax)
                m_cRGB[m_LedMax - (m_LedFocus + 1)] = CRGB::Black;
        }

        m_neoPix.show();
        m_waitTime = millis() + delaytime;
    }

    m_LightEnable = this->countUpLedFocus(delaytime);

    if(m_LedFocus >= m_LedMax)
    {
        this->resetLed();
        buf = true;
    }

    return buf;
    
 }

/** --------------------------------------------------
 * @brief  双方向のシフト点灯
 * @note   
 * @param  brightless: 照度
 * @param  delaytime: 遅延時間
 * @retval 完了したらTrue
 --------------------------------------------------*/
 bool NeoPixcelCtrl::dual_Shift(uint8_t brightless, int delaytime)
 {
    bool buf = false;

    if(m_Status != LedStatus::led_Running)
    {
        this->startUp(brightless, delaytime);
    }

    if(m_LightEnable)
    {
        setAllRGB(0, 0, 0);

        m_cRGB[m_LedFocus] = CRGB(m_Red, m_Green, m_Blue);

        if(m_LedFocus != 0)
            m_cRGB[m_LedFocus - 1 ] = CRGB(m_Red, m_Green, m_Blue);

        m_cRGB[m_LedMax - m_LedFocus] = CRGB(m_Red, m_Green, m_Blue);

        if(m_LedFocus != m_LedMax)
            m_cRGB[m_LedMax - (m_LedFocus + 1)] = CRGB(m_Red, m_Green, m_Blue);

        m_neoPix.show();
        m_waitTime = millis() + delaytime;
    }

    m_LightEnable = this->countUpLedFocus(delaytime);

    if(m_LedFocus >= m_LedMax)
    {
        this->resetLed();
        buf = true;
    }

    return buf;
 }


/** --------------------------------------------------
 * @brief  双方向のシフト消灯
 * @note   
 * @param  brightless: 照度
 * @param  delaytime: 遅延時間
 * @retval 完了したらTrue
 --------------------------------------------------*/
 bool NeoPixcelCtrl::dual_blackShift(uint8_t brightless, int delaytime)
 {
    bool buf = false;

    if(m_Status != LedStatus::led_Running)
    {
        this->startUp(brightless, delaytime);
    }

    if(m_LightEnable)
    {
        setAllRGB(m_Red, m_Green, m_Blue);

        m_cRGB[m_LedFocus] = CRGB::Black;

        if(m_LedFocus != 0)
            m_cRGB[m_LedFocus - 1 ] = CRGB::Black;
        
        m_cRGB[m_LedMax - m_LedFocus] = CRGB::Black;

        if(m_LedFocus != m_LedMax)
            m_cRGB[m_LedMax - (m_LedFocus + 1)] = CRGB::Black;

        m_neoPix.show();
        m_waitTime = millis() + delaytime;
    }

    m_LightEnable = this->countUpLedFocus(delaytime);

    if(m_LedFocus >= m_LedMax)
    {
        this->resetLed();
        buf = true;
    }

    return buf;
 }

 /** --------------------------------------------------
 * @brief  往復でシフト 点灯
 * @note   
 * @param  brightless: 照度
 * @param  delaytime: 遅延時間
 * @retval 完了　true
 --------------------------------------------------*/
bool NeoPixcelCtrl::round_Shift(uint8_t brightless, int delaytime)
{
     bool buf = false;

    if(m_Status != LedStatus::led_Running){
        m_ShiftStep = 0;
    }

    for (uint8_t i = m_ShiftStep; i < 2; i++)
    {
        if(this->Shift(brightless, delaytime, i)) {
            m_ShiftStep ++;
            m_Status = LedStatus::led_Running;
        }
        break;
    }

    if(m_ShiftStep == 2) {
        this->resetLed();
        m_ShiftStep = 0;
        buf = true;
    }

    return buf;
}

/** --------------------------------------------------
 * @brief  往復でシフト 消灯
 * @note   
 * @param  brightless: 照度
 * @param  delaytime: 遅延時間
 * @retval 完了　true
 --------------------------------------------------*/
bool NeoPixcelCtrl::round_blackShift(uint8_t brightless, int delaytime)
{
    bool buf = false;

    if(m_Status != LedStatus::led_Running) {
        m_ShiftStep = 0;
    }

    for (uint8_t i = m_ShiftStep; i < 2; i++)
    {
        if(this->blackShift(brightless, delaytime, i)) {
            m_ShiftStep ++;
            m_Status = LedStatus::led_Running;
        }
        break;
    }

    if(m_ShiftStep == 2) {
        this->resetLed();
        m_ShiftStep = 0;
        buf = true;
    }

    return buf;
}

/** --------------------------------------------------
 * @brief  フラッシュを繰り返す
 * @note   
 * @param  brightless: 照度
 * @param  flashCount: 点滅回数
 * @param  waittime: フラッシュ後の待機時間
 * @retval 完了したらTrue
 --------------------------------------------------*/
bool NeoPixcelCtrl::strobe(uint8_t brightless, uint8_t flashCount, int flashDelayTime , int waittime)
{
    bool buf = false;

    if(m_Status != LedStatus::led_Running && !m_doStrobe) {
        m_flashCount = 0;
    }

    if(m_LightEnable)
    {
        if(this->blink(brightless, flashDelayTime))
        {
            m_flashCount++;

            if(flashCount == m_flashCount)
            {
                m_LightEnable = false;
                m_waitTime = millis() + waittime;
            }
        }
        m_doStrobe = true;
    }
    else
    {
        if(this->passedTime(m_waitTime))
        {
            resetLed();
            buf = true;
        }
    }

    return buf;

}


/** --------------------------------------------------
 * @brief  ランダムに色を指定し、点灯で固定
 * @note   
 * @param  brightless: 照度
 * @param  waittime: 固定時間
 * @retval 完了したらTrue
 --------------------------------------------------*/
bool NeoPixcelCtrl::interior(uint8_t brightless, int waittime)
{
    bool buf = false;

    if(m_Status != LedStatus::led_Running)
    {
        this->startUp(brightless, waittime);
    }

    if(m_LightEnable)
    {
 
        for (uint32_t i = 0; i < m_LedMax; i++)
        {
            m_cRGB[i] = ColorFromPalette(PartyColors_p, random8(0,255), random8(0,255));
        }
        
        m_neoPix.show();
        m_LightEnable = false;
    }

    if(this->passedTime(m_waitTime))
    {
        this->resetLed();
        buf = true;
    }

    return buf;
    
}

/** --------------------------------------------------
 * @brief  炎カラーをセット
 * @note   
 * @param  cooling: 燃え上がりたかさ調整　
 * @param  spaking: 煌めき具合調整
 * @param  fireColor: 0 →　赤　1 →　青　
 * @param  revese: 方向
 * @retval None
 --------------------------------------------------*/
void NeoPixcelCtrl::setFireColor(uint8_t cooling, uint8_t spaking, uint8_t fireColor, bool revese)
{

    random16_add_entropy( random());

    if(fireColor == 0)
    {
        firePalette = CRGBPalette16( CRGB::Black, CRGB::Red, CRGB::Yellow, CRGB::White);
    }
    else
    {
        // 青い炎
        firePalette = CRGBPalette16( CRGB::Black, CRGB::Blue, CRGB::Aqua,  CRGB::White);
    }
   
    this->heat = new byte[m_LedMax];

    for( int i = 0; i < m_LedMax; i++) 
    {
      this->heat[i] = qsub8( this->heat[i],  random8(0, ((cooling * 10) / m_LedMax) + 2));
    }
  
    for( int k= m_LedMax - 1; k >= 2; k--) 
    {
      this->heat[k] = (this->heat[k - 1] + this->heat[k - 2] + this->heat[k - 2] ) / 3;
    }
    
    if( random8() < spaking ) 
    {
      int y = random8(7);
      this->heat[y] = qadd8( this->heat[y], random8(160,255) );
    }

    for( int j = 0; j < m_LedMax; j++)
    {
        byte colorindex = scale8( this->heat[j], 240);
        CRGB color = ColorFromPalette( firePalette, colorindex);
        int pixelnumber;

        if( revese ) 
        {
            pixelnumber = (m_LedMax - 1) - j;
        } 
        else 
        {
            pixelnumber = j;
        }

        m_cRGB[pixelnumber] = color;
    }

    delete this->heat;

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


/** --------------------------------------------------
 * @brief  煌めき箇所をランダムにセット
 * @note   
 * @param  chanceOfGlitter: 
 * @retval None
 --------------------------------------------------*/
void NeoPixcelCtrl::addGlitter(fract8 chanceOfGlitter)
{
  if( random8() < chanceOfGlitter) {
    m_cRGB[ random16(m_LedMax) ] += CRGB::White;
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
    for (int i = 0; i < m_LedMax; i++){
        m_cRGB[i] = CRGB(red, green, blue);
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
    m_Status = LedStatus::led_Finish;
    m_LedFocus = m_chageCount = 0;
    m_doStrobe = false;
    m_LightEnable = true;
    m_waitTime = 0;

    if(ledClear)
        clear();
}

/** --------------------------------------------------
 * @brief  LED フォーカスのカウントアップ
 * @note   
 * @param  dtTime: 設定時間
 * @retval カウントアップOK　true
 --------------------------------------------------*/
bool NeoPixcelCtrl::countUpLedFocus(int dtTime)
{
    bool countUp = false;

    if(this->passedTime(m_waitTime))
    {
        m_LedFocus ++;
        m_waitTime = millis() + dtTime;
        countUp =  true;
    }

    return countUp;
}


/** --------------------------------------------------
 * @brief  LEDエフェクト開始時の変数リセット
 * @note   
 * @param  brightless: 照度 
 * @param  delaytime: 遅延時間 
 * @retval None
 --------------------------------------------------*/
void NeoPixcelCtrl::startUp(uint8_t brightless, int delaytime)
{
    this->resetLed();
    m_Status = LedStatus::led_Running;
    m_neoPix.setBrightness(brightless);
    m_waitTime = (delaytime == 0) ? 0 : millis() + delaytime ;
}