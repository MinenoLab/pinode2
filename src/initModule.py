import time
import os
import json

# 設定ファイルの読み出し
with open('/etc/pinode/config.json') as f:
    config = json.load(f)


def gpio_init():
    # GPIOピン番号4を有効化
    os.system("echo 4 > /sys/class/gpio/export")
    time.sleep(1)

    # GPIOピン番号4をアウトプットモードに設定
    os.system("echo out > /sys/class/gpio/gpio4/direction")
    time.sleep(1)

    # GPIOピン番号4の出力を0に設定
    os.system("echo 0 > /sys/class/gpio/gpio4/value")
    time.sleep(1)

    # GPIOピン番号4の出力を1に設定
    os.system("echo 1 > /sys/class/gpio/gpio4/value")



if __name__ == '__main__':
    gpio_init()
