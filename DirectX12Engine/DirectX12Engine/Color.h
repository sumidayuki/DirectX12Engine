#pragma once

//---------------------------------------------------------------------------------------------------------------------------------------------
// カラー
// 
//  ・「光の三原色(RGB)」を用いたコンピューター内部での色の表現方法。
//  
//  ・「R」は赤色成分で 0.0 ～ 1.0 の値で指定する。
//  ・「G」は緑色成分で 0.0 ～ 1.0 の値で指定する。
//  ・「B」は青色成分で 0.0 ～ 1.0 の値で指定する。
//  ・「A」は  α成分で 0.0 ～ 1.0 の値で指定する。
//  
//  ・α成分は一般的に「不透明度」として利用されるが、それ以外の用途で使用されることもある。
//
//---------------------------------------------------------------------------------------------------------------------------------------------
class Color
{
public:
    // よく知られた色 (Well-known colors)
    static const Color clear;                       // クリア:透明(0, 0, 0, 0)
    static const Color water;                       // ウォーター:水色
    static const Color aliceBlue;                   // アリスブルー(240, 248, 255, 255)
    static const Color antiqueWhite;                // アンティークホワイト(250, 235, 215, 255)
    static const Color aqua;                        // アクア:水浅葱 (255, 0, 255, 255)
    static const Color aquamarine;                  // アクアマリン:藍玉,青緑 (127, 255, 212, 255)
    static const Color azure;                       // アジュール:紺碧 (255, 240, 255, 255)
    static const Color beige;                       // ベージュ:白茶 (220, 245, 245, 255)
    static const Color bisque;                      // べスク (255, 228, 196, 255)
    static const Color black;                       // ブラック:黒 (0, 0, 0, 255)
    static const Color blanchedAlmond;              // ブランチドアーモンド (255, 235, 205, 255)
    static const Color blue;                        // ブルー:青 (255, 0, 0, 255)
    static const Color blueViolet;                  // ブルーバイオレット:青菫 (138, 43, 226, 255)
    static const Color brown;                       // ブラウン:茶 (165, 42, 42, 255)
    static const Color burlyWood;                   // バーリーウッド:堅木 (222, 184, 135, 255)
    static const Color cadetBlue;                   // カデットブルー:国防 (95, 158, 160, 255)
    static const Color chartreuse;                  // シャルトルーズ:薄黄緑 (127, 255, 0, 255)
    static const Color chocolate;                   // チョコレート (210, 105, 30, 255)
    static const Color coral;                       // コーラル:珊瑚 (255, 127, 80, 255)
    static const Color cornflowerBlue;              // コーンフラワーブルー:矢車菊 (100, 149, 237, 255)
    static const Color cornsilk;                    // コーンシルク:小麦絹 (255, 248, 220, 255)
    static const Color crimson;                     // クリムゾン:深紅 (220, 20, 60, 255)
    static const Color cyan;                        // シアン:青緑 (255, 0, 255, 255)
    static const Color darkBlue;                    // ダークブルー:暗青 (0, 0, 139, 255)
    static const Color darkCyan;                    // ダークシアン:暗青緑 (0, 139, 139, 255)
    static const Color darkGoldenrod;               // ダークゴールデンロッド:暗秋麒麟草 (184, 134, 11, 255)
    static const Color darkGray;                    // ダークグレイ:暗灰 (169, 169, 169, 255)
    static const Color darkGreen;                   // ダークグリーン:暗緑 (0, 100, 0, 255)
    static const Color darkKhaki;                   // ダークカーキ (189, 183, 107, 255)
    static const Color darkMagenta;                 // ダークマゼンタ:暗紫紅 (139, 0, 139, 255)
    static const Color darkOliveGreen;              // ダークオリーブグリーン:暗黄緑 (85, 107, 47, 255)
    static const Color darkOrange;                  // ダークオレンジ (255, 140, 0, 255)
    static const Color darkOrchid;                  // ダークオーキッド:暗紫 (153, 50, 204, 255)
    static const Color darkRed;                     // ダークレッド:暗赤 (139, 0, 0, 255)
    static const Color darkSalmon;                  // ダークサーモン:暗鮭 (233, 150, 122, 255)
    static const Color darkSeaGreen;                // ダークシーグリーン:暗海緑 (143, 188, 139, 255)
    static const Color darkSlateBlue;               // ダークスレートブルー:暗青石 (72, 61, 139, 255)
    static const Color darkSlateGray;               // ダークスレートグリーン:暗灰石 (47, 79, 79, 255)
    static const Color darkTurquoise;               // ダークターコイズ:暗青緑 (0, 206, 209, 255)
    static const Color darkViolet;                  // ダークバイオレット:暗菫 (148, 0, 211, 255)
    static const Color deepPink;                    // ディープピンク:深桃 (255, 20, 147, 255)
    static const Color deepSkyBlue;                 // ディープスカイブルー:深青空 (0, 191, 255, 255)
    static const Color dimGray;                     // ディムグレー:薄灰 (105, 105, 105, 255)
    static const Color dodgerBlue;                  // ドッジャーブルー (30, 144, 255, 255)
    static const Color firebrick;                   // ファイヤーブリック:耐火煉瓦 (178, 34, 34, 255)
    static const Color floralWhite;                 // フローラルホワイト:花白 (255, 250, 240, 255)
    static const Color forestGreen;                 // フォレストグリーン:深緑 (34, 139, 34, 255)
    static const Color fuchsia;                     // フクシャ:赤紫 (255, 255, 0, 255)
    static const Color gainsboro;                   // ゲインスボロ:淡灰 (220, 220, 220, 255)
    static const Color ghostWhite;                  // ゴーストホワイト:極白 (255, 248, 248, 255)
    static const Color gold;                        // ゴールド:金 (255, 215, 0, 255)
    static const Color goldenrod;                   // ゴールデンロッド:秋麒麟草 (218, 165, 32, 255)
    static const Color gray;                        // グレー:灰 (128, 128, 128, 255)
    static const Color green;                       // グリーン:緑 (0, 255, 0, 255)
    static const Color greenYellow;                 // グリーンイエロー:緑黄 (173, 255, 47, 255)
    static const Color honeydew;                    // ハニーデュー:甘露 (240, 255, 240, 255)
    static const Color hotPink;                     // ホットピンク:激桃 (255, 105, 180, 255)
    static const Color indianRed;                   // インディアンレッド (205, 92, 92, 255)
    static const Color indigo;                      // インディゴ:藍 (75, 0, 130, 255)
    static const Color ivory;                       // アイボリー:象牙 (255, 255, 240, 255)
    static const Color khaki;                       // カーキ:土埃 (240, 230, 140, 255)
    static const Color lavender;                    // ラベンダー:藤 (230, 230, 250, 255)
    static const Color lavenderBlush;               // ラベンダーブリッシュ:藤紅 (255, 240, 245, 255)
    static const Color lawnGreen;                   // ローングリーン:芝緑 (124, 252, 0, 255)
    static const Color lemonChiffon;                // レモンシフォン:薄檸檬 (255, 250, 205, 255)
    static const Color lightBlue;                   // ライトブルー:薄明青 (173, 216, 230, 255)
    static const Color lightCoral;                  // ライトコーラル:薄明珊瑚 (240, 128, 128, 255)
    static const Color lightCyan;                   // ライトシアン:薄明藍 (255, 224, 255, 255)
    static const Color lightGoldenrodYellow;        // ライトゴールデンロッドイエロー:薄明黄 (250, 250, 210, 255)
    static const Color lightGray;                   // ライトグレー:薄明灰 (211, 211, 211, 255)
    static const Color lightGreen;                  // ライトグリーン:薄明緑 (144, 238, 144, 255)
    static const Color lightPink;                   // ライトピンク:薄明桃 (255, 182, 193, 255)
    static const Color lightSalmon;                 // ライトサーモン:薄明鮭 (255, 160, 122, 255)
    static const Color lightSeaGreen;               // ライトシーグリーン:薄明海緑 (32, 178, 170, 255)
    static const Color lightSkyBlue;                // ライトスカイブルー:薄明青空 (135, 206, 250, 255)
    static const Color lightSlateGray;              // ライトスレートグレー:薄明灰板 (119, 136, 153, 255)
    static const Color lightSteelBlue;              // ライトスチールブルー:薄明青鋼 (176, 196, 222, 255)
    static const Color lightYellow;                 // ライトイエロー:薄明黄 (255, 255, 224, 255)
    static const Color lime;                        // ライム (0, 255, 0, 255)
    static const Color limeGreen;                   // ライムグリーン (50, 205, 50, 255)
    static const Color linen;                       // リニン:亜麻 (250, 240, 230, 255)
    static const Color magenta;                     // マゼンタ:紫紅 (255, 255, 0, 255)
    static const Color maroon;                      // マロン:栗,葡萄茶,海老茶 (128, 0, 0, 255)
    static const Color mediumAquamarine;            // メディアムアクアマリン (102, 205, 170, 255)
    static const Color mediumBlue;                  // メディアムブルー (0, 0, 205, 255)
    static const Color mediumOrchid;                // メディアムオーキッド (186, 85, 211, 255)
    static const Color mediumPurple;                // メディアムパープル (147, 112, 219, 255)
    static const Color mediumSeaGreen;              // メディアムシーグリーン (60, 179, 113, 255)
    static const Color mediumSlateBlue;             // メディアムスレートブルー (123, 104, 238, 255)
    static const Color mediumSpringGreen;           // メディアムスプリンググリーン (0, 250, 154, 255)
    static const Color mediumTurquoise;             // メディアムトーコイズ (72, 209, 204, 255)
    static const Color mediumVioletRed;             // メディアムバイオレットレッド (199, 21, 133, 255)
    static const Color midnightBlue;                // ミッドナイトブルー:真夜青 (25, 25, 112, 255)
    static const Color mintCream;                   // ミントクリーム (245, 255, 250, 255)
    static const Color mistyRose;                   // ミスティーローズ:薄薔薇 (255, 228, 225, 255)
    static const Color moccasin;                    // モカシン (255, 228, 181, 255)
    static const Color navajoWhite;                 // ナバホホワイト (255, 222, 173, 255)
    static const Color navy;                        // ネイビー (0, 0, 128, 255)
    static const Color oldLace;                     // オールドレイス (253, 245, 230, 255)
    static const Color olive;                       // オリーブ:阿列布 (128, 128, 0, 255)
    static const Color oliveDrab;                   // オリーブドラブ　(107, 142, 35, 255)
    static const Color orange;                      // オレンジ:橙 (255, 165, 0, 255)
    static const Color orangeRed;                   // オレンジレッド:赤橙 (255, 69, 0, 255)
    static const Color orchid;                      // オーキッド:蘭 (218, 112, 214, 255)
    static const Color paleGoldenrod;               // ペールゴールデンロッド:薄金赤 (238, 232, 170, 255)
    static const Color paleGreen;                   // ペールグリーン:薄緑 (152, 251, 152, 255)
    static const Color paleTurquoise;               // ペールターコイズ:薄青緑 (175, 238, 238, 255)
    static const Color paleVioletRed;               // ペールバイオレットレッド:薄赤菫 (219, 112, 147, 255)
    static const Color papayaWhip;                  // パパイヤホイップ (255, 239, 213, 255)
    static const Color peachPuff;                   // ピーチパフ (255, 218, 185, 255)
    static const Color peru;                        // ペルー (205, 133, 63, 255) 
    static const Color pink;                        // ピンク:撫子 (255, 192, 203, 255) 
    static const Color plum;                        // プラム:西洋李 (221, 160, 221, 255)
    static const Color powderBlue;                  // パウダーブルー:淡青 (176, 224, 230, 255)
    static const Color purple;                      // パープル:紫 (128, 0, 128, 255)
    static const Color red;                         // レッド:赤 (255, 0, 0, 255)
    static const Color rosyBrown;                   // ロージーブラウン:薔薇茶 (188, 143, 143, 255)
    static const Color royalBlue;                   // ロイヤルブルー:紺青 (65, 105, 225, 255)
    static const Color saddleBrown;                 // サドルブラウン:馬鞍 (139, 69, 19, 255)
    static const Color salmon;                      // サーモン:鮭 (250, 128, 114, 255)
    static const Color sandyBrown;                  // サンディーブラウン:砂茶 (244, 164, 96, 255)
    static const Color seaGreen;                    // シーグリーン:海緑 (46, 139, 87, 255)
    static const Color seaShell;                    // シーシェル:貝殻 (255, 245, 238, 255)
    static const Color sienna;                      // シエンナ (160, 82, 45, 255)
    static const Color silver;                      // シルバー:銀 (192, 192, 192, 255)
    static const Color skyBlue;                     // スカイブルー:空 (135, 206, 235, 255)
    static const Color slateBlue;                   // スレートブルー:石瓦 (106, 90, 205, 255)
    static const Color slateGray;                   // スレートグレー:灰石板 (112, 128, 144, 255)
    static const Color snow;                        // スノー:雪 (255, 250, 250, 255)
    static const Color springGreen;                 // スプリンググリーン:春緑 (0, 255, 127, 255)
    static const Color steelBlue;                   // スチールブルー:青鋼 (70, 130, 180, 255)
    static const Color tan;                         // タン:日焼け (210, 180, 140, 255)
    static const Color teal;                        // ティール:青緑 (0, 128, 128, 255)
    static const Color thistle;                     // シスル:薊 (216, 191, 216, 255)
    static const Color tomato;                      // トマト (255, 99, 71, 255)
    static const Color transparentBlack;            // トランスペアレントブラック:透過黒 (0, 0, 0, 0)
    static const Color transparentWhite;            // トランスペアレントホワイト:透過白 (255, 255, 255, 0)
    static const Color turquoise;                   // ターコイズ (64, 224, 208, 255)
    static const Color violet;                      // バイオレット:菫 (238, 130, 238, 255)
    static const Color wheat;                       // フィート:小麦 (245, 222, 179, 255)
    static const Color white;                       // ホワイト:白 (255, 255, 255, 255)
    static const Color whiteSmoke;                  // ホワイトスモーク:白煙 (245, 245, 245, 255)
    static const Color yellow;                      // イエロー:黄 (255, 255, 0, 255)
    static const Color yellowGreen;                 // イエローグリーン:黄緑 (154, 205, 50, 255)

public:
    union
    {
        // 成分表現
        struct
        {
            float r;    // 赤成分
            float g;    // 緑成分
            float b;    // 青成分
            float a;    // α成分
        };

        // 配列表現
        float components[4];
    };

public:
    // 各成分32ビット浮動小数点数値で構成されたRGBA形式の新しい色を作成します。
    // (各成分の値は初期化されません)
    Color() = default;

    // RGBの値を指定してRGBA形式の新しい色を作成します。
    // (Aの値は1.0に設定されます)
    Color(float r, float g, float b);

    // RGBAの値を指定してRGBA形式の新しい色を作成します。
    Color(float r, float g, float b, float a);

    // RGBの値を指定してRGBA形式の新しい色を作成します。
    // (Aの値は1.0に設定されます)
    Color(const Vector3& rgb);

    // RGBAの値を指定してRGBA形式の新しい色を作成します。
    Color(const Vector4& rgba);

    // RGBAの値を指定してRGBA形式の新しい色を作成します。
    Color(const float components[4]);

    // キャスト演算子のオーバーロード
    operator const float* () const { return components; }

    // 線形補間
    static Color LerpUnclamped(const Color& from, const Color& to, float t);

    // 単項プラス演算子
    const Color& operator + () const;

    // 単項マイナス演算子
    Color operator - () const;

    // 各色成分を加算します。
    Color& operator += (const Color& rhs);

    // 各色成分を減算します。
    Color& operator -= (const Color& rhs);

    // 各色成分を乗算します。
    Color& operator *= (const Color& rhs);

    // 各色成分をスカラー値で乗算します。
    Color& operator *= (float scaleFactor);

    // 各色成分を除算します。
    Color& operator /= (const Color& rhs);

    // 各色成分をスカラー値で除算します。
    Color& operator /= (float divisor);
};

// 各色成分を減算した新しいColorを返します。
Color operator + (const Color& lhs, const Color& rhs);

// 各色成分を加算した新しいColorを返します。
Color operator - (const Color& lhs, const Color& rhs);

// 各色成分を乗算した新しいColorを返します。
Color operator * (const Color& lhs, const Color& rhs);

// 各色成分をスカラー値で乗算した新しいColorを返します。
Color operator * (const Color& lhs, float scaleFactor);

// 各色成分をスカラー値で乗算した新しいColorを返します。
Color operator * (float scaleFactor, const Color& rhs);

// 各色成分を除算した新しいColorを返します。
Color operator / (const Color& lhs, const Color& rhs);

// 各色成分をスカラー値で除算した新しいColorを返します。
Color operator / (const Color& lhs, float divisor);

// 対応する色成分が等しい場合は true を返します。
bool operator == (const Color& lhs, const Color& rhs);

// 対応する色成分が異なる場合は true を返します。
bool operator != (const Color& lhs, const Color& rhs);

