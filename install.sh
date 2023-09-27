#!/bin/bash


# ディレクトリを作成
mkdir -p /etc/pinode
mkdir -p /var/log/old
mkdir -p /var/log/read_sensor
mkdir -p /home/pinode/data/sensor
mkdir -p /home/pinode/data/image1
mkdir -p /home/pinode/data/image2
mkdir -p /home/pinode/data/image3
mkdir -p /home/pinode/data/image4
mkdir -p /home/pinode/send_data/sensor
mkdir -p /home/pinode/send_data/image1
mkdir -p /home/pinode/send_data/image2
mkdir -p /home/pinode/send_data/image3
mkdir -p /home/pinode/send_data/image4
chmod 777 /var/log/old
chmod 777 /var/log/read_sensor
chmod 777 /home/pinode/data/sensor
chmod 777 /home/pinode/data/image1
chmod 777 /home/pinode/data/image2
chmod 777 /home/pinode/data/image3
chmod 777 /home/pinode/data/image4
chmod 777 /home/pinode/send_data/sensor
chmod 777 /home/pinode/send_data/image1
chmod 777 /home/pinode/send_data/image2
chmod 777 /home/pinode/send_data/image3
chmod 777 /home/pinode/send_data/image4


# 空のファイルを作成
touch /var/log/old/lux0
touch /var/log/old/lux1
touch /var/log/old/temp
touch /var/log/old/humi
touch /var/log/old/lux0_e
touch /var/log/old/lux1_e
touch /var/log/old/temp_e
touch /var/log/old/humi_e
touch /var/log/old/stem
touch /var/log/old/stem_e
touch /var/log/old/fruit_diagram
touch /var/log/old/fruit_diagram_e
chmod 666 /var/log/old/*


# エラー回数を入れる
echo 0 > /var/log/old/lux0_e
echo 0 > /var/log/old/lux1_e
echo 0 > /var/log/old/temp_e
echo 0 > /var/log/old/humi_e
echo 0 > /var/log/old/stem_e
echo 0 > /var/log/old/fruit_diagram_e

# python・サービス・設定ファイルを移行する
chmod 755 src/*.py
cp src/*py /usr/local/bin/
chmod 666 config/config.json
cp config/config.json /etc/pinode/config.json
chmod 777 service/*.service
cp service/*.service /etc/systemd/system/
chmod 777 service/*.timer
cp service/*.timer /etc/systemd/system/


# サービスファイルを登録する
systemctl daemon-reload
systemctl enable readImageSensor.service
systemctl enable readImageSensor.timer
systemctl enable priority_client.service


# openCVインストール
apt install python3-opencv -y
apt install libatlas-base-dev -y


# pythonライブラリインストール
pip install -r requirements.txt


# gpioの設定
python /usr/local/bin/initModule.py


# usbポートを判別できるようにする
model=$(grep -m1 -o -w 'Raspberry Pi [0-9]* Model [ABCD]\|Raspberry Pi 3 Model B Plus' /proc/cpuinfo)
echo $model
if [[ "$model" == "Raspberry Pi 3 Model B" ]]; then
	cp config/usb_setting/90-usb_3b.rules /etc/udev/rules.d/90-usb.rules
elif [[ "$model" == "Raspberry Pi 3 Model B Plus"* ]]; then
	cp config/usb_setting/90-usb_3bp.rules /etc/udev/rules.d/90-usb.rules
elif [[ "$model" == "Raspberry Pi 4 Model B" ]]; then
	cp config/usb_setting/90-usb_4b.rules /etc/udev/rules.d/90-usb.rules
else
	echo "This device is not a Raspberry Pi."
	exit 1
fi


# センサドライバの設定
cd ./sensorDriver
make
make install
cd ../


# 優先度通信設定
cd lib/priority_client/src
make client
cd ../../../
cp -r lib/priority_client /usr/local/bin/
