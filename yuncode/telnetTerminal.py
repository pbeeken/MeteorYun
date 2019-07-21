# Bad telnet client
# telnet program example
# Can connect:
# You can also see the Console by opening a terminal window and typing ssh root@yourYunsName.local 'telnet localhost 6571' then pressing enter.


import socket, select, string, sys

#main function
if __name__ == "__main__":

    msg = ""

    if (len(sys.argv) < 3) :
        host = 'localhost'
        port = 6571
    else :
        host = sys.argv[1]
        port = int(sys.argv[2])

    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s.settimeout(2)

    # connect to remote host
    try:
        s.connect((host, port))
    except:
        print 'Unable to connect'
        sys.exit()

    print 'Connected to remote host'

    while True:
        socket_list = [sys.stdin, s]

        # Get the list sockets which are readable
        read_sockets, write_sockets, error_sockets = select.select(socket_list , [], [])

        for sock in read_sockets:
            #incoming message from remote server
            if sock == s:
                data = sock.recv(1024)
                if not data :
                    print 'Connection closed'
                    sys.exit()
                else :
                    #print data
                    print(data)

        #user entered a message
        else:
            msg = sys.stdin.readline()
            s.send(msg)
