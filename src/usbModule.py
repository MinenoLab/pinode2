import os
import subprocess
import logging


# loggerを定義
logger = logging.getLogger(__name__)
# loggerのログレベルをINFOに設定
logger.setLevel(logging.INFO)

# loggerのフォーマットを定義
formatter = logging.Formatter('%(asctime)s - %(levelname)s:%(name)s - %(message)s')
# ファイルに出力するためのFileHandlerを定義
file_handler = logging.FileHandler('/home/pi/service.log')
file_handler.setLevel(logging.ERROR)
file_handler.setFormatter(formatter)
# コンソールに出力するためのStreamHandlerを定義
stream_handler = logging.StreamHandler()
stream_handler.setFormatter(formatter)
# loggerにそれぞれのハンドラーを追加
logger.addHandler(file_handler)
logger.addHandler(stream_handler)


### 接続されているUSBデバイスのポートを確認
# RaapberryPi 3b+の場合 (RaspberryPi 4の場合は使用不可）
# 1 -> 左上
# 2 -> 左下
# 3 -> 右上
# 4 -> 右下
def get_usb_device():
    usb_ports = []
    devices = os.listdir('/dev/')
    for device in devices:
        if 'ttyUSB_' in device:
            usb_ports.append(int(device[-1]))
    return usb_ports

### 指定したポートのデバイスの種類を特定（USB CameraかSPRESENSEのみ）
def identify_usb_device(port):
    device = '/dev/ttyUSB_' + str(port)
    if 'ttyUSB' in os.readlink(device):
        return 'SPRESENSE'
    else:
        return 'USB Camera'

### SPRESENSEの接続に必要なデバイス名の取得
def get_spresense_name(port):
    return '/dev/ttyUSB_' + str(port)

### USB Cameraの接続に必要な番号の取得
def get_usb_camera_name(port):
    model = subprocess.check_output('sudo cat /proc/cpuinfo'.split()).decode()
    device_name = ''
    if 'Raspberry Pi 3 Model B Plus' in model:
        if port == 1:
            device_name = '0:1.1.2:1.0-video'
        elif port == 2:
            device_name = '0:1.1.3:1.0-video'
        elif port == 3:
            device_name = '0:1.3:1.0-video'
        elif port == 4:
            device_name = '0:1.2:1.0-video'
        else:
            logger.error("unknown port")
            return -1
    elif 'Raspberry Pi 4 Model B' in model:
        if port == 1:
            device_name = '0:1.3:1.0-video'
        elif port == 2:
            device_name = '0:1.4:1.0-video'
        elif port == 3:
            device_name = '0:1.1:1.0-video'
        elif port == 4:
            device_name = '0:1.2:1.0-video'
        else:
            logger.error("unknown port")
            return -1
    else:
        device_name = '0:1.' + str(port + 1) + ':1.0-video'


    devices = os.listdir('/dev/v4l/by-path')
    for device in devices:
        if device_name in device:
            retVal = int(os.readlink('/dev/v4l/by-path/' + device)[-1])
            if retVal % 2 == 0:
                return retVal

    logger.error("unknown port")
    return -1

# サンプル
if __name__ == '__main__':
    ports = get_usb_device()
    for port in ports:
        print("port = {}".format(port))
        print("identify = {}".format(identify_usb_device(port)))
        if 'SPRESENSE' == identify_usb_device(port):
            print("connect info = {}".format(get_spresense_name(port)))
        elif 'USB Camera' == identify_usb_device(port):
            print("connect info = {}".format(get_usb_camera_name(port)))
