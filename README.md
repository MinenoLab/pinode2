# Pinode2
Pinode2とは，屋外で気温や湿度などの環境データを収集するためのIoTデータ収集デバイスです（Fig.1）．Pinode2は，Raspberry Pi3，Pinode2専用Hat基板，各種センサ部品で構成されるハードウェアと，センサからデータを取得しサーバへアップロードするソフトウェアから構成されています．


Fig.1　Pinode2デバイス

## Install
### Dependdencies:
* Raspberry Pi3 (Raspberry Pi OS Lite 64-bit ver.??)
* Python 3.8
* OpenCV ??


### Install
ソフトウェアの設定は，設定全般を担当しているconfig.jsonファイルと優先度通信用の設定ファイルであるconfig.hファイルで管理されています．必要に応じてファイルの内容を設定してください．以下にファイルのパスを記述します．
* 設定全般 : pinode2/config/config.json
* 優先度通信 : pinode2/priority_client/src/conf/config.h
設定はインストール手順を実施(install.shを実行)する前に変更してください．インストール後に設定ファイルを変更したい場合は以下のパスを参照してください
* 設定全般 : /etc/pinode/config.json
* 優先度通信 : /usr/local/bin/priority_client/src/conf/config.json

センサ値の収集はi2cを用いて行っています．raspi-configコマンドよりi2cを有効にしてください．

Raspberry Piへのインストール手順を以下に示します．研究室GitLabへアクセスできる環境で行ってください．
```
$ git clone https://github.com/MinenoLab/pinode2.git
$ cd pinode2

$ sudo bash ./install.sh
$ sudo reboot
```