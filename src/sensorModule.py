import subprocess
import time
import json
import spidev

import initModule
import alertModule
import mcp3204

# 設定ファイルの読み出し
with open('/etc/pinode/config.json') as f:
    config = json.load(f)

def get_i_v_light():
    # エラーフラグ
    error_flg = 0

    # 照度センサを読み取るためのコマンド
    lux0_cmd = ['/usr/local/bin/read_sensor', '-t', 's1133', '-i', '0']

    # 3回まで実行する(3回まで実行したらエラーなくなるでしょ理論)
    for i in range(3):
        lux0 = subprocess.run(lux0_cmd, capture_output=True, text=True).stdout.strip()
        time.sleep(2)
        # nanや infがでなければ終了
        if lux0 and lux0 != 'nan' and lux0 != 'inf':
            break
        # 再起動プログラム
        initModule.gpio_init()
        time.sleep(5)

    # エラーカウント取得
    lux0_e = int(open(config['DATA_ERRCNT_LUX0'], 'r').read().strip() or 0)

    # 今回の取得値を前回値として登録
    if lux0 and lux0 != 'nan' and lux0 != 'inf':
        with open(config['DATA_OLD_LUX0'], 'w') as f:
            f.write(lux0)
        lux0_e = 0
    # 3回データをとってもエラーだったらエラー回数を1増やして，前回値を今回の値とする，
    else:
        lux0_t = lux0
        with open(config['DATA_OLD_LUX0'], 'r') as f:
            lux0 = f.read().strip()
        lux0_e += 1
        alertModule.send_teams_message("i_v_light sensor get faild")
        error_flg += 16

    # 5回連続でエラーが出ていた場合今回取得した値を登録
    if lux0_e >= 5:
        if lux0_t and lux0_t != 'nan' and lux0_t != 'inf':
            with open(config['DATA_OLD_LUX0'], 'w') as f:
                f.write(lux0_t)
            lux0_e = 0
            lux0 = lux0_t
            alertModule.send_teams_message("i_v_light sensor clear")
            error_flg += 1024

    with open(config['DATA_ERRCNT_LUX0'], 'w') as f:
        f.write(str(lux0_e))

    return lux0, error_flg

def get_u_v_light():
    # エラーフラグ
    error_flg = 0

    # 照度センサを読み取るためのコマンド
    lux1_cmd = ['/usr/local/bin/read_sensor', '-t', 's1133', '-i', '1']

    # 3回まで実行する(3回まで実行したらエラーなくなるでしょ理論)
    for i in range(3):
        lux1 = subprocess.run(lux1_cmd, capture_output=True, text=True).stdout.strip()
        time.sleep(2)
        # 値が存在していてnanや infがでなければ終了
        if lux1 and lux1 != 'nan' and lux1 != 'inf':
            break
        # 再起動プログラム
        initModule.gpio_init()
        time.sleep(5)

    # エラーカウント取得
    lux1_e = int(open(config['DATA_ERRCNT_LUX1'], 'r').read().strip() or 0)

    # 今回の取得値を前回値として登録
    if lux1 and lux1 != 'nan' and lux1 != 'inf':
        with open(config['DATA_OLD_LUX1'], 'w') as f:
            f.write(lux1)
        lux1_e = 0
    # 3回データをとってもエラーだったらエラー回数を1増やして，前回値を今回の値とする，
    else:
        lux1_t = lux1
        with open(config['DATA_OLD_LUX1'], 'r') as f:
            lux1 = f.read().strip()
        lux1_e += 1
        alertModule.send_teams_message("u_v_light sensor get faild")
        error_flg += 32

    # 5回連続でエラーが出ていた場合今回取得した値を登録
    if lux1_e >= 5:
        if lux1_t and lux1_t != 'nan' and lux1_t != 'inf':
            with open(config['DATA_OLD_LUX1'], 'w') as f:
                f.write(lux1_t)
            lux1_e = 0
            lux1 = lux1_t
            alertModule.send_teams_message("u_v_light sensor clear")
            error_flg += 2048

    with open(config['DATA_ERRCNT_LUX1'], 'w') as f:
        f.write(str(lux1_e))

    return lux1, error_flg

def get_temperature():
    # エラーフラグ
    error_flg = 0

    # 温度センサを読み取るためのコマンド
    temp_cmd = ['/usr/local/bin/read_sensor', '-t', 'sht25', '-i', '0', '-m', 'temp']

    # 3回まで実行する(3回まで実行したらエラーなくなるでしょ理論)
    for i in range(3):
        temp = subprocess.run(temp_cmd, capture_output=True, text=True).stdout.strip()
        time.sleep(2)
        # 値が存在してnanじゃなければ終了
        if temp and temp != 'nan':
            break
        # 再起動プログラム
        initModule.gpio_init()
        time.sleep(5)

    # エラーカウント取得
    temp_e = int(open(config['DATA_ERRCNT_TEMP'], 'r').read().strip() or 0)

    # 前回値から大きくそれた場合は前回値を利用する(nanの時も前回値)
    if temp and temp != 'nan':
        tempc = float(temp)
        tempp = float(open(config['DATA_OLD_TEMP'], 'r').read().strip() or 0)
        if tempp - 3 < tempc < tempp + 3:
            temp = "{:.3f}".format(tempc)
            temp_e = 0
        else:
            temp = "{:.3f}".format(tempp)
            temp_e += 1
            alertModule.send_teams_message("temperature sensor has acquired a large outlier")
            error_flg += 64
        # 5回連続でエラーが出ていた場合今回取得した値を登録
        if temp_e >= 5:
            if tempc and tempc != 'nan' and tempc >= 0 and tempc < 60:
                temp = tempc
                temp_e = 0
                alertModule.send_teams_message("temperature sensor clear")
                error_flg += 4096

        with open(config['DATA_OLD_TEMP'], 'w') as f:
            f.write(str(temp))
        with open(config['DATA_ERRCNT_TEMP'], 'w') as f:
            f.write(str(temp_e))
    else:
        with open(config['DATA_OLD_TEMP'], 'r') as f:
            temp = f.read().strip()
        alertModule.send_teams_message("temperature sensor get faild")
        error_flg += 64
    return temp, error_flg

def get_humidity():
    # エラーフラグ
    error_flg = 0

    # 温度センサを読み取るためのコマンド
    humi_cmd = ['/usr/local/bin/read_sensor', '-t', 'sht25', '-i', '0', '-m', 'humi']

    # 3回まで実行する(3回まで実行したらエラーなくなるでしょ理論)
    for i in range(3):
        humi = subprocess.run(humi_cmd, capture_output=True, text=True).stdout.strip()
        time.sleep(2)
        # 値が存在してnanじゃなければ終了
        if humi and humi != 'nan':
            break
        initModule.gpio_init()
        time.sleep(5)

    # エラーカウント取得
    humi_e = int(open(config['DATA_ERRCNT_HUMI'], 'r').read().strip() or 0)

    # 前回値から大きくそれた場合は前回値を利用する(nanの時も前回値)
    if humi and humi != 'nan':
        humic = float(humi)
        humip = float(open(config['DATA_OLD_HUMI'], 'r').read().strip() or 0)
        if humip - 3 < humic < humip + 3:
            humi = "{:.3f}".format(humic)
            humi_e = 0
        else:
            humi = "{:.3f}".format(humip)
            humi_e += 1
            alertModule.send_teams_message("humidity sensor has acquired a large outlier")
            error_flg += 128
        # 5回連続でエラーが出ていた場合今回取得した値を登録
        if humi_e >= 5:
            if humic and humic != 'nan' and humic >= 0 and humic < 60:
                humi = humic
                humi_e = 0
                alertModule.send_teams_message("humidity sensor clear")
                error_flg += 8192

        with open(config['DATA_OLD_HUMI'], 'w') as f:
            f.write(str(humi))
        with open(config['DATA_ERRCNT_HUMI'], 'w') as f:
            f.write(str(humi_e))
    else:
        with open(config['DATA_OLD_HUMI'], 'r') as f:
            humi = f.read().strip()
        alertModule.send_teams_message("humidity sensor get faild")
        error_flg += 128
    return humi, error_flg


def get_stem():
    # エラーフラグ
    error_flg = 0

    dev = spidev.SpiDev()
    dev.open(0, 0)
    dev.max_speed_hz = 50000

    mcp = mcp3204.MCP3204(dev)

    try:
        # 3回まで実行する
        for i in range(3):
            # 0は茎径
            stem = mcp.read(0)
            if stem and stem != 'nan':
                break
            time.sleep(1)
    except Exception as e:
        print(e)

    # エラーカウント取得
    stem_e = int(open(config['DATA_ERRCNT_STEM'], 'r').read().strip() or 0)

    # 前回値から大きくそれた場合は前回値を利用する(nanの時も前回値)
    if stem and stem != 'nan':
        stemc = float(stem)
        stemp = float(open(config['DATA_OLD_STEM'], 'r').read().strip() or 0)
        if stemp - 1 < stemc < stemp + 1:
            stem = "{:.3f}".format(stemc)
            stem_e = 0
        else:
            stem = "{:.3f}".format(stemp)
            stem_e += 1
            alertModule.send_teams_message("stem sensor has acquired a large outlier")
            error_flg += 256
        # 5回連続でエラーが出ていた場合今回取得した値を登録
        if stem_e >= 5:
            if stemc and stemc != 'nan' and stemc >= 0 and stemc < 60:
                stem = stemc
                stem_e = 0
                alertModule.send_teams_message("stem sensor clear")
                error_flg += 16384

        with open(config['DATA_OLD_STEM'], 'w') as f:
            f.write(str(stem))
        with open(config['DATA_ERRCNT_STEM'], 'w') as f:
            f.write(str(stem_e))
    else:
        with open(config['DATA_OLD_STEM'], 'r') as f:
            stem = f.read().strip()
            alertModule.send_teams_message("stem sensor get faild")
        error_flg += 256
    return stem, error_flg

def get_fruit_diagram():
    # エラーフラグ
    error_flg = 0

    dev = spidev.SpiDev()
    dev.open(0, 0)
    dev.max_speed_hz = 50000

    mcp = mcp3204.MCP3204(dev)

    try:
        # 3回まで実行する
        for i in range(3):
            # 1は果実径
            fruit_diagram = mcp.read(1)
            if fruit_diagram and fruit_diagram != 'nan':
                break
            time.sleep(1)
    except Exception as e:
        print(e)

    # エラーカウント取得
    fruit_diagram_e = int(open(config['DATA_ERRCNT_FRUIT_DIAGRAM'], 'r').read().strip() or 0)

    # 前回値から大きくそれた場合は前回値を利用する(nanの時も前回値)
    if fruit_diagram and fruit_diagram != 'nan':
        fruit_diagramc = float(fruit_diagram)
        fruit_diagramp = float(open(config['DATA_OLD_FRUIT_DIAGRAM'], 'r').read().strip() or 0)
        if fruit_diagramp - 1 < fruit_diagramc < fruit_diagramp + 1:
            fruit_diagram = "{:.3f}".format(fruit_diagramc)
            fruit_diagram_e = 0
        else:
            fruit_diagram = "{:.3f}".format(fruit_diagramp)
            fruit_diagram_e += 1
            alertModule.send_teams_message("fruit_diagram sensor has acquired a large outlier")
            error_flg += 512
        # 5回連続でエラーが出ていた場合今回取得した値を登録
        if fruit_diagram_e >= 5:
            if fruit_diagramc and fruit_diagramc != 'nan' and fruit_diagramc >= 0 and fruit_diagramc < 60:
                fruit_diagram = fruit_diagramc
                fruit_diagram_e = 0
                alertModule.send_teams_message("fruit_diagram sensor clear")
                error_flg += 16384

        with open(config['DATA_OLD_FRUIT_DIAGRAM'], 'w') as f:
            f.write(str(fruit_diagram))
        with open(config['DATA_ERRCNT_FRUIT_DIAGRAM'], 'w') as f:
            f.write(str(fruit_diagram_e))
    else:
        with open(config['DATA_OLD_FRUIT_DIAGRAM'], 'r') as f:
            fruit_diagram = f.read().strip()
            alertModule.send_teams_message("fruit_diagram sensor get faild")
        error_flg += 512
    return fruit_diagram, error_flg

def get_cpu_temp():
    with open('/sys/class/thermal/thermal_zone0/temp', 'r') as f:
        cpu_temp = float(f.read().strip()) / 1000.0
    return cpu_temp

def save_sensor_data(TIME, ERROR_FLG):
    # DEVICE_IDと時間の追加
    sensor_content_header = '#datatype measurement,dateTime:RFC3339'
    sensor_content_column = 'deviceid,time'
    sensor_content_value  = "{},{}".format(config['DEVICE_ID'],TIME.strftime('%Y-%m-%dT%H:%M:00Z'))

    # 内部照度追加
    i_v_light, error_flg = get_i_v_light()
    ERROR_FLG += error_flg
    if i_v_light != '':
        sensor_content_header += ',double'
        sensor_content_column += ',i_v_light'
        sensor_content_value  += ",{}".format(i_v_light)

    # 外部照度追加
    u_v_light, error_flg = get_u_v_light()
    ERROR_FLG += error_flg
    if u_v_light != '':
        sensor_content_header += ',double'
        sensor_content_column += ',u_v_light'
        sensor_content_value  += ",{}".format(u_v_light)

    # 温度追加
    temperature, error_flg = get_temperature()
    ERROR_FLG += error_flg
    if temperature != '':
        sensor_content_header += ',double'
        sensor_content_column += ',temperature'
        sensor_content_value  += ",{}".format(temperature)

    # 湿度追加
    humidity, error_flg = get_humidity()
    ERROR_FLG += error_flg
    if humidity != '':
        sensor_content_header += ',double'
        sensor_content_column += ',humidity'
        sensor_content_value  += ",{}".format(humidity)

    # 茎径追加
    dev = spidev.SpiDev()
    stem, error_flg = get_stem()
    if float(stem) >= 0.01:
        ERROR_FLG += error_flg
        sensor_content_header += ',double'
        sensor_content_column += ',stem'
        sensor_content_value  += ",{}".format(stem)

    # 果実径追加
    fruit_diagram, error_flg = get_fruit_diagram()
    if float(fruit_diagram) >= 0.01:
        ERROR_FLG += error_flg
        sensor_content_header += ',double'
        sensor_content_column += ',fruit_diagram'
        sensor_content_value  += ",{}".format(fruit_diagram)

    # CPU温度追加
    cpu_temp = get_cpu_temp()
    ERROR_FLG += error_flg
    if temperature != '':
        sensor_content_header += ',double'
        sensor_content_column += ',cpu_temperature'
        sensor_content_value  += ",{}".format(cpu_temp)

    # エラーフラグ追加
    sensor_content_header += ',long'
    sensor_content_column += ',error_flg'
    sensor_content_value += ",{}".format(ERROR_FLG)

    with open("{}/sensor/{}_{}.csv".format(config['DIR_ALL_DATA'], config['DEVICE_ID'], TIME.strftime('%Y%m%d-%H%M')), "w") as f:
        f.write(sensor_content_header + "\n" + sensor_content_column + "\n" + sensor_content_value)


if __name__ == '__main__':
    print("i_v_light = {}".format(get_i_v_light()))
    print("u_v_light = {}".format(get_u_v_light()))
    print("temperature = {}".format(get_temperature()))
    print("humidity = {}".format(get_humidity()))
    print("cpu temperature = {}".format(get_cpu_temp))
    dev = spidev.SpiDev()
    print("stem = {}".format(get_stem()))
    print("fruit_diagram = {}".format(get_fruit_diagram()))
