import socket
import time
from enum import IntEnum


SERVER_IP = ""
SERVER_PORT = 12345

try:
    udp_socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
except socket.error as e:
    print(f"ソケットの作成に失敗しました: {e}")
    raise

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
    CENTER      = 0x000
    UP          = 0x001
    UP_RIGHT    = 0x002
    RIGHT       = 0x003
    DOWN_RIGHT  = 0x004
    DOWN        = 0x005
    DOWN_LEFT   = 0x006
    LEFT        = 0x007
    UP_LEFT     = 0x008

# --- コマンド送信関数群 ---
def send_command(btn_command, time_val=1):
    query = f'button:{btn_command}:{time_val}'
    udp_socket.sendto(query.encode(), (SERVER_IP, SERVER_PORT))
    print("Sent:", query)

def send_hat_command(btn_command, time_val=1):
    query = f'hat:{btn_command}:{time_val}'
    udp_socket.sendto(query.encode(), (SERVER_IP, SERVER_PORT))
    print("Sent:", query)

def send_stick_command(rx=0, ry=0, lx=0, ly=0):
    query = f'stick:{rx}.{ry}:{lx}.{ly}'
    udp_socket.sendto(query.encode(), (SERVER_IP, SERVER_PORT))
    print("Sent:", query)

def send_press_command(btn_command):
    query = f'press:{btn_command}:'
    udp_socket.sendto(query.encode(), (SERVER_IP, SERVER_PORT))
    print("Sent:", query)

def send_release_command(btn_command):
    query = f'release:{btn_command}:'
    udp_socket.sendto(query.encode(), (SERVER_IP, SERVER_PORT))
    print("Sent:", query)

def send_reset_command():
    query = f'reset::'
    udp_socket.sendto(query.encode(), (SERVER_IP, SERVER_PORT))
    print("Sent:", query)

def close_socket():
    """UDPソケットを安全に閉じる"""
    try:
        udp_socket.close()
        print("ソケットを閉じました。")
    except Exception as e:
        print(f"ソケットを閉じる際にエラー: {e}")