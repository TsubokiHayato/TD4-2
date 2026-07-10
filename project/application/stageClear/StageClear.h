#pragma once
#include <vector>
#include "rubikCube/BaseRubikCubeState.h" // SixCube / OneCube
#include "WallData.h"                     // WallData / FaceType / HoleType / GridPos

/// <summary>
/// クリア判定モジュール。
///
/// 壁の穴(WallData)とルービックキューブの先端(SixCube)は面インデックスの規約が
/// 異なるため、名前では突き合わせできない。そこで両者を同じワールド空間の整数
/// サーフェス座標 (x,y,z) ∈ {-1,0,1}^3 に射影して対応付ける。
///
/// - 先端セル (面i, row, col) の座標は RubikCube の先端配置ロジックと一致させる。
/// - 穴 (FaceType, localPos) の座標は StageBuilder::GetWorldPos を cellSize=1・
///   margin=0 相当に正規化したもの。
///
/// 判定に使う穴の面は、壁を描画するのと同じ StageBuilder(WallData)から取ること。
/// (CubeMapConverter と StageBuilder は Front/Back の面名が入れ替わっているため、
///  描画と判定のソースを混ぜると位置がずれる。)
/// </summary>
class StageClear {
public:
    /// <summary>
    /// 壁データから「先端が必要な位置」(=クリアに要る先端配置)を生成する。
    /// holeType が None の壁(穴なし)は先端を要求しない。
    /// 返り値の各セルは 0=先端不要 / 1=先端が必要。
    /// </summary>
    static SixCube BuildRequired(const std::vector<WallData>& walls, int cubeSize = 3);

    /// <summary>
    /// 展開図セル(StageBuilder に通した WallData)から、キューブの先端配置を作る。
    /// BuildRequired と違い、穴の有無に関係なく**有効セルすべてを先端(=1)**にする。
    /// (キューブCSVは「先端の有無」を表すため。値2/3も先端として扱う。)
    /// </summary>
    static SixCube BuildCubeState(const std::vector<WallData>& cells, int cubeSize = 3);

    /// <summary>
    /// すべての穴が先端で塞がれていればクリア。
    /// required==1 の全セルについて current の先端が存在(>=1)することを要求する。
    /// (穴のない場所に余分な先端があってもクリア可能。厳密一致にしたい場合は
    ///  下の等値比較に置き換えること。)
    /// </summary>
    static bool IsClear(const SixCube& current, const SixCube& required);

    /// <summary>
    /// ImGui で「必要な位置」と現在の先端の一致状況を可視化する。
    /// </summary>
    static void DrawDebug(const SixCube& current, const SixCube& required);

private:
    struct Coord { int x, y, z; };

    /// 先端セル (面i, row, col) → 整数サーフェス座標 (RubikCube の配置と一致)
    static Coord CubeCellToCoord(int face, int row, int col);

    /// 穴 (FaceType, localPos) → 整数サーフェス座標 (StageBuilder の配置と一致)
    static Coord HoleToCoord(FaceType face, GridPos local, int cubeSize);

    /// FaceType → 面法線が一致するルービック面インデックス
    static int FaceToCubeIndex(FaceType face);

    /// HoleType → 形状ID (Cone=1 / Square=2 / それ以外=0)。
    /// キューブ先端・穴・描画で同じIDを共有する。
    static int ShapeOf(HoleType type);
};
