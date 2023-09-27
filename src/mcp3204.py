

class MCP3204:
    def __init__(self, spi, vref=5.0):
        self.spi = spi
        self.vref = vref

    def read(self, ch):
        if ch < 0 or ch > 4:
            return -1
        data2 = (ch << 6) & 0xff
        ret = self.spi.xfer2([0x06, data2, 0x00])
        raw = ((ret[1] & 0x0f) << 8) + ret[2]
        volts = raw * self.vref / 4096.0
        return volts



if __name__=='__main__':
    import spidev
    import time
    dev = spidev.SpiDev()
    dev.open(0, 0)
    dev.max_speed_hz = 50000

    mcp = MCP3204(dev)

    try:
        while True:
            print(mcp.read(0), "/", mcp.read(1))
            time.sleep(1)
    except KeyboardInterrupt:
        pass

    dev.close()

