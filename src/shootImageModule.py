import serial
import time
import timeout_decorator
import cv2
import logging
import subprocess

BAUD_RATE = 115200  # ボーレート
BUFF_SIZE = 100  # 1回の通信で送られてくるデータサイズ
TYPE_INFO = 0
TYPE_IMAGE = 1
TYPE_FINISH = 2
TYPE_ERROR = 3

# loggerを定義
logger = logging.getLogger(__name__)
# loggerのログレベルをINFOに設定
logger.setLevel(logging.DEBUG)

# loggerのフォーマットを定義
formatter = logging.Formatter('%(asctime)s - %(levelname)s:%(name)s - %(message)s')
# ファイルに出力するためのFileHandlerを定義
file_handler = logging.FileHandler('/home/pi/service.log')
file_handler.setLevel(logging.DEBUG)
file_handler.setFormatter(formatter)
# コンソールに出力するためのStreamHandlerを定義
stream_handler = logging.StreamHandler()
stream_handler.setFormatter(formatter)
# loggerにそれぞれのハンドラーを追加
logger.addHandler(file_handler)
logger.addHandler(stream_handler)

def reboot_spresense():
    subprocess.call("sudo sh -c \"echo -n \"1-1\" > /sys/bus/usb/drivers/usb/unbind\"", shell=True)
    time.sleep(1)
    subprocess.call("sudo sh -c \"echo -n \"1-1\" > /sys/bus/usb/drivers/usb/bind\"", shell=True)
    time.sleep(5)

def __get_packet(ser):
    try:
        img = b''
        while True:
            val = ser.read()
            if val == b'\x00':
                break
            img += val
        decoded = cobs.decode(img)
        index = int(decoded[1]) * 1000 + int(decoded[2]) * 100 + int(decoded[3]) * 10 + int(decoded[4])
        return decoded[0], index, decoded[5:]
    except Exception as e:
        return TYPE_ERROR, 0, b''

def __check_packet(data):
    if len(data) != BUFF_SIZE:
        return False
    return True

def __send_request_image(ser):
    ser.write(str.encode('S\n'))

def __send_complete_image(ser):
    ser.write(str.encode('E\n'))

def __send_request_resend(ser, index):
    ser.write(str.encode(f'R{index}\n'))

@timeout_decorator.timeout(50, use_signals=False)
def __get_image_data(ser):
    img = b''
    resend_index_list = []
    finish_flag = False
    send_flg = []

    __send_request_image(ser)

    while True:
        code, index, data = __get_packet(ser)

        # データ取得
        if code == TYPE_INFO:
            img = bytearray(index * BUFF_SIZE)
            max_index = index
            send_flg = [False] * max_index
        elif code == TYPE_IMAGE:
            print("get", index)
            if __check_packet(data):
                img[index*BUFF_SIZE:(index+1)*BUFF_SIZE] = data
                send_flg[index] = True
                if index in resend_index_list:
                    resend_index_list.remove(index)
            else:
                resend_index_list.append(index)
        elif code == TYPE_FINISH:
            img += data
            finish_flag = True
        elif code == TYPE_ERROR:
            print('cant get data')

        # 終了チェック
        if finish_flag:
            if False in send_flg:
                print("resend", send_flg.index(False))
                __send_request_resend(ser, send_flg.index(False))
            for index in resend_index_list:
                __send_request_resend(ser, index)
            if (len(resend_index_list) == 0) and (not (False in send_flg)):
                print(send_flg)
                __send_complete_image(ser)
                break

    return img

# SPRESENSEから画像データの取得を行う
def get_image_on_spresense(port_num, file_path):
    # 3回実行してエラーの場合は終了する
    for i in range(3):
        try:
            with serial.Serial(port_num, BAUD_RATE, timeout = 3) as ser:
                time.sleep(2)     # Arduino側との接続のための待ち時間
                img = __get_image_data(ser)
                with open(file_path, "wb") as f:
                    f.write(img)
                logger.info("shoot image success")
            return True
        except Exception as e:
            logger.error(e)
    logger.error("Failed Shoot HDR Camera")
    return False


@timeout_decorator.timeout(20)
def get_image_on_usb_camera(device_id, file_path):
    cap = cv2.VideoCapture(device_id)
    for i in range(3):
        ret, frame = cap.read()
    while(ret == False):
        ret, frame = cap.read()
    cv2.imwrite(file_path, frame)


if __name__ == '__main__':
    ### カメラ画像取得
    success_count = 0
    for i in range(100):
        if get_image_on_spresense("/dev/ttyUSB_1", "test.jpg") is True:
            success_count += 1
    # get_image_on_spresense("/dev/ttyUSB_1", "test.jpg")

    print("result : {} / 100".format(success_count))

