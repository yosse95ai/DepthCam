#include "hedder.h";

class DepthSensor
{
public:
    void initialize()
    {
        // デバイスを取得
        openni::Status ret = device.open(openni::ANY_DEVICE);
        if (ret != openni::STATUS_OK) {
            throw std::runtime_error("openni::Device::open() failed.");
        }

        // カラーストリームを有効にする
        colorStream.create(device, openni::SENSOR_COLOR);
        //cangeResolution(colorStream);
        colorStream.start();
    }

    // フレームの更新処理
    void update()
    {
        openni::VideoFrameRef colorFrame;

        // 更新されたフレームの取得
        colorStream.readFrame(&colorFrame);

        // フレームデータを表示できる形に変換する
        frame = showColorStream(colorFrame);

        //二値化処理(マスクの作成)
        cv::Mat green = green_detect(frame);

        //ブロブ解析の結果を取得
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

        // フレームデータを表示する
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

    // カラーストリームを表示できる形に変換する
    cv::Mat showColorStream(const openni::VideoFrameRef& colorFrame)
    {
        // OpeCVの形にする
        cv::Mat frame = cv::Mat(colorFrame.getHeight(), colorFrame.getWidth(),
            CV_8UC3, (unsigned char*)colorFrame.getData());

        // BGR 2 RGB
        cv::cvtColor(frame, frame, cv::COLOR_RGB2BGR);

        return frame;
    }


    openni::Device device;              // 使用するデバイス
    openni::VideoStream colorStream;    // カラーストリーム
    cv::Mat frame;                 // 表示用データ

};

int main(int argc, const char* argv[])
{
    try {
        // OpenNIを初期化
        openni::OpenNI::initialize();

        // センサーを初期化する
        DepthSensor sensor;
        sensor.initialize();

        // メインループ
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