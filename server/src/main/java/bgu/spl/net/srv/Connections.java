package bgu.spl.net.srv;

import java.io.IOException;

public interface Connections<T> {
    void addClient(int connectionId, ConnectionHandler<T> handler);

    void send(int connectionId, T msg) throws IOException;

    void send(String channel, T msg) throws IOException;

    /**
     * logs out the client 
     * @param connectionId the connection id to identify the client (and user)
     */
    void disconnect(int connectionId);
}
