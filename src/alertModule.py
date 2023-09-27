import pymsteams
import datetime
import json


# Teamsでアラートを送信する
def send_teams_message(message):
  # 設定ファイルの読み出し
  with open('/etc/pinode/config.json') as f:
      config = json.load(f)

  if config["ALERT_FLG"]:
    # Microsoft TeamsのWebhook URL"
    webhook_url = "https://scii.webhook.office.com/webhookb2/e9ec7389-0530-4b65-bb91-02f55c5b3e5b@e0d7dc00-4621-4fe0-90b1-df7b1b40b351/IncomingWebhook/d6c0f303e021443ea0a6914458e58005/372cf67b-fac1-41b0-9961-e6a7916c76ff"

    # タイトル設定
    title  = f"PiNode{config['DEVICE_ID']}, date : {datetime.datetime.now().strftime('%Y-%m-%d %H:%M:%S')}"

    ms = pymsteams.connectorcard(webhook_url)
    ms.title(title)
    ms.text(message)
    ms.send()

if __name__ == '__main__':
  message = "Alert Test"
  send_teams_message(message)
