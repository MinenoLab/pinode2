import os
import datetime
import json
import os
import shutil

import shootImageModule
import sensorModule
import usbModule
import alertModule


if __name__ == '__main__':
    # 設定ファイルの読み出し
    with open('/etc/pinode/config.json') as f:
        config = json.load(f)

    # 時間取得
    now_time = datetime.datetime.now()

    # エラーフラグの設定
    error_flg = 0

    ### カメラ画像取得
    # USBカメラかSPRESNESEから画像を取得する
    ports = usbModule.get_usb_device()
    for port in ports:
        # ファイルパス
        file_path_image =  "{}/image{}/".format(config['DIR_ALL_DATA'], port)
        file_path_image_send = "{}/image{}/".format(config['DIR_SEND_ALL_DATA'], port)
        if 'SPRESENSE' == usbModule.identify_usb_device(port):
            try:
                file_name_image = "{}_{:02}_HDR_{}.jpg".format(config['DEVICE_ID'], port, now_time.strftime('%Y%m%d-%H%M'))
                shootImageModule.get_image_on_spresense(usbModule.get_spresense_name(port), file_path_image +  file_name_image)
                # 研究室ネットワークにアップロードするためのフォルダに移行する
                if config['UPLOAD_FLG'] and os.path.exists(file_path_image + file_name_image):
                    shutil.copyfile(file_path_image + file_name_image, file_path_image_send + file_name_image)
            except Exception as e:
                alertModule.send_teams_message("spresense connection timeout")
                error_flg += 2 ** (port - 1)
        elif 'USB Camera' == usbModule.identify_usb_device(port):
            try:
                file_name_image = "{}_{:02}_RGB_{}.jpg".format(config['DEVICE_ID'], port, now_time.strftime('%Y%m%d-%H%M'))
                shootImageModule.get_image_on_usb_camera(usbModule.get_usb_camera_name(port), file_path_image + file_name_image)
                # 研究室ネットワークにアップロードするためのフォルダに移行する
                if config['UPLOAD_FLG'] and os.path.exists(file_path_image + file_name_image):
                    shutil.copyfile(file_path_image + file_name_image, file_path_image_send + file_name_image)
            except Exception as e:
                alertModule.send_teams_message("usb camera connection timeout")
                error_flg += 2 ** (port - 1)
        else:
            alertModule.send_teams_message("unknown device on usb port")

    # ファイルパス
    file_path_sensor = "{}/sensor/".format(config['DIR_ALL_DATA'])
    file_path_sensor_send = "{}/sensor/".format(config['DIR_SEND_ALL_DATA'])
    file_name_sensor = "{}_{}.csv".format(config['DEVICE_ID'], now_time.strftime('%Y%m%d-%H%M'))

    # センサデータ記録
    sensorModule.save_sensor_data(now_time, error_flg)
    # 研究室ネットワークにアップロードするためのフォルダに移行する
    if config['UPLOAD_FLG'] and os.path.exists(file_path_sensor + file_name_sensor):
        shutil.copyfile(file_path_sensor + file_name_sensor, file_path_sensor_send + file_name_sensor)
