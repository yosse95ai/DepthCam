#pragma once
#include<iostream>              //stdの利用
#include<unordered_map>         //順序なし連想配列の利用
#include <stdexcept>
#include<vector>                //Vector型の利用
#include <opencv2/opencv.hpp>	// OpenCV
#include <OpenNI.h>				// OpenNI

#define GREEN_MINIMUM_SIZE 9                  // 領域の最小面積(px)
#define GREEN_FLOOR cv::Scalar(30, 128, 50)      // 緑色範囲1
#define GREEN_UPPER cv::Scalar(100, 255, 255)   // 緑色範囲2

/*--構造体宣言--*/
typedef std::unordered_map<std::string, double> dict;   // 辞書型の宣言(順序無し)

/*---関数定義---*/

cv::Mat green_detect(cv::Mat frame);        // 緑色領域を抽出する
std::vector<dict> blob_anarysis(cv::Mat frame); // ブロブ解析

/*--------------*/


/// <summary>
/// 緑色領域を抽出する
/// </summary>
/// <param name="frame">元画像</param>
/// <returns>二値画像 (0: 他色領域, 255: 緑色領域)</returns>
cv::Mat green_detect(cv::Mat frame)
{
    cv::Mat hsv;            // frameをそのまま使うと出力もHSV画像になってしまう

    cv::cvtColor(frame, hsv, cv::COLOR_BGR2HSV);        // HSV画像に変換する
    cv::inRange(hsv, GREEN_FLOOR, GREEN_UPPER, frame);  // 緑色領域出力

    return frame;
}

/// <summary>
/// ブロブ解析を行う
/// </summary>
/// <param name="frame">二値画像 (0: 他色領域, 255: 緑色領域)</param>
/// <returns>領域</returns>
std::vector<dict> blob_anarysis(cv::Mat frame)
{
    cv::Mat labels, stats, centroids;           // 情報の登録先
    int nLab = cv::connectedComponentsWithStats(frame, labels, stats, centroids);   // 情報取得

    std::vector<cv::Mat> stat_ = {};            //stats保存用配列
    std::vector<int> areas = {};                //面積情報保存用配列

    // ラベリング情報の登録
    for (int i = 1; i < nLab; ++i)              // 背景を除くすべての面積情報を参照していく
    {
        int* statsPtr = stats.ptr<int>(i);              // stats用ポインタ
        //double* centerPtr = centroids.ptr<double>(i);   //centroids用ポインタ

        int area = statsPtr[cv::ConnectedComponentsTypes::CC_STAT_AREA];
        if (area > GREEN_MINIMUM_SIZE)            // GREEN_MINIMUM_SIZEより小さな領域は無視
        {
            stat_.push_back(stats.row(i));
            areas.push_back(area);
        }
    }

    std::vector<dict> ret = {  };
    int loop = 0;
    // 2個以下の最大値を抽出
    if (areas.size() > 1)
    {
        loop = 2;
        ret.push_back({ {"FLAG", 2} });
    }
    else if (areas.size() > 0)
    {
        loop = 1;
        ret.push_back({ {"FLAG", 1} });
    }
    else
    {
        ret.push_back({ {"FLAG", 0} });
        return ret;
    }
    for (int i = 0; i < loop; i++)
    {
        auto detail_itr = std::max_element(areas.begin(), areas.end());     // areasに対するイテレータ
        int max_index = std::distance(areas.begin(), detail_itr);           // areas内の最大値を取得
        int* max_stats = stat_[max_index].ptr<int>(0);
        ret.push_back({     //情報を格納する
                {"left", max_stats[cv::ConnectedComponentsTypes::CC_STAT_LEFT] },       // 左上x軸
                {"top", max_stats[cv::ConnectedComponentsTypes::CC_STAT_TOP] },         // 左上y軸
                {"height", max_stats[cv::ConnectedComponentsTypes::CC_STAT_HEIGHT] },   // 領域の高さ
                {"width", max_stats[cv::ConnectedComponentsTypes::CC_STAT_WIDTH] },     // 領域の幅
                {"area", areas[max_index] }                                             // 領域面積
            });
        areas[max_index] = 0;

    }
    return ret;
}