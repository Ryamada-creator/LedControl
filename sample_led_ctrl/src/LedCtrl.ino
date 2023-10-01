
/* _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
    
    [概要]
        シリアルコマンドにて　LEDを点灯させるソフト
    
        シリアルコマンド数　Max:35個
        コマンドは全て数値で送信する事

    [コマンドリスト]
     < 0 ~ 9 >
    　・　カラーセレクト
        0  : 黒   
        1  : 赤   
        2  : オレンジ　  
        3  : 黄色
        4  : 緑   
        5  : 青   
        6  : 藍        
        7  : ピンク 
        8  : 白   
        9  : 紫

     < 10 ~ 35 >
    　・　エフェクトリスト
        10 : Blink(点滅)　 　　　
        11 : Fade(点灯→消灯)　 　　
        12 : Cycron(七色往復)
        13 : TurnRainbow(七色wipe)  　
        14 : RainbowCycle(虹色シフト)  　　　
        15 : Rainbow(虹色)
        16 : StripOneColor(区切って点灯)  
        17 : StripRainbow(区切って七色にシフト)  
        18 : glitterColor(キラキラ)
        19 : bpm(七色シフト)　 
        20 : wipe(順次点灯)  
        21 : collision(衝突)
        22 : charge(積み重ね)  
        23 : sinelon(小さな炎)  
        24 : juggle(ジャグリング)
        25 : confetti(パラパラ七色) 
        26 : fire(炎)
        27 : blueFire(青い炎)
        28 : scanner 単体のLED浮遊
        29 : round_Shift 往復シフトカラー
        30 : round_blackShift　往復消灯シフト
        31 : dual_Shift  双方向のLEDシフト
        32 : dual_blackShift　双方向の消灯シフト
        33 : strobe フラッシュ
        34 : interior インテリア
        35 : 消灯

        TODO: Seeeduino 対応可 (Nano では動作確認済み)


_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/*/

#include <Arduino.h>

#include "INA226.h"
#include "Wire.h"

#include "LedCtrlSample/NeoPixcelCtrl.h"
#include "LedCtrlSample/LedCommon.h"

/* -------------------------------------------------------------------------- */
/*                                 クラス　インスタンス                                 */
/* -------------------------------------------------------------------------- */
CFastLED neop;                                  // FastLED Class
CRGB leds[LED_NUM];                             // CRGB Class
NeoPixcelCtrl n_ctrl;                           // NeoPixcelCtrl Class

INA226 INA(0x40);

/* -------------------------------------------------------------------------- */
/*                                   グローバル変数                                  */
/* -------------------------------------------------------------------------- */
uint8_t g_Request = 0;                          // LED アクション選択番号
bool g_ledEnable = true;                        // LED 点灯許可

enum LED_MODE {Exhibition, Remote};             // Exhibition : 定期的にエフェクトを自動で変更
enum LED_MODE g_Mode = LED_MODE::Exhibition; 

/* -------------------------------------------------------------------------- */
/*                                   セットアップ                                   */
/* -------------------------------------------------------------------------- */
void setup() 
{
    Serial.begin(115200);

    cout << F("\n\nStarting Led Effecter ... \n");

    pinMode(LED_PIN, OUTPUT);

    n_ctrl.begin(neop, leds, LED_NUM);
    neop.addLeds<NEOPIXEL, LED_PIN>(leds, LED_NUM);

    n_ctrl.resetLed(true);


    Wire.begin();
    if (!INA.begin() )
    {
        Serial.println("could not connect. Fix and Reboot");
    }
    INA.setMaxCurrentShunt(1, 0.002);

    cout << F("Setup Done !") << endl;
}

/* -------------------------------------------------------------------------- */
/*                                   メインループ                                   */
/* -------------------------------------------------------------------------- */
void loop()
{  
    // シリアル通信確認
    checkUART();

    // LED Effect 実行
    if(g_ledEnable) 
    {
        if(ledLightning(g_Request) && g_Mode == LED_MODE::Exhibition)
        {   
            g_Request++;
            if(g_Request == 25)
                g_Request = 0;
        }
    }

    printf("%fV,%fA\n", 
           static_cast<double>(INA.getShuntVoltage_mV()),
           static_cast<double>(INA.getCurrent_mA()));
}

/** --------------------------------------------------
 * @brief  シリアルバッファに文字列が受信されているか確認し、行動する
 * @note   
 * @retval None
 --------------------------------------------------*/
void checkUART()
{
    String str = "";

    // シリアルバッファのバイト確認
    if(Serial.available() > 0)
    {
        // 文字列取得
        str = Serial.readStringUntil('\n');
        str.trim();
    }

    if(str == "")
        return;

    #if (_DEBUG_MASSEGE != 0)
        cout << F("[ Serial ] Command : ") << str;
    #endif

    // シリアルコマンドフロー
    if(UARTEvent(str))
    {
        // LED の情報を表示
        mainControllerInformation();
    }

}

/** --------------------------------------------------
 * @brief  シリアルコマンドによるフロー
 * @note   
 * @param  str: シリアルコマンド
 * @retval 該当するコマンドがあれば True
 --------------------------------------------------*/
bool UARTEvent(String str)
{
    bool ret = true;

    // Exhibition モード LEDエフェクトを順次変更
    if(str == "ex")
    {
        g_Mode = LED_MODE::Exhibition;
        g_Request = 0;
        g_ledEnable = true;
    }
    // Remote コマンド
    else if(isDigit(str[0]))
    {
        uint8_t actNo = str.toInt(); 

        // 数値であるか、コマンドリストに対応しているか
        if(actNo <= COMMAND_LIST)
        {
            // シリアルコマンドに応じてLED設定変更
            actionCommand(actNo);
        }
    }
    else
    {
        cout << F(" -> Unkhown Command\n");
        ret = false;
    }

    cout << endl;

    return ret;
}

/** --------------------------------------------------
 * @brief  LED　エフェクト　リスト
 * @note   
 * @param  effectNo: 実行したいエフェクトナンバー
 * @retval LED エフェクト完了したらTrue
 --------------------------------------------------*/
bool ledLightning(uint8_t effectNo)
{
    static uint8_t prevEffectNo = 100, endEffectCount = 0, exeCount = 0;

    bool led_Complete = false;

    // エフェクトが変更された時は初期化処理
    if(effectNo != prevEffectNo)
    {
        // 初期化
        prevEffectNo = effectNo; 
        endEffectCount = exeCount = 0;
    }

    // LED エフェクト実行
    switch (effectNo)
    {
        // 点滅　
        case 0:
            if(endEffectCount == 0) endEffectCount = 3;
            if(n_ctrl.blink(150, 700))
                exeCount ++;
            break;   

        // 点灯　→　消灯
        case 1:
            if(!endEffectCount) endEffectCount = 6;
            if(n_ctrl.fade(150, 2))
                exeCount ++;
            break;

        // 七色に往復
        case 2:
            if(!endEffectCount) endEffectCount = 10;
            if(n_ctrl.cycron(150, 2))
                exeCount ++;
            break;

        // 七色wipe
        case 3:
            if(!endEffectCount) endEffectCount = 7;
            if(n_ctrl.turnRainbow(150, 10, true))
                exeCount ++;
            break;

        // 虹色シフト
        case 4:
            if(!endEffectCount) endEffectCount = 1;
            if(n_ctrl.rainbowCycle(150, 30, 500))
                exeCount ++;
            break;

        // 虹色
        case 5:
            if(!endEffectCount) endEffectCount = 2;
            if(n_ctrl.rainbow(150, 20))
                exeCount ++;
            break;

        // 区切って点灯
        case 6:
            if(!endEffectCount) endEffectCount = 1;
            if(n_ctrl.stripOneColor(150, 10, 500, true, false))
                exeCount ++;
            break;

        // 区切って七色にシフト
        case 7:
            if(!endEffectCount) endEffectCount = 1;
            if(n_ctrl.stripRainbow(150, 10, 500, true, false))
                exeCount ++;
            break;

        // キラキラ
        case 8:
            if(!endEffectCount) endEffectCount = 3;
            if(n_ctrl.glitterColor(10, 200))
                exeCount ++;
            break;

        // bpm 七色シフト
        case 9:
            if(!endEffectCount) endEffectCount = 2;
            if(n_ctrl.bpm(150, 10, 500))
                exeCount ++;
            break;

        // 順次点灯
        case 10:
            if(!endEffectCount) endEffectCount = 8;
            if(n_ctrl.wipe(150, 30, true))
            {
                n_ctrl.resetLed(true);
                exeCount ++;
            }
            break;

        // 衝突
        case 11:
            if(!endEffectCount) endEffectCount = 4;
            if(n_ctrl.collision(15))
                exeCount ++;
            break;

        // 積み重ね
        case 12:
            if(!endEffectCount) endEffectCount = 3;
            if(n_ctrl.charge(150, 2, true))
            {
                n_ctrl.resetLed(true);
                exeCount ++;
            }
            break;

        // 蕩々と　小さな炎の様に
        case 13:
            if(!endEffectCount) endEffectCount = 100;
            if(n_ctrl.sinelon(150,4))
                exeCount ++;
            break;

        // ジャグリング
        case 14:
            if(!endEffectCount) endEffectCount = 10;
            if(n_ctrl.juggle(150, 0, 500))
                exeCount ++;
            break;

        // パラパラ七色
        case 15:
            if(!endEffectCount) endEffectCount = 10;
            if(n_ctrl.confetti(150, 0, 500))
                exeCount ++;
            break;

        // 炎
        case 16:
            if(!endEffectCount) endEffectCount = 3;
            if(n_ctrl.fire(200, 55, 120, 0))
                exeCount ++;
            break;

        // 青い炎
        case 17:
            if(!endEffectCount) endEffectCount = 3;
            if(n_ctrl.blueFire(200, 55, 120, 0))
                exeCount ++;
            break;

        // scanner LEDの柔らかな移動
        case 18:
            if(!endEffectCount) endEffectCount = 100;
            if(n_ctrl.scanner(150, 0))
                exeCount ++;
            break;

        //　往復シフト
        case 19:
            if(!endEffectCount) endEffectCount = 8;
            if(n_ctrl.round_Shift(150, 15))
                exeCount ++;
            break;

        //　往復消灯シフト
        case 20:
            if(!endEffectCount) endEffectCount = 8;
            if(n_ctrl.round_blackShift(150, 15))
                exeCount ++;
            break;

        // 双方向のシフト
        case 21:
            if(!endEffectCount) endEffectCount = 5;
            if(n_ctrl.dual_Shift(150, 15))
                exeCount ++;
            break;

        // 双方向の消灯シフト
        case 22:
            if(!endEffectCount) endEffectCount = 5;
            if(n_ctrl.dual_blackShift(150, 15))
                exeCount ++;
            break;


        // ストロボ　フラッシュを繰り返し
        case 23:
            if(!endEffectCount) endEffectCount = 4;
            if(n_ctrl.strobe(200, 3, 40, 1000))
                exeCount ++;
            break;

        // インテリア
        case 24:
            if(!endEffectCount) endEffectCount = 3;
            if(n_ctrl.interior(200, 3000))
                exeCount ++;
            break;

        // LED 消灯
        default:
            n_ctrl.resetLed(true);
            g_ledEnable = false;
            break;
    }

    //指定した回数エフェクトを実行したら
    if(endEffectCount <= exeCount)
    {
        // リセット処理
        n_ctrl.resetLed(true);
        endEffectCount = exeCount = 0;
        led_Complete = true;
    }

    return led_Complete;

}


/** --------------------------------------------------
 * @brief  シリアルコマンドに応じて行動する
 * @note   
 * @param  Command: シリアルコマンド
 * @retval None
 --------------------------------------------------*/
void actionCommand(const uint8_t &actionNo)
{

    if(actionNo > COMMAND_LIST)
        return;

    //　LEDカラーをセット
    if(actionNo < 10)
    {
        // LEDの色をセット
        n_ctrl.setColor(static_cast<ledColors>(actionNo));
    }

    // LEDのエフェクト点灯
    if(actionNo >= 10) 
    {
        n_ctrl.resetLed(true);

        // Remote へ移行
        g_Mode = LED_MODE::Remote;

        // LED 許可
        g_ledEnable = ( COMMAND_LIST == actionNo ) ? false : true;
        g_Request = actionNo - 10;
    }
}


/** --------------------------------------------------
 * @brief  split 関数
 * @note   
 * @param  data: 分割したい文字列
 * @param  delimit: 分割ポイント
 * @param  *dst: 格納する配列変数
 * @retval 配列要素数
 --------------------------------------------------*/
int split(String data, char delimit, String *dst)
{
    int idx = 0;
    int arrSize = (sizeof(data)/sizeof((data)[0]));  
    int dlen = data.length();

    for (int i = 0; i < dlen; i++) 
    {
        char tmp = data.charAt(i);

        if ( tmp == delimit ) 
        {
            idx++;
            if ( idx > (arrSize - 1)) return -1;
        }
        else 
            dst[idx] += tmp;
    }

    return (idx + 1);
}


/** --------------------------------------------------
 * @brief  Led 情報表示
 * @note   
 * @retval None
 --------------------------------------------------*/
void mainControllerInformation()
{
    const String effectMember[] = {"Blink", "Fade", "Cycron", "turnRainbow", "rainbowCycle",
                                   "rainbow", "stripOneColor", "stripRainbow", "glitterColor",
                                   "bpm", "wipe", "collision", "charge", "sinelon", "juggle", 
                                   "confetti", "Fire", "BlueFire","scanner","round_Shift",
                                   "round_blackShift", "dual_Shift", "dual_blackShift","Strobe", 
                                   "interior", "Reset LED"};

    uint8_t red = 0, green = 0, blue = 0;
    n_ctrl.getRGB(red, green, blue);

    cout << F("\n< LED Information >\n");

    cout << F(" Effect No.  : ") << g_Request << endl
         << F(" Effect Name : ") << effectMember[g_Request] << endl
         << F(" LED RGB     : ") 
         << red<< F(", ") << green << F(", ") << blue << endl
         << F(" LED Enable  : ") << String(( g_ledEnable ) ? "ON\n" : "OFF\n")
         << F(" LED Mode    : ") 
         << String( (g_Mode==LED_MODE::Exhibition) ? "Exhibition\n" : "Remote\n")
         << endl;
}
