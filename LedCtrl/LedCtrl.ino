

/* _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/

    [作成]　　　R yamda
    [作成日]　　2020/5/25
    
    [概要]
        シリアルコマンドにて　LEDを点灯させるソフト
    
        シリアルコマンド数　Max:25個
        コマンドは全て数値で送信する事(　0 〜　25 )

    [コマンドリスト]
    　・　カラーセレクト
        0  : 黒   1  : 赤   2 : オレンジ　  3 : 黄色
        4  : 緑   5  : 青   6 : 藍        7 : ピンク 
        8  : 白   9  : 紫

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
        26 : 消灯

        TODO: Seeeduino 対応可 (Nano では動作確認済み)


_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/*/

#include "LedCommon.h"
#include "NeoPixcelCtrl.h"

/* -------------------------------------------------------------------------- */
/*                                 クラス　インスタンス                                 */
/* -------------------------------------------------------------------------- */
CFastLED neop;                                  // FastLED Class
CRGB leds[LED_NUM];                             // CRGB Class
NeoPixcelCtrl n_ctrl;                           // NeoPixcelCtrl Class


/* -------------------------------------------------------------------------- */
/*                                   グローバル変数                                  */
/* -------------------------------------------------------------------------- */
uint8_t request = 9;                            // LED アクション選択番号
bool led_Enable = false;                        // LED 点灯許可

/* -------------------------------------------------------------------------- */
/*                                   セットアップ                                   */
/* -------------------------------------------------------------------------- */
void setup() 
{

    request = 0;

    Serial.begin(9600);

    cout << F("\n\nStarting Led Effecter ... \n");

    pinMode(LED_PIN, OUTPUT);

    n_ctrl.begin(neop, leds, LED_NUM);
    neop.addLeds<NEOPIXEL,LED_PIN>(leds, LED_NUM);

    n_ctrl.resetLed(true);

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
    if(led_Enable) {
        ledLightning(request);
    }

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


    uint8_t actNo = str.toInt(); 

    // 数値であるか、コマンドリストに対応しているか
    if(!isDigit(str[0]) || actNo > COMMAND_LIST)
    {

        #if (_DEBUG_MASSEGE != 0)
            cout << F(" -> Unkhown Command\n");
        #endif

        return;
    }

    cout << endl;

    // シリアルコマンドに応じてLED設定変更
    actionCommand(actNo);

}


/** --------------------------------------------------
 * @brief  LED　エフェクト　リスト
 * @note   
 * @param  &patturnNo: 実行したいエフェクトナンバー
 * @retval None
 --------------------------------------------------*/
void ledLightning(uint8_t effectNo)
{
    static uint8_t prevEffectNo = 100, endEffectCount = 0, exeCount = 0;

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
            if(!endEffectCount) endEffectCount = 4;
            if(n_ctrl.fade(150, 2))
                exeCount ++;
            break;

        // 七色に往復
        case 2:
            if(!endEffectCount) endEffectCount = 20;
            if(n_ctrl.cycron(150, 5))
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
            if(!endEffectCount) endEffectCount = 2;
            if(n_ctrl.rainbowCycle(150, 30, 500))
                exeCount ++;
            break;

        // 虹色
        case 5:
            if(!endEffectCount) endEffectCount = 2;
            if(n_ctrl.rainbow(150))
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
            if(!endEffectCount) endEffectCount = 1;
            if(n_ctrl.charge(150, 2, true))
            {
                n_ctrl.resetLed(true);
                exeCount ++;
            }
            break;

        // 蕩々と　小さな炎の様に
        case 13:
            if(!endEffectCount) endEffectCount = 50;
            if(n_ctrl.sinelon(150,0))
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

        // LED 消灯
        default:
            n_ctrl.resetLed(true);
            led_Enable = false;
            break;
    }

    //指定した回数エフェクトを実行したら
    if(endEffectCount <= exeCount)
    {
        // リセット処理
        n_ctrl.resetLed(true);
        endEffectCount = exeCount = 0;
    }

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

        // LED 許可
        led_Enable = ( COMMAND_LIST == actionNo ) ? false : true;
        request = actionNo - 10;
    }

    // LED の情報を表示
    #if (_DEBUG_LED_INFO != 0)
        mainControllerInformation();
    #endif
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
                                   "confetti", "Reset LED"};

    uint8_t red = 0, green = 0, blue = 0;
    n_ctrl.getRGB(red, green, blue);

    cout << F("\n< LED Information >\n");

    cout << F(" Effect No.  : ") << request << endl
         << F(" Effect Name : ") << effectMember[request] << endl
         << F(" LED RGB     : ") 
         << red<< F(", ") << green << F(", ") << blue << endl
         << F(" LED Enable  : ") << String(( led_Enable ) ? "ON\n" : "OFF\n")
         << endl;
}
