# Raspi_Node-RED_elastic
前提条件：raspi上にElasticsearch、kibanaを構築済み

node-redに読み込んでください。

# 1 MQTT broker
Aedes MQTT brokerを使用（moscaはサポート切れのため）
必要に応じて処理ノードを追加してください。
port:1883

# 2 Topic:OneData
一つデータ
_index：sensors-sensor_aになります。

# 3 Topic:TwoData
2つデータ
_index：sensors-sensor_a+sensor_bになります。
