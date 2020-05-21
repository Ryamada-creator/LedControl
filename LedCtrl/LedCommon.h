#ifndef _LED_COMMON_H_
#define _LED_COMMON_H_


/* -------------------------------------------------------------------------- */
/*                                    定数宣言                                  */
/* -------------------------------------------------------------------------- */
#define _DEBUG_MASSEGE  (0)                     // 有効 : 1 シリアル文字列を表示
#define _DEBUG_LED_INFO (1)                     // 有効 : 1 LEDの各種設定、情報を表示 
#define COMMAND_LIST    (26)                    // コマンド数 
#define LED_PIN         (4)                    // for arduino
//#define LED_PIN         (A4)
#define LED_NUM         (60)                    // LED 点灯個数

/* -------------------------------------------------------------------------- */
/*                                 テンプレートマクロ定義                                */
/* -------------------------------------------------------------------------- */
#define arrySize(x) (sizeof(x)/sizeof(x[0]))    // 配列の要素数を返す


// cout << を利用するための定義
struct _stream_tag{};

template<typename T>_stream_tag const& operator << (_stream_tag const& os, T const& t) 
{
    Serial.print(t);
    return os;
}
static constexpr _stream_tag cout;
static constexpr auto endl = '\n';


#endif