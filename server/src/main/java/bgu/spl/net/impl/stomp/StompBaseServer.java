package bgu.spl.net.impl.stomp;

import java.io.IOException;
import java.net.ServerSocket;
import java.net.Socket;
import java.util.function.Supplier;

import bgu.spl.net.api.MessageEncoderDecoder;
import bgu.spl.net.api.StompMessagingProtocol;
import bgu.spl.net.srv.Connections;
import bgu.spl.net.srv.Server;

public abstract class StompBaseServer<T> implements Server<T> {
    private final int port;
    private final Supplier<StompMessagingProtocol<T>> protocolFactory;
    private final Supplier<MessageEncoderDecoder<T>> encdecFactory;
    private ServerSocket sock;
    private Connections<T> connections;
    private int lastConnectionId;

    public StompBaseServer(
            int port,
            Supplier<StompMessagingProtocol<T>> protocolFactory,
            Supplier<MessageEncoderDecoder<T>> encdecFactory) {

        this.port = port;
        this.protocolFactory = protocolFactory;
        this.encdecFactory = encdecFactory;
		this.sock = null;
        this.connections = new ConnectionsImpl<>();
        this.lastConnectionId = 0;
    }

    @Override
    public void serve() {
        try (ServerSocket serverSock = new ServerSocket(port)) {
			System.out.println("Server started");

            this.sock = serverSock; //just to be able to close

            while (!Thread.currentThread().isInterrupted()) {

                Socket clientSock = serverSock.accept();

                StompBlockingConnectionHandler<T> handler = new StompBlockingConnectionHandler<>(
                        clientSock,
                        encdecFactory.get(),
                        protocolFactory.get());

                handler.startProtocol(lastConnectionId, connections);
                connections.addClient(lastConnectionId, handler);

                lastConnectionId++;

                execute(handler);
            }
        } catch (IOException ex) {
        }

        System.out.println("server closed!!!");
        
    }

    @Override
    public void close() throws IOException {
		if (sock != null)
			sock.close();
    }

    protected abstract void execute(StompBlockingConnectionHandler<T>  handler);
    
}
