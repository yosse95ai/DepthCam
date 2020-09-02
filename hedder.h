#pragma once
#include<iostream>              //std�̗��p
#include<unordered_map>         //�����Ȃ��A�z�z��̗��p
#include <stdexcept>
#include<vector>                //Vector�^�̗��p
#include <opencv2/opencv.hpp>	// OpenCV
#include <OpenNI.h>				// OpenNI

#define GREEN_MINIMUM_SIZE 9                  // �̈�̍ŏ��ʐ�(px)
#define GREEN_FLOOR cv::Scalar(30, 128, 50)      // �ΐF�͈�1
#define GREEN_UPPER cv::Scalar(100, 255, 255)   // �ΐF�͈�2

/*--�\���̐錾--*/
typedef std::unordered_map<std::string, double> dict;   // �����^�̐錾(��������)

/*---�֐���`---*/

cv::Mat green_detect(cv::Mat frame);        // �ΐF�̈�𒊏o����
std::vector<dict> blob_anarysis(cv::Mat frame); // �u���u���

/*--------------*/


/// <summary>
/// �ΐF�̈�𒊏o����
/// </summary>
/// <param name="frame">���摜</param>
/// <returns>��l�摜 (0: ���F�̈�, 255: �ΐF�̈�)</returns>
cv::Mat green_detect(cv::Mat frame)
{
    cv::Mat hsv;            // frame�����̂܂܎g���Əo�͂�HSV�摜�ɂȂ��Ă��܂�

    cv::cvtColor(frame, hsv, cv::COLOR_BGR2HSV);        // HSV�摜�ɕϊ�����
    cv::inRange(hsv, GREEN_FLOOR, GREEN_UPPER, frame);  // �ΐF�̈�o��

    return frame;
}

/// <summary>
/// �u���u��͂��s��
/// </summary>
/// <param name="frame">��l�摜 (0: ���F�̈�, 255: �ΐF�̈�)</param>
/// <returns>�̈�</returns>
std::vector<dict> blob_anarysis(cv::Mat frame)
{
    cv::Mat labels, stats, centroids;           // ���̓o�^��
    int nLab = cv::connectedComponentsWithStats(frame, labels, stats, centroids);   // ���擾

    std::vector<cv::Mat> stat_ = {};            //stats�ۑ��p�z��
    std::vector<int> areas = {};                //�ʐϏ��ۑ��p�z��

    // ���x�����O���̓o�^
    for (int i = 1; i < nLab; ++i)              // �w�i���������ׂĂ̖ʐϏ����Q�Ƃ��Ă���
    {
        int* statsPtr = stats.ptr<int>(i);              // stats�p�|�C���^
        //double* centerPtr = centroids.ptr<double>(i);   //centroids�p�|�C���^

        int area = statsPtr[cv::ConnectedComponentsTypes::CC_STAT_AREA];
        if (area > GREEN_MINIMUM_SIZE)            // GREEN_MINIMUM_SIZE��菬���ȗ̈�͖���
        {
            stat_.push_back(stats.row(i));
            areas.push_back(area);
        }
    }

    std::vector<dict> ret = {  };
    int loop = 0;
    // 2�ȉ��̍ő�l�𒊏o
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
        auto detail_itr = std::max_element(areas.begin(), areas.end());     // areas�ɑ΂���C�e���[�^
        int max_index = std::distance(areas.begin(), detail_itr);           // areas���̍ő�l���擾
        int* max_stats = stat_[max_index].ptr<int>(0);
        ret.push_back({     //�����i�[����
                {"left", max_stats[cv::ConnectedComponentsTypes::CC_STAT_LEFT] },       // ����x��
                {"top", max_stats[cv::ConnectedComponentsTypes::CC_STAT_TOP] },         // ����y��
                {"height", max_stats[cv::ConnectedComponentsTypes::CC_STAT_HEIGHT] },   // �̈�̍���
                {"width", max_stats[cv::ConnectedComponentsTypes::CC_STAT_WIDTH] },     // �̈�̕�
                {"area", areas[max_index] }                                             // �̈�ʐ�
            });
        areas[max_index] = 0;

    }
    return ret;
}