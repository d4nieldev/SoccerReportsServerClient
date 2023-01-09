package bgu.spl.net.srv;

import java.io.IOException;

public interface Connections<T> {
    void addClient(int connectionId, ConnectionHandler<T> handler);

    /**
     * logs the user into the server
     * @param host The host to connect to
     * @param login The username
     * @param password The password
     * @return "USER_ERR"   iff the user is logged in from some client
     * 
     *         "CLIENT_ERR" iff the client has some user logged in from it
     * 
     *         "PASS_ERR"   iff wrong password
     * 
     *         "SUCCESS"    iff the login was successful
     * 
     *         null         iff the client related to the connectionId is not active
     */
    String connect(int connectionId, String host, String login, String password);

    boolean send(int connectionId, T msg);

    void send(String channel, T msg);

    void disconnect(int connectionId);
}
