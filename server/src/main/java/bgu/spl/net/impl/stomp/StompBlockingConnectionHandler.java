package bgu.spl.net.impl.stomp;

import java.io.BufferedInputStream;
import java.io.BufferedOutputStream;
import java.io.IOException;
import java.net.Socket;

import bgu.spl.net.api.MessageEncoderDecoder;
import bgu.spl.net.api.StompMessagingProtocol;
import bgu.spl.net.srv.ConnectionHandler;
import bgu.spl.net.srv.Connections;

public class StompBlockingConnectionHandler<T> implements Runnable, ConnectionHandler<T> {
    private final StompMessagingProtocol<T> protocol;
    private final MessageEncoderDecoder<T> encdec;
    private final Socket sock;
    private BufferedInputStream in;
    private BufferedOutputStream out;
    private volatile boolean connected = true;

    public StompBlockingConnectionHandler(Socket sock, MessageEncoderDecoder<T> reader, StompMessagingProtocol<T> protocol) {
        this.sock = sock;
        this.encdec = reader;
        this.protocol = protocol;
    }

    public void startProtocol(int connectionId, Connections<T> connections){
        protocol.start(connectionId, connections);
    }

    @Override
    public void run() {
        try (Socket sock = this.sock) { //just for automatic closing
            int read;

            in = new BufferedInputStream(sock.getInputStream());
            out = new BufferedOutputStream(sock.getOutputStream());

            while (!protocol.shouldTerminate() && connected && (read = in.read()) >= 0) {
                T nextMessage = encdec.decodeNextByte((byte) read);
                if (nextMessage != null) {
                    protocol.process(nextMessage);
                }
            }

    } catch (IOException ex) {
            ex.printStackTrace();
            try {
                Data.getInstance().disconnect(protocol.getConnectionId());
            } catch (StompException e) {
                System.out.println("UNEXPECTED ERROR WHILE TRYING TO DISCONNECT CLIENT THAT ALREADY DISCONNECTED");
                e.printStackTrace();
            }
        }

    }

    @Override
    public void close() throws IOException {
        connected = false;
        sock.close();
    }

    @Override
    public void send(T msg) throws IOException{
        out.write(encdec.encode(msg));
        out.flush();
    }
}
