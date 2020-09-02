#include "hedder.h";

class DepthSensor
{
public:
    void initialize()
    {
        // �f�o�C�X���擾
        openni::Status ret = device.open(openni::ANY_DEVICE);
        if (ret != openni::STATUS_OK) {
            throw std::runtime_error("openni::Device::open() failed.");
        }

        // �J���[�X�g���[����L���ɂ���
        colorStream.create(device, openni::SENSOR_COLOR);
        //cangeResolution(colorStream);
        colorStream.start();
    }

    // �t���[���̍X�V����
    void update()
    {
        openni::VideoFrameRef colorFrame;

        // �X�V���ꂽ�t���[���̎擾
        colorStream.readFrame(&colorFrame);

        // �t���[���f�[�^��\���ł���`�ɕϊ�����
        frame = showColorStream(colorFrame);

        //��l������(�}�X�N�̍쐬)
        cv::Mat green = green_detect(frame);

        //�u���u��͂̌��ʂ��擾
        std::vector<dict> target = blob_anarysis(green);

        if (target[0]["FLAG"] == 2)
        {
            dict stick1 = target[1];
            dict stick2 = target[2];
            cv::rectangle(frame, cv::Rect((int)stick1["left"], (int)stick1["top"],
                (int)stick1["width"], (int)stick1["height"]), cv::Scalar(255, 0, 255), 2);
            cv::rectangle(frame, cv::Rect((int)stick2["left"], (int)stick2["top"],
                (int)stick2["width"], (int)stick2["height"]), cv::Scalar(255, 0, 255), 2);
        }
        else if (target[0]["FLAG"] == 1)
        {
            cv::rectangle(frame, cv::Rect((int)target[1]["left"], (int)target[1]["top"],
                (int)target[1]["width"], (int)target[1]["height"]), cv::Scalar(255, 0, 255), 2);
        }

        // �t���[���f�[�^��\������
        cv::imshow("ColotStream", frame);
    }

private:
    void changeResolutioin(openni::VideoStream& stream)
    {
        openni::VideoMode mode = stream.getVideoMode();
        mode.setResolution(640, 480);
        mode.setFps(30);
        stream.setVideoMode(mode);
    }

    // �J���[�X�g���[����\���ł���`�ɕϊ�����
    cv::Mat showColorStream(const openni::VideoFrameRef& colorFrame)
    {
        // OpeCV�̌`�ɂ���
        cv::Mat frame = cv::Mat(colorFrame.getHeight(), colorFrame.getWidth(),
            CV_8UC3, (unsigned char*)colorFrame.getData());

        // BGR 2 RGB
        cv::cvtColor(frame, frame, cv::COLOR_RGB2BGR);

        return frame;
    }


    openni::Device device;              // �g�p����f�o�C�X
    openni::VideoStream colorStream;    // �J���[�X�g���[��
    cv::Mat frame;                 // �\���p�f�[�^

};

int main(int argc, const char* argv[])
{
    try {
        // OpenNI��������
        openni::OpenNI::initialize();

        // �Z���T�[������������
        DepthSensor sensor;
        sensor.initialize();

        // ���C�����[�v
        while (1) {
            sensor.update();
            int key = cv::waitKey(10);
            if (key == 'q') {
                break;
            }
        }
    }
    catch (std::exception&) {
        std::cout << openni::OpenNI::getExtendedError() << std::endl;
    }

    return 0;
}