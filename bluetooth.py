import asyncio
from enum import IntEnum
from bleak import BleakScanner, BleakClient

CHAR_UUID = "6E400002-B5A3-F393-E0A9-E50E24DCCA9E"
DEVICE_NAME = "Atom 3S"

class command(IntEnum):
    Y       = 0x0001
    B       = 0x0002
    A       = 0x0004
    X       = 0x0008
    L       = 0x0010
    R       = 0x0020
    ZL      = 0x0040
    ZR      = 0x0080
    MINUS   = 0x0100
    PLUS    = 0x0200
    LSTICK  = 0x0400
    RSTICK  = 0x0800
    HOME    = 0x1000
    CAPTURE = 0x2000

class hat_command(IntEnum):
    CENTER     = 0x000
    UP         = 0x001
    UP_RIGHT   = 0x002
    RIGHT      = 0x003
    DOWN_RIGHT = 0x004
    DOWN       = 0x005
    DOWN_LEFT  = 0x006
    LEFT       = 0x007
    UP_LEFT    = 0x008

async def find_device() -> str:
    devices = await BleakScanner.discover()
    for device in devices:
        if device.name == DEVICE_NAME:
            return device.address
    return None

async def send_command(client, command, time=1):
    query = f'button:{command}:{time}'
    await client.write_gatt_char(CHAR_UUID, query.encode())
    print("Sent:", query)
    
async def send_hat_command(client, command, time=1):
    query = f'hat:{command}:{time}'
    await client.write_gatt_char(CHAR_UUID, query.encode())
    print("Sent:", query)
        
async def send_stick_command(client, rx = 0, ry = 0 ,lx =0 , ly = 0):
    query = f'stick:{rx}.{ry}:{lx}.{ly}'
    await client.write_gatt_char(CHAR_UUID, query.encode())
    print("Sent:", query)
    
async def send_press_command(client, command):
    query = f'press:{command}:'
    await client.write_gatt_char(CHAR_UUID, query.encode())
    print("Sent:", query)
    
async def send_release_command(client, command):
    query = f'release:{command}:'
    await client.write_gatt_char(CHAR_UUID, query.encode())
    print("Sent:", query)
    
async def send_reset_command(client):
    query = f'reset::'
    await client.write_gatt_char(CHAR_UUID, query.encode())
    print("Sent:", query)

# --- メインループ ---
async def connect_loop():
    while True:
        address = await find_device()
        if address is None:
            print("Device not found. Retrying in 5s...")
            await asyncio.sleep(5)
            continue

        try:
            async with BleakClient(address) as client:
                print("Connected!")
                
                async def stick_loop():
                    while client.is_connected:
                        await send_press_command(client, command.A)
                        await asyncio.sleep(2)
                        await send_release_command(client, command.A)
                        

                await stick_loop()

        except Exception as e:
            print("Error:", e, "Retrying in 5s...")
            await asyncio.sleep(5)
            
asyncio.run(connect_loop())